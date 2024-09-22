#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

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

struct Disk {
    char* mount_point;
    char* filesystem;

    unsigned long capacity, used;   
};

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

    struct Disk* disklist = malloc(sizeof(struct Disk) * 10);
    int count = get_disk_information(disklist, 10);

    for (int i = 0; i < count; i++) {
        printf("[%s] %u/%u - %s\n", disklist[i].mount_point, disklist[i].used, disklist[i].capacity, disklist[i].filesystem);
    }
}