#ifndef NLM_H
#define NLM_H 1

#include <linux/netlink.h>

static inline void *nlm_next_data(struct nlmsghdr *hdr, size_t len)
{
	return (char *)hdr + NLMSG_SPACE(len);
}

#endif/*NLM_H*/
