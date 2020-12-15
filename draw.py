import argparse
import os
import sys
import subprocess
import glob
import time
import matplotlib.pyplot as plt
import csv
import numpy as np
from scipy.interpolate import interp1d
from statistics import mean 

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-i", dest="max_time", type=int, default=3600)
    p.add_argument("-r", dest="rounds", type=int, default=3)
    return p.parse_args()

def tomin(time_list):
    for i in range(0, len(time_list)):
        time_list[i] = time_list[i] / 60.0
    return time_list

def getxy(csvfile, MAX_TIME): # carry the smallest MAX_TIME so the plot is correct
    time_list = []
    cover_list =[]
    ratio_list = []
    max_time = MAX_TIME
    with open(csvfile, "r") as fd:
        rows = csv.reader(fd, delimiter=',')
        next(rows)
        for row in rows:
            if float(row[0]) > MAX_TIME: # only aquire the first 1h result
                break
            time_list.append(float(row[0]))
            cover_list.append(int(row[1]))
            ratio_list.append(float(row[2].strip('%')))    
    return time_list, cover_list, time_list[-1]

def plot_csv(csvfile, label, MAX_TIME):
    time_list, cover_list, max_time = getxy(csvfile, MAX_TIME)    
    time_list = tomin(time_list)
    plt.plot(time_list, cover_list, marker=".", label=label, linewidth=2)

def get_average(maxtime, f1, f2, f3, label, islog=0): # input: the 3 csv files
    x1, y1, maxtime = getxy(f1, maxtime)
    x2, y2, maxtime = getxy(f2, maxtime)
    x3, y3, maxtime = getxy(f3, maxtime)

    xnew = np.linspace(0, maxtime, 10000)

    f1 = interp1d(x1, y1, fill_value="extrapolate")
    f2 = interp1d(x2, y2, fill_value="extrapolate")
    f3 = interp1d(x3, y3, fill_value="extrapolate")

    ynew = [mean(k) for k in zip(f1(xnew), f2(xnew), f3(xnew))]
    xnew = tomin(xnew)

    if islog:
        plt.xscale('log')
    plt.plot(xnew, ynew, label=label)


def draw1 ():
    file1 = "RG.log"
    file2 = "VN.log"

    args = parse_args()

    plt.xlabel("Time(min)", fontsize = "large")
    plt.ylabel("Cumulative edge coverage", fontsize="large")
    
    plt.suptitle('Recipe_Database',fontsize=20, y=0.5)

    plot_csv(file1, "RG", args.max_time)
    plot_csv(file2, "AFL", args.max_time)
    plt.legend(loc='lower right',fontsize='large')

    plt.savefig('RD.png',format='png')

def main():
    # plot the average of 3 run and save the png file
    args = parse_args()
    if args.rounds == 1:
        draw1() 
        return

    plt.xlabel("Time(min)", fontsize = "large")
    plt.ylabel("Cumulative edge coverage", fontsize="large")
    
    plt.suptitle('Recipe_Database',fontsize=20, y=0.5)

    for conf in ["RG", "AFL"]:
        get_average(args.max_time, conf+"_1.log", conf+"_2.log", conf+"_3.log", conf)
    
    plt.legend(loc='lower right',fontsize='large')
    plt.savefig('RD-3run.png',format='png')      


if __name__ == "__main__":
	main()