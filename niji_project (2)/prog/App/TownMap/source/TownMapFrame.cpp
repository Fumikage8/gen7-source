//=============================================================================
/**
 * @file    TownMapFrame.cpp
 * @brief   タウンマップ画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================

// module
#include "App/TownMap/include/TownMapAppParam.h"
#include "TownMapFrame.h"
#include "TownMapDraw.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/TownMapGraphic.gaix"
#include "arc_index/TownMapData.gaix"
#include "arc_index/rotom_fade.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,      //!< 開始
  SEQ_MAIN,           //!< メイン
  SEQ_END             //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
TownMapFrame::TownMapFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_pUpperLytResBuff( NULL )
  , m_pLowerLytResBuff( NULL )
  , m_pRotomLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pWordSetLoader( NULL )
  , m_pDraw( NULL )
  , m_pPartsDraw( NULL )
  , m_pResZoneData( NULL )
  , m_resZoneDataLen( 0 )
  , m_pResIconData( NULL )
  , m_resIconDataLen( 0 )
  , m_putWarpMsgID( -2 )  // @note PUT_WARPMSG_NONE(-1)だと初期メッセージが出ない
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
TownMapFrame::~TownMapFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.10.05
 */
//-----------------------------------------------------------------------------
/*
void TownMapFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveMisc = gd->GetMisc();
  m_myItem   = gd->GetMyItem();
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.10.13
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void TownMapFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;

  if( m_appParam->land_no >= LAND_DATA_MAX )
  {
    m_appParam->land_no = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.10.13
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TownMapFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    InitFade();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    if( LoadTownMapData() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 4:
    if( CreateWordSetLoader() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 5:
    CreateDraw();
    AddSubSeq();
    /* FALL THROUGH */
  case 6:
    if( CreatePartsDraw() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.10.13
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TownMapFrame::EndFunc(void)
{
  if( DeletePartsDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  DeleteWordSetLoader();

  UnloadTownMapData();
  UnloadMessage();
  UnloadLayoutResource();

  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.10.13
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TownMapFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    break;

  case SEQ_MAIN:
    PutWarpPointMessage();
    PutUpperInfo();
    break;

  case SEQ_END:
    if( Update_SeqEnd() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.10.13
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void TownMapFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}



//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) TownMapDraw( m_heap, m_renderingManager );
  m_pDraw->Init( m_pUpperLytResBuff, m_pRotomLytResBuff, m_pMsgBuff, m_pWordSetLoader );
  m_pDraw->SetTownmapData(
    static_cast<ZONE_RESOURCE*>(m_pResZoneData), m_resZoneDataLen,
    static_cast<ICON_RESOURCE*>(m_pResIconData), m_resIconDataLen );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreatePartsDraw
 * @brief   下画面マップ描画クラス生成
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::CreatePartsDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->call_mode == MODE_TOWNMAP )
    {
      m_pPartsDraw = GFL_NEW(m_heap->GetSystemHeap()) TownMapPartsDraw( TownMapPartsDraw::CALL_TOWNMAP, m_heap, m_renderingManager, m_appParam->land_no );
    }
    else
    {
      m_pPartsDraw = GFL_NEW(m_heap->GetSystemHeap()) TownMapPartsDraw( TownMapPartsDraw::CALL_RIDE_FLY, m_heap, m_renderingManager, m_appParam->land_no );
    }
    m_pPartsDraw->SetMapListener( this );
    m_pPartsDraw->SetTownmapData(
      static_cast<ZONE_RESOURCE*>(m_pResZoneData), m_resZoneDataLen,
      static_cast<ICON_RESOURCE*>(m_pResIconData), m_resIconDataLen );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPartsDraw->Init( m_pLowerLytResBuff, m_pMsgBuff ) == false )
    {
      break;
    }
    if( m_appParam->call_mode == MODE_FLY )
    {
      m_pPartsDraw->PutWarpPointWindow();
    }
    m_pDraw->AddSubView( m_pPartsDraw );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeletePartsDraw
 * @brief   描画クラス削除
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::DeletePartsDraw(void)
{
  if( m_pPartsDraw != NULL )
  {
    if( m_pPartsDraw->End() == false )
    {
      return false;
    }
    m_pPartsDraw->RemoveFromSuperView();
    GFL_SAFE_DELETE( m_pPartsDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_TOWNMAP_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_TOWNMAP_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_TOWNMAP_GRA,
      GARC_TownMapGraphic_TownMapUpp_applyt_COMP,
      &m_pUpperLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pUpperLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_TOWNMAP_GRA,
      GARC_TownMapGraphic_TownMapLow_applyt_COMP,
      &m_pLowerLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLowerLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_TOWNMAP_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_TOWNMAP_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileOpen( ARCID_ROTOM_FADE, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 5:
    if( app::util::FileAccessor::IsFileOpen( ARCID_ROTOM_FADE ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_ROTOM_FADE,
      GARC_rotom_fade_rotom_fade_applyt_COMP,
      &m_pRotomLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 6:
    if( app::util::FileAccessor::IsFileLoad( &m_pRotomLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_ROTOM_FADE, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 7:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_ROTOM_FADE ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void TownMapFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pRotomLytResBuff );
  GflHeapFreeMemory( m_pLowerLytResBuff );
  GflHeapFreeMemory( m_pUpperLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.10.13
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_townmap_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void TownMapFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadTownMapData
 * @brief   タウンマップデータ読み込み
 * @date    2015.10.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::LoadTownMapData(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_TOWNMAP_DATA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ゾーンデータロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_TOWNMAP_DATA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_TOWNMAP_DATA,
      GARC_TownMapData_niji_townmap_zonedata_BIN,
      &m_pResZoneData,
      m_heap->GetDeviceHeap(),
      false,
      4,
      gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
      NULL,
      &m_resZoneDataLen );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // アイコンデータロード
    if( app::util::FileAccessor::IsFileLoad( &m_pResZoneData ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_TOWNMAP_DATA,
      GARC_TownMapData_niji_townmap_icondata_BIN,
      &m_pResIconData,
      m_heap->GetDeviceHeap(),
      false,
      4,
      gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
      NULL,
      &m_resIconDataLen );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pResIconData ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_TOWNMAP_DATA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_TOWNMAP_DATA ) == false )
    {
      break;
    }
    // サイズをデータ配列長に変換
    m_resZoneDataLen = m_resZoneDataLen / sizeof(ZONE_RESOURCE);
    m_resIconDataLen = m_resIconDataLen / sizeof(ICON_RESOURCE);
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadTownMapData
 * @brief   タウンマップデータ削除
 * @date    2015.10.26
 */
//-----------------------------------------------------------------------------
void TownMapFrame::UnloadTownMapData(void)
{
  GflHeapFreeMemory( m_pResIconData );
  GflHeapFreeMemory( m_pResZoneData );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateWordSetLoader
 * @brief   WordSetLoader生成
 * @date    2015.10.29
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::CreateWordSetLoader(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_PLACENAME );
    m_pWordSetLoader->StartLoad( m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pWordSetLoader->WaitLoad() == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteWordSetLoader
 * @brief   WordSetLoader削除
 * @date    2015.10.29
 */
//-----------------------------------------------------------------------------
void TownMapFrame::DeleteWordSetLoader(void)
{
  GFL_SAFE_DELETE( m_pWordSetLoader );
}


//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2015.10.15
 */
//-----------------------------------------------------------------------------
void TownMapFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.10.15
 */
//-----------------------------------------------------------------------------
void TownMapFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 0 );  // @note デザイナーの指定でsync=0
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.10.15
 */
//-----------------------------------------------------------------------------
void TownMapFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.10.15
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::IsFadeEnd(void)
{
  gfl2::Fade::FadeManager * man = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  if( man->IsEnd(gfl2::Fade::DISP_UPPER) == false || man->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2015.10.13
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void TownMapFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.10.14
 */
//-----------------------------------------------------------------------------
void TownMapFrame::Update_SeqStart(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->Start( m_appParam->land_no );
    m_pPartsDraw->Start();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    PutWarpPointMessage();
    SetFadeIn();
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( IsFadeEnd() == false )
    {
      break;
    }
    m_mainSeq++;
    break;

  case 3:
    m_pDraw->StartRotom();
    m_mainSeq++;
    /* FALL THROUGH */
  case 4:
    if( m_pDraw->IsStart() == false )
    {
      break;
    }
    m_mainSeq++;
    /* FALL THROUGH */
  case 5:
    if( m_pPartsDraw->IsStart() == false )
    {
      break;
    }
    ChangeSequence( SEQ_MAIN );
    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2015.10.16
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TownMapFrame::Update_SeqEnd(void)
{
  switch( m_mainSeq )
  {
  case 0:
    SetFadeOut();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsFadeEnd() == false )
    {
      break;
    }
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    PuttWarpPointMessage
 * @brief   "○○にとびますか？"のメッセージ表示
 * @date    2015.10.29
 */
//-----------------------------------------------------------------------------
void TownMapFrame::PutWarpPointMessage(void)
{
  if( m_appParam->call_mode != MODE_FLY )
  {
    return;
  }

  s32 id = 0;
  u32 is_place_name = 0;

  if( m_pPartsDraw->GetWarpPointPlaceNameID( &id, &is_place_name ) == false )
  {
    id = PUT_WARPMSG_NONE;
    is_place_name = 0;
    if( m_putWarpMsgID == id )
    {
      return;
    }
  }
  else
  {
    if( m_putWarpMsgID == (static_cast<s32>(is_place_name<<31)+id) )
    {
      return;
    }
  }

  gfl2::str::StrBuf * str = m_pDraw->GetWarpPointMessage( id, is_place_name );
  m_pPartsDraw->SetWarpPointMessage( str );

  m_putWarpMsgID = static_cast<s32>( is_place_name << 31 ) + id;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutUpperInfo
 * @brief   上画面情報表示
 * @date    2015.10.29
 */
//-----------------------------------------------------------------------------
void TownMapFrame::PutUpperInfo(void)
{
/*
  // イベント情報
  if( m_pPartsDraw->GetCursorPosIconParam( PLACE_TYPE_EVENT, false ) != TM_DUMMY_DATA )
  {
    u16 title, info;
    m_pPartsDraw->GetEventMessage( &title, &info );
    m_pDraw->PutEventInfo( title, info );
  }
  else
*/
  {
//    m_pDraw->PutEventInfo();

    // ゾーン情報
    ZoneID zone_id = m_pPartsDraw->GetCursorPosZoneID();
    m_pDraw->PutZoneInfo( zone_id );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力時のリスナー
 * @date    2015.10.13
 *
 * @param   id    動作ID
 *
 * @note  override TownMapPartsDraw::SubScreenMapListener
 */
//-----------------------------------------------------------------------------
void TownMapFrame::OnAction( TownMapPartsDraw::SubScreenMapListener::ActionID id )
{
  switch( id )
  {
  case SubScreenMapListener::ACTION_ID_RETURN:  // 戻る
    m_appParam->warp_id = FLY_WARPID_CANCEL;
    ChangeSequence( SEQ_END );
    break;

  case SubScreenMapListener::ACTION_ID_SELECT:  // 選択
    if( m_appParam->call_mode == MODE_FLY )
    {
      m_appParam->warp_id = m_pPartsDraw->GetSelectWarpID();
      ChangeSequence( SEQ_END );
    }
    break;

  case SubScreenMapListener::ACTION_ID_ZOOM:    // 拡縮開始
    break;

  case SubScreenMapListener::ACTION_ID_ZOOM_END:         // 拡縮終了
    break;

  case SubScreenMapListener::ACTION_ID_LAND_CHANGE:      // 島切り替え
    break;

  case SubScreenMapListener::ACTION_ID_LAND_CHANGE_END:  // 島切り替え終了
    m_pDraw->ChangeLand( m_pPartsDraw->GetLandNo() );
    break;
  }
}


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)
