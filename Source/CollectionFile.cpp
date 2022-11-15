
#include <iostream>
#include <fstream>

#include "Common.h"
#include "CollectionFile.h"

Mutex CollectionFile::MapMutex;
std::map< uint, CollectionFile* > CollectionFile::MapFiles;

CollectionFile::CollectionFile( ): refcounter( 0 ), Buffer( nullptr ), Size( 0 ), Hash( 0 )
{

}

CollectionFile::~CollectionFile( )
{
    if( Buffer )
    {
        delete[ Size ] Buffer;
        Buffer = nullptr;
    }
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

uint CollectionFile::GetHash( ) const
{
    return Hash;
}

uint CollectionFile::GetSize( ) const
{
    return Size;
}

const char* CollectionFile::GetBuffer( ) const
{
    return Buffer;
}

CollectionFile* CollectionFile::Get( uint hash )
{
    MapMutex.Lock( );
    CollectionFile* result = nullptr;
    auto find = MapFiles.find( hash );
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
    result->Hash = Str::GetHash( filePath );

    return result;
}

void CollectionFile::Close( CollectionFile* file )
{
    if( !file )
        return;

    delete file;
}
