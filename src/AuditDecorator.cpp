#include "AuditDecorator.h"

#include "WorkflowParticipant.h"

AuditDecorator::AuditDecorator(WorkItem* component) : WorkItemDecorator(component) {}

std::vector<std::string> AuditDecorator::getAuditLog() const { return log; }

void AuditDecorator::record(const std::string& entry) { log.push_back(entry); }

// Each action logs "attempt", forwards, then logs "ok". If the state throws,
// the attempt stays in the log as FAILED and the exception continues to the caller.
void AuditDecorator::assign(WorkflowParticipant* participant) {
    const std::string what = "assign to " + (participant ? participant->getName() : std::string("nobody"));
    try {
        WorkItemDecorator::assign(participant);
        record(what + ": ok");
    } catch (...) {
        record(what + ": FAILED");
        throw;
    }
}

void AuditDecorator::start() {
    try { WorkItemDecorator::start(); record("start: ok"); }
    catch (...) { record("start: FAILED"); throw; }
}

void AuditDecorator::complete() {
    try { WorkItemDecorator::complete(); record("complete: ok"); }
    catch (...) { record("complete: FAILED"); throw; }
}

void AuditDecorator::approve() {
    try { WorkItemDecorator::approve(); record("approve: ok"); }
    catch (...) { record("approve: FAILED"); throw; }
}

void AuditDecorator::reject(const std::string& reason) {
    try { WorkItemDecorator::reject(reason); record("reject (" + reason + "): ok"); }
    catch (...) { record("reject (" + reason + "): FAILED"); throw; }
}

void AuditDecorator::escalate() {
    try { WorkItemDecorator::escalate(); record("escalate: ok"); }
    catch (...) { record("escalate: FAILED"); throw; }
}
