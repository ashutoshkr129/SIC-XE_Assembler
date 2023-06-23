#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<cstdio>
#include<map>
#include<vector>

#include "csect.cpp"
#include "utility.cpp"

using namespace std;

vector<csect> csects;

ifstream input_file;
ofstream intermediate;
ofstream error;



void execPass1(){

    vector<string> input;
    string line;

    //Opening the corresponding input and error files.
    //And checking if not exists.
    input_file.open("input.txt");
    if(!input_file.is_open()){
        cout<<"ERROR: Input File not found"<<endl;
        exit(0);
    }
    error.open("error.txt");
    if(!error.is_open()){
        cout<<"ERROR: Error File not found"<<endl;
        exit(0);
    }
    

    //Reading the input file and storing the lines into a vector input.
    while(getline(input_file,line)){
        if(line!="")
            input.push_back(line);
    }


    int locctr=0;
    vector<instruction> instructions;
    vector<pair<string,info_literal>> literals;
    for(int i=0;i<input.size();i++){

        int count=0;
        vector<string> words = parseWords(input[i]);
        string label = words[0];
        string mnemonic = words[1];
        string operand = words[2];


        instruction inst(label,mnemonic,operand);
        if(csects.empty()){
            
            csect section = csect(label,locctr);
            csects.push_back(section);
        }
        if(!csects.back().instructions.empty()){
            inst.address = csects.back().instructions.back().address;
        }
        else{
            inst.address = locctr;
        }
        csects.back().instructions.push_back(inst);
        inst = csects.back().instructions.back();

        if(mnemonic=="START"){
            if(isHex(operand)){
                locctr = strToHex(operand);
            }
            else{
                error << "ERROR: Invalid operand for START" << endl;
                exit(0);
            }
            csects.back().instructions.back().address = locctr;
            csects.back().start = locctr;
        }

        else if(mnemonic=="END"){
            csects.back().instructions.pop_back();
            while(!literals.empty()){
                pair<string,info_literal> lit = literals.back();
                literals.pop_back();
                lit.second.address = locctr;
                csects.back().instructions.push_back(instruction("*",lit.first,""));
                csects.back().instructions.back().address = locctr;
                csects.back().litTab[lit.first] = lit.second;
                locctr += lit.second.length;
            }
            csects.back().end=locctr;
            csects.back().instructions.push_back(inst);
            break;
        }

        else if(mnemonic=="BYTE"){
            csects.back().instructions.back().address = locctr;
            if(operand[0]=='X'){
                locctr += (operand.length()-2)/2;
                csects.back().instructions.back().length = (operand.length()-2)/2;
            }
            else{
                locctr += operand.length()-3;
                csects.back().instructions.back().length = operand.length()-3;
            }
        }

        else if(mnemonic=="WORD"){
            csects.back().instructions.back().address = locctr;
            csects.back().instructions.back().length = 3;
            locctr += 3;
        }

        else if(mnemonic=="RESW"){
            csects.back().instructions.back().address = locctr;
            if(isNumber(operand)){
                csects.back().instructions.back().length = 3*strToDec(operand);
                locctr += 3*strToDec(operand);
            }
            else{
                error<<"Line "<<i+1<<" : Invalid Operand For RESW Statement";
                exit(0);
            }
        }

        else if(mnemonic=="RESB"){
            csects.back().instructions.back().address = locctr;
            if(isNumber(operand)){
                csects.back().instructions.back().length = strToDec(operand);
                locctr += strToDec(operand);
            }
            else{
                error<<"Line "<<i+1<<" : Invalid Operand For RESB Statement";
                exit(0);
            }
        }

        else if(mnemonic=="BASE"){
            csects.back().valid_base=1;
            continue;
        }

        else if(mnemonic=="NOBASE"){
            csects.back().valid_base=0;
            continue;
        }

        else if(mnemonic=="LTORG"){
            while(!literals.empty()){
                pair<string,info_literal> lit = literals.back();
                literals.pop_back();
                lit.second.address = locctr;
                csects.back().instructions.push_back(instruction("*",lit.first,""));
                csects.back().instructions.back().address = locctr;
                csects.back().instructions.back().length = lit.second.length;
                csects.back().litTab[lit.first] = lit.second;
                locctr += lit.second.length;
            }
            continue;
        }

        else if(mnemonic=="EQU"){
            vector<string> params = findParameters(operand);
            int plus=0,minus=0;
            int value=0;
            for(int j=0;j<params.size();j++){
                if(params[j][0]=='+'){
                    if(params[j].substr(1,params[j].length()-1)=="*"){
                        plus++;
                        value += locctr;
                    }
                    else if(isNumber(params[j].substr(1,params[j].length()-1))){
                        value += strToDec(params[j].substr(1,params[j].length()-1));
                    }
                    else if(csects.back().symTab.find(params[j].substr(1,params[j].length()-1))==csects.back().symTab.end()){
                        error<<"Line "<<i+1<<" : Symbol Not Found : "<<(params[j].substr(1,params[j].length()-1));
                        exit(0);
                    }
                    else{
                        if(!csects.back().symTab[params[j].substr(1,params[j].length()-1)].absolute){
                            plus++;
                        }
                        value += csects.back().symTab[params[j].substr(1,params[j].length()-1)].address;
                    }
                }
                else if(params[j][0]=='-'){
                    if(params[j].substr(1,params[j].length()-1)=="*"){
                        minus++;
                        value -= locctr;
                    }
                    else if(isNumber(params[j].substr(1,params[j].length()-1))){
                            value -= strToDec(params[j].substr(1,params[j].length()-1));
                    }
                    else if(csects.back().symTab.find(params[j].substr(1,params[j].length()-1))==csects.back().symTab.end()){
                        error<<"Line "<<i+1<<" : Symbol Not Found : "<<(params[j].substr(1,params[j].length()-1));
                        exit(0);
                    }
                    else{
                        if(!csects.back().symTab[params[j].substr(1,params[j].length()-1)].absolute){
                            minus++;
                        }
                        value -= csects.back().symTab[params[j].substr(1,params[j].length()-1)].address;
                    }
                }
            }
            if((plus-minus)!=0 && (plus-minus)!=1){
                error<<"Line "<<i+1<<" : Invalid Expression For EQU Statement";
                exit(0);
            }
            if(label!="" && csects.back().symTab.find(label)==csects.back().symTab.end()){
                csects.back().symTab[label] = info_sym(value);
            }
            else if(label!="" && csects.back().symTab.find(label)!=csects.back().symTab.end()){
                error<<"ERROR: Duplicate Symbol"<<endl;
                exit(0);
            }
            if((plus-minus)==1){
                csects.back().symTab[label].absolute = false;
            }
            else{
                csects.back().symTab[label].absolute = true;
            }
            continue;
        }
        
        else if(mnemonic=="ORG"){
            continue;
        }
        else if(mnemonic=="CSECT"){
            while(!literals.empty()){
                pair<string,info_literal> lit = literals.back();
                literals.pop_back();
                lit.second.address = locctr;
                csects.back().instructions.push_back(instruction("*",lit.first,""));
                csects.back().instructions.back().address = locctr;
                csects.back().instructions.back().length = lit.second.length;
                csects.back().litTab[lit.first] = lit.second;
                locctr += lit.second.length;
            }
            csects.back().end=locctr;
            csect section = csect(label,0);
            csects.push_back(section);
            locctr=0;
            continue;
        }
        else if(mnemonic=="EXTDEF"){
            continue;
        }
        else if(mnemonic=="EXTREF"){
            vector<string> refs = splitCommas(operand);
            for(string s:refs){
                csects.back().ref.push_back(s);
            }
            continue;
        }
        else if(mnemonic==""){
            continue;
        }
        else if(mnemonic[0]=='+'){
            csects.back().instructions.back().address = locctr;
            csects.back().instructions.back().length = 4;
            locctr += 4;
        }
        else if(label =="."){
            csects.back().instructions.pop_back();
            continue;
        }
        else if(OPTAB.find(mnemonic)!=OPTAB.end()){
            csects.back().instructions.back().address = locctr;
            csects.back().instructions.back().length = OPTAB[mnemonic].format;
            locctr += OPTAB[mnemonic].format;
        }
        else{
            error<<"Line "<<i+1<<" in "<<csects.back().name<<" : Invalid Operation Code: " << mnemonic << "\n";
            exit(0);
        }
        if(label!="" && csects.back().instructions.back().address!=-0x1){    //condition for symbol
                
            if(label!="" && csects.back().symTab.find(label)==csects.back().symTab.end()){
                csects.back().symTab[label] = info_sym(csects.back().instructions.back().address);
            }
            else if(label!="" && csects.back().symTab.find(label)!=csects.back().symTab.end()){
                error<<"ERROR: Duplicate Symbol"<<endl;
                exit(0);
            }
        }
        // Make literal table
        if(operand[0]=='='){
            if(csects.back().litTab.find(operand)==csects.back().litTab.end()){
                csects.back().litTab[operand] = info_literal();
                if(operand[1]=='X'){
                    csects.back().litTab[operand].length = (operand.length()-3)/2;
                    if(isHex(operand.substr(3,operand.length()-4))){
                        csects.back().litTab[operand].value = strToHex(operand.substr(3,operand.length()-4));
                    }
                    else{
                        error<<"Line "<<i+1<<" in "<<csects.back().name<<" : Invalid Hexadecimal Literal: " << operand << "\n";
                        cout << "ERROR: Invalid Hexadecimal Literal" << endl;
                        exit(0);
                    }
                }
                else{
                    csects.back().litTab[operand].length = operand.length()-4;
                    csects.back().litTab[operand].value = strToASCII(operand.substr(3,operand.length()-4));
                }
                literals.push_back({operand,csects.back().litTab[operand]});
            }
        }
    }
    error.close();

    ofstream symtab("symtab.txt");
    for(csect c:csects){
        // print in a pretty format
        symtab<<"Symbol Table for "<<c.name<<'\n';
        symtab<<"\tSymbol\tAddress\tRel/Abs\n";
        for(auto it = c.symTab.begin(); it!=c.symTab.end(); it++){
            symtab<<'\t'<<it->first<<"\t";
            if(it->first.length()<3)
                symtab<<"\t";
            symtab<<toHex(it->second.address,6)<<"\t";
            if(it->second.absolute){
                symtab<<"Abs"<<"\n";
            }
            else{
                symtab<<"Rel\n";
            }
        }
        symtab<<'\n';
    }
    symtab.close();
    // print litTab in litTab.txt
    ofstream lit_tab("lit_tab.txt");
    for(csect c:csects){
        // print in a pretty format

        lit_tab<<"Literal Table for "<<c.name<<'\n';
        lit_tab<<"\tLiteral\tAddress\tLength\tValue\n";
        for(auto it = c.litTab.begin(); it!=c.litTab.end(); it++){
            lit_tab<<'\t'<<it->first<<"\t";
            if(it->first.length()<4)
                lit_tab<<"\t";
            lit_tab<<toHex(it->second.address,6)<<"\t"<<it->second.length;
            if(it->second.length<100)
                lit_tab<<"\t";
            lit_tab<<"\t"<<it->second.value<<"\n";
        }
        lit_tab<<'\n';
    }
    lit_tab.close();
    cout<<"PASS 1 Executed successfully\n";
}

