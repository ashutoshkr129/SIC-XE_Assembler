#include<string>
#include<map>
#include<vector>
#include<utility>

#include "tables.cpp"

using namespace std;


struct instruction{
    int address;    //locctr
    string label;
    string mnemonic;
    string operand;
    string objCode;
    int length;

    instruction(){
        this->address=-0x1;
        this->label="";
        this->mnemonic="";
        this->operand="";
        this->objCode="";
        this->length=0;
    }

    instruction(string label,string mnemonic,string operand){
        this->address=-0x1;
        this->label=label;
        this->mnemonic=mnemonic;
        this->operand=operand;
        this->objCode="";
        this->length=0;
    }
};

struct csect
{
    string name;
    int start;
    int end;
    bool valid_base;
    map<string,int> def;
    vector<string> ref;
    map<string, info_literal> litTab;
	vector<info_mod> mod_record;   // MODIFICATION RECORD
    vector<instruction> instructions;
	map<string, info_sym> symTab;
    csect(string name, int init_locctr){
        this->start=init_locctr;
        this->name=name;
        this->valid_base=0;
    }

};