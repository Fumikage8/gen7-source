#if !defined( __FINDER_STUDIO_EXT_SAVE_DATA_H_INCLUDED__ )
#define __FINDER_STUDIO_EXT_SAVE_DATA_H_INCLUDED__
#pragma once
//======================================================================
/**
* @file    FinderStudioExtSaveData.h
* @date    2017/01/06 15:27:38
* @author  fukushima_yuya
* @brief   ファインダースタジオ用拡張セーブデータ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include  "./NijiExtSaveSystem.h"

#include <App/FinderStudioStatic/include/FinderStudioDefine.h>
#include <System/include/SystemEventListener.h>

#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN( ExtSavedata )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファインダースタジオ用拡張セーブデータ
///
//////////////////////////////////////////////////////////////////////////
class FinderStudioExtSaveData
  : public System::SystemEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderStudioExtSaveData );

public:
  static const u32 SAVE_DATA_SIZE = sizeof( App::FinderStudio::Static::IntegratedData );
  static const u32 DATA_FILE_SIZE = SAVE_DATA_SIZE * App::FinderStudio::Static::SAVE_PHOTO_MAX;

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FinderStudioExtSaveData( void );
  virtual ~FinderStudioExtSaveData( void );

public:
  //-------------------------------------------------------
  // @brief   ファイル生成(単体)：公開
  //-------------------------------------------------------
  bool CreateDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _CreateDataFile_Sync( fileIndex, dstResult );
  }
  bool CreateDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
  {
    return _CreateDataFile_Async( workHeap, fileIndex );
  }
  bool IsFinishCreateDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _IsFinishCreateDataFile_Async( dstResult );
  }

  //-------------------------------------------------------
  // @brief   ファイル削除(単体)：公開
  //-------------------------------------------------------
  bool DeleteDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _DeleteDataFile_Sync( fileIndex, dstResult );
  }
  bool DeleteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
  {
    return _DeleteDataFile_Async( workHeap, fileIndex );
  }
  bool IsFinishDeleteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _IsFinishDeleteDataFile_Async( dstResult );
  }

  //-------------------------------------------------------
  // @brief   ファイル読込み(単体)：公開
  //-------------------------------------------------------
  bool ReadDataFile_Sync( u32 fileIndex, void* dstData, gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _ReadDataFile_Sync( fileIndex, dstData, dstResult );
  }
  bool ReadDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, void* dstData )
  {
    return _ReadDataFile_Async( workHeap, fileIndex, dstData );
  }
  bool IsFinishReadDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _IsFinishReadDataFile_Async( dstResult );
  }

  //-------------------------------------------------------
  // @brief   ファイル書き込み(単体)：公開
  //-------------------------------------------------------
  bool WriteDataFile_Sync( u32 fileIndex, const void* srcData, gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _WriteDataFile_Sync( fileIndex, srcData, dstResult );
  }
  bool WriteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, const void* srcData )
  {
    return _WriteDataFile_Async( workHeap, fileIndex, srcData );
  }
  bool IsFinishWriteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _IsFinishWriteDataFile_Async( dstResult );
  }

  //-------------------------------------------------------
  // @brief   ファイルチェック(単体)：公開
  //-------------------------------------------------------
  bool ExistDataFile_Sync( u32 fileIndex )
  {
    return _ExistDataFile_Sync( fileIndex );
  }
  bool ExistDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
  {
    return _ExistDataFile_Async( workHeap, fileIndex );
  }
  bool IsFinishExistDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
  {
    return _IsFinishExistDataFile_Async( dstResult );
  }

private:
  //-------------------------------------------------------
  // @brief   ファイル生成(実行部分)：非公開
  //-------------------------------------------------------
  bool _CreateDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult );
  bool _CreateDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex );
  bool _IsFinishCreateDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult );

  //-------------------------------------------------------
  // @brief   ファイル削除(実行部分)：非公開
  //-------------------------------------------------------
  bool _DeleteDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult );
  bool _DeleteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex );
  bool _IsFinishDeleteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult );

  //-------------------------------------------------------
  // @brief   ファイル読込み(実行部分)：非公開
  //-------------------------------------------------------
  bool _ReadDataFile_Sync( u32 fileIndex, void* dstData, gfl2::fs::ExtSaveFile::Result* dstResult );
  bool _ReadDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, void* dstData );
  bool _IsFinishReadDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult );

  //-------------------------------------------------------
  // @brief   ファイル書き込み(実行部分)：非公開
  //-------------------------------------------------------
  bool _WriteDataFile_Sync( u32 fileIndex, const void* srcData, gfl2::fs::ExtSaveFile::Result* dstResult );
  bool _WriteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, const void* srcData );
  bool _IsFinishWriteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult );

  //-------------------------------------------------------
  // @brief   ファイルチェック(実行部分)：非公開
  //-------------------------------------------------------
  bool _ExistDataFile_Sync( u32 fileIndex );
  bool _ExistDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex );
  bool _IsFinishExistDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult );

private:
  NijiExtSaveSystem*      m_pExtSaveSystem;
};

GFL_NAMESPACE_END( ExtSavedata )

#endif // __FINDER_STUDIO_EXT_SAVE_DATA_H_INCLUDED__
