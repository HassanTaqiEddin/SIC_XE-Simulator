#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>

using namespace std;
//Global_variables
string  op;
string format;
string N;//shift value
string n, i, x, b, p, e, flags;
string A = "000000000000000000000000";
string X = "000000000000000000000000";
string L = "000000000000000000000000";
string PC;
string TA;
string SW = "000000000000000000000000";
string B = "000000000000000000000000";
string S = "000000000000000000000000";
string T = "000000000000000000000000";
string F = "000000000000000000000000";
string address;
string disp;
string newPC;
string PCstart;
string PClast;
string cc;
string TAplus1, TAplus2, TAplus3;
string r1;
string r2;
string ins_name;
string reg_name;
string id;
vector<string> binaryNumbers;// New vector to hold binary representations
vector<string> porttext;//TD,RD,WD



//classes
class Memory {
private:
    vector<string> memory;
    const int ADDRESS_BITS = 20;
    const int ELEMENT_SIZE = 8;

public:
    Memory(int size) {
        memory.resize(size, "00000000"); // Initialize memory with 8-bit zeros
    }

    void write(const string& address, const  string& value)
    {


        int index = bitset<20>(address).to_ulong();
        if (value.size() != ELEMENT_SIZE)
        {
            cout << "Invalid value size." << endl;
            return;
        }

        memory[index] = value;
    }

    string read(const string& address) const
    {

        int index = std::bitset<20>(address).to_ulong();
        return memory[index];
    }
};
enum class InstructionFormat {
    StandardSIC,
    Format1,
    Format2,
    Format3,
    Format4
};
Memory memory(1024 * 1024);// Memory




