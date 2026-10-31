
var workflows = [];   
var currentKey = null;  
var currentInstance = null;
var design = null;   

function esc(text) {
  if (text === undefined || text === null) return "";
  return String(text)
    .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;").replace(/'/g, "&#39;");
}

async function api(method, url, body) {
  var options = { method: method, headers: { "Content-Type": "application/json" } };
  if (body !== undefined) options.body = JSON.stringify(body);
  try {
    var response = await fetch(url, options);
    return await response.json();
  } catch (e) {
    return { ok: false, error: "Could not connect to the server. Is tividy_gui running?" };
  }
}

function showMessage(text, isError) {
  var box = document.getElementById("message");
  box.textContent = text;
  box.className = isError ? "error" : "";
}

function setContent(html) {
  document.getElementById("content").innerHTML = html;
}

function findWorkflow(id) {
  for (var i = 0; i < workflows.length; i++) {
    if (workflows[i].id === id) return workflows[i];
  }
  return null;
}

async function showHome() {
  currentKey = null;
  var data = await api("GET", "/api/overview");
  if (!data.ok) {
    showMessage(data.error, true);
    setContent("");
    return;
  }
  workflows = data.definitions;

  var html = "<h2>Start a new instance</h2>";
  html += "<div class='form-row'><label>Workflow:</label><select id='workflowSelect' onchange='fillParameters()'>";
  for (var i = 0; i < workflows.length; i++) {
    html += "<option value='" + esc(workflows[i].id) + "'>" + esc(workflows[i].name) + "</option>";
  }
  html += "</select></div>";
  html += "<div class='form-row'><label>Test case:</label><select id='testCaseSelect' onchange='useTestCase()'></select></div>";
  html += "<div id='parameters'></div>";
  html += "<button onclick='startInstance()'>Start Instance</button>";

  html += "<h2>Instances</h2>";
  if (data.instances.length === 0) {
    html += "<p>No instances yet.</p>";
  } else {
    html += "<table><tr><th>Instance</th><th>Workflow</th><th>Status</th><th>Progress</th><th>Current item</th><th></th></tr>";
    for (var j = 0; j < data.instances.length; j++) {
      var inst = data.instances[j];
      html += "<tr><td>" + esc(inst.key) + "</td><td>" + esc(inst.workflowName) + "</td><td>" + esc(inst.status) + "</td>" +
              "<td>" + inst.completed + "/" + inst.total + "</td><td>" + esc(inst.current || "-") + "</td>" +
              "<td><button onclick=\"openInstance('" + esc(inst.key) + "')\">Open</button></td></tr>";
    }
    html += "</table>";
  }

  html += "<h2>Workflow definitions</h2>";
  html += "<table><tr><th>ID</th><th>Name</th><th>Description</th><th>Type</th><th></th></tr>";
  for (var k = 0; k < workflows.length; k++) {
    var w = workflows[k];
    html += "<tr><td>" + esc(w.id) + "</td><td>" + esc(w.name) + "</td><td>" + esc(w.description) + "</td>" +
            "<td>" + (w.custom ? "Custom" : "Built-in") + "</td><td>" +
            (w.custom ? "<button onclick=\"deleteWorkflow('" + esc(w.id) + "')\">Delete</button>" : "") + "</td></tr>";
  }
  html += "</table>";

  setContent(html);
  fillParameters();
}


function fillParameters() {
  var workflow = findWorkflow(document.getElementById("workflowSelect").value);
  if (!workflow) return;

  var tests = "<option value=''>-- none --</option>";
  for (var i = 0; i < workflow.presets.length; i++) {
    tests += "<option value='" + i + "'>" + esc(workflow.presets[i].label) + "</option>";
  }
  document.getElementById("testCaseSelect").innerHTML = tests;

  var html = "";
  for (var j = 0; j < workflow.parameters.length; j++) {
    var p = workflow.parameters[j];
    html += "<div class='form-row'><label>" + esc(p.name) + ":</label>" +
            "<input type='text' id='param_" + esc(p.name) + "' value='" + esc(p.default) + "'></div>";
  }
  document.getElementById("parameters").innerHTML = html;
}


