#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "utils_linux.h"

int pci_get_resource0(int dev_id, char *resource0_path){

  char *result = (char*) malloc(512+1 * sizeof(char));
  
  //open /sys/devices/pci0000:00/ directory
  
  DIR *dir;
  struct dirent *de;


  if ( !(dir = opendir("/sys/devices/pci0000:00/"))){
      return 1;
  }

  while ( (de = readdir(dir)) ) {
    printf("de->d_name: %s\n", de->d_name);
  }


  strcpy(resource0_path, "pci_get_resource0");
  return 0;

}
