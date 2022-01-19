#include "OverlayWindowsOGL.h"

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
	OverlayWindows* windowsOverlay = new OverlayWindowsOGL( );

	return windowsOverlay;
}

void FOnline::DestroyOverlay( Overlay* overlay )
{
	if( overlay )
		delete overlay;
}

OverlayWindowsOGL::OverlayWindowsOGL( )
{

}

void FOnline::OverlayWindowsOGL::Render( )
{

}

void OverlayWindowsOGL::Finish( )
{
	OverlayWindows::Finish( );
}

void FOnline::OverlayWindowsOGL::CreateDevice( )
{
	
}

Device_ FOnline::OverlayWindowsOGL::GetDevice( )
{
	return GLuint(0);
}

void FOnline::OverlayWindowsOGL::DestroyDevice( )
{
	
}

void OverlayWindowsOGL::Init( )
{
	OverlayWindows::Init( );
}
