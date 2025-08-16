#include "pch.h"
#include "pipe_manager.h"

PipeManager& PipeManager::getInstance()
{
    static PipeManager instance;
    return instance;
}

bool PipeManager::isUsingPipes()
{
    static std::optional<bool> usePipes;

    if (!usePipes.has_value())
    {
        const std::string pipeName = PIPE_FLAG_NAME + std::to_string(GetCurrentProcessId());
        const HANDLE hMapFile = OpenFileMappingA(FILE_MAP_READ, FALSE, pipeName.c_str());
        if (!hMapFile)
        {
            usePipes = false;
            return *usePipes;
        }

        const LPVOID pBuf = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 1);
        if (!pBuf)
        {
            CloseHandle(hMapFile);
            usePipes = false;
            return *usePipes;
        }

        usePipes = *static_cast<char*>(pBuf) == 1;

        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
    }

    return *usePipes;
}

PipeManager::PipeManager() = default;

PipeManager::~PipeManager()
{
    stop();
}

void PipeManager::start()
{
    if (m_running.load()) return;

    m_running.store(true);
    m_serverThread = std::thread(&PipeManager::runServer, this);
}

void PipeManager::stop()
{
    if (!m_running.load()) return;

    m_running.store(false);
    if (m_serverThread.joinable()) m_serverThread.join();
}

void PipeManager::runServer()
{
    LOG_INFO("Starting named pipe server...");
    while (m_running)
    {
        const HANDLE hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024, 1024, 0, nullptr);

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR("Failed to create named pipe: {}", GetLastError());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        LOG_INFO("Waiting for client connection...");
        const auto connected = ConnectNamedPipe(hPipe, nullptr);
        if (connected || GetLastError() == ERROR_PIPE_CONNECTED)
        {
            LOG_INFO("Client connected successfully.");
            handleClient(hPipe);
        }
        else
        {
            LOG_ERROR("Failed to connect to client: {}", GetLastError());
        }

        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    LOG_INFO("Named pipe server stopped.");
}

void PipeManager::handleClient(HANDLE hPipe)
{
    char buffer[1024];
    DWORD bytesRead;

    while (m_running)
    {
        memset(buffer, 0, sizeof(buffer));

        if (const auto success = ReadFile(
            hPipe,
            buffer,
            sizeof(buffer) - 1,
            &bytesRead,
            nullptr); !success || bytesRead == 0)
        {
            DWORD error = GetLastError();
            if (error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA)
            {
                LOG_INFO("Client disconnected gracefully.");
            }
            else
            {
                LOG_ERROR("Failed to read from pipe: {}", error);
            }
            break;
        }

        // Ensure null termination
        buffer[bytesRead] = '\0';
        std::string message(buffer, bytesRead);

        LOG_INFO("Received message: '{}' (length: {})", message.c_str(), bytesRead);

        // Message format: "feature:featureName:enable/disable"
        const size_t colon1 = message.find(':');
        const size_t colon2 = message.find(':', colon1 + 1);

        if (colon1 != std::string::npos && colon2 != std::string::npos)
        {
            std::string cmd = message.substr(0, colon1);
            std::string feature = message.substr(colon1 + 1, colon2 - colon1 - 1);
            std::string state = message.substr(colon2 + 1);

            cmd.erase(cmd.find_last_not_of(" \t\r\n") + 1);
            feature.erase(feature.find_last_not_of(" \t\r\n") + 1);
            state.erase(state.find_last_not_of(" \t\r\n") + 1);

            if (cmd == "feature")
            {
                const bool enabled = state == "enable";
                m_features[feature] = enabled;
                LOG_INFO("Feature '{}' set to {}", feature.c_str(), enabled ? "enabled" : "disabled");

                // Send acknowledgment back to client
                std::string response = "OK: " + feature + ":" + (enabled ? "enabled" : "disabled");
                DWORD bytesWritten;
                const DWORD toWrite = static_cast<DWORD>(response.size());
                WriteFile(hPipe, response.c_str(), toWrite, &bytesWritten, nullptr);
                FlushFileBuffers(hPipe);
            }
            else
            {
                LOG_ERROR("Unknown command: '{}'", cmd.c_str());
                std::string response = "ERROR: Unknown command";
                DWORD bytesWritten;
                const DWORD toWrite = static_cast<DWORD>(response.size());
                WriteFile(hPipe, response.c_str(), toWrite, &bytesWritten, nullptr);
                FlushFileBuffers(hPipe);
            }
        }
        else
        {
            LOG_ERROR("Invalid message format: '{}'", message.c_str());
            std::string response = "ERROR: Invalid format";
            DWORD bytesWritten;
            const DWORD toWrite = static_cast<DWORD>(response.size());
            WriteFile(hPipe, response.c_str(), toWrite, &bytesWritten, nullptr);
            FlushFileBuffers(hPipe);
        }
    }
}

bool PipeManager::isFeatureEnabled(const std::string& featureName) const
{
    std::lock_guard lock(m_featuresMutex);
    const auto it = m_features.find(featureName);
    return it != m_features.end() ? it->second : false;
}

void PipeManager::setFeatureState(const std::string& featureName, bool enabled)
{
    std::lock_guard lock(m_featuresMutex);
    m_features[featureName] = enabled;
    LOG_INFO("Feature {} set to {}", featureName.c_str(), enabled ? "enabled" : "disabled");
}
