/*

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
/* and

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

void printMatching(char closing) {
    string opening = "Q";

    if (closing == ')') opening = '(';
    else if (closing == ']') opening = '[';
    else if (closing == '}') opening = '{';

    cout << "pair matching " << opening << " and " << closing << endl;
}

bool matchingBraces(char closing, char opening) { //reference so we don't create a stack copy -- inefficient
    char neededOpening = 'Q';

    if (closing == ')') neededOpening = '(';
    else if (closing == ']') neededOpening = '[';
    else if (closing == '}') neededOpening = '{';
    else if (closing == '>') neededOpening = '<'; // FOR BLOCK COMMENTS
    if (opening == neededOpening) 
        return true;

    else
        return false;
}

int findLastQuoteIndex(string currentLine, int CurrentQuoteIndex, char quoteType) { //pass in ' or "
    int lastQuoteIndex = -1;
    
    for (int i = CurrentQuoteIndex + 1; i < currentLine.size(); i++) {
        if (currentLine[i] == quoteType)
            return i;
            //lastQuoteIndex = i;
    }

    return lastQuoteIndex;
}

bool balanceFile(istream& dictfile) {
    
    stack<char> stk;            //INVARIANT: 
    stack<int> lineNumbers;     //POP AND PUSH STACKS AT THE SAME TIME

    string currentLine;
    int numCurrentLine = 0;
    bool match = true;

    //HANDLES MULTI-LINE COMMENTS -- NEEDED OUTSIDE WHILE LOOP B/C TRACKS MULTIPLE LINES IN THE FILE
    bool inBlockComment = false;
    bool inQt = false;

    while (getline(dictfile, currentLine)) {
        numCurrentLine++;
        char popFromStack;
        
        // Handling single-line and multi-line comments
        bool continueLine = true;

        //Handling Quotation marks
        bool checkedSingleQt = false;
        bool checkedDoubleQt = false;
        int lastDoubleQtIndx = -1;
        int lastSingleQtIndx = -1;

        for (int i = 0; i < currentLine.size(); i++) {


            //Check for in-line comments and block comments
            // Make sure that you can check one character ahead in the line
            
            
            if (i < currentLine.size() - 1) {
                if (currentLine[i] == '/') {
                    if (currentLine[i + 1] == '*' && !inQt) {
                        stk.push('<'); // Send /* as <
                        lineNumbers.push(numCurrentLine);
                        inBlockComment = true;
                        continue;
                    }
                    else if (currentLine[i + 1] == '/') {
                        continueLine = false; // Break out of reading this line if // 
                        continue;
                    }
                }

                else if (currentLine[i] == '*' && currentLine[i + 1] == '/' && !inQt) {
                    if (stk.empty()) {
                        popFromStack = 0;
                    }

                    else {
                        popFromStack = stk.top();
                        stk.pop();
                        lineNumbers.pop();
                    }

                    
                    if (!matchingBraces('>', popFromStack)) {
                        cout << "unmatched " << "" << " on line " << numCurrentLine;
                    }
                    else {
                        cout << "pair matching /* and " << endl;
                        inBlockComment = false;
                    }

                }
            }

            if (!continueLine)
                break;


            //HANDLING QUOTATION MARKS and Looking For Block Comment
            if (i < lastDoubleQtIndx || i < lastSingleQtIndx) {
                inQt = true;
                continue;
            }

            else
                inQt = false;
                

            if (currentLine[i] == '\'' && !checkedSingleQt) {
                checkedSingleQt = true;
                stk.push('\'');
                lineNumbers.push(numCurrentLine);

                lastSingleQtIndx = findLastQuoteIndex(currentLine, i, currentLine[i]);

                if (lastSingleQtIndx < 1)
                    match = false;
                else {
                    stk.pop();
                    lineNumbers.pop();
                    cout << "pair matching \' and \'" << endl;
                }
            }

            if (currentLine[i] == '\"' && !checkedDoubleQt) {
                checkedDoubleQt = true;
                stk.push('\"');
                lineNumbers.push(numCurrentLine);

                lastDoubleQtIndx = findLastQuoteIndex(currentLine, i, currentLine[i]);
                if (lastDoubleQtIndx < 1)
                    match = false;
                else {
                    stk.pop();
                    lineNumbers.pop();
                    cout << "pair matching \" and \"" << endl;
                }
            }
            //END OF: HANDLING QUOTATION MARKS
            
            //HANDLING BRACES
            switch (currentLine[i]) {
            case '(':
            case '{':
            case '[':
                stk.push(currentLine[i]);
                lineNumbers.push(numCurrentLine);
                continue;
            }

            switch (currentLine[i]) {
            case ')':
            case '}':
            case ']':
                if (stk.empty()) {
                    match = false;
                    cout << "unmatched " << currentLine[i] << " on line " << numCurrentLine;
                    break;
                }
                
                if (!stk.empty()) {
                    popFromStack = stk.top();
                    stk.pop();
                    lineNumbers.pop();
                }
                
               
                if (!matchingBraces(currentLine[i], popFromStack)) {
                    match = false;
                    cout << "unmatched " << currentLine[i] << " on line " << numCurrentLine;
                    break;
                }

                printMatching(currentLine[i]);
            }
            //END OF: HANDLING BRACES
        }
        if (!match)
            break;
    }
    
    if (!stk.empty()) {
        match = false;
        string  mismatch = string(1, stk.top());
        int mismatchedLine = lineNumbers.top();
        
        switch (stk.top()) { // IDK how else to make this print properly. 
        case '<':
            mismatch = "/*";
            break;
        case'\'':
            mismatch = "\'";
            break;
        case '\"':
            mismatch = '\"';
            break;
        }     

        cout << "unmatched " << mismatch << " on line " << mismatchedLine << endl;
    }


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
*/