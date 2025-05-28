#ifndef RATELIMIT_MAPS_H
#define RATELIMIT_MAPS_H

#include "ratelimit_types.h"
#include <bpf/bpf_helpers.h>

#define MAX_ENTRY 1000000

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRY);
    __type(key, struct ip_port_pair);
    __type(value, __u64);
    __uint(pinning, LIBBPF_PIN_BY_NAME);
    __uint(map_flags, BPF_F_NO_PREALLOC);
} ip_port_pair_rl SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRY);
    __type(key, __be32);
    __type(value, __u64);
    __uint(pinning, LIBBPF_PIN_BY_NAME);
    __uint(map_flags, BPF_F_NO_PREALLOC);
} ips_rl SEC(".maps");

#endif //RATELIMIT_MAPS_H
