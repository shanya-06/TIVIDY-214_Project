#ifndef WORKFLOWFACADE_H
#define WORKFLOWFACADE_H

#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ApprovalItemCreator.h"
#include "CreatorRegistry.h"
#include "SubprocessItemCreator.h"
#include "TaskItemCreator.h"
#include "WorkItemSpec.h"
#include "WorkflowBuilder.h"
#include "WorkflowDefinition.h"
#include "WorkflowObserver.h"

class WorkFlowInstance;
class WorkItem;
class WorkflowParticipant;

/**
 * @class WorkflowFacade
 * @brief One simple entry point to the whole engine (Gang of Four Pattern: Facade).
 *
 * A user interface (the GUI) only needs this class. It hides the Director/Builder,
 * the instances, the mediator, the commands and the state machine behind a handful
 * of calls, and hands back plain data (the Info structs) so the UI never touches
 * engine objects or raw pointers.
 *
 * Instances are addressed by a key such as "INS-CLAIM-1". Every action refuses
 * (std::logic_error) if the instance is not RUNNING and throws std::invalid_argument
 * for an unknown key, item or participant. Engine errors (invalid transitions)
 * propagate as std::logic_error so the UI can simply print what().
 */
class WorkflowFacade {
public:
    typedef std::map<std::string, std::string> Params;
    typedef std::vector<std::pair<std::string, std::string> > ParamList; ///< (name, default)
    typedef std::function<WorkflowBuilder*(const Params&)> BuilderFactory;

    struct WorkflowInfo {
        std::string id, name, description;
        ParamList parameters;
    };
    struct InstanceInfo {
        std::string key, workflow, status;
        int completed, total;
    };
    struct ItemInfo {
        std::string id, name, role, state, assignee;
        bool needsApproval, awaitingApproval, inProgress, finished;
    };
    struct ParticipantInfo {
        std::string id, name, role;
        bool available;
        int workload;
    };

    WorkflowFacade();
    ~WorkflowFacade();
    WorkflowFacade(const WorkflowFacade&) = delete;
    WorkflowFacade& operator=(const WorkflowFacade&) = delete;

    /** @brief Registers an industry workflow; the factory makes a builder for given parameters. */
    void addWorkflow(const WorkflowDefinition& definition, const std::string& description,
                     const ParamList& parameters, BuilderFactory factory);
    std::vector<WorkflowInfo> workflows() const;

    /**
     * @brief Removes a workflow definition so no new instances can be started from it.
     * Instances already running keep working (they own their parts).
     */
    void removeWorkflow(const std::string& workflowId);

    /** @brief Starts a new independent instance; missing parameters use their defaults. Returns its key. */
    std::string startWorkflow(const std::string& workflowId, const Params& params = Params());
    std::vector<InstanceInfo> instances() const;
    InstanceInfo instance(const std::string& key) const;
    void cancel(const std::string& key);

   
    std::vector<ItemInfo> items(const std::string& key) const;
    ItemInfo item(const std::string& key, const std::string& itemId) const;
    /** @brief Items that can currently be acted on (empty if the instance is not running). */
    std::vector<ItemInfo> availableWork(const std::string& key) const;
    std::vector<ParticipantInfo> participants(const std::string& key) const;
    std::vector<std::string> events(const std::string& key) const; 
    std::vector<std::string> commands(const std::string& key) const;

    
    void startWork(const std::string& key, const std::string& itemId);
    void completeWork(const std::string& key, const std::string& itemId);
    void rejectWork(const std::string& key, const std::string& itemId, const std::string& reason);
    void escalateWork(const std::string& key, const std::string& itemId);
    void assignWork(const std::string& key, const std::string& itemId, const std::string& participantId);
    void setAvailable(const std::string& key, const std::string& participantId, bool available);

    /**
     * @brief Adds an ad-hoc work item to a running instance, straight after afterItemId.
     *
     * The item is made by the Factory Method registry and decorated as the spec asks,
     * then joins the instance's normal flow: it opens when the item before it completes
     * and the instance's assignment Strategy picks who does it.
     * @throws std::logic_error if the instance is not running, the position is before the
     *         current work, or no participant has the required role.
     * @throws std::invalid_argument for an unknown item or a duplicate ID.
     */
    void insertWorkItem(const std::string& key, const std::string& afterItemId, const WorkItemSpec& spec);

    /** @brief Switches the engine's live observer/notification lines on or off. */
    void showLiveMessages(bool show);

    /**
     * @brief Sends the engine's live observer/notification lines to sink instead
     * (nullptr restores std::cout). Lets a UI collect them and show them in a panel.
     */
    void sendLiveMessagesTo(std::ostream* sink);

private:
    struct WorkflowType {
        WorkflowDefinition definition;
        std::string description;
        ParamList parameters;
        BuilderFactory factory;
    };
    struct Running {
        std::string key, workflowId;
        std::unique_ptr<WorkflowBuilder> builder;
        WorkFlowInstance* instance;
    };

    Running& find(const std::string& key) const;
    Running& findRunning(const std::string& key) const;
    WorkItem* findItem(Running& r, const std::string& itemId) const;
    WorkflowParticipant* findParticipant(Running& r, const std::string& id) const;
    static ItemInfo describe(WorkItem* item);

    AuditObserver adHocAudit;               
    TaskItemCreator taskCreator;
    ApprovalItemCreator approvalCreator;
    SubprocessItemCreator subprocessCreator;
    CreatorRegistry adHocItems;          
    std::vector<WorkflowType> types;
    std::vector<std::unique_ptr<Running> > running;
    std::map<std::string, int> counters;
};

#endif
