//======================================================================
/**
* @file    FinderStudioExtSaveData.cpp
* @date    2017/01/06 16:03:40
* @author  fukushima_yuya
* @brief   ファインダースタジオ用拡張セーブデータ
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "ExtSavedata/include/FinderStudioExtSaveData.h"

#include <GameSys/include/GameManager.h>
#include <System/include/SystemEventManager.h>


GFL_NAMESPACE_BEGIN( ExtSavedata )

//////////////////////////////////////////////////////////////////////////
/// ---[ FinderStudioExtSaveDataクラス begin ]---
///
/// @note   ファインダースタジオ用の拡張セーブデータにアクセスするクラス。
///         
///         主な機能：
///           データファイルの生成        ：同期・非同期
///           データファイルの削除        ：同期・非同期
///           データファイルの読込み      ：同期・非同期
///           データファイルの書き込み    ：同期・非同期
///           データファイルの存在チェック：同期・非同期
///         
///         ※非同期版(～_Async)を使用する際は
///           IsFinish～関数で終了したかをチェックしてください。
///         
///         ※ファイルアクセスの結果は[gfl2::fs::ExtSaveFile::Result]の値で判断してください。
///           リザルトは使用側から引数に渡してください。
///         



//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
FinderStudioExtSaveData::FinderStudioExtSaveData( void )
  : m_pExtSaveSystem( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetNijiExtSaveSystem() )
{
  GFL_SINGLETON_INSTANCE( System::SystemEventManager )->RegistSystemEventListener( this );
}

/**
 * @brief   デストラクタ
 */
FinderStudioExtSaveData::~FinderStudioExtSaveData( void )
{
  GFL_SINGLETON_INSTANCE( System::SystemEventManager )->RemoveSystemEventListener( this );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイル生成(実行部分)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイル生成：同期
 *
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[out]  dstResult   リザルト格納先
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveData::_CreateDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイル生成
  *dstResult = m_pExtSaveSystem->CreateExtFile(
    EXTID_FINDERSTUDIO,
    fileIndex,
    DATA_FILE_SIZE );

  return ( *dstResult == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK );
}

/**
 * @brief   ファイル生成の開始：非同期
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   fileIndex   データファイルのインデックス
 *
 * @retval  "true"なら非同期リクエスト開始
 * @retval  "false"なら非同期リクエスト無効なため失敗
 */
bool FinderStudioExtSaveData::_CreateDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
{
  GFL_ASSERT( workHeap );

  // データファイル生成の開始
  bool result = m_pExtSaveSystem->StartCreateFile(
    workHeap,
    EXTID_FINDERSTUDIO,
    fileIndex,
    DATA_FILE_SIZE );

  return result;
}

/**
 * @brief   ファイル生成の完了待ち：非同期
 *
 * @param[out]  dstResult   リザルト格納先
 *
 * @retval  "true"なら完了
 * @retval  "false"なら処理中
 */
bool FinderStudioExtSaveData::_IsFinishCreateDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイルの生成
  return m_pExtSaveSystem->WaitCreateFile( dstResult );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイル削除(実行部分)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイル削除：同期
 *
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[out]  dstResult   リザルト格納先
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveData::_DeleteDataFile_Sync( u32 fileIndex, gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイル削除
  *dstResult = m_pExtSaveSystem->DeleteExtFile(
    EXTID_FINDERSTUDIO,
    fileIndex );

  return ( *dstResult == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK );
}

/**
 * @brief   ファイル削除の開始：非同期
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   fileIndex   データファイルのインデックス
 *
 * @retval  "true"なら非同期リクエスト開始
 * @retval  "false"なら非同期リクエスト無効なため失敗
 */
bool FinderStudioExtSaveData::_DeleteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
{
  GFL_ASSERT( workHeap );

  // データファイルの削除開始
  bool result = m_pExtSaveSystem->StartDeleteFile(
    workHeap,
    EXTID_FINDERSTUDIO,
    fileIndex );

  return result;
}

/**
 * @brief   ファイル削除の完了待ち：非同期
 *
 * @param[out]  dstResult   リザルト格納先
 *
 * @retval  "true"なら完了
 * @retval  "false"なら処理中
 */
bool FinderStudioExtSaveData::_IsFinishDeleteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイルの削除
  return m_pExtSaveSystem->WaitDeleteFile( dstResult );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイル読込み(実行部分)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイル読込み：同期
 *
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[out]  dstData     データの格納先
 * @param[out]  dstResult   リザルトの格納先
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveData::_ReadDataFile_Sync( u32 fileIndex, void* dstData, gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstData );
  GFL_ASSERT( dstResult );

  // データファイルの読込み
  *dstResult = m_pExtSaveSystem->ReadFile(
    EXTID_FINDERSTUDIO,
    fileIndex,
    dstData,
    DATA_FILE_SIZE );

  return ( *dstResult == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK );
}

