//============================================================================================
/**
 * @file    FinderStudioUtil.cpp
 *
 * @brief   FinderStudio用Utilityクラス
 *          static関数にて機能を提供する。
 */
//============================================================================================
#include "App/FinderStudioStatic/include/FinderStudioUtil.h"
#include "App/FinderStudioStatic/include/FinderStudioImageDBUtil.h"

#include "ExtSavedata/include/FinderStudioExtSaveData.h"

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

// crc
// #include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    コンストラクタ(隠蔽化)
 */
/* ----------------------------------------------------------------------------------------- */
FinderStudioUtil::FinderStudioUtil( void )
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief    デストラクタ(隠蔽化)
 */
/* ----------------------------------------------------------------------------------------- */
FinderStudioUtil::~FinderStudioUtil(void)
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       写真データのCRC一致チェック
 * @param[in]   pPhotoData   検査する写真データ
 *
 * @return trueで正常  falseで不正
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
bool FinderStudioUtil::CheckPhotoDataCrc( const App::FinderStudio::Static::PhotoData* pPhotoData )
{
  GFL_ASSERT( pPhotoData );
  if( pPhotoData != NULL )
  {
    ICHI_PRINT( "CRCチェック buffer[0x%x]== calc[0x%x]\n", pPhotoData->deco_param.crc, GetPhotoDataCrc( pPhotoData ) );
    return pPhotoData->deco_param.crc == GetPhotoDataCrc( pPhotoData );
  }
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief       写真データのCRC取得
 * @param[in]   pPhotoData   写真データ
 *
 * @return crc
 */
/* ----------------------------------------------------------------------------------------- */
// @static_function
u32 FinderStudioUtil::GetPhotoDataCrc( const App::FinderStudio::Static::PhotoData* pPhotoData )
{
  GFL_ASSERT( pPhotoData );

  u32 crc = 0;

#if defined(GF_PLATFORM_CTR)
  if( pPhotoData != NULL )
  {
    // 末尾のcrc4バイトをマイナス
    crc = nn::util::Crc32::Calculate( pPhotoData, sizeof( App::FinderStudio::Static::PhotoData ) - 4 );
  }
#endif

  ICHI_PRINT( "PhotoData CRC [0x%x]\n", crc );
  return crc;
}



//////////////////////////////////////////////////////////////////////////
/// ---[ FinderStudioExtSaveDataAccessorクラス begin ]---
///
/// @note   ファインダースタジオ用の拡張セーブデータの一連の処理を行うクラス。
///         
///         主な機能：
///           データファイルの読込み        ：同期・非同期
///           データファイルの新規書き込み  ：非同期
///           データファイルの上書き        ：非同期
///         
///         ※読込み書き込みをした際は以下のリザルトが返ってきます。
///           RESULT_CONTINUE ：処理継続中
///           RESULT_SUCCESS  ：処理正常終了。
///           RESULT_FAILED   ：処理失敗。GetResultで詳細を取得
///           RESULT_NOT_EXIST：処理正常終了。ファイルが一つも存在しなかった。
///           RESULT_BROKEN   ：処理正常終了。ファイルに破損・改ざんがあったためデータ削除しました。
///         




//////////////////////////////////////////////////////////////////////////
///
/// @brief    コンストラクタ・デストラクタ
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   コンストラクタ
 */
FinderStudioExtSaveDataAccessor::FinderStudioExtSaveDataAccessor( gfl2::heap::HeapBase* heap )
  : m_pHeap( heap )
  , m_pExtSaveData( NULL )
  , m_Result( gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK )
  , m_MainSeq( 0 )
  , m_SubSeq( 0 )
  , m_DataCount( 0 )
  , m_bFileExist( false )
  , m_bFileBroken( false )
  , m_bDataUpdate( false )
{
  _ResetData_AllSaveData();
  _ResetData_AllDataIndex();
}

/**
 * @brief   デストラクタ
 */
FinderStudioExtSaveDataAccessor::~FinderStudioExtSaveDataAccessor( void )
{
  GFL_ASSERT( m_pExtSaveData == NULL );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイルアクセス
///
//////////////////////////////////////////////////////////////////////////
/**
* @brief   ファイルアクセス：読込み
*
* @param[out]  dstDataArray     データ格納配列
*
* @retval  リザルト：FinderStudioExtSaveDataAccessor::Result
*/
FinderStudioExtSaveDataAccessor::Result FinderStudioExtSaveDataAccessor::ReadFile( IntegratedData* dstDataArray /* = NULL */ )
{
  enum {
    READ_SEQ_INIT,
    READ_SEQ_EXIST,
    READ_SEQ_READ,
    READ_SEQ_MATCHING,
    READ_SEQ_WRITE,
    READ_SEQ_FILE_SUCCESS,
    READ_SEQ_FILE_NOT_EXIST,
    READ_SEQ_FILE_BROKEN,
    READ_SEQ_FILE_ERROR,
  };

  //-------------------------------------------------------
  // @brief   読込み：初期化
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_INIT )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Init]\n" );

    // 拡張セーブデータの生成
    _CreateFinderStudioExtSaveData();
    // データインデックスの初期化
    _ResetData_AllDataIndex();
    // ファイルカウント
    m_DataCount = 0;
    // 存在フラグ
    _SetFileExistFlg( true );
    // 破損フラグ
    _SetFileBrokenFlg( false );
    // データ更新フラグ
    _SetDataUpdateFlg( false );

    _SetMainSeq( READ_SEQ_EXIST );
  }

  //-------------------------------------------------------
  // @brief   読込み：存在チェック
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_EXIST )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Exist]\n" );

    // ファイルが存在する
    if( _FileExist_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "ExistFile\n" );

      _SetMainSeq( READ_SEQ_READ );      //!< 読込み処理へ
    }
    // ファイルが存在しない
    else {
      FUKUSHIMA_PRINT( "NotExistFile\n" );

      _SetMainSeq( READ_SEQ_FILE_NOT_EXIST );
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：読込み
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_READ )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Read]\n" );

    // ファイル読込み
    if( _FileRead_Sync( SAVE_FILE_INDEX, m_Data ) )
    {
      FUKUSHIMA_PRINT( "Success\n" );

      _SetMainSeq( READ_SEQ_MATCHING );
    }
    // 破損／改ざん
    else if( _IsFileResultBroken() )
    {
      FUKUSHIMA_PRINT( "Broken\n" );

      _SetMainSeq( READ_SEQ_FILE_BROKEN );
    }
    // エラー
    else {
      FUKUSHIMA_PRINT( "Error\n" );

      _SetMainSeq( READ_SEQ_FILE_ERROR );
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：整合性チェック
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_MATCHING )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Matching]\n" );

    // ImageDB内の検索
    ImageDBUtil::SearchPhotoData( m_pHeap->GetLowerHandle(), m_pHeap->GetLowerHandle() );

    for( u32 i = 0; i < SAVE_DATA_MAX; ++i )
    {
      IntegratedData* data = &m_Data[i];

      FUKUSHIMA_PRINT( "index=[%d] filepath=[%ls]\n", i, data->management_data.filepath );

      // データの存在チェック
      if( !_IsExistSaveData( data ) )
      {
        FUKUSHIMA_PRINT( "NotExist SaveData : index=[%d]\n", i );
        continue;   //!< 無ければ次へ
      }
      // 整合性チェック：ファイルパス
      else if( !_IsMatchFilepath( data ) )
      {
        FUKUSHIMA_PRINT( "NotMatch Filepath : index=[%d]\n", i );

        _ResetData_SaveData( data );
        _SetDataUpdateFlg( true );
      }
      // 整合性チェック：CRC
      else if( !_IsMatchCRC( data ) ) // @fix MMCat[374]
      {
        FUKUSHIMA_PRINT( "NotMatch CRC : index=[%d]\n", i );

        _ResetData_SaveData( data );
        _SetDataUpdateFlg( true );
      }
    }

    // ImageDBの検索データを削除
    ImageDBUtil::DeletePictureInfo();

    // データインデックスの設定
    _SetDataIndex();

    // データを外部格納先に設定
    if( dstDataArray )
    {
      // 全データ分をコピー
      gfl2::std::MemCopy( m_Data, dstDataArray, DATA_FILE_SIZE );
    }

    // データの更新が行われた
    if( _GetDataUpdateFlg() )
    {
      // 拡張セーブデータに書き込む
      _SetMainSeq( READ_SEQ_WRITE );
    }
    else {
      _SetMainSeq( READ_SEQ_FILE_SUCCESS );
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：データ更新に伴い書き込み
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_WRITE )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Write]\n" );

    // ファイル書き込み
    if( _FileWrite_Async( SAVE_FILE_INDEX, m_Data ) )
    {
      // 成功
      if( _IsFileResultSuccess() )
      {
        FUKUSHIMA_PRINT( "Write Success\n" );

        _SetMainSeq( READ_SEQ_FILE_SUCCESS );
      }
      // 破損／改竄
      else if( _IsFileResultBroken() )
      {
        FUKUSHIMA_PRINT( "Write Broken\n" );

        _SetMainSeq( READ_SEQ_FILE_BROKEN );
      }
      // エラー
      else {
        FUKUSHIMA_PRINT( "Write Error\n" );

        _SetMainSeq( READ_SEQ_FILE_ERROR );
      }
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：正常終了
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_FILE_SUCCESS )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Success]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    if( _GetDataUpdateFlg() )
    {
      return RESULT_BROKEN;
    }
    else {
      return RESULT_SUCCESS;
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：ファイルなし
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_FILE_NOT_EXIST )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[NotExist]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    if( _GetFileBrokenFlg() )
    {
      return RESULT_BROKEN;
    }
    else {
      return RESULT_NOT_EXIST;
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：破損／改竄
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_FILE_BROKEN )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Broken]\n" );

    _SetFileBrokenFlg( true );

    // ファイル削除：成功
    if( _FileDelete_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "Delete Success\n" );

      _SetMainSeq( READ_SEQ_FILE_NOT_EXIST );
    }
    // ファイル削除：失敗
    else {
      FUKUSHIMA_PRINT( "Delete Failed\n" );

      _SetMainSeq( READ_SEQ_FILE_ERROR );
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：エラー
  //-------------------------------------------------------
  if( m_MainSeq == READ_SEQ_FILE_ERROR )
  {
    FUKUSHIMA_PRINT( "ReadFile : Sequence=[Error]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    return RESULT_FAILED;
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   ファイルアクセス：書き込み
 *
 * @param[in]   srcData     保存データ
 * @param[in]   dataIndex   保存先のインデックス
 *
 * @retval  リザルト：FinderStudioExtSaveDataAccessor::Result
 */
FinderStudioExtSaveDataAccessor::Result FinderStudioExtSaveDataAccessor::WriteFile( IntegratedData* srcData, const u32 dataIndex /* = SAVE_DATA_MAX */ )
{
  enum {
    WRITE_SEQ_INIT,
    WRITE_SEQ_FILE_EXIST,
    WRITE_SEQ_READ,
    WRITE_SEQ_CREATE,
    WRITE_SEQ_UPDATE_DATA,
    WRITE_SEQ_WRITE,
    WRITE_SEQ_FILE_SUCCESS,
    WRITE_SEQ_FILE_BROKEN,
    WRITE_SEQ_FILE_ERROR,
  };

  //-------------------------------------------------------
  // @brief   書き込み：初期化
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_INIT )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Init]\n" );

    // 拡張セーブデータの生成
    _CreateFinderStudioExtSaveData();
    // 存在フラグ
    _SetFileExistFlg( true );
    // 破損フラグ
    _SetFileBrokenFlg( false );
    // データ更新フラグ
    _SetDataUpdateFlg( false );

    _SetMainSeq( WRITE_SEQ_FILE_EXIST );
  }

  //-------------------------------------------------------
  // @brief   書き込み：存在チェック
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_FILE_EXIST )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Exist]\n" );

    // ファイルが存在する
    if( _FileExist_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "ExistFile\n" );

      _SetMainSeq( WRITE_SEQ_READ );      //!< 読込み
    }
    // ファイルが存在しない
    else {
      FUKUSHIMA_PRINT( "NotExistFile\n" );

      _SetMainSeq( WRITE_SEQ_CREATE );    //!< 生成
    }
  }

  //-------------------------------------------------------
  // @brief   書き込み：読込み
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_READ )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Read]\n" );

    // ファイル読込み
    if( _FileRead_Sync( SAVE_FILE_INDEX, m_Data ) )
    {
      FUKUSHIMA_PRINT( "Success\n" );
      _SetMainSeq( WRITE_SEQ_UPDATE_DATA );
    }
    // 破損／改ざん
    else if( _IsFileResultBroken() )
    {
      FUKUSHIMA_PRINT( "Broken\n" );
      _SetMainSeq( WRITE_SEQ_FILE_BROKEN );
    }
    // エラー
    else {
      FUKUSHIMA_PRINT( "Error\n" );
      _SetMainSeq( WRITE_SEQ_FILE_ERROR );
    }
  }

  //-------------------------------------------------------
  // @brief   書き込み：生成
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_CREATE )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Create]\n" );

    // ファイル生成
    if( _FileCreate_Async( SAVE_FILE_INDEX ) )
    {
      // 成功
      if( _IsFileResultSuccess() )
      {
        FUKUSHIMA_PRINT( "Create Success\n" );
        _ResetData_AllSaveData();
        _SetMainSeq( WRITE_SEQ_UPDATE_DATA );
      }
      // 破損／改竄
      else if( _IsFileResultBroken() )
      {
        FUKUSHIMA_PRINT( "Create Broken\n" );

        _SetMainSeq( WRITE_SEQ_FILE_BROKEN );
      }
      // エラー
      else {
        FUKUSHIMA_PRINT( "Create Error\n" );

        _SetMainSeq( WRITE_SEQ_FILE_ERROR );
      }
    }
  }

  //-------------------------------------------------------
  // @brief   書き込み：データ更新
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_UPDATE_DATA )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[UpdateData]\n" );

    // 保存データの設定
    _SetWriteData( srcData, dataIndex );
    // データインデックスの設定
    _SetDataIndex();

    _SetMainSeq( WRITE_SEQ_WRITE );
  }

  //-------------------------------------------------------
  // @brief   書き込み：書き込み
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_WRITE )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Write]\n" );

    // ファイル書き込み
    if( _FileWrite_Async( SAVE_FILE_INDEX, m_Data ) )
    {
      // 成功
      if( _IsFileResultSuccess() )
      {
        FUKUSHIMA_PRINT( "Write Success\n" );
        _SetMainSeq( WRITE_SEQ_FILE_SUCCESS );
      }
      // 破損／改竄
      else if( _IsFileResultBroken() )
      {
        FUKUSHIMA_PRINT( "Write Broken\n" );

        _SetMainSeq( WRITE_SEQ_FILE_BROKEN );
      }
      // エラー
      else {
        FUKUSHIMA_PRINT( "Write Error\n" );

        _SetMainSeq( WRITE_SEQ_FILE_ERROR );
      }
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：正常終了
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_FILE_SUCCESS )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Success]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    if( _GetFileBrokenFlg() )
    {
      return RESULT_BROKEN;
    }
    else {
      return RESULT_SUCCESS;
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：破損／改竄
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_FILE_BROKEN )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Broken]\n" );

    // ファイル削除：成功
    if( !_GetFileBrokenFlg() && _FileDelete_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "Delete Success\n" );

      _SetFileBrokenFlg( true );

      _SetMainSeq( WRITE_SEQ_CREATE );    //!< ファイルを新規作成しにいく
    }
    // ファイル削除：失敗
    else {
      FUKUSHIMA_PRINT( "Delete Failed\n" );

      _SetMainSeq( WRITE_SEQ_FILE_ERROR );
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：エラー
  //-------------------------------------------------------
  if( m_MainSeq == WRITE_SEQ_FILE_ERROR )
  {
    FUKUSHIMA_PRINT( "WriteFile : Sequence=[Error]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    return RESULT_FAILED;
  }

  return RESULT_CONTINUE;
}

/**
 * @brief   ファイルアクセス：削除
 *
 * @param[in]   dataIndex   データインデックス
 *
 * @retval  リザルト：FinderStudioExtSaveDataAccessor::Result
 */
FinderStudioExtSaveDataAccessor::Result FinderStudioExtSaveDataAccessor::DeleteData( const u32 dataIndex )
{
  enum {
    DELETE_SEQ_INIT,
    DELETE_SEQ_EXIST,
    DELETE_SEQ_READ,
    DELETE_SEQ_DATA_DELETE,
    DELETE_SEQ_WRITE,
    DELETE_SEQ_FILE_SUCCESS,
    DELETE_SEQ_FILE_BROKEN,
    DELETE_SEQ_FILE_ERROR,
  };

  //-------------------------------------------------------
  // @brief   削除：初期化
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_INIT )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Init]\n" );

    // 拡張セーブデータの生成
    _CreateFinderStudioExtSaveData();
    // 存在フラグ
    _SetFileExistFlg( true );
    // 破損フラグ
    _SetFileBrokenFlg( false );
    // データ更新フラグ
    _SetDataUpdateFlg( false );

    _SetMainSeq( DELETE_SEQ_EXIST );
  }

  //-------------------------------------------------------
  // @brief   削除：存在チェック
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_EXIST )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Exist]\n" );

    // ファイルが存在する
    if( _FileExist_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "ExistFile\n" );

      _SetMainSeq( DELETE_SEQ_READ );      //!< 読込み
    }
    // ファイルが存在しない
    else {
      FUKUSHIMA_PRINT( "NotExistFile\n" );
    }
  }

  //-------------------------------------------------------
  // @brief   削除：読込み
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_READ )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Read]\n" );

    // ファイル読込み
    if( _FileRead_Sync( SAVE_FILE_INDEX, m_Data ) )
    {
      FUKUSHIMA_PRINT( "Success\n" );
      _SetMainSeq( DELETE_SEQ_DATA_DELETE );
    }
    // 破損／改ざん
    else if( _IsFileResultBroken() )
    {
      FUKUSHIMA_PRINT( "Broken\n" );
      _SetMainSeq( DELETE_SEQ_FILE_BROKEN );
    }
    // エラー
    else {
      FUKUSHIMA_PRINT( "Error\n" );
      _SetMainSeq( DELETE_SEQ_FILE_ERROR );
    }
  }

  //-------------------------------------------------------
  // @brief   削除：データ削除
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_DATA_DELETE )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[DataDelete]\n" );

    // データを消去
    _ResetData_SaveData( &m_Data[dataIndex] );
    // データインデックスの設定
    _SetDataIndex();

    _SetMainSeq( DELETE_SEQ_WRITE );
  }

  //-------------------------------------------------------
  // @brief   削除：書き込み
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_WRITE )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Write]\n" );

    // ファイル書き込み
    if( _FileWrite_Async( SAVE_FILE_INDEX, m_Data ) )
    {
      // 成功
      if( _IsFileResultSuccess() )
      {
        FUKUSHIMA_PRINT( "Success\n" );
        _SetMainSeq( DELETE_SEQ_FILE_SUCCESS );
      }
      // 破損／改竄
      else if( _IsFileResultBroken() )
      {
        FUKUSHIMA_PRINT( "Broken\n" );
        _SetMainSeq( DELETE_SEQ_FILE_BROKEN );
      }
      // エラー
      else {
        FUKUSHIMA_PRINT( "Error\n" );
        _SetMainSeq( DELETE_SEQ_FILE_ERROR );
      }
    }
  }

  //-------------------------------------------------------
  // @brief   読込み：正常終了
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_FILE_SUCCESS )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Success]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    return RESULT_SUCCESS;
  }
  //-------------------------------------------------------
  // @brief   読込み：破損／改竄
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_FILE_BROKEN )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Broken]\n" );

    _SetFileBrokenFlg( true );

    // ファイル削除：成功
    if( _FileDelete_Sync( SAVE_FILE_INDEX ) )
    {
      FUKUSHIMA_PRINT( "Delete Success\n" );

      _SetMainSeq( DELETE_SEQ_FILE_SUCCESS );
    }
    // ファイル削除：失敗
    else {
      FUKUSHIMA_PRINT( "Delete Failed\n" );

      _SetMainSeq( DELETE_SEQ_FILE_ERROR );
    }
  }
  //-------------------------------------------------------
  // @brief   読込み：エラー
  //-------------------------------------------------------
  if( m_MainSeq == DELETE_SEQ_FILE_ERROR )
  {
    FUKUSHIMA_PRINT( "DeleteData : Sequence=[Error]\n" );

    // シーケンスを初期化
    _SetMainSeq( 0 );
    // 拡張セーブデータの破棄
    _DeleteFinderStudioExtSaveData();

    return RESULT_FAILED;
  }

  return RESULT_CONTINUE;
}

