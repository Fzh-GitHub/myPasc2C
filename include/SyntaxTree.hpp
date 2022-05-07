#ifndef _SYNTAXTREE_HPP_
#define _SYNTAXTREE_HPP_
#include "Symbol.hpp"
#include <fstream>
#include <vector>
using namespace std;

class SyntaxNode
{
    public:
        int id;             // 当前节点id
        int father;         // 父节点id
        int line;           // 行号
        int col;            // 列号
        int isTerminal;     // 是否为终结符
        string value;       // 携带值
        string stringValue; // only TSymbol，终结符名
        string type;        // 文法类型
        vector<int> son;    // 子节点集
        string numType;     // TSymbol
        SyntaxNode(const int &id,const int &line,const int &col,const string &value,const string &type,const string &numType);// NSymbol
        SyntaxNode(const int &id,const string &value,const string &type);
        string to_string();
};

typedef vector<SyntaxNode> SyntaxNodeStack;
class SyntaxTree
{
    public:
        vector<SyntaxNode> nodeList;                // 节点集
        int nodeNum;                                // 总结点数
        int root;                                   // 根节点id
        SyntaxTree(){nodeList.clear();nodeNum=0;};  // 构造函数
        ~SyntaxTree(){};                            // 析构函数
        void insertNode(const SyntaxNode &node);    // 插入节点操作
        virtual void showTree(const string &path);  // 展示语法树
        virtual void showTree();                    // 输出语法树
    private:
        void dfsTravsersal(SyntaxNode &node,string &res,const int level);
                                                    // 遍历语法树
};
#endif