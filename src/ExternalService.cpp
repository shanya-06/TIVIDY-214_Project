
#include "ExternalService.h"
#include <stdexcept>

// process a request from the adapter
std::string ExternalService::performRequest(const std::string& operation, const std::map<std::string, std::string>& payload) {

    if (operation.empty()) {
        throw std::invalid_argument("Operation cannot be empty");
    }

    if (operation == "payment") {
        if (payload.find("claimId") == payload.end() ||
            payload.find("amount") == payload.end()) {
            return "Payment failed: missing claim details";
        }

        return "Payment processed for claim " + payload.at("claimId") + ", amount: " + payload.at("amount");
    }

    if (operation == "academicVerification") {
        if (payload.find("studentId") == payload.end()) {
            return "Verification failed: missing student ID";
        }

        return "Academic records verified for student " + payload.at("studentId");
    }

    return "Unsupported external operation: " + operation;
}
