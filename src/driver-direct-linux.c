#define _GNU_SOURCE

#include <sys/io.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>

#include "driver-os.h"
#include "debug.h"
#include "util.h"
#include "utils_linux.h"

#define MAX_NUM_BOARDS 32

static int dev_id_array[MAX_NUM_BOARDS];
static int base_addr_array[MAX_NUM_BOARDS];
static char* pci_resource0path_array = NULL;
static int my_getline (char **lineptr, size_t * n, FILE * stream);
static int num_cards = -1;

/**
 * This function returns the number of boards with a given vendor id.
 *
 *\param vend_id The vendor ID user for SpinCore boards will be passed
 * as a parameter.
 *\return number of boards present, or -1 on error.
 */

int
os_count_boards (int vend_id)
{
  FILE *f;
  char *buf;
  int i; 
  size_t size;

  size = 1000;

  // We clean up the memory because is function
  // is called sereval times during initialization
  if(pci_resource0path_array){
    free(pci_resource0path_array);
  }
  //512 is the max length of path.  (+1 is for null terminator)
  pci_resource0path_array = (char*) malloc(MAX_NUM_BOARDS * (512 +1) * sizeof(char)); 


  buf = (char *) malloc (sizeof (char) * size);
  if (!buf) {
    debug (DEBUG_ERROR, "os_count_boards: Internal error: failed to allocate buffer.");
    return -1;
  }

  f = fopen ("/proc/bus/pci/devices", "r");

  if (!f) {
		debug (DEBUG_ERROR, "os_count_boards: could not open /proc/bus/pci/devices");
    free(buf);
    return -1;
  }

  int id, dummy;
  char name[512];

  int detected_base;
  int detected_vend_id, detected_dev_id;

  i = 0;
  while (1) {
    if (my_getline (&buf, &size, f) < 0) {
			break;
		}
		// ret = sscanf (buf, "%x %x %s %x ", &dummy, &id, name, &detected_base); 
		// commented since ret is never used
    sscanf (buf, "%x %x %s %x ", &dummy, &id, name, &detected_base);
      
    detected_vend_id = (0xFFFF0000 & id) >> 16;
    detected_dev_id = 0x0000FFFF & id;
    detected_base &= ~(0x01);	// bit 0 of base address is the IO/Mem bit and not part of the address

    if (detected_vend_id == vend_id) {
			if (i >= MAX_NUM_BOARDS) {
	      debug (DEBUG_ERROR, "os_count_boards: Found too many boards");
	      fclose(f);
	      free(buf);
	      return -1;
			}

			debug (DEBUG_INFO, "os_count_boards: Found dev_id 0x%x, base_address 0x%x\n", 
			  detected_dev_id, 
			  detected_base);

			base_addr_array[i] = detected_base;
			dev_id_array[i] = detected_dev_id;

      // We are trying to get the "resource0" for every card.
      // If it is not a PCI card it will not find anything.
      // Later we will use this output for cards that for sure has a PCIe interface.
      if (pci_get_resource0(detected_dev_id, &pci_resource0path_array[512*i], i) == -1){
        debug (DEBUG_INFO, "os_count_boards(): It is not a PCI card.");
      }

      printf("os_count_boards, i: %d\n", i);
      printf("os_count_boards, detected_dev_id: %d\n", detected_dev_id);
      printf("os_count_boards, &pci_resource0path_array[512*i]: %s\n", &pci_resource0path_array[512*i]);

			i++;
		}
  }

  fclose(f);
  free(buf);
  return i;
}

/**
 * Initialize OS so the outxx functions have access to the hardware. IO permissions
 * are obtained with the iopl() function, which means you must be running as root.
 *
 *\return -1 on error
 */
