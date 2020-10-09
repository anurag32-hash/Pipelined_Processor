//#include "Pipelined_processorVd.h"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <array>
#include <fstream>
#include <cmath>
#include <cstddef>
#include "AluVd.cpp"
#include "Data_memVd.cpp"
#include "Instruction_memVd.cpp"
#include "Reg_fileVd.cpp"
#include "QueueVd.cpp"

using namespace std;

struct Pipelined_processorVd
{

	bitset<5> check = bitset<5>("11111");
	bitset<32> check1 = bitset<32>("11111111111111111111111111111111");
	bitset<1> write = bitset<1>(1);
	bitset<1> read = bitset<1>(0);
	QueueVd IF_ID;
	QueueVd ID_EX;
	QueueVd EX_MEM;
	QueueVd MEM_WB;
	bool instr_fetch = true;
	bool flag = false;
	bool halt = false;
	bool if_id_flag, id_ex_flag, ex_mem_flag, mem_wb_flag = false;
	bool endgame = false;
	bool d_flag1, d_flag2 = false;
	Reg_fileVd myregfile;
	AluVd myalu;
	Data_memVd mydatamem;
	Instruction_memVd myinstrmem;
	bitset<32> programCounter = bitset<32>(0);
	bitset<32> pc25 = bitset<32>(0);
	bitset<32> instruction, instructionId;
	string instructionEx, instructionMem, instructionWb;
	string type_instr, type_func;
	bitset<5> addr_rs, addr_rt, addr_rd;
	bitset<32> $rs, $rt, $rd, aluresult;
	vector<bitset<5>> Raddress;
	vector<string> Iaddress;
	string $rs_value_Ex;
	string $rt_value_Ex;
	string $rd_address_Ex;
	string regwrite_Ex;
	string aluOp_Ex;
	string Mem_write_Ex;
	string Mem_read_Ex;
	string finImm_Ex;
	string $rs_addr_Ex;
	string $rt_addr_Ex;
	string $rd_addr0;
	string new_$rs_value_Ex;
	string new_$rt_value_Ex;

	string aluresult_Mem;
	string $rd_address_Mem;
	string regwrite_Mem;
	string Mem_read_Mem;
	string Mem_write_Mem;
	string aluresult_Wb;
	string $rd_address_Wb;
	string regwrite_Wb;
	unsigned long counter = 0;
	bitset<3> add_opp = bitset<3>(1);
	bitset<3> sub_opp = bitset<3>(3);
	bitset<3> sll_opp = bitset<3>(2);
	bitset<3> srl_opp = bitset<3>(4);
	int cycles = 0;
	int instr_numb = 0;
	int N, x;
	int y = 1;
	bool v_delay = false;
	bool return_delay = false;
	double chance;

