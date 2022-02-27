#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
int main(int argc ,char *argv[]){
	int key = atoi(argv[1]);

	if(argc < 2){
		printf("Usage: ./exec <key>");
		return 1;
	}	
	int *x = new int(2); 
	if( key == *x){
		delete x;
		cout<<"SUCCESS."<<endl;

	}	
	else{
		cout<<"Wrong."<<endl;	
		exit(1);
	}	
}