function useTestCase() {
  var workflow = findWorkflow(document.getElementById("workflowSelect").value);
  var index = document.getElementById("testCaseSelect").value;
  if (!workflow || index === "") return;
  var values = workflow.presets[Number(index)].params;
  for (var name in values) {
    var input = document.getElementById("param_" + name);
    if (input) input.value = values[name];
  }
}

async function startInstance() {
  var workflow = findWorkflow(document.getElementById("workflowSelect").value);
  var params = {};
  for (var i = 0; i < workflow.parameters.length; i++) {
    var name = workflow.parameters[i].name;
    params[name] = document.getElementById("param_" + name).value;
  }
  var data = await api("POST", "/api/instances", { workflow: workflow.id, params: params });
  if (!data.ok) {
    showMessage("Error: " + data.error, true);
    return;
  }
  showMessage("Started " + data.key);
  showInstance(data.instance);
}

async function deleteWorkflow(id) {
  if (!confirm("Delete workflow " + id + "?")) return;
  var data = await api("POST", "/api/workflows/" + encodeURIComponent(id) + "/delete", {});
  if (data.ok) showMessage("Deleted " + id);
  else showMessage("Error: " + data.error, true);
  showHome();
}


async function openInstance(key) {
  var data = await api("GET", "/api/instances/" + encodeURIComponent(key));
  if (!data.ok) {
    showMessage("Error: " + data.error, true);
    return;
  }
  showMessage("");
  showInstance(data);
}

