# rm /data/debug.log
# rm /data/cover.log
rm -rf res_dir_vani/VN/
mkdir -p res_dir_vani/VN/queue
python monitor.py -i res_dir_vani/VN/queue -o vanimoni-output -m vanitmp_output -l /data/VN.log -- ./Recipe_Database.cov