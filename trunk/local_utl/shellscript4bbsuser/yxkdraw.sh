#!/bin/bash
#script auto plot and ftp
#�������ж�����ʱ��
year=`date +%Y`
mon=`date +%b`
day=`date +%d`
#��ȡstat.txt�ļ����ϴ�cgi-bin/www�ĳ���cgi-bin/bbs��������ˡ���
#��ÿ�����������������ֵ���뵽day.txt�ļ�
echo -n `sort -k2 -n -r /home/bbs/ftphome/root/boards/bbslists/stat.txt|sed -n 2p|cut -f2` >>/home/bbs/ftphome/root/boards/bbslists/day.txt
#��ÿ��������������ƽ��ֵ���뵽day.txt��avg.pl���ݼ���
sort -k2 -n -r /home/bbs/ftphome/root/boards/bbslists/stat.txt|sed -n 2,241p|cut -f2|/home/bbs/bin/avg.pl>>/home/bbs/ftphome/root/boards/bbslists/day.txt
#��������������day.txt
echo $mon" "$day" "$year>>/home/bbs/ftphome/root/boards/bbslists/day.txt
#������stat.txt���浽oldĿ¼
cp /home/bbs/ftphome/root/boards/bbslists/stat.txt /home/bbs/statbak/$year"_"$mon"_"$day.txt
#��ͼ
gnuplot</home/bbs/etc/day.plt
