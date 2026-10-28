#ifndef APPROVALHANDLER_H
#define APPROVALHANDLER_H

class WorkItem;

class ApprovalHandler {
protected:
    ApprovalHandler* successor = nullptr;
    void forwardRequest(WorkItem& item);

public:
    virtual ~ApprovalHandler() = default;
    void setSuccessor(ApprovalHandler* next);
    virtual void handleRequest(WorkItem& item) = 0;
};

#endif