//Functions used in the instructions
bool checkhexa(char c)
{
    // Check if the character is a valid hexadecimal digit (0-9 or a-f)
    return (isdigit(c) || (tolower(c) >= 'a' && tolower(c) <= 'f'));
}
string addBinaryStrings(const string& one, const string& two)
{
    int maxLength = max(one.length(), two.length());
    string adding1 = string(maxLength - one.length(), '0') + one;
    string adding2 = string(maxLength - two.length(), '0') + two;

    int carry = 0;
    string result;

    for (int i = maxLength - 1; i >= 0; --i) {
        int sum = (adding1[i] - '0') + (adding2[i] - '0') + carry;
        result = char(sum % 2 + '0') + result;
        carry = sum / 2;
    }

    if (carry) {
        result = '1' + result;
    }

    return result;
}
string AND(const string& binary1, const string& binary2) {
    if (binary1.size() != binary2.size()) {
        cout << "Binary numbers must have the same length." << endl;
        return "";
    }

    string result;
    result.reserve(binary1.size());

    for (size_t i = 0; i < binary1.size(); ++i) {
        if (binary1[i] == '1' && binary2[i] == '1') {
            result += '1';
        }
        else {
            result += '0';
        }
    }

    return result;
}
string Sub(string a, string b)
{
    while (b.size() < a.size())
    {
        b = '0' + b;
    }

    string result = "";
    int carry = 0;

    for (int i = a.size() - 1; i >= 0; i--)
    {
        int bit1 = a[i] - '0';
        int bit2 = b[i] - '0';

        int sub = bit1 - bit2 - carry;

        if (sub < 0) {
            sub += 2;
            carry = 1;
        }
        else {
            carry = 0;
        }

        result = char(sub + '0') + result;
    }

    while (result.size() > 1 && result[0] == '0')
    {
        result = result.substr(1);
    }

    return result;
}
string Divide(const string& bin_dividend, const string& bin_divisor)
{
    int dividend = stoi(bin_dividend, nullptr, 2);
    int divisor = stoi(bin_divisor, nullptr, 2);

    if (divisor == 0) {
        return "Undefined";
    }

    int quotient = dividend / divisor;
    int remainder = dividend % divisor;

    string quotient_bin = "";
    while (quotient > 0 || quotient_bin.length() < 24) {
        quotient_bin = to_string(quotient % 2) + quotient_bin;
        quotient /= 2;
    }

    // Pad with zeros to make sure the result is exactly 24 bits
    while (quotient_bin.length() < 24) {
        quotient_bin = "0" + quotient_bin;
    }

    return quotient_bin;
}
string shiftLeft(const string& binaryNumber, const string& shiftAmount)
{
    string addleading = binaryNumber;
    while (addleading.length() < 24) {
        addleading = "0" + addleading;
    }
    // Ensure binaryNumber and shiftAmount have 24 bits

    string paddedShiftAmount = shiftAmount;
    while (paddedShiftAmount.length() < 24) {
        paddedShiftAmount = "0" + paddedShiftAmount;
    }

    // Perform left rotation
    int shift = stoi(paddedShiftAmount, nullptr, 2) % 24;
    string rotatedBinary = addleading.substr(shift) + addleading.substr(0, shift);

    return rotatedBinary.substr(rotatedBinary.length() - 24); // Ensure the result is 24 bits
}
string ShiftRight(const string& binaryNumber, const string& shiftAmount) {
    // Ensure binaryNumber and shiftAmount have 24 bits
    string paddedBinaryNumber = binaryNumber;
    while (paddedBinaryNumber.length() < 24) {
        paddedBinaryNumber = paddedBinaryNumber[0] + paddedBinaryNumber;
    }

    string paddedShiftAmount = shiftAmount;
    while (paddedShiftAmount.length() < 24) {
        paddedShiftAmount = "0" + paddedShiftAmount;
    }

    // Perform right shift
    int shift = stoi(paddedShiftAmount, nullptr, 2) % 24;
    string shiftedBinary = paddedBinaryNumber.substr(0, 24 - shift);

    // Fill vacated positions with bits set to the leftmost bit
    char fillBit = shiftedBinary[0];
    while (shiftedBinary.length() < 24) {
        shiftedBinary = fillBit + shiftedBinary;
    }

    return shiftedBinary;
}
string Add(string a, string b)
{
    string result;
    int carry = 0;

    int i = a.size() - 1;
    int j = b.size() - 1;

    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0)
            sum += a[i] - '0';
        if (j >= 0)
            sum += b[j] - '0';

        result += to_string(sum % 2);
        carry = sum / 2;

        i--;
        j--;
    }

    reverse(result.begin(), result.end());
    return result;
}
string Multiply(string a, string b) {
    string result = "0";

    for (int i = b.size() - 1; i >= 0; i--) {
        if (b[i] == '1') {
            string temp = a;
            result = Add(result, temp);
        }
        a += '0';
    }

    // Trim the result to 24 bits
    if (result.size() > 24) {
        result = result.substr(result.size() - 24);
    }
    else if (result.size() < 24) {
        string padding(24 - result.size(), '0');
        result = padding + result;
    }

    return result;
}
string OR(string a, string b) {
    string result = "";

    int maxLength = max(a.size(), b.size());

    while (a.size() < maxLength)
    {
        a = "0" + a;
    }
    while (b.size() < maxLength) {
        b = "0" + b;
    }

    for (int i = 0; i < maxLength; i++)
    {
        if (a[i] == '1' || b[i] == '1')
        {
            result += "1";
        }
        else {
            result += "0";
        }
    }

    return result;
}
string replaceBits(const string& original, const string& replacement, size_t Index)
{
    //This Function replace between given bits with another givin bits with givin index
    if (Index + replacement.size() > original.size())
    {
        cout << "Replacement goes beyond the original string size." << endl;
        return original;
    }

    string modified = original;
    for (size_t i = 0; i < replacement.size(); ++i) {
        modified[Index + i] = replacement[i];
    }

    return modified;
}
void Read(const string& filename, const string& label, vector<string>& content)
{
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cout << "Failed to open the file." << endl;
        return;
    }

    string line;
    bool foundId = false;

    while (getline(inputFile, line)) {
        if (line.find("!! I/O '" + label + "'") != string::npos)
        {
            foundId = true;
            while (getline(inputFile, line) && line.find("!!") != 0)
            {
                content.push_back(line);
            }

            break;
        }
    }

    inputFile.close();

    if (!foundId) {
        cout << "ID not found in the file." << endl;
    }
}
void Write(const vector<string>& lines, const string& filename) {
    ofstream outputFile(filename);

    if (!outputFile.is_open()) {
        throw runtime_error("Failed to open the output file.");
    }
    outputFile << "!!I/O '" << id << "'" << endl;
    for (const string& line : lines) {
        outputFile << line << endl;

        if (!outputFile) {
            outputFile.close();
            throw runtime_error("Error writing to file.");
        }
    }

    outputFile.close();
}
string& determine_reg(string R)
{
    if (R == "0000")
    {
        reg_name = "A";
        return A;
    }
    else if (R == "0001")
    {
        reg_name = "X";
        return X;
    }
    else if (R == "0010")
    {
        reg_name = "L";
        return L;
    }
    else if (R == "0011")
    {
        reg_name = "B";
        return B;
    }
    else if (R == "0100")
    {
        reg_name = "S";
        return S;
    }
    else if (R == "0101")
    {
        reg_name = "T";
        return T;
    }
    else if (R == "0110")
    {
        reg_name = "F";
        return F;
    }
    else if (R == "1000")
    {
        reg_name = "PC";
        return PC;
    }
    else if (R == "1001")
    {
        reg_name = "SW";
        return SW;
    }

}
string Addleadzero1(const string& hexString)
{
    if (hexString.size() == 0) {
        return "00";
    }
    else if (hexString.size() == 1) {
        stringstream ss;
        ss << "0" << hexString;
        return ss.str();
    }
    else if (hexString.size() > 2) {
        return hexString.substr(hexString.size() - 2, 2);
    }
    return hexString;
}
string Addleadzero2(const string& hexString)
{
    if (hexString.size() == 0) {
        return "00000000";
    }
    else if (hexString.size() < 8) {
        stringstream ss;
        ss << setw(8) << setfill('0') << hex << hexString;
        return ss.str();
    }
    else if (hexString.size() > 8) {
        return hexString.substr(hexString.size() - 8, 8);
    }
    return hexString;
}




