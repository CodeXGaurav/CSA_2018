#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 256 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
	bitset<32>  PC;
	bool        nop;
};

struct IDStruct {
	bitset<32>  Instr;
	bool        nop;
};

struct EXStruct {
	bitset<32>  Read_data1;
	bitset<32>  Read_data2;
	bitset<16>  Imm;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        is_I_type;
	bool        rd_mem;
	bool        wrt_mem;
	bool        alu_op;     //1 for addu, lw, sw, 0 for subu
	bool        wrt_enable;
	bool        nop;
};

struct MEMStruct {
	bitset<32>  ALUresult;
	bitset<32>  Store_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        rd_mem;
	bool        wrt_mem;
	bool        wrt_enable;
	bool        nop;
};

struct WBStruct {
	bitset<32>  Wrt_data;
	bitset<5>   Rs;
	bitset<5>   Rt;
	bitset<5>   Wrt_reg_addr;
	bool        wrt_enable;
	bool        nop;
};

struct stateStruct {
	IFStruct    IF;
	IDStruct    ID;
	EXStruct    EX;
	MEMStruct   MEM;
	WBStruct    WB;
};

class RF
{
public:
	bitset<32> Reg_data;
	RF()
	{
		Registers.resize(32);
		Registers[0] = bitset<32>(0);
	}

	bitset<32> readRF(bitset<5> Reg_addr)
	{

		Reg_data = Registers[Reg_addr.to_ulong()];
		return Reg_data;
	}

	void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
	{
		Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
	}

	void outputRF()
	{
		ofstream rfout;
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "State of RF:\t" << endl;
			for (int j = 0; j < 32; j++)
			{
				rfout << Registers[j] << endl;
			}
		}
		else cout << "Unable to open file";
		rfout.close();
	}

private:
	vector<bitset<32> >Registers;
};

class INSMem
{
public:
	bitset<32> Instruction;
	INSMem()
	{
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open("imem.txt");
		if (imem.is_open())
		{
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		imem.close();
	}

	bitset<32> readInstr(bitset<32> ReadAddress)
	{
		string insmem;
		insmem.append(IMem[ReadAddress.to_ulong()].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 1].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 2].to_string());
		insmem.append(IMem[ReadAddress.to_ulong() + 3].to_string());
		Instruction = bitset<32>(insmem);		//read instruction memory
		return Instruction;
	}

private:
	vector<bitset<8> > IMem;
};

