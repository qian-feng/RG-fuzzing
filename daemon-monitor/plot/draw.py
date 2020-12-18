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
import collections

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-i", dest="max_time", type=int, default=3600)
    p.add_argument("-r", dest="rounds", type=int, default=3)
    #p.add_argument("-q", dest="ijon_queue")
    p.add_argument("-t", dest="targ_name", required=True)
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

    # xnew.append(60)
    # ynew.append(ynew[-1])


    if islog:
        plt.xscale('log')
    plt.plot(xnew, ynew,  label=label, linewidth=2)


def draw1 (args):
    file1 = "RG.log"
    file2 = "VN.log"    

    plt.xlabel("Time(min)", fontsize = "large")
    #plt.ylabel("Cumulative edge coverage", fontsize="large")
    plt.ylabel("edge coverage ratio(%)", fontsize="large")
    
    plt.suptitle('Recipe_Database',fontsize=20, y=0.5)

    plot_csv(file1, "RG", args.max_time)
    plot_csv(file2, "AFL", args.max_time)
    plt.legend(loc='lower right',fontsize='large')

    plt.savefig('RD.png',format='png')

def rebase(timelist, maxtime):
    ele1 = timelist[0]
    for i in range(0, len(timelist)):
        timelist[i] = (timelist[i] - ele1)/60.0
        if timelist[i] >= maxtime/60.0:
            break
    print(i, timelist[i])
    return timelist[0:i]



def main(args):
    # plot the average of 3 run and save the png file
    
    if args.rounds == 1:
        draw1() 
        return

    plt.xlabel("Time(min)", fontsize = "large")
    plt.ylabel("Cumulative edge coverage", fontsize="large")
    #plt.ylabel("edge coverage ratio(%)", fontsize="large")
    
    plt.suptitle(args.targ_name,fontsize=20, y=0.5)
    for conf in ["RG", "AFL"]:
        get_average(args.max_time, os.path.join(args.targ_name, conf+"_1.log"), os.path.join(args.targ_name, conf+"_2.log"), os.path.join(args.targ_name, conf+"_3.log"), conf)
    
    # draw angora here 
    #get_average(args.max_time, "Angora_1.log", "Angora_1.log", "Angora_1.log", "Angora")
    #getijon(args.ijon_queue, args.max_time)   
    plt.legend(loc='upper left',fontsize='large')
    plt.savefig(args.targ_name +'-3run.png',format='png')      

def getijon(args):    
    plt.xlabel("Time(min)", fontsize = "large")
    plt.ylabel("RG-seed count", fontsize="large")
    for i in range(1, 4):
        dir1 = "../workdir_%s/RG_%d/RG/ijon_max"%(args.targ_name, i)
        timelist = []
        for each in os.listdir(dir1):
            newfile = os.path.join(dir1, each)
            newtime = os.path.getmtime(newfile)
            timelist.append(newtime)
        timelist = sorted(timelist)
        timelist = rebase(timelist, args.max_time)
        filecount = range(1, len(timelist)+1)
        # timelist.append(maxtime/60.0)
        # filecount.append(filecount[-1])
        plt.plot(timelist, filecount, "--", label="RG-"+str(i))
    plt.suptitle(args.targ_name,fontsize=20, y=0.5)
    plt.legend(loc='upper left',fontsize='large')
    plt.savefig(args.targ_name +'-ijonqueue.png',format='png')
    plt.clf()

def plothit(args):
    # plot hit as well
    plt.xlabel("Time(min)", fontsize = "large")
    plt.ylabel("annotations hit", fontsize="large")
    for i in range(1, 4):
        hit_dict = {}
        dir1 = "../workdir_%s/RG_%d/RG/ijon_max"%(args.targ_name, i)
        timelist = []
        for each in os.listdir(dir1):
            newfile = os.path.join(dir1, each)
            bindex = os.path.basename(newfile).split("_")[1]
            # print(os.path.basename(newfile).split("_"))
            newtime = os.path.getmtime(newfile)
            print(bindex, newtime)
            if bindex not in hit_dict:
                hit_dict[bindex] = newtime
            elif newtime < hit_dict[bindex]:
                hit_dict[bindex] = newtime
        

        timelist = sorted(list(hit_dict.values()))
        timelist = rebase(timelist, args.max_time)
        numhit = range(1, len(timelist)+1)


        plt.plot(timelist, numhit, "--", label="RG-"+str(i))
    plt.suptitle(args.targ_name,fontsize=20, y=0.5)
    plt.legend(loc='upper left',fontsize='large')
    plt.savefig(args.targ_name +'-annohit.png',format='png')




if __name__ == "__main__":
    args = parse_args()
    #main(args)    
    getijon(args)
    plothit(args)
    