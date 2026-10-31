#include "WorkItemSpec.h"

#include <ctime>
#include <memory>
#include <stdexcept>

#include "AuditDecorator.h"
#include "CreatorRegistry.h"
#include "DeadlineDecorator.h"
#include "NotificationDecorator.h"
#include "PriorityDecorator.h"
#include "WorkItem.h"

WorkItem* assembleWorkItem(CreatorRegistry& registry, const WorkItemSpec& spec, Mediator* mediator) {
    if (spec.id.empty()) throw std::invalid_argument("A work item needs an ID");
    if (spec.name.empty()) throw std::invalid_argument("Work item " + spec.id + " needs a name");
    if (spec.role.empty()) throw std::invalid_argument("Work item " + spec.id + " needs a role");

    // Approval items always go through the approval chain.
    bool approval = spec.needsApproval || spec.type == "approval";
    std::unique_ptr<WorkItem> item(registry.create(spec.type, spec.id, spec.name, spec.role, approval, mediator));

    // Each decorator takes ownership of the item it wraps.
    if (spec.deadlineMinutes > 0)
        item.reset(new DeadlineDecorator(item.release(), std::time(nullptr) + 60 * spec.deadlineMinutes));
    if (!spec.notify.empty())
        item.reset(new NotificationDecorator(item.release(), spec.notify));
    if (spec.audit)
        item.reset(new AuditDecorator(item.release()));
    if (spec.priority > 0)
        item.reset(new PriorityDecorator(item.release(), spec.priority));
    return item.release();
}