function showInstance(inst) {
  currentKey = inst.key;
  currentInstance = inst;
  var running = inst.status === "RUNNING";
  var disabled = running ? "" : " disabled";

  var html = "<h2>" + esc(inst.key) + " (" + esc(inst.workflowName) + ")</h2>";
  html += "<p>Status: <b>" + esc(inst.status) + "</b><br>";
  html += "Progress: " + inst.completed + " of " + inst.total + " work items completed<br>";
  html += "Current work item: " + esc(inst.current || "none") + "</p>";
  html += "<button onclick='doAction(\"next\", {})'" + disabled + ">Do Next Step</button>";
  html += "<button onclick='cancelInstance()'" + disabled + ">Cancel Instance</button>";
  html += "<button onclick='showMessage(\"\"); showHome()'>Back</button>";


  html += "<h2>Work items</h2>";
  html += "<table><tr><th>#</th><th>ID</th><th>Name</th><th>Role</th><th>State</th><th>Assigned to</th><th>Approval</th><th>Actions</th></tr>";
  for (var i = 0; i < inst.items.length; i++) {
    var item = inst.items[i];
    var id = esc(item.id);
    html += "<tr><td>" + (i + 1) + "</td><td>" + id + "</td><td>" + esc(item.name) + "</td><td>" + esc(item.role) + "</td>" +
            "<td>" + esc(item.state) + "</td><td>" + esc(item.assignee || "-") + "</td>" +
            "<td>" + (item.needsApproval ? "Yes" : "No") + "</td><td>" +
            "<button onclick=\"itemAction('start', '" + id + "')\"" + disabled + ">Start</button>" +
            "<button onclick=\"itemAction('complete', '" + id + "')\"" + disabled + ">Complete</button>" +
            "<button onclick=\"itemAction('escalate', '" + id + "')\"" + disabled + ">Escalate</button>" +
            "<button onclick=\"rejectItem('" + id + "')\"" + disabled + ">Reject</button></td></tr>";
  }
  html += "</table>";

  html += "<div class='form-row'><label>Assign item:</label><select id='assignItem'>";
  for (var a = 0; a < inst.items.length; a++) {
    html += "<option value='" + esc(inst.items[a].id) + "'" + (inst.items[a].id === inst.current ? " selected" : "") + ">" +
            esc(inst.items[a].id) + "</option>";
  }
  html += "</select> to <select id='assignPerson'>";
  for (var b = 0; b < inst.participants.length; b++) {
    var p = inst.participants[b];
    html += "<option value='" + esc(p.id) + "'>" + esc(p.name) + " - " + esc(p.role) + "</option>";
  }
  html += "</select> <button onclick='assignItem()'" + disabled + ">Assign</button></div>";

  
  html += "<h2>Participants</h2>";
  html += "<table><tr><th>ID</th><th>Name</th><th>Role</th><th>Available</th><th>Workload</th><th></th></tr>";
  for (var c = 0; c < inst.participants.length; c++) {
    var person = inst.participants[c];
    html += "<tr><td>" + esc(person.id) + "</td><td>" + esc(person.name) + "</td><td>" + esc(person.role) + "</td>" +
            "<td>" + (person.available ? "Yes" : "No (on leave)") + "</td><td>" + person.workload + "</td><td>" +
            "<button onclick=\"setAvailable('" + esc(person.id) + "', " + (!person.available) + ")\"" + disabled + ">" +
            (person.available ? "Set on leave" : "Set available") + "</button></td></tr>";
  }
  html += "</table>";


  var roles = [];
  for (var r = 0; r < inst.participants.length; r++) {
    if (roles.indexOf(inst.participants[r].role) === -1) roles.push(inst.participants[r].role);
  }
  html += "<h2>Add a work item</h2>";
  html += "<p class='small'>The new item is added after the chosen item and runs when that item is completed. It can only be added after the current work.</p>";
  html += "<div class='form-row'><label>Add after:</label><select id='newAfter'>";
  for (var d = 0; d < inst.items.length; d++) {
    html += "<option value='" + esc(inst.items[d].id) + "'" + (inst.items[d].id === inst.current ? " selected" : "") + ">" +
            esc(inst.items[d].id) + " " + esc(inst.items[d].name) + "</option>";
  }
  html += "</select></div>";
  html += "<div class='form-row'><label>ID:</label><input type='text' id='newId' value='X" + (inst.items.length + 1) + "'></div>";
  html += "<div class='form-row'><label>Name:</label><input type='text' id='newName'></div>";
  html += "<div class='form-row'><label>Type:</label><select id='newType'><option value='task'>Task</option>" +
          "<option value='approval'>Approval</option><option value='subprocess'>Subprocess</option></select></div>";
  html += "<div class='form-row'><label>Role:</label><select id='newRole'>";
  for (var e = 0; e < roles.length; e++) html += "<option>" + esc(roles[e]) + "</option>";
  html += "</select></div>";
  html += "<div class='form-row'><label>Priority (0 = none):</label><input type='number' id='newPriority' value='0' min='0'></div>";
  html += "<div class='form-row'><label>Notify (optional):</label><input type='text' id='newNotify'></div>";
  html += "<button onclick='addWorkItem()'" + disabled + ">Add Work Item</button>";


  html += "<h2>History</h2>";
  html += "<p><b>Commands executed:</b> " + (inst.commands.length ? esc(inst.commands.join(", ")) : "none") + "</p>";
  html += "<table><tr><th>#</th><th>Event</th></tr>";
  for (var h = 0; h < inst.events.length; h++) {
    html += "<tr><td>" + (h + 1) + "</td><td>" + esc(inst.events[h]) + "</td></tr>";
  }
  html += "</table>";

  setContent(html);
}


async function doAction(action, body) {
  var data = await api("POST", "/api/instances/" + encodeURIComponent(currentKey) + "/" + action, body);
  if (!data.report) {
    showMessage("Error: " + data.error, true);
    return;
  }
  var report = data.report;
  if (report.ok) {
    var text = "Done: " + report.label + ".";
    if (report.statusFrom !== report.statusTo) text += " Instance is now " + report.statusTo + ".";
    for (var i = 0; i < report.added.length; i++) text += " New work item: " + report.added[i].id + ".";
    showMessage(text);
  } else {
    showMessage("Error: " + report.error, true);
  }
  showInstance(data.instance);
}

function itemAction(action, itemId) {
  doAction(action, { item: itemId });
}

