#ifndef EXTERNALCALLPROCESSOR_H
#define EXTERNALCALLPROCESSOR_H

#include <map>
#include <string>
#include "WorkItemProcessor.h"

class WorkflowMediator;

/**
 * @brief Processor for an automatic step that calls an external system.
 *
 * validate(): checks that there is data to send.
 * execute(): asks the Mediator to run the operation through the Adapter.
 * route(): completes the item if the reply does not start with
 *          "Request failed", otherwise rejects it.
 *
 * Gang of Four Pattern:
 * - Template Method -> Concrete Class
 */
class ExternalCallProcessor : public WorkItemProcessor {
public:
    /**
     * @brief Constructs an external call processor.
     * @param mediator Mediator coordinating external operations.
     * @param operation Name of the external operation.
     * @param data Data sent to the external system.
     */
    ExternalCallProcessor(WorkflowMediator* mediator, const std::string& operation,
                          const std::map<std::string, std::string>& data);

protected:
    /**
     * @brief Validates the data before executing the external operation.
     * @param item Work item being processed.
     * @throws std::invalid_argument If data is empty.
     */
    void validate(WorkItem* item) override;

    /**
     * @brief Executes the external operation through the Mediator.
     * @param item Work item whose observers receive the response.
     */
    void execute(WorkItem* item) override;

    /**
     * @brief Routes the work item based on the external response.
     * @param item Work item to complete or reject.
     */
    void route(WorkItem* item) override;

private:
    /** @brief Mediator used to request external operations. */
    WorkflowMediator* mediator;

    /** @brief Name of the external operation. */
    std::string operation;

    /** @brief Data sent to the external system. */
    std::map<std::string, std::string> data;

    /** @brief Response received from the external system. */
    std::string reply;
};

#endif
