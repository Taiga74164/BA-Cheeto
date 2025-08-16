#include "pch.h"
#include "hook_manager.h"

HookManager& HookManager::getInstance()
{
    static HookManager instance;
    return instance;
}

HookManager::~HookManager()
{
    shutdown();
}

bool HookManager::initialize()
{
    if (m_initialized) return true;

    auto status = MH_Initialize();
    if (status != MH_OK) return false;

    m_initialized = true;
    return true;
}

void HookManager::shutdown()
{
    if (!m_initialized) return;
    m_blockNewHooks.store(true);

    // Best-effort disable all, then remove each explicitly
    (void)disableAllHooks();

    for (auto& hook : m_hooks)
    {
        if (hook->target)
        {
            MH_DisableHook(hook->target);
            MH_RemoveHook(hook->target);
        }
    }

    MH_Uninitialize();
    m_hooks.clear();
    m_initialized = false;
}

bool HookManager::enableHook(void* target)
{
    if (!m_initialized) return false;

    HookInfo* hook = findHook(target);
    if (!hook) return false;

    if (hook->enabled) return true;

    auto status = MH_EnableHook(hook->target);
    if (status == MH_OK)
    {
        hook->enabled = true;
        return true;
    }

    return false;
}

bool HookManager::disableHook(void* target)
{
    if (!m_initialized) return false;

    HookInfo* hook = findHook(target);
    if (!hook) return false;

    if (!hook->enabled) return true;

    auto status = MH_DisableHook(hook->target);
    if (status == MH_OK)
    {
        hook->enabled = false;
        return true;
    }

    return false;
}

bool HookManager::enableAllHooks()
{
    if (!m_initialized) return false;

    auto status = MH_EnableHook(nullptr);
    if (status != MH_OK) return false;

    for (auto& hook : m_hooks)
    {
        hook->enabled = true;
    }

    return true;
}

bool HookManager::disableAllHooks()
{
    if (!m_initialized) return false;

    auto status = MH_DisableHook(nullptr);
    if (status != MH_OK) return false;

    for (auto& hook : m_hooks)
    {
        hook->enabled = false;
    }

    return true;
}

HookManager::HookInfo* HookManager::findHook(void* target)
{
    auto it = std::find_if(m_hooks.begin(), m_hooks.end(),
                           [target](const std::unique_ptr<HookInfo>& hook) { return hook->target == target; });

    return (it != m_hooks.end()) ? it->get() : nullptr;
}

void* HookManager::resolveModuleFunction(const std::string& moduleName, intptr_t offset)
{
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (!hModule)
    {
        // Try to load the module if it's not already loaded
        hModule = LoadLibraryA(moduleName.c_str());
        if (!hModule)
        {
#ifdef _DEBUG
			std::string debugMsg = "HookManager: Failed to load module " + moduleName + "\n";
			OutputDebugStringA(debugMsg.c_str());
#endif
            return nullptr;
        }
    }

    auto targetFunction = reinterpret_cast<void*>(reinterpret_cast<intptr_t>(hModule) + offset);

#ifdef _DEBUG
	std::string debugMsg = "HookManager: Resolved " + moduleName + " + 0x" + 
						  std::to_string(offset) + " to " + std::to_string(reinterpret_cast<intptr_t>(targetFunction)) + "\n";
	OutputDebugStringA(debugMsg.c_str());
#endif

    return targetFunction;
}
