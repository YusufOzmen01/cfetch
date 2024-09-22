#include "disk.h"

int get_disk_information(struct Disk* disks, int max_count) {
    char* filedata = malloc(sizeof(char) * 1000000);
    FILE* mount_list = fopen("/proc/mounts", "r");

    int count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, mount_list);

        if (i < 1) break;

        count += i;
    }

    filedata = realloc(filedata, sizeof(char) * count);
    char* old = filedata;

    int i = 0, size = 0;
    while (1) {
        if (i == max_count) break;
        int line_size = 0;

        while (filedata + line_size <= old + count && filedata[line_size] != '\n') line_size++;
        if (filedata == NULL || line_size == 0) break;
        
        char* mount_point_start = strstr(filedata, "/mnt");
        if (mount_point_start == NULL || mount_point_start > filedata + line_size) {
            mount_point_start = strstr(filedata, "/run/media");

            if (mount_point_start == NULL || mount_point_start > filedata + line_size) {
                mount_point_start = strstr(filedata, "/ ");

                if (mount_point_start == NULL || mount_point_start > filedata + line_size) {
                    filedata = filedata + line_size + 1;

                    continue;
                };
            };
        }

        size = 0;
        while (mount_point_start[size] != ' ') { // Calculate the size of the mount point by looping until we hit a space
            size++;
        }

        char* mount_point = malloc(sizeof(char) * size);
        strncpy(mount_point, mount_point_start, size);

        // Obtain the filesystem

        char* filesystem_start = mount_point_start + size + 1; // Filesystem start after mount point

        size = 0;
        while (filesystem_start[size] != ' ') { // Calculate the size of the filesystem by looping until we hit a space
            size++;
        }

        char* filesystem = malloc(sizeof(char) * size);
        strncpy(filesystem, filesystem_start, size);

        struct statvfs disk_stat;
        statvfs(mount_point, &disk_stat);

        disks[i].mount_point = mount_point;
        disks[i].filesystem = filesystem;
        disks[i].capacity = (disk_stat.f_blocks * disk_stat.f_bsize) / (1024*1024);
        disks[i].used = ((disk_stat.f_blocks - disk_stat.f_bfree) * disk_stat.f_bsize) / (1024*1024);
        i++;

        filedata = filedata + line_size + 1;
    }

    free(old);

    return i;
}