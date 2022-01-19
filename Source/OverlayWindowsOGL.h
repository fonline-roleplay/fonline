#ifndef OVERLAY_WINDOWS_OGL_H
#define OVERLAY_WINDOWS_OGL_H
#include "StdAfx.h"
#include "OverlayWindows.h"

namespace FOnline
{
	class OverlayWindowsOGL: public FOnline::OverlayWindows
	{
		friend Overlay* FOnline::CreateOverlay( );

		OverlayWindowsOGL( );

		void Finish( ) override;

	protected:
		void CreateDevice( ) override;
		Device_ GetDevice( ) override;
		void DestroyDevice( ) override;

	public:
		void Init( ) override;
		void Render( ) override;
	};
}
#endif // !OVERLAY_WINDOWS_OGL_H
