/*Author Name: Siddharth Singh */
/*Version 7*/
/*Readpolicy 1: LFU Readpolicy 0: LRU*/
/*Writepolicy 1: WTNA Writepolicy 0: WBWA*/
/*Supports Valid bit runs for all versions except LFU */

#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <bitset>
#include <iomanip>
#include <stdio.h>



// HAVE TO CORRECT ALL DECLARATIONS IMPORTANT FOR ASSOC
using namespace std;

// Print Initializer

void startprint(int blocksize,int totalsize, int assoc, int victim, int l2_size, int l2_assoc, string name){
 cout << "===== Simulator configuration =====" << endl;
 cout << "BLOCKSIZE:                    " << blocksize << endl;
cout << "L1_SIZE:                      " << totalsize << endl;
cout << "L1_ASSOC:                     " << assoc << endl;
cout << "Victim_Cache_SIZE:            " << victim << endl;
cout << "L2_SIZE:                      " << l2_size << endl;
cout << "L2_ASSOC:                     " << l2_assoc << endl;
cout << "trace_file:                   " << name << endl;
cout << "===================================" << endl;
}

/* Function for converting Decimal to Binary String*/

string DecToBin2(int number)
{
    string result = "";

    do
    {
        if ( (number & 1) == 0 )
            result += "0";
        else
            result += "1";

        number >>= 1;
    } while ( number );

    reverse(result.begin(), result.end());
    return result;
}

/* Splitting Address in Tag, Index and Block Offset Bits*/

void splitaddress(string mainbits,string& tagbits,string& setbits,string& blockbits,int tag,int sets,int blksize){
	blockbits = mainbits.substr((tag+sets),blksize);
	setbits = mainbits.substr(tag,sets);
	tagbits = mainbits.substr(0,tag);
}


string datamem[10000];

class CACHE
{
private:
	int l1_read;
	int l1_read_miss;
	int l1_write;
	int l1_write_miss;
	int l1_missrate;
	int l1_numwb;
	int l1_memtraffic;
	double l1_average;
	CACHE* l1_nextlevel;
	string *l1_tagbits;
	//string *dirtybits;
	string vc_tagbits;
	string vc_dummy;
	int *vc_blockcount;
	int *vc_dbit;
	int *vc_validbit;
	int vc_assoc;
	int swaps;
	int vc_wb;
	int vc_tag;
	int vc_dirtystore;
	string vc_temp;
	int **l1_blockcount;
	int **l1_validbit;
	int **l1_dbit;
	int l2_read;
	int l2_read_miss;
	int l2_write;
	int l2_write_miss;
	int l2_missrate;
	int l2_numwb;
	int l2_memtraffic;
	double l2_average;
	CACHE* l2_nextlevel;
	string *l2_tagbits;
	int **l2_blockcount;
	int **l2_validbit;
	int **l2_dbit;
public:
	CACHE (int tag, int set_row,int sets, int blksize, int assoc,int l2_tag, int l2_set_row,int l2_sets, int l2_assoc,int vc_size,int blocksize);
	void l1_vcreadFromAddress(string mainbits,int l1_tag,int l1_sets,int origblocksize, int blksize, int repolicy,int l1_assoc,int l2_size,int l2_assoc,int vc_size,int init);
	void l1_vcwriteToAddress(string mainbits,int l1_tag,int l1_sets,int origblocksize,int blksize,int writepolicy,int repolicy,int l1_assoc,int l2_size,int l2_assoc,int vc_size,int init);
	void l2_readFromAddress(string mainbits,int tag,int sets,int blksize,int repolicy,int assoc);
	void l2_writeToAddress(string tagbits,int tag,int sets,int blksize,int writepolicy,int repolicy,int assoc);
	void l1_checkprint(int l1_tag,int sets,int assoc,int totalsize, int blocksize);
	void vc_checkprint(int size, int blocksize);
	void simulate_checkprint(int victim, int l2_size,int l2_assoc,int blocksize,int totalsize,int assoc);
	void l2_checkprint(int l2_tag,int sets,int assoc,int totalsize, int blocksize);
	void l1_readFromAddress(string mainbits,int l1_tag,int l1_sets,int origblocksize, int blksize, int repolicy,int l1_assoc,int l2_size,int l2_assoc);
	void l1_writeToAddress(string mainbits,int l1_tag,int l1_sets,int origblocksize,int blksize,int writepolicy,int repolicy,int l1_assoc,int l2_size,int l2_assoc);
};

void CACHE::l1_checkprint(int l1_tag,int sets,int assoc,int totalsize,int blocksize){
	int i;
	double dub;
	dub = totalsize/(blocksize*assoc);
	int k= dub;
 int index =0;
	int j,l,m;
 string dumb1;
 index =0;
l = 0;
 cout << "===== L1 contents =====" << endl;
 for(j=0;j<k;j++){
	cout << "set " << j << ": ";
 for(i=0;i<assoc;i++){
	for(m=0;m<assoc;m++){
		if(i == l1_blockcount[j][m]){		
			index = m*l1_tag;			
			dumb1 = l1_tagbits[j].substr(index,l1_tag);
			int check1 = strtol(dumb1.c_str(),0,2);
			printf("%X ",check1);
			if(l1_dbit[j][m])
				cout << "D ";
			else
				cout << "  ";				
		}	
 	}
 }
  cout << endl;
 }
}

void CACHE::l2_checkprint(int l2_tag,int sets,int assoc,int totalsize,int blocksize){
int i;
double dub;
dub = totalsize/(blocksize*assoc);
int k= dub;
 int index =0;
	int j,l,m;
 string dumb1;
 index =0;
l = 0;
 cout << "===== L2 contents =====" << endl;
 for(j=0;j<k;j++){
	cout << "set " << j << ": ";
 for(i=0;i<assoc;i++){
	for(m=0;m<assoc;m++){
		if(i == l2_blockcount[j][m]){		
			index = m*l2_tag;			
			dumb1 = l2_tagbits[j].substr(index,l2_tag);
			int check1 = strtol(dumb1.c_str(),0,2);
			printf("%X ",check1);
			if(l2_dbit[j][m])
				cout << "D ";
			else
				cout << "  ";				
		}	
 	}
 }
  cout << endl;
 }
}

