#include "pch.h"
#include "dx11_backend.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "core/rendering/fonts/NotoSans.hpp"
#include "core/rendering/renderer.h"
#include "ui/gui.h"
#include "utils/dx_utils.h"
#include "utils/hotkey_manager.h"
#include "user/cheat/feature_manager.h"

DX11Backend* DX11Backend::s_instance = nullptr;
DX11Backend::Present_t DX11Backend::m_originalPresent = nullptr;
DX11Backend::ResizeBuffers_t DX11Backend::m_originalResizeBuffers = nullptr;
WNDPROC DX11Backend::m_originalWndProc = nullptr;

DX11Backend::DX11Backend()
    : m_initialized(false)
    , m_imguiInitialized(false)
    , m_device(nullptr)
    , m_context(nullptr)
    , m_swapChain(nullptr)
    , m_renderTargetView(nullptr)
    , m_window(nullptr)
{
    assert(s_instance == nullptr);
    s_instance = this;
}

DX11Backend::~DX11Backend()
{
    DX11Backend::shutdown();
    s_instance = nullptr;
}

bool DX11Backend::initialize()
{
    if (m_initialized) return true;

    if (!HookManager::getInstance().initialize()) return false;

    if (!setupHooks()) return false;

    m_initialized = true;
    return true;
}

void DX11Backend::shutdown()
{
    if (!m_initialized) return;

    shutdownImGui();
    cleanupRenderTarget();

    if (m_window && m_originalWndProc)
    {
        SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
        m_originalWndProc = nullptr;
    }

    m_device = nullptr;
    m_context = nullptr;
    m_swapChain = nullptr;

    m_initialized = false;
}

LRESULT CALLBACK DX11Backend::hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

    // Handle hotkeys
    if (uMsg == WM_KEYDOWN)
    {
        GUI& gui = GUI::getInstance();
    auto& hotkeys = HotkeyManager::getInstance();
    if (cheat::FeatureManager::getInstance().onKeyDown((int)wParam)) return true;
    const int toggle = VK_F2;
        if ((int)wParam == toggle) { gui.setVisible(!gui.isVisible()); return true; }
    }

    // TODO: Handle hotkeys later properly
    if (s_instance->onInput(uMsg, wParam, lParam)) return true;

    // Handle specific messages
    switch (uMsg)
    {
        case WM_SIZE:
            if (wParam != SIZE_MINIMIZED) s_instance->onResize(LOWORD(lParam), HIWORD(lParam));
            break;
    }

    // Call original window procedure
    return CallWindowProc(m_originalWndProc, hWnd, uMsg, wParam, lParam);
}

bool DX11Backend::setupHooks()
{
    HWND tempWindow = utils::DXUtils::createTempWindow();
    if (!tempWindow) return false;

    ID3D11Device* tempDevice = nullptr;
    ID3D11DeviceContext* tempContext = nullptr;
    IDXGISwapChain* tempSwapChain = nullptr;

    if (!utils::DXUtils::createTempD3D11Device(tempWindow, &tempDevice, &tempContext, &tempSwapChain))
    {
        utils::DXUtils::destroyTempWindow(tempWindow);
        return false;
    }

    // Get vtable function addresses
    void* presentAddr = getVTableFunction(tempSwapChain, 8);
    void* resizeBuffersAddr = getVTableFunction(tempSwapChain, 13);

    // Clean up
    tempSwapChain->Release();
    tempContext->Release();
    tempDevice->Release();
    utils::DXUtils::destroyTempWindow(tempWindow);

    // Create hooks
    auto& hookManager = HookManager::getInstance();

    bool success = true;
    success &= hookManager.createHook(presentAddr, hookedPresent, &m_originalPresent);
    success &= hookManager.createHook(resizeBuffersAddr, hookedResizeBuffers, &m_originalResizeBuffers);

    return success;
}

void DX11Backend::setupWindowHook()
{
    if (m_window && !m_originalWndProc) m_originalWndProc = (WNDPROC)SetWindowLongPtrW(
        m_window, GWLP_WNDPROC, (LONG_PTR)hookedWndProc);
}

