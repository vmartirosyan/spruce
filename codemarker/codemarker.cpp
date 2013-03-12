#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <regex.h>
#include <vector>
#include <sstream>
#include <limits.h>
using namespace std;

const char marker[] = "123456789";

const char regx_sf[] = "^SF:(.*)";
const char regx_eor[] = "^end_of_record";

//Regex for DB file
const char regx_fn[] = "^FN:([^, ]+)( (.*))?";
const char regx_lft[] = "^LFT:([0-9]+),([0-9]+)( (.*))?";

//Regex for Info file 
const char iregex_fn[] = "^FN:([0-9]+),([^,]+)";
const char iregex_fnda[] = "^FNDA:([0-9]+),([^,]+)";
const char iregex_da[] = "^DA:([0-9]+),(-?[0-9]+)(,[^, ]+)?( (.*))?";
const char iregex_brda[] = "^BRDA:([0-9]+),([0-9]+),([0-9]+),([0-9]+|-)";

const char * regxs[] = {regx_sf, regx_eor, /**/ regx_fn, regx_lft, /**/iregex_fn, iregex_fnda, iregex_da, iregex_brda};


regex_t cregx_sf;
regex_t cregx_eor;

//Regex for DB file
regex_t cregx_fn;
regex_t cregx_lft;

//COmpiled regex for Info file 
regex_t ciregex_fn;
regex_t ciregex_fnda;
regex_t ciregex_da;
regex_t ciregex_brda;	

regex_t * cregxs[] = {&cregx_sf, &cregx_eor, /**/ &cregx_fn, &cregx_lft, /**/&ciregex_fn, &ciregex_fnda, &ciregex_da, &ciregex_brda};


struct LFT_ST 
{
	unsigned int st;
	unsigned int end;
	string desc;
};

vector<string> SF; // Sounre files
vector<string> FN; // Function name 
vector<string> FNDESC; // Function description
vector< vector<LFT_ST> > LFT;

//Define for debug messages
//#define DEBUG

