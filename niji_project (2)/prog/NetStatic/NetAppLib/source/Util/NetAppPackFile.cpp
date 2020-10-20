//============================================================================================
/**
 * @file NetAppPackFile.cpp
 * @brief パックファイルを扱うクラスです
 * @date 2016.02.03
 * @author endo_akira
 */
//============================================================================================
#include "NetStatic/NetAppLib/include/Util/NetAppPackFile.h"
#include "Battle/Regulation/include/Regulation.h"
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


static const u32 PACK_FILE_VERSION       = 1;
static const u32 DEFAULT_ALIGN_SIZE      = 4;   // @note パックファイルのメモリアライメントサイズ(nijiでは4固定)

PackFile::PackFile() :
  m_pTopAddress( NULL ),
  m_pHeader( NULL ),
  m_pOffsetTable( NULL )
{
}


PackFile::PackFile( const u8* pBuffer ) :
  m_pTopAddress( NULL ),
  m_pHeader( NULL ),
  m_pOffsetTable( NULL )
{
  LoadFromMemory( pBuffer );
}


PackFile::~PackFile()
{
}


void PackFile::LoadFromMemory( const u8* pBuffer )
{
  m_pTopAddress = pBuffer;
  m_pHeader = reinterpret_cast<const NetAppLib::Util::PackFile::Header*>( pBuffer );
  m_pOffsetTable = reinterpret_cast<const NetAppLib::Util::PackFile::Offset*>( pBuffer + sizeof( NetAppLib::Util::PackFile::Header ) );
}


//! @brief  レギュレーションパックファイルが破損しているかチェックを行う
//! @param  fileCountMin ファイル数最小値(0指定でチェックしない)
//! @param  fileCountMax ファイル数最大値(0指定でチェックしない)
//! @param  packFileSize パックファイルのサイズ
//! @retval true 壊れている(不正ファイル)
//! @retval false 壊れていない(正常なファイル)
bool PackFile::IsBrokenRegulationPackFile( const u32 fileCountMin, const u32 fileCountMax, const u32 packFileSize )
{
  if( m_pHeader == NULL )
  {
    return true;
  }

  if( m_pHeader->version != PACK_FILE_VERSION )
  {
    GFL_PRINT( "Error !! : Pack file version = %d \n", m_pHeader->version );
    return true; // バージョンが不正
  }

  if( fileCountMin > 0 )
  {
    if( m_pHeader->fileCount < fileCountMin )
    {
      GFL_PRINT( "Error !! : Pack file count = %d, min = %d, max = %d \n", m_pHeader->fileCount, fileCountMin, fileCountMax );
      return true; // ファイル数が不正
    }
  }

  if( fileCountMax > 0 )
  {
    if( m_pHeader->fileCount > fileCountMax )
    {
      GFL_PRINT( "Error !! : Pack file count = %d, min = %d, max = %d \n", m_pHeader->fileCount, fileCountMin, fileCountMax );
      return true; // ファイル数が不正
    }
  }

  if( checkOffsetTableAndPackFileSize( packFileSize ) )
  {
    return true;
  }

  GFL_PRINT( "Success !! : This is a file no problem. \n" );

  return false;
}


//! @retval true 壊れている(不正ファイル)
//! @retval false 壊れていない(正常なファイル)
bool PackFile::checkOffsetTableAndPackFileSize( const u32 packFileSize )
{
  if( m_pOffsetTable == NULL )
  {
    return true;
  }

  u32 address = 0;

  address += sizeof( Header );
  address += ( sizeof( Offset ) * m_pHeader->fileCount );
  address += calcPaddingSize( address, DEFAULT_ALIGN_SIZE );

  for( u32 i = 0; i < m_pHeader->fileCount; ++i )
  {
    if( address != m_pOffsetTable[i].offset )
    {
      GFL_PRINT( "Error !! : Pack file offset table = %d, offset address = %d, file size = %d, calculate offset address = %d \n", i, m_pOffsetTable[i].offset, m_pOffsetTable[i].size, address );

      return true; // オフセットテーブルのアドレスが一致しないので不正
    }

    if( m_pOffsetTable[i].size > Regulation::REGULATION_MAX_DATA_SIZE )
    {
      GFL_PRINT( "Error !! : Pack file offset table = %d, file size over = %d \n", i, m_pOffsetTable[i].size );

      return true; // 想定外のサイズが来たので不正
    }

    address += m_pOffsetTable[i].size;
    address += calcPaddingSize( address, DEFAULT_ALIGN_SIZE );
  }

  if( address != packFileSize )
  {
    GFL_PRINT( "Error !! : Pack file size = %d, calculate file size = %d \n", packFileSize, address );

    return true; // パックファイル全体のファイルサイズが一致しないので不正
  }

  return false;
}


u32 PackFile::calcPaddingSize( const u32 address, const u32 alignSize )
{
  u32 paddingSize = 0;

  u32 temp = address % alignSize;

  if( temp > 0 )
  {
    paddingSize = alignSize - temp;
  }

  return paddingSize;
}


u32 PackFile::GetFileCount()
{
  GFL_ASSERT( m_pHeader ); // @fix

  return ( m_pHeader ) ? m_pHeader->fileCount : 0;
}


u32 PackFile::GetFileSize( u32 index )
{
  u32 fileSize = 0;

  if( index < GetFileCount() )
  {
    if( m_pOffsetTable )
    {
      fileSize = m_pOffsetTable[ index ].size;
    }
  }

  return fileSize;
}


const void* PackFile::GetFileBuffer( u32 index )
{
  const void* pFileBuffer = NULL;

  if( index < GetFileCount() )
  {
    if( m_pTopAddress && m_pOffsetTable )
    {
      pFileBuffer = m_pTopAddress + m_pOffsetTable[ index ].offset;
    }
  }

  return pFileBuffer;
}


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

