#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <sstream>
#define MAINSTART 4420
#define MAINEND 4464
//start of main :4420
//end of main : 4464
using namespace std;

int Occurance(string str,char del){
	int found = 0;
	for(int i = 0 ;i<str.length();i++){

		if(str[i] == del){
			found++;
		}

	}
	return found;

}

vector<pair <string, string > > readDict(){
	vector<pair <string ,string > > insts;
	fstream f("dict.gpd",std::ios::in);
	string input ;
	
	while(f>>input){
		string one = input.substr(0,input.find("/"));
		string two = input.substr(input.find("/")+1);
		int n = Occurance(two,':');		
		for(int i =0;i<n;i++){
			two.replace(two.find(":"),1," ");
		}
		insts.push_back(make_pair(one,two));
	}
	return insts;

}
vector<pair<string , string> > Reader(string filename){
	vector<string>data;
	FILE *bin;
	bin = fopen(filename.c_str(),"r");
	unsigned char bindat[2];
	vector<string>func;
	vector<pair<string,string> > instructions;
	
	fseek(bin,0,SEEK_SET);

	while(true){
		stringstream ss;
		int re = fread(bindat,sizeof(unsigned char),1,bin);
		if(re <= 0){
			break;
		}
		char b[sizeof(bindat[0])];	
		sprintf(b,"%02x",bindat[0]);
		ss<<b;
		string p(ss.str());
	
		data.push_back(p);
		
	}
	for(int i = 0;i<MAINEND;i++){
		if(i>=MAINSTART){
			func.push_back(data[i]);
		}
	}
	vector<pair<string , string > > paired=readDict();
	for(int i =0;i<func.size();i++){

		for(auto const& ins: paired){
			if(func[i] == ins.first){
				if( func[i] == "89" && func[i+1] == "e5"){
					instructions.push_back(make_pair(ins.first,"mov %rsp %rbp"));
				}
				else{	
					instructions.push_back(make_pair(ins.first,ins.second));
				}		


			}
		}


	}
	fclose(bin);
	

	return instructions;
}

void PrintTree(string filename , vector<pair<string , string > > dat){
	ofstream f(filename+".dump",fstream::app | fstream::binary);
	for(auto const& h: dat){
		cout<< &h.first<<"\t"<<h.first<<"\t"<<h.second<<endl;
		f<<&h.first<<"\t"<<h.first<<"\t"<<h.second<<"\n";	
	}
	f.close();
	
}
int main(int argc ,char *argv[]){
	if(argc < 2){
		cout<<"Usage: ./gpd {name of executable}"<<endl;
		return 1;
	}

	string file = argv[1];
	ofstream d(file+".dump");
	d<<"MEMORY DUMP OF "<<file<<":"<<"\n";
	d.close();

	cout<<"\033[1;31mGPD Disassembler v1.0\033[0m\n\rPrototype build to understand opcodes.\n\rHello world example loaded.\n\rDump of assembler code for program '"<<file<<"' : \n"<<endl;

	
	vector<pair <string , string >  > inc = Reader(file);

	PrintTree(file,inc);

	cout<<"Memory dumped and saved to "<<file<<".dump."<<endl;
}
