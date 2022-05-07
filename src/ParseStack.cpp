#include "ParseStack.hpp"
#include <vector>
#include <iostream>
#include <exception>
class SyntaxException
{
    private:
        Token etoken;
    public:
        SyntaxException(const Token &token)
        {
            etoken=token;
        }
        string what()
        {
            if(etoken.isEnd)
            {
                return "SyntaxException:*** the program misses its end.";
            }
            return "SyntaxException:*** ["+
                to_string(etoken.line)+","+to_string(etoken.col)+"]<value: "+
                etoken.value+"   type: "+etoken.type+">";
        }
};


static void split(const string &str,vector<string> &tokens,const string &delimiters=" ")
{
    string::size_type lastpos=str.find_first_not_of(delimiters,0);
    string::size_type pos=str.find_first_of(delimiters,lastpos);
    while(string::npos!=lastpos||string::npos!=pos)
    {
        tokens.push_back(str.substr(lastpos,pos-lastpos));
        lastpos=str.find_first_not_of(delimiters,pos);
        pos=str.find_first_of(delimiters,lastpos);
    }
}

ParseStack::ParseStack(const string &path,LR1ParseTable* parseTable,SymbolPool *TPool,SymbolPool *NPool,ProductionPool *PPool)
{
    this->syntaxNodeStack.clear();
    this->TPool=TPool;
    this->NPool=NPool;
    this->PPool=PPool;
    this->parseTable=parseTable;
    ifstream in(path,ios::in);
    stringstream tmp;
    tmp<<in.rdbuf();
    string content(tmp.str());
    in.close();
    // split content
    vector<string> tokens;
    split(content,tokens," \n");
    // read tokens
    for(int i=0;i<tokens.size();i=i+5)
    {
        this->input.push(Token(atoi(tokens[i].c_str()),atoi(tokens[i+1].c_str()),tokens[i+2],tokens[i+3],tokens[i+4]));
    }
    input.push(Token());
    this->lpDebug<<"Tokens have been read.";
    this->stateStack.push(0);
    this->symbolStack.push(Symbol());
}

ParseStack::~ParseStack()
{

}

SyntaxTree ParseStack::getSyntaxTree()
{
    return this->ast;
}


bool ParseStack::end()
{
    return this->input.empty();
}


bool ParseStack::parseByStep()
{
    if(this->end())
    {
        return true;
    }
    Token curToken=this->input.front();
    int index;
    int curState;
    Action a;
    try
    {
        if(curToken.isEnd)
        {
            index=this->TPool->size();
        }
        else
        {
            index=this->TPool->find(curToken.type);
        }
        curState=this->stateStack.top();
        a=this->parseTable->getAction(curState,index);
        if(a.type==Action::ACC)
        {
            this->ast.nodeList.push_back(this->syntaxNodeStack.back());
            syntaxNodeStack.pop_back();
            input.pop();
            return true;
        }
        else if(a.type==Action::Shift)
        {
            Symbol curSymbol(index,Symbol::TSymbol);
            this->symbolStack.push(curSymbol);
            this->stateStack.push(a.value);
            this->syntaxNodeStack.push_back(SyntaxNode(this->NodeCnt++,curToken.line,curToken.col,curToken.value,curToken.type,curToken.numType));
            this->ast.insertNode(this->syntaxNodeStack.back());
            input.pop();
        }
        else if(a.type==Action::Reduce)
        {
            SingleProduction sprod=this->PPool->getSingleProd(a.value);
            Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
            SyntaxNode parent=SyntaxNode(this->NodeCnt++,this->NPool->getString(leftSymbol.index),this->NPool->getString(leftSymbol.index));
            // judge symbols and states in stacks
            // ...
            while(!sprod.right.empty())
            {
                Symbol topSymbol=this->symbolStack.top();
                int topState=this->stateStack.top();
                if(!(sprod.right.back()==topSymbol))
                {
                    // throw a exception seems better
                    throw SyntaxException(curToken);
                }
                auto back=this->syntaxNodeStack.back();
                back.father=parent.id;
                // parent.line=back.line;
                // parent.col=back.col;
                this->ast.nodeList[back.id].father=parent.id;
                parent.son.insert(parent.son.begin(),this->syntaxNodeStack.back().id);
                sprod.right.pop_back();
                this->stateStack.pop();
                this->symbolStack.pop();
                // syntaxNodeStack is empty ?
                this->syntaxNodeStack.pop_back();
            }
            curState=this->stateStack.top();
            Goto g=this->parseTable->getGoto(curState,sprod.left);
            if(g.nextState==-1)
            {
                // throw a exception seems better
                throw SyntaxException(curToken);
            }
            this->stateStack.push(g.nextState);
            this->symbolStack.push(leftSymbol);
            this->syntaxNodeStack.push_back(parent);
            this->ast.insertNode(parent);
        }
        else
        {
            throw SyntaxException(curToken);
        }
    }
    catch(SymbolNotFoundException& e)
    {
        this->lpDebug<<e.what();
        this->lpDebug<<"\t*** value:"+curToken.value+"\ttype:"+curToken.type;
        return false;
    }
    catch(SyntaxException &e)
    {
        this->lpDebug<<e.what();
        return false;
    }
    return true;
}

