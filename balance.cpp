#include <iostream>
#include <fstream>
#include <istream>
#include <stack> //added
#include <string> //added
#include <iomanip> //added
using namespace std;

/* Pair
[ and ]
( and )
{ and }
' and '
" and "
/* and */

struct Brace {                              //NOTE: WE STORE /* AND */ AS < AND > RESPECTIVELY
    Brace(char b, int l) 
    {
        brace = b;
        line = l;
    }
    char brace;
    int line;
};

void printFile(istream& dictfile) {
    int index = 1;
    while (!dictfile.eof()) {
        string currentLine;
        getline(dictfile, currentLine);
        cout << right << setw(2) << index++;
        cout << left << " " << currentLine << endl;
    }

    dictfile.clear();
    dictfile.seekg(0, dictfile.beg);
}


bool checkInLineComment(const string& currentLine, const int index) {
    if (currentLine[index] == '/' && currentLine[index + 1] == '/')
        return true;
    else
        return false;
}

bool matchingBraces(char closing, char opening) { //reference so we don't create a stack copy -- inefficient
    char neededOpening = 'Q';

    if (closing == ')') neededOpening = '(';
    else if (closing == ']') neededOpening = '[';
    else if (closing == '}') neededOpening = '{';
    
    if (opening == neededOpening)
        return true;

    else
        return false;
}

void printErrorOpening(stack<Brace> &stk) {
    if (stk.top().brace == '<') 
        cout << "unmatched /* at line " << stk.top().line << endl;
    else
        cout << "unmatched " << stk.top().brace << " at line " << stk.top().line << endl;
}

void printMatching(char closing) {
    string opening = "Q";

    if (closing == ')') opening = '(';
    else if (closing == ']') opening = '[';
    else if (closing == '}') opening = '{';
    else if (closing == '\'') opening = '\'';
    else if (closing == '\"') opening = '\"';

    cout << "pair matching " << opening << " and " << closing << endl;
}

bool balanceFile(istream& dictfile) {

    stack<Brace> stk;
    Brace closing('Q',-1);

    string currentLine;
    int lineNumber = 0;
    bool inBlockComment = false;
    
    bool match = true;
    bool errorPrinted = false;

    while (getline(dictfile, currentLine)) {
        lineNumber++;
        
        bool continueLine = true;
        bool inQt = false;

        for (int i = 0; i < currentLine.size(); i++) {
            
            if (!continueLine || !match)
                break;


            // HANDLING BLOCK COMMENTS IF: 
            // NOT IN QUOTATION MARKS AND WE CAN INDEX 
            // AT LEAST TWO CHARACTERS IN CURRENTLINE
            if (!inQt && i < currentLine.size() - 1) {
                //In Line Comments
                if (!inBlockComment && currentLine[i] == '/' && currentLine[i + 1] == '/') {
                    continueLine = false;
                    break;
                }
            
                else if (!inBlockComment && currentLine[i] == '/' && currentLine[i + 1] == '*') {
                    inBlockComment = true;
                    Brace temp('<', lineNumber);
                    stk.push(temp); // Push /* as <
                }

                else if (currentLine[i] == '*' && currentLine[i + 1] == '/') {
                    inBlockComment = false;
                    if (!stk.empty() && stk.top().brace == '<') {
                        stk.pop();
                        cout << "pair matching /* and */" << endl;
                    }

                    else {
                        match = false;
                        errorPrinted = true;
                        cout << "unmatched */ at line " << lineNumber << endl;
                        break;
                    }
                }

                if (inBlockComment) 
                    continue;
      
            }

            // HANDLING QUOTATION MARKS IF WE'RE NOT:
            // IN A BLOCK COMMENT
            if (!inBlockComment) {
                if (currentLine[i] == '\'' || currentLine[i] == '\"') {
                    if (!inQt) {
                        Brace temp(currentLine[i], lineNumber);
                        stk.push(temp);
                        inQt = true;
                    }

                    else if (inQt) {
                        if (!stk.empty() && stk.top().brace == currentLine[i]) {
                            inQt = false;
                            stk.pop();
                            printMatching(currentLine[i]);
                        }
                    }
                }

                else if (inQt)
                    continue;
            }

            //HANDLING BRACES
            if (!inBlockComment && !inQt) {
                switch (currentLine[i]) {
                case '(':
                case '{':
                case '[':
                    stk.push(Brace(currentLine[i], lineNumber));
                    continue;
                }

                switch (currentLine[i]) {
                case ')':
                case '}':
                case ']':
                    if (stk.empty()) {
                        match = false;
                        cout << "unmatched symbol " << currentLine[i] << " at line " << lineNumber << endl;
                        errorPrinted = true;
                        break;
                    }
                    
                    char popFromStack = stk.top().brace;
                    stk.pop();

                    if(!matchingBraces(currentLine[i], popFromStack)){
                        match = false;
                        cout << "unmatched symbol " << currentLine[i] << " at line " << lineNumber << endl;
                        errorPrinted = true;
                        break;
                    }

                    else {
                        printMatching(currentLine[i]);
                    }
                }
                
            } //END OF: HANDLING BRACES

        }

        if(inQt)                //If we're at the end of the line and STILL in a quote -- No Match
            match = false;
        if (!match)             // stop checking after we find out there is no match
            break;


    }

    if (!stk.empty()) 
        match = false;
    
    if (!errorPrinted && !stk.empty()) //Since we only store Opening Braces, We need to check if an error is already printed 
        printErrorOpening(stk);        //e.g. closing brace with no opening  and that the stack is !empty
    
        

    return match; 
}

int main()
{
    ifstream infile;
    string filename;

    cout << "Please enter filename for C++ code: ";
    cin >> filename;
    infile.open(filename.c_str());

    if (!infile) {
        cout << "File not found!" << endl;
        return (1);
    }

    printFile(infile);

    if (balanceFile(infile))
        cout << "balance ok" << endl;
} 
/* /* */