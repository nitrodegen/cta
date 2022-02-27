#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;



void dump(const char *name,void *addr , int len){
	FILE *dumpfile;
	const char *ext=".dump";
	char comb[strlen(name)+strlen(ext)+1];

	strcpy(comb,name);
	strcat(comb,ext);

	dumpfile = fopen(comb,"ab");
	
	int i =0;
	unsigned char *pc = (unsigned char*)addr;
	if(strlen(name) >0 ){
		const char *n = name;
		printf("%s:\n",name);	
		fprintf(dumpfile,"%s:\n",n);
	}

	printf(" %04x ",i);
	for(i = 0;i<len;i++){
		printf(" %02x ",pc[i]);
		
		fprintf(dumpfile," %02x ",pc[i]);

	}
	fclose(dumpfile);		
	
}

void runFunction(char *argv){
	
	char buffer[500];	
	char ar=(char)0x51;
	for(int i = 0;i<sizeof(buffer)+20;i++){
		buffer[i]=ar;
	//	cout<<sizeof(buffer)-strlen(buffer)<<endl;
	}
}

int main(int argc , char *argv[]){
	//runFunction(argv[1]);
	char b[1024];
	snprintf(b,1024,"%p",runFunction);
	printf("\nSTART:%p\n\t",runFunction);
	stringstream ss;
	string mem;
	ss<<b;
	ss>>mem;
	void *addr;
	ss.clear();
	ss<<mem;
	ss>>addr;

	int len = sizeof(addr);
	dump(mem.c_str(),addr,len);
	printf("\nEND:%p\n",dump);
}
