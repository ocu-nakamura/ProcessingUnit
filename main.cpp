#include "processingunit.hpp"
#include "threadFunction.hpp"
#include "service.hpp"
int
main(int argc, char** argv)
{
        ndn::API::ServiceOrganizer organizer;
        try{
                organizer.run();
        }catch(const std::exception& e) {
                std::cerr << "ERROR: " << e.what() << std::endl;
        }
        return 0;
}
