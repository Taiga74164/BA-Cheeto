#include "pch.h"
#include "user/main.h"
#include "utils/config_manager.h"
#include <filesystem>
#include <thread>

// Global module handle used for safe uninjection
HMODULE g_hModule = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            g_hModule = hModule;
            {
                // Log verbosely to file under %APPDATA%\Cunny\logs and also to console
                auto configDir = std::filesystem::path(ConfigManager::getInstance().getConfigFilePath()).parent_path();
                auto logsDir = (configDir / "logs").string();
                Logger::attach()
                    .showTimeStamp(true)
                    .showFileName(true)
                    .showLineNumber(true)
                    .enableColors(true)
                    .logToFile(logsDir);
            }
            if (PipeManager::isUsingPipes())
            {
                Main::run();
                PipeManager::getInstance().start();
            }
            else
            {
                std::thread(Main::run).detach();
            }
            break;

        case DLL_PROCESS_DETACH:
            if (lpReserved == nullptr)
            {
                Main::shutdown();
                if (PipeManager::isUsingPipes())
					PipeManager::getInstance().stop();
            }
            break;

        default:
            break;
    }

    return TRUE;
}
