#pragma once

#include <string>
#include "utils/config_manager.h"

namespace Config {

template <typename T>
class Field {
public:
    Field() = default;
    Field(std::string section, std::string feature, std::string key, const T& defaultValue, bool autoSave = true)
        : m_section(std::move(section))
        , m_feature(std::move(feature))
        , m_key(std::move(key))
        , m_autoSave(autoSave)
    {
        reload(defaultValue);
    }

    // Read current value
    T get() const { return m_value; }
    operator T() const { return m_value; }

    // Assign and persist
    Field& operator=(const T& v) { set(v); return *this; }
    void set(const T& v)
    {
        m_value = v;
    ConfigManager::getInstance().setFeatureValue(m_section, m_feature, m_key, m_value);
    if (m_autoSave) ConfigManager::getInstance().scheduleSave();
    }

    // Ensure defaults exist and/or refresh from disk
    void reload(const T& defaultValue)
    {
    m_value = ConfigManager::getInstance().getFeatureValue<T>(m_section, m_feature, m_key, defaultValue);
    // Ensure the key exists on disk as well
    ConfigManager::getInstance().setFeatureValue(m_section, m_feature, m_key, m_value);
    if (m_autoSave) ConfigManager::getInstance().scheduleSave();
    }

    const std::string& section() const { return m_section; }
    const std::string& feature() const { return m_feature; }
    const std::string& key() const { return m_key; }

private:
    std::string m_section;
    std::string m_feature;
    std::string m_key;
    bool m_autoSave{true};
    T m_value{};
};

} // namespace Config
#pragma once

#include <string>
#include <utility>
#include "config_manager.h"

// Simple self-managed config field wrapper inspired by the review feedback.
// Usage:
//   ConfigField<float> scale{"Game", "Timescale", "scale", 1.0f};
//   float before = scale.get();
//   ImGui::SliderFloat("Scale", &scale.ref(), 0.1f, 10.0f);
//   scale.saveIfChanged(before); // persists if modified
//
// You can also set directly: scale.set(2.0f);
// Or read: float v = scale; (implicit conversion)

template <typename T>
class ConfigField {
public:
    ConfigField(std::string section, std::string name, std::string key, T def)
        : m_section(std::move(section)), m_name(std::move(name)), m_key(std::move(key))
    {
        m_value = ConfigManager::getInstance().getFeatureValue<T>(m_section, m_name, m_key, def);
    }

    // Read
    const T& get() const { return m_value; }
    operator T() const { return m_value; }

    // Write and persist
    void set(const T& v, bool save = true) {
        m_value = v;
        ConfigManager::getInstance().setFeatureValue(m_section, m_name, m_key, m_value);
        if (save) ConfigManager::getInstance().save();
    }

    // ImGui-friendly reference
    T& ref() { return m_value; }

    // Helper to persist only when changed
    bool saveIfChanged(const T& oldValue) {
        if (m_value != oldValue) {
            ConfigManager::getInstance().setFeatureValue(m_section, m_name, m_key, m_value);
            ConfigManager::getInstance().save();
            return true;
        }
        return false;
    }

private:
    std::string m_section;
    std::string m_name;
    std::string m_key;
    T m_value{};
};
