#include <bits/stdc++.h>
using namespace std;
long long size1=(1<<24);
vector<int> MachineCodes;	// stores machinie code
int memory[(1<<24)];		// stores memory
int A, B, PC, SP, lines = 0;	// registers
pair<int, int> rwPair;

// All operations according to the updated ISA
void ldc(int value) {
    int tempB = A;  // Temporary variable for B
    A = value;
    B = tempB;
}

void adc(int value) {
    int tempA = A;  // Temporary variable for A
    A = tempA + value;
}

void ldl(int offset) {
    int tempB = A;  // Temporary variable for B
    A = memory[SP + offset];
    rwPair = {SP + offset, 0};
    B = tempB;
}

void stl (int offset) {
	rwPair = {SP + offset, memory[SP + offset]};
	memory[SP + offset] = A;
	A = B;
} 
void ldnl(int offset) {
    int tempA = A;  // Temporary variable for A
    A = memory[A + offset];
    rwPair = {SP + offset, 0};

}


void stnl(int offset) {
    rwPair = {SP + offset, memory[SP + offset]};
    memory[A + offset] = B;
}

void add(int value) {
    int tempB = B;  // Temporary variable for B
    A += tempB;
}

void sub(int value) {
    int tempB = B;  // Temporary variable for B
    A = tempB - A;
}
void shl(int value) {
    int tempB = B;  // Temporary variable for B
    A = tempB << A;
}

void shr(int value) {
    int tempB = B;  // Temporary variable for B
    A = tempB >> A;
}

void adj(int value) {
    int tempSP = SP;  // Temporary variable for SP
    SP += value;
}

void a2sp(int value) {
    int tempA = A;  // Temporary variable for A
    int tempB = B;  // Temporary variable for B
    SP = tempA;
    A = tempB;
}

void sp2a(int value) {
    int tempA = A;  // Temporary variable for A
    int tempB = B;  // Temporary variable for B
    B = tempA;
    A = SP;
}

void call(int offset) {
    int tempB = A;  // Temporary variable for B
    A = PC;
    PC += offset;
    B = tempB;
}

void ret(int value) {
    int tempB = B;  // Temporary variable for B
    PC = A;
    A = tempB;
}

void brz(int offset) {
    if (A == 0) {
        int tempPC = PC;  // Temporary variable for PC
        PC += offset;
    }
}

void brlz(int offset) {
    if (A < 0) {
        int tempPC = PC;  // Temporary variable for PC
        PC += offset;
    }
}

void br(int offset) {
    int tempPC = PC;  // Temporary variable for PC
    PC += offset;
}

void halt(int value) {
    // stop execution
    return;
}



// some information to call respective function for each operation

vector<string> mnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", 
					"shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};

void (*func[])(int value) = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, 
					shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br, halt};



map<string, pair<string, int> > opTab;		// {mnemonic, opcode, type of operand}

// Opcode table

// Function to initialize opcode table with opcodes and operand types
void fillOpcodeTable() {
	// Using intermediate steps to assign values, enhancing structure variation
	pair<string, int> tempOpcode;

	// Instructions with operand types
	tempOpcode = {"", 1}; opTab["data"] = tempOpcode;
	tempOpcode = {"00", 1}; opTab["ldc"] = tempOpcode;
	tempOpcode = {"01", 1}; opTab["adc"] = tempOpcode;
	tempOpcode = {"02", 2}; opTab["ldl"] = tempOpcode;
	tempOpcode = {"03", 2}; opTab["stl"] = tempOpcode;
	tempOpcode = {"04", 2}; opTab["ldnl"] = tempOpcode;
	tempOpcode = {"05", 2}; opTab["stnl"] = tempOpcode;
	tempOpcode = {"06", 0}; opTab["add"] = tempOpcode;
	tempOpcode = {"07", 0}; opTab["sub"] = tempOpcode;
	tempOpcode = {"08", 0}; opTab["shl"] = tempOpcode;
	tempOpcode = {"09", 0}; opTab["shr"] = tempOpcode;
	tempOpcode = {"0A", 1}; opTab["adj"] = tempOpcode;
	tempOpcode = {"0B", 0}; opTab["a2sp"] = tempOpcode;
	tempOpcode = {"0C", 0}; opTab["sp2a"] = tempOpcode;
	tempOpcode = {"0D", 2}; opTab["call"] = tempOpcode;
	tempOpcode = {"0E", 0}; opTab["return"] = tempOpcode;
	tempOpcode = {"0F", 2}; opTab["brz"] = tempOpcode;
	tempOpcode = {"10", 2}; opTab["brlz"] = tempOpcode;
	tempOpcode = {"11", 2}; opTab["br"] = tempOpcode;
	tempOpcode = {"12", 0}; opTab["HALT"] = tempOpcode;
	tempOpcode = {"", 1}; opTab["SET"] = tempOpcode;
}
// convert to 8 bit hex string
// Convert integer to an 8-character hexadecimal string
string decToHex(int num) {
    unsigned int currentValue = static_cast<unsigned int>(num);
    string hexResult = "";
    int digitCount = 0;

    while (digitCount < 8) {
        int remainder = currentValue % 16;
        hexResult += (remainder < 10) ? char('0' + remainder) : char('A' + remainder - 10);
        currentValue /= 16;
        ++digitCount;
    }

    reverse(hexResult.begin(), hexResult.end());
    return hexResult;
}

