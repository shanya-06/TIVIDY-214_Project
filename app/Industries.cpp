/**
 * @file Industries.cpp
 * @brief The two industries. This is the ONLY place that knows about insurance or
 * university admissions; the engine (includes/ and src/) is reused unchanged.
 */
#include "Industries.h"
#include "IndustryBuilder.h"

#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "ApprovalItemCreator.h"
#include "AuditDecorator.h"
#include "CreatorRegistry.h"
#include "CustomWorkItemProcessor.h"
#include "DeadlineDecorator.h"
#include "DefaultWorkItemProcessor.h"
#include "ExternalCallProcessor.h"
#include "ExternalService.h"
#include "ExternalServiceAdapter.h"
#include "FinalApprovalHandler.h"
#include "FirstLevelApprovalHandler.h"
#include "NotificationDecorator.h"
#include "PriorityDecorator.h"
#include "RoleBasedAssignmentStrategy.h"
#include "RuleBasedAssignmentStrategy.h"
#include "SecondLevelApprovalHandler.h"
#include "SubprocessItemCreator.h"
#include "TaskItemCreator.h"
#include "WorkFlowInstance.h"
#include "WorkItem.h"
#include "WorkItemProcessor.h"
#include "WorkflowBuilder.h"
#include "WorkflowDefinition.h"
#include "WorkflowFacade.h"
#include "WorkflowMediator.h"
#include "WorkflowObserver.h"
#include "WorkflowParticipant.h"
#include "WorkloadAssignmentStrategy.h"

namespace {


int toNumber(const std::string& text, const std::string& what) {
    try {
        std::size_t used = 0;
        int value = std::stoi(text, &used);
        if (used == text.size() && value >= 0) return value;
    } catch (const std::exception&) {}
    throw std::invalid_argument(what + " must be a whole number, e.g. 8000");
}


class InsuranceClaimBuilder : public IndustryBuilder {
public:
    InsuranceClaimBuilder(const std::string& number, int amount, bool bankOk)
        : number(number), amount(amount), bankDetailsOk(bankOk) {}

    void addParticipants() override {
        hire("u1", "Alice (claims admin)", "admin");
        hire("u2", "Ann (assessor)", "assessor");
        hire("u3", "Ben (assessor)", "assessor");
        hire("u4", "Carl (approver)", "approver");
        hire("u5", "Fay (finance)", "finance");
    }

    void addCoordination() override {
        useStrategy(new WorkloadAssignmentStrategy());
        useProcessor(new DefaultWorkItemProcessor());

        const int amount = this->amount;
        useApprovalChain(
            new FirstLevelApprovalHandler([amount](const WorkItem&) { return amount <= 10000; },
                                          [](WorkItem& i) { i.notifyObservers("assessor level approves"); i.approve(); }),
            new SecondLevelApprovalHandler([amount](const WorkItem&) { return amount <= 50000; },
                                           [](WorkItem& i) { i.notifyObservers("senior assessor level approves"); i.approve(); }),
            new FinalApprovalHandler([](const WorkItem&) { return true; },
                                     [amount](WorkItem& i) {
                                         if (amount <= 100000) { i.notifyObservers("claims manager approves"); i.approve(); }
                                         else { i.notifyObservers("claims manager declines"); i.reject("claim exceeds cover limit"); }
                                     }));
    }

    void addWorkItems() override {

        add(new DeadlineDecorator(create("task", "T1", "Validate claim", "admin", false), std::time(nullptr) + 3600));
        WorkItem* assess = add(new PriorityDecorator(
            new NotificationDecorator(create("task", "T2", "Assess claim", "assessor", false), "claimant"), 2));
        add(new AuditDecorator(create("approval", "A1", "Approve claim", "approver", true)));

        std::map<std::string, std::string> request;
        request["claim"] = number;
        request["amount"] = std::to_string(amount);
        request["account"] = bankDetailsOk ? "62-1234-5678" : "";
        WorkItem* pay = create("task", "T3", "Process payment", "finance", false);
        useProcessorFor(pay, new ExternalCallProcessor(mediator, "payClaim", request));
        add(pay);

        add(create("subprocess", "S1", "Close and record claim", "admin", false));


        const int amount = this->amount;
        instance->setRoutingRule([this, assess, amount](WorkFlowInstance&, WorkItem* done) {
            if (done->getId() == "T2" && amount > 30000) {
                WorkItem* extra = create("task", "T4", "Request additional documents", "admin", false);
                insertAfter(assess, extra);
                extra->notifyObservers("routing: claim over R30000, step added");
            }
        });
    }

private:
    std::string number;
    int amount;
    bool bankDetailsOk;
};


class AdmissionsBuilder : public IndustryBuilder {
public:
    AdmissionsBuilder(const std::string& student, int average)
        : student(student), average(average) {}

