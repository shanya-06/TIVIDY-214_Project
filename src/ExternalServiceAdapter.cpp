#include "ExternalServiceAdapter.h"
#include "ExternalService.h"
#include <stdexcept>

//connect the adapter to an external service
ExternalServiceAdapter::ExternalServiceAdapter(ExternalService* service): service(service) {

    if (!service) {
        throw std::invalid_argument("External service cannot be null");
    }
}

//forward requests to external service
std::string ExternalServiceAdapter::executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) {
    
    if (operation.empty()) {
        return "Request failed: invalid operation";
    }

    if (data.empty()) {
        return "Request failed: missing data";
    }

    //forward the request to external service
    return service->performRequest(operation, data);
}
