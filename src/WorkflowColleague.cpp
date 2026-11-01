#include "WorkflowColleague.h"

WorkflowColleague::WorkflowColleague(Mediator* mediator)
    : mediator(mediator)
{
}

WorkflowColleague::WorkflowColleague()
    : mediator(nullptr)
{
}

WorkflowColleague::~WorkflowColleague()
{
}

void WorkflowColleague::changed()
{
    if (mediator != nullptr)
    {
        mediator->notify(this);
    }
}

void WorkflowColleague::setMediator(Mediator* newMediator)
{
    mediator = newMediator;
}

Mediator* WorkflowColleague::getMediator() const
{
    return mediator;
}