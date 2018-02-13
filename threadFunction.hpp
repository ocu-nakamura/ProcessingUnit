#pragma once
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include "unistd.h"
#include "function.hpp"
#include "parser.hpp"
#include "dataCollector.hpp"
#include "tools.hpp"
#include <mutex>
#include <future>

//DataSet をもっと一般化する.→map<string,tuple<bool,string>>に変更すること

std::condition_variable cv;
DataSet dataForNotify;
bool isReady = false;

void collectArgData(const std::string &prefix,std::map<std::string,DataSet> &dataMap){
        INFO lout("Data Collecting Process",prefix);
        ndn::API::DataCollector dataCollector(prefix);
        //もしかしたら Lock の必要があるかも
        DataSet result;
        dataCollector.run(result);
        //dataCollector が取得成功
        if(result.isSuccess == true) {
                std::lock_guard<std::mutex> lock(mtx);
                dataMap[prefix] = result;
        }
}


void startServiceProcess(std::string prefix){
        INFO lout("Start Service Process",prefix);
        DataSet result;
        std::string parentPrefix;
        std::vector<std::string> childPrefix;
        PrefixParser parser;

        if (parser.startParse(prefix,parentPrefix,childPrefix) ==false) {
                //sendNack
                {
                        std::lock_guard<std::mutex> lock(mtx);
                        dataForNotify.isSuccess=false;
                        dataForNotify.dataName = prefix;
                        dataForNotify.dataValue ="";
                        isReady = true;
                }
                cv.notify_one();
                return;
        }
        std::vector<std::thread> collectArgDataPool;
        std::map<std::string,DataSet> dataMap;
        for(int i = 0; i<(int)childPrefix.size(); i++) {
                collectArgDataPool.emplace_back(collectArgData,std::ref(childPrefix[i]),std::ref(dataMap));
        }
        std::for_each(collectArgDataPool.begin(), collectArgDataPool.end(), [](std::thread& t) {
                t.join();
        });
        //データが全て揃っているか確認

        if(dataMap.size()!=childPrefix.size()) {
                //sendNack
                {
                        dataForNotify.dataName = prefix;
                        dataForNotify.dataValue="";
                        dataForNotify.isSuccess=false;
                        isReady = true;
                }
                cv.notify_one();
                return;
        }
        //計算開始
        INFO lout("Start Service",prefix);

        functionZ(prefix,parentPrefix,dataMap,result);
        //計算終了
        {
                //sendData
                std::lock_guard<std::mutex> lock(mtx);
                dataForNotify = result;
                isReady = true;
        }
        cv.notify_one();

}
