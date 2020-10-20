//==============================================================================
/**
 * @file    PokeListBaseFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベースフレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseDraw.h"
#include "App/PokeList/source/PokeListBaseFrame.h"
#include "App/PokeList/source/PokeListUpperDraw.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/AppToolTimeLimit.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_heap.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>
#include <arc_index/message.gaix>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };

  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };
}


//------------------------------------------------------------------------------
/**
 * @brief	  コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListBaseFrame::PokeListBaseFrame( void )
  : m_pAppHeap( NULL )
  , m_pAppRenderingManager( NULL )
  , m_pAppParam( NULL )
  , m_pContext( NULL )
  , m_pStatusAppParam( NULL )
  , m_pBagAppParam( NULL )
  , m_pTimeLimit( NULL )
  , m_pStaticWork( NULL )
  , m_pLocalWork( NULL )
  , m_pBaseDraw( NULL )
  , m_pUpperDraw( NULL )
  , m_pArcReadBuff( NULL )
  , m_pMsgReadBuff( NULL )
  , m_ReadSeq( 0 )
  , m_InitSeq( 0 )
  , m_DrawEnable(false)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListBaseFrame::~PokeListBaseFrame()
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result PokeListBaseFrame::InitFunc( void )
{
#if 0
  GFL_ASSERT( m_pAppParam );

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_CMN_POKELIST;
  u32 dataID = GARC_common_pokeList_common_pokeList_applyt_COMP;

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;
      if( !m_pLocalWork->IsReady() ) break;

      CreatePokeList();   //!< 継承先で個別にDRAWを生成する

      // 上画面の描画
      if( m_pContext->mode != MODE_BATTLE && m_pContext->mode != MODE_JOIN )
      {
        m_pUpperDraw = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokeListUpperDraw( m_pAppHeap, m_pAppRenderingManager );
        m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff );

        m_pBaseDraw->AddSubView( m_pUpperDraw );
      }

      app::util::FileAccessor::FileClose( arcID, devHeap );

      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;

      return applib::frame::RES_FINISH;
    }
    //break;  //!< no break
  }

  return applib::frame::RES_CONTINUE;
#else
  // メッセージデータの読み込み
  if( !LoadMsgData() ) return applib::frame::RES_CONTINUE;

  // ワードセットローダーの生成
  if( !CreateWordSetLoader() ) return applib::frame::RES_CONTINUE;

  // レイアウトデータの読み込み
  if( !LoadLayoutData() ) return applib::frame::RES_CONTINUE;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  // ローカルワークの生成
  m_pLocalWork = GFL_NEW(devHeap) LocalWork( devHeap, m_pMsgData, m_pWordSet, m_pWordSetLoader );

  CreatePokeList();   //!< 継承先で個別にDRAWを生成する

  // 上画面の描画
  if( m_pContext->mode != MODE_BATTLE && m_pContext->mode != MODE_JOIN )
  {
    m_pUpperDraw = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokeListUpperDraw( m_pAppHeap, m_pAppRenderingManager );
    m_pUpperDraw->Init( m_pAppParam, m_pArcReadBuff );

    m_pBaseDraw->AddSubView( m_pUpperDraw );
  }

  // 描画許可フラグ
  if( m_pContext->mode != MODE_BATTLE )
  {
    m_DrawEnable = true;
  }

  return applib::frame::RES_FINISH;
#endif
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------------------
applib::frame::Result PokeListBaseFrame::UpdateFunc( void )
{
  UpdatePokeList();

  applib::frame::Result result;

  switch( GetSubSeq() )
  {
  case MM_SEQ_END:
    {
      result = applib::frame::RES_FINISH;
    } break;

  default:
    {
      result = applib::frame::RES_CONTINUE;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  DrawPokeList( no );
}

//------------------------------------------------------------------------------
/**
 * @brief	  PROCの終了を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------------------
applib::frame::Result PokeListBaseFrame::EndFunc( void )
{
  if( m_pUpperDraw )
  {
    if( !m_pUpperDraw->End() )
    {
      return applib::frame::RES_CONTINUE;
    }

    m_pUpperDraw->RemoveFromSuperView();    // @fix GFNMCat[4752]

    GFL_SAFE_DELETE( m_pUpperDraw );
  }

  if( !EndPokeList() )
  {
    return applib::frame::RES_CONTINUE;
  }

  GFL_SAFE_DELETE( m_pWordSetLoader );
  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

  GflHeapFreeMemory( m_pMsgReadBuff );
  GflHeapFreeMemory( m_pArcReadBuff );

  GFL_SAFE_DELETE( m_pLocalWork );
  GFL_SAFE_DELETE( m_pStaticWork );

  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief	  レイアウトデータの読み込み
 */
