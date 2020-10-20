//=============================================================================
/**
 * @file    StaffListFrame.cpp
 * @brief   momiji追加分のスタッフロール画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2017.03.02
 */
//=============================================================================

// module
#include "StaffListFrame.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// momiji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/StaffListGraphic.gaix"
#include "arc_index/StaffListData.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StaffList)

//! メインシーケンス
enum MainSeq
{
  SEQ_MAIN = 0,   //!< メイン
  SEQ_END,        //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
StaffListFrame::StaffListFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pDraw( NULL )
  , m_pListBuf( NULL )
  , m_listData( NULL )
  , m_listDataMax( 0 )
  , m_isUpdateFunc( false )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
#if PM_DEBUG
  , m_isDebugInput( false )
#endif // PM_DEBUG
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
StaffListFrame::~StaffListFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2017.03.02
 *
 * @param   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void StaffListFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCreated
 * @brief   生成が完了したか
 * @date    2017.03.16
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::IsCreated(void)
{
  return m_isUpdateFunc;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2017.03.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result StaffListFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadListItemData() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    if( CreateDraw() == false )
    {
      break;
    }

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

    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2017.03.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result StaffListFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }

  UnloadListItemData();
  UnloadMessage();
  UnloadLayoutResource();

  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2017.03.02
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result StaffListFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_MAIN:
    Update_SeqMain();
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
 * @date    2017.03.02
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void StaffListFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2017.03.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::CreateDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
#if PM_DEBUG
    m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) StaffListDraw( m_heap, m_renderingManager, this );
#else
    m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) StaffListDraw( m_heap, m_renderingManager );
#endif // PM_DEBUG
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->Init( m_pLytResBuff, m_pMsgBuff ) == false )
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
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2017.03.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::DeleteDraw(void)
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
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2017.03.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_STAFF_LIST_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_STAFF_LIST_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_STAFF_LIST_GRA,
      GARC_StaffListGraphic_StaffList_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_STAFF_LIST_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_STAFF_LIST_GRA ) == false )
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
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListFrame::UnloadLayoutResource(void)
{
  GflHeapSafeFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2017.03.02
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::LoadMessage(void)
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
      GARC_message_staff_list_DAT,
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
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListFrame::UnloadMessage(void)
{
  GflHeapSafeFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadListItemData
 * @brief   リストデータ読み込み
 * @date    2017.03.03
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::LoadListItemData(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_STAFF_LIST_DATA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_STAFF_LIST_DATA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_STAFF_LIST_DATA,
      GARC_StaffListData_momiji_staff_list_BIN,
      &m_pListBuf,
      m_heap->GetDeviceHeap(),
      false,
      4,
      gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
      NULL,
      &m_listDataMax );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pListBuf ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_STAFF_LIST_DATA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_STAFF_LIST_DATA ) == false )
    {
      break;
    }
    m_listData = static_cast<StaffListDraw::LIST_ITEM_DATA*>(m_pListBuf);
    m_listDataMax = m_listDataMax / sizeof(StaffListDraw::LIST_ITEM_DATA);
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
 * @func    UnloadListItemData
 * @brief   リストデータ削除
 * @date    2017.03.03
 */
//-----------------------------------------------------------------------------
void StaffListFrame::UnloadListItemData(void)
{
  GflHeapSafeFreeMemory( m_pListBuf );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2017.03.02
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void StaffListFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqMain
 * @brief   アップデート：メイン処理
 * @date    2017.03.02
 */
//-----------------------------------------------------------------------------
void StaffListFrame::Update_SeqMain(void)
{
  m_isUpdateFunc = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2017.03.02
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::Update_SeqEnd(void)
{
  m_isUpdateFunc = false;
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetRequest
 * @brief   動作リスクエスト
 * @date    2017.03.03
 *
 * @param   no    データ番号
 * @param   mode  背景配置モード
 */
//-----------------------------------------------------------------------------
void StaffListFrame::SetRequest( u32 no, BgPutMode mode )
{
  if( no == EX_REQUEST_NO_END )
  {
    if( m_isUpdateFunc != false )
    {
      ChangeSequence( SEQ_END );
    }
  }
  // 通常データ表示
  else
  {
    m_pDraw->SetListData( no, m_listData, m_listDataMax );
    m_pDraw->PutBgPane( mode );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsAllRequestEnd
 * @brief   全てのリクエストが終了したか
 * @date    2017.03.17
 *
 * @retval  true  = 終了した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool StaffListFrame::IsAllRequestEnd(void)
{
  if( m_pDraw->IsSetLabel() == false )
  {
    return true;
  }
  return false;
}


#if PM_DEBUG
void StaffListFrame::DebugSetup(void)
{
  m_isDebugInput = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   動作リスナー
 * @date    2017.03.02
 *
 * @param   id    動作ID
 *
 * @note  override StaffListDraw::Listener
 */
//-----------------------------------------------------------------------------
void StaffListFrame::OnAction( StaffListDraw::Listener::ActionID id )
{
  if( m_isDebugInput != false )
  {
    switch( id )
    {
    case StaffListDraw::Listener::ACTION_ID_REQ_0:
      SetRequest( 1, BG_PUT_CENTER );
      break;
    case StaffListDraw::Listener::ACTION_ID_REQ_1:
      SetRequest( 6, BG_PUT_LEFT );
      break;
    case StaffListDraw::Listener::ACTION_ID_REQ_2:
      SetRequest( 7, BG_PUT_RIGHT );
      break;
    case StaffListDraw::Listener::ACTION_ID_END:
      SetRequest( EX_REQUEST_NO_END );
      break;
    }
  }
}
#endif // PM_DEBUG


GFL_NAMESPACE_END(StaffList)
GFL_NAMESPACE_END(App)
