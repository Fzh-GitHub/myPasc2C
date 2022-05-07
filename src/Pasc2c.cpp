#include <string.h>
#include <iostream>
#include "Lexer.hpp"
#include "GrammarPool.hpp"
#include "ParseStack.hpp"
#include "cmdline.h"
#include "Semantic.hpp"

int main(int argc, char *argv[])
{
	cmdline::parser cmdParser;
	cmdParser.add<string>("source code",'c',"pascal source file",true);
	cmdParser.add<string>("grammar",'g',"grammar YAML file path",true);
	cmdParser.add<string>("load lr1-table",'l',"load previously built lr1-table YAML file",false);
    cmdParser.add<string>("build-path",'p',"lr1-table YAML file path",false,"./LR1-Parse-Table.yml");
	cmdParser.add<string>("show lr1-table",'s',"show lr1-table in a *.csv file",false);
    cmdParser.add<string>("tokens",'t',"tokens file from lexical analysis",false,"./Tokens.txt");
    cmdParser.add<string>("output",'o',"output AST file path",false,"");
	cmdParser.parse_check(argc,argv);
	string tpath=cmdParser.get<string>("tokens");
	// Lex-parse
	Lexer lexer;
	if(cmdParser.exist("source code"))
	{
		string spath=cmdParser.get<string>("source code");
		lexer.readToBuffer(spath);
		lexer.transWordToToken(tpath);
	}
	// Syntax-parse
	GrammarPool grammarPool;
	string gpath=cmdParser.get<string>("grammar");
	grammarPool.loadGrammar(gpath);
	if(cmdParser.exist("load lr1-table"))
	{
		string lpath=cmdParser.get<string>("load lr1-table");
		grammarPool.loadLR1ParseTable(lpath);
	}
	else
	{
		grammarPool.buildLR1ParseTable();
        if(cmdParser.exist("build-path"))
        {
            grammarPool.outLR1ParseTable(cmdParser.get<string>("build-path"));
        }
	}
	if(cmdParser.exist("show lr1-table"))
	{
		string spath=cmdParser.get<string>("show lr1-table");
		grammarPool.showLR1ParseTable(spath);
	}
	ParseStack parseStack(tpath,grammarPool.getLR1ParseTable(),
		grammarPool.getTPool(),grammarPool.getNPool(),grammarPool.getProdPool());
	if(!parseStack.parseAll())
	{
		return 0;
	}
	if(cmdParser.exist("output")&&!cmdParser.get<string>("output").empty())
	{
		parseStack.showSyntaxTree(cmdParser.get<string>("output"));
	}
	else
	{
		parseStack.showSyntaxTree();
	}
	// get Syntax Tree
	SyntaxTree ast=parseStack.getSyntaxTree();
	// semantic analysis
	// ...
	SemanticAnalyzer semanticAnalyzer(ast);
	cout<<"finish analysis"<<endl;
	semanticAnalyzer.symbolTableController.showTable();
	return 0;
}