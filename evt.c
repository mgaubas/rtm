#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>

#include "err.h"
#include "msg.h"
#include "nlm.h"

#include <assert.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include <errno.h>

#include <net/if.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/socket.h>

#define NL_MSG_LEN 32768U

struct loop_data;

typedef void (*loop_functype)(struct loop_data *data);

struct loop_data {
	int sock_fd;
	int poll_fd;
	char *recv_bp; // _bp: buffer pointer
	loop_functype loop_cb; // _cb: callback
};

static inline void nl_init_sock_addr(struct sockaddr_nl *addr);
static inline long nl_verify_payload(struct nlmsghdr *h, int len);
static inline void loop_cb(struct loop_data *data);
static inline void recv_cb_nd(struct nlmsghdr *h);
//static inline void recv_cb_link(struct nlmsghdr *h);

static struct sockaddr_nl nl_sock_addr;
static struct epoll_event poll_data;
static struct loop_data loop_data;

/* https://git.kernel.org/pub/scm/linux/kernel/git/netdev/net-next.git/commit/?id=d35c99ff77ecb2eb239731b799386f3b3637a31e
 * the size if defined in the kernel source
 * net/netlink/af_netlink.c
 */
static char loop_data_blob[NL_MSG_LEN];

int main(void)
{
	int s;
	int r;
	int poll_fd;

	s  = SOCK_DGRAM;
	s |= SOCK_CLOEXEC;
	s |= SOCK_NONBLOCK;
	s  = socket(AF_NETLINK, s, NETLINK_ROUTE);
	if (-1 == s) {
		eror_line = __LINE__;
		goto err_sys;
	}

	nl_init_sock_addr(&nl_sock_addr);

	r = bind(s, (struct sockaddr *)&nl_sock_addr, sizeof nl_sock_addr);
	if (-1 == r) {
		eror_line = __LINE__;
		goto err_sys;
	}

	poll_fd = EPOLL_CLOEXEC;
	poll_fd = epoll_create1(poll_fd);
	if (-1 == poll_fd) {
		eror_line = __LINE__;
		goto err_sys;
	}

	loop_data.sock_fd   = s;
	loop_data.poll_fd   = poll_fd;
	loop_data.loop_cb   = loop_cb;
	loop_data.recv_bp   = loop_data_blob;

	poll_data.events    = EPOLLIN;
	poll_data.events   |= EPOLLET;
	poll_data.data.ptr  = &loop_data;
	r = epoll_ctl(poll_fd, EPOLL_CTL_ADD, s, &poll_data);
	if (-1 == r) {
		eror_line = __LINE__;
		goto err_sys;
	}

	for (;;) {
		struct loop_data *data;

		r = epoll_wait(poll_fd, &poll_data, sizeof poll_data, -1);
		if (-1 == r) {
			eror_line = __LINE__;
			eror_text = strerror(errno);

			print_error(__func__);

			continue;
		}

		if (EPOLLIN & poll_data.events)
			if (data  = poll_data.data.ptr,
			    NULL != data && NULL != data->loop_cb)
				data->loop_cb(data);

	}

	exit(EXIT_SUCCESS);
err_sys:
	eror_text = strerror(errno);
	goto err;
err:
	print_error(__func__);
	exit(EXIT_FAILURE);
}

static inline void nl_init_sock_addr(struct sockaddr_nl *addr)
{
	memset(addr, 0, sizeof *addr);

	addr->nl_family  = AF_NETLINK;
	addr->nl_groups |= RTMGRP_LINK;
	addr->nl_groups |= RTMGRP_NEIGH;
}

static inline long nl_verify_payload(struct nlmsghdr *h, int len)
{
	struct nlmsgerr *e = NLMSG_DATA(h);

	if (NLMSG_ERROR == h->nlmsg_type)
		goto err_msg;

	assert((unsigned)len == h->nlmsg_len);

	return 0;
err_msg:
	eror_text = strerror(-e->error);
	goto err;
err:
	print_error(__func__);
	return -1;

}

static inline void loop_cb(__attribute__((unused)) struct loop_data *data)
{
	struct nlmsghdr *h;
	int r;

	r = recv(data->sock_fd, data->recv_bp, NL_MSG_LEN, MSG_TRUNC);
	if (-1 == r) {
		eror_line = __LINE__;
		goto err_sys;
	}

	h = (void *)data->recv_bp;

	if (-1 == nl_verify_payload(h, r))
		return;

	switch(h->nlmsg_type) {
	case RTM_NEWNEIGH:
	case RTM_DELNEIGH:
		printf("\n=== NEW MSG ===\n");
		recv_cb_nd(h);
		break;
	case RTM_NEWLINK:
	case RTM_DELLINK:
		printf("recv mesg type == link (%d)\n", h->nlmsg_type);
		break;
	default:
		assert(0);
		break;
	}

	return;
err_sys:
	eror_text = strerror(errno);
	goto err;
err:
	print_error(__func__);
	return;
}

static inline void nd_inet_print(unsigned char inet)
{
	static const char *const inet_text[] = {
		"AF_INET",
		"AF_INET6",
		"UNKNOWN",
	};

	const char *text;

	switch(inet) {
	case AF_INET:
		text = inet_text[0];
		break;
	case AF_INET6:
		text = inet_text[1];
		break;
	default:
		text = inet_text[2];
		break;
	};

	printf("ndm_family... %s\n", text);
}

static inline void nd_state_print(unsigned short state)
{
	for (unsigned short i = 0; i < 8; i++)
		if ((1U << i) & state)
			printf("ndm_state.... %s\n", ndm_cache_state[i]);
}

static inline void nd_ifname_print(struct ndmsg *ndm)
{
	char ifname[IF_NAMESIZE];

	if (NULL == if_indextoname(ndm->ndm_ifindex, ifname))
		goto err_sys;

	printf("ndm_ifname... %s\n", ifname);
	return;
err_sys:
	eror_text = strerror(errno);
	goto err;
err:
	print_error(__func__);
}

static inline void recv_cb_nd(struct nlmsghdr *h)
{
	struct ndmsg  *ndm = NLMSG_DATA(h);
	struct rtattr *rta;

	int len = h->nlmsg_len;

	nd_ifname_print(ndm);
	nd_inet_print(ndm->ndm_family);
	nd_state_print(ndm->ndm_state);

	printf("\n");

	rta  = nlm_next_data(h, sizeof *ndm);
	len -= NLMSG_SPACE(sizeof *ndm);

	while (rta) {
		printf("rta_type: %s\n", rta_type_text[rta->rta_type]);
		(rta_type_call[rta->rta_type])(rta, ndm);

		rta = RTA_NEXT(rta,len);
		if (!RTA_OK(rta,len))
		    break;
	}
}
