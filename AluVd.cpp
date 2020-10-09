//#include "AluVd.h"
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include<cmath>
using namespace std;

struct AluVd {
bitset<32> aluresult;

bitset<32> AluOpr (bitset<3> aluop, bitset<32> parameter1, bitset<32> parameter2){
	unsigned long ans;
	if(aluop.to_string() == "001"){
		ans = parameter1.to_ulong() + parameter2.to_ulong();
	}
	else if(aluop.to_string() == "011"){
		ans = parameter1.to_ulong() - parameter2.to_ulong();
	}
	bitset<32> result((int)ans);
	aluresult = result;
	return aluresult;
}

bitset<32> shifter (bitset<3> aluop, bitset<32> para1, bitset<5> shamt){

	unsigned long shamt1 = shamt.to_ulong();
	int shift = (int) shamt1;
	unsigned long para2 = para1.to_ulong();
    int para = (int)para2;
	int result;

	if(aluop.to_string() == "010"){      //shift left
		result = para*pow(2,shift);

	}
	else if(aluop.to_string() == "100"){  // shift right
		result =  para/pow(2,shift);
	}

	bitset<32> ans(result);
	aluresult = ans;
	return aluresult;
}
};

