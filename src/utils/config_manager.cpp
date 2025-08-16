#include "pch.h"
#include "config_manager.h"
#include "utils/logger.h"

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

std::string ConfigManager::configPath() const {
    char* appData = nullptr;
    size_t len = 0;
    _dupenv_s(&appData, &len, "APPDATA");
    std::filesystem::path base = appData ? appData : ".";
    if (appData) free(appData);

    std::filesystem::path dir = base / "Cunny";
    std::error_code ec;
    if (!std::filesystem::exists(dir, ec)) {
        std::filesystem::create_directories(dir, ec);
        if (ec) LOG_ERROR("Failed to create config directory: {}", ec.message().c_str());
    }
    std::string file = (m_profile == "default") ? std::string("config.json") : ("config." + m_profile + ".json");
    return (dir / file).string();
}

bool ConfigManager::load() {
    const auto path = configPath();
    std::ifstream f(path);
    if (!f.good()) {
        m_data = nlohmann::json{ {"version", 1}, {"features", nlohmann::json::object()} };
        return true;
    }
    try {
        f >> m_data;
        if (!m_data.contains("features")) m_data["features"] = nlohmann::json::object();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse config: {}", e.what());
        m_data = nlohmann::json{ {"version", 1}, {"features", nlohmann::json::object()} };
        return false;
    }
}

bool ConfigManager::save() {
    const auto path = configPath();
    try {
        std::ofstream f(path, std::ios::trunc);
        f << m_data.dump(2);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save config: {}", e.what());
        return false;
    }
}

void ConfigManager::scheduleSave(int debounceMs)
{
    // Increment a sequence and spawn a detached timer that only saves if it is the latest
    const unsigned long long mySeq = ++m_saveSeq;
    std::thread([this, mySeq, debounceMs]() {
        int delay = debounceMs < 0 ? 0 : debounceMs;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if (m_saveSeq.load() == mySeq)
        {
            std::scoped_lock lk(m_saveMutex);
            (void)this->save();
        }
    }).detach();
}

const nlohmann::json* ConfigManager::getFeatureNodeConst(const std::string& section, const std::string& name) const {
    auto itF = m_data.find("features");
    if (itF == m_data.end() || !itF->is_object()) return nullptr;
    auto itS = itF->find(section);
    if (itS == itF->end() || !itS->is_object()) return nullptr;
    auto itN = itS->find(name);
    if (itN == itS->end() || !itN->is_object()) return nullptr;
    return &*itN;
}

nlohmann::json* ConfigManager::getFeatureNode(const std::string& section, const std::string& name) {
    auto itF = m_data.find("features");
    if (itF == m_data.end() || !itF->is_object()) return nullptr;
    auto itS = itF->find(section);
    if (itS == itF->end() || !itS->is_object()) return nullptr;
    auto itN = itS->find(name);
    if (itN == itS->end() || !itN->is_object()) return nullptr;
    return &*itN;
}

nlohmann::json& ConfigManager::getOrCreateFeatureNode(const std::string& section, const std::string& name) {
    if (!m_data.contains("features") || !m_data["features"].is_object()) m_data["features"] = nlohmann::json::object();
    auto& features = m_data["features"];
    if (!features.contains(section) || !features[section].is_object()) features[section] = nlohmann::json::object();
    auto& sec = features[section];
    if (!sec.contains(name) || !sec[name].is_object()) sec[name] = nlohmann::json::object();
    return sec[name];
}

bool ConfigManager::getFeatureEnabled(const std::string& section, const std::string& name, bool def) const {
    const auto* node = getFeatureNodeConst(section, name);
    if (!node) return def;
    auto it = node->find("enabled");
    if (it == node->end()) return def;
    try { return it->get<bool>(); } catch (...) { return def; }
}

void ConfigManager::setFeatureEnabled(const std::string& section, const std::string& name, bool enabled) {
    auto& node = getOrCreateFeatureNode(section, name);
    node["enabled"] = enabled;
}

std::string ConfigManager::profileNameFromFile(const std::string& file) const
{
    // Accept "config.json" -> default, or "config.NAME.json" -> NAME
    std::string name = file;
    if (name == "config.json") return "default";
    const std::string prefix = "config.";
    const std::string suffix = ".json";
    if (name.rfind(prefix, 0) == 0 && name.size() > prefix.size() + suffix.size() &&
        name.substr(name.size() - suffix.size()) == suffix)
    {
        return name.substr(prefix.size(), name.size() - prefix.size() - suffix.size());
    }
    return file; // fallback: already a name
}

void ConfigManager::setProfile(const std::string& profileNameOrFile)
{
    if (profileNameOrFile.empty()) return;
    std::string normalized = profileNameFromFile(profileNameOrFile);
    if (normalized == m_profile) return;

    // Save current profile first
    (void)save();

    m_profile = normalized;
    // Load profile file (separate json per profile)
    // We reuse configPath but append profile name, e.g., config.default.json
    if (!load())
    {
        // if no file yet, start fresh
        m_data = nlohmann::json{ {"version", 1}, {"features", nlohmann::json::object()} };
    }
}

std::vector<std::string> ConfigManager::listProfiles() const
{
    std::vector<std::string> profiles;
    try
    {
        auto base = std::filesystem::path(configPath()).parent_path();
        for (auto& e : std::filesystem::directory_iterator(base))
        {
            if (!e.is_regular_file()) continue;
            auto name = e.path().filename().string();
            if (name == "config.json")
                profiles.push_back("default");
            else if (name.rfind("config.", 0) == 0 && e.path().extension() == ".json")
                profiles.push_back(profileNameFromFile(name));
        }
    }
    catch (...) { }
    // Ensure "default" is present even if file doesn’t exist yet
    if (std::find(profiles.begin(), profiles.end(), "default") == profiles.end()) profiles.push_back("default");
    std::sort(profiles.begin(), profiles.end());
    profiles.erase(std::unique(profiles.begin(), profiles.end()), profiles.end());
    return profiles;
}

void ConfigManager::resetFeature(const std::string& section, const std::string& name)
{
    if (!m_data.contains("features")) return;
    auto& features = m_data["features"];
    if (!features.contains(section)) return;
    features[section].erase(name);
}

void ConfigManager::resetAll()
{
    m_data = nlohmann::json{ {"version", 1}, {"features", nlohmann::json::object()} };
    (void)save();
}

bool ConfigManager::createProfile(const std::string& profileName)
{
    if (profileName.empty() || profileName == "config" || profileName == "json") return false;
    std::string prev = m_profile;
    // Switch to new profile and write a fresh file
    m_profile = profileName;
    m_data = nlohmann::json{ {"version", 1}, {"features", nlohmann::json::object()} };
    bool ok = save();
    if (!ok) m_profile = prev; // revert on failure
    return ok;
}
