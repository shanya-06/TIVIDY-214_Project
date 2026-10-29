#include "TaskItemCreator.h"
#include "TaskItem.h"

TaskItemCreator::TaskItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* TaskItemCreator::createItem() {
    return new TaskItem();
}