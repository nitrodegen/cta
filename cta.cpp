#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <signal.h>
#include <random>
#include <string.h>
#include <sstream>
#include <regex>
#include <unistd.h>
using namespace std;

void debug(vector<string>data){
	for(int i =0;i<data.size();i++){
		cout<<data[i]<<endl;
	}	
}

vector<string>split(string str,char del){
	vector<string>ret;
	string temp="";	
	for(int i =0;i<str.length();i++){
		if(str[i] == del){
			ret.push_back(temp);
			temp="";
		}
		else{
			temp += str[i];
		}
	}
	return ret;
}
vector<string> split1(string str,string del){
	vector<string>dele;
	ssize_t beg,pos=0;
	while((beg=str.find_first_not_of(del,pos)) != string::npos){ // loop until you find everything that isn't a delimiter , and always set that to be beginning
		pos = str.find_first_of(del,pos+1);//position is always the next case of del
	
		dele.push_back(str.substr(beg,pos-beg)); // and push 
	}
	return dele;
}

vector<string>printasm(string toprint ){
	vector<string>sz = split1(toprint,"\"");
	toprint = sz[1];
	stringstream ss;
	string r;
	srand(time(NULL));

	ss<<rand();
	ss>>r;
	string name = "printf"+r;
	ss.clear();
	string len;
	ss<<toprint.length();
	ss>>len;
	string instruction ="\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,"+name+"\n\tmov rdx,"+len+"\n\tsyscall";

	
	vector<string>arr;
	arr.push_back(instruction);
	arr.push_back(name);
	arr.push_back(toprint);
	
	return arr;
	

	
}

