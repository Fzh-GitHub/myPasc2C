#include "Iterms.hpp"
#include <iostream>
Iterm::Iterm(const int &left,const vector<Symbol> &right, const int &prodIndex, const int &pos, const int &forward)
{
    this->left=left;
    this->right=right;
    this->pos=pos;
    this->forward=forward;
    this->prodIndex=prodIndex;
}

bool Iterm::operator<(const Iterm &other) const
{
    if(this->pos!=other.pos||this->left!=other.left||this->right.size()!=other.right.size()
        ||this->pos!=other.pos||this->right!=other.right)
    {
        return true;
    }
    return false;
}

bool Iterm::operator==(const Iterm &other) const
{
    if(this->pos!=other.pos||this->left!=other.left||this->forward!=other.forward
        ||this->right!=other.right)
    {
        return false;
    }
    return true;
}

Iterms::Iterms()
{

}

Iterms::Iterms(ProductionPool *PPool)
{
    this->PPool=PPool;
}

Iterms::Iterms(Iterm &iterm,ProductionPool *PPool)
{
    this->PPool=PPool;
    this->Its.push_back(iterm);
}

Iterms::Iterms(Production &prod,ProductionPool *PPool,const int &forward)
{
    for(int i=0;i<prod.RightsSize();i++)
    {
        Iterm It=Iterm(prod.getLeft(),prod.getRight(i).second,prod.getRight(i).first,0,forward);
        this->Its.push_back(It);
    }
    this->PPool=PPool;
}

int Iterms::size()
{
    return this->Its.size();
}

void Iterms::closure()
{
    int cnt=0;
    int size=this->Its.size();
    set<int> tempForwards;
    while(cnt<size)
    {
        Iterm curIterm=this->Its[cnt];
        tempForwards.clear();
        if(curIterm.pos<curIterm.right.size()&&curIterm.right[curIterm.pos].type==Symbol::NSymbol)
        {
            Symbol curSymbol=curIterm.right[curIterm.pos];
            int next=curIterm.pos+1;
            // find Forwards of curSymbol
            // if nextSymbol exist && is NSymbol
            while(next<curIterm.right.size()&&curIterm.right[next].type==Symbol::NSymbol)
            {
                Symbol nextSymbol=curIterm.right[next];
                set<int> nextSymbolFirstSet=this->PPool->getFirst(nextSymbol.index);
                tempForwards.insert(nextSymbolFirstSet.begin(),nextSymbolFirstSet.end());
                // if nextSymbol's First set do not contain epsilon
                if(!this->PPool->hasEpsilonFirsts[nextSymbol.index])
                {
                    break;
                }
                // else continue
                next++;
            }
            // if nextSymbol exist && is TSymbol
            if(next<curIterm.right.size()&&curIterm.right[next].type==Symbol::TSymbol)
            {
                tempForwards.insert(curIterm.right[next].index);
            }
            else if(next==curIterm.right.size())
            {
                tempForwards.insert(curIterm.forward);
            }
            // find all productions of curSymbol
            Production curProd=this->PPool->find(curSymbol.index);
            // create iterm and insert them into iterms
            for(int i=0;i<curProd.RightsSize();i++)
            {
                for(auto forward:tempForwards)
                {
                    Iterm newIterm=Iterm(curSymbol.index,curProd.getRight(i).second,curProd.getRight(i).first,0,forward);
                    if(!this->itermExisted(newIterm))
                    {
                        this->Its.push_back(newIterm);
                        size++;
                    }
                }
            }
        }
        cnt++;
    }
}

void Iterms::showIterms()
{
    for(auto It:this->Its)
    {
        printf("%d->",It.left);
        for(int i=0;i<It.right.size();i++)
        {
            if(i==It.pos)
            {
                printf(".");
            }
            printf("%d ",It.right[i].index);
        }
        printf("\t");
        printf("%d",It.forward);
        printf("\n");
    }
}

Iterms::~Iterms()
{
    
}

int Iterms::go(const Symbol nextSymbol,Iterms &nextIterms)
{
    if(this->Its.size()==0)
    {
        return 0;
    }
    int flag=0;
    // cout<<"iterm size "<<this->Its.size()<<endl;
    for(auto iterm:this->Its)
    {
        // cout<<"go "<<iterm.left<<endl;
        if(iterm.pos<iterm.right.size()&&iterm.right[iterm.pos]==nextSymbol)
        {
            // cout<<"go "<<iterm.left<<endl;
            Iterm nextIterm=Iterm(iterm.left,iterm.right,iterm.prodIndex,iterm.pos+1,iterm.forward);
            nextIterms.Its.push_back(nextIterm);
            flag=1;
        }
    }
    return flag;
}

bool Iterms::itermExisted(const Iterm &iterm)
{
    for(auto each:this->Its)
    {
        if(each==iterm)
        {
            return true;
        }
    }
    return false;
}

bool Iterms::operator==(const Iterms &iterms) const
{
    if(this->Its.size()!=iterms.Its.size())
    {
        return false;
    }
    for(int i=0;i<this->Its.size();i++)
    {
        if(!(this->Its[i]==iterms.Its[i]))
        {
            return false;
        }
    }
    return true;
}