#if PM_DEBUG
FinderStudioExtSaveDataAccessor::Result FinderStudioExtSaveDataAccessor::DeleteDataFile( void )
{
  // 拡張セーブデータの生成
  _CreateFinderStudioExtSaveData();

  // 存在チェック
  if( _FileExist_Sync( SAVE_FILE_INDEX ) )
  {
    // データファイルの削除
    _FileDelete_Sync( SAVE_FILE_INDEX );
  }

  // 拡張セーブデータの破棄
  _DeleteFinderStudioExtSaveData();

  return RESULT_SUCCESS;
}

void FinderStudioExtSaveDataAccessor::SetDataIndex( void )
{
  // 拡張セーブデータの生成
  _CreateFinderStudioExtSaveData();

  // 存在チェック
  if( _FileExist_Sync( SAVE_FILE_INDEX ) )
  {
    // データファイルの読込み
    _FileRead_Sync( SAVE_FILE_INDEX, m_Data );
    // データカウントの設定
    _SetDataIndex();
  }

  // 拡張セーブデータの破棄
  _DeleteFinderStudioExtSaveData();
}
#endif





//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイルアクセス(同期)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイルアクセス(同期)：ファイルチェック
 *
 * @param   fileIndex   ファイルインデックス
 *
 * @retval  "true"ならファイルが存在する
 * @retval  "false"ならファイルが存在しない
 */