void CACHE::vc_checkprint(int size, int blocksize){
	string dumb1;
	cout << "===== Victim Cache contents =====" << endl;
	int index,i,m;	
	cout << "set " << 0 << ": ";
   for(i=0;i<vc_assoc;i++){
	for(m=0;m<vc_assoc;m++){
		if(i == vc_blockcount[m]){		
			index = m*vc_tag;			
			dumb1 = vc_tagbits.substr(index,vc_tag);
			int check1 = strtol(dumb1.c_str(),0,2);
			printf("%X ",check1);
			if(vc_dbit[m])
				cout << "D ";
			else
				cout << "  ";				
		}	
 	}
  }
  cout <<  endl ;
}

void CACHE::simulate_checkprint(int victim, int l2_size,int l2_assoc,int blocksize,int totalsize,int assoc){
	
	double miss_penalty1 = (double)(20 + (double)(0.5*(double)(blocksize/16)));
	double hit_time1 = (double)(0.25 + (double)(2.5 * (double)(totalsize/(double)(512*1024))) + (double)(0.025*assoc) + (double)(0.025*(double)(blocksize/16))); 
	double miss_rate1 = ((double)(l1_read_miss + l1_write_miss)/(double)(l1_read + l1_write));
	l1_average = (double)(hit_time1 + (double)(miss_penalty1 * miss_rate1));
	double miss_penalty2 = (double)(20 + (double)(0.5*(double)(blocksize/16)));
	double hit_time2 = (double)(2.5 + (double)(2.5 * (double)(l2_size/(double)(512*1024))) + (double)(0.025*l2_assoc) + (double)(0.025*(double)(blocksize/16))); 
	double miss_rate2;
		int miss2 = 0;
	if(l2_size != 0){		
		miss_rate2 = ((double)(l2_read_miss)/(double)(l2_read));
	}
	else{
			miss_rate2 = 0;
	}
	if(l2_size != 0){
		l2_average = (double)((double)hit_time1 + (double)(miss_rate1*(double)(hit_time2 + (double)(miss_penalty2 * miss_rate2))));
	}
	else{
		l2_average = (double)(hit_time1 + (double)(miss_penalty1 * miss_rate1));
	}
	if(victim != 0){
		if(l2_size == 0){
			l2_memtraffic = l1_read_miss + l1_write_miss + vc_wb;
		}
		else{
			l2_memtraffic = l2_read_miss + l2_write_miss + l2_numwb;		
		}
	}
	else{
		l2_memtraffic = l2_read_miss + l2_write_miss + l2_numwb;
	}
cout << "====== Simulation results (raw) ======" << endl;
cout << "a. number of L1 reads:                " << l1_read << endl;
cout << "b. number of L1 read misses:          " << l1_read_miss << endl;
cout << "c. number of L1 writes:		     " << l1_write << endl;
cout << "d. number of L1 write misses:         " << l1_write_miss << endl;
cout << "e. L1 miss rate:                      " << setprecision(4) << fixed << miss_rate1 << endl;
cout << "f. number of swaps:                   " << swaps << endl;
cout << "g. number of victim cache writeback:  " << vc_wb << endl;
cout << "h. number of L2 reads:                " << l2_read << endl;
cout << "i. number of L2 read misses:          " << l2_read_miss << endl;
cout << "j. number of L2 writes:               " << l2_write << endl;
cout << "k. number of L2 write misses:         " << l2_write_miss << endl;
if(miss_rate2)
	cout << "l. L2 miss rate:                      " << setprecision(4) << fixed << miss_rate2 << endl;
else
	cout << "l. L2 miss rate:                      " << miss2 << endl;
cout << "m. number of L2 writebacks:           " << l2_numwb << endl;
cout << "n. total memory traffic:              " << l2_memtraffic << endl;
cout << "==== Simulation results (performance) ====" << endl;
cout << "1. average access time:         " << setprecision(4) << fixed << l2_average << " " << "ns" << endl;
}

/* CACHE Constructor */

CACHE::CACHE (int tag, int set_row,int sets, int blksize, int assoc,int l2_tag, int l2_set_row,int l2_sets, int l2_assoc,int vc_size,int blocksize){
	int i,j;
	l1_read = 0;
	l1_read_miss = 0;
	l1_average = 0.0;
	l1_write = 0;
	l1_write_miss = 0;
	l1_missrate = 0;
	l1_memtraffic = 0;
	l1_numwb = 0;
	vc_dirtystore = 0;
	swaps = 0;
	vc_wb = 0;
	l1_nextlevel = NULL;
	l1_tagbits = new string [set_row];
	string dummy,temp;
	for(i=0;i<tag;i++)
		dummy.push_back('0');
	for(i=0;i<set_row;i++){
		temp = "";
		for(j=0;j<assoc;j++){
			temp.append(dummy);
		}
		l1_tagbits[i].append(temp);
	}
	l1_validbit = new int* [set_row];
	for(i=0;i<set_row;i++){
		l1_validbit[i] = new int [assoc]; 
	}	
	for(i=0;i<set_row;i++){
		for(j=0;j<assoc;j++){
			l1_validbit[i][j] = 0;
		}
	}
	l1_blockcount = new int* [set_row];
	for(i=0;i<set_row;i++){
		l1_blockcount[i] = new int [assoc]; 
	}
	for(i=0;i<set_row;i++){
		for(j=0;j<assoc;j++){
			l1_blockcount[i][j] = (assoc-j-1);
		}
	}	
	l1_dbit = new int* [set_row];
	for(i=0;i<set_row;i++){
		l1_dbit[i] = new int [assoc];
	}	
	for(i=0;i<set_row;i++){
		for(j=0;j<assoc;j++){
			l1_dbit[i][j] = 0;
		}
	}
	l2_read = 0;
	l2_read_miss = 0;
	l2_average = 0.0;
	l2_write = 0;
	l2_write_miss = 0;
	l2_missrate = 0;
	l2_memtraffic = 0;
	l2_numwb = 0;
	l2_nextlevel = NULL;
	string dummy2;
	int vc_total;
	double jason; 
	vc_assoc = vc_size/blocksize;
	vc_tag = 32 - blksize;
	//cout << vc_tag << endl;
	//cout << vc_assoc << " VC assoc " << vc_size << " VC _ SIZE " << vc_tag << " vc tag " << jason << " block bits " << endl;
	vc_total = vc_tag * vc_assoc; // WRONG
	for(i=0;i<vc_total;i++)
		dummy2.push_back('0');
	vc_tagbits.append(dummy2);
	//cout << vc_tagbits.length() << endl;
	for(i=0;i<vc_tag;i++)
		dummy2.push_back('0');
	vc_temp.append(dummy2);
	for(i=0;i<vc_tag;i++)
		dummy2.push_back('0');
	vc_dummy.append(dummy2);
	//cout << " LOLA POLA " << vc_assoc << endl;
	vc_blockcount = new int [vc_assoc];
	vc_dbit = new int [vc_assoc];
	vc_validbit = new int [vc_assoc];
	for(i=0;i<vc_assoc;i++)
		vc_blockcount[i] = vc_assoc-i-1;
	
	for(i=0;i<vc_assoc;i++)
		vc_dbit[i] = 0;	
	for(i=0;i<vc_assoc;i++)
		vc_validbit[i] = 0;
	l2_tagbits = new string [l2_set_row];
	//string dummy,temp;
	for(i=0;i<l2_tag;i++)
		dummy.push_back('0');
	for(i=0;i<l2_set_row;i++){
		temp = "";
		for(j=0;j<l2_assoc;j++){
			temp.append(dummy);
		}
		l2_tagbits[i].append(temp);
	}
	l2_validbit = new int* [l2_set_row];
	for(i=0;i<l2_set_row;i++){
		l2_validbit[i] = new int [l2_assoc]; 
	}	
	for(i=0;i<l2_set_row;i++){
		for(j=0;j<l2_assoc;j++){
			l2_validbit[i][j] = 0;
		}
	}
	l2_blockcount = new int* [l2_set_row];
	for(i=0;i<l2_set_row;i++){
		l2_blockcount[i] = new int [l2_assoc]; 
	}
	for(i=0;i<l2_set_row;i++){
		for(j=0;j<l2_assoc;j++){
			l2_blockcount[i][j] = (l2_assoc-j-1);
		}
	}	
	l2_dbit = new int* [l2_set_row];
	for(i=0;i<l2_set_row;i++){
		l2_dbit[i] = new int [l2_assoc];
	}	
	for(i=0;i<l2_set_row;i++){
		for(j=0;j<l2_assoc;j++){
			l2_dbit[i][j] = 0;
		}
	}
	
}