int
os_init (int card_num)
{
  num_cards = os_count_boards (0x10e8);
  if (num_cards < 0) {
    debug (DEBUG_ERROR, "os_init: os_count_cards() failed.");
    return -1;
  }

  if (card_num >= num_cards || card_num < 0) {
    debug (DEBUG_ERROR, "os_init: Card number out of range");
    return -1;
  }

  // print out some info about the system to the log to aid debugging
  FILE *f;
  char buf[512];
  f = fopen ("/proc/version", "r");
	
  if (!f) {
    debug (DEBUG_INFO, "os_init: unable to open /proc/version\n");
  }
  else {
    fgets (buf, 512, f);
    debug (DEBUG_INFO, "os_init: os info is: \"%s\"\n", buf);
  }
	
  fclose (f);

  // get access to the IO ports
  if (iopl (3) < 0)
    {
      debug (DEBUG_ERROR, "os_init: unable to get IO permissions. make sure you are running as root");
      return -1;
    }
  else
    {
      debug (DEBUG_INFO, "os_init: iopl() successful.");
    }

  return dev_id_array[card_num];

}

/**
 * End access with the board. This should do the opposite of whatever was
 * done is os_init()
 *\return -1 on error
 */
int
os_close (int card_num)
{
  return 0;
}

// The following functions read and write to the IO address space of the card.

/**
 * Write a byte of data to the given card, at given the address.
 * \return -1 on error
 */
int
os_outp (int card_num, unsigned int address, char data)
{
  if (card_num >= num_cards || card_num < 0) {
    debug (DEBUG_ERROR, "os_outp: Card number out of range");
    return -1;
  }
	
  
  // At this place, we have to define all
  // the cards that should be accessed with mmap.
  // PCI Express PulseBlaster (0x887A = 34938)
  if(dev_id_array[card_num] == 34938){  
  
    if(mmap_outb(&pci_resource0path_array[512*card_num], address, data) == -1){
      debug (DEBUG_ERROR, "os_outp: mmap_outb() error");
      return -1;
    }

  }else{
    outb_p (data, base_addr_array[card_num] + address);
    return 0;
  }
}

/**
 * Read a byte of data from the given card, at the given address
 * \return value from IO address
 */

// TODO: This function is supposed to return a value (char), 
// but it can also return an error code (signed int). 
// It's better to return a value in a pointer argument. 
// And the error code using "return".
char
os_inp (int card_num, unsigned int address)
{
  char data;

  if (card_num >= num_cards || card_num < 0) {
      debug (DEBUG_ERROR, "os_inp: Card number out of range");
      return -1;
  }


  // At this place, we have to define all
  // the cards that should be accessed with mmap.
  // PCI Express PulseBlaster (0x887A = 34938)
  if(dev_id_array[card_num] == 34938){

    mmap_inb(&pci_resource0path_array[512*card_num], address, &data);

  }else{
    data = inb_p (base_addr_array[card_num] + address);
  }

  return data;
}

/**
 * Write a 32 bit word to the given card, at the given address
 *\return -1 on error
 */
int
os_outw (int card_num, unsigned int address, unsigned int data)
{
  if (card_num >= num_cards || card_num < 0) {
    debug (DEBUG_ERROR, "os_outw: Card number out of range");
    return -1;
  }

  // At this place, we have to define all
  // the cards that should be accessed with mmap.
  // PCI Express PulseBlaster (0x887A = 34938)
  if(dev_id_array[card_num] == 34938){

    if (mmap_outw(&pci_resource0path_array[512*card_num], address, data) == -1){
      debug (DEBUG_ERROR, "os_outw(): mmap_outw() error");
      return -1;
    }

  }else{
    outl_p (data, base_addr_array[card_num] + address);
  }

  return 0;
}

/**
 * Read a 32 bit word from the given card, at the given address
 *\return value form IO address
 */
unsigned int
os_inw (int card_num, unsigned int address)
{
  if (card_num >= num_cards || card_num < 0) {
    debug (DEBUG_ERROR, "os_inw: Card number out of range");
    return -1;
  }

  // At this place, we have to define all
  // the cards that should be accessed with mmap.
  // PCI Express PulseBlaster (0x887A = 34938)
  if(dev_id_array[card_num] == 34938){

    printf("os_inw, card_num: %d\n", card_num);
    printf("os_inw, &pci_resource0path_array[512*card_num]: %s\n", &pci_resource0path_array[512*card_num]);
    

    int result = 0;
    mmap_inw(&pci_resource0path_array[512*card_num], address, &result);
    return result;

  }else{
    return inl_p (base_addr_array[card_num] + address);
  }

}

int
my_getline (char **lineptr, size_t * n, FILE * stream)
{
  return getline (lineptr, n, stream);
}