function rejectItem(itemId) {
  var reason = prompt("Reason for rejecting " + itemId + ":");
  if (reason === null) return;
  doAction("reject", { item: itemId, reason: reason });
}

function assignItem() {
  doAction("assign", {
    item: document.getElementById("assignItem").value,
    participant: document.getElementById("assignPerson").value
  });
}

function setAvailable(participantId, available) {
  doAction("availability", { participant: participantId, available: available });
}

function cancelInstance() {
  if (confirm("Cancel " + currentKey + "?")) doAction("cancel", {});
}

function addWorkItem() {
  doAction("insert", {
    after: document.getElementById("newAfter").value,
    newItem: {
      id: document.getElementById("newId").value,
      name: document.getElementById("newName").value,
      type: document.getElementById("newType").value,
      role: document.getElementById("newRole").value,
      priority: Number(document.getElementById("newPriority").value),
      notify: document.getElementById("newNotify").value
    }
  });
}

function emptyDesign() {
  return {
    name: "", description: "",
    fields: [{ name: "amount", value: "1000" }],
    people: [{ name: "", role: "" }],
    items: [{ type: "task", name: "", role: "", priority: 0, notify: "", automatic: "" }],
    strategy: "workload", maxWorkload: 3, processor: "default",
    approvalField: "", level1: 0, level2: 0, overLimit: "reject",
    rules: []
  };
}

function exampleDesign() {
  return {
    name: "Hospital admission",
    description: "Triage, doctor assessment, admission approval, medical aid check, bed allocation.",
    fields: [{ name: "patient", value: "J. Dlamini" }, { name: "days", value: "3" }, { name: "medicalAid", value: "MA-55120" }],
    people: [{ name: "Nurse Zulu", role: "nurse" }, { name: "Nurse Patel", role: "nurse" },
             { name: "Dr Venter", role: "doctor" }, { name: "Ward clerk", role: "clerk" }],
    items: [
      { type: "task", name: "Triage patient", role: "nurse", priority: 2, notify: "", automatic: "" },
      { type: "task", name: "Doctor assessment", role: "doctor", priority: 0, notify: "family", automatic: "" },
      { type: "approval", name: "Approve admission", role: "doctor", priority: 0, notify: "", automatic: "" },
      { type: "task", name: "Verify medical aid", role: "clerk", priority: 0, notify: "", automatic: "checkMedicalAid" },
      { type: "subprocess", name: "Allocate bed", role: "nurse", priority: 0, notify: "", automatic: "" }
    ],
    strategy: "workload", maxWorkload: 3, processor: "custom",
    approvalField: "days", level1: 2, level2: 10, overLimit: "reject",
    rules: [{ after: 1, field: "days", op: ">", value: "5", name: "Specialist consult", role: "doctor" }]
  };
}

function showDesigner() {
  if (design === null) design = emptyDesign();
  showMessage("");
  drawDesigner();
}


function textInput(path, value, size) {
  return "<input type='text' size='" + (size || 15) + "' value='" + esc(value) + "' onchange=\"" + path + " = this.value\">";
}

function selectInput(path, value, options) {
  var html = "<select onchange=\"" + path + " = this.value\">";
  for (var i = 0; i < options.length; i++) {
    html += "<option value='" + esc(options[i][0]) + "'" + (String(options[i][0]) === String(value) ? " selected" : "") + ">" +
            esc(options[i][1]) + "</option>";
  }
  return html + "</select>";
}