int main(int argc,char *argv[]){
	if (argc <2){
		cout<<"C to x86_64 Assembly(CtA)\n\tby suibex.\nUsage: ./cta.py <filename> <args>\nExisting args: -k (--keep) -- keeps the source file in assembly language."<<endl;
		exit(1);
	}
	string file = argv[1];
	ifstream t(file);
	string str;
	t.seekg(0,ios::end);
	str.reserve(t.tellg());
	t.seekg(0,ios::beg);
	str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
	
//	cout<<str<<endl;
	string l ="main(){";
	string desc = str.substr(str.find("main(){")+l.length());
	desc.erase(remove(desc.begin(),desc.end(),'\t'),desc.end());
	//desc.replace(desc.find(" "),1,"/");
	vector<string> splitted = split(desc,';');
	//desc.replace(desc.find("="),1,"[");
	vector<pair<string,string > >prep;
	
	
	for(int i =0;i<splitted.size();i++){

		if(splitted[i].find("for") != string::npos){
		
			string isize = splitted[i];
			string looptimes = splitted[i+1];
			string stat = splitted[i+2].substr(0,splitted[i+2].find(")"));
			//cout<<stat<<endl;
			vector<string>insides;
			
			for(int j =i+2;j<splitted.size();j++){
				if(splitted[i].find("}") != string::npos){
					break;
				}
				if(splitted[i] != stat){
					string forbranch = splitted[j].substr(splitted[j].find("{")+1);
					forbranch.erase(remove(forbranch.begin(),forbranch.end(),'\t'),forbranch.end());
					insides.push_back(forbranch);

				}
			}
			for(int i =0;i<insides.size();i++){
				string func = insides[i].substr(0,insides[i].find("("));
				string val = insides[i].substr(insides[i].find("\"")+1);
				val.replace(val.find("\")"),2,"");
				func = "FOR"+func;
				prep.push_back(make_pair(func,val));
				
			}
		 
			string startfrom = isize.substr(isize.find("for(int")+7);
			startfrom = startfrom.substr(startfrom.find("=")+1);
			string loopfor = looptimes.substr(looptimes.find("<")+1);

			string toprep = startfrom+"]"+loopfor+"_/"+stat;
			
			prep.push_back(make_pair("for",toprep));
			
		}
	
		if(splitted[i].find("printf(") != string::npos  && splitted[i].find("<") == string::npos &&  splitted[i].find("++") == string::npos &&  splitted[i].find("for(") == string::npos && splitted[i].find("i++){") == string::npos && splitted[i].find("%d")== string::npos && splitted[i].find("if") == string::npos && splitted[i].find("else")==string::npos){
			
			vector<string>row = split(splitted[i],'"');
			string f = row[0];
		
			f.erase(remove(f.begin(),f.end(),'('),f.end());
	//		cout<<f<<endl;
		
			prep.push_back(make_pair(f,row[1]));
			
		}
		
		
		
		if (splitted[i].find("if") != string::npos){
				//first split tree part , then if part() 
				vector<string>spagi = split1(splitted[i],"{");
				string todo;
				vector<string>ifpart =	split1(spagi[0],"(");
				
				string stat = ifpart[1];
				stat.erase(remove(stat.begin(),stat.end(),')'),stat.end());
			
				vector<string>ifstat = split1(stat," ");

				for(int i =1;i<ifstat.size();i++){
					if(ifstat[i] == ">"){
						string fleki = ifstat[i-1]+"_/"+ifstat[i+1];  
						prep.push_back(make_pair("cmp",fleki));

					}
	

				}
				
			
				
		}

		if(splitted[i].find("%d") != string::npos && splitted[i].find("printf") != string::npos
				 && splitted[i].find("if") == string::npos && splitted[i].find("else") == string::npos && splitted[i].find("for") == string::npos) {
			vector<string>la = split1(splitted[i],",");
			
			vector<string>z = split1(la[0],"(");
			string place = z[1];
		
			place.erase(remove(place.begin(),place.end(),'"'),place.end());		
			string fb =la[1];
		
			fb.erase(remove(fb.begin(),fb.end(),')'),fb.end());
			
			for(auto const& th:prep){
				
				if(fb.compare(th.first)==0){

			
					stringstream ss;
					string val;
					ss<<th.second;
					ss>>val;
					place.replace(place.find("%d"),val.length()+1,val);
					cout<<"PLACE";
				//	cout<<place<<endl;


					prep.push_back(make_pair("printf",place));

				}
				else if(fb.compare(th.second)==0 && th.first.compare("resb")==0){
					string cp = splitted[i];
			
					cp.replace(cp.find("printf("),7,"");
					vector<string>spliti = split1(cp,"%d");
					string text = spliti[0];
					text.erase(remove(text.begin(),text.end(),'"'),text.end());

					string comb = text+"_/"+th.second;
	
					char check =th.second[0];
				
					prep.push_back(make_pair("em",comb));	
					

				}
				
			}
		
		

		}
		if(splitted[i].find("scanf") != string::npos ){
				
			vector<string>teti = split1(splitted[i],",");
			
			string saver = teti[1];
			saver.erase(remove(saver.begin(),saver.end(),')'),saver.end());
			prep.push_back(make_pair("scanf",saver));
		}
	
		if( splitted[i].find("<") == string::npos && splitted[i].find("++") == string::npos && splitted[i].find("else") == string::npos &&  splitted[i].find("for") == string::npos && splitted[i].find("if") == string::npos && splitted[i].find("else") == string::npos && splitted[i].find("printf") == string::npos && splitted[i].find("scanf")== string::npos){
			
		
			vector<string> rowed =split1(splitted[i]," ");
			//debug(rowed);
			
			if(rowed.size() ==2){
				prep.push_back(make_pair("resb",rowed[1]));			
			}
			else{
				prep.push_back(make_pair(rowed[1],rowed[3]));
			}
		
			
		}	
		if(splitted[i].find("else") != string::npos){
			vector<string>test = split1(splitted[i],"{");

			test.erase(test.begin());
			for(int i =0;i<test.size();i++){
				prep.push_back(make_pair("else",test[i]));
		
			}
			
		}
		
		
			
	}
	
	
	
	string heading =";Converted with CoT by Suibex.\nglobal _start\n_start:\n\tpush rbp\n\tmov rbp,rsp\n\tsub rsp,0x10";

	
	vector<string>instructions;
	string objfun="\n\t;quit\n\tmov rax,60\n\tmov rdi,0\n\tsyscall\n\tret\n\t";
	for(auto const& th: prep){
		

		
		if( th.first.find("else") == string::npos  && th.first.find("cmp") == string::npos && th.first.find("for") == string::npos && th.first.find("FORprintf") == string::npos &&   th.first.find("printf") == string::npos && th.first.find("em") == string::npos && th.first.find("scanf") == string::npos && th.first.find("resb") == string::npos){
			string ass=";decimal declared\n\tmov dword[rbp-"+th.second+"],"+th.second+";"+th.first+"\n";

			instructions.push_back(ass);
		}
		
		if(th.first.compare("for")==0){
			string defval = th.second.substr(0,th.second.find("]"));
			string lfor = th.second.substr(th.second.find("]")+1);
			lfor = lfor.substr(0,lfor.find("_/"));
			srand(time(NULL));
			stringstream z;
			z<<rand();
			string fopi;
			z>>fopi;
			z.clear();
			string inc = th.second.substr(th.second.find("_/")+2);
			string ass = "\n\tmov cx,"+lfor+"\nstartloop"+fopi+":\n\tcmp cx,"+defval+"\n\tjz end"+fopi+"\n\tpush cx\nloop"+fopi+":";
			int h=0;
			for(auto const& th:prep){
				
				if(th.first.compare("FOR\nprintf")==0){
					z.clear();
					z<<h;
					string o;
					z>>o;
					z.clear();

					objfun+="\nprintf"+fopi+o+":\n\tdb \""+th.second+"\",0x0a,0x0d";
					z.clear();
					string leni;
					z<<th.second.length();
					z>>leni;
					z.clear();
					ass+="\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,printf"+fopi+o+"\n\tmov rdx,"+leni+"\n\tsyscall\n";

				}
				h++;
			}
			ass+="\n\tpop cx\n\tdec cx\n\tjmp startloop"+fopi;
			ass+="\nend"+fopi+":";
			
			instructions.push_back(ass);

			
		}
		
		if(th.first.compare("cmp")==0){
			vector<string>comp = split1(th.second,"_/");
			string first = comp[0];
			string second = comp[1];
			string addon ;

			for(auto const& titi : prep){
				
				if(first == titi.first){
					
					for(auto const& tho:prep){

						if(second == tho.first){

							stringstream randi;
							string namefunc;
							srand(time(NULL));
							randi<<rand();
							
							string f;
							randi>>f;
							namefunc="cmp"+f;
							int zoza=0;
						
							for(int i =0;i<splitted.size();i++){
								if(splitted[i].find("if") != string::npos && splitted[i].find("printf") != string::npos){
										
									string z = "pr"+f;
					
					
									vector<string>spliti = split1(splitted[i],"\"");
									addon+=z;
									string printz = spliti[1];
									zoza+=printz.length();
									objfun+="\n"+z+":\n\tdb \""+printz+"\"";

								
								}
							}
							

							string ass = ";comparison\n\tmov eax,dword[rbp-"+titi.second+"]\n\tcmp dword[rbp-"+tho.second+"],eax\n\tjle "+namefunc;
							instructions.push_back(ass);


							if(!addon.empty()){
								stringstream ssa;
								ssa<<zoza;
								string lenzi;
								ssa>>lenzi;
								ssa.clear();

								objfun+="\nsection .text\n"+ namefunc+":\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,"+addon+"\n\tmov rdx,"+lenzi+"\n\tsyscall\n\tmov rax,60\n\tmov rdi,0\n\tsyscall";
								
							
							}
							else{
								objfun+="\nsection .text\n"+namefunc+":\n\tmov rax,60\n\tmov rdi,0\n\tsyscall";								
								
							}

						}

					}
				
	
				}

			}
		}
		if(th.first.compare("else")==0){
			vector<string>instr = printasm(th.second);
			string ass = instr[0];
			string name = instr[1];
			string print = instr[2];
			
			objfun+="\nsection .data\n"+name+":\n\tdb\""+print+"\"";
			instructions.push_back(ass);
			

		}
		if(th.first.find("scanf") != string::npos){
			string ass = "\n\tmov rax,0\n\tmov rdi,0\n\tmov rsi,"+th.second+"\n\tmov rdx,9107\n\tsyscall";
			instructions.push_back(ass);

		}
		if(th.first.find("em") != string::npos){
		
			vector<string>downed = split1(th.second,"_/");

			vector<string>updated;
			for(int i = 0;i<downed.size();i++){
		
				if(downed[i+1] != downed[i]){
					updated.push_back(downed[i]);
				}

			}
			//debug(updated);
			//instructions.push_back(ass);
			string ass1 = "\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,"+updated[updated.size()-1]+"\n\tmov rdx,9107\n\tsyscall";	
			
			updated.pop_back();

			for(int i =0;i<updated.size();i++){
				stringstream ss;
				string ri;
				srand(time(NULL));

				ss<<rand();
				ss>>ri;
				string nm ="printf"+ri; 
				ss.clear();
				updated[i].erase(remove(updated[i].begin(),updated[i].end(),'\n'),updated[i].end());
			//	updated[i].erase(remove(updated[i].begin(),updated[i].end(),' '),updated[i].end());
				string len;
				ss<<updated[i].length();
				ss>>len;
			

				string ass = "\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,"+nm+"\n\tmov rdx,"+len+"\n\tsyscall";	
				string add="\nsection .data\n"+nm+":\ndb \""+updated[i]+"\",0xA,0x0";
				objfun+=add;
				instructions.push_back(ass);
			}
			instructions.push_back(ass1);
		
		}
		if(th.first.find("resb") != string::npos){
			objfun+="\nsection .bss\n\t"+th.second+" resb 9107";
			continue;
		}
		
		if(th.first.compare("\nprintf") ==0 || th.first.compare("printf") ==0 ){	
			
		
			stringstream ss;
			string l;
			ss<<th.second.length();
			ss>>l;
			string name = th.first;
			name.erase(remove(name.begin(),name.end(),'\n'),name.end());
			
			vector<string>shit = split1(th.second," ");
			
			if(th.first.find("printf")!= string::npos){
				
				ss.clear();
				string ri;
				ss<<rand();
				ss>>ri;
				string nm ="printf"+ri; 
	
				string ass=";printf\n\tmov rax,1\n\tmov rdi,1\n\tmov rsi,"+nm+" \n\tmov rdx,"+l+"\n\tsyscall";
		        	objfun+="\nsection .data\n\t\n"+nm+":\n\tdb \""+th.second+"\",0xA,0x0";
				instructions.push_back(ass);			
	
			}
		
		}
		
	}
	
	
	

	
	string filename = file.substr(0,file.find("."))+".asm";
	ofstream fifi(filename);
	if(fifi.is_open()){
		fifi<<objfun;
	}	
	fifi.close();
	ifstream zoz(filename);
	string cont;
	zoz.seekg(0,ios::end);
	cont.reserve(zoz.tellg());
	zoz.seekg(0,ios::beg);
	cont.assign((std::istreambuf_iterator<char>(zoz)),
            std::istreambuf_iterator<char>());
	ofstream fizi(filename);
	for(int i =0;i<instructions.size();i++){
		fizi<<"\n"+instructions[i]+"\n";	
//		cout<<instructions[i]<<endl;

	}
	fizi<<cont;

	fizi.close();

	ifstream zoz1(filename);
	string cont1;
	zoz1.seekg(0,ios::end);
	cont1.reserve(zoz.tellg());
	zoz1.seekg(0,ios::beg);
	cont1.assign((std::istreambuf_iterator<char>(zoz1)),
            std::istreambuf_iterator<char>());
	ofstream fiqi(filename);
	if(fiqi.is_open()){
		fiqi<<heading;
		fiqi<<cont1;
	}	

	fiqi.close();
	
	string cmd1= "nasm -f elf64 "+filename;
	system(cmd1.c_str());
	string ofi=filename.substr(0,filename.find("."));
	string flch = ofi+".o";
	string cmd2 = "ld -o "+ofi+" "+flch;
//	cout<<cmd2<<endl;
	system(cmd2.c_str());
	string cmdi = "rm "+ flch;
	system(cmdi.c_str());
	if(argc >=3){
		string spec = argv[2];
		if(spec == "-k" || spec == "--keep")
		{
		
			string c = "./"+ofi;
		//	cout<<c<<endl;
			system(c.c_str());
		}
	}
	else{
		remove(filename.c_str());
		string c = "./"+ofi;
		system(c.c_str());
		remove(c.c_str());
	}

	
}	
		
