#ifndef EXTERNALSERVICEADAPTER_H
#define EXTERNALSERVICEADAPTER_H

#include "ExternalSystemAdapter.h"

class ExternalService;

/**
 * @brief adapts external service requests to the workflow interface.
 *
 * translates workflow operations into requests understood
 * by the external service.
 */
class ExternalServiceAdapter : public ExternalSystemAdapter {
private:
    ExternalService* service;

public:
    /**
     * @brief creates an adapter for an external service.
     * @param service pointer to the external service.
     * @note the adapter does not own the external service.
     */
    explicit ExternalServiceAdapter(ExternalService* service);

    /**
     * @brief translates and forwards an operation to the external service.
     * @param operation the workflow operation to perform.
     * @param data the information associated with the operation.
     * @return the response from the external service.
     */
    std::string executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) override;
};

#endif
