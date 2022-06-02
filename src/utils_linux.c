#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include "debug.h"
#include <sys/mman.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include "utils_linux.h"
#include <stdarg.h>


char* concat(int count, ...)
{
    va_list ap;
    int i;

    // Find required length to store merged string
    int len = 1; // room for NULL
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    // Allocate memory to concat strings
    char *merged = calloc(sizeof(char),len);
    int null_pos = 0;

    // Actually concatenate strings
    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(merged+null_pos, s);
        null_pos += strlen(s);
    }
    va_end(ap);

    return merged;
}


char *strremove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        size_t size = 0;
        while ((p = strstr(p, sub)) != NULL) {
            size = (size == 0) ? (p - str) + strlen(p + len) + 1 : size - len;
            memmove(p, p + len, size - (p - str));
        }
    }
    return str;
}



int find_resource0_listdir(const char *name, int dev_id, char *result)
{
    DIR *dir;
    struct dirent *de;
    FILE* fd;
    char *path_to_resource0;

    if (!(dir = opendir(name)))
        return 2;

    while ((de = readdir(dir)) != NULL) {

        char path[1024];
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", name, de->d_name);

        if (de->d_type == DT_DIR) {

            int res = find_resource0_listdir(path, dev_id, result);
            if (res == 0){
                return 0;
            }

        } else {

            if (! strcmp(de->d_name, "device")){

              if ( (fd = fopen(path, "r")) == NULL )
                continue;  

              char number_s[16];
              int number;
              fgets (number_s, 16, fd);
              fclose(fd);

              sscanf(number_s, "%x", &number);

              if (number == dev_id){
                    char *path_to_pci_device = NULL;
                    path_to_resource0 = concat(2, path, "/resource0");
                    path_to_resource0 = strremove(path_to_resource0, "/device/resource0");
                    path_to_resource0 = concat(2, path_to_resource0, "/resource0");
                    stpcpy(result, path_to_resource0);
                    closedir(dir);
                    return 0;
              }

            }
        }
    }
    closedir(dir);
    return 1;
}


int pci_get_resource0(int dev_id, char *result){

    const char *pci_sysdir = "/sys/devices/pci0000:00";
    find_resource0_listdir(pci_sysdir, dev_id, result);

    FILE *file;
    if ((file = fopen(result, "r")))
    {
        debug (DEBUG_INFO, "pci_get_resource0() successful.");
        fclose(file);
        return 0;
    }else{
        debug (DEBUG_ERROR, "pci_get_resource0(): Cannot get resource0 for the device");
        return -1;
    }  

}



int get_mmap_virt_addr(const char *resource0_path, 
                        int address, 
                        void *virt_addr, 
                        void *map_base, 
                        const int map_size){

    int fd;
    off_t target, target_base;
    int items_count = 1;
    uint64_t read_result;

    target = (off_t)address;
    
    target_base = target & ~(sysconf(_SC_PAGE_SIZE)-1);
    // if (target + items_count*type_width - target_base > map_size)
    //     map_size = target + items_count*type_width - target_base;

    if((fd = open(resource0_path, O_RDWR | O_SYNC)) == -1){
        debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot get resource0 for the device");
        return -1;
    }

    map_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target_base);
    printf("PCI Memory mapped to address 0x%08lx.\n", (unsigned long) map_base);

    if(map_base == (void *) -1){
       debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot mmap"); 
    }

    virt_addr = map_base + target - target_base;
    printf("sofsafe: mmap_outw(): resource0_path: %s\n",  resource0_path);
    printf("sofsafe: mmap_outw(): map_base: 0x%08lx\n",  (unsigned long)map_base);
    printf("sofsafe: mmap_outw(): target: 0x%08lx\n",  target);
    printf("sofsafe: mmap_outw(): target_base: 0x%08lx\n",  target_base);
    printf("sofsafe: mmap_outw(): virt_addr: 0x%08lx\n", (unsigned long)virt_addr); 

    return 0;
}




int mmap_inw(const char *resource0_path, int address, int *fw_result){

    void *virt_addr;
    uint64_t read_result;
    const int map_size = 4096UL;
    const int type_width = 4;
    void *map_base;

    get_mmap_virt_addr(resource0_path, address, virt_addr, map_base, map_size);

    read_result = *((uint32_t *) virt_addr);

    // *fw_result = (int)read_result;

    // if(munmap(map_base, map_size) == -1) debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot munmap"); 

    return 0;
}



int mmap_outw(const char *resource0_path, int address, unsigned int data){


    int fd;
    void *map_base, *virt_addr;
    int type_width;
    off_t target, target_base;
    //On x86-64, for example, sysconf(_SC_PAGESIZE) reports 4096 as page size
    int map_size = 4096UL;
    int items_count = 1;

    uint64_t read_result;

    target = (off_t)address;
    type_width = 4;
    
    target_base = target & ~(sysconf(_SC_PAGE_SIZE)-1);
    if (target + items_count*type_width - target_base > map_size)
        map_size = target + items_count*type_width - target_base;

    if((fd = open(resource0_path, O_RDWR | O_SYNC)) == -1){
        debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot get resource0 for the device");
        return -1;
    }

    map_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target_base);
    printf("PCI Memory mapped to address 0x%08lx.\n", (unsigned long) map_base);

    if(map_base == (void *) -1){
       debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot mmap"); 
    }

    virt_addr = map_base + target - target_base;
    printf("sofsafe: mmap_outw(): resource0_path: %s\n",  resource0_path);
    printf("sofsafe: mmap_outw(): map_base: 0x%08lx\n",  (unsigned long)map_base);
    printf("sofsafe: mmap_outw(): target: 0x%08lx\n",  target);
    printf("sofsafe: mmap_outw(): target_base: 0x%08lx\n",  target_base);
    printf("sofsafe: mmap_outw(): virt_addr: 0x%08lx\n", (unsigned long)virt_addr); 
    
    *((uint32_t *) virt_addr) = data;

    if(munmap(map_base, map_size) == -1) debug (DEBUG_ERROR, "pci_get_firmwareid(): Cannot munmap"); 

    return 0; 


}