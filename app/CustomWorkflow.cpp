#include "CustomWorkflow.h"
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include "CustomWorkItemProcessor.h"
#include "DefaultWorkItemProcessor.h"
#include "ExternalCallProcessor.h"
#include "FinalApprovalHandler.h"
#include "FirstLevelApprovalHandler.h"
#include "IndustryBuilder.h"
#include "RoleBasedAssignmentStrategy.h"
#include "RuleBasedAssignmentStrategy.h"
#include "SecondLevelApprovalHandler.h"
#include "WorkflowDefinition.h"
#include "WorkflowFacade.h"
#include "WorkloadAssignmentStrategy.h"

namespace {

typedef WorkflowFacade::Params Params;


struct ItemDesign {
    WorkItemSpec spec;
    std::string automatic; 
};

struct RouteDesign {
    std::string after, parameter, op, value;
    WorkItemSpec insert;
};

struct Design {
    std::string id, name, description;
    WorkflowFacade::ParamList parameters;
    std::vector<std::vector<std::string> > participants; 
    std::string strategy, processor;
    int maxWorkload;
    std::string approvalParameter;
    double level1Max, level2Max;
    bool approveOverLimit;
    std::vector<ItemDesign> items;
    std::vector<RouteDesign> routes;
};



std::string trim(const std::string& s) {
    std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}


std::string makeId(const std::string& text) {
    std::string out;
    for (std::size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (std::isalnum(c)) out += static_cast<char>(std::toupper(c));
        else if (!out.empty() && out[out.size() - 1] != '-') out += '-';
    }
    while (!out.empty() && out[out.size() - 1] == '-') out.erase(out.size() - 1);
    return out.substr(0, 24);
}

bool isNumber(const std::string& text, double& value) {
    std::string t = trim(text);
    if (t.empty()) return false;
    char* end = nullptr;
    value = std::strtod(t.c_str(), &end);
    return end && *end == '\0';
}


bool holds(const std::string& left, const std::string& op, const std::string& right) {
    double a = 0, b = 0;
    if (isNumber(left, a) && isNumber(right, b)) {
        static std::map<std::string, std::function<bool(double, double)> > numeric;
        if (numeric.empty()) {
            numeric["<"] = [](double x, double y) { return x < y; };
            numeric["<="] = [](double x, double y) { return x <= y; };
            numeric[">"] = [](double x, double y) { return x > y; };
            numeric[">="] = [](double x, double y) { return x >= y; };
            numeric["=="] = [](double x, double y) { return x == y; };
            numeric["!="] = [](double x, double y) { return x != y; };
        }
        return numeric.at(op)(a, b);
    }
    if (op == "==") return trim(left) == trim(right);
    if (op == "!=") return trim(left) != trim(right);
    return false;
}

const std::set<std::string>& operators() {
    static std::set<std::string> ops;
    if (ops.empty()) { ops.insert("<"); ops.insert("<="); ops.insert(">"); ops.insert(">="); ops.insert("=="); ops.insert("!="); }
    return ops;
}


typedef std::function<AssignmentStrategy*(int)> StrategyFactory;
const std::map<std::string, StrategyFactory>& strategyTable() {
    static std::map<std::string, StrategyFactory> table;
    if (table.empty()) {
        table["workload"] = [](int) -> AssignmentStrategy* { return new WorkloadAssignmentStrategy(); };
        table["role"] = [](int) -> AssignmentStrategy* { return new RoleBasedAssignmentStrategy(); };
        table["rule"] = [](int max) -> AssignmentStrategy* { return new RuleBasedAssignmentStrategy(max); };
    }
    return table;
}

typedef std::function<WorkItemProcessor*()> ProcessorFactory;
const std::map<std::string, ProcessorFactory>& processorTable() {
    static std::map<std::string, ProcessorFactory> table;
    if (table.empty()) {
        table["default"] = []() -> WorkItemProcessor* { return new DefaultWorkItemProcessor(); };
        table["custom"] = []() -> WorkItemProcessor* { return new CustomWorkItemProcessor(); };
    }
    return table;
}

const std::set<std::string>& itemTypes() {
    static std::set<std::string> types;
    if (types.empty()) { types.insert("task"); types.insert("approval"); types.insert("subprocess"); }
    return types;
}

void need(bool condition, const std::string& message) {
    if (!condition) throw std::invalid_argument(message);
}



WorkItemSpec readItem(const Json& j) {
    WorkItemSpec s;
    s.type = trim(j["type"].str("task"));
    s.id = makeId(j["id"].str());
    s.name = trim(j["name"].str());
    s.role = trim(j["role"].str());
    s.needsApproval = j["approval"].boolean(false) || s.type == "approval";
    s.priority = static_cast<int>(j["priority"].num(0));
    s.notify = trim(j["notify"].str());
    s.deadlineMinutes = static_cast<int>(j["deadlineMinutes"].num(0));
    s.audit = j["audit"].boolean(false);
    return s;
}

Json writeItem(const WorkItemSpec& s) {
    Json j = Json::object();
    j.set("type", s.type).set("id", s.id).set("name", s.name).set("role", s.role)
     .set("approval", s.needsApproval).set("priority", s.priority).set("notify", s.notify)
     .set("deadlineMinutes", s.deadlineMinutes).set("audit", s.audit);
    return j;
}

Design readDesign(const Json& j) {
    Design d;
    d.name = trim(j["name"].str());
    d.id = makeId(j["id"].str().empty() ? d.name : j["id"].str());
    d.description = trim(j["description"].str());
    for (std::size_t i = 0; i < j["parameters"].size(); ++i) {
        const Json& p = j["parameters"][i];
        d.parameters.push_back(std::make_pair(trim(p["name"].str()), trim(p["default"].str())));
    }
    for (std::size_t i = 0; i < j["participants"].size(); ++i) {
        const Json& p = j["participants"][i];
        std::vector<std::string> person;
        person.push_back(trim(p["id"].str()));
        person.push_back(trim(p["name"].str()));
        person.push_back(trim(p["role"].str()));
        d.participants.push_back(person);
    }
    d.strategy = j["strategy"].str("workload");
    d.maxWorkload = static_cast<int>(j["maxWorkload"].num(3));
    d.processor = j["processor"].str("default");
    const Json& a = j["approval"];
    d.approvalParameter = trim(a["parameter"].str());
    d.level1Max = a["level1Max"].num(0);
    d.level2Max = a["level2Max"].num(0);
    d.approveOverLimit = a["overLimit"].str("reject") == "approve";
    for (std::size_t i = 0; i < j["items"].size(); ++i) {
        ItemDesign item;
        item.spec = readItem(j["items"][i]);
        item.automatic = trim(j["items"][i]["automatic"].str());
        d.items.push_back(item);
    }
    for (std::size_t i = 0; i < j["routes"].size(); ++i) {
        const Json& r = j["routes"][i];
        RouteDesign route;
        route.after = makeId(r["after"].str());
        route.parameter = trim(r["parameter"].str());
        route.op = trim(r["op"].str(">"));
        route.value = trim(r["value"].str());
        route.insert = readItem(r["insert"]);
        d.routes.push_back(route);
    }
    return d;
}

Json writeDesign(const Design& d) {
    Json j = Json::object();
    j.set("id", d.id).set("name", d.name).set("description", d.description).set("version", 1);
    Json params = Json::array();
    for (std::size_t i = 0; i < d.parameters.size(); ++i)
        params.push(Json::object().set("name", d.parameters[i].first).set("default", d.parameters[i].second));
    j.set("parameters", params);
    Json people = Json::array();
    for (std::size_t i = 0; i < d.participants.size(); ++i)
        people.push(Json::object().set("id", d.participants[i][0]).set("name", d.participants[i][1]).set("role", d.participants[i][2]));
    j.set("participants", people);
    j.set("strategy", d.strategy).set("maxWorkload", d.maxWorkload).set("processor", d.processor);
    j.set("approval", Json::object().set("parameter", d.approvalParameter).set("level1Max", d.level1Max)
                          .set("level2Max", d.level2Max).set("overLimit", d.approveOverLimit ? "approve" : "reject"));
    Json items = Json::array();
    for (std::size_t i = 0; i < d.items.size(); ++i) items.push(writeItem(d.items[i].spec).set("automatic", d.items[i].automatic));
    j.set("items", items);
    Json routes = Json::array();
    for (std::size_t i = 0; i < d.routes.size(); ++i) {
        const RouteDesign& r = d.routes[i];
        routes.push(Json::object().set("after", r.after).set("parameter", r.parameter).set("op", r.op)
                        .set("value", r.value).set("insert", writeItem(r.insert)));
    }
    j.set("routes", routes);
    return j;
}


void normalise(Design& d) {
    need(!d.name.empty(), "Give the workflow a name");
    need(!d.id.empty(), "The workflow name must contain letters or digits");

    std::set<std::string> paramNames;
    for (std::size_t i = 0; i < d.parameters.size(); ++i) {
        const std::string& n = d.parameters[i].first;
        need(!n.empty(), "Input field " + std::to_string(i + 1) + " needs a name");
        for (std::size_t c = 0; c < n.size(); ++c)
            need(std::isalnum(static_cast<unsigned char>(n[c])) || n[c] == '_',
                 "Input field '" + n + "' may only use letters, digits and _");
        need(paramNames.insert(n).second, "There are two input fields called '" + n + "'");
    }

    need(!d.participants.empty(), "Add at least one participant");
    std::set<std::string> ids, roles;
    for (std::size_t i = 0; i < d.participants.size(); ++i) {
        std::vector<std::string>& p = d.participants[i];
        if (p[0].empty()) p[0] = "p" + std::to_string(i + 1);
        need(!p[1].empty(), "Participant " + std::to_string(i + 1) + " needs a name");
        need(!p[2].empty(), p[1] + " needs a role");
        need(ids.insert(p[0]).second, "Two participants share the ID '" + p[0] + "'");
        roles.insert(p[2]);
    }

    need(strategyTable().count(d.strategy) == 1, "Unknown assignment strategy '" + d.strategy + "'");
    need(processorTable().count(d.processor) == 1, "Unknown processor '" + d.processor + "'");
    need(d.maxWorkload >= 0, "Maximum workload cannot be negative");

    if (!d.approvalParameter.empty()) {
        need(paramNames.count(d.approvalParameter) == 1,
             "Approval limits use '" + d.approvalParameter + "', which is not one of the input fields");
        need(d.level1Max <= d.level2Max, "The first approval limit must not be higher than the second");
    }

    need(!d.items.empty(), "Add at least one work item");
    std::set<std::string> itemIds;
    for (std::size_t i = 0; i < d.items.size(); ++i) {
        WorkItemSpec& s = d.items[i].spec;
        if (s.id.empty()) s.id = "S" + std::to_string(i + 1);
        need(itemTypes().count(s.type) == 1, "Unknown work item type '" + s.type + "'");
        need(!s.name.empty(), "Work item " + s.id + " needs a name");
        need(roles.count(s.role) == 1, "Nobody has the role '" + s.role + "' needed by " + s.id + " '" + s.name + "'");
        need(itemIds.insert(s.id).second, "Two work items share the ID '" + s.id + "'");
        need(s.priority >= 0 && s.deadlineMinutes >= 0, "Priority and deadline cannot be negative");
    }

    for (std::size_t i = 0; i < d.routes.size(); ++i) {
        RouteDesign& r = d.routes[i];
        std::string label = "Routing rule " + std::to_string(i + 1);
        need(itemIds.count(r.after) == 1, label + ": choose the work item it follows");
        need(paramNames.count(r.parameter) == 1, label + ": '" + r.parameter + "' is not one of the input fields");
        need(operators().count(r.op) == 1, label + ": unknown comparison '" + r.op + "'");
        if (r.insert.id.empty()) r.insert.id = "R" + std::to_string(i + 1);
        need(itemTypes().count(r.insert.type) == 1, label + ": unknown work item type '" + r.insert.type + "'");
        need(!r.insert.name.empty(), label + ": the added step needs a name");
        need(roles.count(r.insert.role) == 1, label + ": nobody has the role '" + r.insert.role + "'");
        need(itemIds.insert(r.insert.id).second, label + ": the ID '" + r.insert.id + "' is already used");
    }
}



/**
 * @brief Concrete Builder that assembles an instance from a user's design
 * (Gang of Four pattern: Builder). Same steps the Director (WorkflowDefinition)
 * calls on the hand-written industry builders.
 */
class ConfigurableBuilder : public IndustryBuilder {
public:
    ConfigurableBuilder(const Design& design, const Params& values) : design(design), values(values) {
        if (!design.approvalParameter.empty()) {
            double v = 0;
            if (!isNumber(values.at(design.approvalParameter), v))
                throw std::invalid_argument(design.approvalParameter + " must be a number (it decides the approval level)");
            approvalValue = v;
        } else {
            approvalValue = 0;
        }
    }

