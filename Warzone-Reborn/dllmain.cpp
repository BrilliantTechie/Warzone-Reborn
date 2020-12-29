#include "dllmain.h"
#include "Renderer.h"
#include "LazyImporter.h"
#include "tahoma.ttf.h"
#include "Settings.h"
#include "Interface.h"
#include "GUI.h"
#include "SDK.h"
#include "Xor.hpp"

LRESULT hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case 0x403:
	case WM_SIZE:
		if (uMsg == WM_SIZE && wParam == SIZE_MINIMIZED)
			break;

		if (imgui::bIsReady) {
			imgui::Clear();
			ImGui_ImplDX12_InvalidateDeviceObjects();
			imgui::bIsImguiNeedToBeReset = true;
		}
		break;
	};

	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	return Window::WndProc(hWnd, uMsg, wParam, lParam);
}

void imgui::InitD3D12(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue) {
	d3d12::pSwapChain = pSwapChain;
	d3d12::pCommandQueue = pCommandQueue;

	if (!SUCCEEDED(d3d12::pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12::pDevice)))
		Exit();

	{
		DXGI_SWAP_CHAIN_DESC1 desc;

		if (!SUCCEEDED(d3d12::pSwapChain->GetDesc1(&desc)))
			Exit();

		d3d12::NUM_BACK_BUFFERS = desc.BufferCount;
		d3d12::NUM_FRAMES_IN_FLIGHT = desc.BufferCount;
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = d3d12::NUM_BACK_BUFFERS;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;

		if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pRtvDescHeap))))
			Exit();
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;

		if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pSrvDescHeap))))
			Exit();
	}

	if (!SUCCEEDED(d3d12::pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12::pFence))))
		Exit();

	d3d12::FrameContextArray = new FrameContext[d3d12::NUM_FRAMES_IN_FLIGHT];
	d3d12::pID3D12ResourceArray = new ID3D12Resource * [d3d12::NUM_BACK_BUFFERS];
	d3d12::RenderTargetDescriptorArray = new D3D12_CPU_DESCRIPTOR_HANDLE[d3d12::NUM_BACK_BUFFERS];

	for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i) {
		if (!SUCCEEDED(d3d12::pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12::FrameContextArray[i].CommandAllocator))))
			Exit();
	}

	SIZE_T nDescriptorSize = d3d12::pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12::pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i) {
		d3d12::RenderTargetDescriptorArray[i] = rtvHandle;
		rtvHandle.ptr += nDescriptorSize;
	}

	if (!SUCCEEDED(d3d12::pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12::FrameContextArray[0].CommandAllocator, NULL, IID_PPV_ARGS(&d3d12::pCommandList))) ||
		!SUCCEEDED(d3d12::pCommandList->Close()))
	{
		Exit();
	}

	d3d12::hSwapChainWaitableObject = d3d12::pSwapChain->GetFrameLatencyWaitableObject();
	d3d12::hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (d3d12::hFenceEvent == NULL)
		Exit();

	ID3D12Resource* pBackBuffer;
	for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i) {
		if (!SUCCEEDED(d3d12::pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer))))
			Exit();

		d3d12::pDevice->CreateRenderTargetView(pBackBuffer, NULL, d3d12::RenderTargetDescriptorArray[i]);
		d3d12::pID3D12ResourceArray[i] = pBackBuffer;
	}
}

void imgui::_Clear() {
	d3d12::pSwapChain = nullptr;
	d3d12::pDevice = nullptr;
	d3d12::pCommandQueue = nullptr;

	if (d3d12::pFence) {
		d3d12::pFence->Release();
		d3d12::pFence = nullptr;
	}

	if (d3d12::pSrvDescHeap) {
		d3d12::pSrvDescHeap->Release();
		d3d12::pSrvDescHeap = nullptr;
	}

	if (d3d12::pRtvDescHeap) {
		d3d12::pRtvDescHeap->Release();
		d3d12::pRtvDescHeap = nullptr;
	}

	if (d3d12::pCommandList) {
		d3d12::pCommandList->Release();
		d3d12::pCommandList = nullptr;
	}

	if (d3d12::FrameContextArray) {
		for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i) {
			if (d3d12::FrameContextArray[i].CommandAllocator) {
				d3d12::FrameContextArray[i].CommandAllocator->Release();
				d3d12::FrameContextArray[i].CommandAllocator = nullptr;
			}
		}

		delete[] d3d12::FrameContextArray;
		d3d12::FrameContextArray = NULL;
	}

	if (d3d12::pID3D12ResourceArray) {
		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i) {
			if (d3d12::pID3D12ResourceArray[i]) {
				d3d12::pID3D12ResourceArray[i]->Release();
				d3d12::pID3D12ResourceArray[i] = nullptr;
			}
		}

		delete[] d3d12::pID3D12ResourceArray;
		d3d12::pID3D12ResourceArray = NULL;
	}

	if (d3d12::RenderTargetDescriptorArray) {
		delete[] d3d12::RenderTargetDescriptorArray;
		d3d12::RenderTargetDescriptorArray = NULL;
	}

	if (d3d12::hSwapChainWaitableObject)
		d3d12::hSwapChainWaitableObject = nullptr;

	if (d3d12::hFenceEvent) {
		CloseHandle(d3d12::hFenceEvent);
		d3d12::hFenceEvent = nullptr;
	}

	d3d12::NUM_FRAMES_IN_FLIGHT = 0;
	d3d12::NUM_BACK_BUFFERS = 0;

	d3d12::frame_index = 0;
}

