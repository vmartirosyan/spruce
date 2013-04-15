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

//Enum for different code types. 
enum CODE_TYPE {DEAD_CODE, FREE_CODE};
string dead_marker = "123456789"; // Marker for dead code
string free_marker = "123456788"; // Marker for free code marking

const int info_buf_length = 512;

//Regex for Info and Markup files
const char regx_sf[] = "^SF:(.*)";
const char regx_eor[] = "^end_of_record";

//Regex for DB file
const char regx_fn[] = "^FN:([^, ]+)( (.*))?";
const char regx_lft[] = "^LFT:([0-9]+),([0-9]+)( (.*))?";
const char regx_dead[] = "^DEADCODE:";
const char regx_free[] = "^FREECODE:";

//Regex for Info file 
const char iregex_fn[] = "^FN:([0-9]+),([^,]+)";
const char iregex_fnda[] = "^FNDA:([0-9]+),([^,]+)";
const char iregex_da[] = "^DA:([0-9]+),(-?[0-9]+)(,[^, ]+)?( (.*))?";
const char iregex_brda[] = "^BRDA:([0-9]+),([0-9]+),([0-9]+),([0-9]+|-)";

const char * regxs[] = {regx_sf, regx_eor, /**/ regx_fn, regx_lft, regx_dead, regx_free, /**/iregex_fn, iregex_fnda, iregex_da, iregex_brda};


regex_t cregx_sf;
regex_t cregx_eor;

//Regex for DB file
regex_t cregx_fn;
regex_t cregx_lft;
regex_t cregx_dead;
regex_t cregx_free;

//COmpiled regex for Info file 
regex_t ciregex_fn;
regex_t ciregex_fnda;
regex_t ciregex_da;
regex_t ciregex_brda;	

regex_t * cregxs[] = {&cregx_sf, &cregx_eor, /**/ &cregx_fn, &cregx_lft, &cregx_dead, &cregx_free, /**/&ciregex_fn, &ciregex_fnda, &ciregex_da, &ciregex_brda};


class LFT_ST 
{
	public: 
	unsigned int st;
	unsigned int end;
	string desc;
	CODE_TYPE type;
	
	string getMarker()
	{
		if(type == FREE_CODE)
			return free_marker;
		return dead_marker;
	}
};

class Function
{
	public: 
	string SF; // Sounre files
	string FN; // Function name 
	string FNDESC;// Function description
	vector<LFT_ST> LFT;
	CODE_TYPE type;

