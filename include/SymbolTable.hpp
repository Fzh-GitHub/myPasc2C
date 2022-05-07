#ifndef _SYMBOLTABLE_HPP_
#define _SYMBOLTABLE_HPP_

#pragma once
#include<map>
#include<string>
#include<vector>
#include "LPDebug.hpp"
using namespace std;
class Argument		
{
public:
	string type;       //作为参数，就是参数的type（integer，real..），作为表项，则为表项的类型（function,program,value,address...）
	string name;       //作为参数和表项都会用到
	string value;                     // 作为表项会用到
	string retType;                   // 作为表项会用到
	string valueType;                 // 作为表项会用到
	vector<int> use;                  // 作为表项会用到
	vector<Argument> argList;         // 作为表项会用到
	vector<pair<int,int>> periodList; // 作为表项会用到
	bool isAddress;                   // 作为参数用到
	int row;                          // 作为参数和表项都会用到
	int dimension;                    // 作为表项会用到，数组维数或函数/过程参数个数
	Argument()                        // 无参构造函数
	{
		this->name = "";
	}
	/*作为参数的构造函数*/
	Argument(string _type, string _name, int _row, bool _isAddress = false) 
	{
		this->retType = "";
		this->row = _row;
		this->name = _name;
		this->type = _type;
		this->isAddress = _isAddress;
	}
	/*作为表项的构造函数*/
	Argument(string _name,string _type,
			string _valueType,string _value,
			int _declaration,vector<pair<int,int>> _periodList,
			vector<int> _use,vector<Argument> _argList,
			int _dimension = 0){
				this->name = _name;
				this->type = _type;
				this->valueType = _valueType;
				this->value = _value;
				this->row = _declaration;
				this->periodList = _periodList;
				this->use = _use;
				this->argList = _argList;
				this->dimension = _dimension;
			}
};

class SymbolTable
{
public:
	string tableName;                          // 表名
	string tableType;                          // 表类型
	string returnType;                         // 作为函数的返回值 
	vector<string> argString;                  // 用于参数匹配
	vector<bool> addressVec;                   // 用于参数匹配 
	SymbolTable* parentNode;                   // 父节点
	map<string,Argument> argList;              // 参数集
	map<string,Argument> elementList;          // 表项集
	SymbolTable(){}                            // 构造函数
	SymbolTable(string _name, 
				SymbolTable* _par,
				string _tableType, 
				vector<Argument> _argList,
				map<string,Argument> _elementList,
				string _returnType){
		vector<string> _argTypes;
		if(_argList.size()){
			for (auto i = _argList.begin(); i != _argList.end(); i++)
			{
				addressVec.push_back(i->isAddress);
				_argTypes.push_back(i->type);
				this->argList[i->name] = *i;
			}
		}
		
		this->parentNode = _par;
		this->tableName = _name;
		this->argString = _argTypes;
		this->tableType = _tableType;
		this->returnType = _returnType;
		this->elementList = _elementList;
	} 
	bool insert(Argument arg);
};


class SymbolTableController
{
public:
	SymbolTable* currentTable = NULL;            // 当前表指针
	SymbolTable* temp = NULL;                    // 作为构造表的临时表
	vector<SymbolTable> tableList;               // 总表集
	map<string,SymbolTable> nameTableMap;        // 表名和表的hash对应
	LPDebug lpDebug;                             // 调试信息

	/* 符号表定位 */
	SymbolTable* locate(string name);
	/* 符号表建表 */           
	SymbolTable* creatTable(string tabName,string tableType,vector<Argument> arglist,string returnType,int delaration);
	/* 符号表中查找表项 */
	Argument* find(string name,string tabName);
	/* 符号表中插入表项 */
	bool insert(Argument arg,string tabName);
	/* 检查参数匹配 */
	bool checkArg(string,vector<string>,vector<string>);
	/* 符号表重定位 */
	void relocation();
	/* 符号表初始化 */ 
	void init(vector<Argument> arglist);
	/* 符号表展示 */
    void showTable();
};

#endif


