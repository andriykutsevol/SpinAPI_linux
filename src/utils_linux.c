#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include "utils_linux.h"




void listdir(const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);
            listdir(path, indent + 2);
        } else {
            printf("%*s- %s\n", indent, "", entry->d_name);
        }
    }
    closedir(dir);
}







int pci_get_resource0(int dev_id, char *resource0_path){

  char *result = (char*) malloc(512+1 * sizeof(char));
  
  //open /sys/devices/pci0000:00/ directory
  
  DIR *dir;
  struct dirent *de;

  FILE* fd;
  char path[512];

  int found = 0;
  int number = 0;
  char number_s[16];

  const char *pci_sysdir = "/sys/devices/pci0000:00";

  listdir(pci_sysdir);




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
