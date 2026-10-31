#ifndef INDUSTRYBUILDER_H
#define INDUSTRYBUILDER_H

/**
 * @file IndustryBuilder.h
 * @brief Base class for the concrete Builders in the app layer (not part of the engine).
 */

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ApprovalHandler.h"
#include "ApprovalItemCreator.h"
#include "AssignmentStrategy.h"
#include "CreatorRegistry.h"
#include "ExternalService.h"
#include "ExternalServiceAdapter.h"
#include "SubprocessItemCreator.h"
#include "TaskItemCreator.h"
#include "WorkFlowInstance.h"
#include "WorkItem.h"
#include "WorkItemProcessor.h"
#include "WorkItemSpec.h"
#include "WorkflowBuilder.h"
#include "WorkflowMediator.h"
#include "WorkflowObserver.h"
#include "WorkflowParticipant.h"

/**
 * Shared part of the concrete builders. Owns everything ONE workflow instance
 * needs (participants, strategy, approval chain, processors, external adapter)
 * so each instance is fully independent, and also holds the helpers the demo
 * script uses to work on the instance. The industry builders below fill in
 * addParticipants(), addCoordination() and addWorkItems().
 * This is industry code; it is not part of the engine.
 */
class IndustryBuilder : public WorkflowBuilder {
public:
    IndustryBuilder()
        : mediator(nullptr), tasks(&audit), approvals(&audit), subprocesses(&audit), adapter(&service) {
        registry.registerCreator("task", &tasks);
        registry.registerCreator("approval", &approvals);
        registry.registerCreator("subprocess", &subprocesses);
    }

    void createInstance(const std::string& id) override {
        instance.reset(new WorkFlowInstance(id));
        mediator = new WorkflowMediator(); 
        instance->setMediator(mediator);
        mediator->setExternalAdapter(&adapter);
    }

    WorkFlowInstance* getInstance() override {
        return instance.get();
    }

    WorkflowParticipant* hire(const std::string& id, const std::string& name, const std::string& role) {
        people.emplace_back(new WorkflowParticipant(id, name, role, mediator));
        mediator->registerParticipant(people.back().get());
        return people.back().get();
    }

    void useStrategy(AssignmentStrategy* s) {
        strategy.reset(s);
        mediator->setAssignmentStrategy(strategy.get());
    }

    void useProcessor(WorkItemProcessor* p) {
        processors.emplace_back(p);
        mediator->setProcessor(p);
    }

    void useProcessorFor(WorkItem* item, WorkItemProcessor* p) {
        processors.emplace_back(p);
        item->setProcessor(p);
    }

  
    void useApprovalChain(ApprovalHandler* first, ApprovalHandler* second, ApprovalHandler* third) {
        handlers.emplace_back(first);
        handlers.emplace_back(second);
        handlers.emplace_back(third);
        first->setSuccessor(second);
        second->setSuccessor(third);
        mediator->setApprovalHandler(first);
    }


    WorkItem* create(const std::string& type, const std::string& id, const std::string& name,
                     const std::string& role, bool needsApproval) {
        return registry.create(type, id, name, role, needsApproval, mediator);
    }

  
    void watch(WorkItem* item, WorkflowObserver* observer) {
        observers.emplace_back(observer);
        item->attach(observer);
    }

  
    WorkItem* create(const WorkItemSpec& spec) {
        return assembleWorkItem(registry, spec, mediator);
    }

   
    WorkItem* add(WorkItem* item) {
        mediator->registerWorkItem(item);
        instance->addWorkItem(item);
        return item;
    }

    
    void insertAfter(WorkItem* after, WorkItem* extra) {
        mediator->registerWorkItem(extra);
        instance->addWorkItemAfter(after, extra);
    }

    AuditObserver audit;
    ExternalService service;
    WorkflowMediator* mediator;

private:
    TaskItemCreator tasks;
    ApprovalItemCreator approvals;
    SubprocessItemCreator subprocesses;
    CreatorRegistry registry;
    ExternalServiceAdapter adapter;
    std::vector<std::unique_ptr<WorkflowParticipant> > people;
    std::unique_ptr<AssignmentStrategy> strategy;
    std::vector<std::unique_ptr<WorkItemProcessor> > processors;
    std::vector<std::unique_ptr<ApprovalHandler> > handlers;
    std::vector<std::unique_ptr<WorkflowObserver> > observers;
public:
    std::unique_ptr<WorkFlowInstance> instance; 
};

#endif