	string InstructionType(string opcode)
	{
		if (opcode == "000000")
			return "R";
		else if (opcode == "000010")
			return "j";
		else if (opcode == "000011")
			return "jal";
		else if (opcode == "111111")
			return "halt";
		else if (opcode == "000101")
			return "bne";
		else if (opcode == "000100")
			return "beq";
		else if (opcode == "000110")
			return "blez";
		else if (opcode == "000111")
			return "bgtz";
		else if (opcode == "100011")
			return "lw";
		else if (opcode == "101011")
			return "sw";
		else
			return "Invalid Opcode";
	}
	string FunctionType(bitset<32> instruction)
	{
		string aluOp = instruction.to_string().substr(26, 6);
		if (aluOp == "100000")
			return "add";
		else if (aluOp == "100010")
			return "sub";
		else if (aluOp == "000000")
			return "sll";
		else if (aluOp == "000010")
			return "srl";
		else if (aluOp == "001000")
			return "jr";
		else
			return "Invalid AluOP";
	}
	vector<bitset<5>> OperandsAddr(bitset<32> instruction)
	{
		vector<bitset<5>> result(3);
		string rs = instruction.to_string().substr(6, 5);
		string rt = instruction.to_string().substr(11, 5);
		string rd = instruction.to_string().substr(16, 5);
		bitset<5> Rd(rd);
		bitset<5> Rs(rs);
		bitset<5> Rt(rt);
		result[0] = Rs;
		result[1] = Rt;
		result[2] = Rd;
		return result;
	}
	vector<string> OperandsAddi(bitset<32> instruction)
	{
		vector<string> result(3);
		string rs = instruction.to_string().substr(6, 5);
		string rt = instruction.to_string().substr(11, 5);
		string imm = instruction.to_string().substr(16, 16);
		result[0] = rs;
		result[1] = rt;
		result[2] = imm;
		return result;
	}
	bitset<32> calc_PC(bitset<32> &programCounter)
	{
		programCounter = bitset<32>(programCounter.to_ulong() + 4);
		return programCounter;
	}
	void if_id_empty()
	{
		if (if_id_flag == true && id_ex_flag == true && ex_mem_flag == true && mem_wb_flag == true && halt == true)
		{
			endgame = true;
		}
		if (instr_fetch)
		{
			//cout<< "chak" << endl;
			instruction = myinstrmem.fetch(programCounter);
			cout << "Instruction Fetch: " << instruction.to_string();
			cout << "\n"
				 << endl;
			IF_ID.push(instruction.to_string());
			instr_numb += 1;
			string opcode = instruction.to_string().substr(0, 6);
			type_instr = InstructionType(opcode);
			if (type_instr == "j" || type_instr == "jal" || type_instr == "bne" || type_instr == "beq" || type_instr == "blez" || type_instr == "bgtz")
			{
				instr_fetch = false;
			}
		}
	}
	void if_id_not_empty()
	{
		bitset<32> instructionId1(IF_ID.pop());
		instructionId = instructionId1;
		cout << "Instruction Decoding: " << instructionId.to_string();
		cout << "\n";
		if (instructionId.to_string() == "11111111111111111111111111111111")
		{
			instr_numb = instr_numb - 1;
			instr_fetch = false;
			halt = true;
		}
		string opcode = instructionId.to_string().substr(0, 6);
		type_instr = InstructionType(opcode);
		if (type_instr == "R")
		{
			Raddress = OperandsAddr(instructionId);
			addr_rs = Raddress[0];
			addr_rt = Raddress[1];
			addr_rd = Raddress[2];

			d_flag1 = myregfile.DH_detection1(addr_rs);
			d_flag2 = myregfile.DH_detection1(addr_rt);

			if (d_flag1 == false && d_flag2 == false)
			{
				myregfile.Impart_DH(addr_rd, false);
				type_func = FunctionType(instructionId);
				myregfile.RegisterAccess(addr_rs, addr_rt, check, check1, read);
				$rs = myregfile.readData1;
				$rt = myregfile.readData2;
				string shamt = instructionId.to_string().substr(21, 5);

				if (type_func == "add")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(add_opp.to_string()); // aluOp
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
				}
				else if (type_func == "sub")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sub_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
				}
				else if (type_func == "sll")
				{
					ID_EX.push(type_func);
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push(shamt);
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sll_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
				}
				else if (type_func == "srl")
				{
					ID_EX.push(type_func);
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push(shamt);
					ID_EX.push("1");				 //regwrite
					ID_EX.push(srl_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
				}
				else if (type_func == "jr")
				{
					pc25 = $rs;
					flag = true;
				}
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				myregfile.Impart_DH(addr_rd, false);
				type_func = FunctionType(instructionId);
				myregfile.RegisterAccess(addr_rs, addr_rt, check, check1, read);
				$rs = myregfile.readData1;
				$rt = myregfile.readData2;
				string shamt = instructionId.to_string().substr(21, 5);

				if (type_func == "add")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(add_opp.to_string()); // aluOp
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rs.to_string());
				}
				else if (type_func == "sub")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sub_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rs.to_string());
				}

				else if (type_func == "jr")
				{
					instr_fetch = false;
					IF_ID.push(instructionId.to_string());
				}
			}
			else if (d_flag1 == false && d_flag2 == true)
			{
				myregfile.Impart_DH(addr_rd, false);
				type_func = FunctionType(instructionId);
				myregfile.RegisterAccess(addr_rs, addr_rt, check, check1, read);
				$rs = myregfile.readData1;
				$rt = myregfile.readData2;
				string shamt = instructionId.to_string().substr(21, 5);

				if (type_func == "add")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(add_opp.to_string()); // aluOp
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rt.to_string());
				}
				else if (type_func == "sub")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sub_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rt.to_string());
				}
				else if (type_func == "sll")
				{
					ID_EX.push(type_func);
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push(shamt);
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sll_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rt.to_string());
				}
				else if (type_func == "srl")
				{
					ID_EX.push(type_func);
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push(shamt);
					ID_EX.push("1");				 //regwrite
					ID_EX.push(srl_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rt.to_string());
				}
			}
			else if (d_flag1 == true && d_flag2 == true)
			{
				myregfile.Impart_DH(addr_rd, false);
				type_func = FunctionType(instructionId);
				myregfile.RegisterAccess(addr_rs, addr_rt, check, check1, read);
				$rs = myregfile.readData1;
				$rt = myregfile.readData2;
				string shamt = instructionId.to_string().substr(21, 5);

				if (type_func == "add")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(add_opp.to_string()); // aluOp
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rs.to_string());
					ID_EX.push(addr_rt.to_string());
				}
				else if (type_func == "sub")
				{
					ID_EX.push(type_func);
					ID_EX.push($rs.to_string());
					ID_EX.push($rt.to_string());
					ID_EX.push(addr_rd.to_string());
					ID_EX.push("1");				 //regwrite
					ID_EX.push(sub_opp.to_string()); //opcode
					ID_EX.push("0");				 // mem write
					ID_EX.push("0");				 // mem read
					ID_EX.push(instructionId.to_string());
					ID_EX.push(addr_rs.to_string());
					ID_EX.push(addr_rt.to_string());
				}
			}
		}
		else if (type_instr == "lw" || type_instr == "sw" || type_instr == "bne" || type_instr == "beq" || type_instr == "blez" || type_instr == "bgtz")
		{
			Iaddress = OperandsAddi(instructionId);
			bitset<5> rsaddrI(Iaddress[0]);
			bitset<5> rtaddrI(Iaddress[1]);
			bitset<16> Imaddr(Iaddress[2]);

			myregfile.RegisterAccess(rsaddrI, check, check, check1, read);
			$rs = myregfile.readData1;
			string finalImStr;
			if (Imaddr.to_string().at(0) == '0')
			{
				finalImStr = "0000000000000000" + Imaddr.to_string();
			}
			else if (Imaddr.to_string().at(0) == '1')
			{
				finalImStr = "1111111111111111" + Imaddr.to_string();
			}
			bitset<32> finalIm(finalImStr);
			d_flag1 = myregfile.DH_detection1(rsaddrI);
			d_flag2 = myregfile.DH_detection1(rtaddrI);

			if (type_instr == "lw" && d_flag1 == false)
			{
				myregfile.Impart_DH(rtaddrI, false);
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push(rtaddrI.to_string());
				ID_EX.push("1");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("0"); // mem write
				ID_EX.push("1"); // mem read
				ID_EX.push(instructionId.to_string());
			}
			else if (type_instr == "lw" && d_flag1 == true)
			{
				myregfile.Impart_DH(rtaddrI, false);

				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push(rtaddrI.to_string());
				ID_EX.push("1");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("0"); // mem write
				ID_EX.push("1"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
			}
			else if ((type_instr == "blez" || type_instr == "bgtz") && d_flag1 == false)
			{
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
			}

			else if ((type_instr == "blez" || type_instr == "bgtz") && d_flag1 == true)
			{
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
			}
			else if ((type_instr == "beq" || type_instr == "bne") && d_flag1 == false && d_flag2 == false)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push($rt.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
			}
			else if ((type_instr == "beq" || type_instr == "bne") && d_flag1 == true && d_flag2 == false)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push($rt.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
			}
			else if ((type_instr == "beq" || type_instr == "bne") && d_flag1 == false && d_flag2 == true)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push($rt.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rtaddrI.to_string());
			}
			else if ((type_instr == "beq" || type_instr == "bne") && d_flag1 == true && d_flag2 == true)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string());
				ID_EX.push($rt.to_string());
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0"); // regwrite
				ID_EX.push("0"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
				ID_EX.push(rtaddrI.to_string());
			}
			else if (type_instr == "sw" && d_flag1 == false && d_flag2 == false)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string()); // rs value
				ID_EX.push($rt.to_string()); // rt value
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("1"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
			}
			else if (type_instr == "sw" && d_flag1 == true && d_flag2 == false)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string()); // rs value
				ID_EX.push($rt.to_string()); // rt value
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("1"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
			}
			else if (type_instr == "sw" && d_flag1 == false && d_flag2 == true)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string()); // rs value
				ID_EX.push($rt.to_string()); // rt value
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("1"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rtaddrI.to_string());
			}
			else if (type_instr == "sw" && d_flag1 == true && d_flag2 == true)
			{
				myregfile.RegisterAccess(rtaddrI, check, check, check1, read); // get value from register
				$rt = myregfile.readData1;
				ID_EX.push(type_instr);
				ID_EX.push($rs.to_string()); // rs value
				ID_EX.push($rt.to_string()); // rt value
				ID_EX.push(finalIm.to_string());
				ID_EX.push("0");
				ID_EX.push(add_opp.to_string());
				ID_EX.push("1"); // mem write
				ID_EX.push("0"); // mem read
				ID_EX.push(instructionId.to_string());
				ID_EX.push(rsaddrI.to_string());
				ID_EX.push(rtaddrI.to_string());
			}
		}
		else if (type_instr == "j")
		{
			ID_EX.push(type_instr);
			ID_EX.push(instruction.to_string().substr(6, 26));
			ID_EX.push("0"); // regwrite
			ID_EX.push("0"); // mem write
			ID_EX.push("0"); // mem read
			ID_EX.push(instructionId.to_string());
		}
		else if (type_instr == "jal")
		{
			ID_EX.push(type_instr);
			ID_EX.push(instruction.to_string().substr(6, 26));
			ID_EX.push("1"); //regwrite
			ID_EX.push("0"); // mem write
			ID_EX.push("0"); // mem read
			ID_EX.push(instructionId.to_string());
		}
		if (flag == true)
		{

			if (instr_fetch)
			{

				programCounter = pc25;
				instruction = myinstrmem.fetch(programCounter);
				cout << "Instruction Fetch: " << instruction.to_string();
				cout << "\n"
					 << endl;
				IF_ID.push(instruction.to_string());
				instr_numb += 1;
				string opcode = instruction.to_string().substr(0, 6);
				type_instr = InstructionType(opcode);
				if (type_instr == "j" || type_instr == "jal" || type_instr == "bne" || type_instr == "beq" || type_instr == "blez" || type_instr == "bgtz")
				{
					instr_fetch = false;
				}
			}
		}
		if (flag == false)
		{

			if (instr_fetch)
			{
				programCounter = calc_PC(programCounter);
				instruction = myinstrmem.fetch(programCounter);
				cout << "Instruction Fetch: " << instruction.to_string();
				cout << "\n"
					 << endl;
				IF_ID.push(instruction.to_string());
				instr_numb += 1;
				string opcode = instruction.to_string().substr(0, 6);
				type_instr = InstructionType(opcode);
				if (type_instr == "j" || type_instr == "jal" || type_instr == "bne" || type_instr == "beq" || type_instr == "blez" || type_instr == "bgtz")
				{
					instr_fetch = false;
				}
			}
		}
	}
	void id_ex_not_empty()
	{
		string type_func_Ex = ID_EX.pop();
		;

		if (type_func_Ex == "add" || type_func_Ex == "sub")
		{
			$rs_value_Ex = ID_EX.pop();
			$rt_value_Ex = ID_EX.pop();
			$rd_address_Ex = ID_EX.pop();

			regwrite_Ex = ID_EX.pop();
			aluOp_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == true && d_flag2 == false)
			{

				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{

					new_$rs_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> $Rt_value_Ex($rt_value_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{

					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> $Rt_value_Ex($rt_value_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
			else if (d_flag1 == false && d_flag2 == true)
			{

				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{

					new_$rt_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex($rs_value_Ex);
					bitset<32> $Rt_value_Ex(new_$rt_value_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rt_addr_Ex != $rd_addr0)
				{

					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex($rs_value_Ex);
					bitset<32> $Rt_value_Ex(new_$rt_value_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
			else if (d_flag1 == true && d_flag2 == true)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];
				}

				if ($rt_addr_Ex != $rd_addr0)
				{
					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
				}

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
				}

				if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
				}

				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
				bitset<32> $Rt_value_Ex(new_$rt_value_Ex);
				aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push($rd_address_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
			else if (d_flag1 == false && d_flag2 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<32> $Rs_value_Ex($rs_value_Ex);
				bitset<32> $Rt_value_Ex($rt_value_Ex);
				aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, $Rt_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push($rd_address_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
		}

		else if (type_func_Ex == "sll" || type_func_Ex == "srl")
		{
			$rt_value_Ex = ID_EX.pop();
			$rd_address_Ex = ID_EX.pop();
			string shamt_value_Ex = ID_EX.pop();

			regwrite_Ex = ID_EX.pop();
			aluOp_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{

				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<5> Shamt_value_Ex(shamt_value_Ex);
				bitset<32> $Rt_value_Ex($rt_value_Ex);
				aluresult = myalu.shifter(AluOp_Ex, $Rt_value_Ex, Shamt_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push($rd_address_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
			else if (d_flag1 == false && d_flag2 == true)
			{
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{

					new_$rt_value_Ex = MEM_WB.queue[1];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<5> Shamt_value_Ex(shamt_value_Ex);
					bitset<32> $Rt_value_Ex(new_$rt_value_Ex);
					aluresult = myalu.shifter(AluOp_Ex, $Rt_value_Ex, Shamt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rt_addr_Ex != $rd_addr0)
				{
					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<5> Shamt_value_Ex(shamt_value_Ex);
					bitset<32> $Rt_value_Ex(new_$rt_value_Ex);
					aluresult = myalu.shifter(AluOp_Ex, $Rt_value_Ex, Shamt_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
		}

		else if (type_func_Ex == "lw")
		{
			$rs_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			$rd_address_Ex = ID_EX.pop();

			regwrite_Ex = ID_EX.pop();
			aluOp_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<32> $Rs_value_Ex($rs_value_Ex);
				bitset<32> finImm_value_Ex(finImm_Ex);
				aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push($rd_address_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
			else if (d_flag1 == true)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rd_address_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
		}

		else if (type_func_Ex == "sw")
		{
			$rs_value_Ex = ID_EX.pop();
			$rt_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			aluOp_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{

				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<32> $Rs_value_Ex($rs_value_Ex);
				bitset<32> finImm_value_Ex(finImm_Ex);
				aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push($rt_value_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rt_value_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push($rt_value_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
			else if (d_flag1 == false && d_flag2 == true)
			{
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex($rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push(new_$rt_value_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
				else if ($rt_addr_Ex != $rd_addr0)
				{
					bitset<5> $rt_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					bitset<3> AluOp_Ex(aluOp_Ex);
					bitset<32> $Rs_value_Ex($rs_value_Ex);
					bitset<32> finImm_value_Ex(finImm_Ex);
					aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

					EX_MEM.push(type_func_Ex);
					EX_MEM.push(aluresult.to_string());
					EX_MEM.push(new_$rt_value_Ex);
					EX_MEM.push(regwrite_Ex);
					EX_MEM.push(Mem_write_Ex);
					EX_MEM.push(Mem_read_Ex);
					EX_MEM.push(instructionEx);
				}
			}
			else if (d_flag1 == true && d_flag2 == true)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
				}
				if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
				}
				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];
				}
				if ($rt_addr_Ex != $rd_addr0)
				{
					bitset<5> $rt_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
				}
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				bitset<3> AluOp_Ex(aluOp_Ex);
				bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
				bitset<32> finImm_value_Ex(finImm_Ex);
				aluresult = myalu.AluOpr(AluOp_Ex, $Rs_value_Ex, finImm_value_Ex);

				EX_MEM.push(type_func_Ex);
				EX_MEM.push(aluresult.to_string());
				EX_MEM.push(new_$rt_value_Ex);
				EX_MEM.push(regwrite_Ex);
				EX_MEM.push(Mem_write_Ex);
				EX_MEM.push(Mem_read_Ex);
				EX_MEM.push(instructionEx);
			}
		}

		else if (type_func_Ex == "bne")
		{
			$rs_value_Ex = ID_EX.pop();
			$rt_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				if ($rs_value_Ex != $rt_value_Ex)
				{

					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc_Ex(counter);
					programCounter = pc_Ex;
					instr_fetch = true;
				}
				else
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rs_value_Ex != $rt_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rs_value_Ex != $rt_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
			else if (d_flag1 == false && d_flag2 == true)
			{

				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rt_value_Ex != $rs_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rt_addr_Ex != $rd_addr0)
				{

					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rt_value_Ex != $rs_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
			else if (d_flag1 == true && d_flag2 == true)
			{

				$rs_addr_Ex = ID_EX.pop();
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
				}
				if ($rs_addr_Ex != $rd_addr0)
				{

					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
				}
				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];
				}
				if ($rt_addr_Ex != $rd_addr0)
				{

					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
				}
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				if (new_$rs_value_Ex != new_$rt_value_Ex)
				{

					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc_Ex(counter);
					programCounter = pc_Ex;
					instr_fetch = true;
				}
				if (new_$rs_value_Ex == new_$rt_value_Ex)
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
		}

		else if (type_func_Ex == "beq")
		{

			$rs_value_Ex = ID_EX.pop();
			$rt_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				if ($rs_value_Ex == $rt_value_Ex)
				{

					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc_Ex(counter);
					programCounter = pc_Ex;
					instr_fetch = true;
				}
				else
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rs_value_Ex == $rt_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rs_value_Ex == $rt_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
			else if (d_flag1 == false && d_flag2 == true)
			{

				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];

					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rt_value_Ex == $rs_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rt_addr_Ex != $rd_addr0)
				{

					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";

					if (new_$rt_value_Ex == $rs_value_Ex)
					{

						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc_Ex(counter);
						programCounter = pc_Ex;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
			else if (d_flag1 == true && d_flag2 == true)
			{

				$rs_addr_Ex = ID_EX.pop();
				$rt_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[1];
				}
				if ($rs_addr_Ex != $rd_addr0)
				{

					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
				}
				if ($rt_addr_Ex == $rd_addr0)
				{
					new_$rt_value_Ex = MEM_WB.queue[1];
				}
				if ($rt_addr_Ex != $rd_addr0)
				{

					bitset<5> $rt_addr_Ex0($rt_addr_Ex);
					myregfile.RegisterAccess($rt_addr_Ex0, check, check, check1, read);
					new_$rt_value_Ex = myregfile.readData1.to_string();
				}
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";

				if (new_$rs_value_Ex == new_$rt_value_Ex)
				{

					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc_Ex(counter);
					programCounter = pc_Ex;
					instr_fetch = true;
				}
				if (new_$rs_value_Ex != new_$rt_value_Ex)
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
		}
		else if (type_func_Ex == "bgtz")
		{
			$rs_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";
				bitset<32> $Rs_value_Ex($rs_value_Ex);
				unsigned long $rs1 = $Rs_value_Ex.to_ulong();
				int $rs2 = (int)$rs1;

				if ($rs2 > 0)
				{
					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc(counter);
					programCounter = pc;
					instr_fetch = true;
				}
				else
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[2];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					unsigned long $rs1 = $Rs_value_Ex.to_ulong();
					int $rs2 = (int)$rs1;

					if ($rs2 > 0)
					{
						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc(counter);
						programCounter = pc;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					unsigned long $rs1 = $Rs_value_Ex.to_ulong();
					int $rs2 = (int)$rs1;

					if ($rs2 > 0)
					{
						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc(counter);
						programCounter = pc;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
		}

		else if (type_func_Ex == "blez")
		{
			$rs_value_Ex = ID_EX.pop();
			finImm_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();

			if (d_flag1 == false && d_flag2 == false)
			{
				cout << "Instruction Execution: " << instructionEx;
				cout << "\n";
				bitset<32> $Rs_value_Ex($rs_value_Ex);
				unsigned long $rs1 = $Rs_value_Ex.to_ulong();
				int $rs2 = (int)$rs1;

				if ($rs2 <= 0)
				{
					string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
					bitset<32> signExtIm_Ex(finalImStr_Ex);
					counter = programCounter.to_ulong();
					counter = counter + 4 + signExtIm_Ex.to_ulong();
					bitset<32> pc(counter);
					programCounter = pc;
					instr_fetch = true;
				}
				else
				{
					programCounter = calc_PC(programCounter);
					instr_fetch = true;
				}
			}
			else if (d_flag1 == true && d_flag2 == false)
			{
				$rs_addr_Ex = ID_EX.pop();
				$rd_addr0 = MEM_WB.queue[2];

				if ($rs_addr_Ex == $rd_addr0)
				{
					new_$rs_value_Ex = MEM_WB.queue[2];
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					unsigned long $rs1 = $Rs_value_Ex.to_ulong();
					int $rs2 = (int)$rs1;

					if ($rs2 <= 0)
					{
						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc(counter);
						programCounter = pc;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
				else if ($rs_addr_Ex != $rd_addr0)
				{
					bitset<5> $rs_addr_Ex0($rs_addr_Ex);
					myregfile.RegisterAccess($rs_addr_Ex0, check, check, check1, read);
					new_$rs_value_Ex = myregfile.readData1.to_string();
					cout << "Instruction Execution: " << instructionEx;
					cout << "\n";
					bitset<32> $Rs_value_Ex(new_$rs_value_Ex);
					unsigned long $rs1 = $Rs_value_Ex.to_ulong();
					int $rs2 = (int)$rs1;

					if ($rs2 <= 0)
					{
						string finalImStr_Ex = finImm_Ex.substr(0, 30) + "00";
						bitset<32> signExtIm_Ex(finalImStr_Ex);
						counter = programCounter.to_ulong();
						counter = counter + 4 + signExtIm_Ex.to_ulong();
						bitset<32> pc(counter);
						programCounter = pc;
						instr_fetch = true;
					}
					else
					{
						programCounter = calc_PC(programCounter);
						instr_fetch = true;
					}
				}
			}
		}

		else if (type_func_Ex == "j")
		{
			string Jaddress_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();
			cout << "Instruction Execution: " << instructionEx;
			cout << "\n";

			counter = programCounter.to_ulong();
			counter = counter + 4;
			bitset<32> next(counter);
			programCounter = next;

			bitset<4> next4MSB(programCounter.to_string().substr(0, 4));
			string nextAddr = next4MSB.to_string() + Jaddress_Ex + "00";
			bitset<32> nextPc(nextAddr);
			programCounter = nextPc;
			instr_fetch = true;
		}
		else if (type_func_Ex == "jal")
		{
			string Jaddress_Ex = ID_EX.pop();
			regwrite_Ex = ID_EX.pop();
			Mem_write_Ex = ID_EX.pop();
			Mem_read_Ex = ID_EX.pop();
			instructionEx = ID_EX.pop();
			cout << "Instruction Execution: " << instructionEx;
			cout << "\n";

			EX_MEM.push(type_func_Ex);
			EX_MEM.push(programCounter.to_string());
			bitset<5> $ra_Ex = bitset<5>(31);
			string $Ra_Ex = $ra_Ex.to_string();
			EX_MEM.push($Ra_Ex);
			EX_MEM.push("1");
			EX_MEM.push("0");
			EX_MEM.push("0");
			EX_MEM.push(instructionEx);

			counter = programCounter.to_ulong();
			counter = counter + 4;
			bitset<32> next(counter);
			programCounter = next;

			bitset<4> next4MSB(programCounter.to_string().substr(0, 4));
			string nextAddr = next4MSB.to_string() + Jaddress_Ex + "00";
			bitset<32> nextPc(nextAddr);
			programCounter = nextPc;
			instr_fetch = true;
		}
	}
	void ex_mem_not_empty()
	{
		string type_func_Mem = EX_MEM.pop();

		if (type_func_Mem == "add" || type_func_Mem == "sub" || type_func_Mem == "srl" || type_func_Mem == "sll")
		{
			aluresult_Mem = EX_MEM.pop();
			$rd_address_Mem = EX_MEM.pop();
			regwrite_Mem = EX_MEM.pop();
			Mem_write_Mem = EX_MEM.pop();
			Mem_read_Mem = EX_MEM.pop();
			instructionMem = EX_MEM.pop();
			cout << "Memory Operation: " << instructionMem;
			cout << "\n";

			MEM_WB.push(type_func_Mem);
			MEM_WB.push(aluresult_Mem);
			MEM_WB.push($rd_address_Mem);
			MEM_WB.push(regwrite_Mem);
			MEM_WB.push(instructionMem);
		}
		else if (type_func_Mem == "lw")
		{
			aluresult_Mem = EX_MEM.pop();
			$rd_address_Mem = EX_MEM.pop();
			regwrite_Mem = EX_MEM.pop();
			Mem_write_Mem = EX_MEM.pop();
			Mem_read_Mem = EX_MEM.pop();
			instructionMem = EX_MEM.pop();
			cout << "Memory Operation: " << instructionMem;
			cout << "\n";

			bitset<32> bit_alu = bitset<32>(aluresult_Mem);
			bitset<32> operand1_Mem = mydatamem.memoryopr(bit_alu, check1, write, read);

			MEM_WB.push(type_func_Mem);
			MEM_WB.push(operand1_Mem.to_string());
			MEM_WB.push($rd_address_Mem);
			MEM_WB.push(regwrite_Mem);
			MEM_WB.push(instructionMem);
		}
		else if (type_func_Mem == "sw")
		{
			aluresult_Mem = EX_MEM.pop();
			string $rd_value_Mem = EX_MEM.pop();
			regwrite_Mem = EX_MEM.pop();
			Mem_write_Mem = EX_MEM.pop();
			Mem_read_Mem = EX_MEM.pop();
			instructionMem = EX_MEM.pop();
			cout << "Memory Operation: " << instructionMem;
			cout << "\n";

			bitset<32> bit_alu = bitset<32>(aluresult_Mem);
			bitset<32> bit_rd = bitset<32>($rd_value_Mem);
			bitset<32> temp = mydatamem.memoryopr(bit_alu, bit_rd, read, write);
		}
		else if (type_func_Mem == "jal")
		{
			aluresult_Mem = EX_MEM.pop();	//pc
			$rd_address_Mem = EX_MEM.pop(); //$ra_address
			regwrite_Mem = EX_MEM.pop();
			Mem_write_Mem = EX_MEM.pop();
			Mem_read_Mem = EX_MEM.pop();
			instructionMem = EX_MEM.pop();
			cout << "Memory Operation: " << instructionMem;
			cout << "\n";

			MEM_WB.push(type_func_Mem);
			MEM_WB.push(aluresult_Mem);
			MEM_WB.push($rd_address_Mem);
			MEM_WB.push(regwrite_Mem);
			MEM_WB.push(instructionMem);
		}
	}
	void mem_wb_not_empty()
	{
		string type_func_Wb = MEM_WB.pop();

		if (type_func_Wb == "add" || type_func_Wb == "sub" || type_func_Wb == "srl" || type_func_Wb == "sll" || type_func_Wb == "lw" || type_func_Wb == "jal")
		{
			aluresult_Wb = MEM_WB.pop();   //pc
			$rd_address_Wb = MEM_WB.pop(); //$ra_address
			regwrite_Wb = MEM_WB.pop();
			instructionWb = MEM_WB.pop();
			cout << "WriteBack Operation: " << instructionWb;
			cout << "\n";

			bitset<5> bit_rd = bitset<5>($rd_address_Wb);
			bitset<32> bit_alu = bitset<32>(aluresult_Wb);
			myregfile.RegisterAccess(check, check, bit_rd, bit_alu, write);
			myregfile.Impart_DH(bit_rd, true);
		}
	}
	int variable_delay(double x)
	{

		array<double, 2> probs = {1 - x, x};
		double r = (double)rand() / (RAND_MAX);
		int index = 0;
		while (r >= 0 and index < probs.size())
		{
			r -= probs[index];
			index += 1;
		}
		return (index - 1);
	}

	int main()
	{

		cout << "Enter the probability in decimal for HIT" << endl;
		cin >> chance;
		cout << "Enter the number of cycles in case of MISS" << endl;
		cin >> N;

		while (1)
		{

			cycles += 1;
			cout << "Cycle No: " << cycles;
			cout << "\n"
				 << endl;

			if (v_delay == true)
			{
				if (y < N - 1)
				{
					y += 1;
					continue;
				}
				v_delay = false;
				return_delay = true;
				y = 1;
				continue;
			}

			if (MEM_WB.isEmpty())
			{
				mem_wb_flag = true;
				if (EX_MEM.isEmpty())
				{
					ex_mem_flag = true;
					if (ID_EX.isEmpty())
					{
						id_ex_flag = true;
						if (IF_ID.isEmpty())
						{
							if_id_flag = true;
							if_id_empty();
							if (endgame == true)
							{
								break;
							}
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
					else if (!ID_EX.isEmpty())
					{
						id_ex_not_empty();
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
				}
				else if (!EX_MEM.isEmpty())
				{
					string itr = EX_MEM.queue[0];
					cout << itr << endl;
					if (itr == "sw" || itr == "lw")
					{
						if (return_delay == true)
						{
							ex_mem_not_empty();
							return_delay = false;
						}
						else if (return_delay == false)
						{
							x = variable_delay(chance);
							if (x == 1)
							{
								cout << "HIT" << endl;
								ex_mem_not_empty();
							}
							if (x == 0)
							{
								cout << "MISS" << endl;
								v_delay = true;
								continue;
							}
						}
					}
					if (itr == "add" || itr == "sub" || itr == "srl" || itr == "sll" || itr == "jal")
					{
						ex_mem_not_empty();
					}
					if (ID_EX.isEmpty())
					{
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
					if (!ID_EX.isEmpty())
					{
						id_ex_not_empty();
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
				}
			}
			else if (!MEM_WB.isEmpty())
			{
				mem_wb_not_empty();
				myregfile.printregisters();
				cout << "\n"
					 << endl;
				//mydatamem.printdatamem();
				//cout << "\n" << endl;

				if (EX_MEM.isEmpty())
				{
					if (ID_EX.isEmpty())
					{
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
					else if (!ID_EX.isEmpty())
					{
						id_ex_not_empty();
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
				}
				else if (!EX_MEM.isEmpty())
				{
					string itr = EX_MEM.queue[0];
					cout << itr << endl;
					if (itr == "sw" || itr == "lw")
					{
						if (return_delay == true)
						{
							ex_mem_not_empty();
							return_delay = false;
						}
						else if (return_delay == false)
						{
							x = variable_delay(chance);
							if (x == 1)
							{
								cout << "HIT" << endl;
								ex_mem_not_empty();
							}
							if (x == 0)
							{
								cout << "MISS" << endl;
								v_delay = true;
								continue;
							}
						}
					}
					if (itr == "add" || itr == "sub" || itr == "srl" || itr == "sll" || itr == "jal")
					{
						ex_mem_not_empty();
					}
					if (ID_EX.isEmpty())
					{
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
					if (!ID_EX.isEmpty())
					{
						id_ex_not_empty();
						if (IF_ID.isEmpty())
						{
							if_id_empty();
							continue;
						}
						else if (!IF_ID.isEmpty())
						{
							if_id_not_empty();
							flag = false;
							continue;
						}
					}
				}
			}
		}
		cout << "Total Instructions Executed: " << instr_numb;
		cout << "\n"
			 << endl;
		cout << "Total cycles: " << cycles - 1;
		cout << "\n"
			 << endl;
		cout << "Instruction Per Cycle: " << instr_numb / (float)(cycles - 1);
		cout << "\n"
			 << endl;
		return 0;
	}
};

int main()
{
	Pipelined_processorVd pp = Pipelined_processorVd();
	pp.main();
	return 0;
}
