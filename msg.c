#include <arpa/inet.h>

#include "msg.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>


static const char * default_cb(struct rtattr *rta, struct ndmsg *ndm);
static const char * dst_cb(struct rtattr *rta, struct ndmsg *ndm);

const char *const rta_type_text[] = {
	[NDA_UNSPEC]       = "unspec",
	[NDA_DST]          = "dst",
	[NDA_LLADDR]       = "lladdr",
	[NDA_CACHEINFO]    = "cacheinfo",
	[NDA_PROBES]       = "probes",
	[NDA_VLAN]         = "vlan",
	[NDA_PORT]         = "port",
	[NDA_VNI]          = "vni",
	[NDA_IFINDEX]      = "ifindex",
	[NDA_MASTER]       = "master",
	[NDA_LINK_NETNSID] = "link_netnsid",
	[NDA_SRC_VNI]      = "src_vni",
	[NDA_PROTOCOL]     = "protocol",
};

rta_type_call_t rta_type_call[] = {
	[NDA_DST]          = dst_cb,
	[NDA_LLADDR]       = default_cb,
	[NDA_CACHEINFO]    = default_cb,
	[NDA_PROBES]       = default_cb,
	[NDA_VLAN]         = default_cb,
	[NDA_PORT]         = default_cb,
	[NDA_VNI]          = default_cb,
	[NDA_IFINDEX]      = default_cb,
	[NDA_MASTER]       = default_cb,
	[NDA_LINK_NETNSID] = default_cb,
	[NDA_SRC_VNI]      = default_cb,
	[NDA_PROTOCOL]     = default_cb,
};

static const char * default_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	(void)ndm;

	printf("rta_data_type: %s\n", rta_type_text[rta->rta_type]);
	printf("rta_data_size: %lu\n", RTA_PAYLOAD(rta));

	return NULL;
}

static const char * dst_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	char  addr[INET6_ADDRSTRLEN];
	const char *data;

	unsigned size = INET6_ADDRSTRLEN;

	data = inet_ntop(ndm->ndm_family, RTA_DATA(rta), addr, size);
	assert(NULL != data);

	printf("rta_data: %s\n", data);
	return NULL;
}
