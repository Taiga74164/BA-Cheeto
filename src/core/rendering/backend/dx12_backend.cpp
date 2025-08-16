#include "pch.h"
#include "dx12_backend.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include "core/rendering/fonts/NotoSans.hpp"
#include "ui/gui.h"
#include "utils/dx_utils.h"
#include "utils/hotkey_manager.h"
#include "user/cheat/feature_manager.h"

DX12Backend* DX12Backend::s_instance = nullptr;
DX12Backend::Present_t DX12Backend::m_originalPresent = nullptr;
DX12Backend::ResizeBuffers_t DX12Backend::m_originalResizeBuffers = nullptr;
DX12Backend::ExecuteCommandLists_t DX12Backend::m_originalExecuteCommandLists = nullptr;
WNDPROC DX12Backend::m_originalWndProc = nullptr;

DX12Backend::DX12Backend()
{
    assert(s_instance == nullptr);
    s_instance = this;
}

DX12Backend::~DX12Backend()
{
    shutdown();
    s_instance = nullptr;
}

bool DX12Backend::initialize()
{
    if (m_initialized) return true;

    if (!HookManager::getInstance().initialize()) return false;

    if (!setupHooks()) return false;

    m_initialized = true;
    return true;
}

void DX12Backend::shutdown()
{
    if (!m_initialized) return;

    shutdownImGui();
    cleanupRenderTargets();
    cleanupDeviceObjects();

    if (m_window && m_originalWndProc)
    {
        SetWindowLongPtr(m_window, GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
        m_originalWndProc = nullptr;
    }

    m_device = nullptr;
    m_commandQueue = nullptr;
    m_swapChain = nullptr;

    m_initialized = false;
}

void* DX12Backend::getVTableFunction(void* instance, int index)
{
    if (!instance) return nullptr;
    return (*static_cast<void***>(instance))[index];
}

bool DX12Backend::setupHooks()
{
    HWND tempWindow = utils::DXUtils::createTempWindow();
    if (!tempWindow) return false;

    ID3D12Device* tempDevice = nullptr;
    ID3D12CommandQueue* tempQueue = nullptr;
    IDXGISwapChain3* tempSwapchain = nullptr;

    if (!utils::DXUtils::createTempD3D12Device(tempWindow, &tempDevice, &tempQueue, &tempSwapchain))
    {
        utils::DXUtils::destroyTempWindow(tempWindow);
        return false;
    }

    void* presentAddr = getVTableFunction(tempSwapchain, 8);
    void* resizeBuffersAddr = getVTableFunction(tempSwapchain, 13);

    tempSwapchain->Release();
    tempQueue->Release();
    tempDevice->Release();
    utils::DXUtils::destroyTempWindow(tempWindow);

    auto& hookManager = HookManager::getInstance();
    bool success = true;
    success &= hookManager.createHook(presentAddr, hookedPresent, &m_originalPresent);
    success &= hookManager.createHook(resizeBuffersAddr, hookedResizeBuffers, &m_originalResizeBuffers);

    return success;
}

void DX12Backend::setupWindowHook()
{
    if (m_window && !m_originalWndProc) m_originalWndProc = (WNDPROC)SetWindowLongPtrW(
        m_window, GWLP_WNDPROC, (LONG_PTR)hookedWndProc);
}

LRESULT CALLBACK DX12Backend::hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

    if (uMsg == WM_KEYDOWN)
    {
        GUI& gui = GUI::getInstance();
    auto& hotkeys = HotkeyManager::getInstance();
    if (cheat::FeatureManager::getInstance().onKeyDown((int)wParam)) return true;
    const int toggle = VK_F2;
        if ((int)wParam == toggle) { gui.setVisible(!gui.isVisible()); return true; }
    }

    return CallWindowProc(m_originalWndProc, hWnd, uMsg, wParam, lParam);
}

bool DX12Backend::getDeviceAndSwapchain(IDXGISwapChain* swapChain)
{
    if (!swapChain) return false;

    m_swapChain = (IDXGISwapChain3*)swapChain;

    DXGI_SWAP_CHAIN_DESC desc{};
    swapChain->GetDesc(&desc);
    m_window = desc.OutputWindow;

    // Try to capture command queue using ExecuteCommandLists hook if available later
    return true;
}

HRESULT WINAPI DX12Backend::hookedPresent(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags)
{
    if (s_instance && !s_instance->m_imguiInitialized)
    {
        if (!s_instance->getDeviceAndSwapchain(swapChain))
            return m_originalPresent(swapChain, SyncInterval, Flags);

        s_instance->setupWindowHook();

        s_instance->initializeImGui();
    }

    if (s_instance && s_instance->m_imguiInitialized)
    {
        s_instance->beginFrame();
        s_instance->renderImGui();
        s_instance->endFrame();
    }

    return m_originalPresent(swapChain, SyncInterval, Flags);
}

