#ifndef PRIORITYDECORATOR_H
#define PRIORITYDECORATOR_H

#include "WorkItemDecorator.h"

/** @brief Adds a priority level (1 = lowest) to a work item. Escalating raises it by one. */
class PriorityDecorator : public WorkItemDecorator {
public:
    PriorityDecorator(WorkItem* component, int priority);

    int getPriority() const;
    virtual std::string getName() const override;
    virtual void escalate() override;

private:
    int priority;
};

#endif