/*Read Function of Class handles LRU and LFU*/
//HAVE TO ADD VARIABLE G from Specs
// DO GENERALIZE FOR REPOLICY ND SET UP VALID BITS REMOVE BLOCKCOUNT ND CHECK AGAIN
void CACHE::l1_readFromAddress(string mainbits,int l1_tag,int l1_sets, int origblk,int blksize, int repolicy,int l1_assoc,int l2_size,int l2_assoc){
	l1_read ++;
	string setbits,tgbits,blockbits;
	splitaddress(mainbits,tgbits,setbits,blockbits,l1_tag,l1_sets,blksize);
	int row_num;
	row_num = strtoull(setbits.c_str(),NULL,2);
	int address_len = 32;	
	double set_bit = l2_size/(origblk*l2_assoc);
	int set_row = set_bit;
	double sets = log(set_bit)/log(2);
	int setnum = sets; //Set Bits
	//cout << "BOND IS HERE " << set_bit << endl;
	//cout << "BOND IS HERE " << sets << endl;
	//cout << "BOND IS HERE " << setnum << endl;
	int tag = address_len - setnum - blksize; // MAIN processing starts from Next line have to iterate over it
	int tagnum = tag; //Tag Size
	//cout << l1_assoc << "  **" << l1_tag << " ** " << l1_sets << endl;
	if(repolicy == 0){
		if(l1_assoc == 1){
			if((tgbits.compare(l1_tagbits[row_num]) == 0) && (l1_validbit[row_num][(l1_assoc-1)] == 1)){
				//cout << "hit" << endl;
			}
			else if(l1_validbit[row_num][(l1_assoc-1)] == 0){
				l1_read_miss ++; 
				//cout << "do u ??" << endl;
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 0;				
				
			}
			else if(tgbits.compare(l1_tagbits[row_num]) != 0){
				l1_read_miss++;
				//cout << "do u ??" << endl;
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 0;			
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			string dummy;
			int point=0;
			bool flag = false;
			for(i=0;i<l1_assoc;i++){
				dummy = l1_tagbits[row_num].substr(index,l1_tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+l1_tag;
			}	
			if((hit == 0) && (l1_validbit[row_num][count]==1)){
				int temp = l1_blockcount[row_num][count];	
				for(i=0;i<l1_assoc;i++){
					if(l1_blockcount[row_num][i] < temp){
						l1_blockcount[row_num][i] += 1;
					}
				}
				l1_blockcount[row_num][count] = 0;
			}
			else {				
				for(i=0;i<l1_assoc;i++){
					if(l1_validbit[row_num][i] == 0){
						flag = true;
						point = i;
						break;
					}
				}
				if(flag == false){	
						l1_read_miss++;							
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}
						//cout << "Zone 1" << endl;
						if(l1_dbit[row_num][count]==1){ //Replace Here for Early Matching 2 nd index (assoc-1)
							string getdirty,dirty2;
							int l2_begin = count*l1_tag;
							//cout << "007  ** "<< count << "**" << l2_begin << endl;
							//cout << l1_tag;
							//cout << l2_begin<<endl;
							getdirty = l1_tagbits[row_num].substr(l2_begin,l1_tag);
							dirty2 = getdirty + setbits;
							string rand1 = dirty2.substr(0,tagnum);
							string rand2 = dirty2.substr(tagnum,setnum);
								//cout << dirty2 << endl;
								//cout << rand1 <<endl;
								//cout << rand2 << endl;
							l2_writeToAddress(dirty2,tagnum,setnum,blksize,0,0,l2_assoc);
							l1_numwb++;	
						}
						l1_dbit[row_num][count] = 0;
						int start = count*l1_tag;
						l1_tagbits[row_num].replace(start,l1_tag,tgbits);
						//cout << tagnum << "*" << setnum << "*" <<  blksize << "*" << endl;
						//cout << mainbits << "*" << l2_assoc << endl;
						l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
				}	
				else{
						l1_read_miss++;						
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}					
						l1_validbit[row_num][count] = 1;
						if(l1_dbit[row_num][count]==1){ //SAME HERE
							string getdirty1,dirty3;
							int l2_begin = count*l1_tag;
							getdirty1 = l1_tagbits[row_num].substr(l2_begin,l1_tag);
							dirty3 = getdirty1 + setbits;
							//cout << dirty3 << endl;
							l2_writeToAddress(dirty3,tagnum,setnum,blksize,0,0,l2_assoc);						
							l1_numwb++;
						}
						l1_dbit[row_num][count] = 0;
						int begin = count*l1_tag;
						l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
						//cout << tagnum << "*" << setnum << "*" <<  blksize << "*" << endl;
						//cout << mainbits << "*" << l2_assoc << endl;
						l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
						//cout << "INVALID ZONE" << endl;
				}
			}
		}
	}	
}

