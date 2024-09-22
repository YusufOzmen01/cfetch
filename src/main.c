#include <stdio.h>

#include "info.h"

void main() {
    char* hostname = get_hostname();
    char* username = get_username();
    char* shell = get_shell();
    struct sysinfo info = get_sysinfo();
    struct utsname u = get_uname();

    printf("%s@%s\n", username, hostname);
    printf("Uptime: %d\n", info.uptime);
    printf("Ram usage: %u/%u\n", (info.totalram - info.freeram)/(1024*1024), info.totalram/(1024*1024));
    printf("Shell: %s\n", shell);
    printf("Kernel: %s\n", u.release);
}