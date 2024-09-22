#include "info.h"

char* get_hostname() {
    FILE* hostname_file;
    struct stat file_stat;

    stat("/etc/hostname", &file_stat);
    hostname_file = fopen("/etc/hostname", "r");

    char* buf = malloc(sizeof(char) * file_stat.st_size);

    fread(buf, sizeof(char), file_stat.st_size, hostname_file);

    fclose(hostname_file);
    if (buf[file_stat.st_size-1] == '\n') buf[file_stat.st_size-1] = '\0';

    return buf;
}

char* get_username() {
    return getenv("USER");
}

char* get_shell() {
    return getenv("SHELL");
}

struct sysinfo get_sysinfo() {
    struct sysinfo info;

    sysinfo(&info);

    return info;
}

struct utsname get_uname() {
    struct utsname u;

    uname(&u);

    return u;
}