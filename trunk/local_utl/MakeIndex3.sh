#!/bin/sh
for i in /home/bbs/0Announce/groups/GROUP_?/*;do
	if [ -L $i ]; then
		:
	else
		/home/bbs/bin/MakeIndex3 $i
	fi
done
