#include "CustomWorkItemProcessor.h"

#include "WorkItem.h"

#include <stdexcept>

CustomWorkItemProcessor::CustomWorkItemProcessor()
{
}

CustomWorkItemProcessor::~CustomWorkItemProcessor()
{
}

void CustomWorkItemProcessor::validate(WorkItem* item)
{
    if (item == nullptr)
    {
        throw std::invalid_argument("Cannot validate a null WorkItem");
    }

    if (item->getState() == nullptr)
    {
        throw std::logic_error("Cannot process a WorkItem without a state");
    }

    item->notifyObservers("Work item validated by custom processor");
}

void CustomWorkItemProcessor::execute(WorkItem* item)
{

    item->notifyObservers("Work item executed by custom processor");
}

void CustomWorkItemProcessor::route(WorkItem* item)
{
    item->notifyObservers("Work item routed by custom processor");
}