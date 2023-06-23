#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<cstdio>
#include<map>

using namespace std;


//stores info of opcode (value,format,exist)
struct info_op
{
    int opcode;
    int format;
    bool exist;
    info_op()
    {
        exist=false;
    }
};

struct info_sym
{
    int address;
    bool absolute;
    info_sym(){
        absolute=false;
    }
    info_sym(int val)
    {
        absolute = false;
        address = val;
    }
};

//stores info of register (value,exist)
struct info_reg
{
    int num;
    bool exist;
    int value;
    info_reg()
    {
        exist=false;
    }
};

//stores info of label (address,value,block,exist)
struct info_literal
{
    int address;// address assigned to the operand
    int value;// operand value
    int length; // length of literal
};

struct info_mod
{
    int address;
    int length;
    string name;
    bool plus;
    info_mod()
    {
        name = "";
    }
    info_mod(int address, int length, string name, bool plus)
    {
        this->address = address;
        this->length = length;
        this->name = name;
        this->plus = plus;
    }
};

//This maps the instructions(mnemonic codes) with thier Opcode and their format
std::map<string,info_op> OPTAB;

//This maps the register mnemonic with their number
std::map<string,info_reg> REGISTER;

int reg_num(char a)
{
    if(a=='A') return 0;
    if(a=='X') return 1;
    if(a=='L') return 2;
    if(a=='B') return 3;
    if(a=='S') return 4;
    if(a=='T') return 5;
    if(a=='F') return 6;
    return -1;
}


