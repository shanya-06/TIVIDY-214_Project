#ifndef WORKITEMSPEC_H
#define WORKITEMSPEC_H

#include <string>

class CreatorRegistry;
class Mediator;
class WorkItem;

/**
 * @brief Plain description of one work item and the optional behaviour to add to it.
 *
 * Used wherever work items are made from data instead of code: the workflow
 * designer (custom workflows) and ad-hoc steps inserted into a running instance.
 */
struct WorkItemSpec {
    std::string type;          ///< creator key in the CreatorRegistry: "task", "approval" or "subprocess"
    std::string id;
    std::string name;
    std::string role;          ///< role a participant needs to be assigned this item
    bool needsApproval;        ///< completion goes through the approval chain
    int priority;              ///< > 0 wraps the item in a PriorityDecorator
    std::string notify;        ///< non-empty wraps it in a NotificationDecorator for this recipient
    int deadlineMinutes;       ///< > 0 wraps it in a DeadlineDecorator
    bool audit;                ///< wraps it in an AuditDecorator

    WorkItemSpec() : needsApproval(false), priority(0), deadlineMinutes(0), audit(false) {}
};

/**
 * @brief Creates the item through the registry (Factory Method) and wraps it in the
 * requested decorators (Decorator). The caller owns the returned item.
 * @throws std::invalid_argument for an unknown type or missing id/name/role.
 */
WorkItem* assembleWorkItem(CreatorRegistry& registry, const WorkItemSpec& spec, Mediator* mediator);

#endif
