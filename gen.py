from random import randint
import subprocess,sys

if(len(sys.argv) != 2):
	# file is a cpp program (generateHash.cpp) that converts password , salt into a hash value
	print("Usage: python gen.py file")
	sys.exit()

numPasswords = 10
pwdlen = [6,7,8]
alphabet = ['a' ,'b'  ,'c'  ,'d'  ,'e'  ,'f'  ,'g'  ,'h'  ,'i'  ,'j'  ,'k'  ,'l'  ,'m'  ,'n'  ,'o'  ,'p'  ,'q'  ,'r'  ,'s'  
 ,'t'  ,'u'  ,'v'  ,'w'  ,'x'  ,'y'  ,'z'  ,'A'  ,'B'  ,'C'  ,'D'  ,'E'  ,'F'  ,'G'  ,'H'  ,'I'  ,'J'  ,'K'  ,'L' 
 ,'M'  ,'N'  ,'O'  ,'P'  ,'Q'  ,'R'  ,'S'  ,'T'  ,'U'  ,'V'  ,'W'  ,'X'  ,'Y'  ,'Z'  ,'0'  ,'1'  ,'2'  ,'3'  ,'4'  
 ,'5'  ,'6' ,'7'  ,'8'  ,'9']

def generatePassword(length):
	s,q = "",""
	for i in range(length):
		s += str(randint(0,9))
	for i in range(2):
		q += alphabet[randint(0,len(alphabet)-1)] 
	return s,q

executable = "Hasher"
subprocess.call(['g++', sys.argv[1], '-lcrypt', '-o', executable])

# index = 0
# mylist = []
file = ""
pwd, salt, hashval = "","",""
for i in pwdlen:
	# file += str(i) + '\n'
	for j in range(numPasswords):
		pwd,salt = generatePassword(i)
	# subprocess.check_output(['your_program.exe', 'arg1', 'arg2'])
		hashval = subprocess.check_output([ ("./" + executable), pwd , salt])
		# (mylist[index]).append((hashval,pwd))
		file += hashval + ", " + pwd + '\n'
	# index += 1
	# file += '\n'

with open('pwd.txt','w') as f:
	f.write(file)