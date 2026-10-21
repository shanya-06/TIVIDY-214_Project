#ifndef WORKFLOWCOLLEAGUE_H
#define WORKFLOWCOLLEAGUE_H

#include "Mediator.h"

class WorkflowColleague {
protected:
    Mediator* mediator;

public:
    WorkflowColleague(Mediator* mediator);
    WorkflowColleague();
    virtual ~WorkflowColleague();
    void changed();
    void setMediator(Mediator* mediator);
    Mediator* getMediator() const;
};

#endif