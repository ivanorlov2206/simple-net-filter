#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include "iptools.h"
#include <linux/module.h>
#include <linux/slab.h>


unsigned int parse_ipv4_string(char *ip) {
	int components[4];
	unsigned int conv_res, res;
	char *found;
	char *temp_s;
	long temp_l;
	int iter = 0;
	
	temp_s = kmalloc(strlen(ip), GFP_KERNEL);
	strncpy(temp_s, ip, strlen(ip));

	while ((found = strsep(&temp_s, ".")) != NULL && iter < 4) {
		conv_res = kstrtol(found, 10, &temp_l);
		if (conv_res != 0) {
			return 0x0;
		}
		components[iter++] = temp_l;
	}

	res = components[3] << 24;
	res |= (components[2] << 16);
	res |= (components[1] << 8);
	res |= (components[0]);
	return res;
}
