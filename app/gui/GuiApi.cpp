#include "GuiApi.h"

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

typedef WorkflowFacade F;

namespace {

std::vector<std::string> split(const std::string& path) {
    std::vector<std::string> parts;
    std::string part;
    std::istringstream in(path);
    while (std::getline(in, part, '/'))
        if (!part.empty()) parts.push_back(part);
    return parts;
}


std::string decode(const std::string& s) {
    std::string out;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            out += static_cast<char>(std::strtol(s.substr(i + 1, 2).c_str(), nullptr, 16));
            i += 2;
        } else {
            out += s[i];
        }
    }
    return out;
}

Json itemJson(const F::ItemInfo& i, std::size_t index) {
    Json j = Json::object();
    j.set("index", static_cast<int>(index)).set("id", i.id).set("name", i.name).set("role", i.role)
     .set("state", i.state).set("assignee", i.assignee).set("needsApproval", i.needsApproval)
     .set("awaitingApproval", i.awaitingApproval).set("inProgress", i.inProgress).set("finished", i.finished);
    return j;
}

Json personJson(const F::ParticipantInfo& p) {
    Json j = Json::object();
    j.set("id", p.id).set("name", p.name).set("role", p.role).set("available", p.available).set("workload", p.workload);
    return j;
}

Json stringsJson(const std::vector<std::string>& list) {
    Json j = Json::array();
    for (std::size_t i = 0; i < list.size(); ++i) j.push(list[i]);
    return j;
}

HttpServer::Response jsonResponse(const Json& body, int status = 200) {
    HttpServer::Response r;
    r.status = status;
    r.body = body.dump();
    return r;
}

Json failure(const std::string& message) {
    return Json::object().set("ok", false).set("error", message);
}

WorkItemSpec specFrom(const Json& j) {
    WorkItemSpec s;
    s.type = j["type"].str("task");
    s.id = j["id"].str();
    for (std::size_t i = 0; i < s.id.size(); ++i) s.id[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s.id[i])));
    s.name = j["name"].str();
    s.role = j["role"].str();
    s.needsApproval = j["approval"].boolean(false);
    s.priority = static_cast<int>(j["priority"].num(0));
    s.notify = j["notify"].str();
    s.deadlineMinutes = static_cast<int>(j["deadlineMinutes"].num(0));
    s.audit = j["audit"].boolean(false);
    return s;
}

} 

GuiApi::GuiApi(WorkflowFacade& f, CustomWorkflowLibrary& lib, const std::vector<DemoPreset>& p, const std::string& root)
    : facade(f), library(lib), presets(p), webRoot(root) {
    facade.sendLiveMessagesTo(&live);
}

GuiApi::~GuiApi() {
    facade.sendLiveMessagesTo(nullptr);
}



HttpServer::Response GuiApi::handle(const HttpServer::Request& request) {
    std::vector<std::string> parts = split(request.path);
    for (std::size_t i = 0; i < parts.size(); ++i) parts[i] = decode(parts[i]);
    if (parts.empty() || parts[0] != "api") return staticFile(request.path);

    const std::string& m = request.method;
    const std::size_t n = parts.size();
    live.str("");
    live.clear();
    try {
        Json body = request.body.empty() ? Json::object() : Json::parse(request.body);

        if (m == "GET" && n == 2 && parts[1] == "overview") return jsonResponse(overview());
        if (m == "GET" && n == 3 && parts[1] == "instances") return jsonResponse(instanceJson(parts[2]));
        if (m == "POST" && n == 2 && parts[1] == "instances") return jsonResponse(startInstance(body));
        if (m == "POST" && n == 4 && parts[1] == "instances") return jsonResponse(runAction(parts[2], parts[3], body));
        if (m == "POST" && n == 2 && parts[1] == "workflows") return jsonResponse(saveWorkflow(body));
        if (m == "POST" && n == 3 && parts[1] == "workflows" && parts[2] == "check") return jsonResponse(checkWorkflow(body));
        if (m == "GET" && n == 4 && parts[1] == "workflows" && parts[3] == "design") return jsonResponse(workflowDesign(parts[2]));
        if (m == "POST" && n == 4 && parts[1] == "workflows" && parts[3] == "delete") return jsonResponse(deleteWorkflow(parts[2]));
        return jsonResponse(failure("No such API endpoint: " + m + " " + request.path), 404);
    } catch (const std::exception& e) {
    
        return jsonResponse(failure(e.what()), 400);
    }
}

