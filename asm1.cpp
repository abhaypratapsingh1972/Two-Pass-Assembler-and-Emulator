#include <bits/stdc++.h>
using namespace std;
// Storing error information
// Struct for error information with custom comparison operator
struct errorInfo {
	int location;
	string msg;
	bool operator< (const errorInfo &other) const {
		return (this->location < other.location);
	}
};

// Struct for warning information
struct warningInfo {
	int location;
	string msg;
	bool operator< (const warningInfo &other) const {
		return (this->location < other.location);
	}
};

// Struct for storing program line information
struct lineInfo {
	int pctr;
	string label;
	string mnemonic;
	string operand;
	string prevOperand;
};

// Struct for listing file details
struct listInfo {
	string address;
	string macCode;
	string statement;
};

// Vectors to store error, warning, line, and list information
vector<errorInfo> Errors;          // Collection of errors
vector<warningInfo> Warnings;      // Collection of warnings
vector<lineInfo> Lines;            // Collection of program lines
vector<listInfo> List;             // Listing file information
vector<string> MachineCodes;       // Machine codes in 8-bit hex form

// Function to log an error
void pushErrors(int locctr, string err) {
	errorInfo newError = {locctr, err}; // Use an intermediate variable for error
	Errors.push_back(newError);
}

// Function to log a warning
void pushWarnings(int locctr, string err) {
	warningInfo newWarning = {locctr, err}; // Use an intermediate variable for warning
	Warnings.push_back(newWarning);
}

// Maps for symbol table, comments, opcodes, label locations, and variables
map<string, pair<int, int>> symTab;          // {label, {address, lineNum}}
map<int, string> comments;                   // {line, comment}
map<string, pair<string, int>> opTab;        // {mnemonic, {opcode, operand type}}
map<string, vector<int>> locLabels;          // {label, list of line numbers using the label}
map<string, string> setVars;                 // {variable (label), associated value}

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

// Helper function to check if the segment is empty or starts with a comment marker (';')
bool isEmptyOrComment(const string &segment) {
    return segment.empty() || segment[0] == ';';
}

// Helper function to check if a segment contains a label (indicated by a ':' character)
bool containsLabel(const string &segment) {
    return segment.find(':') != string::npos && segment.back() != ':'; // Ensures it's a label not ending in ':'
}

// Processes a label segment, extracting the label part before ':' and updating the remaining segment
void processLabelSegment(string &segment, vector<string> &words) {
    size_t colonPos = segment.find(':');         // Find position of ':'
    words.push_back(segment.substr(0, colonPos + 1)); // Add label (e.g., 'label:')
    segment = segment.substr(colonPos + 1);      // Update segment by removing label part
}

// Checks if a segment ends with a comment marker (';')
bool endsWithCommentMarker(const string &segment) {
    return segment.back() == ';';
}
void removeCommentMarker(string &segment) {
     segment.pop_back();
 }
// Extracts meaningful words from a line of code, skipping comments
vector<string> extractWords(string currentLine, int lineLocation) {
    // If the line is empty, return an empty vector
    if (currentLine.empty()) return {};
    
    vector<string> words;            // Stores individual words from the line
    stringstream lineStream(currentLine);  // String stream to read words
    string segment;                      


    // Read each word/segment from the line
    while (lineStream >> segment) {
        // Stop processing if the segment is empty or a comment is found
        if (isEmptyOrComment(segment)) break;
        
        // Check if the segment contains a label (e.g., 'label:')
        if (containsLabel(segment)) {
            // Process label part and store it separately
            processLabelSegment(segment, words);
        }
        
        // If the segment ends with a comment marker (';'), process it accordingly
        if (endsWithCommentMarker(segment)) {
            // Remove the comment marker and store the word
            removeCommentMarker(segment);
            words.push_back(segment);
            break; // Stop further processing as the comment is reached
        }
        
        // Add the processed word to the result
        words.push_back(segment);
    }
    
    return words; // Return the extracted words (excluding comments)
}


// Utility functions for checks
bool isDigit(char ch) {
    // Check if the character is a digit (0-9)
    return ch >= '0' && ch <= '9';
}

bool isAlphabet(char ch) {
    // Convert to lowercase for uniformity and check if it's a letter (a-z)
    char lowerCh = tolower(ch);
    return lowerCh >= 'a' && lowerCh <= 'z';
}

