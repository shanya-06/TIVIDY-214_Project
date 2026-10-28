
#include "ExternalService.h"
#include <stdexcept>

// process a request from the adapter
std::string ExternalService::performRequest(const std::string& operation, const std::map<std::string, std::string>& payload) {

    if (operation.empty()) {
        return "Request failed: invalid operation";
    }

    if (payload.empty()) {
        return "Request failed: missing data";
    }

    //check for empty values
    for (const auto& entry : payload) {
        if (entry.second.empty()) {
            return "Request failed: empty data value";
        }
    }

    return "Request completed successfully";
}