    void addParticipants() override {
        for (std::size_t i = 0; i < design.participants.size(); ++i)
            hire(design.participants[i][0], design.participants[i][1], design.participants[i][2]);
    }

    void addCoordination() override {
        useStrategy(strategyTable().at(design.strategy)(design.maxWorkload));
        useProcessor(processorTable().at(design.processor)());

        const double value = approvalValue;
        const bool limited = !design.approvalParameter.empty();
        const double l1 = design.level1Max, l2 = design.level2Max;
        const bool approveOver = design.approveOverLimit;
        const std::string field = design.approvalParameter;
        useApprovalChain(
            new FirstLevelApprovalHandler([=](const WorkItem&) { return !limited || value <= l1; },
                                          [](WorkItem& i) { i.notifyObservers("first level approves"); i.approve(); }),
            new SecondLevelApprovalHandler([=](const WorkItem&) { return value <= l2; },
                                           [](WorkItem& i) { i.notifyObservers("second level approves"); i.approve(); }),
            new FinalApprovalHandler([](const WorkItem&) { return true; },
                                     [=](WorkItem& i) {
                                         if (approveOver) { i.notifyObservers("final level approves"); i.approve(); }
                                         else { i.notifyObservers("final level declines"); i.reject(field + " is over the limit"); }
                                     }));
    }