// Handling detected errors during execution
void throwError() {
	const char* errorMsg = "Error: Segmentation Fault or potential infinite loop detected.";
	cout << errorMsg << endl;
	cout << "Suggested Action: Verify code logic and memory access patterns." << endl;
	exit(EXIT_FAILURE);
}

// Print the current state of registers for tracing
void Trace() {
	stringstream traceStream;
	traceStream << "PC = " << std::hex << std::uppercase << setw(8) << setfill('0') << PC << ", ";
	traceStream << "SP = " << setw(8) << SP << ", ";
	traceStream << "A = " << setw(8) << A << ", ";
	traceStream << "B = " << setw(8) << B;
	cout << traceStream.str() << " ";
}

// Display memory read operation
void Read() {
	const string addressStr = decToHex(rwPair.first);
	cout << "Read operation: memory[" << addressStr << "] returned value " << decToHex(A) << endl;
}

// Display memory write operation
void Write() {
	const string addressStr = decToHex(rwPair.first);
	cout << "Write operation: memory[" << addressStr << "] previous value " << decToHex(rwPair.second);
	cout << ", updated to " << decToHex(memory[rwPair.first]) << endl;
}

// Memory before execution
void Before() {
    cout << "memory dump before execution" << endl;
    int i = 0;
    
    while (i < (int)MachineCodes.size()) {
        cout << decToHex(i) << " ";
        int j = i;
        
        while (j < min((int)MachineCodes.size(), i + 4)) {
            cout << decToHex(MachineCodes[j]) << " ";
            ++j;
        }
        
        cout << endl;
        i += 4;
    }
}
// Memory after execution
void After() {
    cout << "memory dump after execution" << endl;
    int i = 0;

    while (i < (int)MachineCodes.size()) {
        cout << decToHex(i) << " ";
        int j = i;

        while (j < min((int)MachineCodes.size(), i + 4)) {
            cout << decToHex(memory[j]) << " ";
            ++j;
        }

        cout << endl;
        i += 4;
    }
}
// considering wipe means resetting A, B, SP, PC
void Wipe() {
	A = B = SP = PC = 0;
}

