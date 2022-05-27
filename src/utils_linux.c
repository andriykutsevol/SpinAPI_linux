#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include "utils_linux.h"



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




void find_resource0_listdir(const char *pci_sysdir, int dev_id, const char *top_path)
{
    DIR *dir;
    struct dirent *de;
    FILE* fd;

    if (!(dir = opendir(pci_sysdir)))
        return;

    while ((de = readdir(dir)) != NULL) {

        char path[1024];
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", pci_sysdir, de->d_name);

        if (de->d_type == DT_DIR) {
            //printf("%*s[%s]\n", indent, "", de->d_name);
            find_resource0_listdir(path, dev_id, pci_sysdir);

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
                printf("path: %s, top_path: %s\n", path, top_path);
                char *path_to_pci_device = NULL;
                path_to_pci_device = strremove(path, "/device");

                printf("path_to_pci_device: %s\n", path_to_pci_device);

                char path_to_resource0[1024];

                sscanf(&path_to_resource0, "%s/resource0", path_to_pci_device);

                printf("path_to_resource0: %s", &path_to_resource0);

                // if ( (fd = fopen(path, "r")) == NULL ){
                //   return -1;
                // }else{

                // }

              }
            }
        }
    }
    closedir(dir);
}







int pci_get_resource0(int dev_id, char *resource0_path){

  char *result = (char*) malloc(512+1 * sizeof(char));
  
  //open /sys/devices/pci0000:00/ directory
  const char *pci_sysdir = "/sys/devices/pci0000:00";

  find_resource0_listdir(pci_sysdir, dev_id, pci_sysdir);




  // if ( !(dir = opendir(pci_sysdir))){
  //     return 1;
  // }

  // while ( (de = readdir(dir)) ) {
  //   printf("de->d_name: %s\n", de->d_name);

  //   sprintf(path, "%s/%s/device", pci_sysdir, de->d_name); 

  //   printf("path to device id file: %s \n", path);

  //   if ( (fd = fopen(path, "r")) == NULL )
  //     continue;  


  //   fgets (number_s, 16, fd); 
  //   fclose(fd);

  //   sscanf(number_s, "%x", &number);

  //   printf("number: %x, dev_id: %x\n", number, dev_id);
  

  //   printf("--------------------\n");    

  // }

  // closedir(dir);

  // strcpy(resource0_path, "pci_get_resource0");

  return 0;

}
