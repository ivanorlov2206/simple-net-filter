#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "../api.h"
#define MY_DEVICE "/dev/filter"


void print_usage(char *s) {
	printf("Usage: %s [command] <first address> <last address>\n"
		"commands:\n"
		"\tadd - Add filter rule\n"
		"\tremove - Remove filter rule\n", s);
}


int main(int argc, char **argv) {
	int fd;
	struct ip_range range;

	if (argc != 4) {
		print_usage(argv[0]);
		exit(0);
	}

	range.first_ip = inet_addr(argv[2]);
	range.last_ip = inet_addr(argv[3]);

	if (mknod(MY_DEVICE, 0644 | S_IFCHR, makedev(MMAJOR, 0)) < 0) {
		if (errno != EEXIST) {
			printf("Failed to create device\n");
			exit(0);
		}
	}

	fd = open(MY_DEVICE, O_RDONLY);
	if (fd < 0) {
		printf("Error opening device %s\n", MY_DEVICE);
	} else {
		if (strcmp(argv[1], "add") == 0) {
			if (ioctl(fd, CMD_ADD, &range) < 0)
				printf("Error during adding\n");
		} else if (strcmp(argv[1], "remove") == 0) {
			if (ioctl(fd, CMD_REMOVE, &range) < 0)
				printf("Error during removing\n");
		} else {
			print_usage(argv[0]);
		}
		close(fd);
	}
	unlink(MY_DEVICE);
	return 0;
}