//------------------------------------------------------------------------------
bool PokeListBaseFrame::LoadLayoutData( void )
{
  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = ARCID_CMN_POKELIST;
  u32 dataID = GARC_common_pokeList_common_pokeList_applyt_COMP;

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pArcReadBuff,
        devHeap,
        true,
        nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pArcReadBuff ) ) break;

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      m_ReadSeq = 0;

      return true;
    }
    //break;  //!< no break
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief	  メッセージデータの読み込み
 */
//------------------------------------------------------------------------------
bool PokeListBaseFrame::LoadMsgData( void )
{
  if( m_InitSeq >= 1 ) return true;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();
  u32 arcID = print::GetMessageArcId();
  u32 dataID = GARC_message_pokelist_DAT;

  switch( m_ReadSeq )
  {
  case LOCAL_OPEN_START:
    {
      app::util::FileAccessor::FileOpen( arcID, devHeap );
      m_ReadSeq = LOCAL_READ_START;
    }
    //break;  //!< no break

  case LOCAL_READ_START:
    {
      if( !app::util::FileAccessor::IsFileOpen( arcID ) ) break;

      app::util::FileAccessor::FileLoad(
        arcID,
        dataID,
        &m_pMsgReadBuff,
        devHeap,
        false,
        4 );

      m_ReadSeq = LOCAL_READ_WAIT;
    }
    //break;  //!< no break

  case LOCAL_READ_WAIT:
    {
      if( !app::util::FileAccessor::IsFileLoad( &m_pMsgReadBuff ) ) break;

      // Drawの生成

      app::util::FileAccessor::FileClose( arcID, devHeap );
      m_ReadSeq = LOCAL_CLOSE_WAIT;
    }
    //break;  //!< no break

  case LOCAL_CLOSE_WAIT:
    {
      if( !app::util::FileAccessor::IsFileClose( arcID ) ) break;

      // メッセージデータ
      m_pMsgData = GFL_NEW(devHeap) gfl2::str::MsgData( m_pMsgReadBuff, devHeap );
      GFL_ASSERT( m_pMsgData );

      // ワードセット
      m_pWordSet = GFL_NEW(devHeap) print::WordSet( devHeap );
      GFL_ASSERT( m_pWordSet );

      m_ReadSeq = 0;
      m_InitSeq++;

      return true;
    }
    //break;  //!< no break
  }

  return false;
}
  
//------------------------------------------------------------------------------
/**
 * @brief	  ワードセットローダーの生成
 */
