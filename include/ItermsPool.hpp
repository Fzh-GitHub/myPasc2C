#ifndef _ITERMSPOOL_HPP_
#define _ITERMSPOOL_HPP_
#include "Iterms.hpp"
#include "SymbolPool.hpp"
#include <map>
#include <queue>

class ItermsNode
{
    public:
        int index;
        Iterms iterms;
        map<int,Symbol> mapper;
        ItermsNode(const int &index,Iterms &iterms);
};

class ItermsPool
{
    public:
        vector<ItermsNode> pool;
        queue<pair<int,Iterms>> que;
        ItermsPool(const int &SIndex,Iterms &StartIterms);
        ~ItermsPool();
        int findIterms(Iterms &iterms);
        void getDFA(SymbolPool *spool,SymbolPool *npool);
};

#endif