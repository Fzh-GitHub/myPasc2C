#include<iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>


#include "Semantic.hpp"


using namespace std;


// 1: 非终结符文法类型错误
// 2: 文法生成错误（节点的字子结点数错误）
// 3：出现+/-以外的操作符
// 4：终结符类型错误
// 5：数组下标错误
// 6：assignop（:=）符号两边类型不等
// 7：使用符号表中未声明的变量
// 8：relop（ =、<>、<、<=、>、>=）使用不当（两边操作数类型问题）
// 9：addop（+、- 和 or）使用不当（两边操作数类型问题）
// 10：mulop（*、/、div、mod 和 and）使用错误（两边操作数类型问题）
// 11：调用函数或过程时参数类型与符号表冲突
// 12: 该变量重定义
// 13：该变量不是过程
// 14: 该id不是可修改的变量
// 15: 符号表重定义
// 16: 该id不为record
// 17: 该id不为数组
// 18: 该id不是函数
// 19：read的参数不合规范

/* 语义分析报错 */
void SemanticAnalyzer::error(int type,int line,int id){
  this->isSemanticRight = false;
  string errorInfo;
  if(line) {
    cout<<"Error occured on line "<<line<<endl;
    errorInfo = "Error occured on line ";
    errorInfo += to_string(line);
  }
  cout<<"<NodeID: "<<id<<">  ";
  errorInfo += "  <NodeID:";
  errorInfo +=  to_string(id);
  errorInfo += ">  ";
  switch (type)
  {
    case 1:
      errorInfo += "< Non terminal grammar type error ! >";
      break;
    case 2:
      errorInfo += "< Grammar generation error ! >";
      break;
    case 3:
      errorInfo += "< Operator error ! >";
      break;  
    case 4:
      errorInfo += "< Terminator type error ! >";
      break;  
    case 5:
      errorInfo += "< Array subscript error ! >";
      break; 
    case 6:
      errorInfo += "< Assignop type mismatch ! >";
      break; 
    case 7:
      errorInfo += "< Variable not declared in symbol table ! >";
      break; 
    case 8:
      errorInfo += "< Relop type mismatch ! >";
      break;
    case 9:
      errorInfo += "< Addop type mismatch ! >";
      break;  
    case 10:
      errorInfo += "< Mulop type mismatch ! >";
      break;  
    case 11:
      errorInfo += "< Parameter does not match in symbol table ! >";
      break; 
    case 12:
      errorInfo += "< The item is redefined ! >";
      break;
    case 13:
      errorInfo += "< The id is not a procedure ! >";
      break;
    case 14:
      errorInfo += "< This id can be modify ! >";
      break;
    case 15:
      errorInfo += "< Symbol table redefinition ! >";
      break;
    case 16:
      errorInfo += "< This id is not a record ! >";
      break;
    case 17:
      errorInfo += "< This id is not a array ! >";
      break;
    case 18:
      errorInfo += "< The id is not a function ! >";
      break;
    case 19:
      errorInfo += "< read arguments can't be function ! >";
      break;
    default:
      break;
  }
  cout<<errorInfo<<endl;
  this->lpDebug << errorInfo;
}


/* 构造函数 */
SemanticAnalyzer::SemanticAnalyzer(SyntaxTree &tree){
  this -> lpDebug << "Begin to semantic analysis...";
  //cout<<"begin to analysis"<<endl;
  this->myTree = tree;
  this->myTree.root = this->myTree.nodeList.back().id;
  if(this->myTree.nodeList[this->myTree.root].type == "program_struct"){
    program_struct();
  }
  else error(1,0,this->myTree.root);
}

