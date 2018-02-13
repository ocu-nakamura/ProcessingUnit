#pragma once
#include "unistd.h"
#include "tools.hpp"
void calc(std::string prefixName,std::string processName,std::map<std::string,DataSet> dataMap,DataSet &result){
        result.dataName=prefixName;
        result.dataValue="This is Data of"+prefixName;
        result.isSuccess=true;
}