class DataMem
{
public:
	bitset<32> ReadData;
	DataMem()
	{
		DMem.resize(MemSize);
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open("dmem.txt");
		if (dmem.is_open())
		{
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();
	}

	bitset<32> readDataMem(bitset<32> Address)
	{
		string datamem;
		datamem.append(DMem[Address.to_ulong()].to_string());
		datamem.append(DMem[Address.to_ulong() + 1].to_string());
		datamem.append(DMem[Address.to_ulong() + 2].to_string());
		datamem.append(DMem[Address.to_ulong() + 3].to_string());
		ReadData = bitset<32>(datamem);		//read data memory
		return ReadData;
	}

	void writeDataMem(bitset<32> Address, bitset<32> WriteData)
	{
		DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0, 8));
		DMem[Address.to_ulong() + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
		DMem[Address.to_ulong() + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
		DMem[Address.to_ulong() + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
	}

	void outputDataMem()
	{
		ofstream dmemout;
		dmemout.open("dmemresult.txt");
		if (dmemout.is_open())
		{
			for (int j = 0; j < 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open file";
		dmemout.close();
	}

private:
	vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
	ofstream printstate;
	printstate.open("mystateresult.txt", std::ios_base::app);
	if (printstate.is_open())
	{
		printstate << "State after executing cycle:\t" << cycle << endl;

		printstate << "IF.PC:\t" << state.IF.PC.to_ulong() << endl;
		printstate << "IF.nop:\t" << state.IF.nop << endl;

		printstate << "ID.Instr:\t" << state.ID.Instr << endl;
		printstate << "ID.nop:\t" << state.ID.nop << endl;

		printstate << "EX.Read_data1:\t" << state.EX.Read_data1 << endl;
		printstate << "EX.Read_data2:\t" << state.EX.Read_data2 << endl;
		printstate << "EX.Imm:\t" << state.EX.Imm << endl;
		printstate << "EX.Rs:\t" << state.EX.Rs << endl;
		printstate << "EX.Rt:\t" << state.EX.Rt << endl;
		printstate << "EX.Wrt_reg_addr:\t" << state.EX.Wrt_reg_addr << endl;
		printstate << "EX.is_I_type:\t" << state.EX.is_I_type << endl;
		printstate << "EX.rd_mem:\t" << state.EX.rd_mem << endl;
		printstate << "EX.wrt_mem:\t" << state.EX.wrt_mem << endl;
		printstate << "EX.alu_op:\t" << state.EX.alu_op << endl;
		printstate << "EX.wrt_enable:\t" << state.EX.wrt_enable << endl;
		printstate << "EX.nop:\t" << state.EX.nop << endl;

		printstate << "MEM.ALUresult:\t" << state.MEM.ALUresult << endl;
		printstate << "MEM.Store_data:\t" << state.MEM.Store_data << endl;
		printstate << "MEM.Rs:\t" << state.MEM.Rs << endl;
		printstate << "MEM.Rt:\t" << state.MEM.Rt << endl;
		printstate << "MEM.Wrt_reg_addr:\t" << state.MEM.Wrt_reg_addr << endl;
		printstate << "MEM.rd_mem:\t" << state.MEM.rd_mem << endl;
		printstate << "MEM.wrt_mem:\t" << state.MEM.wrt_mem << endl;
		printstate << "MEM.wrt_enable:\t" << state.MEM.wrt_enable << endl;
		printstate << "MEM.nop:\t" << state.MEM.nop << endl;

		printstate << "WB.Wrt_data:\t" << state.WB.Wrt_data << endl;
		printstate << "WB.Rs:\t" << state.WB.Rs << endl;
		printstate << "WB.Rt:\t" << state.WB.Rt << endl;
		printstate << "WB.Wrt_reg_addr:\t" << state.WB.Wrt_reg_addr << endl;
		printstate << "WB.wrt_enable:\t" << state.WB.wrt_enable << endl;
		printstate << "WB.nop:\t" << state.WB.nop << endl;
	}
	else cout << "Unable to open file";
	printstate.close();
}




bitset<32> signextend (bitset<16> imm)
{
    string sestring;
    //if (imm[15]==0){
        sestring = "0000000000000000"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    //}
    //else{
     //   sestring = "1111111111111111"+imm.to_string<char,std::string::traits_type,std::string::allocator_type>();
    //}
    return (bitset<32> (sestring));

}

class ALU
{
public:
	bitset<32> ALUresult;
	bitset<32> ALUOperation(bool ALUOP, bitset<32> oprand1, bitset<32> oprand2)
	{    // implement the ALU operations by you.
		unsigned long result;

		if (ALUOP == 1) {
			result = oprand1.to_ulong() + oprand2.to_ulong(); // addu operation
		}
		else if (ALUOP == 0) {
			result = oprand1.to_ulong() - oprand2.to_ulong(); // subu operation
		}


		bitset<32> res((int)result);
		ALUresult = res;
		return ALUresult;
	}
};

unsigned long shiftbits(bitset<32> inst, int start)
{

    return ((inst.to_ulong())>>start);

}


int main()
{
	stateStruct state, newstate;
	ALU myALU;
	RF myRF;
	INSMem myInsMem;
	DataMem myDataMem;
	state.IF.PC = 0;
	int cycle = 0;
	bitset<6> opcode, funct;
	bitset<32>Imm;
	bitset<16> Immediate = 0;

	newstate.IF.PC=	0;
	newstate.IF.nop=	0;
	newstate.ID.Instr=	00000000000000000000000000000000;
	newstate.ID.nop=	1;
	newstate.EX.Read_data1=	00000000000000000000000000000000;
	newstate.EX.Read_data2=	00000000000000000000000000000000;
	newstate.EX.Imm=	0000000000000000;
	newstate.EX.Rs=	00000;
	newstate.EX.Rt	= 00000;
	newstate.EX.Wrt_reg_addr=	00000;
	newstate.EX.is_I_type=	0;
	newstate.EX.rd_mem=	0;
	newstate.EX.wrt_mem=	0;
	newstate.EX.alu_op=	1;
	newstate.EX.wrt_enable=	0;
	newstate.EX.nop=	1;
	newstate.MEM.ALUresult=	00000000000000000000000000000000;
	newstate.MEM.Store_data=	00000000000000000000000000000000;
	newstate.MEM.Rs=	00000;
	newstate.MEM.Rt=	00000;
	newstate.MEM.Wrt_reg_addr=	00000;
	newstate.MEM.rd_mem=	0;
	newstate.MEM.wrt_mem=	0;
	newstate.MEM.wrt_enable=	0;
	newstate.MEM.nop=	1;
	newstate.WB.Wrt_data=	00000000000000000000000000000000;
	newstate.WB.Rs=	00000;
	newstate.WB.Rt=	00000;
	newstate.WB.Wrt_reg_addr=	00000;
	newstate.WB.wrt_enable=	0;
	newstate.WB.nop=	1;
	state.IF.nop = 0;
	state.ID.nop =1;
	state.EX.nop =1;
	state.MEM.nop =1;
	state.WB.nop =1;




while(1)
	{
		/* --------------------- WB stage --------------------- */
				if (state.WB.nop==0)
				{
					if (state.WB.wrt_enable==true) {

						myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);

					}
					else;

					if(state.MEM.nop == 1)// for halt condition
					{
					newstate.WB.nop = 1;
					}
				}


				/* --------------------- MEM stage --------------------- *///Load result from or store result to data memory (MEM)


				if (state.MEM.nop==0)
				{
					if (state.MEM.rd_mem==true)

					{
						newstate.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
					}
					else if (state.MEM.wrt_mem==true)
					{
						myDataMem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
					}
					else{
					newstate.WB.Wrt_data = state.MEM.ALUresult;
					}
					newstate.WB.Rs = state.MEM.Rs;
					newstate.WB.Rt = state.MEM.Rt;
					newstate.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
					newstate.WB.wrt_enable = state.MEM.wrt_enable;
					newstate.WB.nop = 0 ;

					if(state.EX.nop == 1)// for halt condition
					{
						newstate.MEM.nop = 1;
							}
					}

				/* --------------------- EX stage --------------------- */
				if (state.EX.nop==0)
				{

					if (state.EX.is_I_type==true)
					{
						Imm = signextend(state.EX.Imm);

						newstate.MEM.ALUresult = myALU.ALUOperation(state.EX.alu_op, state.EX.Read_data1, Imm);

					}
					else
					{
						newstate.MEM.ALUresult = myALU.ALUOperation(state.EX.alu_op, state.EX.Read_data1, state.EX.Read_data2);
					}

					// propogate the rest of the states as it is in MEM stage.
						newstate.MEM.Store_data = state.EX.Read_data2;
						newstate.MEM.Rs = state.EX.Rs;
						newstate.MEM.Rt = state.EX.Rt;
						newstate.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
						newstate.MEM.rd_mem = state.EX.rd_mem;
						newstate.MEM.wrt_mem = state.EX.wrt_mem;
						newstate.MEM.wrt_enable = state.EX.wrt_enable;
						newstate.MEM.nop = 0;
						if(state.ID.nop == 1)
						{
							newstate.EX.nop = 1;
						}

				}



				/* --------------------- ID/RF stage --------------------- */
				if (state.ID.nop==0)	// for decoding opcode and finding it what it is EXactly
				{
					opcode = bitset<6> (shiftbits(state.ID.Instr, 26));
					funct = bitset<6> (shiftbits(state.ID.Instr, 0));
					for (int i = 0; i < 16; i++)
					{
					Immediate[i] = state.ID.Instr[i];
					}

					if (opcode==0x00) // for R type instructions
					{
						if (funct == 0x23)//function subu =0x23 // further identification of addu / subu
						{
						newstate.EX.alu_op = false;
						}
						else if(funct == 0x21) /*function addu = 0x21*/
						{
						newstate.EX.alu_op = true;
						}

						newstate.EX.Rs = bitset<5> (shiftbits(state.ID.Instr, 21));
						newstate.EX.Rt = bitset<5> (shiftbits(state.ID.Instr, 16));
						newstate.EX.Wrt_reg_addr = bitset<5> (shiftbits(state.ID.Instr, 11));
						newstate.EX.is_I_type = false;
						newstate.EX.wrt_enable = true;
						newstate.EX.rd_mem = false;
						newstate.EX.wrt_mem = false;
						//newstate.EX.Imm.to_string<char,std::string::traits_type,std::string::allocator_type>()="0011100000100011";
						cout<<"Rs="<<newstate.EX.Rs<<endl;
						cout<<"Rt="<<newstate.EX.Rt<<endl;
						newstate.EX.Read_data1 = myRF.readRF(newstate.EX.Rs);
						cout<<"read_data1="<<newstate.EX.Read_data1<<endl;
						newstate.EX.Read_data2 = myRF.readRF(newstate.EX.Rt);
						cout<<"read_data2="<<newstate.EX.Read_data2<<endl;
						//cout << " The Instruction is R-Type" << endl;
					}

					else //it will be I type instruction
					{
						newstate.EX.is_I_type = 1;

						newstate.EX.Imm = Immediate;
						newstate.EX.Rs = bitset<5>(shiftbits(state.ID.Instr, 21));
						newstate.EX.Rt = bitset<5>(shiftbits(state.ID.Instr,16));
						newstate.EX.Wrt_reg_addr = bitset<5>(shiftbits(state.ID.Instr,16));
						if (opcode == 0x23)//0x23 for LW instruction )
						{
							//cout << " This is Load Word Instruction" << "\n";
							newstate.EX.rd_mem = true;
							newstate.EX.wrt_enable = true;
							newstate.EX.alu_op = true;
							newstate.EX.wrt_mem = false;
						}
						else if (opcode == 0x2B) // 0x2B for store word instruction
						{
							//cout << " This is Store Word Instruction" << "\n";
							newstate.EX.wrt_mem = true;
							newstate.EX.wrt_enable = false;
							newstate.EX.alu_op = true;
							newstate.EX.rd_mem = false;
						}

						newstate.EX.Read_data1 = myRF.readRF(newstate.EX.Rs);
						newstate.EX.Read_data2 = myRF.readRF(newstate.EX.Rt);

					}

					//newstate.EX.Read_data1 = myRF.readRF(newstate.EX.Rs);
					//newstate.EX.Read_data2 = myRF.readRF(newstate.EX.Rt);
 					newstate.EX.nop = 0;


				}



				/* --------------------- IF stage --------------------- */
		if (state.IF.nop==0)
		{
			newstate.ID.Instr = myInsMem.readInstr(state.IF.PC);

			if (newstate.ID.Instr.to_ulong() == 4294967295)  /* check for HALT condition */
			{
				newstate.ID.nop = 1;
				newstate.IF.nop = 1;
				goto mylabel;

			}


			// Increment the Program Counter to read the nEXt instruction
			newstate.IF.PC = state.IF.PC.to_ulong() + 4;

			newstate.ID.nop = 0;


			//cout << "IF stage complete" << endl;
		}

	mylabel:
	if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
			break;

		printState(newstate, cycle); //print states after EXecuting cycle 0, cycle 1, cycle 2 ...

		state = newstate; /*The end of the cycle and updates the current state with the values calculated in this cycle */
		cycle++;

	}


	myRF.outputRF(); // dump RF;
	myDataMem.outputDataMem(); // dump data mem

	return 0;
}

