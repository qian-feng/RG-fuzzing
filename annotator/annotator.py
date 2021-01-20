import argparse
import os
import sys
import glob
import json
import subprocess
import fileinput

def parse_args():
    p = argparse.ArgumentParser("")
    p.add_argument("-i", dest="rule_file", required=True)
    p.add_argument("-t", dest="targ_dir", required=True)
    return p.parse_args()

def getall(filename):
    thenline = []
    elseline = []
    with open(filename) as fp:
        for i, line in enumerate(fp):
            if 'if' in line:
                thenline.append(i+1)
            if 'switch' in line:
                thenline.append(i+1)
            if 'else' in line:
                elseline.append(i+1)
    return thenline, elseline

def intersection(l1, l2):
    return list(set(l1) & set (l2))

def printdict(dict):
    for each in dict:
        print(each, dict[each])

def parse_dict(thenline, elseline):
    dict = {}
    elseifline = intersection(thenline, elseline)
    for i in range(0, len(thenline)):
        if thenline[i] not in elseifline:
            dict[thenline[i]] = thenline[i]
        else:
            dict[thenline[i]] = dict[thenline[i-1]]
    for i in range(0, len(elseline)):
        if elseline[i] not in elseifline:
            dict[elseline[i]] = elseline[i]
    #printdict(dict)
    return dict

def annotate(filename, linenum, linetext):

    multiline = linetext.split("\n")
    for i in range(0, len(multiline)): # insert each annotation to a newline
        cmd = "sed -i '%di %s' %s" % (linenum+i, multiline[i], filename)
        #print(cmd)
        proc = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, shell=True)
        proc.stdin.close()

def main():
    args = parse_args()
    
    with open(args.rule_file) as fp:
        rule_dict = json.load(fp)  
    
    for targ_file in rule_dict:        
        file_to_write = os.path.join(args.targ_dir, targ_file)

        thenline, elseline = getall(file_to_write)
        dict = parse_dict(thenline, elseline)

        anchors = rule_dict[targ_file]
        anchors = {int(k):v for k,v in anchors.items()}
        if len(anchors):            
            for linenum in sorted(anchors.keys(), reverse=True):           
                # annotate(file_to_write, dict[int(linenum)], list(anchors[linenum].values())[0])
                for brc_anno in list(anchors[linenum].values()):
                    annotate(file_to_write, dict[int(linenum)], brc_anno)
    return

if __name__ == "__main__":    
    main()