#include "WorkflowFacade.h"

#include <ostream>
#include <stdexcept>

#include "Command.h"
#include "Output.h"
#include "WFStatus.h"
#include "WorkFlowInstance.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"
#include "WorkflowParticipant.h"

namespace {
const char* statusName(WFStatus s) {
    static const char* const names[] = {"CREATED", "RUNNING", "COMPLETED", "CANCELLED", "FAILED"};
    return names[s];
}
std::ostream& nullStream() {
    static std::ostream sink(nullptr);
    return sink;
}
}

WorkflowFacade::WorkflowFacade()
    : taskCreator(&adHocAudit), approvalCreator(&adHocAudit), subprocessCreator(&adHocAudit) {
    adHocItems.registerCreator("task", &taskCreator);
    adHocItems.registerCreator("approval", &approvalCreator);
    adHocItems.registerCreator("subprocess", &subprocessCreator);
}


WorkflowFacade::~WorkflowFacade() {
    Output::redirect(nullptr);
    running.clear();
}

void WorkflowFacade::addWorkflow(const WorkflowDefinition& definition, const std::string& description,
                                 const ParamList& parameters, BuilderFactory factory) {
    WorkflowType t = {definition, description, parameters, factory};
    types.push_back(t);
}

std::vector<WorkflowFacade::WorkflowInfo> WorkflowFacade::workflows() const {
    std::vector<WorkflowInfo> out;
    for (const WorkflowType& t : types) {
        WorkflowInfo w = {t.definition.getId(), t.definition.getName(), t.description, t.parameters};
        out.push_back(w);
    }
    return out;
}

std::string WorkflowFacade::startWorkflow(const std::string& workflowId, const Params& params) {
    for (WorkflowType& t : types) {
        if (t.definition.getId() != workflowId) continue;
        Params all;
        for (const auto& p : t.parameters) all[p.first] = p.second;
        for (const auto& p : params) {
            if (all.find(p.first) == all.end())
                throw std::invalid_argument("Unknown parameter: " + p.first);
            all[p.first] = p.second;
        }
        std::unique_ptr<Running> r(new Running());
        r->builder.reset(t.factory(all));
        r->workflowId = workflowId;
        r->key = workflowId + "-" + std::to_string(counters[workflowId] + 1);
        r->instance = t.definition.createInstance(*r->builder, r->key);
        r->instance->start();
        ++counters[workflowId];
        running.push_back(std::move(r));
        return running.back()->key;
    }
    throw std::invalid_argument("Unknown workflow: " + workflowId);
}

WorkflowFacade::Running& WorkflowFacade::find(const std::string& key) const {
    for (const auto& r : running)
        if (r->key == key) return *r;
    throw std::invalid_argument("Unknown instance: " + key);
}

WorkflowFacade::Running& WorkflowFacade::findRunning(const std::string& key) const {
    Running& r = find(key);
    if (r.instance->getStatus() != RUNNING)
        throw std::logic_error("Instance " + key + " is " + statusName(r.instance->getStatus()) + ", no more actions allowed");
    return r;
}

WorkItem* WorkflowFacade::findItem(Running& r, const std::string& itemId) const {
    for (WorkItem* item : r.instance->getWorkItems())
        if (item->getId() == itemId) return item;
    throw std::invalid_argument("Unknown work item: " + itemId);
}

WorkflowParticipant* WorkflowFacade::findParticipant(Running& r, const std::string& id) const {
    for (WorkflowParticipant* p : r.instance->getMediator()->getParticipants())
        if (p->getId() == id) return p;
    throw std::invalid_argument("Unknown participant: " + id);
}

WorkflowFacade::ItemInfo WorkflowFacade::describe(WorkItem* item) {
    ItemInfo i;
    i.id = item->getId();
    i.name = item->getName();
    i.role = item->getRequiredRole();
    i.state = item->getStateName();
    i.assignee = item->getAssignee() ? item->getAssignee()->getName() : "";
    i.needsApproval = item->requiresApproval();
    i.awaitingApproval = item->isAwaitingApproval();
    i.inProgress = item->isInProgress();
    i.finished = item->isCompleted() || item->isRejected();
    return i;
}

WorkflowFacade::InstanceInfo WorkflowFacade::instance(const std::string& key) const {
    Running& r = find(key);
    InstanceInfo info;
    info.key = key;
    info.workflow = r.workflowId;
    info.status = statusName(r.instance->getStatus());
    info.total = 0;
    info.completed = 0;
    for (WorkItem* item : r.instance->getWorkItems()) {
        ++info.total;
        if (item->isCompleted()) ++info.completed;
    }
    return info;
}

std::vector<WorkflowFacade::InstanceInfo> WorkflowFacade::instances() const {
    std::vector<InstanceInfo> out;
    for (const auto& r : running) out.push_back(instance(r->key));
    return out;
}

void WorkflowFacade::cancel(const std::string& key) {
    findRunning(key).instance->cancel();
}

