/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/

/*
In the programs seen in previous chapters, all memory needs were determined before
program execution by defining the variables needed. But there may be cases where the
 memory needs of a program can only be determined during runtime. For example, when
 the memory needed depends on user input. On these cases, programs need to dynamically
 allocate memory, for which the C++ language integrates the operators new and delete.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <stdio.h>
#include <map>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       unsigned long L1blocksize;
       unsigned long L1setsize;
       unsigned long L1size;
       unsigned long L2blocksize;
       unsigned long L2setsize;
       unsigned long L2size;
       };
// By default, all members of a class declared with the class keyword have private access for all its members.
//Therefore, any member that is declared before any other access specifier has private access
//automatically. For example:

/* you can define the cache class here, or design your own data structure for L1 and L2 cache */
class cache{
	// Private variables for cache in class
	vector<unsigned long> L1_counter,L2_counter; // for LRU implementation
    vector<vector<unsigned long>> L1;
    vector<vector<unsigned long>> L2;
public:

    // function for  creating cache L1
    vector<vector<unsigned long>> createL1(unsigned long L1setsize,
    										unsigned long index_L1,
											unsigned long L1blocksize)
	{
        vector<vector<unsigned long> > L1cache;
        L1cache.resize((unsigned long)L1setsize);
                    for(unsigned long i=0; i<L1setsize;i++)
                    {
                        L1cache[i].resize(index_L1);
                    }

        return L1cache;
    }

    // function for  creating cache L2
    vector<vector<unsigned long>> createL2(unsigned long L2setsize,
    										unsigned long index_L2,
											unsigned long L2blocksize)
	{
        vector<vector<unsigned long> > L2cache;
        L2cache.resize((unsigned long)L2setsize);
                    for(unsigned long i=0; i<L2setsize;i++)
                    {
                        L2cache[i].resize(index_L2);
                    }

        return L2cache;
    }

};


	vector<string> Extract_Bits(bitset<32> addr,
								unsigned long L1_tag_bits,
								unsigned long  L1_index_bits,
								unsigned long L1_offset_bits,
								unsigned long L2_tag_bits,
								unsigned long  L2_index_bits,
								unsigned long L2_offset_bits)
			{

	vector<string> bits(6);
	string address = addr.to_string();
    //string string_tag_L1
    bits[0]= address.substr(0,L1_tag_bits);
    //string string_index_L1
    bits[1]= address.substr(L1_tag_bits,L1_index_bits);
    //string string_offset_L1
    bits[2]= address.substr(L1_tag_bits+L1_index_bits, L1_offset_bits);

    //string string_tag_L2
    bits[3]= address.substr(0,L2_tag_bits);
    //string string_index_L2
    bits[4]= address.substr(L2_tag_bits,L2_index_bits);
    //string string_offset_L2
    bits[5]= address.substr(L2_tag_bits+L2_index_bits, L2_offset_bits);

    return bits;
					}



	// conversion from string to unsigned long
	long convert(string a){
		long answer;
		char *ptr;
		answer= strtol(a.c_str(), & ptr, 2);
		return answer;
	}

	// update

unsigned long update(unsigned long counter, vector<unsigned long> &Row, unsigned long way){

	unsigned long evaluate,p{0};
	evaluate = fmod(counter,way) + 1 - way;
	for(unsigned long i=0;i<way; i++){

		cout<<"Row["<<i<<"]="<<Row[i]<<endl;
	}
	for(unsigned long i=0;i<way; i++){

	if (Row[i]<evaluate)
	{
		cout<< Row[i]<<endl;
		p=i;
		break;
	}
	else
		  ;
	}
	return p;
}


