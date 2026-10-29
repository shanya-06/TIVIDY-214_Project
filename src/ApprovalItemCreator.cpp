#include "ApprovalItemCreator.h"
#include "ApprovalItem.h"

ApprovalItemCreator::ApprovalItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* ApprovalItemCreator::createItem() {
    return new ApprovalItem();
}