	string getMarker()
	{
		if(type == FREE_CODE)
			return free_marker;
		return dead_marker;
	}
};

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
	for(int i = 0; i < 10; ++i)
	{
		if ( (r = regcomp(cregxs[i], regxs[i], REG_EXTENDED)) == true) 
		{
			char errbuf[1024];
			regerror(r, cregxs[i], errbuf, sizeof(errbuf));
			cerr<<"Regular expressions compilation error! Err = "<<errbuf<<endl;
			return 1;
		}
	}
	
	vector<Function> fns;
	unsigned int fin = 0;
	
	
	const int nmatch = 10;
	regmatch_t match_arr[nmatch];
	

	char info_buf[info_buf_length];
	vector<LFT_ST> cur_LFT; 

	// Default values for code type
	CODE_TYPE mode = DEAD_CODE;


	while(!db.eof())
	{
		db.getline(info_buf, info_buf_length);
		memset(match_arr, 0, sizeof(match_arr));
		
		//DEADCODE
		if(regexec(&cregx_dead, info_buf, nmatch, match_arr, 0) == 0)
		{	
			mode = DEAD_CODE;
			continue;
		}
		
		//FREECODE
		if(regexec(&cregx_free, info_buf, nmatch, match_arr, 0) == 0)
		{	
			mode = FREE_CODE;
			continue;
		}	
		
		// Source file
		if(regexec(&cregx_sf, info_buf, nmatch, match_arr, 0) == 0)
		{		
			string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
			
			Function fn;
			fn.SF = sub;
			fn.type = mode;
			fn.FN = "";

			while(!db.eof())
			{
				db.getline(info_buf, info_buf_length);
				memset(match_arr, 0, sizeof(match_arr));
				// FN:
				if(regexec(&cregx_fn, info_buf, nmatch, match_arr, 0) == 0)
				{
					string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
					fn.FN = sub;	

					if(match_arr[2].rm_so != -1) // description
					{
						sub = ((string)info_buf).substr(match_arr[2].rm_so + 1, match_arr[2].rm_eo - match_arr[2].rm_so);	
						fn.FNDESC = sub;
					}
					else
					{
						fn.FNDESC = ""; 
					}
					continue;
				}
				// LFT:
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
					lft.type = mode;
					fn.LFT.push_back(lft);
					continue;
				}
				// end_of_record
				if(regexec(&cregx_eor, info_buf, nmatch, match_arr, 0) == 0)
				{
					fns.push_back(fn);
					fin++;
					break;
				}
			}
		}
	
	}
	
	
	while(!info.eof())
	{
		info.getline(info_buf, info_buf_length);
		memset(match_arr, 0, sizeof(match_arr));	
		
		if(regexec(&cregx_sf, info_buf, nmatch, match_arr, 0) == 0)
		{		
			string currSF = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);
			bool notour = true;
			
			for(unsigned int i = 0; i < fns.size(); ++i)
				if(currSF.find(fns[i].SF) != string::npos)
					notour = false; // File is our
			
			fout<<info_buf<<endl;
			
			if(notour)
			{				
				while(!info.eof())
				{
					info.getline(info_buf, info_buf_length);
					fout<<info_buf<<endl;
					if(regexec(&cregx_eor, info_buf, nmatch, match_arr, 0) == 0)  // end_of_record
						break;
				}
				continue;
			}
			
			#ifdef DEBUG
			cout<<endl<<endl;
			#endif
			cout<<"Processing file: "<<info_buf<<endl;
			
			vector<LFT_ST>aviableLft; 
			
			//Add absolute position marckup to aviableLft
			for(unsigned int i = 0; i < fns.size(); ++i)
			{
				if(currSF.find(fns[i].SF) != string::npos)
				{
					if(fns[i].FN == "") // if marckup is absolute
					{
						for(unsigned int j = 0; j < fns[i].LFT.size(); ++j)
						{
							aviableLft.push_back(fns[i].LFT[j]);
						}
					}
				}
			}
			
			while(!info.eof())
			{
				info.getline(info_buf, info_buf_length);
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
					
					for(unsigned int i = 0; i < fns.size(); ++i)
						if(fns[i].FN == sub) // Function found
						{
							
							unsigned int fnst;
							sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so);	
							std::stringstream convert;
							convert << sub;
							convert>>fnst;
							
							if(fns[i].LFT.size())
							{
								for(unsigned int j = 0; j < fns[i].LFT.size(); ++j)
								{
									aviableLft.push_back(fns[i].LFT[j]);
									aviableLft.back().st += fnst; 	// Correct lines start and end
									aviableLft.back().end += fnst;
								}
							}
							else
							{
								// Current functions have no LFT record, creating new fictitious LFT 
								LFT_ST lft;
								lft.desc = fns[i].FNDESC;
								lft.type = fns[i].type;
								lft.st = fnst;
								
								nextfunction:
								//Keep current file position
								int fpos = info.tellg();	
								
								//Find next function start line 						
								info.getline(info_buf, info_buf_length);
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
							cout<<"FN line "<<fnst<<" FN name: "<<fns[i].FN<<endl;						
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
					
					for(unsigned int i = 0; i < fns.size(); ++i)
						if(sub.compare(fns[i].FN) == 0)
						{
							
							int cov;
							sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
							std::stringstream convert;
							convert << sub;
							convert>>cov;
							
							if(fns[i].type == DEAD_CODE)
							{
								
								if(cov == 0) 	
								{
									#ifdef DEBUG
									cout<<(string)"FNDA:" + fns[i].getMarker() + (string)"," + fns[i].FN<<endl;
									#endif
									fout<<(string)"FNDA:" + fns[i].getMarker() + (string)"," + fns[i].FN<<endl; 
								}
								else
								{
									#ifdef DEBUG
									cout<<info_buf<<endl;
									#endif
									fout<<info_buf<<endl;
								}
							}
							else
								fout<<info_buf<<endl;
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
					string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
					std::stringstream convert;
					convert << sub;
					convert>>line;
					
					bool line_added = false;
					
					for(unsigned int i = 0; i < aviableLft.size(); ++i) 
					{
						if((aviableLft[i].st <= line) && (line <= aviableLft[i].end))
						{
							#ifdef DEBUG
							//cout<<(string)"DA:" + sub + (string)"," + aviableLft[i].getMarker();
							#endif
							fout<<(string)"DA:" + sub + (string)"," + aviableLft[i].getMarker();
							if((lastLFTIndex != i) && (aviableLft[i].desc != ""))
							{
								#ifdef DEBUG
								//cout<<(string)" " + aviableLft[i].desc<<endl;
								#endif
								fout<<(string)" " + aviableLft[i].desc<<endl;
								lastLFTIndex = i;
							}
							else
							{
								#ifdef DEBUG
								//cout<<endl;
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
					string sub = ((string)info_buf).substr(match_arr[1].rm_so, match_arr[1].rm_eo - match_arr[1].rm_so); // get coverage count	
					std::stringstream convert;
					convert << sub;
					convert>>line;
					
					bool line_added = false;
					
					for(unsigned int i = 0; i < aviableLft.size(); ++i) 
					{
						if((aviableLft[i].st <= line) && (line <= aviableLft[i].end) && (aviableLft[i].type == DEAD_CODE))
						{
							#ifdef DEBUG
							//cout<<((string)info_buf).substr(0, match_arr[4].rm_so) + aviableLft[i].getMarker()<<endl;
							#endif							
							fout<<((string)info_buf).substr(0, match_arr[4].rm_so) + aviableLft[i].getMarker()<<endl;
							line_added = true; 	
							break;				
						}
/*						else
							fout<<info_buf<<endl;	
						line_added = true; 					*/
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
	cout<<endl<<endl<<"Marker file info"<<endl;
	for(unsigned int i = 0; i < fns.size(); ++i)
	{
		cout<<"FN: "<<fns[i].FN<<endl;
		cout<<"FNDA: "<<fns[i].FNDESC<<" Code type "<<fns[i].getMarker()<< endl;
		for(unsigned int j = 0; j < fns[i].LFT.size(); ++j)
		{
			cout<<"ST: "<<fns[i].LFT[j].st<<" END:"<<fns[i].LFT[j].end;
			if(fns[i].LFT[j].desc != "")
				cout <<" LFTDESC: "<<fns[i].LFT[j].desc<<endl;
			cout<<"Code type "<<fns[i].LFT[j].getMarker()<<endl;
			cout<<endl;
		}
	}
	#endif
		
	return 0;
}


