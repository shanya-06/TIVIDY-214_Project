#ifndef WORKFLOWPARTICIPANT_H
#define WORKFLOWPARTICIPANT_H

#include "WorkflowColleague.h"
#include <string>

class WorkflowParticipant : public WorkflowColleague {
private:
    std::string id;
    std::string name;
    std::string role;
    bool available;
    int workload;

public:
    WorkflowParticipant(const std::string& id,const std::string& name,
        const std::string& role,Mediator* mediator);
    WorkflowParticipant();
    ~WorkflowParticipant() override;
    void setAvailable(bool available);
    bool getAvailable() const;
    int getWorkload() const;
    std::string getId() const;
    std::string getName() const;
    std::string getRole() const;
    void incrementWorkload();
    void decrementWorkload();
};

#endif