    void addWorkItems() override {
        for (std::size_t i = 0; i < design.items.size(); ++i) {
            const ItemDesign& d = design.items[i];
            WorkItem* item = create(d.spec);
            if (!d.automatic.empty()) {
                std::map<std::string, std::string> payload(values.begin(), values.end());
                useProcessorFor(item, new ExternalCallProcessor(mediator, d.automatic, payload));
            }
            placed[d.spec.id] = add(item);
        }

        if (design.routes.empty()) return;
        instance->setRoutingRule([this](WorkFlowInstance&, WorkItem* done) {
            for (std::size_t i = 0; i < design.routes.size(); ++i) {
                const RouteDesign& r = design.routes[i];
                if (done->getId() != r.after || placed.count(r.insert.id)) continue;
                const std::string actual = values.at(r.parameter);
                if (!holds(actual, r.op, r.value)) continue;
                WorkItem* extra = create(r.insert);
                insertAfter(placed.at(r.after), extra);
                placed[r.insert.id] = extra;
                extra->notifyObservers("routing: " + r.parameter + " " + r.op + " " + r.value + " (it is " + actual + "), step added");
            }
        });
    }

private:
    Design design;
    Params values;
    double approvalValue;
    std::map<std::string, WorkItem*> placed;
};

}  



CustomWorkflowLibrary::CustomWorkflowLibrary(WorkflowFacade& f, const std::string& path) : facade(f), file(path) {}

