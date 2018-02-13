// correct way to include ndn-cxx headers
// #include <ndn-cxx/face.hpp>
// #include <ndn-cxx/security/key-chain.hpp>

#include "face.hpp"
#include "security/key-chain.hpp"
#include "tools.hpp"
#include <thread>
#include <regex>
#include <stdlib.h>

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace API {

class ServiceOrganizer : noncopyable
{
public:
ServiceOrganizer();
void run();
private:
void setDefaultService();
int systemCall(std::string inputCommand);
void sendData(Interest interest,std::string value);
void startService(Interest interest,std::string &networkName, std::string serviceName);
void stopService(Interest interest,std::string faceFilterName,std::string serviceName);
void getServiceList(Interest interest);
void onRegisterFailed(const Name &prefix, const std::string& reason);
void onInterest(const InterestFilter &filter, const Interest &interest);
void convPrefixToMember(std::string &strInterest);
private:
Face m_face;
int m_serviceCost;
std::string m_requestedServiceResult;
std::string m_requestedServiceName;
std::string m_requestedServiceParameter;
std::string m_strServiceList;
KeyChain m_keyChain;
std::string m_networkName;
std::string m_nodeName;
std::string m_faceFilterName;
std::string content;
std::map<std::string,FUNCTIONLISTSTATUS> serviceList;
std::string strInterest;
};
void ServiceOrganizer::setDefaultService(){
        serviceList["/service"+m_nodeName] = NON_WORKING;
        serviceList["/service01"] = NON_WORKING;
        serviceList["/service02"] = NON_WORKING;
        serviceList["/service03"] = NON_WORKING;
        serviceList["/service04"] = NON_WORKING;
        serviceList["/service05"] = NON_WORKING;
        serviceList["/service06"] = NON_WORKING;
        serviceList["/service07"] = NON_WORKING;
        serviceList["/service08"] = NON_WORKING;
        serviceList["/service09"] = NON_WORKING;
        serviceList["/service10"] = NON_WORKING;
        serviceList["/service11"] = NON_WORKING;
        serviceList["/service12"] = NON_WORKING;
        serviceList["/service13"] = NON_WORKING;
        serviceList["/service14"] = NON_WORKING;
        serviceList["/service15"] = NON_WORKING;
        serviceList["/service16"] = NON_WORKING;
        serviceList["/service17"] = NON_WORKING;
        serviceList["/service18"] = NON_WORKING;
        serviceList["/service19"] = NON_WORKING;
        serviceList["/service20"] = NON_WORKING;
}
void ServiceOrganizer::sendData(Interest interest,std::string value){
        std::string content = value;
        Name dataName(interest.getName());
        shared_ptr<Data> data = make_shared<Data>();
        data->setName(dataName);
        data->setFreshnessPeriod(time::seconds(10));
        data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
        // Sign Data packet with default identity
        m_keyChain.sign(*data);
        // m_keyChain.sign(data, <identityName>);
        // m_keyChain.sign(data, <certificate>);

        // Return Data packet to the requester
        std::cout << ">> D: " << *data << std::endl;
        m_face.put(*data);
}

ServiceOrganizer::ServiceOrganizer(){
        setDefaultService();
        m_nodeName += "/";
        m_nodeName += std::getenv("HOSTNAME");
        m_networkName = NETWORKNAME;
        m_faceFilterName = m_networkName+m_nodeName;
        INFO lout("NLSR advertise ",m_faceFilterName,systemCall("nlsrc advertise "+m_faceFilterName));
}

int ServiceOrganizer::systemCall(std::string inputCommand){
        return system(inputCommand.c_str());
}

void ServiceOrganizer::startService(Interest interest,std::string &networkName, std::string serviceName){
        //service is not installed
        if(serviceList.count(serviceName) == 0) {
                INFO lout("Service Not Installed",interest.getName().toUri());
                sendData(interest,"NOT_INSTALLED");
        }
        //case サービス正常スタート
        else if(serviceList[serviceName] == NON_WORKING) {
                serviceList[serviceName] = WORKING;
                INFO lout("Success To Start Service",interest.getName().toUri());
                INFO lout("NLSR advertise",m_networkName+serviceName,systemCall("nlsrc advertise "+m_networkName+serviceName));
                sendData(interest,"SUCCESS");
                ndn::API::ProcessingUnit service(networkName,serviceName);
                service.run();
                INFO lout("Service is Terminated",interest.getName().toUri());
                //reset service status & withdraw nlsrc
                INFO lout("NLSR withdraw",m_networkName+serviceName,systemCall("nlsrc withdraw "+m_networkName+serviceName));
                serviceList[serviceName] = NON_WORKING;
        }
        else if(serviceList[serviceName] == WORKING) {
                INFO lout("Service Already Running",interest.getName().toUri());
                sendData(interest,"ALREADY_RUNNING");
        }
        else{
                INFO lout("Unknown Service Request",interest.getName().toUri());
                sendData(interest,"START_SERVICE_UNKNOWN_ISSUE");
        }
}
void ServiceOrganizer::stopService(Interest interest,std::string faceFilterName,std::string serviceName){
        if(serviceList.count(serviceName) == 0) {
                sendData(interest,"NOT_INSTALLED");
        }
        else if(serviceList[serviceName] == NON_WORKING) {
                sendData(interest,"ALREADY_DOWN");
        }
        else if(serviceList[serviceName] == WORKING) {
                ndn::API::DataCollector stopServiceRequester(m_networkName+serviceName+"/stop");
                DataSet stopServiceRequestData;
                stopServiceRequester.run(stopServiceRequestData);
                sendData(interest,stopServiceRequestData.dataValue);
        }
        else{
                sendData(interest,"UNKNOWN_ISSUE");
        }
}

void ServiceOrganizer::getServiceList(Interest interest){
        std::string result;
        for(auto itr = serviceList.begin(); itr !=serviceList.end(); itr++) {
                if(itr->second == WORKING) {
                        result += itr->first + ":WORKING\n";
                }
                else if(itr->second == NON_WORKING) {
                        result += itr->first + ":NON_WORKING\n";
                }
        }
        sendData(interest,result);
}

void ServiceOrganizer::run()
{
        INFO lout("Run Service Creator",m_faceFilterName);
        m_face.setInterestFilter(m_faceFilterName,
                                 bind(&ServiceOrganizer::onInterest, this, _1, _2),
                                 RegisterPrefixSuccessCallback(),
                                 bind(&ServiceOrganizer::onRegisterFailed, this, _1, _2));
        m_face.processEvents();
}
void ServiceOrganizer::onInterest(const InterestFilter& filter, const Interest& interest)
{
        #ifdef INFO
        {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"Interest Received"<<setD<<"] "<<interest<<'\n';
        }
        #endif
        strInterest = interest.getName().toUri();
        //set m_serviceCost,m_requestedServiceName,m_requestedServiceParameter
        convPrefixToMember(strInterest);
        std::cout << m_requestedServiceName<<":"<<m_requestedServiceParameter << '\n';
        //check startService
        if(strcasecmp(m_requestedServiceName.c_str(),"/startService") == 0) {
                std::thread serviceThread(&ServiceOrganizer::startService,this,interest,std::ref(m_networkName),m_requestedServiceParameter);
                serviceThread.detach();
        }
        //check stopService
        else if(strcasecmp(m_requestedServiceName.c_str(),"/stopService") == 0) {
                this->stopService(interest,m_faceFilterName,m_requestedServiceParameter);
        }
        //check getWorkingServiceList
        else if(strcasecmp(m_requestedServiceName.c_str(),"/getServiceList") == 0) {
                this->getServiceList(interest);
        }
        else{
                INFO lout("Unknown Service Request",strInterest);
                sendData(interest,"UNKNOWN_REQUEST");
        }
}

void ServiceOrganizer::convPrefixToMember(std::string &strInterest){
        std::regex reInterest("("+m_networkName+")("+m_nodeName+")(/[a-z|A-Z|0-9]+)(()|(/[a-z|A-Z|0-9]+)+)(()|/COST%3D([0-9]\\d+))");
        std::vector<std::string> prefixList;
        std::smatch matchResult;
        std::regex_match(strInterest,matchResult,reInterest);
        int i = 0;
        for(auto && x:matchResult) {
                if(i == 3) {
                        m_requestedServiceName = x.str();
                }
                else if( i == 4) {
                        m_requestedServiceParameter = x.str();
                }
                else if( i == 9) {
                        if(x.str() == "") {
                                m_serviceCost = 0;
                        }
                        else{
                                m_serviceCost = std::stoi(x.str());
                        }
                }
                i++;
        }
}

void ServiceOrganizer::onRegisterFailed(const Name& prefix, const std::string& reason)
{
        std::cerr << "ERROR: Failed to register prefix \""
                  << prefix << "\" in local hub's daemon (" << reason << ")"
                  << std::endl;
        m_face.shutdown();
}


} // namespace API
} // namespace ndn