// Function to verify the validity of a label based on syntax rules
bool isValidLabel(const string &labelName) {
    // Label must start with an alphabetic character
    bool valid = isAlphabet(labelName[0]);
    
    // Initialize index for while loop
    int index = 0;

    // Loop through each character in the label
    while (index < labelName.size()) {
        char character = labelName[index];
        
        // Check if character is a digit, alphabet, or underscore
        valid = valid && (isDigit(character) || isAlphabet(character) || (character == '_'));

        // Move to the next character
        index++;
    }

    // Return the validity of the label
    return valid;
}

// Function to check if a string represents a decimal number
bool isDecimal(string num) {
    bool valid = true;
    int index = 0;

    // Loop through each character in the string
    while (index < num.size()) {
        char c = num[index];
        
        // Check if the character is a digit
        valid = valid && isDigit(c);

        // Move to the next character
        index++;
    }

    return valid;
}


// Function to check if a string represents an octal number
bool isOctal(string num) {
    bool check = true;
    
    // Check if the number has at least two characters and starts with '0'
    check = check && (num.size() >= 2) && (num[0] == '0');
    
    int index = 0;

    // Check each character to ensure it's within the octal digit range (0-7)
    while (index < num.size()) {
        char c = num[index];
        check = check && (c >= '0' && c <= '7');
        index++;
    }

    return check;
}


// Function to check if a string represents a hexadecimal number
bool isHexadecimal(string num) {
    bool check = true;

    // Check if the number has at least three characters, starts with '0', and is followed by 'x' or 'X'
    check &= (num.size() >= 3) && (num[0] == '0') && (tolower(num[1]) == 'x');

    int index = 2; // Start from the third character

    // Loop through each character to ensure it’s within the hexadecimal digit range (0-9, a-f)
    while (index < num.size()) {
        char c = tolower(num[index]);
        check &= (isDigit(c) || (c >= 'a' && c <= 'f'));
        index++;
    }

    return check;
}

// Function to convert an octal string to a decimal string
string octalToDec(const string &num) {
    int decimalVal = 0;
    int power = 1;
    int i = num.size() - 1;  // Start from the last character of the octal string

    // Loop through each character from the end towards the beginning
    while (i >= 0) {
        decimalVal += power * (num[i] - '0'); // Convert character to integer and add to the result
        power *= 8;                           // Increment the power of 8 for each position
        i--;                                  // Move to the previous character
    }

    return to_string(decimalVal); // Return the decimal value as a string
}
// Function to convert hexadecimal string to decimal string
string hexToDec(const string &num) {
    int decimalVal = 0;
    int power = 1;
    int i = num.size() - 1;  // Start from the last character of the hexadecimal string

    // Loop through each character from the end towards the beginning
    while (i >= 0) {
        // Calculate the decimal value of the current hex character
        if (isDigit(num[i])) {
            decimalVal += power * (num[i] - '0');
        } else {
            decimalVal += power * (tolower(num[i]) - 'a' + 10);
        }
        
        power *= 16;  // Increment the power of 16 for each position
        i--;          // Move to the previous character
    }

    return to_string(decimalVal); // Return the decimal value as a string
}

// Converts an integer to an 8-character hexadecimal string
string decToHex(int decimalNum) {
    unsigned int hexNum = decimalNum; // Convert the decimal number to unsigned to handle negative numbers correctly
    string hexStr = "";

    // Loop to convert the number to hexadecimal
    while (hexNum > 0) {
        int remainder = hexNum % 16; // Get the remainder (hex digit)
        hexStr += (remainder <= 9 ? char(remainder + '0') : char(remainder - 10) + 'A');
        hexNum /= 16; // Divide the number by 16 for the next hex digit
    }

    // Ensure the result is always 8 characters by adding leading zeros
    while (hexStr.size() < 8) {
        hexStr += '0'; // Append zeros to make the length 8
    }

    // Reverse the string to get the correct hex representation
    reverse(hexStr.begin(), hexStr.end());
    return hexStr;
}

