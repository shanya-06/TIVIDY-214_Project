
#ifndef CUSTOMWORKITEMPROCESSOR_H
#define CUSTOMWORKITEMPROCESSOR_H

#include "WorkItemProcessor.h"

class CustomWorkItemProcessor : public WorkItemProcessor {
public:
    CustomWorkItemProcessor();
    ~CustomWorkItemProcessor() override;

protected:
    void validate(WorkItem* item) override;
    void execute(WorkItem* item) override;
    void route(WorkItem* item) override;
};

#endif 
