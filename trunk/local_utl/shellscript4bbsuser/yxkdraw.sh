#!/bin/bash
#script auto plot and ftp
#下面三行定义了时间
year=`date +%Y`
mon=`date +%b`
day=`date +%d`
#获取stat.txt文件，上次cgi-bin/www改成了cgi-bin/bbs结果出错了……
#将每天上线总人数的最大值输入到day.txt文件
echo -n `sort -k2 -n -r /home/bbs/ftphome/root/boards/bbslists/stat.txt|sed -n 2p|cut -f2` >>/home/bbs/ftphome/root/boards/bbslists/day.txt
#将每天上线总人数的平均值输入到day.txt，avg.pl内容见后
sort -k2 -n -r /home/bbs/ftphome/root/boards/bbslists/stat.txt|sed -n 2,241p|cut -f2|/home/bbs/bin/avg.pl>>/home/bbs/ftphome/root/boards/bbslists/day.txt
#将当天日期输入day.txt
echo $mon" "$day" "$year>>/home/bbs/ftphome/root/boards/bbslists/day.txt
#将当天stat.txt保存到old目录
cp /home/bbs/ftphome/root/boards/bbslists/stat.txt /home/bbs/statbak/$year"_"$mon"_"$day.txt
#画图
gnuplot</home/bbs/etc/day.plt
