#!/usr/bin/env bash

### Start of initialization ###
is_RoCE=1
executable="ccKVS-sc" # choose "ccKVS-sc" or "ccKVS-lin" according to the coherence protocol
export MEMCACHED_IP="192.168.122.14" #Node having memcached for to initialize RDMA QPs connections/handlers

export MLX5_SINGLE_THREADED=1
export MLX5_SCATTER_TO_CQE=1
printenv | grep "MEM"
# Setting up a unique machine id via a list of all ip addresses
machine_id=-1
IPs=(192.168.122.14 192.168.122.103)
localIP=$(ip addr | grep 'state UP' -A2 | sed -n 3p | awk '{print $2}' | cut -f1  -d'/')
export allIPs="$localIP"
for i in "${!IPs[@]}"; do
	if [  "${IPs[i]}" ==  "$localIP" ]; then
		machine_id=$i
	else
    allIPs="${allIPs},${IPs[i]}"
	fi
done
# allIPs = localip,remoteip1,remoteip2, ....

# machine_id = # uncomment this line to manually set the machine id
echo Machine-Id "$machine_id"

### End of initialization ###

# A function to echo in blue color
function blue() {
	es=`tput setaf 4`
	ee=`tput sgr0`
	echo "${es}$1${ee}"
}

blue "Removing SHM keys used by the workers 24 -> 24 + Workers_per_machine (request regions hugepages)"
for i in `seq 0 32`; do
	key=`expr 24 + $i`
	sudo ipcrm -M $key 2>/dev/null
done

# free the  pages workers use
blue "Removing SHM keys used by MICA"
for i in `seq 0 28`; do
	key=`expr 3185 + $i`
	sudo ipcrm -M $key 2>/dev/null
	key=`expr 4185 + $i`
	sudo ipcrm -M $key 2>/dev/null
done

: ${MEMCACHED_IP:?"Need to set MEMCACHED_IP non-empty"}


blue "Removing hugepages"
shm-rm.sh 1>/dev/null 2>/dev/null


blue "Reset server QP registry"
sudo killall memcached
sudo killall ccKVS-sc
#memcached -l 0.0.0.0 1>/dev/null 2>/dev/null &
memcached -l 192.168.122.14 1>/dev/null 2>/dev/null &
sleep 1

blue "Running client and worker threads"
sudo LD_LIBRARY_PATH=/usr/local/lib/ -E \
	./${executable} \
	--machine-id $machine_id \
	--is-roce $is_RoCE \
	2>&1
