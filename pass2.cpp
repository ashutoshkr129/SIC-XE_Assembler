#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<cstdio>
#include<map>
#include<vector>
#include<math.h>
#include <algorithm>

#include "pass1.cpp"

using namespace std;

void writeIntermediateFile(){
    intermediate.open("intermediate.txt");
    if(!intermediate.is_open()){
        cout<<"ERROR: Intermediaste File not found"<<endl;
        exit(0);
    }

    int N = csects.size();

    for(int _i=0;_i<N;_i++){
        if(csects[_i].name.length()<=6){
            intermediate<<"\n"<<csects[_i].name;
            intermediate<<"\n";
        }
        else{
            intermediate<<csects[_i].name.substr(0,6)<<"\n";
        }
        instruction inst;
        for(int j=0;j<csects[_i].instructions.size();j++){
            inst = csects[_i].instructions[j];
            if(inst.label=="."){
                continue;
            }
            if(inst.label=="*"){
                if(inst.mnemonic.length()<2){
                    error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid literal: "<<inst.mnemonic<<"\n";
                    exit(0);
                }
                if(inst.mnemonic.substr(0,2)=="=C"){
                    if(inst.mnemonic.length()>34){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Literal too long.\n";
                        exit(0);
                    }
                    inst.objCode = toHex(strToASCII(inst.mnemonic.substr(3,inst.mnemonic.length()-4)),2*(inst.mnemonic.length()-4));
                }
                else if(inst.mnemonic.substr(0,2)=="=X"){
                    if(inst.mnemonic.length()>19){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Literal too long.\n";
                        exit(0);
                    }
                    inst.objCode = inst.mnemonic.substr(3,inst.mnemonic.length()-4);
                }
                else{
                    error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid literal: "<<inst.mnemonic<<"\n";
                    exit(0);
                }
            }
            else if(inst.mnemonic=="BYTE" || inst.mnemonic=="WORD"){
                if(inst.operand[0]=='C'){
                    inst.objCode = toHex(strToASCII(inst.operand.substr(2,inst.operand.length()-3)),2*(inst.operand.length()-3));
                }
                else if(inst.operand[0]=='X'){
                    inst.objCode = inst.operand.substr(2,inst.operand.length()-3);
                }
                else if(isNumber(inst.operand)){
                    if(strToDec(inst.operand)>pow(2,23)-1 || strToDec(inst.operand)<pow(2,23)*(-1)){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Value too large.\n";
                        exit(0);
                    }
                    inst.objCode = toHex(strToDec(inst.operand),6);
                }
                else if(inst.mnemonic=="WORD"){
                    vector<string> params = findParameters(inst.operand);
                    if(params.back()=="**"){
                        error<<"Line "<<j+1<<" : Invalid Expression For WORD Statement";
                        exit(0);
                    }
                    int value=0;
                    for(int p=0;p<params.size();p++){
                        if(params[p][0]=='+'){
                            if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),params[p].substr(1,params[p].length()-1))!=csects[_i].ref.end()){
                                csects[_i].mod_record.push_back(info_mod(inst.address,2*inst.length,params[p].substr(1,params[p].length()-1),1));
                            }
                            if(params[p].substr(1,params[j].length()-1)=="*"){
                                value += inst.address+inst.length;
                            }
                            else if(isNumber(params[p].substr(1,params[p].length()-1))){
                                value += strToDec(params[p].substr(1,params[p].length()-1));
                            }
                            else if(csects[_i].symTab.find(params[p].substr(1,params[p].length()-1))!=csects[_i].symTab.end()){
                                value += csects[_i].symTab[params[p].substr(1,params[p].length()-1)].address;
                            }
                            else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),params[p].substr(1,params[p].length()-1))==csects[_i].ref.end()){
                                error<<"Line "<<j+1<<" : Symbol Not Found : "<<(params[p].substr(1,params[p].length()-1));
                                exit(0);
                            }
                        }
                        else if(params[p][0]=='-'){
                            if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),params[p].substr(1,params[p].length()-1))!=csects[_i].ref.end()){
                                csects[_i].mod_record.push_back(info_mod(inst.address,2*inst.length,params[p].substr(1,params[p].length()-1),0));
                            }
                            if(params[p].substr(1,params[p].length()-1)=="*"){
                                value -= inst.address+inst.length;
                            }
                            else if(isNumber(params[p].substr(1,params[p].length()-1))){
                                    value -= strToDec(params[p].substr(1,params[p].length()-1));
                            }
                            else if(csects[_i].symTab.find(params[p].substr(1,params[p].length()-1))!=csects[_i].symTab.end()){
                                value -= csects[_i].symTab[params[p].substr(1,params[p].length()-1)].address;
                            }
                            else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),params[p].substr(1,params[p].length()-1))==csects[_i].ref.end()){
                                error<<"Line "<<j+1<<" : Symbol Not Found : "<<(params[p].substr(1,params[p].length()-1));
                                exit(0);
                            }
                        }
                    }
                    inst.objCode = toHex(value,6);
                }
                else{
                    error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid value for BYTE or WORD\n";
                    exit(0);
                }
            }
            else if(inst.mnemonic=="EXTDEF"){
                vector<string> defs = splitCommas(inst.operand);
                for(int i=0;i<defs.size();i++){
                    if(defs[i].length()>6){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : External definition too long.\n";
                        exit(0);
                    }
                    else if(csects[_i].symTab.find(defs[i])==csects[_i].symTab.end()){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : External definition not found.\n";
                        exit(0);
                    }
                    else{
                        csects[_i].def[defs[i]] = csects[_i].symTab[defs[i]].address;
                    }
                }
            }
            else if(OPTAB.find(inst.mnemonic)==OPTAB.end()){
                error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid mnemonic :"<<inst.mnemonic<<"\n";
                exit(0);
            }
            else if(OPTAB[inst.mnemonic].format==0){
            }
            else if(OPTAB[inst.mnemonic].format==1){
                inst.objCode = toHex(OPTAB[inst.mnemonic].opcode,2);
            }
            else if(OPTAB[inst.mnemonic].format==2){
                vector<string> regs = splitCommas(inst.operand);
                int r1 = 0, r2 = 0;
                if(regs.size()>=1){
                    if(isNumber(regs[0])){
                        r1 = strToDec(regs[0]);
                    }
                    else if(REGISTER.find(regs[0])!=REGISTER.end()){
                        r1 = REGISTER[regs[0]].num;
                    }
                    else{
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid register" << regs[0] << "\n";
                        exit(0);
                    }
                }
                if(regs.size()>=2){
                    if(isNumber(regs[1])){
                        r2 = strToDec(regs[1]);
                    }
                    else if(REGISTER.find(regs[1])!=REGISTER.end()){
                        r2 = REGISTER[regs[1]].num;
                    }
                    else{
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid register" << regs[1] << "\n";
                        exit(0);
                    }
                }
                inst.objCode = toHex(OPTAB[inst.mnemonic].opcode,2) + toHex(r1,1) + toHex(r2,1);
            }
            else if(OPTAB[inst.mnemonic].format==3){
                if(inst.mnemonic=="RSUB"){
                    inst.objCode = toHex(OPTAB[inst.mnemonic].opcode + 3,2) + toHex(0,4);
                    if(inst.operand!=""){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : RSUB should not have operand: " << inst.operand << "\n";
                        exit(0);
                    }
                }
                else{
                    int n = 0, i = 0, x = 0, b = 0, p = 0, e = 0;
                    int absAddress = 0;
                    bool isnum = false;
                    if(inst.operand[0]=='#'){
                        if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Both immediate and indexed addressing.\n";
                            exit(0);
                        }
                        n = 0;
                        i = 1;
                        if(isNumber(inst.operand.substr(1,inst.operand.length()-1))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(1,inst.operand.length()-1));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(1,inst.operand.length()-1)].value;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }   
                    }
                    else if(inst.operand[0]=='@'){
                        if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Both immediate and indexed addressing.\n";
                            exit(0);
                        }
                        n = 1;
                        i = 0;
                        if(isNumber(inst.operand.substr(1,inst.operand.length()-1))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(1,inst.operand.length()-1));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    else if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                        x=1;n=1;i=1;
                        if(isNumber(inst.operand.substr(0, inst.operand.length() - 2))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(0,inst.operand.length()-2));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(0,inst.operand.length()-2)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(0,inst.operand.length()-2)].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    else{
                        n = 1;
                        i = 1;
                        x=0;
                        if(isNumber(inst.operand)){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand);
                        }
                        else if(csects[_i].symTab.find(inst.operand)!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand)!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand)!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    

                    int disp = absAddress - (inst.address + 3);
                    if(disp>=pow(2,11) || disp<(-pow(2,11))){
                        b=1;p=0;e=0;
                    }
                    else{
                        p=1;b=0;e=0;
                    }

                    int relative_address = absAddress;
                    if(b==1){
                        relative_address -= REGISTER["B"].value;
                    }
                    else if(p==1){
                        relative_address -= (inst.address + inst.length);
                    }
                    if(x==1){
                        relative_address -= REGISTER["X"].value;
                    }
                    if(isnum){
                        p=0;b=0;x=0;
                        relative_address = absAddress;
                    }
                    inst.objCode = toHex(OPTAB[inst.mnemonic].opcode + 2*n + i,2) + toHex(x*8 + b*4 + p*2 + e*1, 1) + toHex(relative_address,3);
                }
            }
            else{
                if(inst.mnemonic=="+RSUB"){
                    inst.objCode = toHex(OPTAB[inst.mnemonic].opcode + 3,2) + toHex(0,6);
                    if(inst.operand!=""){
                        error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : RSUB should not have operand: " << inst.operand << "\n";
                        exit(0);
                    }
                }
                else{
                    int n = 0, i = 0, x = 0, b = 0, p = 0, e = 1;
                    int absAddress = 0;
                    bool isnum = false;
                    if(inst.operand[0]=='#'){
                        if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Both immediate and indexed addressing.\n";
                            exit(0);
                        }
                        n = 0;
                        i = 1;
                        if(isNumber(inst.operand.substr(1,inst.operand.length()-1))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(1,inst.operand.length()-1));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    else if(inst.operand[0]=='@'){
                        if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].ref.end()){
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, inst.operand.substr(1,inst.operand.length()-1),1));
                        }
                        else{
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, csects[_i].name,1));
                        }
                        if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Both immediate and indexed addressing.\n";
                            exit(0);
                        }
                        n = 1;
                        i = 0;
                        if(isNumber(inst.operand.substr(1,inst.operand.length()-1))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(1,inst.operand.length()-1));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(1,inst.operand.length()-1)].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(1,inst.operand.length()-1))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    else if(inst.operand.length()>1 && inst.operand.substr(inst.operand.length()-2, 2) == ",X"){
                        if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].ref.end()){
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, inst.operand.substr(0,inst.operand.length()-2),1));
                        }
                        else{
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, csects[_i].name,1));
                        }
                        x=1;n=1;i=1;
                        if(isNumber(inst.operand.substr(0, inst.operand.length() - 2))){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand.substr(0,inst.operand.length()-2));
                        }
                        else if(csects[_i].symTab.find(inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand.substr(0,inst.operand.length()-2)].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand.substr(0,inst.operand.length()-2)].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand.substr(0,inst.operand.length()-2))!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    else{
                        if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand)!=csects[_i].ref.end()){
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, inst.operand,1));
                        }
                        else{
                            csects[_i].mod_record.push_back(info_mod(inst.address+1, 5, csects[_i].name,1));
                        }
                        n = 1;
                        i = 1;
                        x=0;
                        if(isNumber(inst.operand)){
                            isnum = true;
                            p=0;
                            absAddress = strToDec(inst.operand);
                        }
                        else if(csects[_i].symTab.find(inst.operand)!=csects[_i].symTab.end()){
                            absAddress = csects[_i].symTab[inst.operand].address;
                        }
                        else if(csects[_i].litTab.find(inst.operand)!=csects[_i].litTab.end()){
                            absAddress = csects[_i].litTab[inst.operand].address;
                        }
                        else if(std::find(csects[_i].ref.begin(),csects[_i].ref.end(),inst.operand)!=csects[_i].ref.end()){
                            absAddress = 0;
                        }
                        else{
                            error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Invalid operand: " << inst.operand << "\n";
                            exit(0);
                        }
                    }
                    
                    inst.objCode = toHex(OPTAB[inst.mnemonic].opcode + 2*n + i,2) + toHex(x*8 + b*4 + p*2 + e*1, 1) + toHex(absAddress,5);
                }
            }
            if(inst.objCode.length()>8){
                error<<"Line "<<j+1<<" in "<<csects[_i].name<<" : Object code too long: " << inst.objCode << "\n";
                exit(0);
            }
            intermediate<<toHex(inst.address,6) <<"   "<<spacePadding(inst.label, 10)<<"   "<<spacePadding(inst.mnemonic, 10)<<"   "<<spacePadding(inst.operand, 30)<<" ";
            intermediate<<inst.objCode<<"\n";
            csects[_i].instructions[j] = inst;
        }
    }

    intermediate.close();
    cout<<"Intermediate File Written.\n";
}