std::vector<WorkflowFacade::ItemInfo> WorkflowFacade::items(const std::string& key) const {
    std::vector<ItemInfo> out;
    for (WorkItem* item : find(key).instance->getWorkItems()) out.push_back(describe(item));
    return out;
}

WorkflowFacade::ItemInfo WorkflowFacade::item(const std::string& key, const std::string& itemId) const {
    Running& r = find(key);
    return describe(findItem(r, itemId));
}

std::vector<WorkflowFacade::ItemInfo> WorkflowFacade::availableWork(const std::string& key) const {
    std::vector<ItemInfo> out;
    Running& r = find(key);
    if (r.instance->getStatus() != RUNNING) return out;
    for (WorkItem* item : r.instance->getWorkItems())
        if (item->canBeEscalated() || item->canBeAssigned()) out.push_back(describe(item));
    return out;
}

std::vector<WorkflowFacade::ParticipantInfo> WorkflowFacade::participants(const std::string& key) const {
    std::vector<ParticipantInfo> out;
    for (WorkflowParticipant* p : find(key).instance->getMediator()->getParticipants()) {
        ParticipantInfo i = {p->getId(), p->getName(), p->getRole(), p->getAvailable(), p->getWorkLoad()};
        out.push_back(i);
    }
    return out;
}

std::vector<std::string> WorkflowFacade::events(const std::string& key) const {
    return find(key).instance->getEvents();
}

std::vector<std::string> WorkflowFacade::commands(const std::string& key) const {
    std::vector<std::string> out;
    for (Command* c : find(key).instance->getHistory()) out.push_back(c->getDescription());
    return out;
}

void WorkflowFacade::startWork(const std::string& key, const std::string& itemId) {
    Running& r = findRunning(key);
    findItem(r, itemId)->start();
}

void WorkflowFacade::completeWork(const std::string& key, const std::string& itemId) {
    Running& r = findRunning(key);
    r.instance->executeCommand(new CompleteWorkCommand(findItem(r, itemId)));
}

void WorkflowFacade::rejectWork(const std::string& key, const std::string& itemId, const std::string& reason) {
    Running& r = findRunning(key);
    r.instance->executeCommand(new RejectCommand(findItem(r, itemId), reason));
}

void WorkflowFacade::escalateWork(const std::string& key, const std::string& itemId) {
    Running& r = findRunning(key);
    r.instance->executeCommand(new EscalateCommand(findItem(r, itemId)));
}

void WorkflowFacade::assignWork(const std::string& key, const std::string& itemId, const std::string& participantId) {
    Running& r = findRunning(key);
    WorkItem* item = findItem(r, itemId);
    r.instance->executeCommand(new AssignCommand(item, findParticipant(r, participantId)));
}

void WorkflowFacade::setAvailable(const std::string& key, const std::string& participantId, bool available) {
    Running& r = findRunning(key);
    findParticipant(r, participantId)->setAvailable(available);
}

void WorkflowFacade::showLiveMessages(bool show) {
    Output::redirect(show ? nullptr : &nullStream());
}

void WorkflowFacade::sendLiveMessagesTo(std::ostream* sink) {
    Output::redirect(sink);
}

void WorkflowFacade::removeWorkflow(const std::string& workflowId) {
    for (std::vector<WorkflowType>::iterator it = types.begin(); it != types.end(); ++it) {
        if (it->definition.getId() == workflowId) {
            types.erase(it);
            return;
        }
    }
    throw std::invalid_argument("Unknown workflow: " + workflowId);
}

void WorkflowFacade::insertWorkItem(const std::string& key, const std::string& afterItemId, const WorkItemSpec& spec) {
    Running& r = findRunning(key);
    WorkItem* after = findItem(r, afterItemId);
    std::vector<WorkItem*> items = r.instance->getWorkItems();

    for (WorkItem* item : items)
        if (item->getId() == spec.id)
            throw std::invalid_argument("This instance already has a work item " + spec.id);

   
    std::size_t current = items.size(), position = 0;
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (current == items.size() && !items[i]->isCompleted()) current = i;
        if (items[i] == after) position = i;
    }
    if (position < current)
        throw std::logic_error("New steps can only be added after the current work (" +
                               (current < items.size() ? items[current]->getId() : std::string("none")) + ")");

    bool roleCovered = false;
    for (WorkflowParticipant* p : r.instance->getMediator()->getParticipants())
        if (p->getRole() == spec.role) roleCovered = true;
    if (!roleCovered)
        throw std::logic_error("Nobody in this instance has the role '" + spec.role + "', so the step could never be assigned");

    WorkItem* item = assembleWorkItem(adHocItems, spec, r.instance->getMediator());
    r.instance->getMediator()->registerWorkItem(item);
    try {
        r.instance->addWorkItemAfter(after, item);
    } catch (...) {
        r.instance->getMediator()->unregisterWorkItem(item);
        delete item;
        throw;
    }
    item->notifyObservers("added by hand after " + afterItemId);
}
