#pragma once
#include <thread>
#include <ndn-cxx/face.hpp>
#include <mutex>
#include "tools.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace API {

class DataCollector : noncopyable
{
public:
//コンストラクタ(Timestamp 指定なし)
DataCollector(std::string input_name){
        prefix = input_name;
}
DataCollector(std::string input_name, long input_timeout_limit){
        prefix = input_name;
        timeout_limit = input_timeout_limit;
}
void
run(DataSet &result)
{
        interest.setName(Name(this->prefix));
        if(timeout_limit != 0) {
                interest.setInterestLifetime(time::milliseconds(timeout_limit));
        } else{
                interest.setInterestLifetime(time::milliseconds(5000));
        }
        interest.setMustBeFresh(true);

        m_face.expressInterest(interest,
                               bind(&DataCollector::onData, this,  _1, _2),
                               bind(&DataCollector::onNack, this, _1, _2),
                               bind(&DataCollector::onTimeout, this, _1));

        // processEvents will block until the requested data received or timeout occurs
        m_face.processEvents();
        result.dataValue = value;
        result.isSuccess = exitCode;
}

private:
// Data 受信時に value へ代入
void
onData(const Interest& interest, const Data& data)
{
        value = (char *)data.getContent().value();
        value.erase(data.getContent().size() - 1);
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"Data Received"<<setD<<"] "<<interest.getName()<<value<<'\n';
        exitCode = true;
}

void
onTimeout(const Interest& interest)
{
        #ifdef DEBUG
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"Timeout"<<setD<<"] "<<interest<<'\n';
        #endif
        exitCode = false;
}

void
onNack(const Interest& interest, const lp::Nack& nack)
{
        #ifdef DEBUG
        std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"Received Nack With Reason" << nack.getReason()<<setD<<"] "<<interest<<'\n';
        #endif
        exitCode=false;
}
private:
Interest interest;
Face m_face;
std::string prefix;
std::string value = "";
long timeout_limit = 0;
// Data を要求中は true を返す
bool exitCode;
};

} // namespace examples
} // namespace ndn
