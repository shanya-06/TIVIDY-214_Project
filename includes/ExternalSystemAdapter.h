#ifndef EXTERNALSYSTEMADAPTER_H
#define EXTERNALSYSTEMADAPTER_H

#include <string>
#include <map>

/**
 * @brief target interface for external system operations.
 *
 * allows the workflow mediator to communicate with external
 * services without depending on their specific interfaces.
 */
class ExternalSystemAdapter {
public:
    /**
     * @brief destroys the external system adapter.
     */
    virtual ~ExternalSystemAdapter() = default;

    /**
    * @brief executes an operation through an external system.
     * @param operation the operation to execute.
     * @param data the information required for the operation.
     * @return the result returned by the external system.
     */
    virtual std::string executeOperation(const std::string& operation, const std::map<std::string, std::string>& data) = 0;
};

#endif