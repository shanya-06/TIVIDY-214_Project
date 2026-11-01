#ifndef GUIAPI_H
#define GUIAPI_H

/**
 * @file GuiApi.h
 * @brief JSON API between the browser GUI and the C++ engine.
 *
 * Every button in the GUI becomes one request here, and every request becomes
 * calls on WorkflowFacade (or on the CustomWorkflowLibrary for the designer).
 * The GUI never simulates the workflow itself: it only draws what this API
 * returns. Each action returns a report of what the engine did,
 * worked out by comparing the instance before and after the call.
 */

#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "CustomWorkflow.h"
#include "HttpServer.h"
#include "Industries.h"
#include "Json.h"
#include "WorkflowFacade.h"

class GuiApi {
public:
    GuiApi(WorkflowFacade& facade, CustomWorkflowLibrary& library,
           const std::vector<DemoPreset>& presets, const std::string& webRoot);
    ~GuiApi();
    GuiApi(const GuiApi&) = delete;
    GuiApi& operator=(const GuiApi&) = delete;

    HttpServer::Response handle(const HttpServer::Request& request);

private:
    struct Snapshot {
        WorkflowFacade::InstanceInfo info;
        std::vector<WorkflowFacade::ItemInfo> items;
        std::vector<WorkflowFacade::ParticipantInfo> people;
        std::size_t eventCount;
    };
    typedef std::function<void(const Json& body, Json& report)> Action;

    Json overview() const;
    Json instanceJson(const std::string& key) const;
    Json startInstance(const Json& body);
    Json runAction(const std::string& key, const std::string& action, const Json& body);
    Json saveWorkflow(const Json& body);
    Json checkWorkflow(const Json& body);
    Json deleteWorkflow(const std::string& id);
    Json workflowDesign(const std::string& id) const;
    HttpServer::Response staticFile(const std::string& path) const;


    Snapshot snapshot(const std::string& key) const;
    Json changes(const Snapshot& before, const Snapshot& after);
    std::string currentItem(const std::string& key) const;
    std::string workflowName(const std::string& id) const;

    WorkflowFacade& facade;
    CustomWorkflowLibrary& library;
    std::vector<DemoPreset> presets;
    std::string webRoot;
    std::ostringstream live; 
};

#endif
