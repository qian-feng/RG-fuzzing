import difflib
from deepdiff import DeepDiff
import os
import re
import pdb
import argparse
import json
import rules_pb2

# def parseRange(ra):
# 	#straid
# 	straid = 1
# 	items = ra.split("{")
# 	rs = items[1].split("}")[0]
# 	items = rs.split(";")
# 	if len(items) !=0:
# 		ht = 1
# 	else:
# 		items = rs.split("..")
# 		if len(items) !=0:
# 			ht = 1
# 	if ht == 1:
# 		low = items[0]
# 		high = items[-1]
# 		if len(items) == 1:
# 			return int(low)
# 		if "--" not in low:
# 			low = int(low)
# 		if "--" not in high:
# 			high = int(high)
# 		if len(items) > 2:
# 			straid = int(items[1]) - low
# 		return (low, high, straid)
# 	else:
# 		print "****************parse error********************"
# 		return 0

def valid(var):
	if "__fc" in var:
		return False
	if "__" in var:
		return False
	if "tmp_" in var:
		return False
	if "maps" in var:
		return False
	if "*" in var:
		return False
	if "(" in var:
		return False
	if "malloc" in var:
		return False
	if "argv" in var:
		return False
	if "_" in var:
		return False
	if "&" in var:
		return False
	return True

def getRan(value):
	#all the values
	#ran = root['value'].replace(';',',')
	try:
		ran = re.findall(r"\{([0-9]+)(.*?)([0-9]*)\}", value)[0]
		if ran[1] == '':
			return [ran[0],ran[0]]
		return [ran[0],ran[-1]]
	except:
		return None



def getDiff(idx, f1, f2):
	t1 = json.load(open(f1, "r"))
	t2 = json.load(open(f2, "r"))
	diffs = DeepDiff(t1, t2)
	results =  []
	for key in diffs:
		for x in diffs[key]:
			if key == 'values_changed':
				x = x.replace("root", 't2')
				if "values" not in x:
					continue
				items = re.findall(r"\[([A-Za-z0-9_\']+)\]", x) 
				if '\'value\'' in x:
					root = eval(x.split("[\'value\'")[0])
				else:
					root = eval(x.split("[\'offset\'")[0])
				ran = root['value']
				if not valid(ran):
					continue
				if len(root) == 1:
					#pdb.set_trace()
					offset = ''
				else:
					offset = root['offset']	
					## do not consider array  or objects here.
					continue
				var_name = eval(x.split("[\'values\'")[0])['base']
				if valid(var_name):
					ran = getRan(root['value'])
					if ran:
						st = "IJON_Range({0},{1},{2},{3});".format(idx, var_name, ran[0], ran[1])
						results.append(st)
	return results


def getFiles(dirs):
	rules = {}
	for fullname in os.listdir(dirs):
		items = re.findall(r"(.*?):([0-9]+):([-0-9]+).([a-z0-9]+)_([0-9]+)*", fullname)[0]
		fname = items[0]
		branch = items[3]
		line = int(items[1])
		idx = int(items[2])
		if fname not in rules:
			rules[fname] = {}
		if line not in rules[fname]:
			rules[fname][line] = {}
		if branch not in rules[fname][line]:
			rules[fname][line][branch] = {}
			rules[fname][line][branch]['name'] = []
		rules[fname][line][branch]['idx'] = idx	
		rules[fname][line][branch]['name'].append(fullname)
	return rules

def genSwitch(bran_dic):
	bran_dic['switch']['name'].sort()
	sw_fp = bran_dic['switch']['name'][-1]
	rs = []
	for case in bran_dic:
		if case != 'switch':
			bran_dic[case]['name'].sort()
			case_fp = bran_dic[case]['name'][-1]
			rule_case = getDiff(bran_dic['switch']['idx'],os.path.join(dirs,sw_fp), os.path.join(dirs,case_fp))
			if len(rule_case) != 0:
				rs.append("\n".join(rule_case))
	bran_dic['rule'] = "\n".join(rs)


def genOthers(bran_dic):
	bran_dic['if']['name'].sort()
	if_fp = bran_dic['if']['name'][-1]
	#check then
	if 'then'in bran_dic:
		bran_dic['then']['name'].sort()
		then_fp = bran_dic['then']['name'][-1]
		rule_if = getDiff(bran_dic['if']['idx'],os.path.join(dirs,if_fp), os.path.join(dirs,then_fp))
		if len(rule_if) != 0:
			bran_dic['then_rule'] = "\n".join(rule_if)
	#check else
	if 'else' in bran_dic:
		bran_dic['else']['name'].sort()
		else_fp = bran_dic['else']['name'][-1]
		rule_else = getDiff(bran_dic['else']['idx'],os.path.join(dirs,if_fp), os.path.join(dirs,else_fp))
		if len(rule_else) != 0:
			bran_dic['else_rule'] = "\n".join(rule_else)

def genRules(dirs):
	rules = getFiles(dirs)
	for fname in rules:
		for line in rules[fname]:
			#check if
			bran_dic = rules[fname][line]
			if 'switch' in rules[fname][line]:
				genSwitch(bran_dic)
			else:
				genOthers(bran_dic)
	return rules

def profile(rules):
	n_f = len(rules)
	n_b = 0
	hit = 0
	for f in rules:
		for l in rules[f]:
			if 'then' in rules[f][l]:
				n_b += 1
			if 'else' in rules[f][l]:
				n_b += 1
			if 'then_rule' in rules[f][l]:
				hit += 1
				print f, l, rules[f][l]['then_rule']
			if 'else_rule' in rules[f][l]:
				hit+= 1
				print f, l, rules[f][l]['else_rule']
	print hit*1.0/n_b

def storeRules(rules, outfile):
	rs = rules_pb2.Rules()
	for filename in rules:
		rule = rs.rules.add()
		rule.fname = filename
		# if filename == 'compare_strings.c':
		# 	pdb.set_trace()
		# 	print "s"
		for l in rules[filename]:
			line = rule.line.add()
			line.lnum = l
			if 'then_rule' in rules[filename][l]:
				lrule = line.lrule.add()
				lrule.branch = 'then'
				lrule.r = rules[filename][l]['then_rule']

			elif 'else_rule' in rules[filename][l]:
				lrule = line.lrule.add()
				lrule.branch = 'else'
				lrule.r = rules[filename][l]['else_rule']

			elif 'rule' in rules[filename][l]:
				lrule = line.lrule.add()
				lrule.branch = 'switch'
				lrule.r = rules[filename][l]['rule']
			else:
				del rule.line[-1]

		if len(rule.line) == 0:
			del rs.rules[-1]
	pdb.set_trace()
	f = open(outfile, "wb")
	f.write(rs.SerializeToString())
	f.close()

			
if __name__ == '__main__':
	parser = argparse.ArgumentParser(description='Process some integers.')
	parser.add_argument('-dirs', help='dirs for ranges ')
	parser.add_argument('-out', help='dirs for ranges ')
	args = parser.parse_args()
	dirs = args.dirs
	outfile = args.out
	rules = genRules(dirs)
	#profile(rules)
	storeRules(rules, outfile)
	pdb.set_trace()
	print "Finished!"


    	