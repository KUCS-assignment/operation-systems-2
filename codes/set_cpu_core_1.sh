cd /sys/fs/cgroup/cpuset
mkdir mycpu; cd mycpu
echo 0 > cpuset.cpus
echo 0 > cpuset.mems
echo $$ > tasks
cat tasks
