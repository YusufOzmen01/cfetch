#ifndef DISK_H
#define DISK_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>

struct Disk {
    char* mount_point;
    char* filesystem;

    unsigned long capacity, used;   
};

int get_disk_information(struct Disk* disks, int max_count);

#endif