    void addParticipants() override {
        hire("e1", "Dr Dube (admissions administrator)", "admissions_admin");
        hire("e2", "Ms Naidoo (admissions officer)", "officer");
        hire("e3", "Mr Khan (admissions officer)", "officer");
        hire("e4", "Admissions committee", "committee");
    }

    void addCoordination() override {
        useStrategy(new RuleBasedAssignmentStrategy(3));    
        useProcessor(new CustomWorkItemProcessor());         


        const int avg = average;
        useApprovalChain(
            new FirstLevelApprovalHandler([avg](const WorkItem&) { return avg >= 75; },
                                          [](WorkItem& i) { i.notifyObservers("officer level accepts"); i.approve(); }),
            new SecondLevelApprovalHandler([avg](const WorkItem&) { return avg >= 60; },
                                           [](WorkItem& i) { i.notifyObservers("senior officer level accepts"); i.approve(); }),
            new FinalApprovalHandler([](const WorkItem&) { return true; },
                                     [](WorkItem& i) { i.notifyObservers("committee declines"); i.reject("below minimum requirements"); }));
    }

    void addWorkItems() override {
        add(create("task", "V1", "Validate application", "admissions_admin", false));

        WorkItem* assess = add(create("task", "E1", "Assess academic eligibility", "officer", false));
        WorkItem* decision = add(create("approval", "D1", "Admission decision", "committee", true));
        watch(decision, new NotificationObserver());
        watch(decision, new MonitoringObserver());


        std::map<std::string, std::string> message;
        message["student"] = student;
        message["outcome"] = "offer of admission";
        WorkItem* notify = create("task", "N1", "Notify student", "admissions_admin", false);
        useProcessorFor(notify, new ExternalCallProcessor(mediator, "sendOffer", message));
        add(notify);


        const int avg = average;
        instance->setRoutingRule([this, assess, avg](WorkFlowInstance&, WorkItem* done) {
            if (done->getId() == "E1" && avg >= 60 && avg < 75) {
                WorkItem* extra = create("task", "I1", "Interview applicant", "officer", false);
                insertAfter(assess, extra);
                extra->notifyObservers("routing: borderline average, interview step added");
            }
        });
    }

private:
    std::string student;
    int average;
};


class ShipmentExceptionBuilder : public IndustryBuilder {
public:
    ShipmentExceptionBuilder(const std::string& tracking, int value, bool damaged)
        : tracking(tracking), value(value), damaged(damaged) {}


    void addParticipants() override {
        hire("l1", "Sipho (dispatcher)", "dispatcher");
        hire("l2", "Lerato (investigator)", "investigator");
        hire("l3", "Tom (investigator)", "investigator");
        hire("l4", "Mrs Botha (operations manager)", "manager");
    }


    void addCoordination() override {
        useStrategy(new WorkloadAssignmentStrategy());
        useProcessor(new DefaultWorkItemProcessor());
        const int value = this->value;
        useApprovalChain(
            new FirstLevelApprovalHandler([value](const WorkItem&) { return value <= 5000; },
                                          [](WorkItem& i) { i.notifyObservers("dispatcher level approves compensation"); i.approve(); }),
            new SecondLevelApprovalHandler([value](const WorkItem&) { return value <= 20000; },
                                           [](WorkItem& i) { i.notifyObservers("manager approves compensation"); i.approve(); }),
            new FinalApprovalHandler([](const WorkItem&) { return true; },
                                     [](WorkItem& i) { i.notifyObservers("compensation declined"); i.reject("value above insured limit"); }));
    }