HttpServer::Response GuiApi::staticFile(const std::string& rawPath) const {
    std::string path = rawPath == "/" ? "/index.html" : rawPath;
    HttpServer::Response r;
    if (path.find("..") != std::string::npos) {
        r.status = 404;
        r.body = "Not found";
        return r;
    }
    std::ifstream in((webRoot + path).c_str(), std::ios::binary);
    if (!in) {
        r.status = 404;
        r.contentType = "text/plain; charset=utf-8";
        r.body = "Not found: " + path;
        return r;
    }
    std::stringstream data;
    data << in.rdbuf();
    r.body = data.str();

    static std::map<std::string, std::string> types;
    if (types.empty()) {
        types["html"] = "text/html; charset=utf-8";
        types["css"] = "text/css; charset=utf-8";
        types["js"] = "text/javascript; charset=utf-8";
        types["svg"] = "image/svg+xml";
        types["json"] = "application/json";
        types["woff2"] = "font/woff2";
        types["png"] = "image/png";
        types["ico"] = "image/x-icon";
    }
    std::string ext = path.substr(path.find_last_of('.') + 1);
    r.contentType = types.count(ext) ? types[ext] : "application/octet-stream";
    return r;
}



std::string GuiApi::workflowName(const std::string& id) const {
    std::vector<F::WorkflowInfo> all = facade.workflows();
    for (std::size_t i = 0; i < all.size(); ++i)
        if (all[i].id == id) return all[i].name;
    Json d = library.design(id);
    return d.isNull() ? id : d["name"].str(id);
}

std::string GuiApi::currentItem(const std::string& key) const {
    std::vector<F::ItemInfo> open = facade.availableWork(key);
    return open.empty() ? "" : open.front().id;
}

Json GuiApi::overview() const {
    Json defs = Json::array();
    std::vector<F::WorkflowInfo> all = facade.workflows();
    for (std::size_t i = 0; i < all.size(); ++i) {
        Json params = Json::array();
        for (std::size_t p = 0; p < all[i].parameters.size(); ++p)
            params.push(Json::object().set("name", all[i].parameters[p].first).set("default", all[i].parameters[p].second));
        Json tests = Json::array();
        for (std::size_t p = 0; p < presets.size(); ++p) {
            if (presets[p].workflowId != all[i].id) continue;
            Json values = Json::object();
            for (F::Params::const_iterator it = presets[p].params.begin(); it != presets[p].params.end(); ++it)
                values.set(it->first, it->second);
            tests.push(Json::object().set("label", presets[p].label).set("params", values));
        }
        defs.push(Json::object().set("id", all[i].id).set("name", all[i].name).set("description", all[i].description)
                      .set("parameters", params).set("presets", tests).set("custom", library.isCustom(all[i].id)));
    }

    Json instances = Json::array();
    std::vector<F::InstanceInfo> running = facade.instances();
    for (std::size_t i = 0; i < running.size(); ++i) instances.push(instanceJson(running[i].key));

    return Json::object().set("ok", true).set("definitions", defs).set("instances", instances);
}

Json GuiApi::instanceJson(const std::string& key) const {
    F::InstanceInfo info = facade.instance(key);
    Json items = Json::array();
    std::vector<F::ItemInfo> list = facade.items(key);
    for (std::size_t i = 0; i < list.size(); ++i) items.push(itemJson(list[i], i));
    Json people = Json::array();
    std::vector<F::ParticipantInfo> crew = facade.participants(key);
    for (std::size_t i = 0; i < crew.size(); ++i) people.push(personJson(crew[i]));

    Json j = Json::object();
    j.set("ok", true).set("key", key).set("workflow", info.workflow).set("workflowName", workflowName(info.workflow))
     .set("status", info.status).set("completed", info.completed).set("total", info.total)
     .set("current", currentItem(key)).set("items", items).set("participants", people)
     .set("events", stringsJson(facade.events(key))).set("commands", stringsJson(facade.commands(key)));
    return j;
}

GuiApi::Snapshot GuiApi::snapshot(const std::string& key) const {
    Snapshot s;
    s.info = facade.instance(key);
    s.items = facade.items(key);
    s.people = facade.participants(key);
    s.eventCount = facade.events(key).size();
    return s;
}

Json GuiApi::changes(const Snapshot& before, const Snapshot& after) {
    Json report = Json::object();
    report.set("statusFrom", before.info.status).set("statusTo", after.info.status);

    std::vector<std::string> events = facade.events(after.info.key);
    Json newEvents = Json::array();
    for (std::size_t i = before.eventCount; i < events.size(); ++i) newEvents.push(events[i]);
    report.set("events", newEvents);

    Json assigned = Json::array(), added = Json::array();
    for (std::size_t i = 0; i < after.items.size(); ++i) {
        const F::ItemInfo& now = after.items[i];
        const F::ItemInfo* was = nullptr;
        for (std::size_t j = 0; j < before.items.size(); ++j)
            if (before.items[j].id == now.id) was = &before.items[j];
        if (!was && !before.items.empty()) added.push(Json::object().set("id", now.id).set("name", now.name));
        if (!now.assignee.empty() && (!was || was->assignee != now.assignee))
            assigned.push(Json::object().set("id", now.id).set("name", now.name).set("who", now.assignee));
    }
    report.set("assigned", assigned).set("added", added);

    Json availability = Json::array();
    for (std::size_t i = 0; i < after.people.size() && i < before.people.size(); ++i)
        if (after.people[i].available != before.people[i].available)
            availability.push(Json::object().set("name", after.people[i].name).set("available", after.people[i].available));
    report.set("availability", availability);

    Json messages = Json::array();
    std::istringstream in(live.str());
    std::string line;
    while (std::getline(in, line))
        if (!line.empty()) messages.push(line);
    report.set("messages", messages);
    return report;
}