bool FinderStudioExtSaveDataAccessor::_FileExist_Sync( u32 fileIndex )
{
  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  FUKUSHIMA_PRINT( "FileExistStart : fileIndex[%d]\n", fileIndex );

  bool exist = m_pExtSaveData->ExistDataFile_Sync( fileIndex );

  FUKUSHIMA_PRINT( "FileExistFinish\n" );

  return exist;
}

/**
 * @brief   ファイルアクセス(同期)：ファイル読み込み
 *
 * @param[in]   fileIndex   ファイルインデックス
 * @param[out]  dstData     格納先
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveDataAccessor::_FileRead_Sync( u32 fileIndex, IntegratedData* dstData )
{
  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );
  GFL_ASSERT( dstData );

  FUKUSHIMA_PRINT( "FileReadStart : fileIndex=[%d]\n", fileIndex );

  bool result = m_pExtSaveData->ReadDataFile_Sync( fileIndex, dstData, &m_Result );

  FUKUSHIMA_PRINT( "FileReadFinish\n" );

  return result;
}

/**
 * @brief   ファイルアクセス(非同期)：ファイル削除
 *
 * @param   fileIndex   ファイルインデックス
 *
 * @retval  "true"なら成功
 * @retval  "false"なら失敗
 */
bool FinderStudioExtSaveDataAccessor::_FileDelete_Sync( u32 fileIndex )
{
  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  FUKUSHIMA_PRINT( "FileDeleteStart : fileIndex=[%d]\n", fileIndex );

  bool result = m_pExtSaveData->DeleteDataFile_Sync( fileIndex, &m_Result );

  FUKUSHIMA_PRINT( "FileDeleteFinish\n" );

  return result;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファイルアクセス(非同期)：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイルアクセス(非同期)：ファイルチェック
 *
 * @param   fileIndex   ファイルインデックス
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool FinderStudioExtSaveDataAccessor::_FileExist_Async( u32 fileIndex )
{
  enum {
    FILE_EXIST_ASYNC_START,
    FILE_EXIST_ASYNC_FINISH,
    FILE_EXIST_ASYNC_NUM,

    FILE_EXIST_ASYNC_BEGIN = FILE_EXIST_ASYNC_START,
    FILE_EXIST_ASYNC_END = FILE_EXIST_ASYNC_NUM,
  };

  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  if( m_SubSeq == FILE_EXIST_ASYNC_START )
  {
    FUKUSHIMA_PRINT( "FileExistStart : fileIndex[%d]\n", fileIndex );

    // ファイルの存在チェック：開始
    m_pExtSaveData->ExistDataFile_Async( m_pHeap->GetLowerHandle(), fileIndex );

    _SetSubSeq( FILE_EXIST_ASYNC_FINISH );
  }
  if( m_SubSeq == FILE_EXIST_ASYNC_FINISH )
  {
    // ファイルの存在チェック：待機
    if( m_pExtSaveData->IsFinishExistDataFile_Async( &m_Result ) )
    {
      _SetSubSeq( FILE_EXIST_ASYNC_END );
    }
  }
  if( m_SubSeq == FILE_EXIST_ASYNC_END )
  {
    FUKUSHIMA_PRINT( "FileExistFinish\n" );

    return true;
  }

  return false;
}

/**
 * @brief   ファイルアクセス(非同期)：ファイル読み込み
 *
 * @param[in]   fileIndex   ファイルインデックス
 * @param[out]  dstData     格納先
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool FinderStudioExtSaveDataAccessor::_FileRead_Async( u32 fileIndex, IntegratedData* dstData )
{
  enum {
    FILE_READ_ASYNC_START,
    FILE_READ_ASYNC_FINISH,
    FILE_READ_ASYNC_NUM,

    FILE_READ_ASYNC_BEGIN = FILE_READ_ASYNC_START,
    FILE_READ_ASYNC_END = FILE_READ_ASYNC_NUM,
  };

  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );
  GFL_ASSERT( dstData );

  if( m_SubSeq == FILE_READ_ASYNC_START )
  {
    FUKUSHIMA_PRINT( "FileReadStart : fileIndex=[%d]\n", fileIndex );

    // ファイル読込み：開始
    m_pExtSaveData->ReadDataFile_Async( m_pHeap->GetLowerHandle(), fileIndex, dstData );

    _SetSubSeq( FILE_READ_ASYNC_FINISH );
  }
  if( m_SubSeq == FILE_READ_ASYNC_FINISH )
  {
    // ファイル読込み：待機
    if( m_pExtSaveData->IsFinishReadDataFile_Async( &m_Result ) )
    {
      _SetSubSeq( FILE_READ_ASYNC_END );
    }
  }
  if( m_SubSeq == FILE_READ_ASYNC_END )
  {
    FUKUSHIMA_PRINT( "FileReadFinish\n" );

    for( u32 i = 0; i < SAVE_DATA_MAX; ++i )
    {
      FUKUSHIMA_PRINT( "dstData[%d] : filepath=[%ls]\n", i + 1, dstData[i].management_data.filepath );
    }

    return true;
  }

  return false;
}

/**
 * @brief   ファイルアクセス(非同期)：ファイル書き込み
 *
 * @param[in]   fileIndex   ファイルインデックス
 * @param[in]   srcData     書き込むデータ
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool FinderStudioExtSaveDataAccessor::_FileWrite_Async( u32 fileIndex, const IntegratedData* srcData )
{
  enum {
    FILE_WRITE_ASYNC_START,
    FILE_WRITE_ASYNC_FINISH,
    FILE_WRITE_ASYNC_NUM,

    FILE_WRITE_ASYNC_BEGIN = FILE_WRITE_ASYNC_START,
    FILE_WRITE_ASYNC_END = FILE_WRITE_ASYNC_NUM,
  };

  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  if( m_SubSeq == FILE_WRITE_ASYNC_START )
  {
    FUKUSHIMA_PRINT( "FileWriteStart : fileIndex=[%d]\n", fileIndex );

    // ファイル書き込み：開始
    m_pExtSaveData->WriteDataFile_Async( m_pHeap->GetLowerHandle(), fileIndex, srcData );

    _SetSubSeq( FILE_WRITE_ASYNC_FINISH );
  }
  if( m_SubSeq == FILE_WRITE_ASYNC_FINISH )
  {
    // ファイル書き込み：待機
    if( m_pExtSaveData->IsFinishWriteDataFile_Async( &m_Result ) )
    {
      _SetSubSeq( FILE_WRITE_ASYNC_END );
    }
  }
  if( m_SubSeq == FILE_WRITE_ASYNC_END )
  {
    FUKUSHIMA_PRINT( "FileWriteFinish\n" );

    return true;
  }

  return false;
}

/**
 * @brief   ファイルアクセス(非同期)：ファイル生成
 *
 * @param[in]   fileIndex   ファイルインデックス
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool FinderStudioExtSaveDataAccessor::_FileCreate_Async( u32 fileIndex )
{
  enum {
    FILE_CREATE_ASYNC_START,
    FILE_CREATE_ASYNC_FINISH,
    FILE_CREATE_ASYNC_NUM,

    FILE_CREATE_ASYNC_BEGIN = FILE_CREATE_ASYNC_START,
    FILE_CREATE_ASYNC_END = FILE_CREATE_ASYNC_NUM,
  };

  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  if( m_SubSeq == FILE_CREATE_ASYNC_START )
  {
    FUKUSHIMA_PRINT( "FileCreateStart : fileIndex=[%d]\n", fileIndex );

    // ファイル生成：開始
    m_pExtSaveData->CreateDataFile_Async( m_pHeap->GetLowerHandle(), fileIndex );

    _SetSubSeq( FILE_CREATE_ASYNC_FINISH );
  }
  if( m_SubSeq == FILE_CREATE_ASYNC_FINISH )
  {
    // ファイル生成：待機
    if( m_pExtSaveData->IsFinishCreateDataFile_Async( &m_Result ) )
    {
      _SetSubSeq( FILE_CREATE_ASYNC_END );
    }
  }
  if( m_SubSeq == FILE_CREATE_ASYNC_END )
  {
    FUKUSHIMA_PRINT( "FileCreateFinish\n" );

    return true;
  }

  return false;
}

/**
 * @brief   ファイルアクセス(非同期)：ファイル削除
 *
 * @param[in]   fileIndex   ファイルインデックス
 *
 * @retval  "true"なら終了
 * @retval  "false"なら継続
 */
bool FinderStudioExtSaveDataAccessor::_FileDelete_Async( u32 fileIndex )
{
  enum {
    FILE_DELETE_ASYNC_START,
    FILE_DELETE_ASYNC_FINISH,
    FILE_DELETE_ASYNC_NUM,

    FILE_DELETE_ASYNC_BEGIN = FILE_DELETE_ASYNC_START,
    FILE_DELETE_ASYNC_END = FILE_DELETE_ASYNC_NUM,
  };

  GFL_ASSERT( m_pExtSaveData );
  GFL_ASSERT( fileIndex < SAVE_DATA_MAX );

  if( m_SubSeq == FILE_DELETE_ASYNC_START )
  {
    FUKUSHIMA_PRINT( "FileDeleteStart : fileIndex=[%d]\n", fileIndex );

    // データの破損／改ざんフラグを更新
    m_bFileBroken = true;

    // ファイル削除：開始
    m_pExtSaveData->DeleteDataFile_Async( m_pHeap->GetLowerHandle(), fileIndex );

    _SetSubSeq( FILE_DELETE_ASYNC_FINISH );
  }
  if( m_SubSeq == FILE_DELETE_ASYNC_FINISH )
  {
    // ファイル削除：待機
    if( m_pExtSaveData->IsFinishDeleteDataFile_Async( &m_Result ) )
    {
      _SetSubSeq( FILE_DELETE_ASYNC_END );
    }
  }
  if( m_SubSeq == FILE_DELETE_ASYNC_END )
  {
    FUKUSHIMA_PRINT( "FileDeleteFinish\n" );

    return true;
  }

  return false;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファインダースタジオ用拡張セーブデータ 生成・破棄：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   生成
 */
void FinderStudioExtSaveDataAccessor::_CreateFinderStudioExtSaveData( void )
{
  GFL_ASSERT( m_pHeap );
  GFL_ASSERT( m_pExtSaveData == NULL );

  // アクセサの生成
  m_pExtSaveData = GFL_NEW( m_pHeap->GetLowerHandle() ) ExtSavedata::FinderStudioExtSaveData();
}

/**
 * @brief   破棄
 */
void FinderStudioExtSaveDataAccessor::_DeleteFinderStudioExtSaveData( void )
{
  GFL_ASSERT( m_pExtSaveData );

  GFL_DELETE( m_pExtSaveData );
  m_pExtSaveData = NULL;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    設定・取得：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   保存データの設定
 *
 * @param[in]   srcData     保存データ
 * @param[in]   dataIndex   データインデックス
 */
void FinderStudioExtSaveDataAccessor::_SetWriteData( IntegratedData* srcData, const u32 dataIndex )
{
  u32 index = dataIndex;

  if( dataIndex == SAVE_DATA_MAX )
  {
    for( u32 i = 0; i < SAVE_DATA_MAX; ++i )
    {
      // データの登録されていない箇所を検索
      if( !_IsExistSaveData( &m_Data[i] ) )
      {
        index = i;
        break;
      }
    }
  }

  GFL_ASSERT( index < SAVE_DATA_MAX );

  // CRCの更新
  srcData->photo_data.deco_param.crc =
    FinderStudioUtil::GetPhotoDataCrc( &srcData->photo_data );

  // 保存データをデータファイルのデータにコピー
  gfl2::std::MemCopy( srcData, &m_Data[index], SAVE_DATA_SIZE );
}

/**
 * @brief   データインデックスの設定
 */
void FinderStudioExtSaveDataAccessor::_SetDataIndex( void )
{
  FUKUSHIMA_PRINT( "_SetDataIndex\n" );

  _ResetData_AllDataIndex();
  m_DataCount = 0;

  for( u32 i = 0; i < SAVE_DATA_MAX; ++i )
  {
    // 存在しているデータをカウント
    if( _IsExistSaveData( &m_Data[i] ) )
    {
      m_DataIndex[m_DataCount] = i;
      m_DataCount++;

      FUKUSHIMA_PRINT( "ExistData[%d] : Index=[%d]\n", m_DataCount, i );
    }
  }
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    チェック関数：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   ファイルアクセスが成功したか？
 *
 * @retval  "true"なら成功
 * @retval  "false"ならそれ以外
 */
bool FinderStudioExtSaveDataAccessor::_IsFileResultSuccess( void ) const
{
  return m_Result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_OK;
}

/**
 * @brief   ファイルが破損したか？
 *
 * @retval  "true"なら破損
 * @retval  "false"ならそれ以外
 */
bool FinderStudioExtSaveDataAccessor::_IsFileResultBroken( void ) const
{
  return m_Result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_BROKEN_DATA;
}

/**
 * @brief   ファイルパスが存在していないか？
 *
 * @retval  "true"なら存在していない
 * @retval  "false"ならそれ以外
 */
bool FinderStudioExtSaveDataAccessor::_IsFileResultNotExist( void ) const
{
  return m_Result == gfl2::fs::ExtSaveFile::ACCESS_RESULT_NG_PATH_NOT_EXIST;
}

/**
 * @brief   セーブデータが存在するか？
 *
 * @param[in]   data    セーブデータ
 *
 * @retval  "true"なら有る
 * @retval  "false"なら無し
 */
bool FinderStudioExtSaveDataAccessor::_IsExistSaveData( IntegratedData* data )
{
  const wchar_t* filepath = reinterpret_cast<wchar_t*>( data->management_data.filepath );
  u32 length = ::std::wcslen( filepath );

  FUKUSHIMA_PRINT( "length=[%d]\n", length );

  return length > 0;
}

/**
 * @brief   ファイルパスの一致チェック
 *
 * @param[in]   data    データ
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool FinderStudioExtSaveDataAccessor::_IsMatchFilepath( IntegratedData* data )
{
#if defined(GF_PLATFORM_CTR)
  // 検索した写真の数
  u32 photoCount = ImageDBUtil::GetPictureInfoCount();

  // 拡張セーブデータに格納されているパス
  wchar_t* filepath = reinterpret_cast<wchar_t*>( data->management_data.filepath );

  for( u32 i = 0; i < photoCount; ++i )
  {
    // 検索したファイルパス
    wchar_t* imgdbFilepath = ImageDBUtil::GetImageDBPictureInfo( i ).m_path;

    // ファイルパスの一致チェック
    if( ::std::wcscmp( filepath, imgdbFilepath ) == 0 )
    {
      return true;
    }
  }

  return false;
#else
  return true;
#endif
}

/**
 * @brief   CRCの一致チェック
 *
 * @param[in]   data    参照データ
 *
 * @retval  "true"なら一致
 * @retval  "false"なら不一致
 */
bool FinderStudioExtSaveDataAccessor::_IsMatchCRC( IntegratedData* data )
{
  return FinderStudioUtil::CheckPhotoDataCrc( &data->photo_data );
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    シーケンス：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   メインシーケンスのセット
 *
 * @param   mainSeq   シーケンス
 */
void FinderStudioExtSaveDataAccessor::_SetMainSeq( u8 mainSeq )
{
  m_MainSeq = mainSeq;

  _SetSubSeq( 0 );
}

/**
 * @brief   サブシーケンス
 *
 * @param   subSeq    サブシーケンス
 */
void FinderStudioExtSaveDataAccessor::_SetSubSeq( u8 subSeq )
{
  m_SubSeq = subSeq;
}




//////////////////////////////////////////////////////////////////////////
///
/// @brief    リセット：非公開
///
//////////////////////////////////////////////////////////////////////////
/**
 * @brief   リセット：全セーブデータ
 */
void FinderStudioExtSaveDataAccessor::_ResetData_AllSaveData( void )
{
  gfl2::std::MemClear( &m_Data, DATA_FILE_SIZE );
}

/**
 * @brief   リセット：セーブデータ単体
 *
 * @patam[in]   リセット対象データ
 */
void FinderStudioExtSaveDataAccessor::_ResetData_SaveData( IntegratedData* data )
{
  gfl2::std::MemClear( data, SAVE_DATA_SIZE );
}

/**
 * @brief   リセット：全データインデックス
 */
void FinderStudioExtSaveDataAccessor::_ResetData_AllDataIndex( void )
{
  for( u32 i = 0; i < SAVE_DATA_MAX; ++i )
  {
    m_DataIndex[i] = SAVE_DATA_MAX;
  }
}

/// ---[ FinderStudioExtSaveDataAccessorクラス end ]---
//////////////////////////////////////////////////////////////////////////

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

