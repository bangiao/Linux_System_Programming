#/bin/bash
# time.log
if [ ! -w "time.log" ];then
	touch "time.log"
fi
if [ ! -w log ];then
	mkdir log
fi
i=0
while (( i>=0 ));do
	big=`du -m ./time.log | awk '{ printf $1 }'`
	echo "the file is ${big}M"
	if (( big>=2 ));then
		((i++))
		cp ./time.log ./log/time${i}.bak
		cat /dev/null > ./time.log
	fi
	echo "sleep 1s"
	sleep 1s;
done








