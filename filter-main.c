#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <linux/fs.h>
#include <linux/cdev.h>


#include "iptools.h"
#include "api.h"

#define DEVICE "filter"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Orlov");

typedef struct {
	struct ip_range range;
	struct list_head list;
} internal_ip_range;


static struct cdev chr_dev;


LIST_HEAD(ranges);

static unsigned int hookfn(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph = ip_hdr(skb);
	
	int accept = 0;
	struct list_head *pos;
	internal_ip_range *range;
	
	list_for_each(pos, &ranges) {
		range = list_entry(pos, internal_ip_range, list);
		if (iph->saddr <= (range->range).last_ip && iph->saddr >= (range->range).first_ip) {
			accept = 1;
			break;
		}
	}
	if (accept) {
		printk(KERN_INFO "Message from %pI4 to %pI4 allowed\n", &iph->saddr, &iph->daddr);
		return NF_ACCEPT;
	} else {
		printk(KERN_INFO "Message from %pI4 to %pI4 dropped\n", &iph->saddr, &iph->daddr);
		return NF_DROP;
	}
}

static int open(struct inode *inode, struct file *file) {
	return 0;
}

static int close(struct inode *inode, struct file *file) {
	return 0;
}

static void remove_range(struct ip_range ip_range) {
	struct list_head *pos, *temp;
	internal_ip_range *range;
	
	list_for_each_safe(pos, temp, &ranges) {
		range = list_entry(pos, internal_ip_range, list);
		if (range->range.first_ip == ip_range.first_ip && range->range.last_ip == ip_range.last_ip) {
			list_del(&range->list);
			kfree(range);
			printk(KERN_INFO "Rule removed! From %pI4 to %pI4\n", &ip_range.first_ip, &ip_range.last_ip);
		}
	}
}

static long c_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	struct ip_range ip_range;
	internal_ip_range *internal_range;
	
	if (copy_from_user(&ip_range, (struct ip_range *) arg, sizeof(struct ip_range)) ) {
		printk(KERN_INFO "Failed to copy from user space\n");
		return -EFAULT;
	}
	switch (cmd) {
		case CMD_ADD:
			internal_range = kmalloc(sizeof(internal_ip_range), GFP_KERNEL);
			internal_range->range = ip_range;
			list_add(&internal_range->list, &ranges);
			printk(KERN_INFO "New rule added! From %pI4 to %pI4\n", &ip_range.first_ip, &ip_range.last_ip);
			break;
		case CMD_REMOVE:
			remove_range(ip_range);
			break;
	}
	return 0;
}

static struct nf_hook_ops nfho = {
	.hook = hookfn,
	.hooknum = NF_INET_PRE_ROUTING,
	.pf = PF_INET,
	.priority = NF_IP_PRI_FIRST
};

const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open,
	.release = close,
	.unlocked_ioctl = c_ioctl
};


int __init init(void)
{
	int err;
	/*internal_ip_range range;
	struct ip_range ir = {
		.first_ip = 0,
		.last_ip = 0xFFFFFFFF
	};
	range.range = ir;*/
	err = register_chrdev_region(MKDEV(MMAJOR, 0), 1, DEVICE);
	if (err)
		return err;
	
	cdev_init(&chr_dev, &fops);
	cdev_add(&chr_dev, MKDEV(MMAJOR, 0), 1);
	
	
	
	err = nf_register_net_hook(&init_net, &nfho);
	if (err)
		goto out;
	printk(KERN_INFO "Filter module loaded!\n");
	return 0;
	
out:
	cdev_del(&chr_dev);
	unregister_chrdev_region(MKDEV(MMAJOR, 0), 1);
	return err;
}


void __exit cleanup(void)
{
	
	nf_unregister_net_hook(&init_net, &nfho);
	cdev_del(&chr_dev);
	unregister_chrdev_region(MKDEV(MMAJOR, 0), 1);
}


module_init(init);
module_exit(cleanup);
