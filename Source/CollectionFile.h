
#ifndef FONLINE_COLLECTION_FILE
#define FONLINE_COLLECTION_FILE

#include "Common.h"

class CollectionFile
{
    char* Buffer;
    int Size;
    Mutex FileMutex;
    uint Hash;

    int refcounter;

    static Mutex MapMutex;
    static std::map< uint, CollectionFile* > MapFiles;

    CollectionFile( );
    ~CollectionFile( );

public:
    int AddRef( );
    void Release( );

    uint GetHash( ) const;
    uint GetSize( ) const;
    const char* GetBuffer( ) const;

    static CollectionFile* Get( uint hash );
    static CollectionFile* Open( const char* filePath );
    static void Store( CollectionFile* file );
    static void Close( CollectionFile* file );
};

#endif