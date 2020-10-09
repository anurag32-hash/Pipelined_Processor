//#include "Instruction_memVd.h"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;

struct Instruction_memVd
{
	bitset<32> instruction;
	vector<bitset<8>> Instr_mem;

	Instruction_memVd()
	{
		Instr_mem.resize(4095);
		ifstream imemory;
		string line;
		int i = 0;
		imemory.open("F:\\IITD_Courses\\compArch\\Assignments\\Assignment10\\Pipelined_processorVd_10\\instructionVd.txt");
		if (imemory.is_open())
		{
			while (getline(imemory, line))
			{
				Instr_mem[i] = bitset<8>(line);
				i++;
			}
		}
		else
			cout << "Unable to open";
		imemory.close();
	}

	bitset<32> fetch(bitset<32> read_addr)
	{
		unsigned long addr = read_addr.to_ulong();
		int count = (int)addr;
		string instr = "";

		for (int i = 0; i < 4; i++)
		{
			instr += Instr_mem[count].to_string();
			count += 1;
		}
		bitset<32> inst(instr);
		instruction = inst;
		return instruction;
	}
	void printInstmem()
	{
		for (int i = 0; i < 4095; i++)
		{
			cout << Instr_mem[i] << endl;
		}
	}
};
