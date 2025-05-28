#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <jansson.h>
#include <pthread.h>
#include <linux/types.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include "../include/ratelimit_types.h"

#define JSON_FILENAME "../data/ratelimit_config.json"

#define IP_PORT_PAIR_RL "/sys/fs/bpf/xdp/globals/ip_port_pair_rl"
#define IPS_RL "/sys/fs/bpf/xdp/globals/ips_rl"

json_t *root = NULL;

void *parse_ip_port_pair(void *arg)
{
    int state;
    int ipp_fd = bpf_obj_get(IP_PORT_PAIR_RL);
    if (ipp_fd < 0)
    {
        printf("Failed to open ip_port_pair_rl ebpf map\n");
    }

    json_t *array = (json_t *)arg;
    size_t index;
    json_t *value;

    json_array_foreach(array, index, value)
    {
        const char *source_ip_str = json_string_value(json_object_get(value, "source_ip"));
        int destination_port_int = json_integer_value(json_object_get(value, "destination_port"));
        int allow = json_integer_value(json_object_get(value, "allow"));

        struct ip_port_pair ipp;

        if (inet_pton(AF_INET, source_ip_str, &ipp.source_ip) != 1)
        {
            fprintf(stderr, "Invalid source IP: %s\n", source_ip_str);
            continue;
        }

        ipp.destination_port = htons(destination_port_int);

        printf("[ip_port_pair] src_ip=%d port=%d allow=%d\n",
               ipp.source_ip, ipp.destination_port, allow);

        state = bpf_map_update_elem(ipp_fd, &ipp, &allow, BPF_ANY);

        if (state < 0)
        {
            printf("Failed to update three_tuples ebpf map\n");
        }
    }
}

void *parse_source_ips(void *arg)
{
    int state;
    int ipsrl_fd = bpf_obj_get(IPS_RL);
    if (ipsrl_fd < 0)
    {
        printf("Failed to open IPS_RL BPF map\n");
    }

    json_t *array = (json_t *)arg;
    size_t index;
    json_t *value;

    json_array_foreach(array, index, value)
    {
        const char *source_ip_str = json_string_value(json_object_get(value, "source_ip"));
        int allow = json_integer_value(json_object_get(value, "allow"));

        __be32 source_ip;

        if (inet_pton(AF_INET, source_ip_str, &source_ip) != 1)
        {
            fprintf(stderr, "Invalid source IP in ip_to_any\n");
            continue;
        }

        printf("[source_ip] src_ip=%d allow=%d\n", source_ip, allow);

        state = bpf_map_update_elem(ipsrl_fd, &source_ip, &allow, BPF_ANY);

        if (state < 0)
        {
            printf("Failed to update ips_rl ebpf map\n");
        }
    }
}

int main(void)
{
    json_error_t error;
    root = json_load_file(JSON_FILENAME, 0, &error);

    if (!root)
    {
        fprintf(stderr, "Error parsing JSON from %s: %s\n", JSON_FILENAME, error.text);
        return 1;
    }

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, parse_ip_port_pair, (void *)json_object_get(root, "ip_port_pair"));
    pthread_create(&thread2, NULL, parse_source_ips, (void *)json_object_get(root, "source_ips"));

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}