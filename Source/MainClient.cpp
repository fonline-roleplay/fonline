#include "StdAfx.h"
#include "Client.h"
#include "Exception.h"
#include "Version.h"
#include "Keyboard.h"
#include <locale.h>
#ifdef FO_LINUX
# include <signal.h>
#endif

#include "ImGuiOverlay.h"

FOWindow* MainWindow = NULL;
FOClient* FOEngine = NULL;
Thread    Game;
void GameThread( void* );

int main( int argc, char** argv )
{
    setlocale( LC_ALL, "Russian" );
	int is_multithreading = Fl::lock( );

    RestoreMainDirectory();

    // Threading
    #ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    #endif
    Thread::SetCurrentName( "GUI" );

    // Disable SIGPIPE signal
    #ifdef FO_LINUX
    signal( SIGPIPE, SIG_IGN );
    #endif

    // Exception
    CatchExceptions( "FOnline", CLIENT_VERSION );

    // Make command line
    SetCommandLine( argc, argv );

    // Stuff
    Timer::Init();
    LogToFile( "FOnline.log" );

    // Singleplayer mode initialization
    #ifdef FO_WINDOWS
    char full_path[ MAX_FOPATH ] = { 0 };
    char path[ MAX_FOPATH ] = { 0 };
    char name[ MAX_FOPATH ] = { 0 };
    GetModuleFileName( NULL, full_path, MAX_FOPATH );
    FileManager::ExtractPath( full_path, path );
    FileManager::ExtractFileName( full_path, name );
    if( Str::Substring( name, "Singleplayer" ) || Str::Substring( CommandLine, "Singleplayer" ) )
    {
        WriteLog( "Singleplayer mode.\n" );
        Singleplayer = true;
        Timer::SetGamePause( true );

        // Create interprocess shared data
        HANDLE map_file = SingleplayerData.Init();
        if( !map_file )
        {
            WriteLog( "Can't map shared data to memory.\n" );
            return 0;
        }

        // Fill interprocess initial data
        if( SingleplayerData.Lock() )
        {
            // Initialize other data
            SingleplayerData.NetPort = 0;
            SingleplayerData.Pause = true;

            SingleplayerData.Unlock();
        }
        else
        {
            WriteLog( "Can't lock mapped file.\n" );
            return 0;
        }

        // Config parsing
        IniParser cfg;
        char      server_exe[ MAX_FOPATH ] = { 0 };
        char      server_path[ MAX_FOPATH ] = { 0 };
        char      server_cmdline[ MAX_FOPATH ] = { 0 };
        cfg.LoadFile( GetConfigFileName(), PT_ROOT );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerAppName", "FOserv.exe", server_exe );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerPath", "..\\server\\", server_path );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerCommandLine", "", server_cmdline );

        // Process attributes
        PROCESS_INFORMATION server;
        memzero( &server, sizeof( server ) );
        STARTUPINFOA        sui;
        memzero( &sui, sizeof( sui ) );
        sui.cb = sizeof( sui );
        HANDLE client_process = OpenProcess( SYNCHRONIZE, TRUE, GetCurrentProcessId() );
        char   command_line[ 2048 ];

        // Start server
        Str::Format( command_line, "\"%s%s\" -singleplayer %p %p %s -logpath %s", server_path, server_exe, map_file, client_process, server_cmdline, path );
        if( !CreateProcess( NULL, command_line, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, server_path, &sui, &server ) )
        {
            WriteLog( "Can't start server process, error<%u>.\n", GetLastError() );
            return 0;
        }
        CloseHandle( server.hProcess );
        CloseHandle( server.hThread );
    }
    #endif

    // Init window threading
    #ifdef FO_LINUX
    XInitThreads();
    #endif

    // Check for already runned window
    #ifndef DEV_VESRION
    # ifdef FO_WINDOWS
    if( !Singleplayer && FindWindow( GetWindowName(), GetWindowName() ) != NULL )
    {
        ShowMessage( "FOnline is running already." );
        return 0;
    }
    # else // FO_LINUX
    // Todo: Linux
    # endif
    #endif

    // Options
    GetClientOptions();

    // Create window
    MainWindow = CreateMainWindow( );
    MainWindow->SetLabel( GetWindowName() );
	{
		int w, h;
		MainWindow->GetDesktopResolution( w, h );
		MainWindow->SetPosition( ( w - MODE_WIDTH ) / 2, ( h - MODE_HEIGHT ) / 2 );
	}
    MainWindow->SetSize( MODE_WIDTH, MODE_HEIGHT );
    // MainWindow->size_range( 100, 100 );

    // Icon
    #ifdef FO_WINDOWS
    MainWindow->SetIcon( (char*) LoadIcon( fl_display, MAKEINTRESOURCE( 101 ) ) );
    #else // FO_LINUX
    // Todo: Linux
    #endif

    // OpenGL parameters
    #ifndef FO_D3D
    Fl::gl_visual( FL_RGB | FL_RGB8 | FL_DOUBLE | FL_DEPTH | FL_STENCIL );
    #endif

    // Show window
    MainWindow->Show();

    // Hide cursor
    #ifdef FO_WINDOWS
    MainWindow->SetCursor(FL_CURSOR_NONE, FL_BLACK);
    #else
    char   data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    XColor black;
    black.red = black.green = black.blue = 0;
    Pixmap nodata = XCreateBitmapFromData( fl_display, fl_xid( MainWindow ), data, 8, 8 );
    Cursor cur = XCreatePixmapCursor( fl_display, nodata, nodata, &black, &black, 0, 0 );
    XDefineCursor( fl_display, fl_xid( MainWindow ), cur );
    XFreeCursor( fl_display, cur );
    #endif

    // Fullscreen
    #ifndef FO_D3D
    if( GameOpt.FullScreen )
    {
        int sx, sy, sw, sh;
        MainWindow->GetDesktopXYWH( sx, sy, sw, sh );
        MainWindow->SetBorder( 0 );
        MainWindow->SetSize( sw, sh );
        MainWindow->SetPosition( 0, 0 );
    }
    #endif
	
    // Hide menu
    #ifdef FO_WINDOWS
    SetWindowLong( MainWindow->GetHandle(), GWL_STYLE, GetWindowLong( MainWindow->GetHandle( ), GWL_STYLE ) & ( ~WS_SYSMENU ) );
    #endif

    // Place on top
    #ifdef FO_WINDOWS
    if( GameOpt.AlwaysOnTop )
        SetWindowPos( MainWindow->GetHandle( ), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
    #endif

    // Start
    WriteLog( "Starting FOnline (version %04X-%02X)...\n", CLIENT_VERSION, FO_PROTOCOL_VERSION & 0xFF );
    Game.Start( GameThread, "Main" );

    // Loop
	int visible_window = 0;
	while( !GameOpt.Quit )
	{
		visible_window = Fl::wait( );
		if( visible_window == 0 )
			break;

		while( true )
		{
			void* flmessage = Fl::thread_message( );
			if( !flmessage )
				break;
			FlMessage* mess = ( FlMessage* )flmessage;
			switch( mess->msg )
			{
				case FlMessageIndex::OverlayWindowInit:
					((FOnline::Overlay*)mess->handle)->InitWindow();
					break;
				default: break;
			}
		}
	}

	GameOpt.Quit = true;
    Game.Wait();

    // Finish
    #ifdef FO_WINDOWS
    if( Singleplayer )
        SingleplayerData.Finish();
    #endif
    WriteLog( "FOnline finished.\n" );
    LogFinish( -1 );
	pthread_win32_process_detach_np( );
    return 0;
}

void GameThread( void* )
{
    // Start
    FOEngine = new FOClient();
    if( !FOEngine || !FOEngine->Init() )
    {
        WriteLog( "FOnline engine initialization fail.\n" );
        GameOpt.Quit = true;
        return;
    }

    // Loop
    while( !GameOpt.Quit )
    {
        if( !FOEngine->MainLoop() )
            Sleep( 100 );
    }

    // Finish
    FOEngine->Finish();
    delete FOEngine;
}

bool IsApplicationRun( )
{
	return FOEngine != 0;
}
