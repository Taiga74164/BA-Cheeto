#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    static ConfigManager& getInstance();

    bool load();
    bool save();
    bool saveNow() { return save(); }

    // Debounced saving
    void scheduleSave(int debounceMs = 250);

    // feature path helpers
    bool getFeatureEnabled(const std::string& section, const std::string& name, bool def = false) const;
    void setFeatureEnabled(const std::string& section, const std::string& name, bool enabled);

    template <typename T>
    T getFeatureValue(const std::string& section, const std::string& name, const std::string& key, const T& def) const {
        const nlohmann::json* node = getFeatureNodeConst(section, name);
        if (!node) return def;
        const auto it = node->find(key);
        if (it == node->end()) return def;
        try { return it->get<T>(); } catch (...) { return def; }
    }

    template <typename T>
    void setFeatureValue(const std::string& section, const std::string& name, const std::string& key, const T& value) {
        auto& node = getOrCreateFeatureNode(section, name);
        node[key] = value;
    }

    const nlohmann::json& data() const { return m_data; }
    std::string getConfigFilePath() const { return configPath(); }

    // Profiles
    void setProfile(const std::string& profileNameOrFile);
    std::string getProfile() const { return m_profile; }
    std::vector<std::string> listProfiles() const; // returns normalized names: "default" or custom names
    bool createProfile(const std::string& profileName);

    // Resets
    void resetFeature(const std::string& section, const std::string& name);
    void resetAll();

private:
    ConfigManager() = default;

    std::string configPath() const;
    std::string profileNameFromFile(const std::string& file) const;
    nlohmann::json* getFeatureNode(const std::string& section, const std::string& name);
    const nlohmann::json* getFeatureNodeConst(const std::string& section, const std::string& name) const;
    nlohmann::json& getOrCreateFeatureNode(const std::string& section, const std::string& name);

    nlohmann::json m_data;
    std::string m_profile{"default"};

    // debounce state
    std::atomic<unsigned long long> m_saveSeq{0};
    mutable std::mutex m_saveMutex;
};
