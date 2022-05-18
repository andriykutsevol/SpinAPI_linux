#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define __int64 int64_t

struct padded_flag {
    __int64 flags;
    uint8_t padding[8];
} __attribute__((__packed__));

static __int64 make_flags() {
    return 0x0123456789abcdef;
}

static void make_pflags_old(__int64 flags, int pflags[3]) {
    struct padded_flag padded;
    memset(&padded, 0, sizeof(padded));
    padded.flags = flags;
    memcpy(pflags, &padded, sizeof(int) * 3);
}

static void pad_flags(__int64 flags, int flags_padded[3])
{
  flags_padded[2] = 0;
  flags_padded[1] = (flags >> 32) & 0xFFFFFFFF;
  flags_padded[0] = flags & 0xFFFFFFFF;
}

static void make_pflags_new(__int64 flags, int pflags[3]) {
    pad_flags(flags, pflags);
}

static void inst(int *pflags, int *instruction) {
    int length = 0;
    int inst = 0;
    int inst_data_direct = 0;
    instruction[0] = length;
    instruction[1] = (0xF & inst) | ((0xFFFFF & inst_data_direct) << 4) | ((0xFF & pflags[0]) <<24);
    instruction[2] = ((0xFFFFFF & (pflags[0] >> 8)) << 0) | ((pflags[1] & 0xFF) << 24);
    instruction[3] = ((0xFFFFFF & (pflags[1] >> 8)) << 0) | ((pflags[2] & 0xFF) << 24);
    {
        size_t i;
        for (i = 0; i < 4; i++) {
            printf("instruction[%zu]=%#08x\n", i, instruction[i]);
        }
    }
}

static void print_pflags(int *pflags, const char *const msg) {
    size_t i;
    printf("%s:\n", msg);
    for (i = 0; i < 3; i++) {
        printf("  %08x\n", pflags[i]);
    }
}

int main(void) {
    int pflags_old[3], pflags_new[3];

    __int64 flags = make_flags();

    make_pflags_old(flags, pflags_old);

    make_pflags_new(flags, pflags_new);

    printf("Testing pflag values directly...");
    if (0 == memcmp(pflags_old, pflags_new, sizeof(pflags_old))) {
        printf("SUCCESS\n");
    } else {
        print_pflags(pflags_old, "old");
        print_pflags(pflags_new, "new");
        printf("FAIL\n");
    }
    return 0;
}
