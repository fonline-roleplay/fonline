#include "FonlineImgui.h"
#include "ImGuiOverlay.h"

using namespace FOnline;

ImFontAtlas FOnline::FonlineImgui::FontAtlas;

std::vector<FonlineImgui*> ScriptInstance;
std::vector<FonlineImgui*> Pool;

void FonlineImgui::ScriptFrame( FOWindow* window )
{

}

void FOnline::FonlineImgui::WorkContext( std::string info )
{
	auto context = ImGui::GetCurrentContext( );
	if( context == Context )
	{
		if( !WorkCounter )
		{
			WriteLog( "Error imgui '%s_%s' <work context counter>, current context invalid\n", Name.c_str(), info.c_str() );
			return;
		}
		WorkCounter++;
		return;
	}
	if( ImGui::GetCurrentContext( ) != nullptr )
	{
		WriteLog( "Error imgui '%s_%s' <work context>, current context invalid\n", Name.c_str( ), info.c_str( ) );
		return;
	}
	WorkCounter = 1;
	ImGui::SetCurrentContext( Context );
}

void FOnline::FonlineImgui::DropContext( std::string info )
{
	if( ImGui::GetCurrentContext( ) != Context )
	{
		WriteLog( "Error imgui '%s_%s' <drop context>, current context invalid\n", Name.c_str( ), info.c_str( ) );
		return;
	}

	if( !WorkCounter )
	{
		WriteLog( "Error imgui '%s_%s' <drop context counter>, current context invalid\n", Name.c_str( ), info.c_str( ) );
	}
	else if( !--WorkCounter )
	{
		ImGui::SetCurrentContext( nullptr );
	}
}

FonlineImgui::FonlineImgui( std::string name ) : Context( nullptr ), IsDemoWindow( false ), Window( nullptr ), WorkCounter( 0 ), Name( name ), InitFlag( false )
{
}

void FonlineImgui::ShowDemo( )
{
	IsDemoWindow = true;
}

void FonlineImgui::Init( ImGuiWindowsData window, Device_ device )
{
	// imgui
	bool isMain = IsMain( );
	if( isMain )
	{
		IMGUI_CHECKVERSION( );
		WriteLog( "Init imgui. Version: %s.\n", IMGUI_VERSION );
	}
	WriteLog( "Init imgui_%s.\n", Name.c_str() );

	Context = ImGui::CreateContext( &FontAtlas );
	WorkCounter = 1;

	if( isMain )
	{
		ImGuiIO& io = GetIO( );
		io.IniFilename = "FOnlineImgui.ini";
		io.LogFilename = "FOnlineImgui.log";

		IsDemoWindow = Str::Substring( CommandLine, "-ImguiDemo" ) != nullptr;
		io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 12.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic( ) );
	}

	InitOS( window );
	InitGraphics( device );
	Window = window;

	DropContext( "Init" );
	if( isMain )
	{
	#ifndef OVERLAY_OFF
		 GetOverlay( )->Init();
	#endif
	}
	InitFlag = true;
}

void FonlineImgui::Finish( )
{
	WorkContext( "Finish" );
	FinishGraphics( );
	FinishOS( );
	ImGui::SetCurrentContext( nullptr );
	ImGui::DestroyContext( Context );
	if( IsMain( ) )
	{
	#ifndef OVERLAY_OFF
		FinishOverlay( );
	#endif
	}
}

void FonlineImgui::Frame( FOWindow* window )
{
	NewFrame( );

	ScriptFrame( window );
	if( IsDemoWindow )
		ImGui::ShowDemoWindow( &IsDemoWindow );

	EndFrame( );
}

void FonlineImgui::RenderIface( )
{
	WorkContext( "RenderIface" );
	ImGui::Render( );
	RenderGraphics( );
	DropContext( "RenderIface" );
}

void FonlineImgui::NewFrame( )
{
	WorkContext( "NewFrame" );
	NewFrameGraphics( );
	NewFrameOS( );
	ImGui::NewFrame( );
}

void FonlineImgui::EndFrame( )
{
	ImGui::EndFrame( );
	DropContext( "EndFrame" );
}

void FonlineImgui::RenderAll( )
{
	GetMainImgui( )->RenderIface( );
	for( uint i = 0, iend = ScriptInstance.size(); i < iend; i++ )
	{
		ScriptInstance[i]->RenderIface( );
	}
}

char * FOnline::FormatText( char * text, FormatTextFlag flag, uint & color )
{
	char* result = &text[ 0 ];
	char* big_buf = Str::GetBigBuf( );
	big_buf[ 0 ] = 0;
	while( *result )
	{
		const char* s0 = result;
		Str::GoTo( result, '|' );
		char* s1 = result;
		Str::GoTo( result, ' ' );
		char* s2 = result;

		uint d_len = ( uint )s2 - ( uint )s1 + 1;
		if( d_len )
		{
			color = strtoul( s1 + 1, NULL, 0 );
		}

		*s1 = 0;
		Str::Append( big_buf, 0x10000, s0 );

		if( !*result )
			break;
		result++;
	}

	unsigned char* buffer = ( unsigned char* )&result[ 0 ];
	const unsigned char* c = ( const unsigned char* )&big_buf[ 0 ];
	while( *c )
	{
		if( *c == '\n' && flag.OnlyLastLine )
		{
			buffer = ( unsigned char* )&result[ 0 ];
		}
		else
		{
			if( *c < 0x0080 )
			{
				*buffer++ = *c;
			}
			else
			{
				if( *c < 240 )
				{
					*buffer++ = 208;
					*buffer++ = *c - 48;
				}
				else
				{
					*buffer++ = 209;
					*buffer++ = *c - 112;
				}
			}
		}
		c++;
	}
	buffer[ 0 ] = 0;
	return result;
}
