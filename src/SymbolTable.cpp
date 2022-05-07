#include"SymbolTable.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;
/* 对符号表的初始化 */
void SymbolTableController::init(vector<Argument> argList)
{
	auto p = this->creatTable("main","program",argList,"",0);
	this->currentTable = p;
}

/* 建表 */
SymbolTable* SymbolTableController::creatTable(string tabName,
												string tableType,
												vector<Argument> argList,
												string returnType,
												int delaration){
	/* 查找表是否重定义 */
	auto iter = nameTableMap.find(tabName);
	if(tabName != "main") {
		auto iiter = this->currentTable->elementList.find(tabName);
		if(iiter != this->currentTable->elementList.end()){
			return NULL;
		}
	} 
	
	if(iter == nameTableMap.end()){
		map<string,Argument> elementList;
		string subtableType = tableType;
		SymbolTable s(tabName,this->currentTable,tableType,argList,elementList,returnType);
		if(tableType == "record") s.parentNode = NULL;
		this->temp = &s;
		this->nameTableMap.insert(make_pair(tabName,s));
		this->tableList.push_back(s);
		/* 表参数作为表项插入 */
		if(argList.size() > 0){
			string addrOrVal;
			for(auto iter : argList){
				if(iter.isAddress){
					addrOrVal = "address";
				}
				else addrOrVal = "value";
				vector<pair<int,int>> periodLists;
        		vector<Argument> newArgList;
        		vector<int> newUse;
				Argument newElement(iter.name,addrOrVal,iter.type,"",iter.row,periodLists,newUse,newArgList);
				this->insert(newElement,tabName);
			}
		}
		/*炼狱修改操作*/
		vector<pair<int,int>> periodLists;
		vector<int> newUse;
		Argument newElement(tabName,subtableType,returnType,"",delaration,periodLists,newUse,argList,argList.size());
		if(tabName != "main") {
			this->insert(newElement,"main");
			this->insert(newElement,tabName);
		}
		this->currentTable = locate(tabName);
		return temp;

	}
	else return NULL;
}

/* 在某一符号表中寻找表项 */
Argument* SymbolTableController::find(string name,string tabName){
    cout<<"<find var "<<name<<" in table "<<tabName<<">"<<endl;
	auto p = &(this->nameTableMap[tabName]);
	Argument* res = nullptr;
	/* 按照链域查询 */
	while (p->elementList.count(name)!= 0 || p->parentNode != nullptr)
	{
		if (p->elementList.count(name) > 0)
		{
			res =&( p->elementList[name]);
			break;
		}
		p = p->parentNode;
	}
	return res;
}

/* 在某一符号表中插入表项 */
bool SymbolTableController::insert(Argument arg,string tabName)
{
	SymbolTable *table = &(this->nameTableMap[tabName]);
    cout<<arg.name<<" insert into ->"<<tabName<<endl;
	if (!table){
		//throw("table does not exsit");
		return false;
	}
	// try{
	// 	cout << arg.name << " insert into ->" << tabName << endl;
	// }
	// catch (std::exception& e){
	// 	throw(e);
	// }
	return table->insert(arg);
}

/* 判断是否可以进行插入 */
bool SymbolTable::insert(Argument arg)
{
	if (this->elementList.count(arg.name) > 0)
	{
		//throw("defined variable");
		return false;
	}
	else
	{
		elementList[arg.name] = arg;
	}
	return true;
}

/* 符号表重定位 */
void SymbolTableController::relocation(){
	/* 对于record符号表进行特判 */
	string parentTable;
	if( !this->currentTable->parentNode ){
		parentTable = "main";
	}
	else parentTable = this->currentTable->parentNode->tableName;
	string outstream = "Relocated table from ";
	outstream += this->currentTable->tableName + " to " + parentTable;
	lpDebug<<outstream;
	if(this->currentTable->tableType == "record" ) this->currentTable = locate("main");
	else if (this->currentTable){
		this->currentTable = this->currentTable->parentNode;
	}
}

/* 符号表定位 */
SymbolTable* SymbolTableController::locate(string name){
	string outstream = "Located to table ";
	outstream += name;
	this->lpDebug<<outstream;
	return &nameTableMap[name];
}

/* 函数/过程参数匹配 */
bool SymbolTableController::checkArg(string fpName,vector<string> nameList,vector<string> type){
	/* 不仅匹配类型，同时匹配传值/传地址 */
	bool flag = true;
	if(nameTableMap.find(fpName) != nameTableMap.end()){
		if(nameTableMap[fpName].argList.size() == type.size()){			
			if(nameTableMap[fpName].argString == type) {
				for(int i=0;i<int(nameList.size());i++){
					if(nameTableMap[fpName].addressVec[i] && nameList[i] == "Expression"){
						return false;
					}
				}
				return true;
			}
			
			else return false;
		}
		else return false;
	}
	else {
		//throw("table does not exsit");
		return false;
	}
}

/* 展示符号表 */
void SymbolTableController::showTable()
{
	fstream file;
	vector<string> text;
	text.push_back("Name");
	text.push_back("Etype");
	text.push_back("Vtype");
	text.push_back("Declaration");
	text.push_back("Period");
	file.open("debug.txt",ios::app);
    for(auto iter:this->nameTableMap){
		file<<endl;
        file<<iter.second.tableName<<":"<<endl;
		for(auto iter:text){
			file<<setw(15)<<left<<iter;
		}
		file<<endl;
        for(auto it:iter.second.elementList){
            file<<setw(15)<<left<<it.second.name;
			file<<setw(15)<<left<<it.second.type;
			file<<setw(15)<<left<<it.second.valueType;
			file<<setw(15)<<left<<it.second.row;
			if(it.second.periodList.size() > 0){
				for(auto period:it.second.periodList){
					file<<setw(15)<<left<<period.first<<" "<<period.second<<" ";
				}
			}
			file<<endl;
        }
    }
	file.close();
}