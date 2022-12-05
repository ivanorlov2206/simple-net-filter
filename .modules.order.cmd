cmd_/home/mineorpe/dev/modules/netfilter/modules.order := {   echo /home/mineorpe/dev/modules/netfilter/filter.ko; :; } | awk '!x[$$0]++' - > /home/mineorpe/dev/modules/netfilter/modules.order
