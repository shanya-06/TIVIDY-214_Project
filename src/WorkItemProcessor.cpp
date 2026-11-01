#include "WorkItemProcessor.h"

#include <stdexcept>

WorkItemProcessor::WorkItemProcessor()
{
}

WorkItemProcessor::~WorkItemProcessor()
{
}

void WorkItemProcessor::process(WorkItem* item)
{
    if (item == nullptr)
    {
        throw std::invalid_argument("Cannot process a null WorkItem");
    }

    validate(item);
    execute(item);
    route(item);
}