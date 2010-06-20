set terminal png small
set size 1,0.75
set output "/home/bbs/ftphome/root/boards/bbslists/day.png"
set xdata time
set timefmt "%b %d %Y"
set xrange ["Jul 1 2003":"Jul 30 2004"]
set xtics "Jul 1 2003",2419200,"Jul 30 2004"
set format x "%m/%d"
set timestamp
set key top left Left reverse
set grid
plot "/home/bbs/ftphome/root/boards/bbslists/day.txt" u 3:1 title "max online" with line 1,\
     "/home/bbs/ftphome/root/boards/bbslists/day.txt" u 3:2 title "avg online" with line 8

