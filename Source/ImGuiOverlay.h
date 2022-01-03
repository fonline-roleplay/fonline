#ifndef IM_GUI_OVERLAY_H
#define IM_GUI_OVERLAY_H
#define OVERLAY_OFF
#include "StdAfx.h"
#include "FonlineImgui.h"

namespace FOnline
{
	struct OverlayFlag;
	class Overlay;
	
	extern Overlay* CreateOverlay( );
	extern void DestroyOverlay( Overlay* overlay );
	extern Overlay* GetOverlay( );
	extern void FinishOverlay( );
	
	struct OverlayFlag
	{
		bool IsInit : 1;
		bool IsFinish : 1;
	};

	class Overlay
	{
		friend Overlay* GetOverlay( );
		friend void FinishOverlay( );

		virtual void Finish( ) = 0;

	protected:
		Overlay( );

		const OverlayFlag* GetFlag( );
	
	public:
		virtual void Init( ) = 0;
	};
}

#endif
