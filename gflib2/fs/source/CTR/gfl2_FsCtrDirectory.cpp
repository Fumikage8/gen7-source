#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @file    gfl_Directory.cpp
 * @brief
 * @author  tamada
 * @date    2012.09.22
 */
//=============================================================================
#include "fs/include/CTR/gfl2_FsCtrDirectory.h"
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
namespace gfl2{
namespace fs{


//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::Directory()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::~Directory()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::Result Directory::Open( const wchar_t * dir_path )
{
  m_last_nn_result = m_dir.TryInitialize( dir_path );
  return GetAccessResult( m_last_nn_result );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Directory::Close( void )
{
  m_dir.Finalize();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::Result Directory::Read( s32 * pNum, nn::fs::DirectoryEntry * pEntries, s32 readMax )
{
  m_last_nn_result = m_dir.TryRead( pNum, pEntries, readMax ); 
  return GetAccessResult( m_last_nn_result );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::Result Directory::GetEntriesCount( gfl2::heap::HeapBase * pHeap, s32 * count )
{
  s32 read_temp = 0;
  s32 total = 0;
  nn::fs::DirectoryEntry * entries;
  entries = GFL_NEW_ARRAY( pHeap ) nn::fs::DirectoryEntry[ USE_ENTRY_MAX ];
  do {
    m_last_nn_result = m_dir.TryRead( &read_temp, entries, USE_ENTRY_MAX );
    if (m_last_nn_result.IsFailure()) break;
    total += read_temp;
  }while ( read_temp != 0 );

  *count = total;
  GFL_DELETE_ARRAY entries;
  return GetAccessResult( m_last_nn_result );
}

//=============================================================================
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Directory::Result Directory::GetAccessResult( nn::Result nn_result )
{
  if ( nn_result.IsSuccess() ) return Directory::ACCESS_RESULT_OK;
  if ( nn::fs::ResultNotFound::Includes(nn_result) )
  {
    if (nn::fs::ResultMediaNotFound::Includes(nn_result) )
    {
      return ACCESS_RESULT_NG_MEDIA_NOT_FOUND;
    }
    return ACCESS_RESULT_NG_PATH_NOT_EXIST;
  }
  else if ( nn::fs::ResultAlreadyExists::Includes(nn_result) )
  {
    return ACCESS_RESULT_NG_PATH_ALREADY_EXIST;
  }
  else if ( nn::fs::ResultVerificationFailed::Includes(nn_result) )
  {
    return ACCESS_RESULT_NG_BROKEN_DATA;
  }
  else if ( nn::fs::ResultArchiveInvalidated::Includes(nn_result) )
  {
    return ACCESS_RESULT_NG_ARCHIVE_INVALIDATED;
  }
  else if ( nn::fs::ResultOperationDenied::Includes(nn_result) )
  {
    if ( nn::fs::ResultWriteProtected::Includes(nn_result) )
    {
      return ACCESS_RESULT_NG_WRITE_PROTECTED;
    }
    else if ( nn::fs::ResultMediaAccessError::Includes(nn_result) )
    {
      return ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR;
    }
    else
    {
      return ACCESS_RESULT_NG_UNKNOWN;
    }
  }
  else if ( nn::fs::ResultNotEnoughSpace::Includes(nn_result) )
  {
    return ACCESS_RESULT_NG_LACK_OF_SPACE;
  }
  else
  {
    return ACCESS_RESULT_NG_ERROR;
  }
}

}; //namespace fs
}; //namespace gfl



#endif