#ifndef WORKITEMPROCESSOR_H
#define WORKITEMPROCESSOR_H

class WorkItem;

class WorkItemProcessor {
public:
    WorkItemProcessor();
    virtual ~WorkItemProcessor();
    void process(WorkItem* item);

protected:
    virtual void validate(WorkItem* item) = 0;
    virtual void execute(WorkItem* item) = 0;
    virtual void route(WorkItem* item) = 0;
};

#endif