//------------------------------------------------------------------------------
bool PokeListBaseFrame::CreateWordSetLoader( void )
{
  if( m_InitSeq >= 2 ) return true;

  gfl2::heap::HeapBase* devHeap = m_pAppHeap->GetDeviceHeap();

  switch( m_ReadSeq )
  {
  case 0:
    {
      u32 id = 0;

      id = PRELOAD_ITEMNAME | PRELOAD_WAZANAME;

      m_pWordSetLoader = GFL_NEW(devHeap) WordSetLoader( devHeap, id );
      m_pWordSetLoader->StartLoad( devHeap->GetLowerHandle() );

      m_ReadSeq++;
    } //break;

  case 1:
    {
      if( !m_pWordSetLoader->WaitLoad() ) break;

      m_ReadSeq = 0;
      m_InitSeq++;

      return true;
    } // break;
  }
  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief	  動作に必要な引数を登録
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::Setup( App::PokeList::APP_PARAM* pAppParam, App::PokeList::CONTEXT_PARAM* pContext )
{
  m_pAppParam = pAppParam;
  m_pContext = pContext;

  m_pAppHeap = m_pAppParam->mpAppHeap;
  m_pAppRenderingManager = m_pAppParam->mpAppRenderingManager;

  m_pTimeLimit = pContext->time_limit;

  SetupPokeList();  //!< 継承先での個別のセットアップ

  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();
  GameSys::GameManager * gm = m_pAppParam->mpGameManager;
  app::tool::AppCommonGrpIconData * pCmnIcon = m_pAppParam->mpCmnIcon;

  // スタティックワーク
  m_pStaticWork = GFL_NEW(devHeap)
    StaticWork( gm, devHeap, m_pContext, m_pTimeLimit, pCmnIcon );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータのセット
 * @param   pStatusParam    ステータス用外部設定パラメータのポインタ
 * @param   pBagParam       バッグ用外部設定パラメータのポインタ
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::SetOutParam( Status::APP_PARAM* pStatusParam, app::bag::APP_PARAM* pBagParam )
{
  m_pStatusAppParam = pStatusParam;
  m_pBagAppParam = pBagParam;
}

//------------------------------------------------------------------------------
/**
 * @brief	  DRAWのセット
 * @param   pBaseDraw   ベースDRAW
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::SetBaseDraw( PokeListBaseDraw* pBaseDraw )
{
  m_pBaseDraw = pBaseDraw;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンステータスの呼び出し
 * @param   pokeIndex   ポケモンのインデックス
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::CallPokeStatus( u8 pokeIndex )
{
  IntermediateData* pData = m_pStaticWork->GetPokeParty( PARTY_SELF );

  m_pStatusAppParam->party_data.SetParameter( pData->GetPokeParty() );

  m_pStatusAppParam->poke_index = pokeIndex;
  m_pStatusAppParam->time_limit = m_pTimeLimit;

  if( m_pStaticWork->GetProcMode() == MODE_FIELD )
  {
    m_pStatusAppParam->update_prohibition = false;
  }

  SetupOutData( END_MODE_CALL_STATUS );
  m_pBaseDraw->PokeListClose( END_MODE_CALL_STATUS );

  m_pStaticWork->SyncPokeOrder();

  // 参加選択の時は参加状況を保存
  if( m_pStaticWork->GetProcMode() == MODE_JOIN )
  {
    m_pStaticWork->SetResultJoinPokeIndex( false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   バッグの呼び出し
 * @param   pokeIndex   ポケモンのインデックス
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::CallBag( u8 pokeIndex )
{
  m_pStaticWork->SyncPokeOrder();

  m_pBagAppParam->call_mode = app::bag::CALL_FIELD_POKELIST;

  IntermediateData* pData = m_pStaticWork->GetPokeParty( PARTY_SELF );
  m_pBagAppParam->pokemon = pData;
  m_pBagAppParam->pokemon_index = pokeIndex;

  m_pBaseDraw->PokeListClose( END_MODE_CALL_BAG );
}

//------------------------------------------------------------------------------
/**
 * @brief   バッグ(かいふく)の呼び出し
 * @param   pokeIndex   ポケモンのインデックス
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::CallBagRepair( u8 pokeIndex )
{
  m_pStaticWork->SyncPokeOrder();

  m_pBagAppParam->call_mode = app::bag::CALL_FIELD_POKELIST;

  IntermediateData* pData = m_pStaticWork->GetPokeParty( PARTY_SELF );
  m_pBagAppParam->pokemon       = pData;
  m_pBagAppParam->pokemon_index = pokeIndex;
  m_pBagAppParam->pocket_no     = BAG_POCKET_DRUG;

  m_pBaseDraw->PokeListClose( END_MODE_CALL_BAG );
}

//------------------------------------------------------------------------------
/**
 * @brief   参加選択で自分の選択を終えた
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::FinishJoinSelect( void )
{
  m_pContext->selected_wait[PARTY_SELF] = JOIN_SELECT_WAITING;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの再起動
 * @note    タッグ戦のみ使用
 */
//------------------------------------------------------------------------------
void PokeListBaseFrame::RebootPokeList( App::PokeList::CONTEXT_PARAM& rContextParam )
{
  m_pBaseDraw->RebootPokeList( rContextParam );
}

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
