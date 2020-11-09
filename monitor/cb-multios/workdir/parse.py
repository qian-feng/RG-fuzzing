import argparse
import os

covered = set()

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-n", dest="targname", default="")
    p.add_argument("-m", dest="maxnum", type=int)
    p.add_argument("-p", dest="minnum", type=int, default=0)
    return p.parse_args()

def parsefile(filename):
    if not os.path.isfile(filename):
        print("%s not exists"%(filename))
        return 
    with open(filename, "r", errors='ignore') as fp:
        lines = fp.readlines()
        for line in lines:
            if line.startswith("[touched] 0x"):
                items = line.split(" ")
                try:
                    num = int(items[1], 16)
                except:
                    continue
                #print(items[1])
                if len(items[1]) > 0 and len(items[1]) <= len("0xf7fcfcbb") and not (":" in items[1]):
                    covered.add(items[1])

def dumptofile(filename):
    #os.unlink(filename)
    with open(filename, "w") as fp:
        for i in covered:
            fp.write(str(i).strip("\n") + "\n")

def loadfromfile(filename):
    with open(filename, "r") as fp:
        lines = fp.readlines()
        for line in lines:
            covered.add(line.strip("\n"))

def main():
    args = parse_args()
    targdir = os.path.join(os.getcwd(), "output/" + args.targname)
    coveredlog = os.path.join(os.getcwd(), args.targname + "-covered.txt")
    oldlen = 0
    if os.path.isfile(coveredlog):
        loadfromfile(coveredlog)
        oldlen = len(covered)
        print("[load]: %d\n"%(oldlen))
    
    for i in range(args.minnum, args.maxnum):
        logfile = os.path.join(targdir, str(i)+".txt")
        #print(logfile)
        parsefile(logfile)
        #print(i, len(covered))
        if oldlen != len(covered):
            print("[%d]: total hit: %d\n" %(i, len(covered)))
            dumptofile(coveredlog)
            oldlen = len(covered)


if __name__ == "__main__":
	main()