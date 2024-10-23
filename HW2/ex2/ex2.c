#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int isIPAddressValid(const char* str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str, &(sa.sin_addr)) != 0;
}

int isIPAddress(const char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            continue;
        }
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

void printHostNameFromAddr(const char* strAddr) {
    struct in_addr addr;
    if(!isIPAddressValid(strAddr)) {
        printf("Invalid address\n");
        return;
    }
    if (inet_aton(strAddr, &addr)) {
        struct hostent* host = gethostbyaddr(&addr, sizeof(addr), AF_INET);
        if (host) {
            printf("Official domain name: %s\n", host->h_name);
            for (int i = 0; host->h_aliases[i]; i++) {
                if (i == 0) {
                    printf("Alias domain name:");
                }
                printf(" %s", host->h_aliases[i]);
            }
            printf("\n");
        } else {
            printf("Not found information\n");
        }
    } else {
        printf("Invalid address\n");
    }
}

void printAddrFromHostName(const char* hostName) {
    struct hostent* host = gethostbyname(hostName);
    if (host) {
        printf("Official name: %s\n", host->h_name);
        for (int i = 0; host->h_aliases[i]; i++) {
            if (i == 0) {
                printf("Alias name:");
            }
            printf(" %s", host->h_aliases[i]);
        }
        printf("\n");

        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
        for (int i = 1; host->h_addr_list[i]; i++) {
            if (i == 1) {
                printf("Alias IP:");
            }
            printf(" %s", inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        }
        printf("\n");
    } else {
        printf("Not found information\n");
    }
}

int main() {
    char input[1024];
    while(1)
    {
        printf("Enter IP address or domain name: ");
        fgets(input, sizeof(input) - 1, stdin);
        while ( input[strlen(input) - 1] == '\r' ||
                input[strlen(input) - 1] == '\n')
        {
            input[strlen(input) - 1] = 0;
        }
        if (strlen(input) < 1) {
                break;
            }
        if (isIPAddress(input)) {
            printHostNameFromAddr(input);
        } else {
            printAddrFromHostName(input);
        }
    }

    return 0;
}