function drawDesigner() {
  var d = design;
  var types = [["task", "Task"], ["approval", "Approval"], ["subprocess", "Subprocess"]];
  var html = "<h2>Create a new workflow</h2>";
  html += "<p class='small'>The new workflow runs on the same engine as the built-in ones. " +
          "Save registers it with the C++ engine and stores it in data/custom_workflows.json.</p>";
  html += "<button onclick='design = exampleDesign(); drawDesigner()'>Load Example</button>";
  html += "<button onclick='design = emptyDesign(); drawDesigner()'>Clear</button>";

  html += "<h3>1. Details</h3>";
  html += "<div class='form-row'><label>Name:</label>" + textInput("design.name", d.name, 30) + "</div>";
  html += "<div class='form-row'><label>Description:</label>" + textInput("design.description", d.description, 60) + "</div>";

  html += "<h3>2. Input fields</h3><table><tr><th>Field name</th><th>Default value</th><th></th></tr>";
  for (var i = 0; i < d.fields.length; i++) {
    html += "<tr><td>" + textInput("design.fields[" + i + "].name", d.fields[i].name) + "</td>" +
            "<td>" + textInput("design.fields[" + i + "].value", d.fields[i].value) + "</td>" +
            "<td><button onclick='design.fields.splice(" + i + ", 1); drawDesigner()'>Remove</button></td></tr>";
  }
  html += "</table><button onclick='design.fields.push({ name: \"\", value: \"\" }); drawDesigner()'>Add Field</button>";

  html += "<h3>3. Participants</h3><table><tr><th>Name</th><th>Role</th><th></th></tr>";
  for (var j = 0; j < d.people.length; j++) {
    html += "<tr><td>" + textInput("design.people[" + j + "].name", d.people[j].name) + "</td>" +
            "<td>" + textInput("design.people[" + j + "].role", d.people[j].role) + "</td>" +
            "<td><button onclick='design.people.splice(" + j + ", 1); drawDesigner()'>Remove</button></td></tr>";
  }
  html += "</table><button onclick='design.people.push({ name: \"\", role: \"\" }); drawDesigner()'>Add Participant</button>";

  html += "<h3>4. Work items (run in this order)</h3>";
  html += "<table><tr><th>ID</th><th>Type</th><th>Name</th><th>Role</th><th>Priority</th><th>Notify</th><th>Automatic call</th><th></th></tr>";
  for (var k = 0; k < d.items.length; k++) {
    var p = "design.items[" + k + "]";
    html += "<tr><td>S" + (k + 1) + "</td><td>" + selectInput(p + ".type", d.items[k].type, types) + "</td>" +
            "<td>" + textInput(p + ".name", d.items[k].name, 20) + "</td>" +
            "<td>" + textInput(p + ".role", d.items[k].role, 10) + "</td>" +
            "<td><input type='number' min='0' style='width:50px' value='" + esc(d.items[k].priority) + "' onchange='" + p + ".priority = Number(this.value)'></td>" +
            "<td>" + textInput(p + ".notify", d.items[k].notify, 10) + "</td>" +
            "<td>" + textInput(p + ".automatic", d.items[k].automatic, 14) + "</td>" +
            "<td><button onclick='design.items.splice(" + k + ", 1); drawDesigner()'>Remove</button></td></tr>";
  }
  html += "</table><button onclick='design.items.push({ type: \"task\", name: \"\", role: \"\", priority: 0, notify: \"\", automatic: \"\" }); drawDesigner()'>Add Work Item</button>";
  html += "<p class='small'>Automatic call: the item calls the external service with the instance data and fails if a value is empty.</p>";

  var fieldOptions = [["", "(none)"]];
  for (var f = 0; f < d.fields.length; f++) fieldOptions.push([d.fields[f].name, d.fields[f].name]);

  html += "<h3>5. Rules</h3>";
  html += "<div class='form-row'><label>Assignment strategy:</label>" + selectInput("design.strategy", d.strategy,
          [["workload", "Lowest workload"], ["role", "First available with the role"], ["rule", "Workload limit"]]) + "</div>";
  html += "<div class='form-row'><label>Workload limit:</label><input type='number' min='0' value='" + esc(d.maxWorkload) + "' onchange='design.maxWorkload = Number(this.value)'></div>";
  html += "<div class='form-row'><label>Processor:</label>" + selectInput("design.processor", d.processor,
          [["default", "Default"], ["custom", "Custom (with validation)"]]) + "</div>";
  html += "<div class='form-row'><label>Approval based on:</label>" + selectInput("design.approvalField", d.approvalField, fieldOptions) +
          " level 1 approves up to <input type='number' style='width:80px' value='" + esc(d.level1) + "' onchange='design.level1 = Number(this.value)'>" +
          " level 2 approves up to <input type='number' style='width:80px' value='" + esc(d.level2) + "' onchange='design.level2 = Number(this.value)'>" +
          " above that: " + selectInput("design.overLimit", d.overLimit, [["reject", "reject"], ["approve", "approve"]]) + "</div>";

  html += "<h4>Routing rules</h4>";
  html += "<table><tr><th>After item</th><th>Field</th><th>Operator</th><th>Value</th><th>New item name</th><th>New item role</th><th></th></tr>";
  var itemOptions = [];
  for (var m = 0; m < d.items.length; m++) itemOptions.push([m, "S" + (m + 1)]);
  var ops = [[">", ">"], [">=", ">="], ["<", "<"], ["<=", "<="], ["==", "=="], ["!=", "!="]];
  for (var n = 0; n < d.rules.length; n++) {
    var r = "design.rules[" + n + "]";
    html += "<tr><td>" + selectInput(r + ".after", d.rules[n].after, itemOptions) + "</td>" +
            "<td>" + selectInput(r + ".field", d.rules[n].field, fieldOptions.slice(1)) + "</td>" +
            "<td>" + selectInput(r + ".op", d.rules[n].op, ops) + "</td>" +
            "<td>" + textInput(r + ".value", d.rules[n].value, 8) + "</td>" +
            "<td>" + textInput(r + ".name", d.rules[n].name, 18) + "</td>" +
            "<td>" + textInput(r + ".role", d.rules[n].role, 10) + "</td>" +
            "<td><button onclick='design.rules.splice(" + n + ", 1); drawDesigner()'>Remove</button></td></tr>";
  }
  html += "</table><button onclick='addRule()'>Add Rule</button>";

  html += "<hr><button onclick='checkDesign()'>Check</button> <button onclick='saveDesign()'>Save Workflow</button>";
  setContent(html);
}

