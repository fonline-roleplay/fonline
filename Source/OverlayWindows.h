#ifndef OVERLAY_WINDOWS_H
#define OVERLAY_WINDOWS_H
#include "StdAfx.h"
#include "ImGuiOverlay.h"

namespace FOnline
{
	class OverlayWindows: public FOnline::Overlay
	{
		friend Overlay* FOnline::CreateOverlay( );

		FonlineImgui* Imgui;
		ImGuiWindowsData Window;

		Thread OverlayThread;

		static void WindowLoop(void*);

	protected:
		OverlayWindows( );

		virtual void CreateDevice( ) = 0;
		virtual Device_ GetDevice( ) = 0;
		virtual void DestroyDevice( ) = 0;

		virtual void Render( ) = 0;

		void Finish( ) override;

		inline FonlineImgui* GetImgui( ) { return Imgui; }

	public:
		ImGuiWindowsData GetWindow( ) override { return Window; }

		LRESULT CALLBACK Process( HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam );

		void Loop( ) override;
		void Init( ) override;
		void InitWindow( ) override;

		Mutex mutex;
	};
}
#endif // !OVERLAY_WINDOWS_H
