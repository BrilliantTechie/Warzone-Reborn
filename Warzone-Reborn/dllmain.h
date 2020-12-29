#pragma once
#include "Main.hpp"

typedef long(__fastcall* Present)(IDXGISwapChain*, UINT, UINT);
static Present oPresent = NULL;

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);

struct FrameContext {
	ID3D12CommandAllocator*		CommandAllocator;
	UINT64						FenceValue;
};

namespace Type {
	typedef void(CALLBACK* tInterface)(ImFont*);
};

namespace d3d12 {
	IDXGISwapChain3*			  pSwapChain;
	ID3D12CommandQueue*			  pCommandQueue;
	ID3D12Device*				  pDevice;
	ID3D12Fence*				  pFence;
	ID3D12DescriptorHeap*		  d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12DescriptorHeap*		  d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12DescriptorHeap*		  pSrvDescHeap = nullptr;
	ID3D12DescriptorHeap*		  pRtvDescHeap = nullptr;
	ID3D12GraphicsCommandList*	  pCommandList;

	FrameContext*				  FrameContextArray;
	ID3D12Resource**			  pID3D12ResourceArray;
	D3D12_CPU_DESCRIPTOR_HANDLE*  RenderTargetDescriptorArray;

	HANDLE						  hFenceEvent;
	HANDLE						  hSwapChainWaitableObject;

	UINT						  NUM_BACK_BUFFERS;
	UINT						  NUM_FRAMES_IN_FLIGHT;

	UINT						  frame_index = 0;
	UINT64						  fenceLastSignaledValue = 0;
};

namespace imgui {
	bool	bIsReady = false;
	bool	bIsImguiNeedToBeReset = false;

	void InitD3D12(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue);
	void _Clear();
	void Clear();
	FrameContext* WaitForNextFrameResources();
	ImFont* Start(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue);
	ImFont* AddFont(const char* font_path, float font_size);
	void ImguiFrameEnd();
	void ImguiNoBorder(Type::tInterface aimbot_function, ImFont* font);
};

namespace Window {
	tWndProc WndProc;
};