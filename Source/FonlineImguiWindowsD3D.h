#ifndef FONLINE_IMGUI_WINDOWSD3D_H
#define FONLINE_IMGUI_WINDOWSD3D_H
#include "StdAfx.h"
#include "FonlineImguiWindows.h"

namespace FOnline
{
	class FonlineImguiWindowsD3D : public FonlineImguiWindows
	{
		FonlineImguiWindowsD3D( ): FonlineImguiWindows( "error" ) {}

		void InitGraphics( Device_ device ) override;
		void FinishGraphics( ) override;
		void NewFrameGraphics( ) override;
		void RenderGraphics( ) override;

	public:
		FonlineImguiWindowsD3D( std::string name ): FonlineImguiWindows( name ) {}
	};
#endif // FONLINE_IMGUI_WINDOWSD3D_H
}