int main(int argc, char* argv[]){
	unsigned long cycle{};
	 unsigned long c_L1{};
	 unsigned long c_L2{};
	 unsigned long value;
	 vector<unsigned long> valueL1;
	 vector<unsigned long> valueL2;
    config cacheconfig;
     ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
		while(!cache_params.eof())  // read config file
		{
		  cache_params>>dummyLine;
		  cache_params>>cacheconfig.L1blocksize;
		  cache_params>>cacheconfig.L1setsize;
		  cache_params>>cacheconfig.L1size;
		  cache_params>>dummyLine;
		  cache_params>>cacheconfig.L2blocksize;
		  cache_params>>cacheconfig.L2setsize;
		  cache_params>>cacheconfig.L2size;
		  }
    cout<< cacheconfig.L1blocksize << endl;

    // defining parameters for L1
    unsigned long L1_offset_bits = log2(cacheconfig.L1blocksize);
    unsigned long L1_index_bits = log2((cacheconfig.L1size*1024)/(cacheconfig.L1blocksize * cacheconfig.L1setsize));
	unsigned long L1_tag_bits = 32 - L1_offset_bits - L1_index_bits;

    // defining parameters for L2
    unsigned long L2_offset_bits = log2(cacheconfig.L2blocksize);
    unsigned long L2_index_bits = log2((cacheconfig.L2size*1024)/(cacheconfig.L2blocksize * cacheconfig.L2setsize));
 	unsigned long L2_tag_bits = 32 - L2_offset_bits - L2_index_bits;

 	// number of rows in caches for indexing
    unsigned long index_L1 = (unsigned long)pow(2, L1_index_bits);
    unsigned long index_L2 = (unsigned long)pow(2, L2_index_bits);


   // Implement by you:
   // initialize the hierarchy cache system with those configurations
   // probably you may define a Cache class for L1 and L2, or any data structure you like
    cache cacheObj;

    vector<vector<unsigned long> > L1 = cacheObj.createL1(cacheconfig.L1setsize,
    														index_L1,
															cacheconfig.L1blocksize);
    // counter for implementing LRU policy
    vector<vector<unsigned long> > counter_L1 = cacheObj.createL1(cacheconfig.L1setsize,
        														index_L1,
    															cacheconfig.L1blocksize);

    vector<vector<unsigned long> > L2 = cacheObj.createL2(cacheconfig.L2setsize,
    														index_L2,
															cacheconfig.L2blocksize);
    // counter for implementing LRU policy
    vector<vector<unsigned long> > counter_L2 = cacheObj.createL2(cacheconfig.L2setsize,
        														index_L2,
    															cacheconfig.L2blocksize);


    unsigned long sizeL2Way = L2.size();
    //unsigned long sizeL2Way = L2_cache.size();
    unsigned long RowL2 = L2[0].size();
    cout<< sizeL2Way<< endl;
    cout<< RowL2<< endl;

  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;

  	ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";

    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    cout<< accessaddr<<endl;
    	if (traces.is_open()&&tracesout.is_open()){
    			while (getline (traces,line)){   // read mem access file and access Cache

				   istringstream iss(line);
				   if (!(iss >> accesstype >> xaddr)) {break;}
				   stringstream saddr(xaddr);
				   saddr >> std::hex >> addr;
				   accessaddr = bitset<32> (addr);
				   cout<<accesstype<<endl;
				   cout<< accessaddr<< endl;
					vector<string> bits = Extract_Bits(accessaddr,
												L1_tag_bits,
												L1_index_bits,
												L1_offset_bits,
												L2_tag_bits,
												L2_index_bits,
												L2_offset_bits);

								   unsigned long tag_L1= convert(bits[0]);
								   unsigned long index_L1= convert(bits[1]);


								   unsigned long tag_L2= convert(bits[3]);
								   unsigned long index_L2= convert(bits[4]);

				                   unsigned long hit_L1=0, hit_L2=0;
				                   bool L1_access{}, L2_access{};
				                   unsigned c_L1 = cacheconfig.L1setsize-1;
				                   unsigned c_L2 = cacheconfig.L2setsize-1;
						// checking for tag match in given ways
									 for(unsigned long i=0; i<cacheconfig.L1setsize; i++)
									 {
										 L1_access = true ;

										 if (tag_L1==L1[i][index_L1])
										 {
											 //read hit
											 hit_L1=1;

										 }
										 else
										 {
											 //miss
											 hit_L1=0;
											 //update tag
							              }

									 }


											 if (hit_L1==0 || accesstype.compare("W")==0 )
											 {
												 L2_access = true;
												 for(unsigned long i=0; i<cacheconfig.L2setsize; i++)
												 {

													 if (tag_L2==L2[i][index_L2])
													 {
														 //read hit
														 hit_L2=1;

													 }
													 else
													 {
														 //miss
														 hit_L2=0;
														 //update tag

													 }

												 }
											 }




						// access the L1 and L2 Cache according to the trace;
					   if (accesstype.compare("R")==0)
								   {
									   //Implement by you:
									   // read access to the L1 Cache,
									   //  and then L2 (if required),
									   // update the L1 and L2 access state variable;

									   if(hit_L1==0 && hit_L2==0 && L1_access==1 && L2_access==1)
									   {
										   L1AcceState = RM;
										   L2AcceState = RM;

											// update L1
											for (unsigned long i=0;i<cacheconfig.L1setsize;i++){
											valueL1.push_back(counter_L1[i][index_L1]);
											cout<<"valueL1["<<i<<"]="<<valueL1[i]<<endl;
											 }
											value = update(c_L1,valueL1,cacheconfig.L1setsize);
											L1[value][index_L1]= tag_L1;
											counter_L1[value][index_L1]= c_L1;

											for (unsigned long i=0;i<cacheconfig.L1setsize;i++){
											cout<<"counter_L1["<<i<<"]="<<counter_L1[i][index_L1]<<endl;
											 }
											cout<< "index_L1="<<index_L1<<endl;
											//update L2
											for (unsigned long i=0;i<cacheconfig.L2setsize;i++){
											valueL2.push_back(counter_L2[i][index_L2]);
											cout<<"valueL2["<<i<<"]="<<valueL2[i]<<endl;
											}
											value = update(c_L2,valueL2,cacheconfig.L2setsize);
											L2[value][index_L2]= tag_L2;
											counter_L2[value][index_L2]= c_L2;
											for (unsigned long i=0;i<cacheconfig.L2setsize;i++){
											cout<<"counter_L2["<<i<<"]="<<counter_L2[i][index_L2]<<endl;
											 }
											cout<< "index_L2="<<index_L2<<endl;
											cout<< "cycle ="<<cycle << "updated L2 & L2   "<<L2AcceState<<L2AcceState<<endl;

									   }
									   else if(hit_L1==0 && hit_L2==1 && L1_access==1 && L2_access==1)
									   {
										   L1AcceState = RM;
										   L2AcceState = RH;
											// update L1
											for (unsigned long i=0;i<cacheconfig.L1setsize;i++){
											valueL1.push_back(counter_L1[i][index_L1]);
											cout<<"valueL1["<<i<<"]="<<valueL1[i]<<endl;
											}
											value = update(c_L1,valueL1,cacheconfig.L1setsize);
											L1[value][index_L1]= tag_L1;
											counter_L1[value][index_L1]= c_L1;
											cout<< "cycle ="<<cycle << "updated L1   "<<L1AcceState<<L2AcceState<<endl;

									   }
									   else if(hit_L1==1 && L1_access==1 && L2_access==0)
									   {
										   L1AcceState = RH;
										   L2AcceState = NA;
										   cout<< "cycle ="<<cycle << "No-update   "<<L1AcceState<<L2AcceState<<endl;
									   }

								   }
								   else if (accesstype.compare("W")==0)
								   {
										//Implement by you:
										// write access to the L1 Cache,
										//and then L2 (if required),
										//update the L1 and L2 access state variable;
									   if(hit_L1==0 && hit_L2==0 && L1_access==1 && L2_access==1)
									   {
										   L1AcceState = WM;
										   L2AcceState = WM;

										   cout<< "cycle ="<<cycle << "No-update   "<<L1AcceState<<L2AcceState<<endl;
									   }
									   else if(hit_L1==0 && hit_L2==1 && L1_access==1 && L2_access==1)
									   {
										   L1AcceState = WM;
										   L2AcceState = WH;
											// update L1
											for (unsigned long i=0;i<cacheconfig.L1setsize;i++){
											valueL1.push_back(counter_L1[i][index_L1]);
											cout<<"valueL1["<<i<<"]="<<valueL1[i]<<endl;
											}
											value = update(c_L1,valueL1,cacheconfig.L1setsize);
											L1[value][index_L1]= tag_L1;
											counter_L1[value][index_L1]= c_L1;
											cout<< "cycle ="<<cycle << "Update L1  "<<L1AcceState<<L2AcceState<<endl;
									   }
									   else if(hit_L1==1)
									   {
										   L1AcceState = WH;
										   L2AcceState = WH;
										   cout<< "cycle ="<<cycle << "No update   "<<L1AcceState<<L2AcceState<<endl;
											// update L1
											for (unsigned long i=0;i<cacheconfig.L1setsize;i++){
											valueL1.push_back(counter_L1[i][index_L1]);
											cout<<"valueL1["<<i<<"]="<<valueL1[i]<<endl;
											}
											value = update(c_L1,valueL1,cacheconfig.L1setsize);
											L1[value][index_L1]= tag_L1;
											counter_L1[value][index_L1]= c_L1;
											cout<< "cycle ="<<cycle << "Update L1  "<<L1AcceState<<L2AcceState<<endl;
											//update L2
											for (unsigned long i=0;i<cacheconfig.L2setsize;i++){
											valueL2.push_back(counter_L2[i][index_L2]);
											cout<<"valueL2["<<i<<"]="<<valueL2[i]<<endl;
											}
											value = update(c_L2,valueL2,cacheconfig.L2setsize);
											L2[value][index_L2]= tag_L2;
											counter_L2[value][index_L2]= c_L2;
											cout<< "cycle ="<<cycle << "updated L1 & L2   "<<L1AcceState<<L2AcceState<<endl;

									   }

								   }
						L1_access=0;
						L2_access=0;
						cycle++;
		c_L1++;
		c_L2++;
		valueL1.clear();
		valueL2.clear();
       tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
           }

           traces.close();
            tracesout.close();

        }
        else {cout<< "Unable to open trace or traceout file "<< endl;}

    return 0;
}
