import matplotlib.pyplot as plt
import csv
import argparse

def parse_args():
	p = argparse.ArgumentParser()
	p.add_argument("-p", dest="path", default="")
	p.add_argument("-f", dest="csvfile", default="")
	return p.parse_args()

def get_time_float(bigstr):
    #print(bigstr)
    l1 = len("2020-04-07")
    l2 = len("14:21:56.300369663")
    newStr = bigstr[l1+1:l1+l2+1]
    #print(newStr)
    time_value = int(newStr[:2]) * 3600 + int(newStr[3:5]) * 60 + float(newStr[6:])
    return time_value

def dump_to_csv(filename, csvfile):
    #filename = "parse_to_be"
    csv_lines = []
    time_list = []
    cov_list = []
    # csvfile = "save_csv.csv"
    with open(filename,'r') as fp:
        rows = fp.readlines()
    with open(csvfile, 'a+') as f1:
        f1.write("time,region,missed-region,cover,func,missed-func,exec-func,lines,missed-lines,cover\n")
        for each in rows:
            if "2021-" in each:
                tmp_time = str(get_time_float(each))
            else:
            
                tmpcov = []
                for validstr in each.split(' '):
                    if validstr and validstr != "TOTAL":
                        tmpcov.append(validstr)
                tmp_cov = ','.join(tmpcov)
                whole_line = tmp_time + "," + tmp_cov
                f1.write(whole_line)

def rebase(x):
    base = x[0]
    for i in range(0, len(x)):
        x[i] = (x[i] - base) 



def main():
    args = parse_args()  
    
    dump_to_csv(args.path, args.csvfile) 

if __name__ == "__main__":
	main()