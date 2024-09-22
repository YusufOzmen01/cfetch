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

    char* buf = calloc(sizeof(char), file_stat.st_size);

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
    char* filedata = calloc(sizeof(char), 1000000);
    FILE* mount_list = fopen("/proc/mounts", "r");

    int count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, mount_list);

        if (i < 1) break;

        count += i;
    }

    fclose(mount_list);

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

        char* mount_point = calloc(sizeof(char), size);
        strncpy(mount_point, mount_point_start, size);

        // Obtain the filesystem

        char* filesystem_start = mount_point_start + size + 1; // Filesystem start after mount point

        size = 0;
        while (filesystem_start[size] != ' ') { // Calculate the size of the filesystem by looping until we hit a space
            size++;
        }

        char* filesystem = calloc(sizeof(char), size);
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

struct CPU {
    char* name;
    int core_count;
    float max_frequency; 
};

struct CPU get_cpu() {
    char* filedata = calloc(sizeof(char), 1000000);
    FILE* cpu_info = fopen("/proc/cpuinfo", "r");
    struct CPU cpu;

    int count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, cpu_info);

        if (i < 1) break;

        count += i;
    }

    fclose(cpu_info);

    filedata = realloc(filedata, sizeof(char) * count);

    int size = 0, start = 0;

    char* model_name_pos = strstr(filedata, "model name");
    char* cores_pos = strstr(filedata, "cpu cores");
    char* frequency_pos = strstr(filedata, "cpu MHz");
    
    while (model_name_pos[start] != ':') { // Calculate the size of the mount point by looping until we hit a space
        start++;
    }
    start += 2;
    
    while (model_name_pos[start + size] != '\n') { // Calculate the size of the mount point by looping until we hit a space
        size++;
    }

    cpu.name = calloc(sizeof(char), size);
    strncpy(cpu.name, model_name_pos + start, size);

    start = 0;
    while (cores_pos[start] != ':') { // Calculate the size of the mount point by looping until we hit a space
        start++;
    }
    start += 2;
    
    size = 0;
    while (cores_pos[start + size] != '\n') { // Calculate the size of the mount point by looping until we hit a space
        size++;
    }

    char* buf = calloc(sizeof(char), size);
    strncpy(buf, cores_pos + start, size);

    cpu.core_count = atoi(buf);
    free(buf);

    start = 0;
    while (frequency_pos[start] != ':') { // Calculate the size of the mount point by looping until we hit a space
        start++;
    }
    start += 2;
    
    size = 0;
    while (frequency_pos[start + size] != '\n') { // Calculate the size of the mount point by looping until we hit a space
        size++;
    }

    buf = calloc(sizeof(char), size);
    strncpy(buf, frequency_pos + start, size);

    cpu.max_frequency = atof(buf);
    free(buf);

    free(filedata);

    return cpu;
}

char* get_gpu_name() {
    char* filedata = calloc(sizeof(char), 1000000);
    FILE* pci_info = popen("lspci", "r");

    int count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, pci_info);

        if (i < 1) break;

        count += i;
    }

    pclose(pci_info);

    filedata = realloc(filedata, sizeof(char) * count);

    int size = 0;

    char* name_pos = strstr(filedata, "3D controller");
    if (name_pos == NULL) return NULL;

    name_pos += 15;
    
    while (name_pos[size] != '\n') { // Calculate the size of the mount point by looping until we hit a space
        size++;
    }

    char* name = calloc(sizeof(char), size);
    strncpy(name, name_pos, size);

    return name;
}

struct ASCIIArt {
    int max_size;
    char** lines;
};

struct ASCIIArt* get_ascii_art() {
    char* filedata = calloc(sizeof(char), 1000000);
    FILE* os_info = fopen("/etc/os-release", "r");

    int count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, os_info);

        if (i < 1) break;

        count += i;
    }

    fclose(os_info);

    filedata = realloc(filedata, sizeof(char) * count);

    int size = 0;

    char* name_pos = strstr(filedata, "ID=");
    if (name_pos == NULL) return NULL;

    name_pos += 3;
    
    while (name_pos[size] != '\n') { // Calculate the size of the mount point by looping until we hit a space
        size++;
    }

    char* id = calloc(sizeof(char), size);
    strncpy(id, name_pos, size);

    free(filedata);
    filedata = calloc(sizeof(char), 1000000);

    char* path = calloc(sizeof(char), size + 9);
    sprintf(path, "logo/%s.txt", id);
    free(id);

    FILE* ascii_data = fopen(path, "r");
    free(path);

    count = 0;
    while (1) {
        int i = fread(filedata + count, sizeof(char), 64, os_info);

        if (i < 1) break;

        count += i;
    }

    fclose(os_info);

    filedata = realloc(filedata, sizeof(char) * count);
    char* old = filedata;

    struct ASCIIArt* art = malloc(sizeof(struct ASCIIArt));
    char** lines = calloc(sizeof(char*), 32);

    int line_count = 0, max_size = 0;
    while (1) {
        if (filedata > old + count) break;

        size = 0;
        while (size < count && filedata[size] != '\n') size++;

        if (size > max_size) max_size = size;

        lines[line_count] = calloc(sizeof(char), size);
        strncpy(lines[line_count], filedata, size);

        filedata += size + 1;
        line_count++;
    }

    art->lines = lines;
    art->max_size = max_size;

    return art;
}

void main() {
    struct ASCIIArt* art = get_ascii_art();
    printf("Max size: %d\n", art->max_size);

    char* hostname = get_hostname();
    char* username = get_username();
    char* shell = get_shell();
    char* gpu_name = get_gpu_name();
    struct sysinfo info = get_sysinfo();
    struct utsname u = get_uname();
    struct CPU cpu = get_cpu();

    int hrs = (info.uptime / 3600);
    int mins = (info.uptime / 60) - hrs * 60;
    int secs = info.uptime - hrs * 3600 - mins * 60;
    
    printf("%s@%s\n", username, hostname);
    printf("CPU: %dx %s [%.2fMHz]\n", cpu.core_count, cpu.name, cpu.max_frequency);
    if (gpu_name != NULL) printf("GPU: %s\n", gpu_name);
    printf("Uptime: %dhrs %dmins %dsecs\n", hrs, mins, secs);
    printf("Ram usage: %u/%u\n", (info.totalram - info.freeram)/(1024*1024), info.totalram/(1024*1024));
    printf("Shell: %s\n", shell);
    printf("Kernel: %s\n", u.release);

    struct Disk* disklist = calloc(sizeof(struct Disk), 5);
    int count = get_disk_information(disklist, 5);

    for (int i = 0; i < count; i++) {
        printf("[%s] %uMiB/%uMiB - %s\n", disklist[i].mount_point, disklist[i].used, disklist[i].capacity, disklist[i].filesystem);
    }
}