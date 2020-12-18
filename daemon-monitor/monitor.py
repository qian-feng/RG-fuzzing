# the daemon process for updating the current progress of fuzzing
# keeping track of the untouched edges by untouched[i]
#   0: untouched conditional brc
#   1: now touched
#   2: not a conditional brc

#import concurrent.futures
import argparse
import os
import sys
import subprocess
import sysv_ipc
import glob
import time

# status value
UNTOUCH = 0 # initial active
TOUCHED = 1 # inactive
NOTCOND = 2 # default

# fix the key and size, seems like the env set from python is not visible to whole system
KEY=1000
SIZE=65536

# untouched brc dictionary, addr as key, [index, status] as value
untouch_dict = {}

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-i", dest="afl_queue", required=True)
    #p.add_argument("-o", dest="output_dir", required=True)
    p.add_argument("-m", dest="tmp_outdir", required=True)
    p.add_argument("-l", dest="logfile", required=True)
    p.add_argument("cmd", nargs="+", help="cmdline")
    return p.parse_args()

def GetInstrumentedPCs(binary):
  # This looks scary, but all it does is extract all offsets where we call:
  # - __sanitizer_cov() or __sanitizer_cov_with_check(),
  # - with call or callq,
  # - directly or via PLT.
  cmd = "objdump -d %s | " \
        "grep '^\s\+[0-9a-f]\+:.*\scall\(q\|\)\s\+[0-9a-f]\+ <__sanitizer_cov\(_with_check\|\|_trace_pc_guard\)\(@plt\|\)>' | " \
        "grep '^\s\+[0-9a-f]\+' -o" % binary
  proc = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          shell=True)
  proc.stdin.close()
  # The PCs we get from objdump are off by 4 bytes, as they point to the
  # beginning of the callq instruction. Empirically this is true on x86 and
  # x86_64.
  return set(int(line.strip(), 16) + 4 for line in proc.stdout)

# create & initialize shared memory to inform fuzzer about branch status
def create_shm(size):
    try: # overwrite if necessary
        mem = sysv_ipc.SharedMemory(KEY)
        mem.detach()
        mem.remove()
    except sysv_ipc.ExistentialError:
        print("No stale SharedMemory.")

    mem = sysv_ipc.SharedMemory(KEY, sysv_ipc.IPC_CREX, size=SIZE)
    if mem.id < 0:
        print("Error creating shm: errno is %d\n" % (mem.id))
        exit(1)
    mem.write("0"*size)
    mem.write("2"*(SIZE-size), size)
    # os.environ["AIF_SIZE"] = str(size)
    # os.environ["AIF_KEY"] = str(mem.key) 
    # os.system("export AIF_SIZE=%s"%str(size))
    # os.system("export AIF_KEY=%s"%str(mem.key))
    return mem

# Task: parse addr and call addr2line 
# Output form: txt file with each line be:
#     <hex_addr>, <line_info>, <brc index>
# def print_untouched(binary, joblist, filename):
def print_untouched(joblist):
    brc_id = 0
    for each in joblist:
        untouch_dict[each] = [brc_id, UNTOUCH, ""]
        brc_id = brc_id + 1
    assert len(joblist) == len(untouch_dict)
    # with open(filename, "w+") as fp:
    #     for each in joblist:
    #         cmd = "addr2line -e %s 0x%x" % (binary, each)
    #         proc = subprocess.Popen(cmd.split(),  stdout = subprocess.PIPE, stderr = subprocess.PIPE)
    #         stdout, stderr = proc.communicate()
    #         # write to file
    #         fp.write("0x%x,%s,%d\n" % (each, stdout.strip("\n"), brc_id))
    #         # write to dict
    #         untouch_dict[each] = [brc_id, UNTOUCH, stdout.strip("\n")]
    #         brc_id = brc_id + 1
    # assert len(joblist) == len(untouch_dict)

def dump_covered():
    filename = "/data/cover.log"
    # my debug func, to dump the newly covered edge:
    if os.path.isfile(filename):
        os.unlink(filename)
    for i in untouch_dict:
        if untouch_dict[i][1] == TOUCHED:
            with open(filename, "a+") as fp:
                fp.write("covered: 0x%x, index: %d, %s\n" % (i, untouch_dict[i][0], untouch_dict[i][2]))

def main():   
    args = parse_args()

    # initialization stage:
    instrumented = GetInstrumentedPCs(args.cmd[0])
    shm = create_shm(len(instrumented))
    # print_untouched(args.cmd[0], instrumented, os.path.join(args.output_dir, args.cmd[0]+".txt")) 
    print_untouched(instrumented) 
    print("[+] Initialization done!\n")
    # second stage of initialization:
    # TODO: load from TIS and update for status=2
    
    # spin for new seed and update shm accordingly
    q_index = 0
    covered_num = 0
    last_cov = 0
    total_num = len(instrumented)
    start_time = 0
    while True:
        newseed = glob.glob(os.path.join(args.afl_queue, "id:%06d*"%q_index))
        if not len(newseed):
            continue
        
        if not start_time:
            start_time = time.time()
            if os.path.isfile(args.logfile):
                os.unlink(args.logfile) # refresh the plotfile
            with open(args.logfile, "a+") as f1:        
                f1.write("time,covered,cov-ratio\n")

        newseed = newseed[0]
        print(newseed)

        parse_file = "%s/%d.txt"%(args.tmp_outdir, q_index)
        cmd = "LD_LIBRARY_PATH=%s %s < %s 1>%s 2>/dev/null" % (os.getcwd(), args.cmd[0], newseed, parse_file)
        #print(cmd)
        os.system(cmd)
        
        with open(parse_file, "r") as fp:
            lines = fp.readlines()
            for line in lines:
                #print(line)
                if line.startswith("[touched] 0x"):
                    items = line.split(" ")
                    try:
                        num = int(items[1], 16)                        
                    except:
                        continue
                    # print(num, num in untouch_dict)
                    if num in untouch_dict:
                        if untouch_dict[num][1] != TOUCHED:
                            covered_num = covered_num + 1
                            offset = untouch_dict[num][0]
                            oldstatus = untouch_dict[num][1]
                            untouch_dict[num][1] = TOUCHED
                            #print("write to %d from %d to %d\n" %(offset, oldstatus, TOUCHED))
                            shm.write(str(TOUCHED), offset)
        q_index = q_index + 1
        
        
        if last_cov != covered_num:
            print("[coverage]: %d total, %d covered, %.2f%% coverage ratio, by %dth seed\n" % (total_num, covered_num, 100.0 * covered_num / total_num, q_index))
            # for plotting
            with open(args.logfile, "a+") as f1:        
                #f1.write("[coverage]: %d total, %d covered, %.2f%% coverage ratio, by %dth seed\n" % (total_num, covered_num, 100.0 * covered_num / total_num, q_index))
                f1.write("%.2f,%d,%.2f%%\n"%(time.time()-start_time,covered_num,100.0 * covered_num / total_num))
            last_cov = covered_num
            dump_covered()

if __name__ == "__main__":
	main()