int main (int argc, char* argv[]) 
{
	if(argc < 3) 
	{
		cout << "Usage : " << argv[0] << "<info_file> <uncovered_code.db> <output_file>" << endl;
		cout << "By default <output> file is out.info"<< endl;
		return 1;
	}
		
	fstream info;
	info.open(argv[1], fstream::in | fstream::out); 
	if(info == 0)
	{
		cout<<"Cannot open <info_file>"<<endl;
		return 1; 
	}	
	
	ifstream db;
	db.open(argv[2], fstream::in); 
	if(db == 0)
	{
		cout<<"Cannot open <uncovered_code.db>"<<endl;
		return 1; 
	}
	
	ofstream fout;
	if(argc == 4)
	{
		fout.open(argv[3], fstream::out | fstream::trunc);
		if(fout == 0)
		{
			cout<<"Cannot create <output_file>"<<endl;
			return 1; 			
		}
	}
	else
	{
		fout.open("out.info", fstream::out | fstream::trunc);
	}
	
	
	int r;
	for(int i = 0; i < 8; ++i)
	{
		if ( (r = regcomp(cregxs[i], regxs[i], REG_EXTENDED)) == true) 
		{
			char errbuf[1024];
			regerror(r, cregxs[i], errbuf, sizeof(errbuf));
			cerr<<"Regular expressions compilation error! Err = "<<errbuf<<endl;
			return 1;
		}
	}
	
	
	
	const int nmatch = 10;
	regmatch_t match_arr[nmatch];
	

	char info_buf[100];
	vector<LFT_ST> cur_LFT; 
	
	while(!db.eof())
	{
		db.getline(info_buf, 100);
		
		memset(match_arr, 0, sizeof(match_arr));
		if(regexec(&cregx_sf, info_buf, nmatch, match_arr, 0) == 0)
		{		
			string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
			SF.push_back(sub);
			continue;
		}
		
		if(regexec(&cregx_fn, info_buf, nmatch, match_arr, 0) == 0)
		{
			string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
			FN.push_back(sub);	
			
			if(match_arr[2].rm_so != -1) // description
			{
				sub = ((string)info_buf).substr(match_arr[2].rm_so + 1, match_arr[2].rm_eo - match_arr[2].rm_so);	
				FNDESC.push_back(sub);
			}
			else
				FNDESC.push_back("");
			
			cur_LFT.clear();
			continue;
		}
		
		if(regexec(&cregx_lft, info_buf, nmatch, match_arr, 0) == 0)
		{
			LFT_ST lft; 
						
			if(match_arr[2].rm_so == -1) 
			{
				cerr<<"Incorrect data in LFT"<<endl;
				return 1;
			}
			
			string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
			std::stringstream convert;
			convert << sub;
			convert>>lft.st;

			sub = ((string)info_buf).substr(match_arr[2].rm_so, match_arr[2].rm_eo - match_arr[2].rm_so);
			convert.clear();
			convert << sub;
			convert>>lft.end;
			
			if(match_arr[3].rm_so != -1) 
			{
				lft.desc = ((string)info_buf).substr(match_arr[3].rm_so + 1, match_arr[3].rm_eo - match_arr[3].rm_so);
			}
			
			cur_LFT.push_back(lft);
			continue;
		}
		
		if(regexec(&cregx_eor, info_buf, nmatch, match_arr, 0) == 0)
		{
			LFT.push_back(cur_LFT);
			continue;
		}
	
	}
	
	
	
	while(!info.eof())
	{
		info.getline(info_buf, 100);
		
		memset(match_arr, 0, sizeof(match_arr));
		if(regexec(&cregx_sf, info_buf, nmatch, match_arr, 0) == 0)
		{		
			string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);
			bool notour = true;
			
			for(vector<string>::iterator it = SF.begin(); it != SF.end(); ++it)
				if(sub.find(*it) != string::npos)
					notour = false; // File is our
			
			fout<<info_buf<<endl;
			
			if(notour)
			{				
				for(;;)
				{
					info.getline(info_buf, 100);
					fout<<info_buf<<endl;
					if(regexec(&cregx_eor, info_buf, nmatch, match_arr, 0) == 0)  // end_of_record
						break;
				}
				continue;
			}
			
			cout<<"Processing file: "<<info_buf<<endl;
			
			vector<LFT_ST>aviableLft; 
			while(!info.eof())
			{
				info.getline(info_buf, 100);
				
				memset(match_arr, 0, sizeof(match_arr));			

				// FN: 
				if(regexec(&ciregex_fn, info_buf, nmatch, match_arr, 0) == 0)
				{		
					if(match_arr[2].rm_so == -1) 
					{
						cerr<<"Incorrect data in info file, FN"<<endl;
						return 1;
					}
					
					string sub = ((string)info_buf).substr(match_arr[2].rm_so, match_arr[2].rm_eo - match_arr[2].rm_so); // get function name
					bool line_added = false;
					
					for(unsigned int i = 0; i < FN.size(); ++i)
						if(FN[i] == sub) // Function found
						{
							
							unsigned int fnst;
							sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
							std::stringstream convert;
							convert << sub;
							convert>>fnst;
							
							if(LFT[i].size())
							{
								for(unsigned int j = 0; j < LFT[i].size(); ++j)
								{
									aviableLft.push_back(LFT[i][j]);
									aviableLft.back().st += fnst; 	// Correct lines start and end
									aviableLft.back().end += fnst;
								}
							}
							else
							{
								// Current functions have no LFT record, creating new fictitious LFT 
								LFT_ST lft;
								lft.desc = FNDESC[i];
								lft.st = fnst;
								
								nextfunction:
								//Keep current file position
								int fpos = info.tellg();	
								
								//Find next function start line 						
								info.getline(info_buf, 100);
								memset(match_arr, 0, sizeof(match_arr));			
								// FN: 
								if(regexec(&ciregex_fn, info_buf, nmatch, match_arr, 0) == 0)	
								{
									sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
									std::stringstream convert;
									convert << sub;
									convert>>fnst;	
									
									// Hack for *.isra functions (bug of gcov)
									if(lft.st == fnst)
									{
										fout<<info_buf<<endl;
										goto nextfunction;
									}
									
									lft.end = fnst - 1;
								}
								else
								{
									// Next record is not function declaration FN:...
									lft.end = INT_MAX;									
								}	
								
								aviableLft.push_back(lft);
								info.seekg(fpos);											
							}
							
							
							fout<<(string)info_buf<<endl; 
							#ifdef DEBUG
							cout<<(string)info_buf<<endl; 
							#endif
							line_added = true;
							
							if(aviableLft.size() == 0)
								cerr<<"Not aviable LFT for some function!"<<endl;
								
							#ifdef DEBUG
							cout<<"FN line "<<fnst<<" FN name: "<<FN[i]<<endl;						
							cout<<"Aviable LFT for current function :"<<endl;
							for(unsigned int i = 0; i < aviableLft.size(); ++i)
								cout<<"LFT. St: "<<aviableLft[i].st<<" End: "<<aviableLft[i].end<<" Desc: "<<aviableLft[i].desc<<endl;
							#endif
						}
					if(!line_added)
						fout<<info_buf<<endl;
					continue;
				}


				// FNDA: 
				if(regexec(&ciregex_fnda, info_buf, nmatch, match_arr, 0) == 0)
				{		
					if(match_arr[2].rm_so == -1) 
					{
						cerr<<"Uncorrect data in info file, FNDA"<<endl;
						return 1;
					}
					
					string sub = ((string)info_buf).substr(match_arr[2].rm_so, match_arr[2].rm_eo - match_arr[2].rm_so); // get function name
					bool line_added = false;
					
					for(unsigned int i = 0; i < FN.size(); ++i)
						if(sub.compare(FN[i]) == 0)
						{
							
							int cov;
							sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
							std::stringstream convert;
							convert << sub;
							convert>>cov;
							
							#ifdef DEBUG
							if(cov != 0) 
								cout<<"Function is dead but is covered! "<<info_buf<<endl;	
							cout<<(string)"FNDA:" + marker + (string)"," + FN[i]<<endl;
							#endif
							fout<<(string)"FNDA:" + marker + (string)"," + FN[i]<<endl; 
							line_added = true;
						}
					if(!line_added)
						fout<<info_buf<<endl;
					continue;
				}				
			
			
				// DA
				static unsigned int lastLFTIndex;
				if(regexec(&ciregex_da, info_buf, nmatch, match_arr, 0) == 0)
				{						
					unsigned int line;
					sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
					std::stringstream convert;
					convert << sub;
					convert>>line;
					
					bool line_added = false;
					
					for(unsigned int i = 0; i < aviableLft.size(); ++i) 
					{
						if((aviableLft[i].st <= line) && (line <= aviableLft[i].end))
						{
							#ifdef DEBUG
							cout<<(string)"DA:" + sub + (string)"," + marker;
							#endif
							fout<<(string)"DA:" + sub + (string)"," + marker;
							if((lastLFTIndex != i) && (aviableLft[i].desc != ""))
							{
								#ifdef DEBUG
								cout<<(string)" " + aviableLft[i].desc<<endl;
								#endif
								fout<<(string)" " + aviableLft[i].desc<<endl;
								lastLFTIndex = i;
							}
							else
							{
								#ifdef DEBUG
								cout<<endl;
								#endif
								fout<<endl;
							}
							line_added = true; 	
							break;				
						}						
					}								
					
					if(!line_added)
						fout<<info_buf<<endl;
					continue;
				}	
				lastLFTIndex = INT_MAX;
 
				
				// BDDA
				if(regexec(&ciregex_brda, info_buf, nmatch, match_arr, 0) == 0)
				{						
					unsigned int line;
					sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
					std::stringstream convert;
					convert << sub;
					convert>>line;
					
					bool line_added = false;
					
					for(unsigned int i = 0; i < aviableLft.size(); ++i) 
					{
						if((aviableLft[i].st <= line) && (line <= aviableLft[i].end))
						{
							#ifdef DEBUG
							cout<<((string)info_buf).substr(0, match_arr[4].rm_so) + marker<<endl;
							#endif							
							fout<<((string)info_buf).substr(0, match_arr[4].rm_so) + marker<<endl;
							line_added = true; 	
							break;				
						}						
					}			
					
					
					if(!line_added)
						fout<<info_buf<<endl;
					continue;
				}	
				
				// end_of_record						
				fout<<info_buf<<endl;
				if(regexec(&cregx_eor, info_buf, nmatch, match_arr, 0) == 0)
					break;
			}
			
			continue;
		}		
		fout<<info_buf<<endl;
	}

	fout.close();
	
	
	#ifdef DEBUG
	for(unsigned int i = 0; i < FN.size(); ++i)
	{
		cout<<"FN: "<<FN[i]<<endl;
		cout<<"FNDA: "<<FNDESC[i]<<endl;
		for(unsigned int j = 0; j < LFT[i].size(); ++j)
		{
			cout<<"ST: "<<LFT[i][j].st<<" END:"<<LFT[i][j].end;
			if(LFT[i][j].desc != "")
				cout <<" LFTDESC: "<<LFT[i][j].desc;
			cout<<endl;
		}
	}
	#endif
		
	return 0;
}


