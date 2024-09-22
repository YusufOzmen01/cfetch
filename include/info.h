#ifndef INFO_H
#define INFO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>

char* get_hostname();
char* get_username();
struct sysinfo get_sysinfo();

#endif