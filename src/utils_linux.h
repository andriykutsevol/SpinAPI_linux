int pci_get_resource0(int dev_id, char* resource0_path, char* pci_resource0path_array, int devices_found);

// ToDo: to make all arguments const?
int is_pcie_device_found(char *result, char* pci_resource0path_array, int devices_found);


int get_mmap_virt_addr(const char *resource0_path, 
                        int address, 
                        void **virt_addr, 
                        void **map_base, 
                        const int map_size);

int mmap_inw(const char *resource0_path, int address, int *result);

int mmap_outw(const char *resource0_path, int address, unsigned int data);

int mmap_inb(const char *resource0_path, int address, char *result);

int mmap_outb(const char *resource0_path, int address, char data);