/**
 * @brief   ファイル読込みの開始：非同期
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[out]  dstData     データの格納先
 *
 * @retval  "true"なら非同期リクエスト開始
 * @retval  "false"なら非同期リクエスト無効なため失敗
 */
bool FinderStudioExtSaveData::_ReadDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, void* dstData )
{
  GFL_ASSERT( workHeap );
  GFL_ASSERT( dstData );

  // データファイルの読込み開始
  bool result = m_pExtSaveSystem->StartReadFile(
    workHeap,
    EXTID_FINDERSTUDIO,
    fileIndex,
    dstData,
    DATA_FILE_SIZE );

  return result;
}

/**
 * @brief   ファイル読込みの完了待ち：非同期
 *
 * @param[out]  dstResult   リザルト格納先
 *
 * @retval  "true"なら完了
 * @retval  "false"なら処理中
 */
bool FinderStudioExtSaveData::_IsFinishReadDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイルの読込み
  return m_pExtSaveSystem->WaitReadFile( dstResult );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイル書き込み(実行部分)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイル書き込み：同期
 *
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[in]   srcData     書き込むデータ
 * @param[out]  dstResult   リザルトの格納先
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveData::_WriteDataFile_Sync( u32 fileIndex, const void* srcData, gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( srcData );
  GFL_ASSERT( dstResult );

  // データファイルの書き込み
  *dstResult = m_pExtSaveSystem->WriteFile(
    EXTID_FINDERSTUDIO,
    fileIndex,
    srcData,
    DATA_FILE_SIZE );

  return ( *dstResult == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK );
}

/**
 * @brief   ファイル書き込みの開始：非同期
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   fileIndex   データファイルのインデックス
 * @param[in]   srcData     書き込むデータ
 *
 * @retval  "true"なら非同期リクエスト開始
 * @retval  "false"なら非同期リクエスト無効なため失敗
 */
bool FinderStudioExtSaveData::_WriteDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex, const void* srcData )
{
  GFL_ASSERT( workHeap );
  GFL_ASSERT( srcData );

  // データファイルの書き込み開始
  bool result = m_pExtSaveSystem->StartWriteFile(
    workHeap,
    EXTID_FINDERSTUDIO,
    fileIndex,
    srcData,
    DATA_FILE_SIZE );

  return result;
}

/**
 * @brief   ファイル書き込みの完了待ち：非同期
 *
 * @param[out]  dstResult   リザルトの格納先
 *
 * @retval  "true"なら完了
 * @retval  "false"なら処理中
 */
bool FinderStudioExtSaveData::_IsFinishWriteDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイルの読込み
  return m_pExtSaveSystem->WaitWriteFile( dstResult );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイルチェック(実行部分)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイルの存在チェック：同期
 *
 * @param[in]   fileIndex   データファイルのインデックス
 *
 * @retval  "true"ならファイルが存在する
 * @retval  "false"ならファイルが存在しない
 */
bool FinderStudioExtSaveData::_ExistDataFile_Sync( u32 fileIndex )
{
  // データファイルの存在チェック
  return m_pExtSaveSystem->IsFileExists( EXTID_FINDERSTUDIO, fileIndex );
}

/**
 * @brief   ファイルの存在チェックの開始：非同期
 *
 * @param[in]   workHeap    作業ヒープ
 * @param[in]   fileIndex   データファイルのインデックス
 *
 * @retval  "true"なら非同期リクエスト開始
 * @retval  "false"なら非同期リクエスト無効なため失敗
 */
bool FinderStudioExtSaveData::_ExistDataFile_Async( gfl2::heap::HeapBase* workHeap, u32 fileIndex )
{
  GFL_ASSERT( workHeap );

  // データファイルの存在チェック開始
  bool result = m_pExtSaveSystem->StartIsFileExists(
    workHeap,
    EXTID_FINDERSTUDIO,
    fileIndex );

  return result;
}

/**
 * @brief   ファイルの存在チェックの完了待ち：非同期
 *
 * @param[out]  dstResult   リザルトの格納先
 *
 * @retval  "true"なら完了
 * @retval  "false"なら処理中
 */
bool FinderStudioExtSaveData::_IsFinishExistDataFile_Async( gfl2::fs::ExtSaveFile::Result* dstResult )
{
  GFL_ASSERT( dstResult );

  // データファイルの存在チェック
  return m_pExtSaveSystem->WaitWriteFile( dstResult );
}



/// ---[ FinderStudioExtSaveDataクラス end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( ExtSavedata )
