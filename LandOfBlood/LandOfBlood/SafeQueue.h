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
        int maxSize = -1;
        volatile int size = 0;

        SafeQueue(){

        }
        void push(T val){
                mut.lock();                   
                if(maxSize != -1 && size>=maxSize) {
                        mut.unlock();
                        while (size>=maxSize);
                        mut.lock();                    
                }
                size++; 
                que.push(val);
                mut.unlock();
        }

        T pop(){
                mut.lock();     
                T res = que.front();
                que.pop();
                size--;
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



template<class T>
class SafeEleemnt{
    private:
        T  elem;
        bool isEmpty = true;
        std::mutex     mut,lc;
        

    public:

        SafeEleemnt(){

        }
        void push(T val){
                while (!isEmpty);               

                mut.lock();                   
                elem = val;
                isEmpty = false;
                mut.unlock();
        }

        T pop(){              
                mut.lock();     
                T res = elem;
                isEmpty = true;
                mut.unlock();
                return res;
        }

        bool empty(){
                mut.lock(); 
                bool q = isEmpty;
                mut.unlock();
                return q;
        }
};
