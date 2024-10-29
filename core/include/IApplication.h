#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <fstream>
#include <iterator>
#include <string_view>

#include "Timer.h"
#include "IRenderer.h"

class IApplication {
public:
	IApplication();
	~IApplication();

	bool Create(int32_t resX, int32_t resY, const std::string& title);

	void Run();

	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void OnUpdate(float frametime) = 0;
	virtual void OnDraw(IRenderer& renderer) = 0;

	inline int32_t GetWidth() const { return m_iWidth; }
	inline int32_t GetHeight() const { return m_iHeight; }
	inline HWND GetWindow() const { return m_Window; }

	inline bool IsActive() const { return m_bActive; }
	void SetActive(bool set);

	inline float GetAspect() const { return (float)m_iWidth / (float)m_iHeight; }

	inline float GetFrameTime() const { return m_Timer.GetElapsedSeconds(); }

	static IApplication* GetApp() { return m_pApp; }

	inline IRenderer* GetRenderer() { return m_pRenderer.get(); }

	static void Debug(const wchar_t* msg);
	static void Debug(const char* msg);
	static void Debug(const std::string& msg);

protected:
	virtual void OnScreenChanged(int32_t widthPixels, int32_t heightPixels) {}

	virtual bool OnEvent(UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND MakeWindow(int32_t width, int32_t height, const std::string& title);
	static long WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	// app data
	int32_t						m_iWidth;
	int32_t						m_iHeight;
	bool						m_bActive;
	Timer						m_Timer;

	HWND						m_Window;

	std::unique_ptr<IRenderer>	m_pRenderer;

	static IApplication* m_pApp;
};