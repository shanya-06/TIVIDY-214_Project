#include "WorkflowObserver.h"
#include "WorkItem.h"
#include "Output.h"

//record work item activity
void AuditObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    log.push_back(item->getId() + " (" + item->getName() + "): " + event);
    Output::stream() << "[audit] work item " << item->getId()
              << " (" << item->getName() << "): "
              << event << std::endl;
}

const std::vector<std::string>& AuditObserver::getLog() const {
    return log;
}

// notify relevant participants
void NotificationObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    Output::stream() << "[notification] work item "
              << item->getName()
              << ": " << event << std::endl;
}

//monitor work item progress
void MonitoringObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    Output::stream() << "[monitoring] work item "
              << item->getId()
              << " is now " << item->getStateName()
              << " (event: " << event << ")"
              << std::endl;
}