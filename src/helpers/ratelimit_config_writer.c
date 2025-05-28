#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELD 64
#define CONFIG_PATH "../src/data/ratelimit_config.json"

void write_ip_port_pair(FILE *fp) {
    char source_ip[MAX_FIELD];
    char port[MAX_FIELD];
    char allow[MAX_FIELD];

    fprintf(fp, "  \"ip_port_pair\": [\n");

    int first = 1;
    while (1) {
        printf("source_ip: ");
        fgets(source_ip, sizeof(source_ip), stdin);
        source_ip[strcspn(source_ip, "\n")] = 0;

        printf("port: ");
        fgets(port, sizeof(port), stdin);
        port[strcspn(port, "\n")] = 0;

        printf("allow (0 or 1): ");
        fgets(allow, sizeof(allow), stdin);
        allow[strcspn(allow, "\n")] = 0;

        if (!first) fprintf(fp, ",\n");
        fprintf(fp, "    {\"source_ip\": \"%s\", \"port\": %s, \"allow\": %s}", source_ip, port, allow);
        first = 0;

        char more[4];
        printf("Add another to 'ip_port_pair'? (y/n): ");
        fgets(more, sizeof(more), stdin);
        if (more[0] != 'y' && more[0] != 'Y') break;
    }

    fprintf(fp, "\n  ]");
}

void write_source_ips(FILE *fp) {
    char source_ip[MAX_FIELD];
    char allow[MAX_FIELD];

    fprintf(fp, ",\n  \"source_ips\": [\n");

    int first = 1;
    while (1) {
        printf("source_ip: ");
        fgets(source_ip, sizeof(source_ip), stdin);
        source_ip[strcspn(source_ip, "\n")] = 0;

        printf("allow (0 or 1): ");
        fgets(allow, sizeof(allow), stdin);
        allow[strcspn(allow, "\n")] = 0;

        if (!first) fprintf(fp, ",\n");
        fprintf(fp, "    {\"source_ip\": \"%s\", \"allow\": %s}", source_ip, allow);
        first = 0;

        char more[4];
        printf("Add another to 'source_ips'? (y/n): ");
        fgets(more, sizeof(more), stdin);
        if (more[0] != 'y' && more[0] != 'Y') break;
    }

    fprintf(fp, "\n  ]");
}

int main(void) {
    char mode[8];
    printf("Do you want to create a new config or reset the existing one? (new/reset): ");
    fgets(mode, sizeof(mode), stdin);
    mode[strcspn(mode, "\n")] = 0;

    if (strcmp(mode, "reset") == 0) {
        FILE *check = fopen(CONFIG_PATH, "r");
        if (!check) {
            printf("File not found. Cannot reset.\n");
            return 1;
        }
        fclose(check);

        FILE *fp = fopen(CONFIG_PATH, "w");
        if (!fp) {
            perror("fopen");
            return 1;
        }
        fprintf(fp, "{}\n");
        fclose(fp);
        printf("File reset: %s\n", CONFIG_PATH);
        return 0;
    }

    FILE *fp = fopen(CONFIG_PATH, "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    fprintf(fp, "{\n");
    write_ip_port_pair(fp);
    write_source_ips(fp);
    fprintf(fp, "\n}\n");
    fclose(fp);

    printf("JSON config written to %s\n", CONFIG_PATH);
    return 0;
}
