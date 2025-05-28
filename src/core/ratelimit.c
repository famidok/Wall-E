#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/types.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <netinet/in.h>
#include <string.h>
#include "../include/ratelimit_types.h"
#include "../include/ratelimit_maps.h"

SEC("prog")
int ratelimit(struct xdp_md *ctx)
{
    struct ip_port_pair ipp;
    memset(&ipp, 0, sizeof(ipp));

    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;

    if ((void *)(eth + 1) > data_end)
    {
        return XDP_PASS;
    }

    if (eth->h_proto != htons(ETH_P_IP))
    {
        return XDP_PASS;
    }

    struct iphdr *iph = (struct iphdr *)(eth + 1);

    if ((void *)(iph + 1) > data_end)
    {
        return XDP_PASS;
    }

    ipp.source_ip = iph->saddr;
    source_ip = iph->saddr;

    if (iph->protocol == IPPROTO_TCP)
    {
        struct tcphdr *tcph = (struct tcphdr *)(iph + 1);
        if ((void *)(tcph + 1) > data_end)
        {
            return XDP_DROP;
        }
        ipp.destination_port = tcph->dest;
    }
    else if (iph->protocol == IPPROTO_UDP)
    {
        struct udphdr *udph = (struct udphdr *)(iph + 1);
        if ((void *)(udph + 1) > data_end)
        {
            return XDP_DROP;
        }
        ipp.destination_port = udph->dest;
    }
    /*
    // Check limit active values
    // ip_port_pair
    __u64 *ipp_value = bpf_map_lookup_elem(&ip_port_pair_rl, &ipp);
    if (ipp_value > MAX_RATE_LIMIT)
    {
        return XDP_DROP;
    }
    else
    {
        *ipp_value += 1;
        bpf_map_update_elem(&ip_port_pair_rl, &ipp, &ipp_value, BPF_ANY);
    }

    // ip
    __u64 *ip_value = bpf_map_lookup_elem(&ips_rl, &source_ip);
    if (ip_value > MAX_RATE_LIMIT)
    {
        return XDP_DROP;
    }
    else
    {
        *ip_value += 1;
        bpf_map_update_elem(&ip_port_pair_rl, &source_ip, &ip_value, BPF_ANY);
    }
    */
    // Rate limit check for IP + Port pair
    __u64 *ipp_value = bpf_map_lookup_elem(&ip_port_pair_rl, &ipp);
    __u64 new_ipp_val;

    if (ipp_value) {
        new_ipp_val = *ipp_value + 1;
    } else {
        new_ipp_val = 1;
    }

    if (new_ipp_val > MAX_RATE_LIMIT) {
        return XDP_DROP;
    }

    bpf_map_update_elem(&ip_port_pair_rl, &ipp, &new_ipp_val, BPF_ANY);

    // Rate limit check for source IP only
    __u64 *ip_value = bpf_map_lookup_elem(&ips_rl, &source_ip);
    __u64 new_ip_val;

    if (ip_value) {
        new_ip_val = *ip_value + 1;
    } else {
        new_ip_val = 1;
    }

    if (new_ip_val > MAX_RATE_LIMIT) {
        return XDP_DROP;
    }

    bpf_map_update_elem(&ips_rl, &source_ip, &new_ip_val, BPF_ANY);


    return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";