void* DX11Backend::getVTableFunction(void* instance, int index)
{
    if (!instance) return nullptr;

    return (*static_cast<void***>(instance))[index];
}

HRESULT WINAPI DX11Backend::hookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
{
    static bool initialized = false;

    if (s_instance && !initialized)
    {
        s_instance->m_swapChain = swapChain;

        if (s_instance->getDeviceAndContext())
        {
            DXGI_SWAP_CHAIN_DESC desc;
            swapChain->GetDesc(&desc);
            s_instance->m_window = desc.OutputWindow;

            // Hook window procedure after we have the real window
            s_instance->setupWindowHook();

            // Initialize ImGui
            s_instance->initializeImGui();
            initialized = true;
        }
    }

    if (s_instance && s_instance->m_imguiInitialized && Renderer::getInstance().isInitialized())
    {
        s_instance->beginFrame();
        s_instance->renderImGui();
        s_instance->endFrame();
    }

    return m_originalPresent(swapChain, syncInterval, flags);
}

HRESULT WINAPI DX11Backend::hookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount,
                                                UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
    if (s_instance && s_instance->m_imguiInitialized) s_instance->cleanupRenderTarget();

    HRESULT result = m_originalResizeBuffers(swapChain, bufferCount, width, height, newFormat, swapChainFlags);

    if (s_instance && SUCCEEDED(result) && s_instance->m_imguiInitialized) s_instance->createRenderTarget();

    return result;
}

bool DX11Backend::getDeviceAndContext()
{
    if (m_device && m_context) return true;

    if (!m_swapChain) return false;

    HRESULT hr = m_swapChain->GetDevice(IID_PPV_ARGS(&m_device));
    if (FAILED(hr)) return false;

    m_device->GetImmediateContext(&m_context);

    // Get window handle
    DXGI_SWAP_CHAIN_DESC desc;
    m_swapChain->GetDesc(&desc);
    m_window = desc.OutputWindow;

    return createRenderTarget();
}

bool DX11Backend::createRenderTarget()
{
    if (!m_swapChain || !m_device) return false;

    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(hr) || !backBuffer) return false;

    hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
    backBuffer->Release();

    return SUCCEEDED(hr);
}

void DX11Backend::cleanupRenderTarget()
{
    if (m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
}

bool DX11Backend::initializeImGui()
{
    if (m_imguiInitialized || !m_device || !m_context || !m_window) return false;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;

    io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(
        NotoSans_compressed_data,
        *NotoSans_compressed_data,
        15.0f,
        &fontConfig,
        io.Fonts->GetGlyphRangesDefault()
        );

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX11_Init(m_device, m_context);

    m_imguiInitialized = true;
    return true;
}

void DX11Backend::shutdownImGui()
{
    if (!m_imguiInitialized) return;

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_imguiInitialized = false;
}

void DX11Backend::beginFrame()
{
    if (!m_imguiInitialized || !m_context) return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void DX11Backend::endFrame()
{
    if (!m_imguiInitialized || !m_context || !m_renderTargetView) return;

    ImGui::EndFrame();
    ImGui::Render();

    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DX11Backend::renderImGui()
{
    if (m_imguiInitialized)
    {
        GUI::getInstance().render();
    }
}

void DX11Backend::onResize(int width, int height)
{
}

int DX11Backend::onInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();

    auto isKeyboardMessage = [](UINT msg) -> bool
    {
        switch (msg)
        {
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_CHAR:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                return true;
            default:
                return false;
        }
    };

    auto isInputMessage = [](UINT msg) -> bool
    {
        switch (msg)
        {
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_CHAR:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:

            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
                return true;

            default:
                return false;
        }
    };

    if (io.WantCaptureKeyboard && isKeyboardMessage(msg))
    {
        return 1;
    }

    if (io.WantCaptureMouse && isInputMessage(msg))
    {
        return 1;
    }

    // TODO: Handle input.
    return 0;
}