// Displaying the Instruction Set Architecture (ISA)
void ISA() {
	struct Instruction {
		string opcode;
		string mnemonic;
		string operand;
	};

	// Instruction list setup
	Instruction instructions[] = {
		{"0", "ldc", "value"},
		{"1", "adc", "value"},
		{"2", "ldl", "value"},
		{"3", "stl", "value"},
		{"4", "ldnl", "value"},
		{"5", "stnl", "value"},
		{"6", "add", ""},
		{"7", "sub", ""},
		{"9", "shr", ""},
		{"10", "adj", "value"},
		{"11", "a2sp", ""},
		{"12", "sp2a", ""},
		{"13", "call", "offset"},
		{"14", "return", ""},
		{"15", "brz", "offset"},
		{"16", "brlz", "offset"},
		{"17", "br", "offset"},
		{"18", "HALT", ""},
		{"", "SET", "value"}
	};

	// Print header
	cout << "Opcode   Mnemonic   Operand" << '\n';
	for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); ++i) {
		cout << instructions[i].opcode << "        " 
		     << instructions[i].mnemonic << "       " 
		     << instructions[i].operand << '\n';
	}
}
// Execute each line after decoding 
bool executeInstruction(int currentLine, int flag) {
	int opcode = currentLine & 0xFF;
	int value = (currentLine - opcode) >> 8;
	++lines;

	// Execute function based on opcode
	func[opcode](value);

	// Error check for PC and lines count
	if ((PC < 0) || (PC >= static_cast<int>(MachineCodes.size())) || (lines > (1 << 24))) {
		throwError();
		return true;
	}

	// Process flag cases and opcode-based actions
	switch (flag) {
		case 0:  // Trace and print for flag 0
			Trace();
			cout << mnemonics[opcode] << " ";
			if (opTab[mnemonics[opcode]].second > 0) {
                int t5=0;
				cout << decToHex(value);
			}
			cout << endl;
			break;

		case 1:  // Read on specific opcodes for flag 1
			switch (opcode) {
				case 2:
				case 4:
					Read();
					break;
			}
			break;

		case 2:  // Write on specific opcodes for flag 2
			switch (opcode) {
				case 3:
				case 5:
					Write();
					break;
			}
			break;
	}

	// Check if opcode represents HALT
	return opcode >= 18;
}
// call for each line
void Run(int flag) {
	for (PC = 0; PC < static_cast<int>(MachineCodes.size()); ++PC) {
		int currentLine = MachineCodes[PC];
		bool toQuit = executeInstruction(currentLine, flag);
		if (toQuit) break;
	}
}
// checking the command chosen
void executeCommand(string command) {
    switch (command[1]) {  // Command starts after the '-' symbol
        case 't':  // "-trace"
            if (command == "-trace") {
                Run(0);
                cout << "Program execution finished!" << endl;
            }
            break;
        case 'r':  // "-read"
            if (command == "-read") {
                Run(1);
            }
            break;
        case 'w':  // "-write"
            if (command == "-write") {
                Run(2);
            }
            break;
        case 'b':  // "-before"
            if (command == "-before") {
                Run(3);
                Before();
            }
            break;
        case 'a':  // "-after"
            if (command == "-after") {
                Run(3);
                After();
            }
            break;
        case 'i':  // "-isa"
            if (command == "-isa") {
                ISA();
            }
            break;
        case 'p':  // "-wipe"
            if (command == "-wipe") {
                Wipe();
            }
            break;
        default:
            cout << "Invalid command" << endl;
            exit(0);
    }
    int t1=0;
    cout << lines << " instructions executed" << endl;
}
// reading input from .o file
void takeInput(string filename) {
    size1++;
    ifstream file(filename, ios::in | ios::binary);
    unsigned int cur;
    int counter = 0;
    for (; file.read((char*)&cur, sizeof(int));) {  // Using a for loop to read the file
        MachineCodes.push_back(cur);
        memory[counter++] = cur;
        int tb=0;
    }
}


int main(int argCount, char* argValues[]) {
    // Validate input arguments count
    if (argCount < 3) {
        size1++;
        cout << "Incorrect Format" << endl;
        cout << "Usage: ./emu [option] <file.o>" << endl;
        size1++;
        cout << "Options include:" << endl;
        cout << "-trace  : Display instruction trace" << endl;
        cout << "-read   : Show memory read operations" << endl;
        cout << "-write  : Show memory write operations" << endl;
        cout << "-before : Dump memory before execution" << endl;
        size1++;
        cout << "-after  : Dump memory after execution" << endl;
        cout << "-wipe   : Clear written flags pre-execution" << endl;
        cout << "-isa    : Display ISA info" << endl;
        return 0;
    }
    size1++;
    // Assign file and command from arguments
    string file = argValues[2];
    string cmd = argValues[1];

    // Prepare opcode table and parse input
    fillOpcodeTable();
    takeInput(file);

    // Process the given command
    executeCommand(cmd);
    
    return 0;
}