bool ParseStack::parseAll()
{
    while(!this->end())
    {
        Token curToken=this->input.front();
        int index;
        int curState;
        Action a;
        try
        {
            if(curToken.isEnd)
            {
                index=this->TPool->size();
            }
            else
            {
                index=this->TPool->find(curToken.type);
            }
            curState=this->stateStack.top();
            a=this->parseTable->getAction(curState,index);
            if(a.type==Action::ACC)
            {
                // SyntaxTree root
                this->ast.nodeList.push_back(syntaxNodeStack.back());
                syntaxNodeStack.pop_back();
                input.pop();
                this->lpDebug<<"Syntax right.";
                return true;
            }
            else if(a.type==Action::Shift)
            {
                Symbol curSymbol(index,Symbol::TSymbol);
                this->symbolStack.push(curSymbol);
                this->stateStack.push(a.value);
                this->syntaxNodeStack.push_back(SyntaxNode(this->NodeCnt++,curToken.line,curToken.col,curToken.value,curToken.type,curToken.numType));
                this->ast.insertNode(this->syntaxNodeStack.back());
                input.pop();
            }
            else if(a.type==Action::Reduce)
            {
                SingleProduction sprod=this->PPool->getSingleProd(a.value);
                Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
                SyntaxNode parent=SyntaxNode(this->NodeCnt++,this->NPool->getString(leftSymbol.index),this->NPool->getString(leftSymbol.index));
                // judge symbols and states in stacks
                // ...
                while(!sprod.right.empty())
                {
                    Symbol topSymbol=this->symbolStack.top();
                    int topState=this->stateStack.top();
                    if(!(sprod.right.back()==topSymbol))
                    {
                        // throw a exception seems better
                        throw SyntaxException(curToken);
                    }
                    auto back=this->syntaxNodeStack.back();
                    back.father=parent.id;
                    // parent.line=back.line;
                    // parent.col=back.col;
                    this->ast.nodeList[back.id].father=parent.id;
                    parent.son.insert(parent.son.begin(),back.id);
                    sprod.right.pop_back();
                    this->stateStack.pop();
                    this->symbolStack.pop();
                    // syntaxNodeStack is empty ?
                    this->syntaxNodeStack.pop_back();
                }
                curState=this->stateStack.top();
                Goto g=this->parseTable->getGoto(curState,sprod.left);
                if(g.nextState==-1)
                {
                    // throw a exception seems better
                    // ..
                    // return false;
                    throw SyntaxException(curToken);
                }
                this->stateStack.push(g.nextState);
                this->symbolStack.push(leftSymbol);
                this->syntaxNodeStack.push_back(parent);
                this->ast.insertNode(parent);
            }
            else
            {
                throw SyntaxException(curToken);
            }
        }
        catch(SymbolNotFoundException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"\t*** value:"+curToken.value+"\ttype:"+curToken.type;
            return false;
        }
        catch(SyntaxException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"Syntax Error.";
            return false;
        }
    }
    if(!this->symbolStack.empty()||!this->stateStack.empty())
    {
        this->lpDebug<<"Syntax Error.";
        return false;
    }
    return true;
}

void ParseStack::showSyntaxTree()
{
    this->ast.showTree();
}

void ParseStack::showSyntaxTree(const string &path)
{
    this->ast.showTree(path);
}