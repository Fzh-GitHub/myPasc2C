#ifndef _PARSESTACK_HPP_
#define _PARSESTACK_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <stack>
#include "LPDebug.hpp"
#include "SymbolPool.hpp"
#include "ProductionPool.hpp"
#include "LR1ParseTable.hpp"
#include "Token.hpp"
#include "Symbol.hpp"
#include "SyntaxTree.hpp"
using namespace std;

class ParseStack
{
    private:
        int NodeCnt=0;
        LPDebug lpDebug;
        LR1ParseTable* parseTable;
        SymbolPool* TPool;
        SymbolPool* NPool;
        ProductionPool* PPool;
        queue<Token> input;
        stack<Symbol> symbolStack;
        stack<int> stateStack;
        SyntaxNodeStack syntaxNodeStack;
        SyntaxTree ast;
    public:
        ParseStack(const string &path,LR1ParseTable* parseTable,SymbolPool *TPool,SymbolPool *NPool,ProductionPool *PPool);
        ~ParseStack();
        bool end();
        bool parseByStep();
        bool parseAll();
        SyntaxTree getSyntaxTree();
        virtual void showSyntaxTree(const string &path);
        virtual void showSyntaxTree();
};

#endif