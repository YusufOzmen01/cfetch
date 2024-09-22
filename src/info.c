#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

char* get_hostname() {
    FILE* hostname_file;
    struct stat file_stat;

    stat("/etc/hostname", &file_stat);
    hostname_file = fopen("/etc/hostname", "r");

    char* buf = malloc(sizeof(char) * file_stat.st_size);

    fread(buf, sizeof(char), file_stat.st_size, hostname_file);

    fclose(hostname_file);

    return buf;
}

char* get_username() {
    return getenv("USER");
}