#include "OverlayWindowsD3D.h"

using namespace FOnline;

const string TextMessage[] =
{
	""
};

namespace IndexMessage
{
	enum Enum
	{
		None
	};
};

inline void Message( const IndexMessage::Enum index )
{
	WriteLog( "%s\n", TextMessage[ ( size_t )index ].c_str( ) );
}

Overlay* FOnline::CreateOverlay( )
{
	OverlayWindows* windowsOverlay = new OverlayWindowsD3D( );

	return windowsOverlay;
}

void FOnline::DestroyOverlay( Overlay* overlay )
{
	if( overlay )
		delete overlay;
}

OverlayWindowsD3D::OverlayWindowsD3D( ): g_pD3D( nullptr ), g_pd3dDevice( nullptr ), g_d3dpp( )
{

}

void OverlayWindowsD3D::Finish( )
{
	OverlayWindows::Finish( );
}

void FOnline::OverlayWindowsD3D::CreateDevice( )
{
	auto window = GetWindow( );
	if( !window )
	{
		WriteLog( "Error create overlay device: invalid window.\n" );
		return;
	}
	if( ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) == NULL )
	{
		WriteLog( "Error create overlay device #0.\n" );
		return;
	}
	ZeroMemory( &g_d3dpp, sizeof( g_d3dpp ) );
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	auto result = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetWindow( ), D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice );
	string result_str = "";
	switch( result )
	{
		case D3DERR_DEVICELOST:
			result_str = "error <device lost>";
			break;
		case D3DERR_INVALIDCALL:
			result_str = "error <invalid call>";
			break;
		case D3DERR_NOTAVAILABLE:
			result_str = "error <not avialable>";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			result_str = "error <out of video memory>";
			break;
		case D3D_OK:
			result_str = "complete";
			break;
		default:
			result_str = "error <unknow>";
			break;
	}

	if( result != D3D_OK )
	{
		if( g_pD3D )
		{
			g_pD3D->Release( );
			g_pD3D = NULL;
		}
	}
	WriteLog( "Create overlay device %s.\n", result_str.c_str() );
}

Device_ FOnline::OverlayWindowsD3D::GetDevice( )
{
	return g_pd3dDevice;
}

void FOnline::OverlayWindowsD3D::DestroyDevice( )
{
	if( g_pd3dDevice ) 
	{ 
		g_pd3dDevice->Release( );
		g_pd3dDevice = NULL;
	}

	if( g_pD3D )
	{
		g_pD3D->Release( );
		g_pD3D = NULL;
	}
}

void FOnline::OverlayWindowsD3D::ResetDevice( )
{
	HRESULT hr = g_pd3dDevice->Reset( &g_d3dpp );
}

void FOnline::OverlayWindowsD3D::Render( )
{
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
	
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BackGroundColor, 1.0f, 0 );
	if( g_pd3dDevice->BeginScene( ) >= 0 )
	{
		GetImgui( )->RenderIface();
		g_pd3dDevice->EndScene( );
	}
	HRESULT result = g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	if( result != D3D_OK )
	{
		// Handle loss of D3D9 device
		if( result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel( ) == D3DERR_DEVICENOTRESET )
			ResetDevice( );
		WriteLog( "D3D Present error %u\n", result );
	}
}

void OverlayWindowsD3D::Init( )
{
	OverlayWindows::Init( );
	ImVec4 color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
	BackGroundColor = D3DCOLOR_RGBA( ( int )( color.x*color.w*255.0f ), ( int )( color.y*color.w*255.0f ), ( int )( color.z*color.w*255.0f ), ( int )( color.w*255.0f ) );
}
