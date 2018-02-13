#pragma once
#include "unistd.h"
#include "tools.hpp"
void functionA(const std::string &prefixName,const std::string &processName,const std::map<std::string,DataSet> &dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void functionB(const std::string &prefixName,const std::string &processName,const std::map<std::string,DataSet> &dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
}
void functionC(const std::string &prefixName,const std::string &processName,const std::map<std::string,DataSet> &dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
void functionD(const std::string &prefixName,const std::string &processName,const std::map<std::string,DataSet> &dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
}
void functionZ(const std::string &prefixName,const std::string &processName,const std::map<std::string,DataSet> &dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
