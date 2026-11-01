#include "DefaultWorkItemProcessor.h"

#include "WorkItem.h"

#include <stdexcept>

DefaultWorkItemProcessor::DefaultWorkItemProcessor()
{
}

DefaultWorkItemProcessor::~DefaultWorkItemProcessor()
{
}

void DefaultWorkItemProcessor::validate(WorkItem* item)
{
    if (item == nullptr)
    {
        throw std::invalid_argument("Cannot validate a null WorkItem");
    }

    if (item->getState() == nullptr)
    {
        throw std::logic_error("Cannot process a WorkItem without a state");
    }
}

void DefaultWorkItemProcessor::execute(WorkItem* item)
{
    item->notifyObservers("Work item executed by default processor");
}

void DefaultWorkItemProcessor::route(WorkItem* item)
{
    item->notifyObservers("Work item routed by default processor");
}