void imgui::Clear() {
	if (d3d12::FrameContextArray) {
		FrameContext* frameCtxt = &d3d12::FrameContextArray[d3d12::frame_index % d3d12::NUM_FRAMES_IN_FLIGHT];
		UINT64 fenceValue = frameCtxt->FenceValue;

		// No fence was signaled
		if (fenceValue == 0)
			return;

		frameCtxt->FenceValue = 0;
		
		bool bWait = d3d12::pFence->GetCompletedValue() >= fenceValue;
		if (!bWait) {
			d3d12::pFence->SetEventOnCompletion(fenceValue, d3d12::hFenceEvent);
			WaitForSingleObject(d3d12::hFenceEvent, INFINITE);
		}

		_Clear();
	}
}

FrameContext* imgui::WaitForNextFrameResources() {
	UINT nextFrameIndex = d3d12::frame_index + 1;
	d3d12::frame_index = nextFrameIndex;
	constexpr DWORD numWaitableObjects = 1;

	HANDLE waitableObjects[] = { d3d12::hSwapChainWaitableObject, NULL };
	FrameContext* frameCtxt = &d3d12::FrameContextArray[nextFrameIndex % d3d12::NUM_FRAMES_IN_FLIGHT];

	WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

	return frameCtxt;
}

ImFont* imgui::Start(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue) {
	static ImFont* sMainFont;

	if (bIsReady) {
		if (bIsImguiNeedToBeReset) {
			InitD3D12(pSwapChain, pCommandQueue);
			ImGui_ImplDX12_CreateDeviceObjects();

			bIsImguiNeedToBeReset = false;
		}

		return sMainFont;
	}

	InitD3D12(pSwapChain, pCommandQueue);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsClassic();

	ImGui_ImplWin32_Init(g_Data::hWind);
	ImGui_ImplDX12_Init(
		d3d12::pDevice, d3d12::NUM_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM, d3d12::pSrvDescHeap,
		d3d12::pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
		d3d12::pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

	ImFont* main_font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
	if (main_font == nullptr)
		Exit();

	sMainFont = main_font;

	// Hook WndProc
	Window::WndProc = (WNDPROC)SetWindowLongPtrW(g_Data::hWind, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

	bIsReady = true;

	return sMainFont;
}

ImFont* imgui::AddFont(const char* font_path, float font_size) {
	if (!bIsReady)
		return nullptr;

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
	if (font == nullptr)
		return 0;

	return font;
}

void imgui::ImguiFrameEnd() {
	FrameContext* frameCtxt = WaitForNextFrameResources();
	UINT backBufferIdx = d3d12::pSwapChain->GetCurrentBackBufferIndex();

	{
		frameCtxt->CommandAllocator->Reset();
		static D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];
		d3d12::pCommandList->Reset(frameCtxt->CommandAllocator, NULL);
		d3d12::pCommandList->ResourceBarrier(1, &barrier);
		d3d12::pCommandList->OMSetRenderTargets(1, &d3d12::RenderTargetDescriptorArray[backBufferIdx], FALSE, NULL);
		d3d12::pCommandList->SetDescriptorHeaps(1, &d3d12::pSrvDescHeap);
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12::pCommandList);

	static D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];

	d3d12::pCommandList->ResourceBarrier(1, &barrier);
	d3d12::pCommandList->Close();

	d3d12::pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&d3d12::pCommandList);

	UINT64 fenceValue = d3d12::fenceLastSignaledValue + 1;
	d3d12::pCommandQueue->Signal(d3d12::pFence, fenceValue);
	d3d12::fenceLastSignaledValue = fenceValue;
	frameCtxt->FenceValue = fenceValue;
}

void imgui::ImguiNoBorder(Type::tInterface Interface_Init, ImFont* font) {
	ImGuiStyle& style = ImGui::GetStyle();

	Interface_Init(font);

	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 10.0f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 12.0f; // Make all elements (check boxes, etc) circles
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 20.0f; // Make grab a circle
	style.GrabRounding = 12.0f;
	style.PopupRounding = 7.f;
	style.Alpha = 1.0;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.75f, 0.75f, 0.75f, 0.70f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
	colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.95f, 0.95f, 0.30f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.66f, 0.66f, 0.66f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.000f, 0.777f, 0.578f, 0.780f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.987f, 0.611f, 0.600f);
	colors[ImGuiCol_Button] = ImVec4(1.00f, 0.54f, 0.01f, 0.71f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.96f, 0.73f, 0.09f, 0.90f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.97f, 0.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(1.00f, 0.54f, 0.01f, 0.71f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.96f, 0.73f, 0.09f, 0.90f);
	colors[ImGuiCol_TabActive] = ImVec4(1.00f, 0.97f, 0.00f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

__declspec(dllexport)HRESULT hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!pSwapChain)
		return oPresent(pSwapChain, SyncInterval, Flags);

	ImFont* main_font = imgui::Start(
		static_cast<IDXGISwapChain3*>(pSwapChain),
		reinterpret_cast<ID3D12CommandQueue*>(*(QWORD*)(g_Data::base + OFFSET_DIRECTX_COMMAND_QUEUE)));

	// Frame Header
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Menu
	GUI::RenderMenu();

	// Interface Thread
	imgui::ImguiNoBorder(Interface::Init, main_font);

	imgui::ImguiFrameEnd();
	
	return oPresent(pSwapChain, SyncInterval, Flags);
}

int WINAPI DllMain(HMODULE _, DWORD reason, LPVOID __) {
	if (reason == DLL_PROCESS_ATTACH) {
		Settings::g_menuSettings.Load();
		g_Data::Init();

		if (Renderer::init(Renderer::RenderType::D3D12) == Renderer::Status::Success) {
			Renderer::bind(140, (void**)&oPresent, hkPresent);
		}
	}

	return TRUE;
}