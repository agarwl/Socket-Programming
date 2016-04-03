import subprocess

pwdlen = [6,7,8]
numPasswords = 10
index = 0
mylist = [[],[],[]]
hashval,output = "",""

with open('pwd.txt','r') as f:
	for line in f:
		hashval = line.split(",")[0]
		(mylist[index]).append(hashval)
		if(len(mylist[index]) == 10):
			index += 1

port = 5000
hostname = str(subprocess.check_output(['hostname']))
hostname = hostname.rstrip('\n')
# subprocess.Popen(['./server',str(port)], shell=True)
plen = 6
with open('output.txt','w') as f:
	for i in mylist:
		f.write(str(plen) + '\n')
		for hashv in i:
			 output = subprocess.check_output(['./user', hostname, str(port), hashv, str(plen) , '001'])
			 print(output)
			 f.write(output + '\n')
		plen += 1