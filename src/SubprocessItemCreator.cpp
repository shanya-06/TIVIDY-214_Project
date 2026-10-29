#include "SubprocessItemCreator.h"
#include "SubprocessItem.h"

SubprocessItemCreator::SubprocessItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* SubprocessItemCreator::createItem() {
    return new SubprocessItem();
}