//a creator
#include "WorkItemCreator.h"
#include "CreatedState.h"

WorkItemCreator::WorkItemCreator(WorkflowObserver* observer)
    : auditObserver(observer) {}

WorkItem* WorkItemCreator::instantiate() {
    WorkItem* item = createItem();
    item->setState(new CreatedState());
    item->attach(auditObserver);
    return item;
}