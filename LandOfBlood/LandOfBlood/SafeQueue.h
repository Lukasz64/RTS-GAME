#pragma once
#include <iostream>
#include <mutex>
#include <queue>

template<class T>
class SafeQueue{
    private:
        std::queue<T>  que;
        std::mutex     mut;
    public:

        SafeQueue(){
        }
        void push(T val){
                mut.lock();     
                que.push(val);
                mut.unlock();
        }

        T pop(){
                mut.lock();     
                T res = que.front();
                que.pop();
                mut.unlock();
                return res;
        }

        bool isEmpty(){
                mut.lock(); 
                bool q = que.empty();
                mut.unlock();
                return q;
        }
};
