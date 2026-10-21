#ifndef EXTERNALSERVICE_H
#define EXTERNALSERVICE_H

#include <string>
#include <map>

class ExternalService {
public:
    std::string performRequest(const std::string& operation, const std::map<std::string, std::string>& payload);
};

#endif