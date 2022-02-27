#!/opt/homebrew/Caskroom/miniforge/base/envs/mlp/bin/python3
"""
	Really shitty attempt at building a compiler
		Really did it because i was bored. not that bad at the end lol.
	Learned how compilers actually work ,aarch64 assembly, registers etc... (not that much)
	14. Feb 22.
		-suibex

"""
import os,io,sys
from threading import Thread
#import nltk
import numpy as np 
import pickle
import random
import json
#from nltk.stem import WordNetLemmatizer 
#from tensorflow.keras.models import load_model

if(len(sys.argv) <2):
	print("C to Assembly (CtA)\n\tby suibex.\nUsage: ./cta.py <filename> <args>\nExisting args: -k (--keep) -- keeps the source file in assembly language.")
	sys.exit(1)

name = sys.argv[1]

f = open(name,"r")
cont = f.read()
ind = cont.find("main()")

f.seek(ind+7)
cont = f.read()
cont = cont.split()
updated=""
for i in range(len(cont)):
        if(cont[i] != "{" and cont[i] != "}"):
                updated+=" "+cont[i]
#print(cont)
#lem= WordNetLemmatizer() 
words=updated.split(";")

#print(f"WORDS:{words}")
prep=[]

			
for i in range(len(words)):
#		print(words[i])
		if words[i] == '':
			#print("skip..")
			continue
		if "printf(" not in words[i]:
	
			wow =words[i].split()
				
			#print(f"WOW{wow}")
		#	sys.exit(1)
			#wrd=wow.replace(';','')		
			prep.append([wow[1],wow[3]])
	#		break					
		
		if "printf(" in words[i]:
			ws = words[i].split('"')
			#print(ws)
			f = ws[0].replace('(',"")
			val =ws[1]
			prep.append([f,val])
 
#print(prep)
prepared=prep
asm = []
i=0
objfun="\n\t;quit\n\tmov w0,#0x0\n\tadd sp,sp,#0x10\n\tret"


while True:
	if(len(prepared)<=0):
		break
	#Create assembly code now and execute it.
	if ("printf" not in prepared[i][0]):
		ass=f"\tsub sp,sp,#0x10\n\tmov w8,{prepared[i][1]};{prepared[i][0]}\n\tstr w8,[sp,#0xc]\n"
#		print(ass)
		asm.append(ass)
		prepared.pop(i)	
	else:
		ass=f"\tmov x0,#1\n\tadr x1,{prepared[i][1].split()[0]}\n\tmov x2,#{len(prepared[i][1])}\n\tmov x16,#4\n\tsvc #0x80"
		objfun=f"\n\t;quit\n\tmov x0,#0\n\tmov x16,#1\n\tsvc #0x80\n{prepared[i][1].split()[0]}: .ascii \"{prepared[i][1]}\""
#		print(ass)
		asm.append(ass)
		prepared.pop(i)

fname = name.split(".")
nam=f"{fname[0]}.S"
f = open(nam,"w")
f.write(objfun)
f.close()
z = open(nam,"r")
content = z.read()

f = open(nam,"w")
for i in range(len(asm)):
	compiled = f"\n{asm[i]}\n"
	f.write(compiled)
	
f.write(content)
f.close()
mainf =".global _start;Converted with CoT by Suibex.\n.align 2\n_start:\n"
z =open(nam,"r")
content = z.read()
f = open(nam,"w")
f.write(mainf)
f.write(content)
f.close()
#print(nam)

os.system(f"as -o obj.o {nam}")
os.system(f"ld -o {fname[0]} obj.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64")
os.remove("obj.o")
if(len(sys.argv) >=3):
	spec = sys.argv[2]
	if(spec =="-k" or spec=="--keep"):
	
		os.system(f"./{fname[0]}")
else:
	os.remove(nam)
	os.system(f"./{fname[0]}")
	os.remove(fname[0])