Json GuiApi::startInstance(const Json& body) {
    F::Params params;
    const Json& values = body["params"];
    for (std::size_t i = 0; i < values.keys().size(); ++i)
        params[values.keys()[i]] = values[values.keys()[i]].str();

    std::string key = facade.startWorkflow(body["workflow"].str(), params);

    Snapshot empty;
    empty.info.key = key;
    empty.info.completed = empty.info.total = 0;
    empty.eventCount = 0;
    Json report = changes(empty, snapshot(key));
    report.set("ok", true).set("label", "Started " + key);
    return Json::object().set("ok", true).set("key", key).set("report", report).set("instance", instanceJson(key));
}

Json GuiApi::runAction(const std::string& key, const std::string& action, const Json& body) {
  
    std::map<std::string, Action> actions;
    const std::string item = body["item"].str();

    actions["next"] = [&](const Json&, Json& report) {
        std::string id = currentItem(key);
        if (id.empty()) throw std::logic_error("Nothing to do: the instance is " + facade.instance(key).status);
        F::ItemInfo it = facade.item(key, id);
        if (it.assignee.empty())
            throw std::logic_error(id + " has nobody assigned. Assign someone, or bring a participant back from leave.");
        report.set("label", it.assignee + " works on " + id);
        if (!facade.item(key, id).inProgress) facade.startWork(key, id);
        F::ItemInfo now = facade.item(key, id);
        if (now.inProgress && !now.awaitingApproval) facade.completeWork(key, id);
    };
    actions["start"] = [&](const Json&, Json& report) {
        report.set("label", "Start " + item);
        facade.startWork(key, item);
    };
    actions["complete"] = [&](const Json&, Json& report) {
        report.set("label", "Complete " + item);
        facade.completeWork(key, item);
    };
    actions["reject"] = [&](const Json& b, Json& report) {
        report.set("label", "Reject " + item);
        facade.rejectWork(key, item, b["reason"].str());
    };
    actions["escalate"] = [&](const Json&, Json& report) {
        report.set("label", "Escalate " + item);
        facade.escalateWork(key, item);
    };
    actions["assign"] = [&](const Json& b, Json& report) {
        report.set("label", "Assign " + item + " to " + b["participant"].str());
        facade.assignWork(key, item, b["participant"].str());
    };
    actions["availability"] = [&](const Json& b, Json& report) {
        bool available = b["available"].boolean(true);
        std::string who = b["participant"].str();
        std::vector<F::ParticipantInfo> crew = facade.participants(key);
        for (std::size_t i = 0; i < crew.size(); ++i)
            if (crew[i].id == who) who = crew[i].name;
        report.set("label", who + (available ? " comes back" : " goes on leave"));
        facade.setAvailable(key, b["participant"].str(), available);
    };
    actions["cancel"] = [&](const Json&, Json& report) {
        report.set("label", "Cancel " + key);
        facade.cancel(key);
    };
    actions["insert"] = [&](const Json& b, Json& report) {
        WorkItemSpec spec = specFrom(b["newItem"]);
        report.set("label", "Add " + spec.id + " '" + spec.name + "' after " + b["after"].str());
        facade.insertWorkItem(key, b["after"].str(), spec);
    };

    std::map<std::string, Action>::iterator chosen = actions.find(action);
    if (chosen == actions.end()) throw std::invalid_argument("Unknown action: " + action);

    Snapshot before = snapshot(key);
    Json report = Json::object();
    report.set("label", action);
    bool ok = true;
    try {
        chosen->second(body, report);
    } catch (const std::exception& e) {
        ok = false;
        report.set("error", e.what());  
    }
    Json diff = changes(before, snapshot(key));
    for (std::size_t i = 0; i < diff.keys().size(); ++i) report.set(diff.keys()[i], diff[diff.keys()[i]]);
    report.set("ok", ok);
    return Json::object().set("ok", ok).set("report", report).set("instance", instanceJson(key));
}



Json GuiApi::checkWorkflow(const Json& body) {
    return Json::object().set("ok", true).set("design", library.validate(body));
}

Json GuiApi::saveWorkflow(const Json& body) {
    Json design = library.add(body);
    return Json::object().set("ok", true).set("design", design);
}

Json GuiApi::deleteWorkflow(const std::string& id) {
    library.remove(id);
    return Json::object().set("ok", true);
}

Json GuiApi::workflowDesign(const std::string& id) const {
    Json d = library.design(id);
    if (d.isNull()) throw std::invalid_argument(id + " is a built-in workflow; its design lives in app/Industries.cpp");
    return Json::object().set("ok", true).set("design", d);
}