    void addWorkItems() override {
        add(create("task", "L1", "Log shipment exception", "dispatcher", false));
        WorkItem* investigate = add(create("task", "L2", "Investigate exception", "investigator", false));
        add(create("approval", "C1", "Approve compensation", "manager", true));

        std::map<std::string, std::string> message;
        message["tracking"] = tracking;
        message["status"] = "exception resolved";
        WorkItem* notify = create("task", "N1", "Notify customer", "dispatcher", false);
        useProcessorFor(notify, new ExternalCallProcessor(mediator, "notifyCustomer", message));
        add(notify);

     
        const bool damaged = this->damaged;
        instance->setRoutingRule([this, investigate, damaged](WorkFlowInstance&, WorkItem* done) {
            if (done->getId() == "L2" && damaged) {
                WorkItem* extra = create("task", "R1", "Arrange replacement shipment", "dispatcher", false);
                insertAfter(investigate, extra);
                extra->notifyObservers("routing: goods damaged, replacement step added");
            }
        });
    }

private:
    std::string tracking;
    int value;
    bool damaged;
};

} 
void registerIndustries(WorkflowFacade& facade) {
    typedef WorkflowFacade::Params Params;
    typedef WorkflowFacade::ParamList ParamList;

    ParamList claim;
    claim.push_back(std::make_pair("claimNumber", "CL-2001"));
    claim.push_back(std::make_pair("amount", "8000"));
    claim.push_back(std::make_pair("bank", "valid"));  
    facade.addWorkflow(
        WorkflowDefinition("INS-CLAIM", "Insurance claim assessment", 1),
        "Validate, assess, approve by claim value, pay through the payment service, close.",
        claim,
        [](const Params& p) -> WorkflowBuilder* {
            return new InsuranceClaimBuilder(p.at("claimNumber"), toNumber(p.at("amount"), "amount"),
                                             p.at("bank") != "missing");
        });

    ParamList application;
    application.push_back(std::make_pair("student", "Ana"));
    application.push_back(std::make_pair("average", "82"));
    facade.addWorkflow(
        WorkflowDefinition("EDU-APPLICATION", "University application processing", 1),
        "Validate documents, assess eligibility, committee decision by average, notify the student.",
        application,
        [](const Params& p) -> WorkflowBuilder* {
            return new AdmissionsBuilder(p.at("student"), toNumber(p.at("average"), "average"));
        });
    
    ParamList shipment;
    shipment.push_back(std::make_pair("trackingNo", "TRK-5001"));
    shipment.push_back(std::make_pair("value", "3000"));
    shipment.push_back(std::make_pair("damaged", "no"));   
    facade.addWorkflow(
        WorkflowDefinition("LOG-EXCEPTION", "Shipment exception handling", 1),
        "Log, investigate, approve compensation by value, notify the customer.",
        shipment,
        [](const Params& p) -> WorkflowBuilder* {
            return new ShipmentExceptionBuilder(p.at("trackingNo"), toNumber(p.at("value"), "value"),
                                                p.at("damaged") == "yes");
        });
}

namespace {
DemoPreset preset(const std::string& workflow, const std::string& label,
                  const std::string& k1, const std::string& v1,
                  const std::string& k2, const std::string& v2,
                  const std::string& k3 = "", const std::string& v3 = "") {
    DemoPreset p;
    p.workflowId = workflow;
    p.label = label;
    p.params[k1] = v1;
    p.params[k2] = v2;
    if (!k3.empty()) p.params[k3] = v3;
    return p;
}
} 

std::vector<DemoPreset> demoPresets() {
    std::vector<DemoPreset> list;

    list.push_back(preset("INS-CLAIM", "Small claim R8 000: approved at assessor level and paid",
                          "claimNumber", "CL-3001", "amount", "8000", "bank", "valid"));
    list.push_back(preset("INS-CLAIM", "Large claim R35 000: routing adds 'Request additional documents'",
                          "claimNumber", "CL-3002", "amount", "35000", "bank", "valid"));
    list.push_back(preset("INS-CLAIM", "Claim R250 000: over cover limit, declined by manager (exception)",
                          "claimNumber", "CL-3003", "amount", "250000", "bank", "valid"));
    list.push_back(preset("INS-CLAIM", "Missing bank details: payment service fails (exception)",
                          "claimNumber", "CL-3004", "amount", "20000", "bank", "missing"));

    list.push_back(preset("EDU-APPLICATION", "Strong average 82%: accepted, offer sent",
                          "student", "Ana Mokoena", "average", "82"));
    list.push_back(preset("EDU-APPLICATION", "Borderline average 66%: routing adds an interview step",
                          "student", "Ben Pillay", "average", "66"));
    list.push_back(preset("EDU-APPLICATION", "Low average 41%: declined by committee (exception)",
                          "student", "Dan Smith", "average", "41"));
   
    list.push_back(preset("LOG-EXCEPTION", "Late parcel R3 000: compensated at dispatcher level",
                          "trackingNo", "TRK-5001", "value", "3000", "damaged", "no"));
    list.push_back(preset("LOG-EXCEPTION", "Damaged goods R12 000: replacement step added, manager approves",
                          "trackingNo", "TRK-5002", "value", "12000", "damaged", "yes"));
    list.push_back(preset("LOG-EXCEPTION", "High value R80 000: compensation declined (exception)",
                          "trackingNo", "TRK-5003", "value", "80000", "damaged", "no"));
    return list;
}
