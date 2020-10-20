//=============================================================================
/**
 * @file    MyAlbumPhotoData.cpp
 * @brief   マイアルバム画面写真データ管理
 * @author  Hiroyuki Nakamura
 * @date    2017.02.08
 */
//=============================================================================

// module
#include "App/MyAlbum/include/MyAlbumAppParam.h"
#include "MyAlbumPhotoData.h"
// gflib2
#include <util/include/gfl2_std_string.h>
// momiji
#include "AppLib/include/Util/app_util_Heap.h"
#include "App/FinderStudioStatic/include/FinderStudioImageDBUtil.h"
#include "ExtSavedata/include/FinderStudioExtSaveData.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MyAlbum)

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 *
 * @param heap  ヒープ
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::MyAlbumPhotoData( app::util::Heap * heap )
  : m_heap( heap )
  , m_pExtSaveData( NULL )
  , m_extDataCount( 0 )
  , m_pReadDataArray( NULL )
  , m_readDataLoadResult( SDC_ACCESS_RESULT_OK )
  , m_saveDataIndex( 0 )
  , m_photoDataMax( 0 )
  , m_readDataCount( 0 )
  , m_sortDataMax( 0 )
  , m_pImageDBUtil( NULL )
  , m_thumbnailLoadIndex( THUMBNAIL_NONE )
  , m_thumbnailLoadResult( SDC_ACCESS_RESULT_OK )
  , m_mainSeq( 0 )
  , m_isBroken( false )
{
  gfl2::std::MemClear( m_pThumbnail, sizeof(THUMBNAIL_DATA)*THUMBNAIL_MAX );

  for( u32 i=0; i<PHOTO_DATA_MAX; i++ )
  {
    ClearData( &m_photoData[i] );
    m_sortData[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::~MyAlbumPhotoData()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   アップデート
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::Update(void)
{
  LoadThumbnail();
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期設定
 * @date    2017.02.09
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::Initialize(void)
{
  CreateThumbnail();
  CreateImageDBUtil();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了設定
 * @date    2017.02.09
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::Terminate(void)
{
  if( IsTerminate() == false )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  DeleteImageDBUtil();
  DeleteThumbnail();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsTerminate
 * @brief   終了できるか
 * @date    2017.02.09
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsTerminate(void)
{
  for( u32 i=0; i<THUMBNAIL_MAX; i++ )
  {
    if( IsLoadThumbnail( i ) == false )
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateThumbnail
 * @brief   サムネイルデータ作成
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::CreateThumbnail(void)
{
  for( u32 i=0; i<THUMBNAIL_MAX; i++ )
  {
    m_pThumbnail[i].buffer = GflHeapAllocMemoryAlign( m_heap->GetDeviceHeap(), THUMBNAIL_BUFFER_SIZE, 128 );
    m_pThumbnail[i].index  = THUMBNAIL_NONE;
    m_pThumbnail[i].req    = THUMBNAIL_NONE;
    m_pThumbnail[i].seq    = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteThumbnail
 * @brief   サムネイルデータ削除
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::DeleteThumbnail(void)
{
  for( u32 i=0; i<THUMBNAIL_MAX; i++ )
  {
    GflHeapFreeMemory( m_pThumbnail[i].buffer );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateImageDBUtil
 * @brief   イメージDB作成
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::CreateImageDBUtil(void)
{
  m_pImageDBUtil = GFL_NEW(m_heap->GetSystemHeap()) App::FinderStudio::Static::ImageDBUtil();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateImageDBUtil
 * @brief   イメージDB削除
 * @date    2017.02.09
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::DeleteImageDBUtil(void)
{
  GFL_SAFE_DELETE( m_pImageDBUtil );
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckImageDBError
 * @brief   イメージDBのエラーチェック
 * @date    2017.03.13
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumPhotoData::CheckImageDBError(void)
{
  gfl2::imagedb::ImageDBAccessor::imgDbResult result;
  m_pImageDBUtil->CheckSDCard( &result, m_heap->GetDeviceHeap() );

  switch( App::FinderStudio::ErrorConverter::ConvertImageDBErrorFromImgDBResult(result) )
  {
  case App::FinderStudio::ErrorConverter::IMGDB_RESULT_SUCCESS:     // 成功
    return SDC_ACCESS_RESULT_OK;
  case App::FinderStudio::ErrorConverter::IMGDB_RESULT_FATAL_ERROR: // フェイタルエラー
    return SDC_ACCESS_RESULT_NG_FATAL;
  case App::FinderStudio::ErrorConverter::IMGDB_RESULT_NOT_SPACE:   // 空き容量不足
    return SDC_ACCESS_RESULT_NG_LACK_OF_SPACE;
  case App::FinderStudio::ErrorConverter::IMGDB_RESULT_OVER_IMAGE:  // ImgeDB管理枚数オーバー
    return SDC_ACCESS_RESULT_NG_OVER_IMAGE;
  case App::FinderStudio::ErrorConverter::IMGDB_RESULT_OTHER_ERROR: // その他のエラー
    return SDC_ACCESS_RESULT_NG_ETC;
  }

  GFL_ASSERT( 0 );
  return SDC_ACCESS_RESULT_OK;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDataBroken
 * @brief   データが破損していたか
 * @date    2017.02.16
 *
 * @retval  true  = 破損
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsDataBroken(void)
{
  return m_isBroken;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitDataBrokenFlag
 * @brief   データ破損フラグを初期化
 * @date    2017.03.13
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::InitDataBrokenFlag(void)
{
  m_isBroken = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadData
 * @brief   データ読み込み
 * @date    2017.02.08
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumPhotoData::LoadData(void)
{
  switch( m_mainSeq )
  {
    case 0:
    {
      m_pExtSaveData = GFL_NEW( m_heap->GetSystemHeap() ) App::FinderStudio::Static::FinderStudioExtSaveDataAccessor( m_heap->GetDeviceHeap() );
      m_pReadDataArray = GFL_NEW_ARRAY( m_heap->GetSystemHeap() ) App::FinderStudio::Static::IntegratedData[PHOTO_DATA_MAX];
      m_mainSeq++;
    } // no break;

    case 1:
    {
      // 読込み
      App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->ReadFile( m_pReadDataArray );

      if( result != App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
      {
        if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_SUCCESS ||
          result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_NOT_EXIST )
        {
          m_readDataLoadResult = SDC_ACCESS_RESULT_OK;
          m_mainSeq++;
        }
        else if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_BROKEN )
        {
          m_readDataLoadResult = SDC_ACCESS_RESULT_NG_BROKEN;
          m_mainSeq++;
        }
        else if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_FAILED )
        {
          // 独自リザルトに変換
          App::FinderStudio::ErrorConverter::SDCardResultCode resultCode =
            App::FinderStudio::ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );

          // フェイタルエラー
          if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
          {
            m_readDataLoadResult = SDC_ACCESS_RESULT_NG_FATAL;
            m_mainSeq = 3;
          }
          // 空き容量不足
          else if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_NOT_SPACE )
          {
            m_readDataLoadResult = SDC_ACCESS_RESULT_NG_LACK_OF_SPACE;
            m_mainSeq = 3;
          }
          else {
            GFL_ASSERT( 0 );  // ここへはこないはず
            m_readDataLoadResult = SDC_ACCESS_RESULT_NG_FATAL;
            m_mainSeq = 3;
          }
        }
      }
    } break;

    /// 正常終了フロー
    case 2:
    {
      // データカウントの取得
      m_photoDataMax = m_pExtSaveData->GetDataCount();
      FUKUSHIMA_PRINT( "m_photoDataMax=[%d]\n", m_photoDataMax );
      // データインデックスを取得
      for( u32 i = 0; i < PHOTO_DATA_MAX; ++i )
      {
        m_photoData[i].index = ( i < m_photoDataMax ) ? m_pExtSaveData->GetDataIndex( i ) : PHOTO_DATA_MAX;
        FUKUSHIMA_PRINT( "m_photoData[%d].index=[%d]\n", i, m_photoData[i].index );
      }
      // 読み込んだデータを設定
      for( u32 i = 0; i < m_photoDataMax; ++i )
      {
        App::FinderStudio::Static::IntegratedData* data = &m_photoData[i].read_data;
        u32 dataIndex = m_photoData[i].index;

        GFL_ASSERT( dataIndex < PHOTO_DATA_MAX );

        gfl2::std::MemCopy( &m_pReadDataArray[dataIndex], data, ExtSavedata::FinderStudioExtSaveData::SAVE_DATA_SIZE );
        FUKUSHIMA_PRINT( "m_photoData[%d].read_data : filepath=[%ls]\n", i, data->management_data.filepath );
      }
      m_mainSeq++;
    } // no break;

    /// 終了処理
    case 3:
    {
      m_mainSeq = 0;
      GFL_SAFE_DELETE_ARRAY( m_pReadDataArray );

      return m_readDataLoadResult;
    }
  }

  return SDC_ACCESS_RESULT_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadData
 * @brief   データ削除
 * @date    2017.02.08
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::UnloadData(void)
{
  GFL_SAFE_DELETE( m_pExtSaveData );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckPhotoNum
 * @brief   写真保存枚数をチェック
 * @date    2017.03.13
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::ResultPhotoNum MyAlbumPhotoData::CheckPhotoNum(void)
{
  u32 num = PHOTO_DATA_MAX - m_photoDataMax;

  if( num == 0 )
  {
    return RESULT_PHOTO_NUM_MAX;
  }
  else if( num < 10 )
  {
    return RESULT_PHOTO_NUM_WARNING;
  }

  return RESULT_PHOTO_NUM_NORMAL;
}

//-----------------------------------------------------------------------------
/**
 * @func    SaveData
 * @brief   データセーブ
 * @date    2017.02.14
 *
 * @param   index   インデックス
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumPhotoData::SaveData( u32 index )
{
  switch( m_mainSeq )
  {
    case 0:
    {
      // 読み込み終了待ち
      if( m_thumbnailLoadIndex != THUMBNAIL_NONE || m_thumbnailLoadResult != SDC_ACCESS_RESULT_OK )
      {
        break;
      }

      m_saveDataIndex = PHOTO_DATA_MAX;

      for( u32 i = 0; i < m_photoDataMax; i++ )
      {
        if( m_photoData[i].index == index )
        {
          m_saveDataIndex = i;
          m_mainSeq++;
          break;
        }
      }

      if( m_saveDataIndex == PHOTO_DATA_MAX )
      {
        GFL_ASSERT( 0 );    //!< インデックス無効
        return SDC_ACCESS_RESULT_OK;
      }
    } // no break;

    case 1:
    {
      // ファイル書き込み
      App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::Result result =
        m_pExtSaveData->WriteFile( &m_photoData[m_saveDataIndex].read_data, m_photoData[m_saveDataIndex].index );

      if( result != App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_CONTINUE )
      {
        m_mainSeq = 0;

        if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_SUCCESS )
        {
          return SDC_ACCESS_RESULT_OK;
        }
        else if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_BROKEN )
        {
          m_isBroken = true;
          return SDC_ACCESS_RESULT_NG_BROKEN;
        }
        else if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_FAILED )
        {
          App::FinderStudio::ErrorConverter::SDCardResultCode resultCode =
            App::FinderStudio::ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );

          if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_NOT_SPACE )
          {
            return SDC_ACCESS_RESULT_NG_LACK_OF_SPACE;
          }
          else if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
          {
            return SDC_ACCESS_RESULT_NG_FATAL;
          }
          else
          {
            GFL_ASSERT( 0 );
            return SDC_ACCESS_RESULT_NG_FATAL;
          }
        }
      }
    } break;
  }

  return SDC_ACCESS_RESULT_WAIT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetData
 * @brief   写真データ取得
 * @date    2017.02.13
 *
 * @param   pos   データ位置
 *
 * @return  写真データ
 */
//-----------------------------------------------------------------------------
App::FinderStudio::Static::IntegratedData * MyAlbumPhotoData::GetData( u32 pos )
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return NULL;
  }
  return &m_sortData[pos]->read_data;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDataIndex
 * @brief   インデックスから写真データ取得
 * @date    2017.02.15
 *
 * @param   index   SDカード上のインデックス
 *
 * @return  写真データ
 */
//-----------------------------------------------------------------------------
App::FinderStudio::Static::IntegratedData * MyAlbumPhotoData::GetDataIndex( u32 index )
{
  for( u32 i=0; i<m_photoDataMax; i++ )
  {
    if( m_photoData[i].index == index )
    {
      return &m_photoData[i].read_data;
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIndexOrigin
 * @brief   写真データのSDカード上のインデックスを取得（全データ版）
 * @date    2017.02.27
 *
 * @param   pos   データ位置
 *
 * @return  写真データインデックス
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::GetIndexOrigin( u32 pos )
{
  return m_photoData[pos].index;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetIndex
 * @brief   写真データのSDカード上のインデックスを取得（絞込み済みデータ版）
 * @date    2017.02.14
 *
 * @param   pos   データ位置
 *
 * @return  写真データインデックス
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::GetIndex( u32 pos )
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return PHOTO_DATA_NONE;
  }
  return m_sortData[pos]->index;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDataCount
 * @brief   絞込み済みデータ数取得
 * @date    2017.02.08
 *
 * @return  データ数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::GetDataCount(void) const
{
  return m_sortDataMax;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDataCountOrigin
 * @brief   全データ数取得
 * @date    2017.02.27
 *
 * @return  データ数
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::GetDataCountOrigin(void) const
{
  return m_photoDataMax;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMonsNo
 * @brief   ポケモン番号取得
 * @date    2017.02.08
 *
 * @param   pos   データ位置
 *
 * @return  ポケモン番号
 */
//-----------------------------------------------------------------------------
MonsNo MyAlbumPhotoData::GetMonsNo( u32 pos ) const
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return MONSNO_NULL;
  }
  return static_cast<MonsNo>( m_sortData[pos]->read_data.management_data.mons_no );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetDate
 * @brief   日付取得
 * @date    2017.02.08
 *
 * @param   pos     データ位置
 * @param   yesr    年格納場所
 * @param   month   月格納場所
 * @param   day     日格納場所
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::GetDate( u32 pos, u32 * year, u32 * month, u32 * day ) const
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    *year  = 2000;
    *month = 1;
    *day   = 1;
    return;
  }
  *year  = m_sortData[pos]->read_data.management_data.date.year;
  *month = m_sortData[pos]->read_data.management_data.date.month;
  *day   = m_sortData[pos]->read_data.management_data.date.day;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFavoriteOrigin
 * @brief   お気に入りに登録されているか（全データ版）
 * @date    2017.02.27
 *
 * @param   pos   データ位置
 *
 * @retval  true  = 登録されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsFavoriteOrigin( u32 pos ) const
{
  if( pos >= m_photoDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( m_photoData[pos].read_data.management_data.favorite_flg == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFavorite
 * @brief   お気に入りに登録されているか（絞込み済みデータ版）
 * @date    2017.02.08
 *
 * @param   pos   データ位置
 *
 * @retval  true  = 登録されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsFavorite( u32 pos ) const
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( m_sortData[pos]->read_data.management_data.favorite_flg == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsShareOrigin
 * @brief   共有されているか（全データ版）
 * @date    2017.02.27
 *
 * @param   pos   データ位置
 *
 * @retval  true  = 共有されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsShareOrigin( u32 pos ) const
{
  if( pos >= m_photoDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( m_photoData[pos].read_data.management_data.joint_flg == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsShare
 * @brief   共有されているか（絞込み済みデータ版）
 * @date    2017.02.08
 *
 * @param   pos   データ位置
 *
 * @retval  true  = 共有されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsShare( u32 pos ) const
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( m_sortData[pos]->read_data.management_data.joint_flg == 0 )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFavoriteOrigin
 * @brief   お気に入り設定（全データ版）
 * @date    2017.02.27
 *
 * @param   pos   データ位置
 * @param   flg   true = 登録
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::SetFavoriteOrigin( u32 pos, bool flg )
{
  if( pos >= m_photoDataMax )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( flg == false )
  {
    m_photoData[pos].read_data.management_data.favorite_flg = 0;
  }
  else
  {
    m_photoData[pos].read_data.management_data.favorite_flg = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFavorite
 * @brief   お気に入り設定（絞込み済みデータ版）
 * @date    2017.02.08
 *
 * @param   pos   データ位置
 * @param   flg   true = 登録
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::SetFavorite( u32 pos, bool flg )
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( flg == false )
  {
    m_sortData[pos]->read_data.management_data.favorite_flg = 0;
  }
  else
  {
    m_sortData[pos]->read_data.management_data.favorite_flg = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetShareOrigin
 * @brief   共有設定（全データ版）
 * @date    2017.02.27
 *
 * @param   pos   データ位置
 * @param   flg   true = 共有
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::SetShareOrigin( u32 pos, bool flg )
{
  if( pos >= m_photoDataMax )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( flg == false )
  {
    m_photoData[pos].read_data.management_data.joint_flg = 0;
  }
  else
  {
    // 他の写真を非共有にする
    for( u32 i=0; i<m_photoDataMax; i++ )
    {
      if( m_photoData[i].read_data.management_data.joint_flg != 0 )
      {
        m_photoData[i].read_data.management_data.joint_flg = 0;
        break;
      }
    }
    m_photoData[pos].read_data.management_data.joint_flg = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetShare
 * @brief   共有設定（絞込み済みデータ版）
 * @date    2017.02.08
 *
 * @param   pos   データ位置
 * @param   flg   true = 共有
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::SetShare( u32 pos, bool flg )
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return;
  }

  if( flg == false )
  {
    m_sortData[pos]->read_data.management_data.joint_flg = 0;
  }
  else
  {
    // 他の写真を非共有にする
    for( u32 i=0; i<m_photoDataMax; i++ )
    {
      if( m_photoData[i].read_data.management_data.joint_flg != 0 )
      {
        m_photoData[i].read_data.management_data.joint_flg = 0;
        break;
      }
    }
    m_sortData[pos]->read_data.management_data.joint_flg = 1;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetShareIndex
 * @brief   共有データのインデックスを取得
 * @date    2017.02.14
 *
 * @return  インデックス
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::GetShareIndex(void)
{
  for( u32 i=0; i<m_photoDataMax; i++ )
  {
    if( m_photoData[i].read_data.management_data.joint_flg != 0 )
    {
      return m_photoData[i].index;
    }
  }
  return PHOTO_DATA_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDeco
 * @brief   飾りつけされているか
 * @date    2017.02.13
 *
 * @param   pos   データ位置
 *
 * @retval  true  = されている
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsDeco( u32 pos )
{
  if( pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }
  if( m_sortData[pos]->read_data.management_data.deco_flg != 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ClearData
 * @brief   データクリア
 * @date    2017.01.26
 *
 * @param   data  写真データ
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::ClearData( PHOTO_DATA * data )
{
  gfl2::std::MemClear( &data->read_data, sizeof(App::FinderStudio::Static::IntegratedData) );
  data->index = PHOTO_DATA_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitDataRow
 * @brief   データの空きを詰める
 * @date    2017.01.26
 *
 * @param   param   ソートパラメータ
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::InitDataRow( const SORT_DATA * param )
{
  for( u32 i=0; i<PHOTO_DATA_MAX-1; i++ )
  {
    if( m_photoData[i].index == PHOTO_DATA_MAX )
    {
      bool is_update = false;
      for( u32 j=i+1; j<PHOTO_DATA_MAX; j++ )
      {
        if( m_photoData[j].index != PHOTO_DATA_MAX )
        {
          m_photoData[i] = m_photoData[j];
          ClearData( &m_photoData[j] );
          is_update = true;
          break;
        }
      }
      if( is_update == false )
      {
        break;
      }
    }
  }
  // データ数更新
  m_photoDataMax = 0;
  for( u32 i=0; i<PHOTO_DATA_MAX; i++ )
  {
    if( m_photoData[i].index != PHOTO_DATA_MAX )
    {
      m_photoDataMax++;
    }
  }

  // 絞込み再生成
  CreateSortData( param );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteData
 * @brief   写真データ削除
 * @date    2017.02.13
 *
 * @param   pos   データ位置
 *
 * @return  処理結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumPhotoData::DeleteData( u32 pos )
{
  switch( m_mainSeq )
  {
  case 0:
    if( pos >= m_sortDataMax )
    {
      GFL_ASSERT( 0 );
      return SDC_ACCESS_RESULT_OK;
    }
    // 読み込み終了待ち
    if( m_thumbnailLoadIndex != THUMBNAIL_NONE || m_thumbnailLoadResult != SDC_ACCESS_RESULT_OK )
    {
      break;
    }
    // jpeg削除
    m_pImageDBUtil->DeleteImage(
      m_heap->GetDeviceHeap()->GetLowerHandle(),
      reinterpret_cast<wchar_t*>(m_sortData[pos]->read_data.management_data.filepath) );
    {
      App::FinderStudio::ErrorConverter::ImageDBResultCode result = m_pImageDBUtil->GetResult();
      if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_FATAL_ERROR )
      {
        return SDC_ACCESS_RESULT_NG_FATAL;
      }
      else if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_NOT_SPACE )
      {
        GFL_ASSERT( 0 );  // 削除なので容量不足にはならないはず
        return SDC_ACCESS_RESULT_NG_ETC;
      }
      else if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_OVER_IMAGE )
      {
        GFL_ASSERT( 0 );  // 削除なので保存エラーにはならないはず
        return SDC_ACCESS_RESULT_NG_ETC;
      }
      else if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_OTHER_ERROR )
      {
        return SDC_ACCESS_RESULT_NG_ETC;
      }
    }
    m_mainSeq++;
    /* FALL THROUGH */

  case 1:
    {
      App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::Result result = m_pExtSaveData->DeleteData( m_sortData[pos]->index );

      if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_SUCCESS )
      {
        ClearData( m_sortData[pos] );
        return SDC_ACCESS_RESULT_OK;
      }
      else if( result == App::FinderStudio::Static::FinderStudioExtSaveDataAccessor::RESULT_FAILED )
      {
        App::FinderStudio::ErrorConverter::SDCardResultCode resultCode = App::FinderStudio::ErrorConverter::ConvertSDCardErrorFromExtSaveResult( m_pExtSaveData->GetResult() );

        if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_FATAL_ERROR )
        {
          return SDC_ACCESS_RESULT_NG_FATAL;
        }
        else if( resultCode == App::FinderStudio::ErrorConverter::SDCARD_RESULT_BROKEN_DATA )
        {
          m_isBroken = true;
          ClearData( m_sortData[pos] );
          return SDC_ACCESS_RESULT_NG_BROKEN;
        }
        else
        {
          GFL_ASSERT( 0 );  // 削除なので他のエラーにはならないはず
          return SDC_ACCESS_RESULT_NG_ETC;
        }
      }
    }
    break;
  }

  return SDC_ACCESS_RESULT_WAIT;
}

//-----------------------------------------------------------------------------
/**F
 * @func    LoadThumbnailRequest
 * @brief   サムネイル読み込みリクエスト
 * @date    2017.02.09
 *
 * @param   data_pos        写真データ位置
 * @param   thumbnail_pos   サムネイルデータ位置
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::LoadThumbnailRequest( u32 data_pos, u32 thumbnail_pos )
{
  if( data_pos >= m_sortDataMax )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  if( m_thumbnailLoadResult != SDC_ACCESS_RESULT_OK )
  {
    return false;
  }

  THUMBNAIL_DATA * data = &m_pThumbnail[thumbnail_pos];

  if( data->req == THUMBNAIL_NONE && data->index == m_sortData[data_pos]->index )
  {
    return false;
  }

  if( data->req != m_sortData[data_pos]->index )
  {
    data->req = m_sortData[data_pos]->index;
    if( m_thumbnailLoadIndex == THUMBNAIL_NONE )
    {
      m_thumbnailLoadIndex = thumbnail_pos;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadThumbnail
 * @brief   サムネイル読み込み（全体）
 * @date    2017.02.09
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::LoadThumbnail(void)
{
  if( m_thumbnailLoadIndex != THUMBNAIL_NONE )
  {
    for( u32 i=0; i<THUMBNAIL_MAX; i++ )
    {
      if( LoadThumbnail( m_thumbnailLoadIndex ) == false )
      {
        return false;
      }
      m_thumbnailLoadIndex++;
      if( m_thumbnailLoadIndex >= THUMBNAIL_MAX )
      {
        m_thumbnailLoadIndex = 0;
      }
    }
    m_thumbnailLoadIndex = THUMBNAIL_NONE;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadThumbnail
 * @brief   サムネイル読み込み（個別）
 * @date    2017.02.09
 *
 * @param   pos   サムネイルデータ位置
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::LoadThumbnail( u32 pos )
{
  THUMBNAIL_DATA * data = &m_pThumbnail[pos];

  switch( data->seq )
  {
  case 0:
    if( data->req == THUMBNAIL_NONE )
    {
      return true;
    }
    data->index = data->req;
    for( u32 i=0; i<m_photoDataMax; i++ )
    {
      if( m_photoData[i].index == data->index )
      {
        m_pImageDBUtil->StartLoadImage(
          m_heap->GetDeviceHeap()->GetLowerHandle(),
          reinterpret_cast<wchar_t*>(m_photoData[i].read_data.management_data.filepath),
          data->buffer,
          true );
        break;
      }
    }
    data->seq++;
    /* FALL THROUGH */
  case 1:
    if( m_pImageDBUtil->IsFinishLoadImage() != false )
    {
      App::FinderStudio::ErrorConverter::ImageDBResultCode result = m_pImageDBUtil->GetResult();
      // 成功
      if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_SUCCESS )
      {
        data->seq = 0;
        if( data->req == data->index )
        {
          data->req = THUMBNAIL_NONE;
          return true;
        }
      }
      // エラー
      else
      {
        // 他のリクエストを無効化
        for( u32 i=0; i<THUMBNAIL_MAX; i++ )
        {
          m_pThumbnail[i].req = THUMBNAIL_NONE;
          m_pThumbnail[i].seq = 0;
        }
        // フェイタルエラー
        if( result == App::FinderStudio::ErrorConverter::IMGDB_RESULT_FATAL_ERROR )
        {
          m_thumbnailLoadResult = SDC_ACCESS_RESULT_NG_FATAL;
        }
        else
        {
          m_thumbnailLoadResult = SDC_ACCESS_RESULT_NG_ETC;
        }
      }
    }
    break;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsLoadThumbnail
 * @brief   サムネイルデータの読み込みが完了したか
 * @date    2017.02.09
 *
 * @param   pos   サムネイルデータ位置
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsLoadThumbnail( u32 pos )
{
  THUMBNAIL_DATA * data = &m_pThumbnail[pos];
  if( data->req != THUMBNAIL_NONE )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetThumbnailBuffer
 * @brief   サムネイルのバッファを取得
 * @date    2017.02.10
 *
 * @param   pos   サムネイルデータ位置
 *
 * @return  バッファ
 */
//-----------------------------------------------------------------------------
void * MyAlbumPhotoData::GetThumbnailBuffer( u32 pos )
{
  return m_pThumbnail[pos].buffer;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckThumbnailLoadError
 * @brief   サムネイル読み込み時のエラーチェック
 * @date    2017.02.17
 *
 * @return  チェック結果
 */
//-----------------------------------------------------------------------------
MyAlbumPhotoData::SDCardAccessResult MyAlbumPhotoData::CheckThumbnailLoadError(void)
{
  return m_thumbnailLoadResult;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckMonsNoData
 * @brief   指定ポケモンの写真が存在するか
 * @date    2017.02.22
 *
 * @param   mons  ポケモン番号
 *
 * @retval  true  = 存在する
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::CheckMonsNoData( MonsNo mons ) const
{
  for( u32 i=0; i<m_photoDataMax; i++ )
  {
    if( static_cast<MonsNo>(m_photoData[i].read_data.management_data.mons_no) == mons )
    {
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateSortData
 * @brief   写真データをソート
 * @date    2017.02.23
 *
 * @param   param   ソートパラメータ
 */
//-----------------------------------------------------------------------------
void MyAlbumPhotoData::CreateSortData( const SORT_DATA * param )
{
  m_sortDataMax = 0;

  for( u32 i=0; i<m_photoDataMax; i++ )
  {
    if( IsAddSortData( param, &m_photoData[i] ) != false )
    {
      m_sortData[m_sortDataMax] = &m_photoData[i];
      m_sortDataMax++;
    }
  }

  if( m_sortDataMax == 0 )
  {
    return;
  }

  // 新しい順
  if( param->low == 0 )
  {
    for( u32 i=0; i<m_sortDataMax-1; i++ )
    {
      u32 date_i = CalcDate( m_sortData[i] );
      u32 time_i = CalcTime( m_sortData[i] );
      for( u32 j=i; j<m_sortDataMax; j++ )
      {
        u32 date_j = CalcDate( m_sortData[j] );
        u32 time_j = CalcTime( m_sortData[j] );
        if( date_i < date_j || ( date_i == date_j && time_i < time_j ) )
        {
          PHOTO_DATA * tmp = m_sortData[i];
          m_sortData[i] = m_sortData[j];
          m_sortData[j] = tmp;
          date_i = date_j;
          time_i = time_j;
        }
      }
    }
  }
  // 古い順
  else
  {
    for( u32 i=0; i<m_sortDataMax-1; i++ )
    {
      u32 date_i = CalcDate( m_sortData[i] );
      u32 time_i = CalcTime( m_sortData[i] );
      for( u32 j=i; j<m_sortDataMax; j++ )
      {
        u32 date_j = CalcDate( m_sortData[j] );
        u32 time_j = CalcTime( m_sortData[j] );
        if( date_i > date_j || ( date_i == date_j && time_i > time_j ) )
        {
          PHOTO_DATA * tmp = m_sortData[i];
          m_sortData[i] = m_sortData[j];
          m_sortData[j] = tmp;
          date_i = date_j;
          time_i = time_j;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsAddSortData
 * @brief   指定データがソート条件を満たすか
 * @date    2017.02.23
 *
 * @param   param   ソートパラメータ
 * @param   data    写真データ
 *
 * @retval  true  = 条件を満たす
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool MyAlbumPhotoData::IsAddSortData( const SORT_DATA * param, const PHOTO_DATA * data )
{
  // ポケモン番号
  if( param->mons_no != 0 )
  {
    if( param->mons_no != data->read_data.management_data.mons_no )
    {
      return false;
    }
  }
  // 背景番号
  if( param->bg_no != 0 )
  {
    if( (param->bg_no-1) != data->read_data.management_data.bg_index )
    {
      return false;
    }
  }
  // 飾りつけ
  if( param->deco != 0 )
  {
    if( ((param->deco-1)^1) != data->read_data.management_data.deco_flg )
    {
      return false;
    }
  }
  // お気に入り
  if( param->favorite != 0 )
  {
    if( ((param->favorite-1)^1) != data->read_data.management_data.favorite_flg )
    {
      return false;
    }
  }
  // 撮り方
  if( param->photo != 0 )
  {
    if( (param->photo-1) != data->read_data.management_data.cap_mode )
    {
      return false;
    }
  }
  // 共有
  if( param->share != 0 )
  {
    if( ((param->share-1)^1) != data->read_data.management_data.joint_flg )
    {
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcDate
 * @brief   日付を32bitデータに変換
 * @date    2017.02.23
 *
 * @param   data    写真データ
 *
 * @return  変換した値
 *
 * @note  上から 23bit: 年, 4bit: 月, 5bit: 日
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::CalcDate( const PHOTO_DATA * data )
{
  return (data->read_data.management_data.date.day+(data->read_data.management_data.date.month<<5)+(data->read_data.management_data.date.year<<9));
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcTime
 * @brief   時間を32bitデータに変換
 * @date    2017.02.23
 *
 * @param   data    写真データ
 *
 * @return  変換した値
 *
 * @note  上から 20bit: 時, 6bit: 分, 6bit: 秒
 */
//-----------------------------------------------------------------------------
u32 MyAlbumPhotoData::CalcTime( const PHOTO_DATA * data )
{
  return (data->read_data.management_data.date.second+(data->read_data.management_data.date.minute<<6)+(data->read_data.management_data.date.hour<<12));
}


GFL_NAMESPACE_END(MyAlbum)
GFL_NAMESPACE_END(App)