function addRule() {
  var firstField = design.fields.length ? design.fields[0].name : "";
  design.rules.push({ after: 0, field: firstField, op: ">", value: "", name: "", role: "" });
  drawDesigner();
}


function designToJson() {
  var d = design;
  var json = {
    name: d.name, description: d.description,
    parameters: [], participants: [], items: [], routes: [],
    strategy: d.strategy, maxWorkload: d.maxWorkload, processor: d.processor,
    approval: { parameter: d.approvalField, level1Max: d.level1, level2Max: d.level2, overLimit: d.overLimit }
  };
  for (var i = 0; i < d.fields.length; i++) json.parameters.push({ name: d.fields[i].name, "default": d.fields[i].value });
  for (var j = 0; j < d.people.length; j++) json.participants.push({ id: "p" + (j + 1), name: d.people[j].name, role: d.people[j].role });
  for (var k = 0; k < d.items.length; k++) {
    var it = d.items[k];
    json.items.push({ id: "S" + (k + 1), type: it.type, name: it.name, role: it.role,
                      priority: it.priority, notify: it.notify, automatic: it.automatic });
  }
  for (var n = 0; n < d.rules.length; n++) {
    var r = d.rules[n];
    json.routes.push({ after: "S" + (Number(r.after) + 1), parameter: r.field, op: r.op, value: r.value,
                       insert: { id: "R" + (n + 1), type: "task", name: r.name, role: r.role } });
  }
  return json;
}

async function checkDesign() {
  var data = await api("POST", "/api/workflows/check", designToJson());
  if (data.ok) showMessage("The design is valid.");
  else showMessage("Error: " + data.error, true);
}

async function saveDesign() {
  var data = await api("POST", "/api/workflows", designToJson());
  if (!data.ok) {
    showMessage("Error: " + data.error, true);
    return;
  }
  design = null;
  await showHome();
  showMessage("Saved workflow " + data.design.id + ". You can now start an instance of it.");
}

showHome();
