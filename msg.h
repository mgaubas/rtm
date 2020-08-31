#ifndef MSG_H
#define MSG_H 1

#include <linux/neighbour.h>
#include <linux/rtnetlink.h>

typedef const char * (*rta_type_call_t)(struct rtattr *rta, struct ndmsg *ndm);

extern const char *const rta_type_text[];
extern rta_type_call_t   rta_type_call[];

#endif/*MSG_H*/
