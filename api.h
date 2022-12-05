#include <asm/ioctl.h>

#define CMD_ADD _IOW('k', 1, unsigned int)
#define CMD_REMOVE _IOW('k', 2, unsigned int)

#define MMAJOR 42

struct ip_range {
	int first_ip;
	int last_ip;
};
