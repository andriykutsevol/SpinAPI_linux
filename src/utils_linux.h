int pci_get_resource0(int dev_id, char* resource0_path);

int get_mmap_virt_addr(const char *resource0_path, 
                        int address, 
                        void *virt_addr, 
                        void *map_base, 
                        const int map_size);

int mmap_inw(const char *resource0_path, int address, int *fw_result);

int mmap_outw(const char *resource0_path, int address, unsigned int data);

