#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>

class HotkeyManager {
public:
    static HotkeyManager& getInstance();

    // Register a hotkey with a default VK
    void registerHotkey(const std::string& id, int defaultVk);

    // Set/Get
    void setVk(const std::string& id, int vk);
    int getVk(const std::string& id) const;
    void clear(const std::string& id); // set to 0 and persist

    // Query bindings
    std::vector<std::string> getIdsForVk(int vk) const;

    // Capture flow
    void beginCapture(const std::string& id) { m_capturingId = id; }
    bool isCapturing() const { return !m_capturingId.empty(); }
    const std::string& captureId() const { return m_capturingId; }
    void setCaptured(int vk) { if (!m_capturingId.empty()) { setVk(m_capturingId, vk); m_capturingId.clear(); } }
    void cancelCapture() { m_capturingId.clear(); }

    // Persistence
    void load();
    void save() const;

    // Utilities
    static const std::vector<std::pair<const char*, int>>& keyList();
    static const char* keyName(int vk);

private:
    HotkeyManager() = default;
    std::unordered_map<std::string, int> m_map; // id -> VK_*
    std::string m_capturingId;
};
