#ifndef RATELIMIT_TYPES_H
#define RATELIMIT_TYPES_H

#include <linux/types.h>

#define MAX_RATE_LIMIT 100

// For ip + port rate limit
struct ip_port_pair
{
    __be32 source_ip;
    __be16 destination_port;
};

// For ip based rate limit
__be32 source_ip;



#endif //RATELIMIT_TYPES_H
