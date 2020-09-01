#include <arpa/inet.h>

#include "msg.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define NDA_PROTOCOL 12

static void default_cb(struct rtattr *rta, struct ndmsg *ndm);
static void dst_cb(struct rtattr *rta, struct ndmsg *ndm);
static void lladdr_cb(struct rtattr *rta, struct ndmsg *ndm);
static void probes_cb(struct rtattr *rta, struct ndmsg *ndm);
static void cacheinfo_cb(struct rtattr *rta, struct ndmsg *ndm);

const char *const ndm_cache_state[] = {
	[0] = "incomplete",
	[1] = "reachable",
	[2] = "stale",
	[3] = "delay",
	[4] = "probe",
	[5] = "failed",
	[6] = "noarp",
	[7] = "permanent",
};

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
	[NDA_LLADDR]       = lladdr_cb,
	[NDA_CACHEINFO]    = cacheinfo_cb,
	[NDA_PROBES]       = probes_cb,
	[NDA_VLAN]         = default_cb,
	[NDA_PORT]         = default_cb,
	[NDA_VNI]          = default_cb,
	[NDA_IFINDEX]      = default_cb,
	[NDA_MASTER]       = default_cb,
	[NDA_LINK_NETNSID] = default_cb,
	[NDA_SRC_VNI]      = default_cb,
	[NDA_PROTOCOL]     = default_cb,
};

static void default_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	(void)ndm;

	printf("rta_data_type: %s\n", rta_type_text[rta->rta_type]);
	printf("rta_data_size: %u\n", (unsigned)RTA_PAYLOAD(rta));
}

static void dst_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	char  addr[INET6_ADDRSTRLEN];
	const char *data;

	unsigned size = INET6_ADDRSTRLEN;

	data = inet_ntop(ndm->ndm_family, RTA_DATA(rta), addr, size);
	assert(NULL != data);

	printf("rta_data: %s\n", data);
}

static void lladdr_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	char data[18] = {0};
	unsigned char *addr = RTA_DATA(rta);

	(void)ndm;

	sprintf(data, "%02x:%02x:%02x:%02x:%02x:%02x",
		addr[0],
		addr[1],
		addr[2],
		addr[3],
		addr[4],
		addr[5]);

	printf("rta_data: %s\n", data);
}

static void cacheinfo_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	struct nda_cacheinfo *ndc = RTA_DATA(rta);

	(void)ndm;

	printf("rta_data: ");
	printf("confirmed... %u\n", ndc->ndm_confirmed);
	printf("\t  used........ %u\n", ndc->ndm_used);
	printf("\t  updated..... %u\n", ndc->ndm_updated);
	printf("\t  refcnt...... %u\n", ndc->ndm_refcnt);
}

static void probes_cb(struct rtattr *rta, struct ndmsg *ndm)
{
	(void)ndm;
	printf("rta_data: %u\n", *(unsigned *)RTA_DATA(rta));
}
