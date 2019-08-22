#include "Common.h"
// #include "Server.h"
// #include "Client.h"
#include "Mapper.h"
#include "AppGui.h"

static void DockSpaceBegin();
static void DockSpaceEnd();

struct GuiWindow
{
    string           Name;
    ImGuiWindowFlags AdditionalFlags = 0;
    GuiWindow( string name ): Name( name ) {}
    virtual bool     Draw() = 0;
    virtual ~GuiWindow() = default;
};

struct ProjectFilesWindow: GuiWindow
{
    string           SelectedTree;
    int              SelectedItem = -1;
    FilesCollection* Scripts;
    FilesCollection* Locations;
    FilesCollection* Maps;
    FilesCollection* Critters;
    FilesCollection* Items;
    FilesCollection* Dialogs;
    FilesCollection* Texts;

    ProjectFilesWindow(): GuiWindow( "Project Files" )
    {
        Scripts = new FilesCollection( "fos" );
        Locations = new FilesCollection( "foloc" );
        Maps = new FilesCollection( "fomap" );
        Critters = new FilesCollection( "focr" );
        Items = new FilesCollection( "foitem" );
        Dialogs = new FilesCollection( "fodlg" );
        Texts = new FilesCollection( "msg" );
    }

    virtual bool Draw() override
    {
        DrawFiles( "Scripts", Scripts );
        DrawFiles( "Locations", Locations );
        DrawFiles( "Maps", Maps );
        DrawFiles( "Critters", Critters );
        DrawFiles( "Items", Items );
        DrawFiles( "Dialogs", Dialogs );
        DrawFiles( "Texts", Texts );
        return true;
    }

    void DrawFiles( const string& tree_name, FilesCollection* files )
    {
        if( ImGui::TreeNode( tree_name.c_str() ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3.0f );

            files->ResetCounter();
            uint files_count = files->GetFilesCount();
            for( uint i = 0; i < files_count; i++ )
            {
                string name, path, relative_path;
                files->GetNextFile( &name, &path, &relative_path );

                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                if( SelectedItem == i && SelectedTree == tree_name )
                    node_flags |= ImGuiTreeNodeFlags_Selected;

                ImGui::TreeNodeEx( (void*) (intptr_t) i, node_flags, "%s", name.c_str() );

                if( ImGui::IsItemClicked() )
                {
                    SelectedTree = tree_name;
                    SelectedItem = i;
                }
            }

            ImGui::PopStyleVar();
            ImGui::TreePop();
        }
    }
};
// static ProjectFilesWindow* ProjectFiles = nullptr;

struct InspectorWindow: GuiWindow
{
    InspectorWindow(): GuiWindow( "Inspector" ) {}

    virtual bool Draw() override
    {
        ImGui::Text( "InspectorWindow" );
        return true;
    }
};

struct LogWindow: GuiWindow
{
    string CurLog;
    string WholeLog;

    LogWindow(): GuiWindow( "Log" )
    {
        AdditionalFlags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar;
    }

    virtual bool Draw() override
    {
        LogGetBuffer( CurLog );
        if( !CurLog.empty() )
        {
            WholeLog += CurLog;
            CurLog.clear();
            if( WholeLog.size() > 100000 )
                WholeLog = WholeLog.substr( WholeLog.size() - 100000 );
        }

        if( !WholeLog.empty() )
            ImGui::TextUnformatted( WholeLog.c_str(), WholeLog.c_str() + WholeLog.size() );

        return true;
    }
};

struct ServerWindow: GuiWindow
{
    ServerWindow(): GuiWindow( "Server" ) {}

    virtual bool Draw() override
    {
        ImGui::ShowDemoWindow();
        return true;
    }
};

struct ClientWindow: GuiWindow
{
    ClientWindow(): GuiWindow( "Client" ) {}

    virtual bool Draw() override
    {
        ImGui::Text( "ClientWindow" );
        return true;
    }
};

struct MapperWindow: GuiWindow
{
    FOMapper* MapInstance = nullptr;

    MapperWindow( std::string map_name ): GuiWindow( "Map" )
    {
        // FileManager& map_file = ProjectFiles->Maps->FindFile(map_name);
        // if (!map_file.IsLoaded())
        //	return;

        // MapInstance = new FOMapper();
        // if (!MapInstance->Init() || !MapInstance->Lo)
        //	SAFEDEL(MapInstance);
    }

