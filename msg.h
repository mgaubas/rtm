#ifndef MSG_H
#define MSG_H 1

#include <linux/neighbour.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>

typedef void (*rta_type_call_t)(struct rtattr *rta, struct ndmsg *ndm);

extern const char *const ndm_cache_state[];
extern const char *const ndm_rta_type[];
extern rta_type_call_t   ndm_rta_type_call[];

extern const char *const iim_flag [];

#endif/*MSG_H*/
