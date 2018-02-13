void serviceCreateThread(std::string networkName,std::string serviceName,std::exception_ptr &exception){
        ndn::API::ProcessingUnit service(networkName,serviceName);
        try{
                service.run();
        }
        catch(...) {
                std::cout << "<"<<setB<<std::this_thread::get_id()<<setD<< ">"<<setY<< "LOG"<<setD<<":["<<setY<<"EXCEPTION"<<setD<<"] "<<prefix<<'\n';
                exception = std::current_exception();
        }
}
