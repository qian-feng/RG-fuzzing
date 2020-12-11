prog=${1}
sudo docker run -it --cpus="3" --name="$prog" -v `pwd`:/data fuzzcgc /bin/bash
