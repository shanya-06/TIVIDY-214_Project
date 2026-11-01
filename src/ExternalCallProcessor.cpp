#include "ExternalCallProcessor.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"
#include <stdexcept>

ExternalCallProcessor::ExternalCallProcessor(WorkflowMediator* mediator, const std::string& operation,
                                             const std::map<std::string, std::string>& data)
    : mediator(mediator), operation(operation), data(data) {}

void ExternalCallProcessor::validate(WorkItem* item) {
    (void)item;
    if (data.empty()) {
        throw std::invalid_argument("No data to send to the external system");
    }
}

void ExternalCallProcessor::execute(WorkItem* item) {
    reply = mediator->requestExternalOperation(operation, data);
    item->notifyObservers("External system replied: " + reply);
}

void ExternalCallProcessor::route(WorkItem* item) {
    if (reply.find("Request failed") == 0) {
        item->reject(operation + " failed: " + reply);
    } else {
        item->complete();
    }
}
