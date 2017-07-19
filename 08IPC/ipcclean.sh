#!/bin/bash

#check parameters
if [ "$#" != "2" ]; then
	echo "usage: $0 user shm|sem|msq|all"
	exit 1
elif [ "$2" != "shm" -a "$2" != "sem" -a "$2" != "msq" -a "$2" != "all" ];then
	echo "usage: $0 user shm|sem|msq|all"
	exit 1
fi

#delete the shm
if [ "$2" = "shm" -o "$2" = "all" ];then
	START=`ipcs | sed -n '/shmid/='`
	echo ${START}
	END=`ipcs | sed -n '/semid/='`
	echo ${END}
	for i in `ipcs | sed -n ${START},${END}p | cut -d '' -f 2`
	do
		ipcrm -m $i
		echo -e $i
	done
fi

#delete the sem 
if [ "$2" = "sem" -o "$2" = "all" ];then
	START=`ipcs | sed -n '/semid/='`
	echo ${START}
	END=`ipcs | sed -n '/msqid/='`
	echo ${END}
	for i in `ipcs | sed -n ${START},${END}p | cut -d '' -f 2`
	do
		ipcrm -s $i
		echo -e $i
	done
fi

# delete the msq
if [ "$2" = "msq" -o "$2" = "all" ];then
	START=`ipcs | sed -n '/msqid/='`
	echo ${START}
	for i in `ipcs | sed -n ${START},$$p| grep $1 | cut -d '' -f 2`
	do
		ipcrm -q $i
		echo -e $i
	done
fi
