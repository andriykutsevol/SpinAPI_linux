#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include "utils_linux.h"



// char* concat(const char *s1, const char *s2)
// {
//     char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
//     // in real code you would check for errors in malloc here
//     strcpy(result, s1);
//     strcat(result, s2);
//     return result;
// }


// // If you did happen to be bothered by performance then you would want to avoid repeatedly scanning the 
// // input buffers looking for the null-terminator.

// char* concat(const char *s1, const char *s2)
// {
//     const size_t len1 = strlen(s1);
//     const size_t len2 = strlen(s2);
//     char *result = malloc(len1 + len2 + 1); // +1 for the null-terminator
//     // in real code you would check for errors in malloc here
//     memcpy(result, s1, len1);
//     memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
//     return result;
// }



// We can write a useful variadic function to concatenate any number of strings:
#include <stdlib.h>       // calloc
#include <stdarg.h>       // va_*
#include <string.h>       // strlen, strcpy

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




void find_resource0_listdir(const char *pci_sysdir, int dev_id, char *result)
{
    DIR *dir;
    struct dirent *de;
    FILE* fd;
    char *path_to_resource0;

    if (!(dir = opendir(pci_sysdir)))
        return;

    while ((de = readdir(dir)) != NULL) {

        char path[1024];
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", pci_sysdir, de->d_name);

        if (de->d_type == DT_DIR) {
            find_resource0_listdir(path, dev_id, result);

        } else {

            if (! strcmp(de->d_name, "device")){

              //printf("path:%s\n", path);
              if ( (fd = fopen(path, "r")) == NULL )
                continue; 

              char number_s[16];
              int number;

              fgets (number_s, 16, fd);
              fclose(fd);

              sscanf(number_s, "%x", &number);

              if (number == dev_id){
                printf("number: %x, dev_id: %x\n", number, dev_id);
                char *path_to_pci_device = NULL;
                path_to_pci_device = strremove(path, "/device");

                printf("path_to_pci_device: %s\n", path_to_pci_device);

                path_to_resource0 = concat(2, path_to_pci_device, "/resource0");

                printf("path_to_resource0: %s\n", path_to_resource0);

                stpcpy(result, path_to_resource0);

                closedir(dir);

                return 0;

              }
            }
        }
    }
}







int pci_get_resource0(int dev_id, char *result){


  //open /sys/devices/pci0000:00/ directory
  const char *pci_sysdir = "/sys/devices/pci0000:00";

  find_resource0_listdir(pci_sysdir, dev_id, result);

  return 0;

}
