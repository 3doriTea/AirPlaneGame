#pragma once
#include "string"
#include <d3d11.h>
#include <Windows.h>
#include <dxgi.h>
#include <Dxgi1_2.h>
//typedef struct tagWNDCLASSEXA WNDCLASSEX;
//typedef struct HWND__* HWND;
//typedef struct tagMSG MSG;
//struct RECT;
//struct IDXGISwapChain;
//struct ID3D11RenderTargetView;
//struct ID3D11Texture2D;
//struct ID3D11DepthStencilView;
//struct D3D11_VIEWPORT;



class WindowRenderContext
{
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WindowRenderContext();
	void Initialize(HWND windowHandle);
	void Release();
	HWND hWnd_;
	MSG* pPeekedMessage_;
	WNDCLASSEXW windowClass_;
	RECT windowRect_;
	IDXGISwapChain1* pSwapChain_;
	IDXGIOutput* pOutput_;
	ID3D11RenderTargetView* pRenderTargetView_;
	ID3D11Texture2D* pDepthStencil_;
	ID3D11DepthStencilView* pDepthStencilView_;
	D3D11_VIEWPORT viewport_;
	std::wstring windowTitle_;
	std::wstring windowClassName_;
	bool isActive_;

	int outputIndex_;
	static int outputCount;
};
