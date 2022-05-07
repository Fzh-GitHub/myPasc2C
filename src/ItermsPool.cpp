#include "ItermsPool.hpp"

ItermsNode::ItermsNode(const int &index,Iterms &iterms)
{
    this->index=index;
    this->iterms=iterms;
}

ItermsPool::ItermsPool(const int &SIndex,Iterms &StartIterms)
{
    StartIterms.closure();
    this->que.push(pair<int,Iterms>(SIndex,StartIterms));
}

ItermsPool::~ItermsPool()
{

}

int ItermsPool::findIterms(Iterms &iterms)
{
    for(int i=0;i<this->pool.size();i++)
    {
        if(this->pool[i].iterms==iterms)
        {
            return i;
        }
    }
    queue<pair<int,Iterms>> temp=this->que;
    while(!temp.empty())
    {
        pair<int,Iterms> head=temp.front();
        temp.pop();
        if(head.second==iterms)
        {
            return head.first;
        }
    }
    return -1;
}

void ItermsPool::getDFA(SymbolPool *tpool,SymbolPool *npool)
{
    int k=0;
    int cnt=1;
    while(!que.empty())
    {
        Iterms head=que.front().second;
        ItermsNode curNode=ItermsNode(k,head);
        // print DFA
        // cout<<endl<<"I"<<k<<":"<<endl;
        // for(int i=0;i<head.size();i++)
        // {
        //     Iterm tempIterm=head.Its[i];
        //     cout<<npool->getString(tempIterm.left)<<"->";
        //     for(int j=0;j<tempIterm.right.size();j++)
        //     {
        //         if(j==tempIterm.pos)
        //             cout<<"• ";
        //         if(tempIterm.right[j].type==Symbol::TSymbol)
        //             cout<<tpool->getString(tempIterm.right[j].index)<<" ";
        //         else
        //             cout<<npool->getString(tempIterm.right[j].index)<<" ";
        //     }
        //     if(tempIterm.pos==tempIterm.right.size())
        //         cout<<"•";

        //     if(tempIterm.forward==-1)
        //         cout<<"     $"<<endl;
        //     else
        //         cout<<"     "<<tpool->getString(tempIterm.forward)<<endl;
        // }
        // NSymbol
        Iterms nextIterms=Iterms(head.PPool);
        for(int j=0;j<npool->size();j++)
        {
            nextIterms.Its.clear();
            if(head.go(Symbol(j,Symbol::NSymbol),nextIterms))
            {
                nextIterms.closure();
                int index=this->findIterms(nextIterms);
                if(index<0)
                {
                    curNode.mapper.insert(pair<int,Symbol>(cnt,Symbol(j,Symbol::NSymbol)));
                    this->que.push(pair<int,Iterms>(cnt,nextIterms));
                    cnt++;
                }
                else
                {
                    curNode.mapper.insert(pair<int,Symbol>(index,Symbol(j,Symbol::NSymbol)));
                }
            }    
        }
        // TSymbol
        for(int j=0;j<tpool->size();j++)
        {
            nextIterms.Its.clear();
            if(head.go(Symbol(j,Symbol::TSymbol),nextIterms))
            {
                nextIterms.closure();
                int index=this->findIterms(nextIterms);
                if(index<0)
                {
                    curNode.mapper.insert(pair<int,Symbol>(cnt,Symbol(j,Symbol::TSymbol)));
                    this->que.push(pair<int,Iterms>(cnt,nextIterms));
                    cnt++;
                }
                else
                {
                    curNode.mapper.insert(pair<int,Symbol>(index,Symbol(j,Symbol::TSymbol)));
                }
            }
        }
        this->pool.push_back(curNode);
        k++;
        que.pop();
    }
}