void CACHE::l1_writeToAddress(string mainbits,int l1_tag,int l1_sets,int origblk,int blksize,int writepolicy,int repolicy,int l1_assoc,int l2_size,int l2_assoc){
	l1_write++;
	string setbits,tgbits,blockbits;
	int hexa;
	splitaddress(mainbits,tgbits,setbits,blockbits,l1_tag,l1_sets,blksize);
	int row_num;
	row_num = strtoull(setbits.c_str(),NULL,2);
	int address_len = 32;	
	double set_bit = l2_size/(origblk*l2_assoc);
	double sets = log(set_bit)/log(2);
	int setnum = sets; //Set Bits
	int tag = address_len - setnum - blksize; // MAIN processing starts from Next line have to iterate over it
	int tagnum = tag; //Tag Size
	int set_row = set_bit;
	if(repolicy == 0){
		if(writepolicy == 0){
		 if(l1_assoc == 1){
			if((tgbits.compare(l1_tagbits[row_num]) == 0) && (l1_validbit[row_num][(l1_assoc-1)] == 1)){
				l1_dbit[row_num][(l1_assoc-1)] = 1;
			}
			else if(l1_validbit[row_num][(l1_assoc-1)] == 0){
				l1_write_miss++;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 1;
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				// HAVE TO SETUP WRITE BACK UPDATE HERE TOO DEPENDING UPON WRITE POLICY
			}
			else if(tgbits.compare(l1_tagbits[row_num]) != 0){
				l1_write_miss++;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 1;
				l1_tagbits[row_num] = tgbits;
				//cout << tagbits[row_num] << endl;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			string dummy;
			int point=0;
			bool flag = false;
			for(i=0;i<l1_assoc;i++){
				dummy = l1_tagbits[row_num].substr(index,l1_tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+l1_tag;
			}	
			if((hit == 0) && (l1_validbit[row_num][count]==1)){
				int temp = l1_blockcount[row_num][count];	
				for(i=0;i<l1_assoc;i++){
					if(l1_blockcount[row_num][i] < temp){
						l1_blockcount[row_num][i] += 1;
					}
				}
				l1_blockcount[row_num][count] = 0;
				l1_dbit[row_num][count] = 1;
			}
			else {				
				for(i=0;i<l1_assoc;i++){
					if(l1_validbit[row_num][i] == 0){
						flag = true;
						point = i;
						break;
					}
				}
				if(flag == false){	
						l1_write_miss++;							
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}
						//cout << "Zone 1" << endl;
						if(l1_dbit[row_num][count]==1){ //DOG TRAILS
							string getdirty,dirty2;
							int l2_begin = count*l1_tag;
							getdirty = l1_tagbits[row_num].substr(l2_begin,l1_tag);
							dirty2 = getdirty + setbits;
							//cout << dirty2 << endl;
							l2_writeToAddress(dirty2,tagnum,setnum,blksize,0,0,l2_assoc);							
							l1_numwb++;
						}
						l1_dbit[row_num][count] = 1;
						int start = count*l1_tag;
						l1_tagbits[row_num].replace(start,l1_tag,tgbits);
						//cout << tagnum << "*" << setnum << "*" <<  blksize << "*" << endl;
						//cout << mainbits << "*" << l2_assoc << endl;
						l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
				}	
				else{
						l1_write_miss++;						
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}					
						l1_validbit[row_num][count] = 1;
						if(l1_dbit[row_num][count]==1){ //DOG TRAILS
							string getdirty1,dirty3;
							int l2_begin = count*l1_tag;
							getdirty1 = l1_tagbits[row_num].substr(l2_begin,l1_tag);
							dirty3 = getdirty1+setbits;
								//cout << dirty3 << endl;
							l2_writeToAddress(dirty3,tagnum,setnum,blksize,0,0,l2_assoc);						
							l1_numwb++;
						}
						l1_dbit[row_num][count] = 1;
						int begin = count*l1_tag;
						l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
						//cout << "INVALID ZONE" << endl;
						//cout << tagnum << "*" << setnum << "*" <<  blksize << "*" << endl;
						//cout << mainbits << "*" << l2_assoc << endl;
						l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
				}
			}
		}
	}
    }
}


