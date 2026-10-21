#ifndef EXTERNALSYSTEMADAPTER_H
#define EXTERNALSYSTEMADAPTER_H

#include <string>
#include <map>

class ExternalSystemAdapter {
public:
    virtual ~ExternalSystemAdapter() = default;

    virtual std::string executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) = 0;
};

#endif