// Function to check and process errors associated with labels
void processLabel(string label, int locctr, int pctr) {
    
    if (label.empty()) return;  // Return if the label is empty

    bool isValid = isValidLabel(label);  // Check if the label is valid

    // Using switch-case structure to handle different label processing scenarios
    switch (isValid) {
        case true:  // If the label is valid
            // Check if the label already exists in the symbol table and is not a forward reference
            switch (symTab.count(label) && symTab[label].first != -1) {
                case true:  // Duplicate label definition found
                    pushErrors(locctr, "Duplicate label definition");  // Push error for duplicate label
                    break;
                default:  // No duplicate found, add the label to symbol table
                    symTab[label] = {pctr, locctr};
                    break;
            }
            break;
        default:  // If the label is not valid
            pushErrors(locctr, "Bogus Label name");  // Push error for invalid label
            break;
    }
}
string processOperand(string operand, int locctr) {
    string result = "";  // Changed `ret` to `result` for a unique local name

    // Check if operand is a valid label and update symbol table if needed
    if (isValidLabel(operand)) {
        if (symTab.find(operand) == symTab.end()) {  // Using `find` instead of `count`
            symTab[operand] = {-1, locctr};  // Mark label as used but not declared
        }
        locLabels[operand].emplace_back(locctr);
        return operand;
    }

    // Handle possible sign and isolate numeric portion
    bool hasSign = (operand[0] == '-' || operand[0] == '+');
    if (hasSign) {
        result += operand[0];  // Append sign
        operand = operand.substr(1);  // Remove the sign from operand
    }

    // Set type flag based on operand type
    enum OperandType { OCTAL, HEX, DECIMAL, INVALID } operandType;
    if (isOctal(operand)) {
        operandType = OCTAL;
    } else if (isHexadecimal(operand)) {
        operandType = HEX;
    } else if (isDecimal(operand)) {
        operandType = DECIMAL;
    } else {
        operandType = INVALID;
    }

    // Use switch based on the operand type
    switch (operandType) {
        case OCTAL:
            result += octalToDec(operand.substr(1));
            break;
        case HEX:
            result += hexToDec(operand.substr(2));
            break;
        case DECIMAL:
            result += operand;
            break;
        default:
            result.clear();  // Reset result if operand is invalid
            break;
    }

    return result;
}
// Process and validate mnemonics, checking for errors in the input format
void processMnemonic(string instName, string &operand, int locctr, int pctr, int remaining, bool &statusFlag) {
    // Exit immediately if instName is empty
    bool hasInstName = !instName.empty();
    if (!(hasInstName)) return;

    // Check if the mnemonic exists in the opcode table
    bool mnemonicExists = (opTab.find(instName) != opTab.end());
    if (!(mnemonicExists)) {
        pushErrors(locctr, "Unrecognized Mnemonic");
        return;
    }

    int instType = opTab[instName].second;
    bool hasOperand = !operand.empty();

    // Use switch statement to handle mnemonics based on operand requirements
    switch (instType > 0) {
        case true: {  // Mnemonics that require an operand
            bool operandMissing = !hasOperand;
            bool extraDataPresent = remaining > 0;

            if (!(operandMissing == false)) {
                pushErrors(locctr, "Missing operand");
            } else if (!(extraDataPresent == false)) {
                pushErrors(locctr, "Extra data at end of line");
            } else {
                string processedOperand = processOperand(operand, locctr);
                bool invalidOperand = processedOperand.empty();
                if (!(invalidOperand == false)) {
                    pushErrors(locctr, "Invalid operand: not a valid label or numeric value");
                } else {
                    operand = processedOperand;
                    statusFlag = true;
                }
            }
            break;
        }
        case false: {  // Mnemonics that should not have an operand
            bool unexpectedOperand = (instType == 0 && hasOperand);
            if (!(unexpectedOperand == false)) {
                pushErrors(locctr, "Unexpected operand found");
            } else {
                statusFlag = true;
            }
            break;
        }
    }
}// Analyze lines to identify errors and store each line in {pctr, label, mnemonic, operand} format for pass2
void pass1(const vector<string> &readLines) {
    int locctr = 0, pctr = 0;

    int lineIndex = 0;
    while (lineIndex < readLines.size()) {
        ++locctr;
        string currentLine = readLines[lineIndex];
        auto tokens = extractWords(currentLine, locctr);
        bool tokensExist = !tokens.empty();
        if (!tokensExist) {
            ++lineIndex;
            continue;
        }

        string label = "", instName = "", operand = "";
        int position = 0, tokenSize = tokens.size();

        bool firstTokenIsLabel = (tokens[position].back() == ':');
        switch (firstTokenIsLabel) {
            case true:
                label = tokens[position];
                label.pop_back();
                ++position;
                break;
            default:
                break;
        }

        bool hasInstName = (position < tokenSize);
        switch (hasInstName) {
            case true:
                instName = tokens[position];
                ++position;
                break;
            default:
                break;
        }

        bool hasOperand = (position < tokenSize);
        switch (hasOperand) {
            case true:
                operand = tokens[position];
                ++position;
                break;
            default:
                break;
        }

        processLabel(label, locctr, pctr);

        bool statusFlag = false;
        string originalOperand = operand;
        processMnemonic(instName, operand, locctr, pctr, tokenSize - position, statusFlag);
        Lines.push_back({pctr, label, instName, operand, originalOperand});

        pctr += statusFlag;

        // Define missing_Label before the switch statement
        bool missing_Label = label.empty();
        bool isSetInstruction = (statusFlag && instName == "SET");
        switch (isSetInstruction) {
            case true:
                switch (missing_Label) {
                    case true:
                        pushErrors(locctr, "Label or variable name is missing");
                        break;
                    default:
                        setVars[label] = operand;
                        break;
                }
                break;
            default:
                break;
        }

        ++lineIndex;
    }

    auto symTabIterator = symTab.begin();
    while (symTabIterator != symTab.end()) {
        const auto& label = *symTabIterator;
        switch (label.second.first) {
            case -1: {
                for (const auto& lineNum : locLabels[label.first]) {
                    pushErrors(lineNum, "No such label");
                }
                break;
            }
            default: {
                if (!locLabels.count(label.first)) {
                    pushWarnings(label.second.second, "Label declared but not used");
                }
                break;
            }
        }
        ++symTabIterator;
    }
}

