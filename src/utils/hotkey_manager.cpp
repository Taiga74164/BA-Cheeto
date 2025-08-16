#include "pch.h"
#include "hotkey_manager.h"
#include "config_manager.h"

HotkeyManager& HotkeyManager::getInstance()
{
    static HotkeyManager inst; return inst;
}

void HotkeyManager::registerHotkey(const std::string& id, int defaultVk)
{
    // Always read from current profile's config to keep in sync on profile switches
    int v = ConfigManager::getInstance().getFeatureValue<int>("Settings", "Hotkeys", id, defaultVk);
    m_map[id] = v;
}

void HotkeyManager::setVk(const std::string& id, int vk)
{
    m_map[id] = vk;
    ConfigManager::getInstance().setFeatureValue("Settings", "Hotkeys", id, vk);
    ConfigManager::getInstance().scheduleSave();
}

int HotkeyManager::getVk(const std::string& id) const
{
    auto it = m_map.find(id);
    return it == m_map.end() ? 0 : it->second;
}

void HotkeyManager::load()
{
    for (auto& kv : m_map)
    {
        int vk = ConfigManager::getInstance().getFeatureValue<int>("Settings", "Hotkeys", kv.first, kv.second);
        kv.second = vk;
    }
}

void HotkeyManager::save() const
{
    for (const auto& kv : m_map)
    {
        ConfigManager::getInstance().setFeatureValue("Settings", "Hotkeys", kv.first, kv.second);
    }
    ConfigManager::getInstance().save();
}

const std::vector<std::pair<const char*, int>>& HotkeyManager::keyList()
{
    static std::vector<std::pair<const char*, int>> list = {
        {"None", 0},
        {"INSERT", VK_INSERT}, {"DELETE", VK_DELETE}, {"HOME", VK_HOME}, {"END", VK_END},
        {"PAGE UP", VK_PRIOR}, {"PAGE DOWN", VK_NEXT},
        {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4}, {"F5", VK_F5},
        {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8}, {"F9", VK_F9}, {"F10", VK_F10},
        {"F11", VK_F11}, {"F12", VK_F12},
        {"TAB", VK_TAB}, {"CAPS", VK_CAPITAL}, {"SHIFT", VK_SHIFT}, {"CTRL", VK_CONTROL}, {"ALT", VK_MENU},
        {"SPACE", VK_SPACE}, {"BACKSPACE", VK_BACK},
        {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'}, {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'}, {"0", '0'},
        {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'}, {"F", 'F'}, {"G", 'G'}, {"H", 'H'},
        {"I", 'I'}, {"J", 'J'}, {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'}, {"O", 'O'}, {"P", 'P'},
        {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'}, {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'}, {"Z", 'Z'}
    };
    return list;
}

const char* HotkeyManager::keyName(int vk)
{
    for (const auto& p : keyList()) if (p.second == vk) return p.first;
    return "?";
}

void HotkeyManager::clear(const std::string& id)
{
    setVk(id, 0);
}

std::vector<std::string> HotkeyManager::getIdsForVk(int vk) const
{
    std::vector<std::string> ids;
    for (const auto& kv : m_map) if (kv.second == vk) ids.push_back(kv.first);
    return ids;
}
