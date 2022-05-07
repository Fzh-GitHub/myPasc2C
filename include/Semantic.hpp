#ifndef _SEMANTIC_HPP_
#define _SEMANTIC_HPP_

#include <iostream>
#include <windows.h>
#include <cstdio>
#include <unordered_map>

#include "SyntaxTree.hpp"
#include "SymbolTable.hpp"

// 数组信息
class arrayInfo{
  public:
    int arraySize;                   // 数组大小
    string arrayType;                // 数组元素类型
    vector<pair<int,int>> period;    // 数组上下维集
    int periodDimension;             // 数组维数
};

// 参数信息
class parameters{
  public:
    parameters(){                   // 构造函数
      this->isArray = false; 
      this->isAddress = false;
    };
    string name;                    // 参数名 
    string type;                    // 参数类型
    bool isArray;                   // 是否为数组
    bool isAddress;                 // 是否为传引用
    arrayInfo info;                 // 作为数组的信息或其他信息
    int declaration;                // 声明行
};

// 常量信息
class consts{
  public:
    string value;                   // 常量值 
    string type;                    // 常量类型
};

// 语义分析控制台
class SemanticAnalyzer{
  private:
    SyntaxTree myTree;                            // 语法树
    LPDebug lpDebug;                              // 调试信息
    bool isSemanticRight = true;                  // 语义分析结果
    vector<parameters> recordList;                // 自定义结构集
  public:
    SymbolTableController symbolTableController;  // 符号表与类型检查的接口控制台
    SemanticAnalyzer(SyntaxTree &tree);           // 构造函数
    void program_struct();
    void program_head(int );
    void program_body(int );
    vector<parameters> idList(int );
    void const_declarations(int);
    void const_declaration(int);
    consts const_value(int);
    void var_declarations(int);
    void var_declaration(int);
    parameters type(int);
    void record_type(int );
    void field_list(int );
    void fixed_fields(int );
    string basic_type(int);
    vector<pair<int,int>>  period(int);
    void subprogram_declarations(int);
    void subprogram(int);
    void subprogram_head(int);
    vector<parameters> formal_parameter(int);
    vector<parameters> parameter_list(int);
    vector<parameters> parameter(int);
    vector<parameters> var_parameter(int);
    vector<parameters> value_parameter(int);
    void subprogram_body(int);
    void compound_statement(int);
    void statement_list(int);
    void statement(int);
    vector<parameters> variable_list(int);
    parameters variable(int);
    void id_varpart(int);
    void procedure_call(int);
    void else_part(int);
    vector<parameters> expression_list(int);
    parameters expression(int);
    parameters simple_expression(int);
    parameters term(int);
    parameters factor(int);
    void error(int,int,int);
    ~SemanticAnalyzer(){
      this->lpDebug<<"Finish semantic analysis.";
      if(!this->isSemanticRight) this->lpDebug<<"Semantic error.";
      else this->lpDebug<<"Semantic right.";
    }
};

#endif