#include "stdafx.h"
#include "Mapper.h"
#include "Exception.h"
#include "Version.h"
#include <locale.h>

FOWindow* MainWindow = NULL;
FOMapper* Mapper = NULL;
Thread    Game;
void GameThread( void* );

int main( int argc, char** argv )
{
    setlocale( LC_ALL, "Russian" );
    RestoreMainDirectory();

    // Threading
    #ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    #endif
    Thread::SetCurrentName( "GUI" );

    // Exceptions
    CatchExceptions( "FOnlineMapper", MAPPER_VERSION );

    // Make command line
    SetCommandLine( argc, argv );

    // Timer
    Timer::Init();

    LogToFile( "FOMapper.log" );

    GetClientOptions();

    WriteLog( "Starting Mapper (%s)...\n", MAPPER_VERSION_STR );

    // Init window threading
    #ifdef FO_LINUX
    XInitThreads();
    #endif
    Fl::lock();

    // Create window
    MainWindow = CreateMainWindow();
    MainWindow->SetLabel( GetWindowName() );
    MainWindow->SetPosition( ( Fl::w() - MODE_WIDTH ) / 2, ( Fl::h() - MODE_HEIGHT ) / 2 );
    MainWindow->SetSize( MODE_WIDTH, MODE_HEIGHT );
	MainWindow->SetIcon( ( char* )LoadIcon( fl_display, MAKEINTRESOURCE( 101 ) ) );

    // OpenGL parameters
    #ifndef FO_D3D
    Fl::gl_visual( FL_RGB | FL_RGB8 | FL_DOUBLE  );
    #endif

    // Show window
    MainWindow->Show();

    // Hide cursor
    #ifdef FO_WINDOWS
    ShowCursor( FALSE );
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
		Fl::lock( );
        Fl::screen_xywh( sx, sy, sw, sh );
		Fl::unlock( );
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
    Game.Start( GameThread, "Main" );

    // Loop
    while( !GameOpt.Quit && Fl::wait() )
        ;
    Fl::unlock();
    GameOpt.Quit = true;
    Game.Wait();

    // Finish
    #ifdef FO_WINDOWS
    if( Singleplayer )
        SingleplayerData.Finish();
    #endif
    WriteLog( "FOnline finished.\n" );
    LogFinish( -1 );

    return 0;
}

void GameThread( void* )
{
    // Start
    Mapper = new FOMapper();
    if( !Mapper || !Mapper->Init() )
    {
        WriteLog( "FOnline engine initialization fail.\n" );
        GameOpt.Quit = true;
        return;
    }

    // Loop
    while( !GameOpt.Quit )
    {
        Mapper->MainLoop();
    }

    // Finish
    Mapper->Finish();
    delete Mapper;
}

bool IsApplicationRun( )
{
	return Mapper != 0;
}
