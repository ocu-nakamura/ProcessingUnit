#pragma once
#include "ndn-cxx/face.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include <regex>
#include <thread>
#include "unistd.h"
#include "threadFunction.hpp"
#include "tools.hpp"


// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace API {

class ProcessingUnit : noncopyable
{
private:
void sendData(){
        if(dataForNotify.isSuccess ==false) {
                INFO lout("Send Nack(Not Implemented)",dataForNotify.dataName);
                //sendNack
                //nack は送信できていない

                /*
                   Name name(dataForNotify.dataName);
                   //name.appendTimestamp();
                   Interest nackInterest(name);
                   lp::Nack nack(nackInterest);
                   lp::NackReason nackReason = lp::NackReason::NONE;
                   nack.setReason(nackReason);
                   m_face.put(nack);
                 */

                std::string content ="NACK";
                ndn::Name dataName(dataForNotify.dataName);
                // Create Data packet
                shared_ptr<ndn::Data> data = make_shared<ndn::Data>();
                data->setName(dataName);
                data->setFreshnessPeriod(time::seconds(10));
                data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

                // Sign Data packet with default identity
                m_keyChain.sign(*data);
                // m_keyChain.sign(data, <identityName>);
                // m_keyChain.sign(data, <certificate>);

                // Return Data packet to the requester
                m_face.put(*data);
                return;
        }
        INFO lout("Send Data",dataForNotify.dataName);
        //data の生成
        std::string content =dataForNotify.dataValue;
        ndn::Name dataName(dataForNotify.dataName);
        // Create Data packet
        shared_ptr<ndn::Data> data = make_shared<ndn::Data>();
        data->setName(dataName);
        data->setFreshnessPeriod(time::seconds(10));
        data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

        // Sign Data packet with default identity
        m_keyChain.sign(*data);
        // m_keyChain.sign(data, <identityName>);
        // m_keyChain.sign(data, <certificate>);

        // Return Data packet to the requester
        m_face.put(*data);
        return;
}

public:
void onSendDataRequest(){
        while(1) {
                {
                        std::unique_lock<std::mutex> uniq_lk(mtx);
                        cv.wait(uniq_lk, [] { return isReady;});
                }
                {
                        //std::lock_guard<std::mutex> lock(mtx);
                        sendData();
                        isReady = false;
                }
        }
}
ProcessingUnit(std::string networkName,std::string serviceName){
        m_networkName = networkName;
        m_serviceName = m_networkName+serviceName;
        std::thread sendDataThread(&ProcessingUnit::onSendDataRequest,this);
        sendDataThread.detach();
}
void
run()
{
        INFO lout("Run Processing Unit",m_serviceName);
        m_face.setInterestFilter(m_serviceName, bind(&ProcessingUnit::onInterest, this, _1, _2),    RegisterPrefixSuccessCallback(),    bind(&ProcessingUnit::onRegisterFailed, this, _1, _2));
        m_face.processEvents();

}

private:
void
onInterest(const InterestFilter& filter, const Interest& interest)
{
        //受信したプレフィクス
        std::string inputPrefix = interest.getName().toUri();
        //終了コールか確認
        if(inputPrefix == m_serviceName+"/stop") {
                INFO lout("Stop Service Request",m_serviceName);
                //終了要求に対するsendData
                std::string content = "EXIT";
                ndn::Name dataName(inputPrefix);
                shared_ptr<ndn::Data> data = make_shared<ndn::Data>();
                data->setName(dataName);
                data->setFreshnessPeriod(time::seconds(10));
                data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());

                // Sign Data packet with default identity
                m_keyChain.sign(*data);
                // m_keyChain.sign(data, <identityName>);
                // m_keyChain.sign(data, <certificate>);

                // Return Data packet to the requester
                m_face.put(*data);
                m_face.shutdown();
                return;
        }
        // ここがプロセスのスレッドスタート地点
        processThreadPool.emplace_back(startServiceProcess,inputPrefix);
        auto itr = processThreadPool.end()-1;
        itr->detach();
        processThreadPool.erase(itr);
}

void
onRegisterFailed(const Name& prefix, const std::string& reason)
{
    #ifdef INFO
        {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"Failed To Register Prefix"<<setD<<"] "<<reason<<'\n';
                m_face.shutdown();
        }
    #endif
}

private:
std::vector<std::thread> processThreadPool;
Face m_face;
KeyChain m_keyChain;
std::string m_networkName;
std::string m_serviceName;

};

} // namespace API
} // namespace ndn
