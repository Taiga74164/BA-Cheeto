﻿#pragma once

#include <vector>
#include <memory>
#include <MinHook.h>

#ifdef _DEBUG
#define CALL_ORIGINAL(handler, ...) \
HookManager::getInstance().callOriginal(handler, __func__, __VA_ARGS__)
#else
#define CALL_ORIGINAL(handler, ...) \
HookManager::getInstance().callOriginal(handler, __VA_ARGS__)
#endif

class HookManager
{
public:
    struct HookInfo
    {
        void* target;
        void* detour;
        void* original;
        bool enabled;
        std::string moduleName;
        intptr_t offset;

        HookInfo(void* t, void* d, void* o, std::string module = "", intptr_t off = 0)
            : target(t)
            , detour(d)
            , original(o)
            , enabled(false)
            , moduleName(std::move(module))
            , offset(off)
        {
        }
    };

    static HookManager& getInstance();

    // Shutdown and cleanup all hooks
    void shutdown();

    // Install hook using direct function pointer
    template <typename T>
    static bool install(T target, T detour);

    // Install hook using module name + offset
    template <typename T>
    static bool install(const std::string& moduleName, intptr_t offset, T detour);

    // Hook management
    bool enableHook(void* target);
    bool disableHook(void* target);

    // Get original function from handler
    template <typename T>
    T getOriginal(T handler) const;

    // Call original function through handler with automatic lookup
#ifdef _DEBUG
    template <typename RType, typename... Params>
    RType callOriginal(RType (*handler)(Params...), const char* callerName, Params... params);
#else
    template <typename RType, typename... Params>
    RType callOriginal(RType (*handler)(Params...), Params... params);
#endif

    _NODISCARD bool isInitialized() const { return m_initialized; }
    _NODISCARD const std::vector<std::unique_ptr<HookInfo>>& getHooks() const { return m_hooks; }

private:
    HookManager() = default;
    ~HookManager();

    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

    std::vector<std::unique_ptr<HookInfo>> m_hooks;
    std::unordered_map<void*, void*> m_detourToOriginal;
    bool m_initialized = false;

    bool initialize();
    HookInfo* findHook(void* target);
    void* resolveModuleFunction(const std::string& moduleName, intptr_t offset);
};

template <typename T>
bool HookManager::install(T target, T detour)
{
    auto& instance = getInstance();
    if (!instance.initialize() && !instance.m_initialized) return false;

    void* originalPtr = nullptr;
    auto status = MH_CreateHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(detour), &originalPtr);
    if (status != MH_OK)
    {
        LOG_ERROR("Failed to create hook {}", magic_enum::enum_name(status));
        return false;
    }

    status = MH_EnableHook(reinterpret_cast<void*>(target));
    if (status != MH_OK)
    {
        MH_RemoveHook(reinterpret_cast<void*>(target));
        LOG_ERROR("Failed to enable hook: {}", magic_enum::enum_name(status));
        return false;
    }

    auto hookInfo = std::make_unique<HookInfo>(reinterpret_cast<void*>(target), reinterpret_cast<void*>(detour),
                                               originalPtr);
    hookInfo->enabled = true;
    instance.m_hooks.push_back(std::move(hookInfo));

    // Add to fast lookup map
    instance.m_detourToOriginal[reinterpret_cast<void*>(detour)] = originalPtr;
    return true;
}

template <typename T>
bool HookManager::install(const std::string& moduleName, intptr_t offset, T detour)
{
    auto& instance = getInstance();
    if (!instance.initialize() && !instance.m_initialized) return false;

    void* target = instance.resolveModuleFunction(moduleName, offset);
    if (!target) return false;

    void* originalPtr = nullptr;
    auto status = MH_CreateHook(target, reinterpret_cast<void*>(detour), &originalPtr);
    if (status != MH_OK)
    {
        LOG_ERROR("Failed to create hook %d", magic_enum::enum_name(status));
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        MH_RemoveHook(target);
        LOG_ERROR("Failed to enable hook: %d", magic_enum::enum_name(status));
        return false;
    }

    auto hookInfo = std::make_unique<
        HookInfo>(target, reinterpret_cast<void*>(detour), originalPtr, moduleName, offset);
    hookInfo->enabled = true;
    instance.m_hooks.push_back(std::move(hookInfo));

    instance.m_detourToOriginal[reinterpret_cast<void*>(detour)] = originalPtr;
    return true;
}

template <typename T>
T HookManager::getOriginal(T handler) const
{
    auto it = m_detourToOriginal.find(reinterpret_cast<void*>(handler));
    if (it != m_detourToOriginal.end()) return reinterpret_cast<T>(it->second);

#ifdef _DEBUG
    // In debug mode, provide more detailed error information
    OutputDebugStringA("HookManager: Original function not found for handler\n");
#endif

    return nullptr;
}

#ifdef _DEBUG
template <typename RType, typename... Params>
RType HookManager::callOriginal(RType (*handler)(Params...), const char* callerName, Params... params)
{
    auto original = getOriginal(handler);
    if (original != nullptr)
        return original(params...);

    if (callerName)
    {
        std::string debugMsg = "HookManager: Original function not found for handler in " + std::string(callerName) +
            "\n";
        OutputDebugStringA(debugMsg.c_str());
    }

    return RType{};
}
#else
template <typename RType, typename... Params>
RType HookManager::callOriginal(RType (*handler)(Params...), Params... params)
{
    auto original = getOriginal(handler);
    if (original != nullptr) return original(params...);

    return RType{};
}
#endif
