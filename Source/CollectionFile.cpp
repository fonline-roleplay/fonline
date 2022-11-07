
#include <iostream>
#include <fstream>

#include "Common.h"
#include "CollectionFile.h"

Mutex CollectionFile::MapMutex;
std::map<const char*, CollectionFile* > CollectionFile::MapFiles;

CollectionFile::CollectionFile( ): refcounter( 0 ), Buffer( nullptr ), Size( 0 ), Path( )
{

}

CollectionFile::~CollectionFile( )
{
    if( Buffer )
    {
        delete[ Size ] Buffer;
        Buffer = nullptr;
    }
    Path.clear( );
}

int CollectionFile::AddRef( )
{
    return ++refcounter;
}

void CollectionFile::Release( )
{
    if( !( refcounter-- ) )
        Close( this );
}

CollectionFile* CollectionFile::Get( const char* file )
{
    MapMutex.Lock( );
    CollectionFile* result = nullptr;
    auto find = MapFiles.find( file );
    if( find != MapFiles.end( ) )
        result = find->second;
    MapMutex.Unlock( );
    return result;
}

void CollectionFile::Store( CollectionFile* file )
{

}

CollectionFile* CollectionFile::Open( const char* filePath )
{
    std::ifstream file( filePath );
    if( !file.is_open( ) )
        return nullptr;

    file.seekg( ios_base::end );
    int size = (int)file.tellg( );
    file.seekg( ios_base::beg );

    char* buffer = new char[ ( unsigned int )size ];
    file >> buffer;

    file.close( );

    CollectionFile* result = new CollectionFile( );
    result->Size = size;
    result->Buffer = buffer;
    result->Path = filePath;

    return result;
}

void CollectionFile::Close( CollectionFile* file )
{
    if( !file )
        return;

    delete file;
}
