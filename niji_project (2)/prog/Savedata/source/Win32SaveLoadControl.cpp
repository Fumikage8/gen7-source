#if   defined(GF_PLATFORM_WIN32)
//===================================================================
/**
 * @file    WIN32SaveLoadControl.cpp
 * @brief   セーブ＆ロードのコントロールを行うクラス
 * @author  
 * @date    
*/
//===================================================================
#include <Savedata/include/Win32SaveLoadControl.h>

#include <math/include/gfl2_math_control.h>

namespace Savedata{


bool Win32SaveLoadControl::FormatMain( gfl2::fs::FsCtrBackupSystem::Result * res )
{
	DeleteFile(
    WIN32_SAVE_FILENAME 
	);

  *res = gfl2::fs::FsCtrBackupSystem::RESULT_OK;
  return true;
}

bool Win32SaveLoadControl::LoadMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
  bool isExists = false;
	HANDLE hFile;

	hFile = CreateFile(
    WIN32_SAVE_FILENAME , GENERIC_READ , 0 , NULL ,
		OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) 
  {
    isExists = true;
	}

  if( isExists )
  {
    u8* pBuffer = GFL_NEW_ARRAY( m_pHeap ) u8[SAVE_BUFFER_SIZE];
    DWORD readSize = 0;

    BOOL ret = ReadFile(
	    hFile ,
	    pBuffer ,
	    SAVE_BUFFER_SIZE ,
	    &readSize ,
	    NULL
    );


    if( !ret )
    {
      GFL_ASSERT(0);//@fix
    }
    else
    {
      ISaveDataTotal * save_total = m_pSaveTotal;

      int max = save_total->GetSaveObjectNum();
      size_t align = save_total->GetAlignmentSize();
      u8* pReadPoint = pBuffer;
      for (int index = 0; index < max; ++ index )
      {
        SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );
        u32 size = pSaveObj->GetDataSize();
        void * data = pSaveObj->GetData();

        ::std::memcpy( data , pReadPoint , size );

        pReadPoint += gfl2::math::RoundUp( size, align );

        GFL_ASSERT( static_cast<DWORD>( pReadPoint - pBuffer ) <= readSize );//サイズは一致するはず
      }


    }

    GFL_SAFE_DELETE( pBuffer );
  }

  CloseHandle(hFile);

  *result = gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK;

  return true;
}

gfl2::fs::FsCtrBackupFile::Result Win32SaveLoadControl::LoadAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total )
{
  m_pSaveTotal = save_total;
  m_pHeap = pHeap;

  gfl2::fs::FsCtrBackupFile::Result result;
  LoadMain( &result );

  return result;
}


bool Win32SaveLoadControl::SaveMain( gfl2::fs::FsCtrBackupFile::Result * result )
{
	HANDLE hFile;

  CreateDirectory( WIN32_SAVE_FILEPATH, NULL );

	hFile = CreateFile(
    WIN32_SAVE_FILENAME , GENERIC_WRITE , 0 , NULL ,
		CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL , NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) 
  {
    GFL_ASSERT(0);//@fix
	}

  u8* pBuffer = GFL_NEW_ARRAY( m_pHeap ) u8[SAVE_BUFFER_SIZE];

  ISaveDataTotal * save_total = m_pSaveTotal;
  save_total->SetupCheckData();

  int max = save_total->GetSaveObjectNum();
  size_t align = save_total->GetAlignmentSize();
  u8* pWritePoint = pBuffer;
  for (int index = 0; index < max; ++ index )
  {
    SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );
    u32 size = pSaveObj->GetDataSize();
    void * data = pSaveObj->GetData();

    ::std::memcpy( pWritePoint , data , size );

    pWritePoint += gfl2::math::RoundUp( size, align );
  }

  size_t totalSize = pWritePoint - pBuffer;

  DWORD writeSize = 0;
  BOOL ret = WriteFile(
	  hFile ,
	  pBuffer ,
	  totalSize ,
	  &writeSize ,
	  NULL
  );

  GFL_ASSERT( totalSize == writeSize );//サイズは一致するはず

  if( !ret )
  {
    GFL_ASSERT(0);//@fix
  }

  GFL_SAFE_DELETE( pBuffer );

  CloseHandle(hFile);

  *result = gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK;

  return true;
}


SaveLoadControl::ARC_STATUS Win32SaveLoadControl::CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap )
{
  SaveLoadControl::ARC_STATUS result = SaveLoadControl::ARC_STATUS_NEED_INIT;
  bool isExists = false;
	HANDLE hFile;

	hFile = CreateFile(
    WIN32_SAVE_FILENAME , GENERIC_READ , 0 , NULL ,
		OPEN_EXISTING , FILE_ATTRIBUTE_NORMAL , NULL
	);
	if (hFile != INVALID_HANDLE_VALUE) 
  {
    isExists = true;
	}

  if( isExists )
  {
    DWORD size = GetFileSize( hFile , NULL );
    u32    totalSize = 0;
    {
      ISaveDataTotal * save_total = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSaveData();
      int    max    = save_total->GetSaveObjectNum();
      size_t align  = save_total->GetAlignmentSize();

      for (int index = 0; index < max; ++ index )
      {
        SaveDataInterface * pSaveObj = save_total->GetSaveObject( index );

        totalSize += gfl2::math::RoundUp( pSaveObj->GetDataSize(), align );
      }
    }

    GFL_PRINT("SAVEDATA_SIZE[%d][%d]\n",size,totalSize);

    if( size == totalSize )
    {//セーブデータサイズ変更なし
      result = SaveLoadControl::ARC_STATUS_OK;
    }
    else
    {//セーブデータサイズ変更あり
      result = SaveLoadControl::ARC_STATUS_NEED_INIT;
    }
      
  }
  else
  {//セーブファイル存在しない
    result = SaveLoadControl::ARC_STATUS_NEED_INIT;
  }

  CloseHandle(hFile);

  return result;
}


} //namespace savedata

#endif