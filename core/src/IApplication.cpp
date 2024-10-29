#include "../include/IApplication.h"
#include "../include/OpenGLRenderer.h"

IApplication* IApplication::m_pApp = nullptr;

IApplication::IApplication() :
	m_iWidth(0),
	m_iHeight(0),
	m_bActive(false),
	m_Window(nullptr)
{
	m_pApp = this;
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

		m_pRenderer = std::make_unique<OpenGLRenderer>();

		if (!m_pRenderer->Create()) {
			return false;
		}

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

			OnUpdate(m_Timer.GetElapsedSeconds());
			OnDraw(*m_pRenderer);

			m_pRenderer->Flip();
		}
	}

	OnDestroy();
	m_pRenderer = nullptr;
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