// Inserting into List vector
void pushInList(int pctr, string macCode, string label, string mnemonic, string operand) {
    string modifiedLabel = label; // Create a new variable for label modification
    string modifiedMnemonic = mnemonic; // Create a new variable for mnemonic modification

    // Handle label modification using switch
    switch (modifiedLabel.empty()) {
        case false: 
            modifiedLabel += ": "; // Append colon if label is not empty
            break;
        default:
            // No modification needed if label is empty
            break;
    }

    // Handle mnemonic modification using switch
    switch (modifiedMnemonic.empty()) {
        case false:
            modifiedMnemonic += " "; // Append space if mnemonic is not empty
            break;
        default:
            // No modification needed if mnemonic is empty
            break;
    }

    string combinedStatement = modifiedLabel + modifiedMnemonic + operand; // Combine label, mnemonic, and operand
    
    // Convert pctr to hex
    string hexPctr = decToHex(pctr);

    // Push the formatted data to List
    List.push_back({hexPctr, macCode, combinedStatement});
}
// Generating machine codes and building list vector
void pass2() {
    int i = 0;  // Initialize the index variable for the while loop

    while (i < Lines.size()) {  // Continue while there are elements in Lines
        auto curLine = Lines[i];  // Access the current line using the index
        string label = curLine.label, mnemonic = curLine.mnemonic, operand = curLine.operand;
        
        string prevOperand = curLine.prevOperand;
        int pctr = curLine.pctr, type = -1;
        
        if (!mnemonic.empty()) {
            type = opTab[mnemonic].second;
        }

        string Mcode = "        ";  // Default value for machine code
        
        switch (type) {
            case 2: {
                // offset is required
                int offset = (symTab.count(operand) ? symTab[operand].first - (pctr + 1) : stoi(operand));
                Mcode = decToHex(offset).substr(2) + opTab[mnemonic].first;
                break;
            }
            case 1: {
                if (mnemonic != "data" && mnemonic != "SET") {
                    // value is required
                    int value = (symTab.count(operand) ? symTab[operand].first : stoi(operand));
                    Mcode = decToHex(value).substr(2) + opTab[mnemonic].first;

                    if (setVars.count(operand)) {
                       
                        // if in case the operand is a variable used in SET operation
                        Mcode = decToHex(stoi(setVars[operand])).substr(2) + opTab[mnemonic].first;
                    }
                } else {
                    // Special case for data and SET mnemonic
                    Mcode = decToHex(stoi(operand));
                }
                break;
            }
            case 0: {
                // nothing is required
                Mcode = "000000" + opTab[mnemonic].first;
                break;
            }
            default: {
                // do nothing
                break;
            }
        }

        // Push the generated machine code and update the list
        MachineCodes.push_back(Mcode);
        
        // Push instruction info to list vector
        pushInList(pctr, Mcode, label, mnemonic, prevOperand);

        i++;  // Increment the index to move to the next line
    }
}
// Writing errors / warnings into .log file
// Writing errors / warnings into .log file
void throwErrorsAndWarnings() {
    ofstream coutErrors("logfile.log");

    // Sort errors and warnings separately
    sort(Errors.begin(), Errors.end());
    int a6=0;
    sort(Warnings.begin(), Warnings.end());

    cout << "Errors(.log) file generated" << endl;

    // Initialize indices for errors and warnings
    int errorIndex = 0, warningIndex = 0;
    
    // Check for the presence of errors and warnings
    switch (Errors.empty()) {
        case true: {
            // If there are no errors, output "No errors!" and then warnings
            coutErrors << "No errors!" << endl;
            while (warningIndex < Warnings.size()) {
                coutErrors << "Line: " << Warnings[warningIndex].location << " WARNING: " << Warnings[warningIndex].msg << endl;
                warningIndex++;
            }
            break;
        }
        case false: {
            // If there are errors, output them first
            while (errorIndex < Errors.size()) {
                coutErrors << "Line: " << Errors[errorIndex].location << " ERROR: " << Errors[errorIndex].msg << endl;
                errorIndex++;
            }
            // After errors, output warnings (if any)
            while (warningIndex < Warnings.size()) {
                coutErrors << "Line: " << Warnings[warningIndex].location << " WARNING: " << Warnings[warningIndex].msg << endl;
                warningIndex++;
            }
            break;
        }
    }

    // Close the file after writing
    coutErrors.close();
}
vector<string> readLines; 
// Reading from the input file
void read() {
    ifstream cinfile;
    cout << "write the testfile: " << endl;
    string s;
    cin >> s;
    cinfile.open(s);

    // Switch statement to handle file opening failure or success
    switch (cinfile.fail()) {
        case true: {
            cout << "Input file doesn't exist" << endl;
            exit(0);
        }
        case false: {
            string curLine;
            // While loop to read all lines from the file
            while (getline(cinfile, curLine)) {
                readLines.push_back(curLine);
            }
            cinfile.close();
            break;
        }
    }
}
// Writing into listing file(.lst) and machine codes object file(.o)
void write() {
    ofstream coutList("listfile.lst");

    int listIndex = 0;
    // Use a while loop to write List content
    while (listIndex < List.size()) {
        coutList << List[listIndex].address << " " 
                 << List[listIndex].macCode << " " 
                 << List[listIndex].statement << endl;
        listIndex++;
    }
    coutList.close();
    cout << "Listing(.lst) file generated" << endl;

    // Writing machine code object file
    ofstream coutMCode;
    coutMCode.open("machineCode.o", ios::binary | ios::out);

    int codeIndex = 0;
    // Use a while loop to write MachineCodes content
    while (codeIndex < MachineCodes.size()) {
        string code = MachineCodes[codeIndex];
        switch (code.empty() || code == "        ") {
            case true: {
                // Skip empty or invalid machine code
                break;
            }
            case false: {
                unsigned int cur = (unsigned int)stoi(hexToDec(code));
                static_cast<int>(cur); // Type cast to avoid warnings
                coutMCode.write((const char*)&cur, sizeof(unsigned int));
                break;
            }
        }
        codeIndex++;
    }

    coutMCode.close();
    cout << "Machine code object(.o) file generated" << endl;
}

int main() {
	read();										// read from file
	fillOpcodeTable();							// intialise Opcode table
	pass1(readLines);							// first pass
	throwErrorsAndWarnings();					// if found errors, write and terminate 
	if (Errors.empty()) {						// if no errors
		pass2();								// go for second pass
		write();								// write machine code and listing file	
	}
	return 0;
}