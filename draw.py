import argparse
import os
import sys
import subprocess
import glob
import time
import matplotlib.pyplot as plt
import csv

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-i", dest="max_time", type=int, default=3600)
    return p.parse_args()


def plot_csv(csvfile, label, MAX_TIME):
    time_list = []
    cover_list =[]
    ratio_list = []
    with open(csvfile, "r") as fd:
        rows = csv.reader(fd, delimiter=',')
        next(rows)

        for row in rows:
            if float(row[0]) > MAX_TIME: # only aquire the first 1h result 
                # time_list.append(MAX_TIME)
                # cover_list.append(cover_list[-1])
                #plt.text(time_list[-1], cover_list[-1], "%f,%f"%(time_list[-1], cover_list[-1]))
                break
            time_list.append(float(row[0]))
            cover_list.append(int(row[1]))
            ratio_list.append(float(row[2].strip('%')))
    
    plt.plot(time_list, cover_list, marker=".", label=label, linewidth=2)


def main ():
    file1 = "RG.log"
    file2 = "VN.log"

    args = parse_args()

    plt.xlabel("Time(s)", fontsize = "large")
    plt.ylabel("Cumulative edge coverage", fontsize="large")
    
    plt.suptitle('Recipe_Database',fontsize=20, y=0.5)

    plot_csv(file1, "RG", args.max_time)
    plot_csv(file2, "AFL", args.max_time)
    plt.legend(loc='lower right',fontsize='large')

    plt.savefig('RD.png',format='png')

if __name__ == "__main__":
	main()