Json CustomWorkflowLibrary::validate(const Json& raw) const {
    Design d = readDesign(raw);
    normalise(d);
    std::vector<WorkflowFacade::WorkflowInfo> existing = facade.workflows();
    for (std::size_t i = 0; i < existing.size(); ++i)
        need(existing[i].id != d.id, "A workflow with the ID " + d.id + " already exists. Choose another name.");

  
    Params defaults(d.parameters.begin(), d.parameters.end());
    ConfigurableBuilder trial(d, defaults);
    WorkflowDefinition(d.id, d.name, 1).createInstance(trial, d.id + "-TRIAL");
    return writeDesign(d);
}

void CustomWorkflowLibrary::registerDesign(const Json& normalised) {
    Design d = readDesign(normalised);
    normalise(d);
    std::string description = d.description.empty() ? "Designed in the line planner." : d.description;
    facade.addWorkflow(WorkflowDefinition(d.id, d.name, 1), description, d.parameters,
                       [d](const Params& values) -> WorkflowBuilder* { return new ConfigurableBuilder(d, values); });
}

Json CustomWorkflowLibrary::add(const Json& raw) {
    Json normalised = validate(raw);
    registerDesign(normalised);
    saved.push_back(normalised);
    save();
    return normalised;
}

void CustomWorkflowLibrary::remove(const std::string& workflowId) {
    for (std::size_t i = 0; i < saved.size(); ++i) {
        if (saved[i]["id"].str() == workflowId) {
            facade.removeWorkflow(workflowId);
            saved.erase(saved.begin() + static_cast<long>(i));
            save();
            return;
        }
    }
    throw std::invalid_argument(workflowId + " is not a custom workflow (built-in ones cannot be removed)");
}

bool CustomWorkflowLibrary::isCustom(const std::string& workflowId) const {
    return !design(workflowId).isNull();
}

Json CustomWorkflowLibrary::design(const std::string& workflowId) const {
    for (std::size_t i = 0; i < saved.size(); ++i)
        if (saved[i]["id"].str() == workflowId) return saved[i];
    return Json();
}

std::vector<std::string> CustomWorkflowLibrary::loadSaved() {
    std::vector<std::string> warnings;
    if (file.empty()) return warnings;
    std::ifstream in(file.c_str());
    if (!in) return warnings;  
    std::stringstream text;
    text << in.rdbuf();
    Json all;
    try {
        all = Json::parse(text.str());
    } catch (const std::exception& e) {
        warnings.push_back(file + " could not be read: " + e.what());
        return warnings;
    }
    for (std::size_t i = 0; i < all.size(); ++i) {
        try {
            Json normalised = validate(all[i]);
            registerDesign(normalised);
            saved.push_back(normalised);
        } catch (const std::exception& e) {
            warnings.push_back("Skipped saved workflow '" + all[i]["name"].str() + "': " + e.what());
        }
    }
    return warnings;
}

void CustomWorkflowLibrary::save() const {
    if (file.empty()) return;
    Json all = Json::array();
    for (std::size_t i = 0; i < saved.size(); ++i) all.push(saved[i]);
    std::ofstream out(file.c_str());
    if (out) out << all.dump() << "\n";
}
