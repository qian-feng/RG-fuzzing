dname=${1}
sudo docker run -it --cpus="3" --name="$dname" -v `pwd`:/data fuzzcgc /bin/bash
