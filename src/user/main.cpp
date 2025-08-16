#include "pch.h"
#include "main.h"

#include "core/rendering/renderer.h"
#include <atomic>
#include "cheat/cheat.h"

void Main::run()
{
    LOG_INFO("Starting initialization...");
    Sleep(1000);

    if (!initializeUnity())
        LOG_ERROR("Unable to initialize Unity! Maybe assemblies are not found?");

    if (!PipeManager::isUsingPipes())
    {
        LOG_INFO("{}", Renderer::getInstance().initialize()
                 ? "Renderer initialized! Press INSERT to toggle GUI visibility"
                 : "Failed to initialize renderer!");
    }

    cheat::init();
}

void Main::shutdown()
{
    static std::atomic_bool s_shuttingDown{ false };
    bool expected = false;
    if (!s_shuttingDown.compare_exchange_strong(expected, true))
        return; // already shutting down

    LOG_INFO("Shutting down...");

    // Tear down renderer first to restore WndProc and ImGui before disabling hooks
    Renderer::getInstance().shutdown();

    // Then tear down hooks and other cheat systems
    cheat::shutdown();

    // Small grace period to let any in-flight frames settle
    Sleep(100);

    // Close logs; keep console attached by default (soft-uninject hides UI only)
    Logger::close();
}


Main::UnityModuleBackendInfo Main::getUnityBackend()
{
    LOG_INFO("Finding Unity backend...");

    UnityModuleBackendInfo info;
    info.module = nullptr;
    info.mode = UnityResolve::Mode::Mono;

    // Check if Unity is loaded
    if (const auto unityModule = GetModuleHandleA("UnityPlayer.dll"); unityModule == nullptr)
    {
        LOG_ERROR("UnityPlayer.dll not found! Is this a Unity game?");
        return info;
    }

    auto assembly = GetModuleHandleA("GameAssembly.dll");
    if (assembly)
    {
        info.module = assembly;
        info.mode = UnityResolve::Mode::Il2Cpp;
        LOG_INFO("Found Il2Cpp backend!");
        return info;
    }
    LOG_WARNING("GameAssembly.dll not found, trying fallback to Mono...");

    std::vector<std::string> monoModules = {
        "mono-2.0.dll",
        "mono-2.0-bdwgc.dll",
        "mono.dll"
    };

    for (const auto& monoModule : monoModules)
    {
        if (const auto monoHandle = GetModuleHandleA(monoModule.c_str()); monoHandle)
        {
            info.module = monoHandle;
            info.mode = UnityResolve::Mode::Mono;
            LOG_INFO("Found Mono backend: %s", monoModule.c_str());
            return info;
        }
    }

    LOG_ERROR("Unable to find Unity backend! Neither GameAssembly.dll nor mono.dll found.");
    return info;
}

bool Main::initializeUnity()
{
    auto [module, mode] = getUnityBackend();
    if (module == nullptr)
    {
        LOG_ERROR("Unity backend not found!");
        return false;
    }

    UnityResolve::Init(module, mode);

    return true;
}
