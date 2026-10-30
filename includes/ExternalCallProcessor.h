#ifndef EXTERNALCALLPROCESSOR_H
#define EXTERNALCALLPROCESSOR_H

#include <map>
#include <string>
#include "WorkItemProcessor.h"

class WorkflowMediator;

/**
 * @brief Processor for an automatic step that calls an external system.
 *
 * validate(): checks there is data to send.
 * execute():  asks the Mediator to run the operation through the Adapter.
 * route():    completes the item if the reply is not "Request failed...",
 *             otherwise rejects it.
 *
 * Gang of Four Pattern:
 * - Template Method -> Concrete Class
 */
class ExternalCallProcessor : public WorkItemProcessor {
public:
    ExternalCallProcessor(WorkflowMediator* mediator, const std::string& operation,
                          const std::map<std::string, std::string>& data);

protected:
    void validate(WorkItem* item) override;
    void execute(WorkItem* item) override;
    void route(WorkItem* item) override;

private:
    WorkflowMediator* mediator;
    std::string operation;
    std::map<std::string, std::string> data;
    std::string reply;
};

#endif
