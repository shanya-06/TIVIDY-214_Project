/**
 * @file gui_main.cpp
 * @brief Entry point of the TIVIDY graphical interface.
 *
 * Starts the C++ engine, serves the GUI on http://127.0.0.1:8214 and opens it in
 * the default browser. Every click in the browser is a request to this program,
 * which calls the WorkflowFacade; the browser only draws the results.
 *
 * Usage: ./tividy_gui [--port N] [--no-browser] [--web DIR] [--data FILE]
 */
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MAKE_DIR(p) mkdir(p, 0755)
#endif

#include "CustomWorkflow.h"
#include "GuiApi.h"
#include "HttpServer.h"
#include "Industries.h"
#include "WorkflowFacade.h"

namespace {
bool exists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

void openBrowser(const std::string& url) {
#if defined(_WIN32)
    std::string command = "start \"\" \"" + url + "\"";
#elif defined(__APPLE__)
    std::string command = "open \"" + url + "\" >/dev/null 2>&1 &";
#else
    std::string command = "xdg-open \"" + url + "\" >/dev/null 2>&1 &";
#endif
    if (std::system(command.c_str()) != 0)
        std::cout << "  (could not open a browser automatically; open the address above yourself)\n";
}
}

int main(int argc, char** argv) {
    int port = 8214;
    bool browser = true;
    std::string web, data = "data/custom_workflows.json";
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--port" && i + 1 < argc) port = std::atoi(argv[++i]);
        else if (a == "--no-browser") browser = false;
        else if (a == "--web" && i + 1 < argc) web = argv[++i];
        else if (a == "--data" && i + 1 < argc) data = argv[++i];
        else {
            std::cout << "Usage: " << argv[0] << " [--port N] [--no-browser] [--web DIR] [--data FILE]\n";
            return a == "--help" ? 0 : 1;
        }
    }
    if (web.empty()) web = exists("gui/index.html") ? "gui" : "../gui";
    if (!exists(web + "/index.html")) {
        std::cerr << "Cannot find the GUI files (gui/index.html). Run tividy_gui from the project folder.\n";
        return 1;
    }
    if (data.find('/') != std::string::npos) MAKE_DIR(data.substr(0, data.rfind('/')).c_str());

    WorkflowFacade facade;
    registerIndustries(facade);  

    CustomWorkflowLibrary library(facade, data);
    std::vector<std::string> warnings = library.loadSaved(); 
    for (std::size_t i = 0; i < warnings.size(); ++i) std::cerr << "  warning: " << warnings[i] << "\n";

    GuiApi api(facade, library, demoPresets(), web);
    HttpServer server(port);
    try {
        server.open();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    std::string url = "http://127.0.0.1:" + std::to_string(port) + "/";
    std::cout << "TIVIDY GUI is running at " << url << "\n"
              << "  " << library.designs().size() << " saved custom workflow(s) loaded from " << data << "\n"
              << "  Press Ctrl+C here to stop." << std::endl;
    if (browser) openBrowser(url);

    server.serve([&api](const HttpServer::Request& r) { return api.handle(r); });
    return 0;
}
