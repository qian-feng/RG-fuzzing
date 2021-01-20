import csv
import argparse
import os
import matplotlib.pyplot as plt
# import statistics
from statistics import mean 
import math

from scipy.interpolate import interp1d
import numpy as np


def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-n", dest="pname", default="")
    #p.add_argument("-m", dest="rounds", type=int, default=6)
    return p.parse_args()

def get_time_float(bigstr):
    #print(bigstr)
    l1 = len("2020-04-07")
    l2 = len("14:21:56.300369663")
    newStr = bigstr[l1+1:l1+l2+1]
    #print(newStr)
    time_value = int(newStr[:2]) * 3600 + int(newStr[3:5]) * 60 + float(newStr[6:])
    return time_value

def rebase(x):
    base = x[0]
    for i in range(0, len(x)):
        x[i] = (x[i] - base)
        if x[i] < 0:
            x[i] += 24*3600
    return x


def plot_from_csv(csvfile, label, islog=0):
    #csvfile="save_csv.csv"
    time_list = []
    lineperc_list = []
    linecov_list = []
    testcase_id = []
    init = 0
    last = 0
    if not os.path.isfile(csvfile):
        return
    with open(csvfile, 'r') as fd:
        rows = csv.reader(fd, delimiter=',')
        next(rows)
        try:
            for row in rows:
                if len(row) > 2:
                    continue
                #print(row[0], row[-1])
                if (init == 0):
                    init = float(row[0])
                # if (float(row[0])-init > 7200):
                #     break
                #last = float(row[0])
                time_list.append(float(row[0]))
                testcase_id.append(len(time_list))
                #linecov_list.append(int(row[-3]) - int(row[-2]))
                lineperc_list.append(float(row[-1].strip('%')))
        except ValueError as e:
            print ("error", e, row)

    time_list = rebase(time_list)
    if islog:
        plt.xscale('log')
    print(csvfile, len(time_list), len(lineperc_list))
    plt.plot(time_list, lineperc_list, label=label, linewidth=2)

def getxy(csvfile):
    time_list = []
    lineperc_list = []
    linecov_list = []
    testcase_id = []
    init = 0
    last = 0
    if not os.path.isfile(csvfile):
        return
    with open(csvfile, 'r') as fd:
        rows = csv.reader(fd, delimiter=',')
        next(rows)
        try:
            for row in rows:
                if len(row) > 2:
                    continue
                #print(row[0], row[-1])
                if (init == 0):
                    init = float(row[0])
                # if (float(row[0])-init > 7200):
                #     break
                #last = float(row[0])
                time_list.append(float(row[0]))
                testcase_id.append(len(time_list))
                #linecov_list.append(int(row[-3]) - int(row[-2]))
                lineperc_list.append(float(row[-1].strip('%')))
        except ValueError as e:
            print ("error", e, row)

    time_list = rebase(time_list)
    return time_list, lineperc_list

def get_average(f1, f2, f3, label, confn, islog=0): # input: the 3 csv files
    print(f1, f2, f3)
    x1, y1 = getxy(f1)
    x2, y2 = getxy(f2)
    x3, y3 = getxy(f3)    

    xnew = np.linspace(0, 24*3600, 10000)

    f1 = interp1d(x1, y1, fill_value="extrapolate")
    f2 = interp1d(x2, y2, fill_value="extrapolate")
    f3 = interp1d(x3, y3, fill_value="extrapolate")

    ynew = [mean(k) for k in zip(f1(xnew), f2(xnew), f3(xnew))]

    if islog:
        plt.xscale('log')

    for i in range(0, len(xnew)):
        xnew[i] = xnew[i] / 60.0 # in min
    plt.plot(xnew, ynew, label=label+confn)

    

def plotprog(label, maxc=6):
    # normal pic
    for i in range(2, maxc+1):
        get_average(label+str(i)+"_1.csv", label+str(i)+"_2.csv", label+str(i)+"_3.csv", label, "-conf"+str(i))

    plt.legend(loc='lower right',fontsize='large')
    plt.suptitle(label, fontsize=20, y=0.5)
    plt.savefig(os.getcwd() + '/' + label + '.png', format='png')

    plt.clf()
    # log scale
    for i in range(2, maxc+1):
        get_average(label+str(i)+"_1.csv", label+str(i)+"_2.csv", label+str(i)+"_3.csv", label, "-conf"+str(i), 1)

    #plt.legend(loc='upper left',fontsize='large')
    plt.legend(loc='lower right',fontsize='large')
    plt.suptitle(label, fontsize=20, y=0.5)
    plt.savefig(os.getcwd() + '/log' + label + '.png', format='png')

def plot(label):
    # Recipe_Database_AFL_1.csv
    for conf in ["RG", "AFL"]:
        get_average(label+"_"+conf+"_1.csv", label+"_"+conf+"_2.csv", label+"_"+conf+"_3.csv", label, "-"+conf)

    plt.legend(loc='lower right',fontsize='large')
    plt.xlabel('time(min)')
    plt.ylabel('general edge cov')
    plt.suptitle(label, fontsize=20, y=0.5)
    plt.savefig(os.getcwd() + '/z' + label + '.png', format='png')
    plt.clf()

def main():    
    # args = parse_args()
    # plot(args.pname)
    for prog in ["Recipe_Database", "Childs_Game", "Board_Game"]:
        plot(prog)

if __name__ == "__main__":
	main()