/*---------OPTABLE-----------*/
void initTables()
{
    cout << "Initializing Tables" << endl;

    OPTAB["ADDR"].opcode=0x90;
    OPTAB["ADDR"].format=2;
    OPTAB["ADDR"].exist=true;

    OPTAB["CLEAR"].opcode=0xb4;
    OPTAB["CLEAR"].format=2;
    OPTAB["CLEAR"].exist=true;

    OPTAB["COMPR"].opcode=0xa0;
    OPTAB["COMPR"].format=2;
    OPTAB["COMPR"].exist=true;

    OPTAB["DIVR"].opcode=0x9c;
    OPTAB["DIVR"].format=2;
    OPTAB["DIVR"].exist=true;

    OPTAB["FIX"].opcode=0xc4;
    OPTAB["FIX"].format=1;
    OPTAB["FIX"].exist=true;

    OPTAB["FLOAT"].opcode=0xc0;
    OPTAB["FLOAT"].format=1;
    OPTAB["FLOAT"].exist=true;

    OPTAB["HIO"].opcode=0xf4;
    OPTAB["HIO"].format=1;
    OPTAB["HIO"].exist=true;

    OPTAB["MULR"].opcode=0x98;
    OPTAB["MULR"].format=2;
    OPTAB["MULR"].exist=true;

    OPTAB["NORM"].opcode=0xC8;
    OPTAB["NORM"].format=1;
    OPTAB["NORM"].exist=true;

    OPTAB["RMO"].opcode=0xAC;
    OPTAB["RMO"].format=2;
    OPTAB["RMO"].exist=true;

    OPTAB["SHIFTL"].opcode=0xA4;
    OPTAB["SHIFTL"].format=2;
    OPTAB["SHIFTL"].exist=true;

    OPTAB["SHIFTR"].opcode=0xA8;
    OPTAB["SHIFTR"].format=2;
    OPTAB["SHIFTR"].exist=true;

    OPTAB["SIO"].opcode=0xF0;
    OPTAB["SIO"].format=1;
    OPTAB["SIO"].exist=true;

    OPTAB["SUBR"].opcode=0x94;
    OPTAB["SUBR"].format=2;
    OPTAB["SUBR"].exist=true;

    OPTAB["SVC"].opcode=0xB0;
    OPTAB["SVC"].format=2;
    OPTAB["SVC"].exist=true;

    OPTAB["TIO"].opcode=0xF8;
    OPTAB["TIO"].format=1;
    OPTAB["TIO"].exist=true;

    OPTAB["TIXR"].opcode=0xB8;
    OPTAB["TIXR"].format=2;
    OPTAB["TIXR"].exist=true;

    // ------------------------------ format 3

    OPTAB["ADD"].opcode=0x18;
    OPTAB["ADD"].format=3;
    OPTAB["ADD"].exist=true;

    OPTAB["ADDF"].opcode=0x58;
    OPTAB["ADDF"].format=3;
    OPTAB["ADDF"].exist=true;

    OPTAB["AND"].opcode=0x40;
    OPTAB["AND"].format=3;
    OPTAB["AND"].exist=true;

    OPTAB["COMP"].opcode=0x28;
    OPTAB["COMP"].format=3;
    OPTAB["COMP"].exist=true;

    OPTAB["COMPF"].opcode=0x88;
    OPTAB["COMPF"].format=3;
    OPTAB["COMPF"].exist=true;

    OPTAB["DIV"].opcode=0x24;
    OPTAB["DIV"].format=3;
    OPTAB["DIV"].exist=true;

    OPTAB["DIVF"].opcode=0x64;
    OPTAB["DIVF"].format=3;
    OPTAB["DIVF"].exist=true;

    OPTAB["J"].opcode=0x3c;
    OPTAB["J"].format=3;
    OPTAB["J"].exist=true;

    OPTAB["JEQ"].opcode=0x30;
    OPTAB["JEQ"].format=3;
    OPTAB["JEQ"].exist=true;

    OPTAB["JGT"].opcode=0x34;
    OPTAB["JGT"].format=3;
    OPTAB["JGT"].exist=true;

    OPTAB["JLT"].opcode=0x38;
    OPTAB["JLT"].format=3;
    OPTAB["JLT"].exist=true;

    OPTAB["JSUB"].opcode=0x48;
    OPTAB["JSUB"].format=3;
    OPTAB["JSUB"].exist=true;

    OPTAB["LDA"].opcode=0x00;
    OPTAB["LDA"].format=3;
    OPTAB["LDA"].exist=true;

    OPTAB["LDB"].opcode=0x68;
    OPTAB["LDB"].format=3;
    OPTAB["LDB"].exist=true;

    OPTAB["LDCH"].opcode=0x50;
    OPTAB["LDCH"].format=3;
    OPTAB["LDCH"].exist=true;

    OPTAB["LDF"].opcode=0x70;
    OPTAB["LDF"].format=3;
    OPTAB["LDF"].exist=true;

    OPTAB["LDL"].opcode=0x08;
    OPTAB["LDL"].format=3;
    OPTAB["LDL"].exist=true;

    OPTAB["LDS"].opcode=0x6C;
    OPTAB["LDS"].format=3;
    OPTAB["LDS"].exist=true;

    OPTAB["LDT"].opcode=0x74;
    OPTAB["LDT"].format=3;
    OPTAB["LDT"].exist=true;

    OPTAB["LDX"].opcode=0x04;
    OPTAB["LDX"].format=3;
    OPTAB["LDX"].exist=true;

    OPTAB["LPS"].opcode=0xD0;
    OPTAB["LPS"].format=3;
    OPTAB["LPS"].exist=true;

    OPTAB["MUL"].opcode=0x20;
    OPTAB["MUL"].format=3;
    OPTAB["MUL"].exist=true;

    OPTAB["MULF"].opcode=0x60;
    OPTAB["MULF"].format=3;
    OPTAB["MULF"].exist=true;

    OPTAB["OR"].opcode=0x44;
    OPTAB["OR"].format=3;
    OPTAB["OR"].exist=true;

    OPTAB["RD"].opcode=0xD8;
    OPTAB["RD"].format=3;
    OPTAB["RD"].exist=true;

    OPTAB["RSUB"].opcode=0x4C;
    OPTAB["RSUB"].format=3;
    OPTAB["RSUB"].exist=true;

    OPTAB["SSK"].opcode=0xEC;
    OPTAB["SSK"].format=3;
    OPTAB["SSK"].exist=true;

    OPTAB["STA"].opcode=0x0C;
    OPTAB["STA"].format=3;
    OPTAB["STA"].exist=true;

    OPTAB["STB"].opcode=0x78;
    OPTAB["STB"].format=3;
    OPTAB["STB"].exist=true;

    OPTAB["STCH"].opcode=0x54;
    OPTAB["STCH"].format=3;
    OPTAB["STCH"].exist=true;

    OPTAB["STF"].opcode=0x80;
    OPTAB["STF"].format=3;
    OPTAB["STF"].exist=true;

    OPTAB["STI"].opcode=0xD4;
    OPTAB["STI"].format=3;
    OPTAB["STI"].exist=true;

    OPTAB["STL"].opcode=0x14;
    OPTAB["STL"].format=3;
    OPTAB["STL"].exist=true;

    OPTAB["STS"].opcode=0x7C;
    OPTAB["STS"].format=3;
    OPTAB["STS"].exist=true;

    OPTAB["STSW"].opcode=0xE8;
    OPTAB["STSW"].format=3;
    OPTAB["STSW"].exist=true;

    OPTAB["STT"].opcode=0x84;
    OPTAB["STT"].format=3;
    OPTAB["STT"].exist=true;

    OPTAB["STX"].opcode=0x10;
    OPTAB["STX"].format=3;
    OPTAB["STX"].exist=true;

    OPTAB["SUB"].opcode=0x1C;
    OPTAB["SUB"].format=3;
    OPTAB["SUB"].exist=true;

    OPTAB["SUBF"].opcode=0x5C;
    OPTAB["SUBF"].format=3;
    OPTAB["SUBF"].exist=true;

    OPTAB["TD"].opcode=0xE0;
    OPTAB["TD"].format=3;
    OPTAB["TD"].exist=true;

    OPTAB["TIX"].opcode=0x2C;
    OPTAB["TIX"].format=3;
    OPTAB["TIX"].exist=true;

    OPTAB["WD"].opcode=0xDC;
    OPTAB["WD"].format=3;
    OPTAB["WD"].exist=true;

    // -------------------------format 4

    OPTAB["+ADD"].opcode=0x18;
    OPTAB["+ADD"].format=4;
    OPTAB["+ADD"].exist=true;

    OPTAB["+ADDF"].opcode=0x58;
    OPTAB["+ADDF"].format=4;
    OPTAB["+ADDF"].exist=true;

    OPTAB["+AND"].opcode=0x40;
    OPTAB["+AND"].format=4;
    OPTAB["+AND"].exist=true;

    OPTAB["+COMP"].opcode=0x28;
    OPTAB["+COMP"].format=4;
    OPTAB["+COMP"].exist=true;

    OPTAB["+COMPF"].opcode=0x88;
    OPTAB["+COMPF"].format=4;
    OPTAB["+COMPF"].exist=true;

    OPTAB["+DIV"].opcode=0x24;
    OPTAB["+DIV"].format=4;
    OPTAB["+DIV"].exist=true;

    OPTAB["+DIVF"].opcode=0x64;
    OPTAB["+DIVF"].format=4;
    OPTAB["+DIVF"].exist=true;

    OPTAB["+J"].opcode=0x3c;
    OPTAB["+J"].format=4;
    OPTAB["+J"].exist=true;

    OPTAB["+JEQ"].opcode=0x30;
    OPTAB["+JEQ"].format=4;
    OPTAB["+JEQ"].exist=true;

    OPTAB["+JGT"].opcode=0x34;
    OPTAB["+JGT"].format=4;
    OPTAB["+JGT"].exist=true;

    OPTAB["+JLT"].opcode=0x38;
    OPTAB["+JLT"].format=4;
    OPTAB["+JLT"].exist=true;

    OPTAB["+JSUB"].opcode=0x48;
    OPTAB["+JSUB"].format=4;
    OPTAB["+JSUB"].exist=true;

    OPTAB["+LDA"].opcode=0x00;
    OPTAB["+LDA"].format=4;
    OPTAB["+LDA"].exist=true;

    OPTAB["+LDB"].opcode=0x68;
    OPTAB["+LDB"].format=4;
    OPTAB["+LDB"].exist=true;

    OPTAB["+LDCH"].opcode=0x50;
    OPTAB["+LDCH"].format=4;
    OPTAB["+LDCH"].exist=true;

    OPTAB["+LDF"].opcode=0x70;
    OPTAB["+LDF"].format=4;
    OPTAB["+LDF"].exist=true;

    OPTAB["+LDL"].opcode=0x08;
    OPTAB["+LDL"].format=4;
    OPTAB["+LDL"].exist=true;

    OPTAB["+LDS"].opcode=0x6C;
    OPTAB["+LDS"].format=4;
    OPTAB["+LDS"].exist=true;

    OPTAB["+LDT"].opcode=0x74;
    OPTAB["+LDT"].format=4;
    OPTAB["+LDT"].exist=true;

    OPTAB["+LDX"].opcode=0x04;
    OPTAB["+LDX"].format=4;
    OPTAB["+LDX"].exist=true;

    OPTAB["+LPS"].opcode=0xD0;
    OPTAB["+LPS"].format=4;
    OPTAB["+LPS"].exist=true;

    OPTAB["+MUL"].opcode=0x20;
    OPTAB["+MUL"].format=4;
    OPTAB["+MUL"].exist=true;

    OPTAB["+MULF"].opcode=0x60;
    OPTAB["+MULF"].format=4;
    OPTAB["+MULF"].exist=true;

    OPTAB["+OR"].opcode=0x44;
    OPTAB["+OR"].format=4;
    OPTAB["+OR"].exist=true;

    OPTAB["+RD"].opcode=0xD8;
    OPTAB["+RD"].format=4;
    OPTAB["+RD"].exist=true;

    OPTAB["+RSUB"].opcode=0x4C;
    OPTAB["+RSUB"].format=4;
    OPTAB["+RSUB"].exist=true;

    OPTAB["+SSK"].opcode=0xEC;
    OPTAB["+SSK"].format=4;
    OPTAB["+SSK"].exist=true;

    OPTAB["+STA"].opcode=0x0C;
    OPTAB["+STA"].format=4;
    OPTAB["+STA"].exist=true;

    OPTAB["+STB"].opcode=0x78;
    OPTAB["+STB"].format=4;
    OPTAB["+STB"].exist=true;

    OPTAB["+STCH"].opcode=0x54;
    OPTAB["+STCH"].format=4;
    OPTAB["+STCH"].exist=true;

    OPTAB["+STF"].opcode=0x80;
    OPTAB["+STF"].format=4;
    OPTAB["+STF"].exist=true;

    OPTAB["+STI"].opcode=0xD4;
    OPTAB["+STI"].format=4;
    OPTAB["+STI"].exist=true;

    OPTAB["+STL"].opcode=0x14;
    OPTAB["+STL"].format=4;
    OPTAB["+STL"].exist=true;

    OPTAB["+STS"].opcode=0x7C;
    OPTAB["+STS"].format=4;
    OPTAB["+STS"].exist=true;

    OPTAB["+STSW"].opcode=0xE8;
    OPTAB["+STSW"].format=4;
    OPTAB["+STSW"].exist=true;

    OPTAB["+STT"].opcode=0x84;
    OPTAB["+STT"].format=4;
    OPTAB["+STT"].exist=true;

    OPTAB["+STX"].opcode=0x10;
    OPTAB["+STX"].format=4;
    OPTAB["+STX"].exist=true;

    OPTAB["+SUB"].opcode=0x1C;
    OPTAB["+SUB"].format=4;
    OPTAB["+SUB"].exist=true;

    OPTAB["+SUBF"].opcode=0x5C;
    OPTAB["+SUBF"].format=4;
    OPTAB["+SUBF"].exist=true;

    OPTAB["+TD"].opcode=0xE0;
    OPTAB["+TD"].format=4;
    OPTAB["+TD"].exist=true;

    OPTAB["+TIX"].opcode=0x2C;
    OPTAB["+TIX"].format=4;
    OPTAB["+TIX"].exist=true;

    OPTAB["+WD"].opcode=0xDC;
    OPTAB["+WD"].format=4;
    OPTAB["+WD"].exist=true;

    // ------------------------------ assembler directives

    OPTAB["WORD"].opcode=0;
    OPTAB["WORD"].format=0;
    OPTAB["WORD"].exist=true;

    OPTAB["RESW"].opcode=0;
    OPTAB["RESW"].format=0;
    OPTAB["RESW"].exist=true;

    OPTAB["RESB"].opcode=0;
    OPTAB["RESB"].format=0;
    OPTAB["RESB"].exist=true;

    OPTAB["BYTE"].opcode=0;
    OPTAB["BYTE"].format=0;
    OPTAB["BYTE"].exist=true;

    OPTAB["BASE"].opcode=0;
    OPTAB["BASE"].format=0;
    OPTAB["BASE"].exist=true;

    OPTAB["NOBASE"].opcode=0;
    OPTAB["NOBASE"].format=0;
    OPTAB["NOBASE"].exist=true;

    OPTAB["EQU"].opcode=0;
    OPTAB["EQU"].format=0;
    OPTAB["EQU"].exist=true;

    OPTAB["START"].opcode=0;
    OPTAB["START"].format=0;
    OPTAB["START"].exist=true;

    OPTAB["END"].opcode=0;
    OPTAB["END"].format=0;
    OPTAB["END"].exist=true;

    OPTAB["LTORG"].opcode=0;
    OPTAB["LTORG"].format=0;
    OPTAB["LTORG"].exist=true;

    OPTAB["CSECT"].opcode=0;
    OPTAB["CSECT"].format=0;
    OPTAB["CSECT"].exist=true;

    OPTAB["EXTDEF"].opcode=0;
    OPTAB["EXTDEF"].format=0;
    OPTAB["EXTDEF"].exist=true;

    OPTAB["EXTREF"].opcode=0;
    OPTAB["EXTREF"].format=0;
    OPTAB["EXTREF"].exist=true;

    OPTAB["USE"].opcode=0;
    OPTAB["USE"].format=0;
    OPTAB["USE"].exist=true;


    /*-------------------------*/


    /*---REGISTERS'-INFO-------*/

    REGISTER["A"].num=0;
    REGISTER["A"].exist=true;

    REGISTER["X"].num=1;
    REGISTER["X"].exist=true;

    REGISTER["L"].num=2;
    REGISTER["L"].exist=true;

    REGISTER["B"].num=3;
    REGISTER["B"].exist=true;

    REGISTER["S"].num=4;
    REGISTER["S"].exist=true;

    REGISTER["T"].num=5;
    REGISTER["T"].exist=true;

    REGISTER["F"].num=6;
    REGISTER["F"].exist=true;

    REGISTER["PC"].num=8;
    REGISTER["PC"].exist=true;

    REGISTER["SW"].num=9;
    REGISTER["SW"].exist=true;

/*--------------------------*/

}


