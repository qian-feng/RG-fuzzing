import os
import sys
import fcntl
import sysv_ipc
import argparse
import time 

BRC_SHM_ENV_VAR = "__BRC_SHM_ID"
path= "/tmp/fifopipe"

TOUCHED = 1 
TRAPPED = 2 

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-l", dest="logfile", required=True)
    return p.parse_args()

def main():
    args = parse_args()

    # refresh the plotfile
    start_time = time.time()
    if os.path.isfile(args.logfile):
        os.unlink(args.logfile)
    with open(args.logfile, "a+") as f1:        
        f1.write("time,touched,trapped\n")

    fifo=open(path,'r')
    str1=fifo.read()

    print("acquire the shm id:", str1)
    fifo.close()    

    # inspect the status of all bits 
    mem = sysv_ipc.attach(int(str1))

    last = [0,0,0]
    stat = [0,0,0]

    while(1):
    #if(1):
        stat = [0,0,0]
        cnt = 0
        for each in mem.read(): 
            
            #print("[log]:" +str(cnt) + "," + str(ord(each)))
            tmp = ord(each)
            #print("log: " , tmp , "-")
            #for i in [0,1,2]:
            if tmp == 0:
                stat[0] += 1
            elif tmp == 49:
                stat[1] += 1
            elif tmp == 50:
                stat[2] += 1
            else:
                print("[Error]: ", tmp, stat, last)
                # if tmp == i:
                #     stat[i] += 1
            # if tmp not in [0,1,2]:
            #     print("[Error]: ", tmp, stat, last)
            cnt += 1 
        if last != stat:
            # there's new prog
            #print("old: %s, new: %s\n" % (str(last), str(stat)))
            print("%.2f\t %s\n" % (time.time()-start_time, str(stat)))
            last = stat
            with open(args.logfile, "a+") as fp:
                fp.write("%.2f,%d,%d\n" % (time.time()-start_time,stat[TOUCHED],stat[TRAPPED]))
            
if __name__ == "__main__":
	main()