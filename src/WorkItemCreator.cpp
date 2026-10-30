//a creator
#include "WorkItemCreator.h"
#include "CreatedState.h"
#include <memory>

WorkItemCreator::WorkItemCreator(WorkflowObserver* observer)
    : auditObserver(observer) {}

WorkItem* WorkItemCreator::instantiate(const std::string& id, const std::string& name,
                                       const std::string& requiredRole, bool needsApproval,
                                       Mediator* mediator) {
    std::unique_ptr<WorkItem> item(createItem(id, name, requiredRole, needsApproval, mediator));
    item->setState(new CreatedState());
    item->attach(auditObserver);
    return item.release();
}
