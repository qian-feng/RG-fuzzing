import subprocess
import pdb

class tis:

	def __init__(self):
		return 

	def test(self, filen):
		return True
	
	def RunTis(self,filename):
		fn = filename.replace(".c", "_v.c")
		tis_cmd = "tis-analyzer -slevel 5000 -wlevel 100 -val " + fn
		p = subprocess.Popen(tis_cmd, stdout=subprocess.PIPE, shell=True)
		(output, err) = p.communicate()
		pdb.set_trace()
		if output.find("division by zero") != -1:
			return 1
		return 0

if __name__ == '__main__':
	tis().RunTis("/home/tis/auto-verification-suite/mock_generator/tis-mock-gen/loops/src/loop03_v.c")
