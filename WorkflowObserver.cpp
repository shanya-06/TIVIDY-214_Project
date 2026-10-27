#include "WorkflowObserver.h"
#include "WorkItem.h"
#include <iostream>

//record work item activity
void AuditObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    std::cout << "[audit] work item " << item->getId()
              << " (" << item->getName() << "): "
              << event << std::endl;
}

// notify relevant participants
void NotificationObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    std::cout << "[notification] work item "
              << item->getName()
              << ": " << event << std::endl;
}

//monitor work item progress
void MonitoringObserver::update(WorkItem* item, const std::string& event) {
    if (!item) return;

    std::cout << "[monitoring] work item "
              << item->getId()
              << " is now " << item->getStateName()
              << " (event: " << event << ")"
              << std::endl;
}