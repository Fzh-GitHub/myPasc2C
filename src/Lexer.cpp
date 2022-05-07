#include "Lexer.hpp"

Lexer::Lexer(){
    init();
    //readToBuffer();
    // transWordToToken();
}
void Lexer::init(){
    pos.now_col = pos.now_line = pos.lines = 1;
    for( int i = 0; i < MAXLINE; i++ ){
        pos.line_num[i] = 0;
    }
    pos_buffer = 0;
    len_output = 0;
    state = 0;
    word = "";
    c = 0;
}

void Lexer::readToBuffer(const string &path){
    ifstream ifptr;
    ifptr.open(path,ios::in);
    if( !ifptr.is_open() ){
        cout << "未成功打开文件"<< endl;
        exit(0);
    }
    char tempc = 0;
    while( (tempc = ifptr.get()) != EOF ){
        buffer.push_back(tempc);
    }
    ifptr.close();
    buffer.push_back(-1);
}
void Lexer::getChar(){
    c = buffer[pos_buffer++];
    if( pos_buffer > buffer.size() ){
        cerr << "Please stop,buffer out of range!" << endl;
        exit(-1);
    }
    changePos(c,1);
    //if( c == -1 ) return;

    word += c;
    // if( isLegalChar(c) == false ) {
    //     processError(0);
    // }
}
void Lexer::getBlank(){
    char tempc = buffer[pos_buffer];
    while( tempc == ' ' || tempc == '\n'){
        pos_buffer++;
        changePos(tempc,1);
        tempc = buffer[pos_buffer];
    }
}
void Lexer::retract(){
    char tempc = word[word.length()-1];
    word.pop_back();
    pos_buffer--;
    changePos(tempc,-1);
}
string Lexer::getNote(){
    string note = "";
    char tempc;
    while( (tempc = buffer[pos_buffer++]) != '}' && tempc != -1 ){
        changePos(tempc,1);
        note += tempc;
    }
    if( tempc == -1 ) pos_buffer--;
    changePos(tempc,1);
    return note;
}
bool Lexer::isKey(string str){
   for( int i = 0; i < save_words.size(); i++ ){
       if( word == save_words[i] ) {
           return true;
        }
   }
   return false;
}
bool Lexer::isDigit(char c){
    if( c >= '0' && c <= '9' ) return true;
    else return false;
}
bool Lexer::isLetter(char c){
    if( (c >= 'a'&& c <= 'z') || (c >= 'A' && c <='Z') ) return true;
    else return false;
}
bool Lexer::isMulop(string str){
   for( int i = 0; i < mulops.size(); i++ ){
       if( str == mulops[i] ) return true;
   }
   return false;
}
void Lexer::fillOutput(string word,string token){
    Output temp;
    string token2 = "";
    
    if( isKey(word) && ( word[0] >= 'A' && word[0] <= 'Z' ) && !isMulop(word) ) token[0] = token[0] - ( 'A' -'a' );
     if( word == "or" || word == "Or" ) token = "addop";

    if( state == 2 ) token2 = "integer";
    else if( state == 3 ) token2 = "real";
    else token2 = token;
    if( addCol != 0 ) {
        temp.create(pos.now_line,pos.now_col-word.length()-addCol,word,token,token2);
        addCol = 0;
    }
    else{
    temp.create(pos.now_line,pos.now_col-word.length(),word,token,token2);
    }
    output.push_back(temp);
    len_output++;
}
void Lexer::backToState0(int out,string token){
    if( out == 1 ) retract();
    fillOutput(word,token);
    state = 0;
}
bool Lexer::isMyself(char c){
    int temp = pos_buffer;
    if( c == '+' || c == '-' ){
        temp -= 2;
        while( buffer[temp] == ' ' ) temp --;
        if( buffer[temp] == '=' ) return true;
        else return false;
    }
    else if( c == '=' ){
        string str = "";
        while( buffer[temp] != '\n' ){
            str += buffer[temp];
            temp --;
        }
        if( str.find("tsnoc") == str.npos ) return false;
        else return true;
    
    }
    return true;
}
void Lexer::changePos(char c,int type){
    if( type == 1 ){
        if( c == '\n' ){
            pos.line_num[pos.now_line] = pos.now_col;
            pos.now_col = 1;
            pos.now_line++;
            pos.lines++;
        }
        else if( c == -1 ){
            pos.line_num[pos.now_line] = pos.now_col;
            pos.now_col++;
        }
        else pos.now_col++;
        
    }
    else if( type == -1 ){
        if( c == '\n' ){
            pos.now_line--;
            pos.lines--;
            pos.now_col = pos.line_num[pos.now_line];
        }
        else pos.now_col--;
    }
}
bool Lexer::isLegalChar(char c){
    if( isDigit(c) ) return true;
    else if( isLetter(c) ) return true;
    for( int i = 0; i < legalChar.size();i++ ){
        if( c == legalChar[i] ) return true;
    }
    return false;
}
void Lexer::processError(int index){
    Error er;
    bool fix ;
    fix = fixError(index);
    if( fix == true ) return;
    switch(index){
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
            getUntilBlank();
        case 4:
            er.create(pos.now_line,pos.now_col-word.length(),index,word);
            state = 0;
            break;
        default:
            cout << "Unexpected Error!" << endl;
            break;
    }
    error.push_back(er);
    //cout << pos.now_line << "  " << pos.now_col << ":" <<word<<"  " <<index << " " << pos_buffer <<endl;
}
void Lexer::processWarn(int index){
    Warn wn;
    int col = 0;
    switch (index)
    {
    case 0:
        col = pos.now_col-word.length()-addCol;
        wn.create(pos.now_line,col,index,word);
        break;
    case 1:
        col = pos.now_col-1;
        wn.create(pos.now_line,col,index,word);
        break;
    default:
        cout << "No such Warn" << endl;
        break;
    }
    
    warn.push_back(wn);
}
bool Lexer::haveBlank(){
    if( word == "program" || word == "end") return true;
    if( c != ' ' && c != '\n' && c !='{') return false;
    int len = word.length();
    int temp_pos_buffer = pos_buffer;
    char tempc = buffer[temp_pos_buffer-len-1];
    if( tempc != ' ' && tempc != '\n' && c !='}' ) return false;
    return true;
}
void Lexer::outPut(const string &tpath){
    ofstream ofptr2;
    ofptr2.open("debug.txt",ios::out);

    /*输出相关程序信息*/
    int sum = 0;
    cout << "Total Line:  " << pos.lines << endl;
    ofptr2 << "Total Line  " << pos.lines << endl;
    for( int i = 1; i <= pos.lines; i++ ) {
        sum += pos.line_num[i];
    }
    cout << "Total character:  " << sum-1 << endl << endl;
    ofptr2 << "Total character:  " << sum-1 << endl << endl;

    /*输出warn相关信息*/
    Warn w;
    cout << "Warn Num:  " << warn.size() << endl;
    ofptr2 << "Warn Num:  " << warn.size() << endl;
    for( int i = 0; i < warn.size(); i++ ){
        w = warn[i];
        cout << "[Warn" << i+1 << "]: in line " << w.line << ",col " << w.col << ".(Type" << w.state << ")\n WarnMessage:   ";
        cout << warn_message[ w.state ] << w.mes << endl << endl;
        ofptr2 << "[Warn" << i+1 << "]: in line " << w.line << ",col " << w.col << ".(Type" << w.state << ")\n WarnMessage:   ";
        ofptr2 << warn_message[ w.state ] << w.mes << endl << endl;
    }

    /*输出错误信息*/
    if( error.size() > 0 ){
        Error e;  
        cout << "Error Num:  " << error.size() << endl;
        ofptr2 << "Error Num:  " << error.size() << endl;
        for( int i = 0; i < error.size(); i++ ){
            e = error[i];
            cout << "[Error" << i+1 << "]: in line " << e.line << ",col " << e.col << ".(Type" << e.state << ") \nerrMessage:  ";
            cout << err_message[error[i].state] << error[i].mes << endl << endl;
            ofptr2 << "[Error" << i+1 << "]: in line " << e.line << ",col " << e.col << ".(Type" << e.state << ") \nerrMessage:  ";
            ofptr2 << err_message[error[i].state] << error[i].mes << endl << endl; 
        }
        cerr << "Please check your .pas program" << endl;
        ofptr2 << "Please check your .pas program" << endl;
    } 

    /*输出tokens*/
    else{
        ofstream ofptr;
        Output t;
        ofptr.open(tpath,ios::out);
        int len = 0;
        for( int i = 0; i < output.size(); i++ ){
            t = output[i];
            len = t.word.length();
            cout << t.line << ' ' << t.col << ' ' << t.word << ' ' << t.token << ' ' << t.token2 << endl;
            ofptr << t.line << ' ' << t.col << ' ' << t.word << ' ' << t.token << ' ' << t.token2 << endl;
        }
        ofptr.close();
    }
    ofptr2.close();
    
}
void Lexer::getUntilBlank(){
        getChar();
            while( c != ' ' && c != '\n' && c != -1 && c != '{'){
                getChar();
            }
        retract();    
}
bool Lexer::fixError(int index){
    /*删除了交互的部分*/
    // Error e;
    // e.create(pos.now_line,pos.now_col-word.length(),index,word);
    // cout << "\n[Attention]:  At " << e.line <<" line," << e.col << " col. one error occur!\n" << err_message[index] << e.mes << "\nFix it and continue?[Y/y] or exit?[e/E]:  ";
    // char choice = 0;
    // cin >> choice;
    // if( choice != 'y' && choice !='Y' ) return false;
    // if( choice == 'e' || choice == 'E' ) exit(0);
    Warn w;
    switch (index)
    {
    case 0:
    case 1:
    case 6:
        processWarn(1);
        word.pop_back();
        addCol++;
        return true;
    case 5:
    case 2:
    case 4:
    case 7:
    default:
        return false;
    }
}
void Lexer::transWordToToken(const string &tpath){
   string token = "";
    while( c != -1 ){
        switch(state){
            case 0 :
                word = ""; getBlank(); getChar();
                if( isLetter(c) ) state = 1;
                else if( isDigit(c) ) state = 2;
                else if( c ==  '=' ) state = 4;
                else if( c ==  '<' ) state = 5;
                else if( c ==  '>' ) state = 8;
                else if( c ==  '+' ) state = 10;
                else if( c ==  '-' ) state = 10;
                else if( c ==  '*' ) state = 10;
                else if( c ==  '/' ) state = 10;
                else if( c ==  ':' ) state = 11;
                else if( c ==  '(' ) state = 13;
                else if( c ==  ')' ) state = 13;
                else if( c ==  '[' ) state = 13;
                else if( c ==  ']' ) state = 13;
                else if( c ==  ',' ) state = 13;
                else if( c ==  ';' ) state = 13;
                else if( c ==  '.' ) state = 14;
                else if( c ==  '\'' ) state = 16;
                else if( c ==  '{' ) state = 18;
                else if( c == -1 ) continue; 
                else processError(6);
            break;
            case 1:
                getChar();
                if( isDigit(c) || isLetter(c) ){
                    state = 1;
                }
                else if( isLegalChar(c) == false ) processError(0); 
                else{    
                    retract();
                    if( isMulop(word) ) backToState0(0,"mulop");
                    else{
                        if( isKey(word) ) {
                            token = word;
                            if( haveBlank() == false) {
                                processWarn(0); 
                            }
                        }
                        else {
                            token = "id";
                            if( word.length() > MAXID ) {
                                processError(4);
                            }
                        }
                        backToState0(0,token);
                    }
                    
                }
            break;
            case 2:
                getChar();
                if( isDigit(c) ) state = 2;
                else if ( c == '.' ) state = 19;
                else if( isLetter(c) ) processError(1);
                else {
                    backToState0(1,"num");
                }
            break;
            case 3:
                getChar();
                if( isDigit(c) ) state = 3;
                else if( isLetter(c) ) { 
                    processError(1);
                    if( isDigit(buffer[pos_buffer]) ) state =3;
                }
                else if( isLegalChar(c) == false ){
                    processError(0);
                    if( isDigit(buffer[pos_buffer]) ) state =3;
                }
                else {
                    backToState0(1,"num");
                }
            break;
            case 4:
                if( isMyself('=') ) backToState0(0,"=");
                else backToState0(0,"relop");
            break;
            case 5:
                getChar();
                if( c == '>' ){
                    state = 6;
                }
                else if( c == '=' ){
                    state = 7;
                }
                else{
                    backToState0(1,"relop");
                }
            break;
            case 6:
            case 7:
                backToState0(0,"relop");
            break;
            case 8:
                getChar();
                if( c == '=' ) state = 9;
                else{
                    backToState0(1,"relop");
                }
            break;
            case 9:
                backToState0(0,"relop");
            break;
            case 10:
                if( c == '+' ) {
                    if( isMyself(c) ) token = "+";
                    else token = "addop";
                }
                else if( c == '-' ) {
                    if( isMyself(c) ) token = "-";
                    else token = "addop";
                }
                else token = "mulop";
                backToState0(0,token);
            break;
            case 11:
                getChar();
                if( c == '=' ){
                    state = 12;
                }
                else{
                    token = ":";
                    backToState0(1,token);
                }
            break;
            case 12:
                token = "assignop";
                backToState0(0,token);
            break;
            case 13:
                token = ""; token += c;
                backToState0(0,token);
            break;
            case 14:
                getChar();
                if( c == '.' ){
                    state = 15;
                }
                else if( c == -1 || c == '{' || c == ' ' || c == '\n' || isLetter(c)) {
                    token = ".";
                    backToState0(1,token);
                }
                else{
                    processError(5);
                }

            break;
            case 15:
                token = "..";
                backToState0(0,token);
            break;
            case 16:
                token = "\'";
                backToState0(0,token);
                word ="";
                getChar();
                if( isLetter(c) ) state = 17;
                else{
                    processError(7);
                }
            break;
            case 17:
                token = "letter";
                backToState0(0,token);
                word = "";
                getChar();
                if( c == '\'' ){
                    token = "\'";
                    backToState0(0,token);
                }   
                else{
                    processError(2);    //单引号中字符多于一个
                }
            break;
            case 18:
                getNote();
                state = 0;
            break;
            case 19:
                getChar();
                if( c == '.' ) {
                    //0..
                    retract();
                    state = 2;
                    backToState0(1,"num");
                }
                else if( isDigit(c) ) state = 3;
                else processError(5);
        }
    }
    outPut(tpath);
}