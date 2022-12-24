#include "OverlayWindows.h"

#include "windowsx.h"
#include "backends/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#include <Windows.h>
#include <Commdlg.h>

using namespace FOnline;

const string TextMessage[] =
{
	"Init overlay.",
	"Finish overlay."
};

namespace IndexMessage
{
	enum Enum
	{
		Info_Init,
		Info_Finish
	};
};

inline void Message( const IndexMessage::Enum index )
{
	WriteLog( "%s\n", TextMessage[ ( size_t )index ].c_str( ) );
}

OverlayWindows::OverlayWindows( ) : Imgui( nullptr ), Window( nullptr )
{

}

void FOnline::FocusOverlay( )
{
	auto overlay = GetOverlay( );
	if( overlay )
	{
		auto win = overlay->GetWindow( );
		if( win != GetFocus( ) )
			SetFocus( win );
	}
}

void FOnline::LoopOverlay( )
{
	auto overlay = GetOverlay( );
	if( overlay )
	{
		overlay->Loop( );
	}
}

void OverlayWindows::Finish( )
{
	Message( IndexMessage::Info_Finish );
	DestroyImgui( Imgui );
	DestroyDevice( );

}

void FOnline::OverlayWindows::Loop( )
{
	if( Window )
	{
		MSG msg;
		while( ::PeekMessage( &msg, Window, 0U, 0U, PM_REMOVE ) )
		{
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			if( msg.message == WM_QUIT )
			{

			}
		}
		
		//GetImgui( )->Frame( nullptr );
		//Render( );
	}
}

inline OverlayWindows* GetOverlayWindows( )
{
	return ( OverlayWindows* )GetOverlay( );
}

LRESULT CALLBACK ChildProc( HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam )
{
	auto overlay = GetOverlayWindows( );
	overlay->mutex.Lock( );
	auto result = overlay->Process( hwnd, Message, wparam, lparam );
	overlay->mutex.Unlock( );
	return result;
}

LRESULT CALLBACK FOnline::OverlayWindows::Process( HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam )
{
	if( !Window || !Imgui->IsInit( ) )
		return ::DefWindowProc( hwnd, Message, wparam, lparam );

	Imgui->WorkContext( "WinProcess" );
	auto result = ImGui_ImplWin32_WndProcHandler( hwnd, Message, wparam, lparam );
	Imgui->DropContext( "WinProcess" );
	if( result )
		return true;

	switch( Message )
	{
		case WM_SIZE:
			/*if( GetDevice( ) != NULL && wparam != SIZE_MINIMIZED )
			{
				g_d3dpp.BackBufferWidth = LOWORD( lparam );
				g_d3dpp.BackBufferHeight = HIWORD( lparam );
				// ResetDevice( );
			}*/
			return 0;
		case WM_SYSCOMMAND:
			if( ( wparam & 0xfff0 ) == SC_KEYMENU ) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			// ::PostQuitMessage( 0 );
			return 0;
	}
	return ::DefWindowProc( hwnd, Message, wparam, lparam );
}

void OverlayWindows::InitWindow( )
{
	UnregisterClass( "FOnlineOverlay", GetModuleHandle( nullptr ) );
	WNDCLASS w = {};
	w.lpfnWndProc = ChildProc;
	w.hInstance = GetModuleHandle( nullptr );
	w.hbrBackground = GetStockBrush( WHITE_BRUSH );
	w.lpszClassName = "FOnlineOverlay";
	RegisterClass( &w );
	Window = CreateWindowEx( WS_EX_TOOLWINDOW | WS_EX_TOPMOST, "FOnlineOverlay", "FOnlineOverlay", WS_VISIBLE, 0, 0,
							 300, 600, nullptr, nullptr, GetModuleHandle( nullptr ), nullptr );
	if( !Window )
	{
		Window = ( ImGuiWindowsData )1;
		return;
	}
	ShowWindow( Window, SW_NORMAL );
}

void OverlayWindows::Init( )
{
	Message( IndexMessage::Info_Init );
	Imgui = CreateImgui( "Overlay" );
	FlMessage message = { FlMessageIndex::OverlayWindowInit, this };
	Fl::awake( &message );
	while( !Window || (int)Window == 1 )
	{
		if( ( int )Window == 1 )
			return;
		Sleep( 0 );
	}

	this->mutex.Lock( );
	CreateDevice( );
	Imgui->Init( Window, GetDevice( ) );
	this->mutex.Unlock( );
}

string WindowsExplorer_OpenFileName( const char* filter )
{
	OPENFILENAME ofn;
	char fileName[ MAX_PATH ] = "";
	ZeroMemory( &ofn, sizeof( ofn ) );
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrInitialDir = ".";

	if( GetOpenFileName( &ofn ) )
		return fileName;
	return "";
}