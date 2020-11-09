# ./rundoc.sh Barcoder 
prog=${1}
sudo docker exec $prog bash -c "cd cb-multios/workdir && ./runall.sh ${prog}"
