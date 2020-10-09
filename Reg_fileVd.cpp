//#include "Reg_fileVd.h"
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;

struct Reg_fileVd {
vector<bitset<32>> registers;
vector<bool> dh_flag;
bitset<32> readData1,readData2;

Reg_fileVd (){
    registers.resize(32);
    dh_flag.resize(32);
    for(int i=0;i<32;i++){
		registers[i] = bitset<32>((int)i);
		dh_flag[i] = true;
    }
}

void RegisterAccess (bitset<5> read_addr1, bitset<5> read_addr2, bitset<5> write_addr, bitset<32> writedata, bitset<1> write_enable){

	bitset<5> check = bitset<5>("11111");

	if((read_addr1^check).to_ulong() >=1){
		readData1 = registers[read_addr1.to_ulong()];
	}
	if((read_addr2^check).to_ulong() >=1){
		readData2 = registers[read_addr2.to_ulong()];
	}

	unsigned long write_ena = write_enable.to_ulong();
	int wena = (int) write_ena;
	if(wena == 1){
		registers[write_addr.to_ulong()] = writedata;
	}
}

bool DH_detection (bitset<5> read_addr1, bitset<5> read_addr2){

	bool rd1,rd2;
	bitset<5> check = bitset<5>("11111");

	if((read_addr1^check).to_ulong() >=1){
		rd1 = dh_flag[read_addr1.to_ulong()];
	}
	if((read_addr2^check).to_ulong() >=1){
		rd2 = dh_flag[read_addr2.to_ulong()];
	}

	if(rd1 == true && rd2 == true){
		return false;
	}
	else{
		return true;
	}
}

bool DH_detection1 (bitset<5> read_addr1){

	bool rd1;
	bitset<5> check = bitset<5>("11111");

	if((read_addr1^check).to_ulong() >=1){
		rd1 = dh_flag[read_addr1.to_ulong()];
	}

	if(rd1 == true){
		return false;
	}
	else{
		return true;
	}

}

void Impart_DH (bitset<5> write_addr,bool value){

	dh_flag[write_addr.to_ulong()] = value;
}

void printregisters (){

	for(int i=0;i<32;i++){
		bitset<32> t = registers.at(i);
		unsigned long te = t.to_ulong();
		cout << (int)te << endl;
	}
}
};
