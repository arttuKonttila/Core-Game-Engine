#include "../include/IApplication.h"
#include "../include/OpenGLRenderer.h"

IApplication* IApplication::m_pApp = nullptr;

IApplication::IApplication() :
	m_iWidth(0),
	m_iHeight(0),
	m_bActive(false),
	m_vMousePosDelta(0.0f),
	m_Window(nullptr)
{
	m_pApp = this;
	std::memset(m_MouseButtonStates, 0, 8);
}

IApplication::~IApplication()
{
	m_pApp = nullptr;
}

bool IApplication::Create(int32_t resX, int32_t resY, const std::string& title)
{
	m_Window = MakeWindow(resX, resY, title);
	if (m_Window) {
		m_iWidth = resX;
		m_iHeight = resY;

		// init mouse
		InitMouse(m_Window);

		// create render object
		m_pRenderer = std::make_unique<OpenGLRenderer>();

		if (!m_pRenderer->Create()) {
			return false;
		}

		// call OnCreate
		if (OnCreate()) {
			SetActive(true);
			return true;
		}
	}

	return false;
}

void IApplication::Run()
{
	MSG msg;
	BOOL gotMsg = false;
	::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	while (msg.message != WM_QUIT) {
		if (IsActive()) {
			gotMsg = ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
		}
		else {
			gotMsg = ::GetMessage(&msg, nullptr, 0, 0);
		}

		if (gotMsg) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (msg.message != WM_QUIT) {
			m_Timer.EndTimer();
			m_Timer.BeginTimer();

			// read mouse status
			m_vMousePosDelta = ReadMouse(m_MouseButtonStates);

			OnUpdate(m_Timer.GetElapsedSeconds());
			OnDraw(*m_pRenderer);

			m_pRenderer->Flip();
		}
	}

	// release mouse and direct input
	ReleaseMouse();

	OnDestroy();
	m_pRenderer = nullptr;
}

void IApplication::Close()
{
	::PostQuitMessage(0);
}

void IApplication::SetActive(bool set)
{
	m_bActive = set;
	m_Timer.BeginTimer();
}

HWND IApplication::MakeWindow(int32_t width, int32_t height, const std::string& title)
{
	// configure window
	HINSTANCE inst = ::GetModuleHandle(nullptr);
	DWORD windowStyle = 0;

	windowStyle = WS_OVERLAPPED |
		WS_CAPTION |
		WS_SYSMENU |
		WS_THICKFRAME |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX;

	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.hInstance = inst;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = L"GRAPHICSENGINE_WNDCLASS";

	if (!::RegisterClass(&wc)) {
		Debug("Failed to register window class, exiting...");
		return nullptr;
	}

	// create window
	auto widestr = std::wstring(title.begin(), title.end());
	HWND window = ::CreateWindow(
		wc.lpszClassName,
		widestr.c_str(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		nullptr,
		nullptr,
		inst,
		nullptr
	);

	if (!window) {
		Debug("Failed to create window, exiting...");
		return nullptr;
	}

	// resize window
	::SetWindowLong(window, GWL_STYLE, windowStyle);
	RECT clientArea = { 0, 0, width, height };
	::AdjustWindowRectEx(&clientArea, windowStyle, FALSE, 0);
	::SetWindowPos(
		window,
		nullptr,
		0,
		0,
		clientArea.right - clientArea.left,
		clientArea.bottom - clientArea.top,
		SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW
	);
	::UpdateWindow(window);
	::ShowWindow(window, SW_SHOWNORMAL);

	return window;
}

long __stdcall IApplication::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
		break;

	case WM_CREATE:
		::SetForegroundWindow(hwnd);
		break;

	case WM_CLOSE:
		::DestroyWindow(hwnd);
		return 0;
		break;

	default:
		break;
	};

	bool callDefWndProc = true;
	auto app = IApplication::GetApp();
	if (app) {
		callDefWndProc = !app->OnEvent(message, wParam, lParam);
	}

	if (callDefWndProc) {
		return (long) ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

// high performance mouse with DirectInput
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

LPDIRECTINPUT8 g_pDI = nullptr;
LPDIRECTINPUTDEVICE8 g_pMouse = nullptr;

void IApplication::InitMouse(HWND hwnd)
{
	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&g_pDI, NULL);
	g_pDI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL);
	g_pMouse->SetDataFormat(&c_dfDIMouse2);
	g_pMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	g_pMouse->Acquire();
}

glm::vec2 IApplication::ReadMouse(uint8_t* buttons)
{
	glm::vec2 mouseDelta(0.0f);

	if (g_pMouse) {
		DIMOUSESTATE2 dims2;
		std::memset(&dims2, 0, sizeof(dims2));

		HRESULT hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &dims2);
		if (FAILED(hr)) {
			hr = g_pMouse->Acquire();
		}
		else {
			mouseDelta.x = (float)dims2.lX;
			mouseDelta.y = (float)dims2.lY;
			std::memcpy(buttons, dims2.rgbButtons, 8);
		}
	}

	return mouseDelta;
}

void IApplication::ReleaseMouse()
{
	if (g_pMouse) {
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse = nullptr;
	}

	if (g_pDI) {
		g_pDI->Release();
		g_pDI = nullptr;
	}
}

bool IApplication::OnEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED) {
			SetActive(false);
		}
		else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) {
			RECT rect;
			::GetClientRect(GetWindow(), &rect);
			const int32_t windowWidth = rect.right - rect.left;
			const int32_t windowHeight = rect.bottom - rect.top;

			if (windowWidth != m_iWidth || windowHeight != m_iHeight) {
				m_iWidth = windowWidth;
				m_iHeight = windowHeight;
				if (m_pRenderer) {
					m_pRenderer->SetViewport({ 0, 0, m_iWidth, m_iHeight });
					OnScreenChanged(m_iWidth, m_iHeight);
				}
			}

			SetActive(true);
		}
		break;

	case WM_KEYDOWN:
		OnKeyDown((uint32_t)wParam);
		break;

	case WM_LBUTTONDOWN:
		OnMouseBegin(0, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MBUTTONDOWN:
		OnMouseBegin(1, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_RBUTTONDOWN:
		OnMouseBegin(2, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_LBUTTONUP:
		OnMouseEnd(0, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MBUTTONUP:
		OnMouseEnd(1, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_RBUTTONUP:
		OnMouseEnd(2, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MOUSEMOVE:
	{
		int32_t pointerIndex = -1;
		if ((wParam & MK_LBUTTON) != 0) pointerIndex = 0;
		else if ((wParam & MK_MBUTTON) != 0) pointerIndex = 1;
		else if ((wParam & MK_RBUTTON) != 0) pointerIndex = 2;
		if (pointerIndex != -1) {
			OnMouseDrag(pointerIndex, glm::vec2(LOWORD(lParam), HIWORD(lParam)));
		}
	}
	break;

	default:
		break;
	}

	return false;
}

void IApplication::Debug(const wchar_t* msg)
{
	::OutputDebugStringW(msg);
}

void IApplication::Debug(const char* msg)
{
	::OutputDebugStringA(msg);
}

void IApplication::Debug(const std::string& msg)
{
	::OutputDebugStringA(msg.c_str());
}

bool IApplication::IsKeyDown(int32_t keyCode)
{
	return ::GetAsyncKeyState(keyCode);
}

bool IApplication::IsMouseButtonDown(int32_t buttonIndex) const
{
	return (buttonIndex < 8) ? m_MouseButtonStates[buttonIndex] : false;
}