#ifndef _ITERMS_HPP_
#define _ITERMS_HPP_
#include <vector>
#include "Symbol.hpp"
#include "ProductionPool.hpp"

using namespace std;

class Iterm
{
    public:
        int left;
        int prodIndex;
        vector<Symbol> right;
        int pos;
        int forward;
        Iterm(const int &left, const vector<Symbol> &right, const int &ProdIndex, const int &pos, const int &forward);
        bool operator<(const Iterm &other) const;
        bool operator==(const Iterm &other) const;
};

class Iterms
{
    public:
        ProductionPool *PPool;
        vector<Iterm> Its;
        Iterms();
        Iterms(ProductionPool *PPool);
        Iterms(Iterm &iterm,ProductionPool *PPool);
        Iterms(Production &sp, ProductionPool *PPool,const int &forward);
        ~Iterms();
        int go(const Symbol nextSymbol,Iterms &nextIterms);
        void closure();
        void showIterms();
        int size();
        bool itermExisted(const Iterm &iterm);
        bool operator==(const Iterms &iterms) const;
};

#endif