    virtual bool Draw() override
    {
        if( !MapInstance )
            return false;

        ImGui::Text( "MapWindow" );

        MapInstance->MainLoop();
        return true;
    }

    virtual ~MapperWindow() override
    {
        if( MapInstance )
            MapInstance->Finish();
        SAFEDEL( MapInstance );
    }
};

static vector< GuiWindow* > Windows;
static vector< GuiWindow* > NewWindows;
static vector< GuiWindow* > CloseWindows;

extern "C" int main( int argc, char** argv ) // Handled by SDL
{
    InitialSetup( "FOnlineEditor", argc, argv );

    // Threading
    Thread::SetCurrentName( "GUI" );

    // Logging
    LogToFile( "FOnlineEditor.log" );
    LogToBuffer( true );
    WriteLog( "FOnline Editor v.{}.\n", FONLINE_VERSION );

    // Options
    // GetServerOptions();
    GetClientOptions();

    // Initialize Gui
    bool use_dx = ( MainConfig->GetInt( "", "UseDirectX" ) != 0 );
    if( !AppGui::Init( "FOnline Editor", use_dx, true, true ) )
        return -1;

    // Basic windows
    Windows.push_back( new ProjectFilesWindow() );
    Windows.push_back( new InspectorWindow() );
    Windows.push_back( new LogWindow() );
    Windows.push_back( new ServerWindow() );
    Windows.push_back( new ClientWindow() );

    // Main loop
    while( !GameOpt.Quit )
    {
        if( !AppGui::BeginFrame() )
            break;

        DockSpaceBegin();

        for( GuiWindow* window : Windows )
        {
            bool keep_alive = true;
            if( ImGui::Begin( window->Name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | window->AdditionalFlags ) )
                keep_alive = window->Draw();
            ImGui::End();

            if( !keep_alive )
                CloseWindows.push_back( window );
        }

        if( !NewWindows.empty() )
        {
            Windows.insert( Windows.end(), NewWindows.begin(), NewWindows.end() );
            NewWindows.clear();
        }

        while( !CloseWindows.empty() )
        {
            auto       it = std::find( Windows.begin(), Windows.end(), CloseWindows.back() );
            RUNTIME_ASSERT( it != Windows.end() );
            GuiWindow* window = *it;
            Windows.erase( it );
            CloseWindows.pop_back();
            delete window;
        }

        if( Windows.empty() )
            break;

        DockSpaceEnd();

        AppGui::EndFrame();
    }

    // Graceful shutdown
    // Other stuff will be reseted by operating system
    for( GuiWindow* window : Windows )
        delete window;

    return 0;
}

static void DockSpaceBegin()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos( viewport->Pos );
    ImGui::SetNextWindowSize( viewport->Size );
    ImGui::SetNextWindowViewport( viewport->ID );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    ImGui::SetNextWindowBgAlpha( 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    ImGui::Begin( "DockSpace", nullptr, window_flags );
    ImGui::PopStyleVar();
    ImGui::PopStyleVar( 2 );

    ImGuiID dockspace_id = ImGui::GetID( "DockSpace" );
    if( !ImGui::DockBuilderGetNode( dockspace_id ) )
    {
        ImGui::DockBuilderRemoveNode( dockspace_id );
        ImGui::DockBuilderAddNode( dockspace_id, ImGuiDockNodeFlags_None );

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_left_id = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Left, 0.15f, nullptr, &dock_main_id );
        ImGuiID dock_right_id = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id );
        ImGuiID dock_down_id = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id );

        ImGui::DockBuilderDockWindow( "Project Files", dock_left_id );
        ImGui::DockBuilderDockWindow( "Inspector", dock_right_id );
        ImGui::DockBuilderDockWindow( "Log", dock_down_id );
        ImGui::DockBuilderDockWindow( "Server", dock_main_id );
        ImGui::DockBuilderDockWindow( "Client", dock_main_id );
        ImGui::DockBuilderDockWindow( "Mapper", dock_main_id );
        ImGui::DockBuilderFinish( dock_main_id );
    }

    ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_PassthruCentralNode );
}

static void DockSpaceEnd()
{
    ImGui::End();
}