// programstruct -> program_head ; program_body .
void SemanticAnalyzer::program_struct(){
   cout<<" hello program_struct"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[this->myTree.root];
  if(curNode.son.size() != 4){
    error(2,0,curNode.id);
  }
  else{
    program_head(curNode.son[0]);
    if(this->myTree.nodeList[curNode.son[1]].type != ";"){
      error(1,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
    program_body(curNode.son[2]);
    if(this->myTree.nodeList[curNode.son[3]].type != "."){
      error(1,this->myTree.nodeList[curNode.son[3]].line,curNode.id);
    }
  }
}


// program_head -> program id ( idlist ) | program id
void SemanticAnalyzer::program_head(int nodeId){
     // cout<<nodeId<<" hello program_head"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 2){
    if(this->myTree.nodeList[curNode.son[0]].type != "program"
    || this->myTree.nodeList[curNode.son[1]].type != "id"){
      error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else {
      vector<Argument> paramList;
      if(!this->symbolTableController.creatTable("main","program",paramList,"",this->myTree.nodeList[curNode.son[0]].line)){
        error(15,this->myTree.nodeList[curNode.son[0]].line,nodeId);
      }
    }
  }
  else if(curNode.son.size() == 5){
    if(this->myTree.nodeList[curNode.son[0]].type != "program"
    || this->myTree.nodeList[curNode.son[1]].type != "id"
    || this->myTree.nodeList[curNode.son[2]].type != "("){
      error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else{
      vector<parameters> params = this->idList(curNode.son[3]);
      vector<Argument> paramList;
      for(auto &iter:params){
        Argument temp(iter.type,iter.name,iter.declaration);
        paramList.push_back(temp);
      }
      if(!this->symbolTableController.creatTable("main","program",paramList,"",this->myTree.nodeList[curNode.son[0]].line)){
        error(15,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
      }
      // 此处返回一个vector<参数>，参数class在符号表中定义
      // 建立符号表
    }
  }
  else{
    error(2,0,curNode.id);
  }
}

// program_body-> const_declarations var_declarations subprogram_declarations
// compound_statement
void SemanticAnalyzer::program_body(int nodeId){
   // cout<<nodeId<<" hello program_body"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(this->myTree.nodeList[curNode.son[0]].type != "const_declarations"
  ||this->myTree.nodeList[curNode.son[1]].type != "var_declarations"
  ||this->myTree.nodeList[curNode.son[2]].type != "subprogram_declarations"
  ||this->myTree.nodeList[curNode.son[3]].type != "compound_statement"){
    error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
  }
  else{
    this->const_declarations(curNode.son[0]);
    this->var_declarations(curNode.son[1]);
    this->subprogram_declarations(curNode.son[2]);
    this->compound_statement(curNode.son[3]);
  }
}

// idlist -> idlist, id | id
vector<parameters> SemanticAnalyzer::idList(int nodeId){
     // cout<<nodeId<<" hello idlist"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 1){
      if(this->myTree.nodeList[curNode.son[0]].type != "id"){
          error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
      }
      else{
          parameters temp;
          SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
          temp.name = idNode.stringValue;
          temp.type = idNode.type;
          temp.declaration = idNode.line;
          //此处对temp初始化
          params.push_back(temp);
      }
  }
  else if(curNode.son.size() == 3){
      if(this->myTree.nodeList[curNode.son[0]].type != "idlist"
      ||this->myTree.nodeList[curNode.son[1]].type != ","
      ||this->myTree.nodeList[curNode.son[2]].type != "id"){
          error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
      }
      else{
          vector<parameters> paramsTemp = this->idList(curNode.son[0]);
          for(auto &iter:paramsTemp) params.push_back(iter);
          parameters temp;
          SyntaxNode idNode = this->myTree.nodeList[curNode.son[2]];
          temp.name = idNode.stringValue;
          temp.type = idNode.type;
          temp.declaration = idNode.line;
          //此处对temp初始化
          params.push_back(temp);
      }
  }
  else error(2,0,curNode.id);
  return params;
}

// const_declarations -> const const_declaration ; | None
void SemanticAnalyzer::const_declarations(int nodeId){
     // cout<<nodeId<<" hello const_declarations"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(curNode.son.size() != 3 && curNode.son.size() != 0){
        error(2,0,curNode.id);
    }
    else if(curNode.son.size() == 3 ){
        if(this->myTree.nodeList[curNode.son[0]].type != "const"
      ||this->myTree.nodeList[curNode.son[1]].type != "const_declaration"
      ||this->myTree.nodeList[curNode.son[2]].type != ";"){
          error(1,this->myTree.nodeList[curNode.son[2]].line,curNode.id);
      }
      else{
          this->const_declaration(curNode.son[1]);
      }
    }
}


// const_declaration -> const_declaration ; id = const_value |  id = const_value
void SemanticAnalyzer::const_declaration(int nodeId){
    // cout<<nodeId<<" hello const_declaration"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(curNode.son.size() == 3){
        if(this->myTree.nodeList[curNode.son[0]].type != "id"
      ||this->myTree.nodeList[curNode.son[1]].type != "="
      ||this->myTree.nodeList[curNode.son[2]].type != "const_value"){
          error(1,this->myTree.nodeList[curNode.son[2]].line,curNode.id);
      }
      else{
          SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
          consts constInfo = this->const_value(curNode.son[2]);
          vector<pair<int,int>> periodList;
          vector<Argument> argList;
          vector<int> use;
          Argument newElement(idNode.stringValue,"const",constInfo.type,constInfo.value,idNode.line,periodList,use,argList);
          // SymbolTableElement newElement = SymbolTableElement(idNode.stringValue,"const",constInfo.type,constInfo.value,periodList,idNode.line,use,argList);
          // 符号表操作curNode.son[0]
          if(!this->symbolTableController.insert(newElement,this->symbolTableController.currentTable->tableName)){
            error(12,idNode.line,curNode.id);
          }
      }
    }
    else if(curNode.son.size() == 5){
      if(this->myTree.nodeList[curNode.son[0]].type != "const_declaration"
      ||this->myTree.nodeList[curNode.son[1]].type != ";"
      ||this->myTree.nodeList[curNode.son[2]].type != "id"
      ||this->myTree.nodeList[curNode.son[3]].type != "="
      ||this->myTree.nodeList[curNode.son[4]].type != "const_value"){
          error(1,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else{
        this->const_declaration(curNode.son[0]);
        SyntaxNode idNode = this->myTree.nodeList[curNode.son[2]];
        consts constInfo = this->const_value(curNode.son[4]);
        //符号表操作curNode.son[2]
        vector<pair<int,int>> periodList;
        vector<Argument> argList;
        vector<int> use;
        Argument newElement(idNode.stringValue,"const",constInfo.type,constInfo.value,idNode.line,periodList,use,argList);
        //SymbolTableElement newElement = SymbolTableElement(idNode.stringValue,"const",constInfo.type,constInfo.value,periodList,idNode.line,use,argList);
        if(!this->symbolTableController.insert(newElement,this->symbolTableController.currentTable->tableName)){
            error(12,idNode.line,curNode.id);
          }
      }
    }
    else error(2,0,curNode.id);
}

// const_value ->  + num  |  - num  |  num |  ′ letter ′
consts SemanticAnalyzer::const_value(int nodeId){
     // cout<<nodeId<<" hello const_value"<<endl;
  consts constInfo;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 1){
    if(this->myTree.nodeList[curNode.son[0]].type != "num"){
      error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else{
      SyntaxNode numNode = this->myTree.nodeList[curNode.son[0]];
      constInfo.type = numNode.numType;
      constInfo.value = numNode.value;
    }
  }
  else if(curNode.son.size() == 2){
    if(this->myTree.nodeList[curNode.son[1]].type != "num"){
      error(1,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
    else{
      SyntaxNode numNode = this->myTree.nodeList[curNode.son[1]];
      constInfo.type = numNode.numType;
      if(this->myTree.nodeList[curNode.son[0]].type == "+"){
        constInfo.value = numNode.value;
      }
      else if(this->myTree.nodeList[curNode.son[0]].type == "-"){
        constInfo.value = "-" + numNode.value;
      }
      else error(3,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
  }
  else if(curNode.son.size() == 3){
    if(this->myTree.nodeList[curNode.son[0]].type != "\'" 
    ||this->myTree.nodeList[curNode.son[2]].type != "\'" 
    ||this->myTree.nodeList[curNode.son[1]].type != "letter" ){
      error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else{
      SyntaxNode letterNode = this->myTree.nodeList[curNode.son[1]];
      constInfo.type = "char";
      constInfo.value = letterNode.stringValue;
    }
  }
  else error(2,0,curNode.id);
  return constInfo;
}

// var_declarations->var var_declaration ; | #
void SemanticAnalyzer::var_declarations(int nodeId){
     // cout<<nodeId<<" hello var_declarations"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3 && curNode.son.size() != 0){
    error(2,0,curNode.id);
  }
  else{
    if(curNode.son.size() == 3){
      if(this->myTree.nodeList[curNode.son[0]].type != "var"
      ||this->myTree.nodeList[curNode.son[1]].type != "var_declaration"
      ||this->myTree.nodeList[curNode.son[2]].type != ";"){
        error(1,this->myTree.nodeList[curNode.son[2]].line,curNode.id);
      }
      else{
        this->var_declaration(curNode.son[1]);
      }
    }
  }
}

// var_declaration -> var_declaration  ;  idlist  :  type |   idlist  :  type
void SemanticAnalyzer::var_declaration(int nodeId){
  // cout<<nodeId<<" hello var_declaration"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if( curNode.son.size() == 5 ){
    this->var_declaration(curNode.son[0]);
    vector<parameters> params = this->idList(curNode.son[2]);
    this->recordList = params;
    parameters paramType = this->type(curNode.son[4]);
    string temp[2];
    if(paramType.isArray){
      temp[0] = "array";
      temp[1] = paramType.info.arrayType;
    }
    else{
      if(paramType.type == "record") return;
      else{
        temp[0] = "variable";
        temp[1] = paramType.type;
      }
    }

    for(auto &iter:params){
      vector<int> use;
      vector<Argument> argList;
      Argument newElement(iter.name,temp[0],temp[1],to_string(iter.info.arraySize),iter.declaration,paramType.info.period,use,argList,paramType.info.periodDimension);
      if(!this->symbolTableController.insert(newElement,this->symbolTableController.currentTable->tableName)){
        error(12,this->myTree.nodeList[curNode.son[3]].line,curNode.id);
      }
    }
  }
  //var_declaration -> idlist : type
  else if( curNode.son.size() == 3 ){
    vector<parameters> params = this->idList(curNode.son[0]);
    this->recordList = params;
    parameters paramType = this->type(curNode.son[2]);
    string temp[2];
    if(paramType.isArray){
      temp[0] = "array";
      temp[1] = paramType.info.arrayType;
    }
    else{
      if(paramType.type == "record") return;
      else{
        temp[0] = "variable";
        temp[1] = paramType.type;
      }
    }
    for(auto &iter:params){
      vector<int> use;
      vector<Argument> argList;
      Argument newElement(iter.name,temp[0],temp[1],
                          to_string(iter.info.arraySize),
                          iter.declaration,paramType.info.period,
                          use,argList,paramType.info.periodDimension);
      if(!this->symbolTableController.insert(newElement,this->symbolTableController.currentTable->tableName)){
        error(12,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
    }
  }
  else error(2,0,curNode.id);
}

// type -> basic_type | array [ period ] of basic_type | record_type | array [ period ] of record_type 
parameters SemanticAnalyzer::type(int nodeId){
  // cout<<nodeId<<" hello type"<<endl;
  parameters paramReturn;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 1){
    if(this->myTree.nodeList[curNode.son[0]].type == "basic_type" ){
      paramReturn.type = this->basic_type(curNode.son[0]);

    }
    else if(this->myTree.nodeList[curNode.son[0]].type == "record_type"){
      paramReturn.type = "record";
      //cout<<"this is record!"<<endl;
      this->record_type(curNode.son[0]);
    }
    else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
  }
  else if(curNode.son.size() == 6){
    paramReturn.isArray = true;
    paramReturn.type = "array";
    if(( this->myTree.nodeList[curNode.son[5]].type != "basic_type"
      && this->myTree.nodeList[curNode.son[5]].type != "record_type")
      || this->myTree.nodeList[curNode.son[2]].type != "period"){
      error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else{
      vector<pair<int,int>> periodVec = this->period(curNode.son[2]);
      if(this->myTree.nodeList[curNode.son[5]].type == "basic_type"){
        paramReturn.info.arrayType = this->basic_type(curNode.son[5]);
      }
      else{
        paramReturn.info.arrayType = "record";
        this->record_type(curNode.son[5]);
      }
      int sizeArray = 0;
      for(auto &iter:periodVec){
        sizeArray += (iter.second-iter.first);
      }
      paramReturn.info.arraySize = sizeArray;
      paramReturn.info.periodDimension = periodVec.size();
      paramReturn.info.period = periodVec;
    }
  }
  else error(2,0,curNode.id);
  return paramReturn;
}

// basic_type -> integer  |  real  |  boolean  |  char
string SemanticAnalyzer::basic_type(int nodeId){
   // cout<<nodeId<<" hello basic_type"<<endl;
  SyntaxNode sonNode = this->myTree.nodeList[this->myTree.nodeList[nodeId].son[0]];
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 1) {
    error(2,0,curNode.id);
  }
  else{
      if(sonNode.type == "integer"
    || sonNode.type == "real"
    || sonNode.type == "boolean"
    || sonNode.type == "char" ){
      return sonNode.type;
    }
    else error(1,sonNode.line,curNode.id);
  }
  return "";
}

// record_type -> record field_list end
void SemanticAnalyzer::record_type(int nodeId){
  // cout<<nodeId<<" hello record_type"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3) error(2,0,curNode.id);
  else{
    for(auto &iter:this->recordList){
      vector<Argument> argList;
      if(!this->symbolTableController.creatTable(iter.name,"record",argList,"",this->myTree.nodeList[curNode.son[0]].line)){
        error(15,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
      }
    }
    cout<<"*******"<<endl;
    this->field_list(curNode.son[1]);
    this->symbolTableController.relocation();
  }
}

// field_list -> fixed_fields ; 
void SemanticAnalyzer::field_list(int nodeId){
   // cout<<nodeId<<" hello field_list"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 2){
    this->fixed_fields(curNode.son[0]);
  }
  else error(2,0,curNode.id);
}

// fixed_fields -> idlist : type | fixed_fields ; idlist : type
void SemanticAnalyzer::fixed_fields(int nodeId){
   // cout<<nodeId<<" hello fixed_fields"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  vector<parameters> parameterList;
  if(curNode.son.size() == 3){
    parameterList = this -> idList(curNode.son[0]);
    parameters type = this -> type(curNode.son[2]);
    string temp[2];
    if(type.isArray){
      temp[0] = "array";
      temp[1] = type.info.arrayType;
    } 
    else {
      temp[0] = "variable";
      temp[1] =  type.type;
    }
    for(auto table:this->recordList){
      for(auto &iter:parameterList){
        vector<Argument> argList;
        vector<int> use;
        Argument temp(iter.name,temp[0],temp[1],to_string(iter.info.arraySize),iter.declaration,type.info.period,use,argList,iter.info.period.size());
        if(!this->symbolTableController.insert(temp,table.name)){
          error(12,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
        }
      }
    }
  }
  else if(curNode.son.size() == 5){
    this->fixed_fields(curNode.son[0]);
    parameterList = this->idList(curNode.son[2]);
    parameters type = this->type(curNode.son[4]);
    string temp[2];
    if(type.isArray){
        temp[0] = "array";
        temp[1] = type.info.arrayType;
    } 
    else {
      temp[0] = "variable";
      temp[1] =  type.type;
    }
    for(auto table:this->recordList){
      for(auto &iter:parameterList){
        vector<Argument> argList;
        vector<int> use;
        Argument temp(iter.name,temp[0],temp[1],to_string(iter.info.arraySize),iter.declaration,type.info.period,use,argList,iter.info.period.size());
        if(!this->symbolTableController.insert(temp,table.name)){
          error(12,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
        }
      }
    }
  }
}

// period -> period , digits .. digits | digits .. digits
vector<pair<int,int>> SemanticAnalyzer::period(int nodeId){
     // cout<<nodeId<<" hello period"<<endl;
  vector<pair<int,int>>  periodMap;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 3){
    SyntaxNode digitNode1 = this->myTree.nodeList[curNode.son[0]];
    SyntaxNode digitNode2 = this->myTree.nodeList[curNode.son[2]];
    if( digitNode1.numType != "integer" || atof(digitNode1.value.c_str()) < 0
    || digitNode2.numType != "integer" || atof(digitNode2.value.c_str()) < 0
    || atof(digitNode2.value.c_str()) < atof(digitNode1.value.c_str()) ){
      error(5,digitNode1.line,curNode.id);
      return periodMap;
    }
    else{
      periodMap.push_back({atoi(digitNode1.value.c_str()),atoi(digitNode2.value.c_str())});
    }
  }
  else if(curNode.son.size() == 5 ){
    SyntaxNode digitNode1 = this->myTree.nodeList[curNode.son[2]];
    SyntaxNode digitNode2 = this->myTree.nodeList[curNode.son[4]];
    vector<pair<int,int>> subperiodMap = this->period(curNode.son[0]);
    for( auto &iter : subperiodMap ){
      periodMap.push_back(iter);
    }
    if( digitNode1.numType != "integer" || atoi(digitNode1.value.c_str()) < 0
    || digitNode2.numType != "integer" || atoi(digitNode2.value.c_str()) < 0
    || atoi(digitNode2.value.c_str()) < atoi(digitNode1.value.c_str()) ){
      error(5,digitNode1.line,curNode.id);
      return periodMap;
    }
    else{
      periodMap.push_back({atoi(digitNode1.value.c_str()),atoi(digitNode2.value.c_str())});
    }
  }
  else error(2,0,curNode.id);
  return periodMap;
}


// subprogram_declarations -> subprogram_declarations subprogram  ;|  #
void SemanticAnalyzer::subprogram_declarations(int nodeId){
   // cout<<nodeId<<" hello subprogram_declarations"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3 && curNode.son.size() != 0) error(2,0,curNode.id);
  else if(curNode.son.size() == 3){
    this->subprogram_declarations(curNode.son[0]);
    this->subprogram(curNode.son[1]);
  }
}

// subprogram -> subprogram_head   ;  subprogram_body
void SemanticAnalyzer::subprogram(int nodeId){
   // cout<<nodeId<<" hello subprogram"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 3){
    this->subprogram_head(curNode.son[0]);
    this->subprogram_body(curNode.son[2]);
  }
  else error(2,0,curNode.id);
}

// subprogram_head -> procedure id  formal_parameter | function id  formal_parameter  :  basic_type 
void SemanticAnalyzer::subprogram_head(int nodeId){
   // cout<<nodeId<<" hello subprogram_head"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 3){
    SyntaxNode idNode = this->myTree.nodeList[curNode.son[1]];
    string tableName = idNode.stringValue;
    vector<parameters> params = this->formal_parameter(curNode.son[2]);
    vector<Argument> paramList;
    for(auto &iter:params){
      Argument temp(iter.type,iter.name,iter.declaration,iter.isAddress);
      paramList.push_back(temp);
    }
    if(!this->symbolTableController.creatTable(tableName,"procedure",paramList,"",this->myTree.nodeList[curNode.son[0]].line)){
      error(15,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    // 创建符号表
  }
  else if(curNode.son.size() == 5){
    vector<parameters> params = this->formal_parameter(curNode.son[2]);
    string basicType = this->basic_type(curNode.son[4]);
    SyntaxNode idNode = this->myTree.nodeList[curNode.son[1]];
    string tableName = idNode.stringValue;
    //vector<parameters> params = this->formal_parameter(curNode.son[2]);
    vector<Argument> paramList;
    for(auto &iter:params){
      Argument temp(iter.type,iter.name,iter.declaration,iter.isAddress);
      //cout<<temp.name<<" "<<temp.isAddress<<endl;
      paramList.push_back(temp);
    }

    // 创建符号表
    if(!this->symbolTableController.creatTable(tableName,"function",paramList,basicType,this->myTree.nodeList[curNode.son[0]].line)){
      error(15,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
  }
  else error(2,0,curNode.id);
}

// subprogram_body -> const_declarations var_declarations compound_statement
void SemanticAnalyzer::subprogram_body(int nodeId){
     // cout<<nodeId<<" hello subprogram_body"<<endl;
    
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(nodeId == 96) cout<<curNode.son.size()<<endl;
    if(curNode.son.size() == 3){
        this->const_declarations(curNode.son[0]);
        this->var_declarations(curNode.son[1]);
        this->compound_statement(curNode.son[2]);
    }
  else error(2,0,curNode.id);
}

// formal_parameter -> (  parameter_list  ) |  #
vector<parameters> SemanticAnalyzer::formal_parameter(int nodeId){
   // cout<<nodeId<<" hello formal_parameter"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3 && curNode.son.size() != 0 ){
    error(2,0,curNode.id);
  }
  else if( curNode.son.size() == 3 ){
    params = this->parameter_list(curNode.son[1]);
  }
  return params;
}

// parameter_list -> parameter_list  ;  parameter |  parameter
vector<parameters> SemanticAnalyzer::parameter_list(int nodeId){
     // cout<<nodeId<<" hello parameter_list"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 1){
    if(this->myTree.nodeList[curNode.son[0]].type == "parameter"){
      params = this->parameter(curNode.son[0]);
    }
    else error(2,0,curNode.id);
  }
  else if(curNode.son.size() == 3){
    if(this->myTree.nodeList[curNode.son[2]].type == "parameter"
    ||this->myTree.nodeList[curNode.son[0]].type == "parameter_list"){
      vector<parameters> subParams;
      subParams = this->parameter_list(curNode.son[0]);
      for(auto &iter:subParams){
        params.push_back(iter);
        //cout<<iter.name<<" "<<iter.isAddress<<endl;
      }
      for(auto &iter:this->parameter(curNode.son[2])){
        params.push_back(iter);
        //cout<<iter.name<<" "<<iter.isAddress<<endl;
      }
    }
    else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
  }
  return params;
}

// parameter -> var_parameter  |  value_parameter
vector<parameters> SemanticAnalyzer::parameter(int nodeId){
     // cout<<nodeId<<" hello parameter"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 1) error(2,0,curNode.id);
  else{
    if(this->myTree.nodeList[curNode.son[0]].type == "var_parameter"){
      params = this->var_parameter(curNode.son[0]);
    }
    else if(this->myTree.nodeList[curNode.son[0]].type == "value_parameter"){
      params = this->value_parameter(curNode.son[0]);
    }
    else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
  }
  return params;
}

// var_parameter -> var  value_parameter
vector<parameters> SemanticAnalyzer::var_parameter(int nodeId){
     // cout<<nodeId<<" hello var_parameter"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if( curNode.son.size() != 2 ) error(2,0,curNode.id);
  else{
    params = this->value_parameter(curNode.son[1]);
    for( auto &iter : params ) {
      iter.isAddress = true;
      //cout<<iter.name<<" "<<iter.isAddress<<endl;
    }
  }
  return params;
}

// value_parameter -> idlist :  basic_type
vector<parameters> SemanticAnalyzer::value_parameter(int nodeId){
     // cout<<nodeId<<" hello value_parameter"<<endl;
  vector<parameters> params;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3) error(2,0,curNode.id);
  else{
    vector<parameters> idlist = this->idList(curNode.son[0]);
    string idType = this->basic_type(curNode.son[2]);
    for(auto &iter:idlist){
      iter.type = idType;
      params.push_back(iter);
    }
  }
  return params;
}

// compound_statement -> begin statement_list end 
void SemanticAnalyzer::compound_statement(int nodeId){
     // cout<<nodeId<<" hello compound_statement"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 3) error(2,0,curNode.id);
  else{
    if(this->myTree.nodeList[curNode.son[0]].type != "begin"
    ||this->myTree.nodeList[curNode.son[1]].type != "statement_list"
    ||this->myTree.nodeList[curNode.son[2]].type != "end"){
      error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else{
      this->statement_list(curNode.son[1]);
      SyntaxNode fatherNode = this->myTree.nodeList[curNode.father];
      if(fatherNode.type == "subprogram_body"){
        if(this->symbolTableController.currentTable->tableName != "main"){
          cout<<"the current table is "<<this->symbolTableController.currentTable->tableName<<" -> ";
          this->symbolTableController.relocation();
          cout<<this->symbolTableController.currentTable->tableName<<endl;
        }
      }
    }
  }
}

// statement_list -> statement_list  ;  statement |  statement
void SemanticAnalyzer::statement_list(int nodeId){
     // cout<<nodeId<<" hello statement_list"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(curNode.son.size() == 1){
        if(this->myTree.nodeList[curNode.son[0]].type != "statement") error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        else{
            this->statement(curNode.son[0]);
        }
    }
    else if(curNode.son.size() == 3){
        if(this->myTree.nodeList[curNode.son[0]].type != "statement_list"
        ||this->myTree.nodeList[curNode.son[1]].type != ";"
        ||this->myTree.nodeList[curNode.son[2]].type != "statement") error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        else{
            this->statement_list(curNode.son[0]);
            this->statement(curNode.son[2]);
        }
    }
    else error(2,0,curNode.id);
}

// statement -> 
// variable  assignop  expression |  
// procedure_call| 
// compound_statement|  
// if  expression  then statement  else_part |
// for  id  assignop  expression   to  expression  do  statement |
// read ( variable_list  ) | 
// write ( expression_list ) |  
// while expression do statement | #
void SemanticAnalyzer::statement(int nodeId){
    // cout<<nodeId<<" hello statement"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(curNode.son.size() == 1){
        if(this->myTree.nodeList[curNode.son[0]].type == "procedure_call"){
            this->procedure_call(curNode.son[0]);
        }
        else if(this->myTree.nodeList[curNode.son[0]].type == "compound_statement"){
            this->compound_statement(curNode.son[0]);
        }
        else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    //statement -> variable  assignop  expression 
    else if(curNode.son.size() == 3){
        parameters varParam = this->variable(curNode.son[0]);
        parameters expParam = this->expression(curNode.son[2]);
        /*在符号表中寻找varParam，若找到*/
        if(varParam.info.arrayType == "const" || varParam.type == "function") error(14,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
        if(varParam.type != expParam.type) {
          if(varParam.type != "real" || expParam.type != "integer"){
            error(6,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
          }
        }
    }
    // statement -> read ( variable_list  )
    // statement -> write ( expression_list ) 
    // statement -> while expression do statement
    else if(curNode.son.size() == 4 ){
        if(this->myTree.nodeList[curNode.son[0]].type == "read"){
            vector<parameters> varList = this->variable_list(curNode.son[2]);
            for(auto &iter:varList){
                Argument *searchElement = this->symbolTableController.find(iter.name,this->symbolTableController.currentTable->tableName);
                //在符号表中找iter，找不到报错
                if(!searchElement) error(7,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
                else if(searchElement->type == "function") error(19,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
            }
        }
        else if(this->myTree.nodeList[curNode.son[0]].type == "write"){
            vector<parameters> expList = this->expression_list(curNode.son[2]);
        }
        else if(this->myTree.nodeList[curNode.son[0]].type == "while"){
          parameters expression =  this->expression(curNode.son[1]);
          if(expression.type != "boolean") error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
          this->statement(curNode.son[3]);
        }
        else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    //statement -> if  expression  then statement  else_part
    else if(curNode.son.size() == 5 ){
        parameters expParam = this->expression(curNode.son[1]);
        if(expParam.type != "boolean") error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        else{
            this->statement(curNode.son[3]);
            this->else_part(curNode.son[4]);
        }
    }
    //statement -> for id assignop expression to expression do statement
    else if(curNode.son.size() == 8 ){
        //在符号表找id，找不到报错
        SyntaxNode idNode = this->myTree.nodeList[curNode.son[1]];
        Argument *searchElement  = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
        parameters expParam1 = this->expression(curNode.son[3]);
        parameters expParam2 = this->expression(curNode.son[5]);
        if(searchElement){
          if(expParam1.type == "integer" && expParam2.type == "integer" && searchElement->valueType == "integer" && searchElement->type != "function"){
              this->statement(curNode.son[7]);
          }
          else error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        }
        else error(7,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else if(curNode.son.size() == 0 ) return;
    else error(2,0,curNode.id);
}


// variable_list -> variable_list , variable | variable
vector<parameters> SemanticAnalyzer::variable_list(int nodeId){
     // cout<<nodeId<<" hello variable_list"<<endl;
  vector<parameters> paramVec;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() == 1){
    parameters param = this->variable(curNode.son[0]);
    paramVec.push_back(param);
  }
  else if(curNode.son.size() == 3){
    vector<parameters> subParam = this->variable_list(curNode.son[0]);
    parameters param = this->variable(curNode.son[2]);
    for(auto &iter:subParam){
      paramVec.push_back(iter);
    }
    paramVec.push_back(param);
  }
  else error(2,0,curNode.id);
  return paramVec;
}

// variable -> id  id_varpart | id . variable
parameters SemanticAnalyzer::variable(int nodeId){
    //bool isRecord = false;
    // cout<<nodeId<<" hello variable"<<endl;
    parameters variableRet;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
    //cout<<idNode.id<<" "<<idNode.stringValue<<endl;
    if(curNode.son.size() == 2){
      /*在符号表中找到id对应的内容*/
      Argument *searchElement  = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
      this->id_varpart(curNode.son[1]);
      if(searchElement){
          if(searchElement->type != "array" && this->myTree.nodeList[curNode.son[1]].son.size() == 3){
            error(17,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
          }
          if(searchElement->type == "procedure" || searchElement->type == "record" ){
            error(14,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
          }
          else if( searchElement->type == "function" ){
            if(this->symbolTableController.currentTable->tableName == searchElement->name || searchElement->argList.size() == 0){
              variableRet.info.arrayType = "function";
              variableRet.info.arraySize = searchElement->argList.size();
              variableRet.isArray = false;
              variableRet.type = searchElement->valueType;
              variableRet.name = searchElement->name;
              variableRet.declaration = this->myTree.nodeList[curNode.son[0]].line;
            }
            else error(14,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
          }
          else if(searchElement->type == "array"){
            if(this->myTree.nodeList[curNode.son[1]].son.size() == 3){
              variableRet.isArray = true;
              variableRet.type = searchElement->valueType;
              variableRet.name = searchElement->name;
              variableRet.declaration = this->myTree.nodeList[curNode.son[0]].line;
            }
            else error(5,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
          }
          else if(searchElement->type == "const"){
              variableRet.info.arrayType = "const";
              variableRet.isArray = false;
              variableRet.type = searchElement->valueType;//id的type
              variableRet.name = searchElement->name;//对应id的name
              variableRet.declaration = this->myTree.nodeList[curNode.son[0]].line;
          }
          else{
              variableRet.isArray = false;
              variableRet.type = searchElement->valueType;//id的type
              variableRet.name = searchElement->name;//对应id的name
              variableRet.declaration = this->myTree.nodeList[curNode.son[0]].line;
          }
      }
      else if(idNode.value == "true" || idNode.value == "false"){
          variableRet.isArray = false;
          variableRet.type = "boolean";
          variableRet.name = idNode.value;
          variableRet.declaration = this->myTree.nodeList[curNode.son[0]].line;
      }
      else error(7,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    // variable -> id . variable
    else if(curNode.son.size() == 3){
      Argument *searchElement  = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
      if(searchElement){
        if(searchElement->type != "record") error(16,this->myTree.nodeList[curNode.son[1]].line,nodeId);
        else{
          searchElement->use.push_back(this->myTree.nodeList[curNode.son[1]].line);
          this->symbolTableController.currentTable = this->symbolTableController.locate(searchElement->name);
          variableRet = this->variable(curNode.son[2]);
          this->symbolTableController.relocation();
        }
      } 
      else error(7,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else error(2,0,curNode.id);
    return variableRet;
}

// id_varpart -> [ expression_list ]   |   #
void SemanticAnalyzer::id_varpart(int nodeId){
     // cout<<nodeId<<" hello id_varpart"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    if(curNode.son.size() == 3){
        int cnt = 0;
        vector<parameters> expVec = this->expression_list(curNode.son[1]);
        SyntaxNode ID = this->myTree.nodeList[this->myTree.nodeList[curNode.father].son[0]];
        /*判断数组下标是否超出范围*/
        Argument *searchElement = this->symbolTableController.find(ID.stringValue, this->symbolTableController.currentTable->tableName);
        if(expVec.size() != searchElement->periodList.size()){
          error(5,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        }
        else{
          for(auto &iter:expVec){
            cnt++;
            if(iter.type != "integer"){
                error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
            }
            else{
                int dimension = 0;
                for( auto period:searchElement->periodList) {
                    dimension++;
                    if(dimension == cnt){
                        if(iter.info.periodDimension == -1 && ( iter.info.arraySize < period.first || iter.info.arraySize > period.second )){
                          error(5,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
                        }
                    }
                }
            }
          }
        }
    }
    else if(curNode.son.size() != 0 ) error(2,0,curNode.id);
}

// procedure_call -> id  |   id ( expression_list )
void SemanticAnalyzer::procedure_call(int nodeId){
     // cout<<nodeId<<" hello procedure_call"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId]; 
  /*符号表中查找id*/
    /*查找不到，则id未定义，查找到了查该procedure或function有参数还是无参数*/
  if(curNode.son.size() == 1){
    SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
    Argument *searchElement = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
    if(searchElement){
      searchElement->use.push_back(idNode.line);
      if( searchElement->type == "procedure"){
        if(searchElement->argList.size()){
          error(11,idNode.line,curNode.id);
        }
      }
      else{
        error(13,idNode.line,curNode.id);
      }
    }
    else error(7,idNode.line,curNode.id);
  }
  else if(curNode.son.size() == 4){
    /*符号表中查找id*/
    /*查找不到，则id未定义，查找到了查该procedure或function有参数还是无参数*/
    /*查找到了，判断每个id类型*/
    /*paramVec中的每个id的type一一对比*/
    SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
    Argument *searchElement = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
    if(searchElement){
      searchElement->use.push_back(idNode.line);
      if(searchElement->type == "procedure"){
        if(searchElement->argList.size() == 0) error(13,0,curNode.id);
        else{
          vector<parameters> expressionList = this->expression_list(curNode.son[2]);
          vector<string> typeStringList;
          vector<string> nameStringList;
          for(auto expression:expressionList){
            typeStringList.push_back(expression.type);
            nameStringList.push_back(expression.name);
          }
          if(this->symbolTableController.checkArg(idNode.stringValue,nameStringList,typeStringList)){
            //string returnType = this->symbolTableController.tableLists[idNode.stringValue].returnType;
            string returnType = this->symbolTableController.nameTableMap[idNode.stringValue].returnType;
          }
          else error(11,idNode.line,curNode.id);
        }
      }
      else error(13,idNode.line,curNode.id);
    }
    else error(7,idNode.line,curNode.id);
  }
  else error(2,0,curNode.id);
}

// else_part -> else statement   |   #
void SemanticAnalyzer::else_part(int nodeId){
     // cout<<nodeId<<" hello else_part"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  if(curNode.son.size() != 2 &&curNode.son.size() != 0) error(2,0,curNode.id);
  else if(curNode.son.size() == 2) this->statement(curNode.son[1]);
}

// expression_list -> expression_list  ,  expression   |   expression
vector<parameters> SemanticAnalyzer::expression_list(int nodeId){
    // cout<<nodeId<<" hello expression_list"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  vector<parameters> expressionList;
  if(curNode.son.size() == 1){
    parameters expression = this->expression(curNode.son[0]);
    expressionList.push_back(expression);
  }
  else if(curNode.son.size() == 3){
    vector<parameters> subexpList = this->expression_list(curNode.son[0]);
    parameters expression = this->expression(curNode.son[2]);
    for(auto &iter:subexpList){
      expressionList.push_back(iter);
    }
    expressionList.push_back(expression);
  }
  else error(2,0,curNode.id);
  return expressionList;
}

// expression -> simple_expression  relop  simple_expression | simple_expression
parameters SemanticAnalyzer::expression(int nodeId){
  // cout<<nodeId<<" hello expression"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  parameters expression;
  if(curNode.son.size() == 1){
    expression = this->simple_expression(curNode.son[0]);
  }
  else if(curNode.son.size() == 3){
    parameters sipExp1 = this->simple_expression(curNode.son[0]);
    parameters sipExp2 = this->simple_expression(curNode.son[2]);
    if(sipExp1.type == sipExp2.type){
      if(sipExp1.type == "char" || sipExp1.type == "integer" || sipExp2.type == "real"){
        expression.name = "Expression";
        expression.type = "boolean";
      }
      else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
    else error(8,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
  }
  return expression;
}

// simple_expression -> simple_expression  addop term |  term
parameters SemanticAnalyzer::simple_expression(int nodeId){
  // cout<<nodeId<<" hello simple_expression"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  parameters expression;
  if(curNode.son.size() == 1){
    expression = this->term(curNode.son[0]);
  }
  else if(curNode.son.size() == 3){
    parameters subsimExp = this->simple_expression(curNode.son[0]);
    parameters subTerm = this->term(curNode.son[2]);
    if(subsimExp.type == subTerm.type){
      if(this->myTree.nodeList[curNode.son[1]].stringValue == "or"){
        if(subsimExp.type == "integer" || subsimExp.type == "boolean"){
          expression.name = "Expression";
          expression.type = subsimExp.type;
          expression.declaration = subsimExp.declaration;
        }
        else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else if(this->myTree.nodeList[curNode.son[1]].stringValue == "+"
      ||this->myTree.nodeList[curNode.son[1]].stringValue == "-"){
        if(subsimExp.type != "boolean" && subsimExp.type != "char"){
          expression.name = "Expression";
          expression.type = subsimExp.type;
          expression.declaration = subsimExp.declaration;
        }
        else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else error(9,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
    else{
      if(this->myTree.nodeList[curNode.son[1]].stringValue == "or") error(9,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      else if( (subsimExp.type == "integer" && subTerm.type == "real")
      || (subTerm.type == "integer" && subsimExp.type == "real") ){
          expression.name = "Expression";
          expression.type = "real";
          expression.declaration = subsimExp.declaration;
      }
      else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
  }
  else error(2,0,curNode.id);
  return expression;
}

// term -> term  mulop  factor   |   factor
parameters SemanticAnalyzer::term(int nodeId){
  // cout<<nodeId<<" hello term"<<endl;
  SyntaxNode curNode = this->myTree.nodeList[nodeId];
  parameters termParam;
  if(curNode.son.size() == 1){
    termParam = this->factor(curNode.son[0]);
  }
  else if(curNode.son.size() == 3){
    parameters subTerm = this->term(curNode.son[0]);
    parameters subFac = this->factor(curNode.son[2]);
    if(subTerm.type == subFac.type){
      if(subTerm.type == "boolean"){
        if(this->myTree.nodeList[curNode.son[1]].stringValue != "and") error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
        else{
          termParam.name = "Expression";
          termParam.type = "boolean";
          termParam.declaration = subTerm.declaration;
        }
      }
      else if(subTerm.type == "integer"){
        if(this->myTree.nodeList[curNode.son[1]].stringValue == "and"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "*"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "div"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "mod"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "/"){
          termParam.name = "Expression";
          termParam.type = "integer";
          termParam.declaration = subTerm.declaration;
        }
        else error(10,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else if(subTerm.type == "real"){
        if(this->myTree.nodeList[curNode.son[1]].stringValue == "*"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "/"){
          termParam.name = "Expression";
          termParam.type = "real";
          termParam.declaration = subTerm.declaration;
        }
        else error(10,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
    else{
      if(subTerm.type =="integer" && subFac.type =="real"){
        if(this->myTree.nodeList[curNode.son[1]].stringValue == "*"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "/"){
          termParam.name = "Expression";
          termParam.type = "real";
          termParam.declaration = subTerm.declaration;
        }
        else error(10,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else if(subTerm.type =="real" && subFac.type =="integer"){
        if(this->myTree.nodeList[curNode.son[1]].stringValue == "*"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "mod"
        ||this->myTree.nodeList[curNode.son[1]].stringValue == "/"){
          termParam.name = "Expression";
          termParam.type = "real";
          termParam.declaration = subTerm.declaration;
        }
        else error(10,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
      }
      else error(4,this->myTree.nodeList[curNode.son[1]].line,curNode.id);
    }
  }
  else error(2,0,curNode.id);
  return termParam;
}

// factor -> num |  variable |  id  ( expression_list )| 
//( expression ) |  not  factor | uminus  factor | 'letter'
parameters SemanticAnalyzer::factor(int nodeId){
     // cout<<nodeId<<" hello factor"<<endl;
    SyntaxNode curNode = this->myTree.nodeList[nodeId];
    parameters facParam;
    if(curNode.son.size() == 1){
        if(this->myTree.nodeList[curNode.son[0]].type == "num"){
            facParam.name = "Expression";
            facParam.type = this->myTree.nodeList[curNode.son[0]].numType;
            facParam.declaration = this->myTree.nodeList[curNode.son[0]].line;
            facParam.info.arraySize = atoi(this->myTree.nodeList[curNode.son[0]].value.c_str());
            facParam.info.periodDimension = -1;
        }
        else if(this->myTree.nodeList[curNode.son[0]].type == "variable"){
            SyntaxNode sonNode = this->myTree.nodeList[curNode.son[0]];
            SyntaxNode sonsonNode = this->myTree.nodeList[sonNode.son[0]];
            parameters varParam = this->variable(curNode.son[0]);
            if(varParam.isArray){
                facParam.isArray = true;
                facParam.name = varParam.name;
                facParam.type = varParam.type;
                facParam.declaration = varParam.declaration;
            }
            else if(varParam.info.arrayType == "function" && varParam.info.arraySize != 0){
              error(11,sonsonNode.line,curNode.id);
            }
            else {
                facParam.name = varParam.name;
                facParam.type = varParam.type;
                facParam.declaration = varParam.declaration;
            }
        }
        else error(1,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else if(curNode.son.size() == 2){
        if(this->myTree.nodeList[curNode.son[0]].type == "not"){
            parameters subFac = this->factor(curNode.son[1]);
            if(subFac.type == "boolean" || subFac.type == "integer"){
                facParam.type == subFac.type;
                facParam.name = "Expression";
                facParam.declaration = subFac.declaration;
            }
            else error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        }
        else if(this->myTree.nodeList[curNode.son[0]].type == "-"){
            parameters subFac = this->factor(curNode.son[1]);
            if(subFac.type == "real" || subFac.type == "integer"){
                facParam.type = subFac.type;
                facParam.name = "Expression";
                facParam.declaration = subFac.declaration;
            }
            else error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
        }  
        else error(4,this->myTree.nodeList[curNode.son[0]].line,curNode.id);
    }
    else if(curNode.son.size() == 3){
      if(this->myTree.nodeList[curNode.son[1]].type == "letter"){
        SyntaxNode sonNode = this->myTree.nodeList[curNode.son[0]];
        facParam.type = "char";
        facParam.name = "Expression";
        facParam.declaration = sonNode.line;
      }
      else{
        parameters expParam = this->expression(curNode.son[1]);
        facParam.type = expParam.type;
        facParam.name = "Expression";
        facParam.declaration = expParam.declaration;
      }
    }
    //factor -> id  ( expression_list )
    else if(curNode.son.size() == 4){
        SyntaxNode idNode = this->myTree.nodeList[curNode.son[0]];
        Argument *searchElement = this->symbolTableController.find(idNode.stringValue,this->symbolTableController.currentTable->tableName);
        if(searchElement){      //若找到
            if( searchElement->type == "function" ){
                vector<parameters> expList = this->expression_list(curNode.son[2]);
                vector<string> typeStringList;
                vector<string> nameStringList;
                for(auto expression:expList){
                    typeStringList.push_back(expression.type);
                    nameStringList.push_back(expression.name);
                }
                if(this->symbolTableController.checkArg(idNode.stringValue,nameStringList,typeStringList)){
                //若参数表和expList中每个type相同
                    facParam.type = this->symbolTableController.nameTableMap[idNode.stringValue].returnType;//id函数的返回值
                    facParam.name = "Expression";
                    facParam.declaration = idNode.line;
                }
                else error(11,idNode.line,curNode.id);
            }
            else error(18,idNode.line,curNode.id);
        }
        else error(7,idNode.line,curNode.id);
    }
    else error(2,0,curNode.id);
    return facParam;
}
