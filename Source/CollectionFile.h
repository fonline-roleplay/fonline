
class CollectionFile
{
    char* Buffer;
    int Size;
    Mutex FileMutex;
    std::string Path;

    int refcounter;

    static Mutex MapMutex;
    static std::map<const char*, CollectionFile* > MapFiles;

    CollectionFile( );
    ~CollectionFile( );

public:
    int AddRef( );
    void Release( );

    static CollectionFile* Get( const char* file );
    static CollectionFile* Open( const char* filePath );
    static void Store( CollectionFile* file );
    static void Close( CollectionFile* file );
};