void createObjProg(){
    int N = csects.size();
    // write object program
    ofstream obj;
    obj.open("object_program.txt");
    for(int i=0;i<N;i++){
        if(csects[i].name.length()>6){
            error<<"Section name too long: " << csects[i].name << "\n";
            exit(0);
        }
        obj<<"H"<<spacePadding(csects[i].name,6)<<""<<toHex(csects[i].start,6)<<""<<toHex(csects[i].end - csects[i].start,6)<<"\n";
        if(csects[i].def.size()>0){
            obj<<"D";
            for(auto it=csects[i].def.begin();it!=csects[i].def.end();it++){
                if(it->first.length()>6){
                    error<<"Symbol name too long: " << it->first << "\n";
                    exit(0);
                }
                obj<<spacePadding(it->first,6)<<""<<toHex(it->second,6)<<"";
            }
            obj<<"\n";
        }
        if(csects[i].ref.size()>0){
            obj<<"R";
            for(auto it=csects[i].ref.begin();it!=csects[i].ref.end();it++){
                if(it->length()>6){
                    error<<"Symbol name too long: " << *it << "\n";
                    exit(0);
                }
                obj<<spacePadding(*it,6)<<"";
            }
            obj<<"\n";
        }
        // obj << "T";
        int curr_sz = 0;
        int sz=0;
        int _j = 0;
        for(int j=0;j<csects[i].instructions.size();j++){
            if(csects[i].instructions[j].objCode.length()>0){
                curr_sz = csects[i].instructions[j].objCode.length()/2;
                if((sz + curr_sz) > 30){
                    obj<<"T"<<toHex(csects[i].instructions[_j].address,6)<<""<<toHex((sz),2)<<"";
                    for(int k=_j;k<j;k++){
                        if(csects[i].instructions[k].objCode.length()>0)
                            obj<<csects[i].instructions[k].objCode;
                        if(k==j-1)
                            obj<<"";
                    }
                    obj<<"\n";
                    _j = j;
                    sz = curr_sz;
                }
                else{
                    sz += csects[i].instructions[j].objCode.length()/2;
                }
            }
            else if(csects[i].instructions[j].mnemonic == "RESW" || csects[i].instructions[j].mnemonic == "RESB"){
                if(sz>0){
                    obj<<"T"<<toHex(csects[i].instructions[_j].address,6)<<""<<toHex((sz),2)<<"";
                    for(int k=_j;k<j;k++){
                        obj<<csects[i].instructions[k].objCode;
                    }
                    obj<<"\n";
                    _j = j+1;
                    sz = 0;
                }
            }
            else if(sz==0){
                _j = j+1;
            }
        }
        if(sz>0){
            obj<<"T"<<toHex(csects[i].instructions[_j].address,6)<<""<<toHex((sz),2)<<"";
            for(int k=_j;k<csects[i].instructions.size();k++){
                obj<<csects[i].instructions[k].objCode<<"";
            }
            obj<<"\n";
        }
        // modification records
        for(int j=0;j<csects[i].mod_record.size();j++){
            obj<<"M"<<toHex(csects[i].mod_record[j].address,6)<<""<<toHex(csects[i].mod_record[j].length,2)<<""<<((csects[i].mod_record[j].plus)?"+":"-")<<csects[i].mod_record[j].name<<"\n";
        }
        if(i==0)
            obj<<"E"<<toHex(csects[i].start,6)<<"\n\n";
        else
            obj<<"E\n\n";
    }
    intermediate.close();
    obj.close();
    cout<<"Object Program File Written.\n";


}

int main(){
    initTables();
    execPass1();
    writeIntermediateFile();
    createObjProg();
    return 0;
}