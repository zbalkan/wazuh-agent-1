#include <iostream>
#include <cjson/cJSON.h>
#include <shared.h>
#include <defs.h>
#include <logging_helper.h>
#include <pthreads_op.h>

#include <inventory.hpp>
#include <sysInfo.hpp>

constexpr const char* INV_LOGTAG = "modules:inventory"; // Tag for log messages

void Inventory::Start() {

    if (!m_enabled) {
        Log(LOG_INFO, "Module disabled. Exiting...");
        return;
    }

    Log(LOG_INFO, "Starting inventory.");

    ShowConfig();

    DBSync::initialize(LogErrorInventory);

    try
    {
        Inventory::Instance().Init(std::make_shared<SysInfo>(),
                                    [this](const std::string& diff) { this->SendDeltaEvent(diff); },
                                    INVENTORY_DB_DISK_PATH,
                                    INVENTORY_NORM_CONFIG_DISK_PATH,
                                    INVENTORY_NORM_TYPE);
    }
    catch (const std::exception& ex)
    {
        LogErrorInventory(ex.what());
    }

    Log(LOG_INFO, "Module finished.");

}

void Inventory::Setup(const configuration::ConfigurationParser& configurationParser) {

    m_enabled = !configurationParser.GetConfig<bool>("inventory", "disabled");
    m_intervalValue = std::chrono::seconds{configurationParser.GetConfig<int>("inventory", "interval")};
    m_scanOnStart = configurationParser.GetConfig<bool>("inventory", "scan_on_start");
    m_hardware = configurationParser.GetConfig<bool>("inventory", "hardware");
    m_os = configurationParser.GetConfig<bool>("inventory", "os");
    m_network = configurationParser.GetConfig<bool>("inventory", "network");
    m_packages = configurationParser.GetConfig<bool>("inventory", "packages");
    m_ports = configurationParser.GetConfig<bool>("inventory", "ports");
    m_portsAll = configurationParser.GetConfig<bool>("inventory", "ports_all");
    m_processes = configurationParser.GetConfig<bool>("inventory", "processes");
    m_hotfixes = configurationParser.GetConfig<bool>("inventory", "hotfixes");
}

void Inventory::Stop() {
    Log(LOG_INFO, "Module stopped.");
    Inventory::Instance().Destroy();
}

std::string Inventory::Command(const std::string & query) {
    Log(LOG_INFO, "Query: " + query);
    return "OK";
}

void Inventory::SetMessageQueue(const std::shared_ptr<IMultiTypeQueue> queue) {
    m_messageQueue = queue;
}

void Inventory::SendDeltaEvent(const std::string& data) {

    const auto jsonData = nlohmann::json::parse(data);
    const Message message{ MessageType::STATELESS, jsonData, Name() };

    if(!m_messageQueue->push(message)) {
        Log(LOG_WARNING, "Delta event can't be pushed into the message queue: " + data);
    }
    else {
        Log(LOG_DEBUG_VERBOSE, "Delta sent: " + data);
    }
}

void Inventory::ShowConfig()
{
    cJSON * configJson = Dump();
    if (configJson) {
        char * configString = cJSON_PrintUnformatted(configJson);
        if (configString) {
            Log(LOG_DEBUG, configString);
            cJSON_free(configString);
        }
        cJSON_Delete(configJson);
    }
}

cJSON * Inventory::Dump() {

    cJSON *rootJson = cJSON_CreateObject();
    cJSON *invJson = cJSON_CreateObject();

    // System provider values
    if (m_enabled) cJSON_AddStringToObject(invJson,"disabled","no"); else cJSON_AddStringToObject(invJson,"disabled","yes");
    if (m_scanOnStart) cJSON_AddStringToObject(invJson,"scan-on-start","yes"); else cJSON_AddStringToObject(invJson,"scan-on-start","no");
    cJSON_AddNumberToObject(invJson, "interval", static_cast<double>(m_intervalValue.count()));
    if (m_network) cJSON_AddStringToObject(invJson,"network","yes"); else cJSON_AddStringToObject(invJson,"network","no");
    if (m_os) cJSON_AddStringToObject(invJson,"os","yes"); else cJSON_AddStringToObject(invJson,"os","no");
    if (m_hardware) cJSON_AddStringToObject(invJson,"hardware","yes"); else cJSON_AddStringToObject(invJson,"hardware","no");
    if (m_packages) cJSON_AddStringToObject(invJson,"packages","yes"); else cJSON_AddStringToObject(invJson,"packages","no");
    if (m_ports) cJSON_AddStringToObject(invJson,"ports","yes"); else cJSON_AddStringToObject(invJson,"ports","no");
    if (m_portsAll) cJSON_AddStringToObject(invJson,"ports_all","yes"); else cJSON_AddStringToObject(invJson,"ports_all","no");
    if (m_processes) cJSON_AddStringToObject(invJson,"processes","yes"); else cJSON_AddStringToObject(invJson,"processes","no");
#ifdef WIN32
    if (m_hotfixes) cJSON_AddStringToObject(invJson,"hotfixes","yes"); else cJSON_AddStringToObject(invJson,"hotfixes","no");
#endif

    cJSON_AddItemToObject(rootJson,"inventory",invJson);

    return rootJson;
}

void Inventory::Log(const modules_log_level_t level, const std::string& log)
{
    taggedLogFunction(level, log.c_str(), INV_LOGTAG);
}

void Inventory::LogErrorInventory(const std::string& log)
{
    taggedLogFunction(LOG_ERROR, log.c_str(), INV_LOGTAG);
}
