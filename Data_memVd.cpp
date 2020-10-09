//#include "Data_memVd.h"
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

struct Data_memVd {
vector<bitset<8>> data_mem;
bitset<32> readData;


Data_memVd()
{
		data_mem.resize(4095);
        /*ifstream dmemory;
        string line;
        int i=0;
        dmemory.open("dmem.txt");
        if (dmemory.is_open())
        {
            while (getline(dmemory,line))
            {
                Data_mem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout<<"Unable to open";
        dmemory.close();*/
        for (int i=0;i<4095;i++){
			data_mem[i]=  bitset<8>(i);
        }
}

bitset<32> memoryopr (bitset<32> read_addr, bitset<32> write_data, bitset<1> mem_read, bitset<1> mem_write){

	unsigned long count = read_addr.to_ulong();
	int addr = (int) count;

	if((mem_read.to_ulong()==1) && (mem_write.to_ulong()==0)){
		string data;
		for(int i=0;i<4;i++){
			data += data_mem[addr].to_string();
			addr+=1;
		}
		bitset<32> fresult(data);
		readData = fresult;
	}

	else if ((mem_read.to_ulong()==0) && (mem_write.to_ulong()==1)){
		for(int i=0;i<32;i=i+8){
			string result = write_data.to_string().substr(i,8);
			bitset<8> fresult(result);
			data_mem[addr] = fresult;
			addr+=1;
		}
	}

	return 	readData;
}

void printdatamem ()
{
	for (int i=0;i<11;i++){
		cout<< data_mem[i]<< endl;
	}
}
};