void CACHE::l1_vcreadFromAddress(string mainbits,int l1_tag,int l1_sets, int origblk,int blksize, int repolicy,int l1_assoc,int l2_size,int l2_assoc,int vc_size,int init){
	l1_read ++;
	int check1;
	string setbits,tgbits,blockbits;
	vc_temp = mainbits.substr(0,vc_tag);
	//cout << vc_tag << endl;
	//cout << "string for vc " << vc_temp << " main address " << mainbits << endl;
	//cout << vc_temp << "  " << mainbits << endl;
	splitaddress(mainbits,tgbits,setbits,blockbits,l1_tag,l1_sets,blksize);
	check1 = strtol(mainbits.c_str(),0,2);
	init++;
	//cout << init << ". Address ";
	//printf("%X",check1);	
	//cout << " READ" << endl;  
	int row_num;
	row_num = strtoull(setbits.c_str(),NULL,2);	
	int address_len = 32;	  //L2 VARIABLES
	double set_bit;
	if(l2_size != 0){
		set_bit = l2_size/(origblk*l2_assoc);
	}
	int set_row = set_bit;
	double sets;
	if(l2_size != 0){
		sets = log(set_bit)/log(2);
	}
	int setnum = sets; //Set Bits
	int tag = address_len - setnum - blksize; // MAIN processing starts from Next line have to iterate over it
	int tagnum = tag; //Tag Size
	if(repolicy == 0){
		if(l1_assoc == 1){
			if((tgbits.compare(l1_tagbits[row_num]) == 0) && (l1_validbit[row_num][(l1_assoc-1)] == 1)){
				//cout << "hit" << endl;
			}
			else if(l1_validbit[row_num][(l1_assoc-1)] == 0){
				l1_read_miss ++; 
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 0;				
				
			}
			else if(tgbits.compare(l1_tagbits[row_num]) != 0){
				l1_read_miss++;
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 0;			
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			string dummy;
			int point=0;
			bool flag = true;
			for(i=0;i<l1_assoc;i++){
				dummy = l1_tagbits[row_num].substr(index,l1_tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+l1_tag;
			}	
			if((hit == 0) && (l1_validbit[row_num][count]==1)){
				int temp = l1_blockcount[row_num][count];
				//cout << "HIT L1" << endl;
				//cout << endl;	
				for(i=0;i<l1_assoc;i++){
					if(l1_blockcount[row_num][i] < temp){
						l1_blockcount[row_num][i] += 1;
					}
				}
				l1_blockcount[row_num][count] = 0;
			}
			else {				
				for(i=0;i<l1_assoc;i++){
					if(l1_validbit[row_num][i] == 0){
						flag = false;
						break;
					}
				}
				if(flag == false){	// No Changes here its L1 Invalid MISS
						l1_read_miss++;	
						//cout << "MISS L1 INVALID" << endl;
						//cout << "MISS VICTIM CACHE" << endl;
						//cout << endl;	
						//cout << "Zone 1" << endl;
						max = -100;						
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}
						l1_dbit[row_num][count] = 0;
						l1_validbit[row_num][count] = 1;
						int start = count*l1_tag;
						l1_tagbits[row_num].replace(start,l1_tag,tgbits);
						if(l2_size != 0){
							l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
						}
				}	
				else{
						max = -100;
						count=0;				
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						// DO SAVE A COPY HERE
						string l1_copy1;
						int l1_dcopy,copy1;
						copy1 = count*l1_tag;
						l1_copy1 = l1_tagbits[row_num].substr(copy1,l1_tag); // COPY of TAG
						l1_dcopy = l1_dbit[row_num][count]; // COPY OF DBIT
						//cout << l1_copy1 << " l1 evict" << endl;
						int vc_start=0,hit=-1,loc=0;
						for(i=0;i<vc_assoc;i++){
							vc_dummy = vc_tagbits.substr(vc_start,vc_tag);
							hit = vc_dummy.compare(vc_temp);
							if(hit==0){
								break;
							}
							vc_start = vc_start + vc_tag;
							loc++;
						}
						if((hit == 0) && (vc_validbit[loc] == 1)){
							swaps++;
							int temp = vc_blockcount[loc];	
							for(i=0;i<vc_assoc;i++){
								if(vc_blockcount[i] < temp){
									vc_blockcount[i] ++;
								}
							}
							vc_blockcount[loc] = 0;
							vc_dirtystore = vc_dbit[loc]; //STORE HIT VICTIM DIRTY
							vc_dbit[loc] = l1_dcopy; // STORED L1's DIRTY BIT
							string comb_set;
							comb_set = DecToBin2(row_num); // NEW LOGIC FOR VC 
							int diff = l1_sets - comb_set.length();
							//cout << l1_sets << "  && " << endl;
							for(i=0;i<diff;i++){
								comb_set.insert(0,"0");
							}
							//cout << row_num << "RN ** csbits" << comb_set << endl;
							string l1_temp = l1_copy1 + comb_set;//  L1 STRING FOR LRU COPY IF VC HITS
							//cout << l1_temp << "VC TAG" << endl;
							//cout << "VICTIM HIT EVICT" << endl;
							//cout << "HIT VICTIM CACHE, SWAP" << endl;
							//cout << endl;
							int vc_begin = loc*vc_tag;
							vc_tagbits.replace(vc_begin,vc_tag,l1_temp);
							for(i=0;i<l1_assoc;i++){
								if(i == count){
									l1_blockcount[row_num][i] = 0;
								}
								else{
									l1_blockcount[row_num][i] ++;
								}
							}					
							l1_validbit[row_num][count] = 1;
							l1_dbit[row_num][count] = vc_dirtystore;
							int begin = count*l1_tag;
							l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
						}
						else{
							l1_read_miss++;	
							//swaps++;
							max = -100,loc=0;
							for(i=0;i<vc_assoc;i++){
								if(max < vc_blockcount[i]){
									max = vc_blockcount[i];
									loc = i;
								}
								//cout << vc_blockcount[i];
							}
							for(i=0;i<vc_assoc;i++){
								if(i == loc){
									vc_blockcount[i] = 0;
								}
								else{
									vc_blockcount[i] ++;
								}
							}
							vc_validbit[loc] = 1;						
							if(vc_dbit[loc] == 1){
									//DO WRITE BACK TO L2
									int start = loc*vc_tag;
									vc_temp = vc_tagbits.substr(start,vc_tag);
									if(l2_size != 0){
									 l2_writeToAddress(vc_temp,tagnum,setnum,blksize,0,0,l2_assoc);	
									}					
									vc_wb++;
							}
							vc_dbit[loc] = l1_dcopy; // STORED L1's DIRTY BIT
							string comb_set;
							comb_set = DecToBin2(row_num); // NEW LOGIC FOR VC 
							int diff = l1_sets - comb_set.length();
							for(i=0;i<diff;i++){
								comb_set.insert(0,"0");
							}
							string l1_temp = l1_copy1 + comb_set;//  L1 STRING FOR LRU COPY IF VC HITS
							int vc_begin = loc*vc_tag;
							vc_tagbits.replace(vc_begin,vc_tag,l1_temp);
							for(i=0;i<l1_assoc;i++){
								if(i == count){
									l1_blockcount[row_num][i] = 0;
								}
								else{
									l1_blockcount[row_num][i] ++;
								}
							}					
							l1_validbit[row_num][count] = 1;
							l1_dbit[row_num][count] = 0;
							int begin = count*l1_tag;
							l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
							if(l2_size != 0){
								l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
							}
						}
				}
			}
		}
	}	
}

void CACHE::l2_readFromAddress(string mainbits,int tag,int sets, int blksize, int repolicy,int assoc){
	l2_read++;
	string setbits,tgbits,blockbits;
	splitaddress(mainbits,tgbits,setbits,blockbits,tag,sets,blksize);
	//cout << tgbits << endl;	
	int row_num;
	row_num = strtoull(setbits.c_str(),NULL,2);
	if(repolicy == 0){
		if(assoc == 1){
			if((tgbits.compare(l2_tagbits[row_num]) == 0) && (l2_validbit[row_num][(assoc-1)] == 1)){
				//cout << "hit" << endl;
			}
			else if(l2_validbit[row_num][(assoc-1)] == 0){
				l2_read_miss ++; 
				l2_tagbits[row_num] = tgbits;
				l2_validbit[row_num][(assoc-1)] = 1;
				if(l2_dbit[row_num][(assoc-1)]==1){
					l2_numwb++;
				}
				l2_dbit[row_num][(assoc-1)] = 0;				
				
			}
			else if(tgbits.compare(l2_tagbits[row_num]) != 0){
				l2_read_miss++;
				l2_tagbits[row_num] = tgbits;
				l2_validbit[row_num][(assoc-1)] = 1;
				if(l2_dbit[row_num][(assoc-1)]==1){
					l2_numwb++;
				}
				l2_dbit[row_num][(assoc-1)] = 0;			
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			string dummy;
			int point=0;
			bool flag = false;
			for(i=0;i<assoc;i++){
				dummy = l2_tagbits[row_num].substr(index,tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+tag;
			}	
			if((hit == 0) && (l2_validbit[row_num][count]==1)){
				int temp = l2_blockcount[row_num][count];	
				for(i=0;i<assoc;i++){
					if(l2_blockcount[row_num][i] < temp){
						l2_blockcount[row_num][i] += 1;
					}
				}
				l2_blockcount[row_num][count] = 0;
			}
			else {				
				for(i=0;i<assoc;i++){
					if(l2_validbit[row_num][i] == 0){
						flag = true;
						point = i;
						break;
					}
				}
				if(flag == false){	
						l2_read_miss++;							
						for(i=0;i<assoc;i++){
							if(max < l2_blockcount[row_num][i]){
								max = l2_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<assoc;i++){
							if(i == count){
								l2_blockcount[row_num][i] = 0;
							}
							else{
								l2_blockcount[row_num][i] ++;
							}
						}
						//cout << "Zone 1" << endl;
						if(l2_dbit[row_num][count]==1){ //Replace Here for Early Matching 2 nd index (assoc-1)
							l2_numwb++;
						}
						l2_dbit[row_num][count] = 0;
						int start = count*tag;
						l2_tagbits[row_num].replace(start,tag,tgbits);
				}	
				else{
						l2_read_miss++;						
						for(i=0;i<assoc;i++){
							if(max < l2_blockcount[row_num][i]){
								max = l2_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<assoc;i++){
							if(i == count){
								l2_blockcount[row_num][i] = 0;
							}
							else{
								l2_blockcount[row_num][i] ++;
							}
						}					
						l2_validbit[row_num][count] = 1;
						if(l2_dbit[row_num][count]==1){ //SAME HERE
							l2_numwb++;
						}
						l2_dbit[row_num][count] = 0;
						int begin = count*tag;
						l2_tagbits[row_num].replace(begin,tag,tgbits);
						//cout << "INVALID ZONE" << endl;
				}
			}
		}
	}	
}


void CACHE::l1_vcwriteToAddress(string mainbits,int l1_tag,int l1_sets,int origblk,int blksize,int writepolicy,int repolicy,int l1_assoc,int l2_size,int l2_assoc,int vc_size,int init){
	l1_write++;
	//cout << "Where do I vanish" << endl;
	//cout << mainbits << " " << origblk << " " << vc_size << " " << repolicy << " " << writepolicy << " " << blksize << " " << l1_tag << endl;	
	string setbits,tgbits,blockbits;
	int hexa;
	splitaddress(mainbits,tgbits,setbits,blockbits,l1_tag,l1_sets,blksize);
	int row_num;
	vc_temp = mainbits.substr(0,vc_tag);
	//cout << vc_temp << endl;
	int check1 = strtol(mainbits.c_str(),0,2);
	init ++;
	//cout << init << ". Address ";
	//printf("%X",check1);	
	//cout << " WRITE" << endl; 
	row_num = strtoull(setbits.c_str(),NULL,2);
	int address_len = 32;	//L2 VARIABLES
	double set_bit;
	if(l2_size != 0){
	 set_bit = l2_size/(origblk*l2_assoc);
	}
	double sets;
	if(l2_size != 0){
		sets = log(set_bit)/log(2);
	}
	int setnum = sets; //Set Bits
	int tag = address_len - setnum - blksize; // MAIN processing starts from Next line have to iterate over it
	int tagnum = tag; //Tag Size
	int set_row = set_bit;
	if(repolicy == 0){
		if(writepolicy == 0){
		 if(l1_assoc == 1){
			if((tgbits.compare(l1_tagbits[row_num]) == 0) && (l1_validbit[row_num][(l1_assoc-1)] == 1)){
				l1_dbit[row_num][(l1_assoc-1)] = 1;
			}
			else if(l1_validbit[row_num][(l1_assoc-1)] == 0){
				l1_write_miss++;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 1;
				l1_tagbits[row_num] = tgbits;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
				// HAVE TO SETUP WRITE BACK UPDATE HERE TOO DEPENDING UPON WRITE POLICY
			}
			else if(tgbits.compare(l1_tagbits[row_num]) != 0){
				l1_write_miss++;
				if(l1_dbit[row_num][(l1_assoc-1)]==1){
					l1_numwb++;
				}
				l1_dbit[row_num][(l1_assoc-1)] = 1;
				l1_tagbits[row_num] = tgbits;
				//cout << tagbits[row_num] << endl;
				l1_validbit[row_num][(l1_assoc-1)] = 1;
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			//cout << "Mhere 1 " << endl;			
			string dummy;
			int point=0;
			bool flag = true;
			for(i=0;i<l1_assoc;i++){
				dummy = l1_tagbits[row_num].substr(index,l1_tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+l1_tag;
			}	
			if((hit == 0) && (l1_validbit[row_num][count]==1)){
				int temp = l1_blockcount[row_num][count];	
				for(i=0;i<l1_assoc;i++){
					if(l1_blockcount[row_num][i] < temp){
						l1_blockcount[row_num][i] += 1;
					}
				}
				//cout << "HIT L1" << endl;
				//cout << endl;
				l1_blockcount[row_num][count] = 0;
				l1_dbit[row_num][count] = 1;
			}
			else {				
				for(i=0;i<l1_assoc;i++){
					if(l1_validbit[row_num][i] == 0){
						flag = false;
						break;
					}
				}
				if(flag == false){	
						l1_write_miss++;										
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<l1_assoc;i++){
							if(i == count){
								l1_blockcount[row_num][i] = 0;
							}
							else{
								l1_blockcount[row_num][i] ++;
							}
						}
						//cout << "Zone 2" << endl;
						if(l1_dbit[row_num][count]==1){ //DOG TRAILS
							string getdirty,dirty2;
							int l2_begin = count*l1_tag;
							getdirty = l1_tagbits[row_num].substr(l2_begin,l1_tag);
							dirty2 = getdirty + setbits;
							//cout << dirty2 << endl;
							//l2_writeToAddress(dirty2,tagnum,setnum,blksize,0,0,l2_assoc);							
							l1_numwb++;
						}
						l1_dbit[row_num][count] = 1;
						int start = count*l1_tag;
						l1_tagbits[row_num].replace(start,l1_tag,tgbits);
						l1_validbit[row_num][count] = 1;
						if(l2_size != 0){
						  l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
						}
				}	
				else{
						count = 0,max = -100;						
						for(i=0;i<l1_assoc;i++){
							if(max < l1_blockcount[row_num][i]){
								max = l1_blockcount[row_num][i];
								count = i;
							}
						}
						// DO SAVE A COPY HERE
						string l1_copy1;
						int l1_dcopy,copy1;
						copy1 = count*l1_tag;
						l1_copy1 = l1_tagbits[row_num].substr(copy1,l1_tag); // COPY of TAG
						l1_dcopy = l1_dbit[row_num][count]; // COPY OF DBIT
						int vc_start=0,hit=-1,loc=0;
						for(i=0;i<vc_assoc;i++){
							vc_dummy = vc_tagbits.substr(vc_start,vc_tag);
							hit = vc_dummy.compare(vc_temp);
							if(hit==0){
								break;
							}
							vc_start = vc_start + vc_tag;
							loc++;
						}
						if((hit == 0) && (vc_validbit[loc] == 1)){
							swaps++;
							int temp = vc_blockcount[loc];	
							for(i=0;i<vc_assoc;i++){
								if(vc_blockcount[i] < temp){
									vc_blockcount[i] ++;
								}
							}
							vc_blockcount[loc] = 0;
							vc_dirtystore = vc_dbit[loc]; //STORE HIT VICTIM DIRTY
							vc_dbit[loc] = l1_dcopy; // STORED L1's DIRTY BIT
							string comb_set;
							comb_set = DecToBin2(row_num); // NEW LOGIC FOR VC 
							int diff = l1_sets - comb_set.length();
							for(i=0;i<diff;i++){
								comb_set.insert(0,"0");
							}
							string l1_temp = l1_copy1 + comb_set;//  L1 STRING FOR LRU COPY IF VC HITS
							int vc_begin = loc*vc_tag;
							vc_tagbits.replace(vc_begin,vc_tag,l1_temp);
							for(i=0;i<l1_assoc;i++){
								if(i == count){
									l1_blockcount[row_num][i] = 0;
								}
								else{
									l1_blockcount[row_num][i] ++;
								}
							}					
							l1_validbit[row_num][count] = 1;
							l1_dbit[row_num][count] = 1;
							int begin = count*l1_tag;
							l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
						}
						else{
							l1_write_miss++;
							//swaps++;
							//cout << "MISS L1" << endl;
							//cout << "MISS VICTIM CACHE" << endl;
							//cout << endl;
							//cout << "Mhere 4" << endl;					
							max = -100,loc = 0;
							for(i=0;i<vc_assoc;i++){
								if(max < vc_blockcount[i]){
									max = vc_blockcount[i];
									loc = i;
								}
							}
							for(i=0;i<vc_assoc;i++){
								if(i == loc){
									vc_blockcount[i] = 0;
								}
								else{
									vc_blockcount[i] ++;
								}
							}
							vc_validbit[loc] = 1;						
							//int temp = vc_blockcount[loc];	// ?? WHY THIS VARIABLE
							//vc_dirtystore = vc_dbit[loc]; //STORE HIT VICTIM DIRTY
							if(vc_dbit[loc] == 1){
									//DO WRITE BACK TO L2
									int start = loc*vc_tag;
									vc_temp = vc_tagbits.substr(start,vc_tag);
									if(l2_size != 0){
									  l2_writeToAddress(vc_temp,tagnum,setnum,blksize,0,0,l2_assoc);
									}					
									vc_wb++;
							}
							vc_dbit[loc] = l1_dcopy; // STORED L1's DIRTY BIT
							string comb_set;
							comb_set = DecToBin2(row_num); // NEW LOGIC FOR VC 
							int diff = l1_sets - comb_set.length();
							for(i=0;i<diff;i++){
								comb_set.insert(0,"0");
							}
							string l1_temp = l1_copy1 + comb_set;//  L1 STRING FOR LRU COPY IF VC HITS
							int vc_begin = loc*vc_tag;
							vc_tagbits.replace(vc_begin,vc_tag,l1_temp);
							for(i=0;i<l1_assoc;i++){
								if(i == count){
									l1_blockcount[row_num][i] = 0;
								}
								else{
									l1_blockcount[row_num][i] ++;
								}
							}					
							l1_validbit[row_num][count] = 1;
							l1_dbit[row_num][count] = 1;
							int begin = count*l1_tag;
							l1_tagbits[row_num].replace(begin,l1_tag,tgbits);
							if(l2_size != 0){
							  l2_readFromAddress(mainbits,tagnum,setnum,blksize,0,l2_assoc);
							}
						}
				}
			}
		}
	}
    }
}

void CACHE::l2_writeToAddress(string tagbits,int tag,int sets,int blksize,int writepolicy,int repolicy,int assoc){
	l2_write++;
	string setbits,tgbits,blockbits;
	int hexa;
	tgbits = tagbits.substr(0,tag);
	setbits = tagbits.substr(tag,sets);
	int row_num;
	row_num = strtoull(setbits.c_str(),NULL,2);
	if(repolicy == 0){
		if(writepolicy == 0){
		 if(assoc == 1){
			if((tagbits.compare(l2_tagbits[row_num]) == 0) && (l2_validbit[row_num][(assoc-1)] == 1)){
				l2_dbit[row_num][(assoc-1)] = 1;
			}
			else if(l2_validbit[row_num][(assoc-1)] == 0){
				l2_write_miss++;
				if(l2_dbit[row_num][(assoc-1)]==1){
					l2_numwb++;
				}
				l2_dbit[row_num][(assoc-1)] = 1;
				l2_tagbits[row_num] = tagbits;
				l2_validbit[row_num][(assoc-1)] = 1;
				// HAVE TO SETUP WRITE BACK UPDATE HERE TOO DEPENDING UPON WRITE POLICY
			}
			else if(tagbits.compare(l2_tagbits[row_num]) != 0){
				l2_write_miss++;
				if(l2_dbit[row_num][(assoc-1)]==1){
					l2_numwb++;
				}
				l2_dbit[row_num][(assoc-1)] = 1;
				l2_tagbits[row_num] = tagbits;
				l2_validbit[row_num][(assoc-1)] = 1;
			}
		}
		else{
			int i=0,j,index=0,hit=-1,count=0,max = -100;
			string dummy;
			int point=0;
			bool flag = false;
			for(i=0;i<assoc;i++){
				dummy = l2_tagbits[row_num].substr(index,tag);
				hit = dummy.compare(tgbits);
				if(hit == 0){
					break;	
				}
				count++;
				index = index+tag;
			}	
			if((hit == 0) && (l2_validbit[row_num][count]==1)){
				int temp = l2_blockcount[row_num][count];	
				for(i=0;i<assoc;i++){
					if(l2_blockcount[row_num][i] < temp){
						l2_blockcount[row_num][i] += 1;
					}
				}
				l2_blockcount[row_num][count] = 0;
				l2_dbit[row_num][count] = 1;
			}
			else {				
				for(i=0;i<assoc;i++){
					if(l2_validbit[row_num][i] == 0){
						flag = true;
						point = i;
						break;
					}
				}
				if(flag == false){	
						l2_write_miss++;							
						for(i=0;i<assoc;i++){
							if(max < l2_blockcount[row_num][i]){
								max = l2_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<assoc;i++){
							if(i == count){
								l2_blockcount[row_num][i] = 0;
							}
							else{
								l2_blockcount[row_num][i] ++;
							}
						}
						if(l2_dbit[row_num][count]==1){ //DOG TRAILS
							l2_numwb++;
						}
						l2_dbit[row_num][count] = 1;
						int start = count*tag;
						l2_tagbits[row_num].replace(start,tag,tgbits);
				}	
				else{
						l2_write_miss++;						
						for(i=0;i<assoc;i++){
							if(max < l2_blockcount[row_num][i]){
								max = l2_blockcount[row_num][i];
								count = i;
							}
						}
						for(i=0;i<assoc;i++){
							if(i == count){
								l2_blockcount[row_num][i] = 0;
							}
							else{
								l2_blockcount[row_num][i] ++;
							}
						}					
						l2_validbit[row_num][count] = 1;
						if(l2_dbit[row_num][count]==1){ //DOG TRAILS
							l2_numwb++;
						}
						l2_dbit[row_num][count] = 1;
						int begin = count*tag;
						l2_tagbits[row_num].replace(begin,tag,tgbits);
				}
			}
		}
	}
    }
}




int main(int argc,char *argv[]){
	int blocksize,repolicy=0,writepolicy=0,l1_size,l1_assoc,victim,l2_size,l2_assoc;
	string binst[100000];
	int address[100000];	
	blocksize = atoi(argv[1]);
	l1_size = atoi(argv[2]);
	l1_assoc = atoi(argv[3]);
	victim = atoi(argv[4]);
	l2_size = atoi(argv[5]);
	l2_assoc = atoi(argv[6]);
	ifstream infile(argv[7]);
	double sets,tag,index;
	string filename = argv[7];
	char dummy,a[100000];
	string dumb,b[100000];
	string parse = "0";
	int i=0;
	int value;
	int address_len;
	int checker; //temporary debug variable
	string setbits,tagbits,blockbits;
	while(infile >> dummy >> dumb){
		a[i] = dummy;
		b[i] = dumb;	
		value = strtol(dumb.c_str(), 0, 16);
		binst[i] = DecToBin2(value);
		binst[i].insert(0,parse);
		address_len = 32-binst[i].length();		
		while(address_len){
			binst[i].insert(0,parse);
			address_len--;
		}	
		i++;
	}
	address_len = 32;	
	double set_bit = l1_size/(blocksize*l1_assoc);
	sets = log(set_bit)/log(2);
	double blksize = log(blocksize)/log(2);
	tag = address_len - sets - blksize; // MAIN processing starts from Next line have to iterate over it
	int passblksize = blksize; //Block Offset
	int setnum = sets; //Set Bits
	int tagnum = tag; //Tag Size
	int set_row = set_bit;
	double set_bit1;
	double sets1;
	int tag1; // MAIN processing starts from Next line have to iterate over it
	int setnum1; //Set Bits
	int tagnum1; //Tag Size
	int set_row1;
	if(l2_size != 0){		
		set_bit1 = l2_size/(blocksize*l2_assoc);
		sets1 = log(set_bit1)/log(2);
		tag1 = address_len - sets1 - blksize; // MAIN processing starts from Next line have to iterate over it
		setnum1 = sets1; //Set Bits
		tagnum1 = tag1; //Tag Size
		set_row1 = set_bit1;
	}
	else{
		tagnum1 = 0;
		setnum1 = 0;
		sets1 = 0;
		set_row1 = 0;
	}
	CACHE cachetrial (tagnum,set_row,setnum,passblksize,l1_assoc,tagnum1,set_row1,sets1,l2_assoc,victim,blocksize);
	for(int k=0;k<i;k++)
	{
		if((a[k] == 'r') || (a[k] == 'R')){
			if(victim != 0){
				cachetrial.l1_vcreadFromAddress(binst[k],tagnum,setnum,blocksize,passblksize,repolicy,l1_assoc,l2_size,l2_assoc,victim,k);
			}
			else{
			      cachetrial.l1_readFromAddress(binst[k],tagnum,setnum,blocksize,passblksize,repolicy,l1_assoc,l2_size,l2_assoc);
			}
		}
		else if((a[k] == 'w') || (a[k] == 'W')){
			if(victim != 0){
				cachetrial.l1_vcwriteToAddress(binst[k],tagnum,setnum,blocksize,passblksize,writepolicy,repolicy,l1_assoc,l2_size,l2_assoc,victim,k);
			}
			else{
				cachetrial.l1_writeToAddress(binst[k],tagnum,setnum,blocksize,passblksize,writepolicy,repolicy,l1_assoc,l2_size,l2_assoc);
			}
		}
	}
	startprint(blocksize,l1_size,l1_assoc,victim,l2_size,l2_assoc,filename);
	cachetrial.l1_checkprint(tagnum,set_row,l1_assoc,l1_size,blocksize);
	if(victim!=0){
		cachetrial.vc_checkprint(victim,blocksize);
	}
	if(l2_size != 0){
		cachetrial.l2_checkprint(tagnum1,set_row1,l2_assoc,l2_size,blocksize);
	}
	cachetrial.simulate_checkprint(victim,l2_size,l2_assoc,blocksize,l1_size,l1_assoc);
	return 0;
}
