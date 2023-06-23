<a name="readme-top"></a>

# SIC/XE Assembler

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="## Introduction">Introduction</a>
    </li>
    <li>
      <a href="## Assembler">Assembler</a>
      <ul>
        <li>
          <a href="### Assembler : Design">Assembler : Design</a>
          </li>
      </ul>
    </li>
    <li><a href="### Compilation and Execution">Compilation and Execution</a></li>
  </ol>
</details>

***

<!-- ABOUT THE PROJECT -->
## Introduction


SIC/XE stands for "Simpliﬁed Instructional Computer Extra Equipment". It is an
advanced version of the SIC architecture that includes additional features and
capabilities. SIC/XE is designed to be upward-compatible with SIC, which means
that programs written for the SIC architecture can be run on the SIC/XE
architecture without modiﬁcation. This makes it easier for developers to migrate
their programs to the new architecture.

---

<!-- GETTING STARTED -->
## Assembler

The Assembler is implemented using C++ programming language.
This Assembler implements following Machine-Independent Features.
1. Literals
2. Symbol Deﬁning Statements
3. Expressions
4. Control Sections

### Assembler : Design

#### tables.cpp

This ﬁle contains the data structures required:
* The info_op, info_reg, info_sym, info_literal, and info_mod structs are
used to store information about opcodes, registers, symbols, literals,
and modiﬁcations respectively.

* OPTAB map contains information about opcode mnemonics and their
corresponding opcode values and formats as it is mapped to the
info_op struct. Similarly, the REGISTER map contains information about
register mnemonics and their corresponding register numbers as it is
mapped to the info_op struct..
* The reg_num function returns the corresponding register number
given the register mnemonic.
* The init_tables function initializes the OPTAB and REGISTER maps with
their respective values.

#### utility.cpp
It contains useful functions that will be required by other ﬁles .
* bool isNumber(string str): This function takes a string as input and
returns a boolean indicating whether the string represents a decimal
number or not. The function checks if each character in the string is a
digit or not and returns false if any non-digit character is found.
* int strToDec(string s): This function takes a string representing a
decimal number as input and returns the corresponding integer value.
The function works by iterating over the characters in the string from
the right end and multiplying each digit by a power of 10 and adding it
to the result.
* int strToHex(string s): This function takes a string representing a
hexadecimal number as input and returns the corresponding integer
value. The function works similarly to the strToDec function, but
instead multiplies each digit by a power of 16.
* int strToASCII(string s): This function takes a string as input and
returns the corresponding integer value obtained by treating the string
as a sequence of ASCII characters. The function iterates over the characters in the string from the right end and multiplies the ASCII
value of each character by a power of 256 and adds it to the result.
* bool isHex(string str): This function takes a string as input and returns
a boolean indicating whether the string represents a valid
hexadecimal number or not. The function checks if each character in
the string is a digit or a valid hexadecimal character (i.e., A-F or a-f).
* parseWords(string line): This function takes a string
representing a line of instructions as input and returns a vector
containing three strings, which represent the three parameters of the
instruction. The function splits the input string into words using spaces
as delimiters and returns the ﬁrst three words as a vector.
* string toHex(int num, int len): This function takes an integer value and
a length as input and returns a string representing the hexadecimal
value of the integer.

These functions are useful for processing strings of numbers and characters
and converting them to their corresponding integer or hexadecimal
representations. These operations are commonly used in computer
programming and are essential for working with assembly language and
machine code.
#### csect.cpp
The given section contains two structs: instruction and csect.

The instruction struct is used to represent an assembly instruction. It
contains the following members:
* address: An integer representing the location counter (locctr) of
the instruction.
* label: A string representing the label of the instruction.
* mnemonic: A string representing the mnemonic of the
instruction (e.g. "LDA", "ADD", etc.).


* operand: A string representing the operand of the instruction
(e.g. "X'0A'", "BUFFER", etc.).
* objCode: A string representing the object code of the
instruction, which will be generated during the assembly
process.
* length: An integer representing the length of the instruction in
bytes.

The csect struct is used to represent a control section, which is a
portion of the program that is loaded into memory as a single unit. It
contains the following members:
* name: A string representing the name of the control section.
* start: An integer representing the starting address of the control
section.
* end: An integer representing the ending address of the control
section.
* valid_base: A boolean indicating whether a base register has
been set for this control section.
* def: A map that stores the deﬁned symbols and their
corresponding addresses.
* ref: A vector that stores the names of the external symbols that
are referenced in this control section.
* litTab: A map that stores the literals used in the program and
their corresponding information, such as the literal value and its
length.
* mod_record: A vector that stores the modiﬁcation records for
this control section. A modiﬁcation record indicates that a
particular memory location needs to be modiﬁed at link-edit
time.
* instructions: A vector that stores the instructions of the program
in this control section.




* symTab: A map that stores the symbols used in the program and
their corresponding information, such as their addresses and
whether they are relative or absolute.

These structs are used to store information about the program being
assembled, including its instructions, symbols, and control sections. The
information stored in these structs is used during the assembly process to
generate the object code for the program.
#### pass1.cpp
* The program opens the input file, checks for errors, and reads its contents line by line.
* Variables are initialized, including a location counter, instruction and literal vectors, and control section vector.
* Each line is parsed into label, mnemonic, and operand, and a control section is created if not defined.
* Instructions are processed based on their mnemonics, such as setting the location counter for "START" mnemonic.
* Symbol and literal tables, along with instruction addresses and lengths, are written to the intermediate file.
* The program handles various instructions and assembler directives to generate the symbol table, literal table, and intermediate code file.
#### pass2.cpp
* The function "createObjProg()" opens an output file, iterates through control sections, and writes the header record for each section in the object file.
* It writes definition records for symbols defined in the control section and reference records for symbols referred to in the section.
* Text records are written for instructions, handling object code size and creating new records if necessary.
* If an instruction is a reserve word, the current text record is written (if it has object code) and a new record is started for the next instruction.
* Modification records are written for instructions with immediate addressing.
* Finally, the function writes the end record in the object file, closes the output and intermediate files, and displays a message indicating the completion of the object program file creation.

------

### Compilation and Execution
After cloning the repo to your local directory:
1. Open Terminal at that location and type `code .` to open the directory
in VS Code.

2. In VS Code, If a user wants to change the input then can change in
“input.txt”. The Current input is:

3. To run the code type the following command in Terminal : 
`gcc -o assembler pass2.cpp`

4. Terminal Outputs shows that the programs have run successfully.

5. The corresponding intermediate ﬁle and Object
Program ﬁle has been generated in `intermediate.txt` and
`object_program.cpp` respectively. It also generates `error.txt` if
there are any errors.


7. This Assembler also writes LITTABs and SYMTABs of respective
control section.
------
------
------



