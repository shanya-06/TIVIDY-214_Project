#ifndef EXTERNALSERVICEADAPTER_H
#define EXTERNALSERVICEADAPTER_H

#include "ExternalSystemAdapter.h"

class ExternalService;

class ExternalServiceAdapter : public ExternalSystemAdapter {
private:
    ExternalService* service;

public:
    explicit ExternalServiceAdapter(ExternalService* service);

    std::string executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) override;
};

#endif