#ifndef DEFAULTWORKITEMPROCESSOR_H
#define DEFAULTWORKITEMPROCESSOR_H

#include "WorkItemProcessor.h"

class DefaultWorkItemProcessor : public WorkItemProcessor {
public:
    DefaultWorkItemProcessor();
    ~DefaultWorkItemProcessor() override;

protected:
    void validate(WorkItem* item) override;
    void execute(WorkItem* item) override;
    void route(WorkItem* item) override;
};

#endif