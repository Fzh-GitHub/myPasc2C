#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<set>
#define ERROR 100
#define MAXLINE 1000
#define MAXID 20
using namespace std;

struct Output{
    void create(int line,int col,string word,string token,string token2){
        this -> line = line;
        this -> col = col;
        this -> word = word;
        this -> token = token;
        this -> token2 = token2;
    }
    int line;
    int col;
    string word;
    string token;
    string token2;
};
struct Position{
    int now_line;
    int now_col;
    int lines;
    int line_num[MAXLINE];
};
struct Error{
    void create( int line,int col,int state,string mes ){
        this -> line = line;
        this -> col = col;
        this -> state = state;
        this -> mes = mes;
    }
    int line;
    int col;
    int state;
    string mes;
};
struct Warn{
    void create( int line,int col,int state,string mes ){
        this -> line = line;
        this -> col = col;
        this -> state = state;
        this -> mes = mes;
    }
    int line;
    int col;
    int state;
    string mes;
};
class Lexer{
    public:
        Lexer();
        void init();
        void readToBuffer(const string &spath);
        void getChar();
        void getBlank();
        void retract();
        string getNote();
        bool isKey(string str);
        bool isDigit(char c);
        bool isLetter(char c);
        bool isMulop(string str);
        bool isMyself(char c);
        bool isLegalChar(char c);
        void fillOutput(string word,string token);
        void transWordToToken(const string &tpath);
        void backToState0(int out,string token);
        void outPut(const string &tpath);
        void changePos(char c,int type);
        void processError(int index);
        void processWarn(int index);
        bool haveBlank();
        void getUntilBlank();
        bool fixError(int index);
    private:
        char c;
        int state;
        int pos_buffer;
        string word;
        vector<char> buffer;
        vector<Output> output;
        vector<Error> error;
        vector<Warn> warn;
        Position pos;
        int len_output;
        const vector<string> save_words=
        {
        "array",    "begin", "const", "else", "do","end", "for",
        "writeln",  "uminus","and",   "div",   "mod",  "or",
        "function", "if",    "not",   "of",   "procedure", "program", "record",
        "then",     "to",    "type",  "var",  "while",     "read",    "write",
        "integer","char","real","boolean",
        "Array",    "Begin", "Const", "Else", "Do","End", "For",
        "Writeln",  "Uminus","And",   "Div",   "Mod",  "Or",
        "Function", "If",    "Not",   "Of",   "Procedure", "Program", "Record",
        "Then",     "To",    "Type",  "Var",  "While",     "Read",    "Write",
        "Integer","Char","Real","Boolean"
        };
        const vector<string> mulops={"div","mod","and","Div","Mod","And"};
        const vector<char> legalChar={
            ' ','\n','+','-','*','/',
            '=','<','>','(',')','[',']',
            '{','}',',','.',';',':','\''
        };
        const vector<string> err_message={
            "Illegal char:  ",
            "Illegal num:  ",
            "More than one character in '':  ",
            "No blank around key-word:  ",
            "Id over max length:  ",
            "Illegal char after '.':  ",
            "Illegal char at the beginning of word:  ",
            "Not a letter in '\'':  ",
        };
        const vector<string> warn_message={
            "No blank around key-word:  ",
            "Auto correct Error:  "
        };
        int addCol = 0;
};