HRESULT WINAPI DX12Backend::hookedResizeBuffers(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    if (s_instance && s_instance->m_imguiInitialized)
        s_instance->cleanupRenderTargets();

    HRESULT hr = m_originalResizeBuffers(swapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    if (s_instance && SUCCEEDED(hr) && s_instance->m_imguiInitialized)
        s_instance->createRenderTargets();

    return hr;
}

void DX12Backend::onResize(int, int)
{
}

int DX12Backend::onInput(UINT, WPARAM, LPARAM)
{
    return 0;
}

bool DX12Backend::createDeviceObjects()
{
    if (!m_device)
    {
        if (FAILED(m_swapChain->GetDevice(IID_PPV_ARGS(&m_device))))
            return false;
    }

    if (!m_commandList)
    {
        if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, nullptr, nullptr, IID_PPV_ARGS(&m_commandList))))
        {
            m_commandList = nullptr;
            return false;
        }
        m_commandList->Close();
    }

    if (!m_fence)
    {
        if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))) return false;
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fenceEvent) return false;
    }

    return true;
}

void DX12Backend::cleanupDeviceObjects()
{
    if (m_fenceEvent) { CloseHandle(m_fenceEvent); m_fenceEvent = nullptr; }
    if (m_fence) { m_fence->Release(); m_fence = nullptr; }
    if (m_commandList) { m_commandList->Release(); m_commandList = nullptr; }
}

bool DX12Backend::createRenderTargets()
{
    if (!m_swapChain || !m_device) return false;

    DXGI_SWAP_CHAIN_DESC desc{};
    m_swapChain->GetDesc(&desc);
    m_frameCount = desc.BufferCount;

    if (!m_rtvHeap)
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = m_frameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap))))
            return false;
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    m_frames.resize(m_frameCount);
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < m_frameCount; i++)
    {
        FrameContext& ctx = m_frames[i];
        if (!ctx.allocator)
        {
            m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&ctx.allocator));
        }
        ID3D12Resource* backBuffer = nullptr;
        if (SUCCEEDED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
        {
            m_device->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);
            ctx.renderTarget = backBuffer;
            ctx.rtvHandle = rtvHandle;
            rtvHandle.ptr += m_rtvDescriptorSize;
        }
    }

    return true;
}

void DX12Backend::cleanupRenderTargets()
{
    for (auto& ctx : m_frames)
    {
        if (ctx.renderTarget)
        {
            ctx.renderTarget->Release();
            ctx.renderTarget = nullptr;
        }
        if (ctx.allocator)
        {
            ctx.allocator->Release();
            ctx.allocator = nullptr;
        }
    }
    m_frames.clear();

    if (m_rtvHeap) { m_rtvHeap->Release(); m_rtvHeap = nullptr; }
}

bool DX12Backend::initializeImGui()
{
    if (m_imguiInitialized || !m_swapChain) return false;

    if (!createDeviceObjects()) return false;
    if (!createRenderTargets()) return false;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
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

    D3D12_DESCRIPTOR_HEAP_DESC srvDesc = {};
    srvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvDesc.NumDescriptors = 1;
    srvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    if (FAILED(m_device->CreateDescriptorHeap(&srvDesc, IID_PPV_ARGS(&m_srvHeap)))) return false;

    ImGui_ImplWin32_Init(m_window);
    ImGui_ImplDX12_Init(m_device, m_frameCount,
                        DXGI_FORMAT_R8G8B8A8_UNORM, m_srvHeap,
                        m_srvHeap->GetCPUDescriptorHandleForHeapStart(),
                        m_srvHeap->GetGPUDescriptorHandleForHeapStart());

    m_imguiInitialized = true;
    return true;
}

void DX12Backend::shutdownImGui()
{
    if (!m_imguiInitialized) return;

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (m_srvHeap) { m_srvHeap->Release(); m_srvHeap = nullptr; }

    m_imguiInitialized = false;
}

void DX12Backend::beginFrame()
{
    if (!m_imguiInitialized) return;

    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void DX12Backend::endFrame()
{
    if (!m_imguiInitialized) return;

    ImGui::EndFrame();
    ImGui::Render();

    // Let ImGui backend handle command recording via ExecuteCommandLists hook when present
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), nullptr);
}

void DX12Backend::renderImGui()
{
    if (m_imguiInitialized)
    {
        GUI::getInstance().render();
    }
}

void DX12Backend::waitForFence(UINT64 value)
{
    if (!m_fence) return;
    if (m_fence->GetCompletedValue() < value)
    {
        m_fence->SetEventOnCompletion(value, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}
