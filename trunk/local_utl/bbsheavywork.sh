#!/bin/sh
#this script will do some heavy work such as backup on night
if [ a$DELAYT = a ];then
	export DELAYT=400
fi
LDELAYT=`expr $DELAYT / 10`
nowday=`/bin/date +%j`
nowweek=`/bin/date +%w`
nowmday=`/bin/date +%e`
BBSHOME=/home/bbs

cd $BBSHOME

$BBSHOME/bin/nbstat s 1
date
sleep $LDELAYT
#$BBSHOME/bin/auto_deluser
date
sleep $LDELAYT
#$BBSHOME/bin/auto_kickout
date
sleep $LDELAYT
$BBSHOME/bin/bm7day > $BBSHOME/0Announce/groups/GROUP_0/BM_Club/7days
date
sleep $LDELAYT
$BBSHOME/bin/delvote.sh
date
sleep $LDELAYT
$BBSHOME/bin/auto_rm_junk >$BBSHOME/main/junklog
date
sleep $DELAYT
$BBSHOME/bin/clear_junk
date
#sleep $DELAYT
#$BBSHOME/bin/reminder
#date
sleep $LDELAYT
$BBSHOME/bin/MakeIndex3.sh
date
sleep $DELAYT

if [ $nowweek -eq 1 ];then
	$BBSHOME/bin/nbstat amu 7
#	$BBSHOME/bin/cvslog.sh &> /dev/null
	$BBSHOME/bin/delvote.sh
	date
	sleep $LDELAYT
	killall -TTOU bbs
	date
	sleep $LDELAYT
fi

if [ `expr $nowday % 3` -eq 0 ];then
	$BBSHOME/bin/bm |sort -f > $BBSHOME/0Announce/groups/GROUP_0/BM_Club/boardlist
	$BBSHOME/bin/bm2|$BBSHOME/bin/id_boards s|sort -f > $BBSHOME/0Announce/groups/GROUP_0/BM_Club/bmlist
	date
	sleep $DELAYT
fi

#if [ `expr $nowmday % 15` -eq 2 ];then
#	$BBSHOME/bin/anall &> /root/bbsbak/bbslog/xlog
#	date
#	sleep $DELAYT
#fi

if [ `expr $nowday % 3` -eq 1 ];then
	$BBSHOME/bin/find_rm_lost >$BBSHOME/main/lostlog
	date
	sleep $DELAYT
fi

if [ `expr $nowday % 9` -eq 2 ];then
	$BBSHOME/bin/find_lost_mail >$BBSHOME/main/lostmaillog
	date
	sleep $DELAYT
fi

if [ `expr $nowday % 7` -eq 0 ]; then
	$BBSHOME/bin/brcBoards 2>/dev/null | $BBSHOME/bin/boardRelate
	date
	sleep $DELAYT
fi

