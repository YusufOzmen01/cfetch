#ifndef INFO_H
#define INFO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

char* get_shell();
char* get_username();

char* get_hostname();

struct sysinfo get_sysinfo();
struct utsname get_uname();


#endif