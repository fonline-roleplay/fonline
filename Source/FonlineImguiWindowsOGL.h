#ifndef FONLINE_IMGUI_WINDOWSOGL_H
#define FONLINE_IMGUI_WINDOWSOGL_H
#include "StdAfx.h"
#include "FonlineImguiWindows.h"

namespace FOnline
{
	class FonlineImguiWindowsOGL: public FonlineImguiWindows
	{
		FonlineImguiWindowsOGL( ): FonlineImguiWindows( "error" ) {}

		void InitGraphics( Device_ device ) override;
		void FinishGraphics( ) override;
		void NewFrameGraphics( ) override;
		void RenderGraphics( ) override;

	public:
		FonlineImguiWindowsOGL( std::string name ): FonlineImguiWindows( name ) {}
	};
#endif // FONLINE_IMGUI_WINDOWSOGL_H
}