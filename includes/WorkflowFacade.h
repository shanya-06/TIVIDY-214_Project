#ifndef WORKFLOWFACADE_H
#define WORKFLOWFACADE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "WorkflowBuilder.h"
#include "WorkflowDefinition.h"

class WorkFlowInstance;
class WorkItem;
class WorkflowParticipant;

/**
 * @class WorkflowFacade
 * @brief One simple entry point to the whole engine (Gang of Four Pattern: Facade).
 *
 * A user interface (the TUI) only needs this class. It hides the Director/Builder,
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

    /** @brief Starts a new independent instance; missing parameters use their defaults. Returns its key. */
    std::string startWorkflow(const std::string& workflowId, const Params& params = Params());
    std::vector<InstanceInfo> instances() const;
    InstanceInfo instance(const std::string& key) const;
    void cancel(const std::string& key);

    // ---- inspect
    std::vector<ItemInfo> items(const std::string& key) const;
    ItemInfo item(const std::string& key, const std::string& itemId) const;
    /** @brief Items that can currently be acted on (empty if the instance is not running). */
    std::vector<ItemInfo> availableWork(const std::string& key) const;
    std::vector<ParticipantInfo> participants(const std::string& key) const;
    std::vector<std::string> events(const std::string& key) const;   ///< audit trail
    std::vector<std::string> commands(const std::string& key) const; ///< user actions taken

    // ---- act
    void startWork(const std::string& key, const std::string& itemId);
    void completeWork(const std::string& key, const std::string& itemId);
    void rejectWork(const std::string& key, const std::string& itemId, const std::string& reason);
    void escalateWork(const std::string& key, const std::string& itemId);
    void assignWork(const std::string& key, const std::string& itemId, const std::string& participantId);
    void setAvailable(const std::string& key, const std::string& participantId, bool available);

    /** @brief Switches the engine's live observer/notification lines on or off. */
    void showLiveMessages(bool show);

private:
    struct WorkflowType {
        WorkflowDefinition definition;
        std::string description;
        ParamList parameters;
        BuilderFactory factory;
    };
    struct Running {
        std::string key, workflowId;
        std::unique_ptr<WorkflowBuilder> builder; ///< owns the parts; must outlive the instance
        WorkFlowInstance* instance;
    };

    Running& find(const std::string& key) const;
    Running& findRunning(const std::string& key) const;
    WorkItem* findItem(Running& r, const std::string& itemId) const;
    WorkflowParticipant* findParticipant(Running& r, const std::string& id) const;
    static ItemInfo describe(WorkItem* item);

    std::vector<WorkflowType> types;
    std::vector<std::unique_ptr<Running> > running;
    std::map<std::string, int> counters;
};

#endif
