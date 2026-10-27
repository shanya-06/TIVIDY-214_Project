
#include "ExternalServiceAdapter.h"
#include "ExternalService.h"
#include <stdexcept>

//connect the adapter to an external service
ExternalServiceAdapter::ExternalServiceAdapter(ExternalService* service): service(service) {

    if (!service) {
        throw std::invalid_argument("External service cannot be null");
    }
}

// translate and forward requests to the external service
std::string ExternalServiceAdapter::executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) {

    std::string externalOperation = operation;

    if (operation == "processPayment") {
        externalOperation = "payment";
    }
    else if (operation == "verifyAcademicRecord") {
        externalOperation = "academicVerification";
    }

    return service->performRequest(externalOperation, data);
}
