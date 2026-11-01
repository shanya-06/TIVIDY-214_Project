#ifndef EXTERNALSERVICE_H
#define EXTERNALSERVICE_H

#include <string>
#include <map>

/**
 * @brief represents an external service used by the workflow system.
 *
 * acts as the adaptee in the adapter pattern and processes
 * requests received through the external service adapter.
 */
class ExternalService {
public:
    /**
     * @brief processes an external service request.
     * @param operation the external operation to perform.
     * @param payload the data required to process the request.
     * @return a message describing the result of the operation.
     */
    std::string performRequest(
        const std::string& operation,
        const std::map<std::string, std::string>& payload);
};

#endif
