#ifndef MEDIATOR_H
#define MEDIATOR_H

class WorkflowColleague;

class Mediator {
public:
    virtual ~Mediator() = default;

    virtual void notify(WorkflowColleague* originator) = 0;
};

#endif