//Processing functions
string parseHexString(const string& hexString)
{
    string processedString;

    // Convert the entire input string to lowercase
    for (char c : hexString) {
        processedString += tolower(c);
    }

    // Remove spaces and underscores from the processed string
    processedString.erase(remove_if(processedString.begin(), processedString.end(),
        [](char c) { return !checkhexa(c); }), processedString.end());

    return processedString;
}
string convertToBinary(const string& hexString) {
    // Convert the hexadecimal number to binary using bitset
    string processedString = parseHexString(hexString);
    int binarySize = processedString.length() * 4;
    unsigned long long hexNumber = stoull(processedString, nullptr, 16);
    bitset<64> binaryNumber(hexNumber); // Assuming the number of hex digits can be up to 16 (64 bits)

    // Get the binary string with leading zeros to match the required size
    string binaryString = binaryNumber.to_string();
    binaryString = binaryString.substr(binaryString.size() - binarySize);

    return binaryString;
}
void Readinput(const string& filePath)
{
    ifstream inputFile(filePath);
    if (!inputFile) {
        cout << "Error opening the input file." << endl;
        return;
    }

    vector<string> hexNumbers;

    string line;

    while (getline(inputFile, line)) {
        // Check if the line starts with a valid hexadecimal character
        if (line.length() > 0 && checkhexa(line[0])) {
            string hexNumber = parseHexString(line);
            hexNumbers.push_back(hexNumber);

            // Convert the hexadecimal number to binary and store it in the binaryNumbers vector
            binaryNumbers.push_back(convertToBinary(hexNumber));
        }

        // Find "START" in the line and extract the hex number after it
        size_t startPos = line.find("START");
        if (startPos != string::npos) {
            string hexSubstring = line.substr(startPos + 5); // "START" has 5 characters
            string start = parseHexString(hexSubstring);
            int numHexDigits = start.length();
            cout << "Found START: " << hex << setw(6) << setfill('0') << start << " (Binary: " << convertToBinary(start) << ")" << endl;
            PCstart = convertToBinary(start);
            PC = PCstart;
        }

        // Find "LAST" in the line and extract the hex number after it
        size_t lastPos = line.find("LAST");
        if (lastPos != string::npos)
        {
            string hexSubstring = line.substr(lastPos + 4); // "LAST" has 4 characters
            string last = parseHexString(hexSubstring);
            int numHexDigits = last.length();
            cout << "Found LAST: " << hex << setw(6) << setfill('0') << last << " (Binary: " << convertToBinary(last) << ")" << endl;
            PClast = convertToBinary(last);
            cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        }
    }

    inputFile.close(); // Close the input file after reading

    // Print the hexadecimal numbers and their binary representations from the hexNumbers vector
    cout << "Machine Codes:" << endl;
    for (string hexNumber : hexNumbers) {
        // Ensure at least 6 digits by adding leading zeroes
        cout << hex << setw(6) << setfill('0') << hexNumber << " (Binary: " << convertToBinary(hexNumber) << ")" << endl;
    }
}
char valueToHexChar(int value)
{
    if (value >= 0 && value <= 9)
    {
        return '0' + value;
    }
    else if (value >= 10 && value <= 15)
    {
        return 'A' + (value - 10);
    }
    else {
        throw runtime_error("Invalid hexadecimal value");
    }
}
int hexCharToValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    else {
        throw runtime_error("Invalid hexadecimal character");
    }
}
string addHexStrings(const string& hex1, const string& hex2)
{
    //Add hexa values
    int maxLength = max(hex1.length(), hex2.length());
    string paddedHexStr1 = string(maxLength - hex1.length(), '0') + hex1;
    string paddedHexStr2 = string(maxLength - hex2.length(), '0') + hex2;

    int carry = 0;
    string result;

    for (int i = maxLength - 1; i >= 0; --i) {
        int digitSum = hexCharToValue(paddedHexStr1[i]) + hexCharToValue(paddedHexStr2[i]) + carry;
        result = valueToHexChar(digitSum % 16) + result;
        carry = digitSum / 16;
    }

    if (carry) {
        result = valueToHexChar(carry) + result;
    }

    return result;
}
string binaryToHex(const string& binary)
{
    // binary to decimal
    bitset<64> bits(binary);
    unsigned long decimal = bits.to_ulong();

    // decimal to hexadecimal
    stringstream hexStream;
    hexStream << hex << uppercase << decimal;

    return hexStream.str();
}
void compute_TA(string flags, Memory& mem)
{

    if (flags == "110000")
        TA = disp;
    else if (flags == "110001")
        TA = address;
    else if (flags == "110010")
        TA = addBinaryStrings(newPC, disp);

    else  if (flags == "110100")
        TA = addBinaryStrings(B, disp);
    else  if (flags == "111000")
        TA = addBinaryStrings(X, disp);
    else  if (flags == "111001")
        TA = addBinaryStrings(X, address);
    else  if (flags == "111010")
        TA = addBinaryStrings(addBinaryStrings(newPC, X), disp);
    else  if (flags == "111100")
        TA = addBinaryStrings(addBinaryStrings(B, X), disp);
    else  if (n + i + x == "000")
        TA = address;
    else if (n + i + x == "001")
        TA = addBinaryStrings(X, address);
    else  if (flags == "100000")
    {
        TA = disp;
        TA = mem.read(TA);

    }
    else if (flags == "100001")
    {

        TA = address;
        TA = mem.read(TA);
    }
    else if (flags == "100010")
    {
        TA = addBinaryStrings(PC, disp);
        TA = mem.read(TA);
    }
    else if (flags == "100100")
    {
        TA = addBinaryStrings(B, disp);
        TA = mem.read(TA);
    }
    else if (flags == "010000")
        TA = disp;
    else if (flags == "010001")
        TA = address;
    else if (flags == "010010")
        TA = addBinaryStrings(PC, disp);
    else if (flags == "010100")
        TA = addBinaryStrings(B, disp);



}
void instructions()
{
    if (op == "00011000")//ADD
    {
        cout << "ADD,";
        if ((n + i) == "01")
        {
            A = addBinaryStrings(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");

        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = addBinaryStrings(A, memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));
            A = addBinaryStrings(A, "000000000000000000000000");
        }


    }
    else if (op == "10010000")//ADDR
    {
        cout << "ADDR,";

        determine_reg(r2) = addBinaryStrings(determine_reg(r1), determine_reg(r2));
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");

    }
    else if (op == "01000000")//AND
    {
        cout << "AND,";
        if ((n + i) == "01")
        {
            TA = addBinaryStrings(TA, "000000000000000000000000");
            A = addBinaryStrings(A, "000000000000000000000000");
            A = AND(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else {
            A = addBinaryStrings(A, "000000000000000000000000");
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = AND(A, memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));
        }



    }
    else if (op == "10110100") //Clear
    {
        cout << "CLEAR,";
        determine_reg(r1) = "000000000000000000000000";


    }
    else if (op == "00101000")//Comp
    {
        cout << "COMP,";
        if ((n + i) == "01")
        {
            A = addBinaryStrings(A, "000000000000000000000000");
            TA = addBinaryStrings(TA, "000000000000000000000000");
            if (A == TA)
            {
                cc = "1";
                SW = "000000000000000000000000";
            }
            else if (A < TA)
            {
                cc = "-1";
                SW = "000000000000000000000010";
            }
            else
            {
                cc = "0";
                SW = "000000000000000000000001";
            }

        }
        else
        {

            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            if (A == (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)))
            {
                cc = "1";
                SW = "000000000000000000000000";
            }
            else if (A < (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)))
            {
                cc = "-1";
                SW = "000000000000000000000010";
            }
            else
            {
                cc = "0";
                SW = "000000000000000000000001";
            }
        }

    }
    else if (op == "10100000")//CompR
    {
        cout << "COMPR,";
        determine_reg(r1) = addBinaryStrings(determine_reg(r1), "000000000000000000000000");
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");
        if (determine_reg(r1) == determine_reg(r2))
        {
            cc = "1";
            SW = "000000000000000000000000";
        }
        else if (determine_reg(r1) < determine_reg(r2))
        {
            cc = "-1";
            SW = "000000000000000000000010";
        }
        else
        {
            cc = "0";
            SW = "000000000000000000000001";
        }



    }
    else if (op == "00100100")//DIV
    {
        A = addBinaryStrings(A, "000000000000000000000000");
        TA = addBinaryStrings(TA, "000000000000000000000000");
        cout << "DIV,";
        if ((n + i) == "01")
        {

            A = Divide(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = addBinaryStrings(A, "000000000000000000000000");

            A = Divide(A, (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)));
            A = addBinaryStrings(A, "000000000000000000000000");
        }

    }

    else if (op == "10011100")//DIVR
    {
        cout << "DIVR,";
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");
        determine_reg(r1) = addBinaryStrings(determine_reg(r1), "000000000000000000000000");
        determine_reg(r2) = Divide(determine_reg(r2), determine_reg(r1));
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");


    }
    else if (op == "00111100")//Jump
    {
        cout << "JUMP,";

        PC = TA;


    }

    else if (op == "00110000")//JEQ
    {
        cout << "JEQ,";
        if (cc == "1")
            PC = TA;


    }
    else if (op == "00110100")//JGT
    {
        cout << "JGT,";
        if (cc == "0")
            PC = TA;
    }
    else if (op == "00111000")//JLT
    {
        cout << "JLT,";

        if (cc == "-1")
            PC = TA;


    }
    else if (op == "01000000")//JSUB
    {
        cout << "JSUB,";

        L = PC;
        PC = TA;




    }
    else if (op == "01001100")//RSUB
    {
        cout << "RSUB,";

        PC = L;





    }
    else if (op == "00000000")//LDA
    {

        cout << "LDA,";
        if ((n + i) == "01")
        {

            A = TA;
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));

        }
    }
    else if (op == "01101000")//LDB
    {

        cout << "LDB,";
        if ((n + i) == "01")
        {

            B = TA;
            B = addBinaryStrings(B, "000000000000000000000000");
        }
        else {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            B = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));

        }
    }
    else if (op == "01010000")//LDCH
    {
        cout << "LDCH,";
        if ((n + i) == "01")
        {
            string temp;
            temp = TA.substr(4, 8);

            if (format == "3")
            {
                A = replaceBits(A, temp, 16);
            }
            if (format == "4")
            {
                A = replaceBits(A, temp, 24);
            }
        }
        else {
            /*
            0 1 2 3 4 5 6 7
            8 9 10 11 12 13 14 15
            16 17 18 19 20 21 22 23
            24 25 26 27 28 29 30 31

            */
            string temp;
            temp = memory.read(TA);

            if (format == "3")
            {
                A = replaceBits(A, temp, 16);
            }
            if (format == "4")
            {
                A = replaceBits(A, temp, 24);
            }
        }
    }
    else if (op == "00001000")//LDL
    {
        cout << "LDL,";
        if ((n + i) == "01")
        {

            L = TA;
            L = addBinaryStrings(L, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            L = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));

        }


    }
    else if (op == "01101100")//LDS
    {
        cout << "LDS,";
        if ((n + i) == "01")
        {

            S = TA;
            S = addBinaryStrings(S, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            S = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));
        }

    }
    else if (op == "01110100")//LDT
    {
        cout << "LDT,";
        if ((n + i) == "01")
        {

            T = TA;
            T = addBinaryStrings(T, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            T = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));
        }

    }
    else if (op == "00000100")//LDX
    {
        cout << "LDX,";
        if ((n + i) == "01")
        {

            X = TA;
            X = addBinaryStrings(X, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            X = (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));
        }

    }
    else if (op == "00100000")//MUL
    {
        A = addBinaryStrings(A, "000000000000000000000000");
        TA = addBinaryStrings(TA, "000000000000000000000000");
        cout << "MUL,";
        if ((n + i) == "01")
        {

            A = Multiply(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = Multiply(A, (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)));
            A = addBinaryStrings(A, "000000000000000000000000");
        }

    }
    else if (op == "10011000")//MULR
    {
        cout << "MULR,";
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");
        determine_reg(r1) = addBinaryStrings(determine_reg(r1), "000000000000000000000000");
        determine_reg(r2) = Multiply(determine_reg(r2), determine_reg(r1));
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");


    }
    else if (op == "01000100")//OR
    {
        A = addBinaryStrings(A, "000000000000000000000000");
        TA = addBinaryStrings(TA, "000000000000000000000000");
        cout << "OR,";
        if ((n + i) == "01")
        {

            A = OR(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = OR(A, (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)));

        }


    }
    else if (op == "10100100")//SHIFTL
    {
        cout << "SHIFTL,";

        determine_reg(r1) = addBinaryStrings(determine_reg(r1), "000000000000000000000000");
        determine_reg(r1) = shiftLeft(determine_reg(r1), N);



    }
    else if (op == "10101000")//SHIFTR
    {

        cout << "SHIFTR,";

        determine_reg(r1) = addBinaryStrings(determine_reg(r1), "000000000000000000000000");
        determine_reg(r1) = ShiftRight(determine_reg(r1), N);




    }
    else if (op == "00001100")//STA
    {
        cout << "STA,";
        A = addBinaryStrings(A, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, A.substr(16, 8));
        memory.write(TAplus1, A.substr(8, 8));
        memory.write(TAplus2, A.substr(0, 8));
    }
    else if (op == "01111000")//STB
    {
        cout << "STB,";
        B = addBinaryStrings(B, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, B.substr(16, 8));
        memory.write(TAplus1, B.substr(8, 8));
        memory.write(TAplus2, B.substr(0, 8));
    }
    else if (op == "01010100")//STCH
    {
        cout << "STCH,";
        A = addBinaryStrings(A, "000000000000000000000000");
        string TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        string TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        string TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, A.substr(16, 8));

    }
    else if (op == "00010100")//STL
    {
        cout << "STL,";

        L = addBinaryStrings(L, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, L.substr(16, 8));
        memory.write(TAplus1, L.substr(8, 8));
        memory.write(TAplus2, L.substr(0, 8));
    }
    else if (op == "01111100")//STS
    {
        cout << "STS,";
        S = addBinaryStrings(S, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, S.substr(16, 8));
        memory.write(TAplus1, S.substr(8, 8));
        memory.write(TAplus2, S.substr(0, 8));
    }
    else if (op == "11101000")//STSW
    {
        cout << "STSW,";
        SW = addBinaryStrings(SW, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, SW.substr(16, 8));
        memory.write(TAplus1, SW.substr(8, 8));
        memory.write(TAplus2, SW.substr(0, 8));
    }
    else if (op == "10000100")//STT
    {
        cout << "STT,";

        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, T.substr(16, 8));
        memory.write(TAplus1, T.substr(8, 8));
        memory.write(TAplus2, T.substr(0, 8));
    }
    else if (op == "00010000")//STX
    {
        cout << "STX,";
        X = addBinaryStrings(X, "000000000000000000000000");
        TAplus1 = addBinaryStrings(TA, "00000000000000000001");
        TAplus2 = addBinaryStrings(TA, "00000000000000000010");
        TAplus3 = addBinaryStrings(TA, "00000000000000000011");
        memory.write(TA, X.substr(16, 8));
        memory.write(TAplus1, X.substr(8, 8));
        memory.write(TAplus2, X.substr(0, 8));
    }
    else if (op == "00011100")//SUB
    {
        cout << "SUB,";
        A = addBinaryStrings(A, "000000000000000000000000");
        if ((n + i) == "01")
        {

            A = Sub(A, TA);
            A = addBinaryStrings(A, "000000000000000000000000");
        }
        else
        {
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            A = Sub(A, memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA));

        }


    }
    else if (op == "10010100")//SUBR
    {
        cout << "SUBR,";
        determine_reg(r2) = Sub(determine_reg(r2), determine_reg(r1));
        determine_reg(r2) = addBinaryStrings(determine_reg(r2), "000000000000000000000000");

    }
    else if (op == "00101100")//TIX
    {
        cout << "TIX,";
        if ((n + i) == "01")
        {
            X = addBinaryStrings(X, "000000000000000000000001");
            if (X == TA)
            {
                cc = "1";
                SW = "000000000000000000000000";
            }
            else if (X < TA)
            {
                cc = "-1";
                SW = "000000000000000000000010";
            }
            else
            {
                cc = "0";
                SW = "000000000000000000000001";
            }

        }
        else
        {
            X = addBinaryStrings(X, "000000000000000000000001");
            TAplus1 = addBinaryStrings(TA, "00000000000000000001");
            TAplus2 = addBinaryStrings(TA, "00000000000000000010");
            TAplus3 = addBinaryStrings(TA, "00000000000000000011");
            if (X == (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)))
            {
                cc = "1";
                SW = "000000000000000000000000";
            }
            else if (X < (memory.read(TAplus2) + memory.read(TAplus1) + memory.read(TA)))
            {
                cc = "-1";
                SW = "000000000000000000000010";
            }
            else
            {
                cc = "0";
                SW = "000000000000000000000001";
            }
        }
    }
    else if (op == "10111000")//TIXR
    {

        cout << "TIXR,";
        X = addBinaryStrings(X, "000000000000000000000001");


        if (X == determine_reg(r1))
        {
            cc = "1";
            SW = "000000000000000000000000";
        }
        else if (X < determine_reg(r1))
        {
            cc = "-1";
            SW = "000000000000000000000010";
        }
        else
        {
            cc = "0";
            SW = "000000000000000000000001";
        }


    }
    else if (op == "10101100")//RMO
    {
        cout << "RMO,";
        determine_reg(r2) = determine_reg(r1);



    }
    else if (op == "11100000")//TD
    {
        cout << "TD,";
        ifstream inputFile("inputport.txt");//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here

        if (!inputFile.is_open()) {
            { cout << "Device is busy.";
            cc = "1";
            SW = "000000000000000000000000";
            }

        }
        else
        {
            cc = "-1";
            SW = "000000000000000000000010";
            cout << "Device is ready.";
        }
        inputFile.close();


    }
    else if (op == "11011100")//WD
    {
        cout << "WD,";
        Write(porttext, "output.txt");//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here

    }
    else if (op == "11011000")//RD
    {
        cout << "RD,";
        if ((n + i) == "01")
        {
            id = TA;

            id = binaryToHex(id);
            id = Addleadzero1(id);

            Read("inputport.txt", id, porttext);//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here
        }
        else
        {
            id = binaryToHex(memory.read(TA));
            id = Addleadzero1(id);
            Read("inputport.txt", id, porttext);//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here
        }
    }
}
InstructionFormat determineInstructionFormat(const string& binaryNumber, Memory& mem)
{

    if (binaryNumber.length() == 24)
    {
        // Check if it's a Standard SIC instruction format (size = 24 bits and n=0,i=0)
        if (binaryNumber[6] == '0' && binaryNumber[7] == '0') {
            op = binaryNumber.substr(0, 8);
            n = binaryNumber[6];
            i = binaryNumber[7];
            x = binaryNumber[8];
            b = binaryNumber[9];
            p = binaryNumber[10];
            e = binaryNumber[11];
            flags = binaryNumber.substr(6, 6);

            address = binaryNumber.substr(9, 15);
            newPC = addBinaryStrings(PC, "00000000000000000011");
            compute_TA(flags, mem);

            memory.write(PC, binaryNumber.substr(0, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");
            memory.write(PC, binaryNumber.substr(8, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");
            memory.write(PC, binaryNumber.substr(16, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");

            instructions();


            return InstructionFormat::StandardSIC;
        }
        // Check if it's a Format 3 instruction format (size = 24 bits and n=1, i=1, e=0)
        else if ((binaryNumber[6] == '1' && binaryNumber[7] == '1' || binaryNumber[6] == '0' && binaryNumber[7] == '1' || binaryNumber[6] == '1' && binaryNumber[7] == '0') && binaryNumber[11] == '0') {
            op = binaryNumber.substr(0, 6) + "00";
            n = binaryNumber[6];
            i = binaryNumber[7];
            x = binaryNumber[8];
            b = binaryNumber[9];
            p = binaryNumber[10];
            e = binaryNumber[11];
            flags = binaryNumber.substr(6, 6);
            disp = binaryNumber.substr(12, 12);

            format = "3";

            newPC = addBinaryStrings(PC, "00000000000000000011");
            compute_TA(flags, mem);


            memory.write(PC, binaryNumber.substr(0, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");
            memory.write(PC, binaryNumber.substr(8, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");
            memory.write(PC, binaryNumber.substr(16, 8));
            PC = addBinaryStrings(PC, "00000000000000000001");

            instructions();


            return InstructionFormat::Format3;
        }
    }
    else if (binaryNumber.length() == 8) {
        // Check if it's a Format 1 instruction format (size = 8 bits and consists of just opcode)
        op = binaryNumber;
        memory.write(PC, binaryNumber.substr(0, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");
        instructions();

        return InstructionFormat::Format1;
    }
    else if (binaryNumber.length() == 16) {
        // Check if it's a Format 2 instruction format (size = 16 bits and operands are just registers)
        op = binaryNumber.substr(0, 8);
        r1 = binaryNumber.substr(8, 4);
        r2 = binaryNumber.substr(12, 4);
        N = addBinaryStrings(r2, "0001");
        memory.write(PC, binaryNumber.substr(0, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");
        memory.write(PC, binaryNumber.substr(8, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");

        instructions();

        return InstructionFormat::Format2;
    }
    else if (binaryNumber.length() == 32 && binaryNumber[11] == '1') {
        // Check if it's a Format 4 instruction format (size = 32 bits and e=1)
        op = binaryNumber.substr(0, 6) + "00";
        n = binaryNumber[6];
        i = binaryNumber[7];
        x = binaryNumber[8];
        b = binaryNumber[9];
        p = binaryNumber[10];
        e = binaryNumber[11];
        flags = binaryNumber.substr(6, 6);
        address = binaryNumber.substr(12, 20);
        format = "4";

        newPC = addBinaryStrings(PC, "00000000000000000100");
        compute_TA(flags, mem);


        memory.write(PC, binaryNumber.substr(0, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");
        memory.write(PC, binaryNumber.substr(8, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");
        memory.write(PC, binaryNumber.substr(16, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");
        memory.write(PC, binaryNumber.substr(24, 8));
        PC = addBinaryStrings(PC, "00000000000000000001");


        instructions();

        return InstructionFormat::Format4;

    }


}





//print functions
void printInstructionFormats(const vector<string>& binaryNumbers)
{
    //This function only prints on console for testing
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Instruction Formats:" << endl;
    for (const auto& binaryNumber : binaryNumbers)
    {

        InstructionFormat format = determineInstructionFormat(binaryNumber, memory);
        switch (format) {
        case InstructionFormat::StandardSIC:

            cout << "Standard SIC  (24 bits" << ", n =" << n << ", i =" << i << ",x=" << x << ", opcode = " << op << ",address=" << address << ", PC= " << PC << ", TA=" << TA << ")" << endl;
            break;
        case InstructionFormat::Format1:
            cout << "Format 1  (8 bits, opcode only" << ", opcode=" << op << ", PC=" << PC << ")" << endl;
            break;
        case InstructionFormat::Format2:
            cout << "Format 2  (16 bits, register operands," << " opcode = " << op << ", r1=" << r1 << ",r2=" << r2 << ", PC=" << PC << ")" << endl;
            break;
        case InstructionFormat::Format3:

            cout << "Format 3  (24 bits," << "opcode = " << op << ",n=" << n << ",i=" << i << ",x=" << x << ",b=" << b << ",p=" << p << ",e=" << e << ",disp=" << disp << ", PC=" << PC << ", TA=" << TA << ")" << endl;
            break;
        case InstructionFormat::Format4:
            cout << "Format 4  (32 bits," << "opcode=" << op << ",n=" << n << ",i=" << i << ",x=" << x << ",b=" << b << ",p=" << p << ",e=" << e << ",address=" << address << ", PC=" << PC << ", TA=" << TA << ")" << endl;
            break;
        default:
            cout << "Invalid format" << endl;
            break;
        }
    }
}
void printMemory(const Memory& memory, const string& outputFile, const string& pcStart, const string& pcLast) {
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Memory:" << endl;
    ofstream outFile(outputFile, ios::app); // Open the file in append mode
    if (!outFile.is_open()) {
        cout << "Failed to open output file." << endl;
        return;
    }

    int startAddress = bitset<20>(pcStart).to_ulong(); // Convert binary string to integer
    int lastAddress = bitset<20>(pcLast).to_ulong();   // Convert binary string to integer
    outFile << "!! Memory" << endl;
    for (int address = startAddress; address <= lastAddress; ++address) {
        string hexValue = bitset<8>(memory.read(bitset<20>(address).to_string())).to_string();
        string outputLine = "Memory[" + binaryToHex(bitset<20>(address).to_string()) + "] = " + hexValue + "\tInhexa = " + Addleadzero1(binaryToHex(hexValue)) + "\n";
        cout << outputLine;  // Print to console
        outFile << outputLine;    // Write to output file
    }
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << endl;



}
void writeRegiesters()
{
    ofstream outputFile("output.txt", ios::app);//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here

    if (!outputFile.is_open()) {
        cerr << "Unable to open the output file." << endl;
        return;
    }

    outputFile << "!! Registers" << endl;
    outputFile << "A=" << A << "\t0x" << Addleadzero2(binaryToHex(A)) << endl;
    outputFile << "X=" << X << "\t0x" << Addleadzero2(binaryToHex(X)) << endl;
    outputFile << "L=" << L << "\t0x" << Addleadzero2(binaryToHex(L)) << endl;
    outputFile << "PC=" << PC << "\t\t0x" << Addleadzero2(binaryToHex(PC)) << endl;
    outputFile << "SW=" << SW << "\t0x" << Addleadzero2(binaryToHex(SW)) << endl;
    outputFile << "B=" << B << "\t0x" << Addleadzero2(binaryToHex(B)) << endl;
    outputFile << "S=" << S << "\t0x" << Addleadzero2(binaryToHex(S)) << endl;
    outputFile << "T=" << T << "\t0x" << Addleadzero2(binaryToHex(T)) << endl;
    outputFile << endl;
    outputFile.close();
}

//The main
int main()
{
    ofstream outputFile("output.txt");
    outputFile.close();
    cout << "----------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    memory.write("00000000000100011011", "00001111");
    memory.write("00000000000100011100", "11110000");
    memory.write("00000000000100011101", "00001111");
    memory.write("000000000000001", "00001000");
    Readinput(("input.txt"));//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here
    printInstructionFormats(binaryNumbers);
    writeRegiesters();
    printMemory(memory, "output.txt", PCstart, addBinaryStrings(PClast, "10"));//------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter the path here

    return 0;
}