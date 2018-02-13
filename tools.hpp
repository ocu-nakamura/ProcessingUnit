#pragma once
// correct way to include ndn-cxx headers
#include <thread>
#include <ndn-cxx/face.hpp>
#include <mutex>
#include <string>
//#include "face.hpp"

#define setY "\x1b[33m"
#define setD "\x1b[39m"
#define setB "\x1b[34m"
#define setG "\x1b[32m"
#define setR "\x1b[31m"

#define DEBUG
#define _INFO
#define NETWORKNAME "/ocu"

std::mutex mtx;
//ロック中に呼んだ関数の中でロックするとデッドロックするので要注意
class DataSet {
public:
bool isSuccess;
std::string dataName;
std::string dataValue;
};

enum FUNCTIONLISTSTATUS {
        NON_WORKING,
        WORKING
};
#ifdef _DEBUG
#define dump(x) std::cerr<<#x<<"="<<x<<std::endl
#else
#define dump(x) 1 ?  (void) 0 :
#endif

#ifdef _INFO
#define INFO
#else
#define INFO 1 ? (void)0 :
#endif

// #ifdef _DEBUG
// #define DEBUG
// #else
// #define DEBUG 1 ? (void)0 :
// #endif

void lout(std::string y,std::string d,std::string g){
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<y<<setD<<"] "<<d<<"{"<<setG<<g<<setD<<"}"<<'\n';
}
void lout(std::string y,std::string d){
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<y<<setD<<"] "<<d<<'\n';
}
void lout(std::string y,std::string d,bool result){
        std::string g;
        if (result == true) {
                g = "true";
        }
        else{
                g="false";
        }
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<y<<setD<<"] "<<d<<"{"<<setG<<g<<setD<<"}"<<'\n';
}
void lout(std::string y,std::string d,int g){
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<y<<setD<<"] "<<d<<"{"<<setG<<g <<setD<<"}"<<'\n';
}
