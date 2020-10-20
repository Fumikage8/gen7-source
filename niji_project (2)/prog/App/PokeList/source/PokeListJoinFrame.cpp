//==============================================================================
/**
 * @file    PokeListJoinFrame.cpp
 * @author  fukushima_yuya
 * @date    2015.03.25
 * @brief   ポケモンリスト画面参加選択フレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListJoinFrame.h"
#include "App/PokeList/source/PokeListJoinDraw.h"
#include "App/PokeList/source/PokeListJoinUpperDraw.h"
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListMenuWindowDraw.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/work/PokeListLocalWork.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Tool/AppToolTimeLimit.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/common_pokelist.gaix>
#include <niji_conv_header/message/msg_pokelist.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeListJoinFrame::PokeListJoinFrame( void )
  : m_pPokeListLowerDraw( NULL )
  , m_pPokeListUpperDraw( NULL )
  , m_Seq( 0 )
  , m_FrameCount( 60 )
  , m_ForceOut(false)
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeListJoinFrame::~PokeListJoinFrame( void )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   PokeListのセットアップ
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::SetupPokeList( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの生成
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::CreatePokeList( void )
{
  gfl2::heap::HeapBase * devHeap = m_pAppHeap->GetDeviceHeap();

  m_pPokeListLowerDraw = GFL_NEW(devHeap) PokeListJoinDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pPokeListLowerDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, m_pLocalWork );
  m_pPokeListLowerDraw->SetTitleViewUIListener( this );

  m_pPokeListUpperDraw = GFL_NEW(devHeap) PokeListJoinUpperDraw( m_pAppHeap, m_pAppRenderingManager );
  m_pPokeListUpperDraw->Init( m_pAppParam, m_pArcReadBuff, m_pStaticWork, m_pLocalWork );

  m_pPokeListLowerDraw->AddSubView( m_pPokeListUpperDraw );

  this->SetBaseDraw( m_pPokeListLowerDraw );
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの破棄
 */
//------------------------------------------------------------------------------
bool PokeListJoinFrame::EndPokeList( void )
{
  // GFNMCat[4997]
  if( !m_ForceOut )
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

    m_ForceOut = true;
  }

  if( m_pPokeListUpperDraw )
  {
    if( !m_pPokeListUpperDraw->End() ) return false;

    m_pPokeListUpperDraw->RemoveFromSuperView();    // @fix GFNMCat[4752]

    GFL_SAFE_DELETE( m_pPokeListUpperDraw );
  }

  if( m_pPokeListLowerDraw )
  {
    if( !m_pPokeListLowerDraw->End() ) return false;

    GFL_SAFE_DELETE( m_pPokeListLowerDraw );
  }

  //GFL_SAFE_DELETE( m_pTimeLimit );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの更新
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::UpdatePokeList( void )
{
  enum {
    UPD_SEQ_READY,
    UPD_SEQ_MAIN,
    UPD_SEQ_TIME_OUT,
    UPD_SEQ_LIST_BREAK,
    UPD_SEQ_END,
    UPD_SEQ_NUM,
  };

  if( m_pPokeListLowerDraw ) m_pPokeListLowerDraw->UpdateTree();

  switch( m_Seq )
  {
  case UPD_SEQ_READY:
    {
      if( m_pPokeListLowerDraw && !m_pPokeListLowerDraw->IsSeqMain() ) break;
      if( m_pPokeListUpperDraw && !m_pPokeListUpperDraw->IsSeqMain() ) break;

      m_Seq = UPD_SEQ_MAIN;
    } break;

  case UPD_SEQ_MAIN:
    {
      if( m_pStaticWork )
      {
        // 外側からの終了通知を検知したら終了させる
        if( m_pStaticWork->IsListBreak() )
        {
          m_pPokeListUpperDraw->PokeListClose();
          m_pPokeListLowerDraw->PokeListClose();
          m_Seq = UPD_SEQ_LIST_BREAK;
          break;
        }

        if( m_pStaticWork->GetTimeLimitClass() )
        {
          if( m_pStaticWork->IsTimeLimit() )
          {
            // タイムアウトのため入力を無効に
            if( m_pPokeListLowerDraw ) m_pPokeListLowerDraw->SetInputEnabled( false );    // @fix cov_ctr[10169]

            // タイムアウトの処理
            SetJoinOrderTimeOut();

            // 参加ポケモンの決定
            if( m_pPokeListLowerDraw ) m_pPokeListLowerDraw->DecideJoinPoke();

            // 選択が終わったことを通知
            FinishJoinSelect();

            m_Seq = UPD_SEQ_TIME_OUT;
          }
        }
      }
    } break;

  case UPD_SEQ_LIST_BREAK:
  case UPD_SEQ_TIME_OUT: // @fix GFNMcat860
    {
      // 外側からの終了通知を検知したら終了させる
      if( m_pStaticWork->IsListBreak() )
      {
        m_pPokeListUpperDraw->PokeListClose();
        m_pPokeListLowerDraw->PokeListClose();
        m_Seq = UPD_SEQ_END;
      }
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   PokeListDrawの描画
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::DrawPokeList( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pPokeListLowerDraw ) m_pPokeListLowerDraw->DrawTree( no );
}



//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリストの終了
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::PokeListEnd( void )
{
  SetSubSeq( MM_SEQ_END );
}

//------------------------------------------------------------------------------
/**
 * @brief   外部設定パラメータの設定
 * @param   endMode   終了モード
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::SetupOutData( EndMode endMode )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   制限時間切れの参加ポケモンのセット
 */
//------------------------------------------------------------------------------
void PokeListJoinFrame::SetJoinOrderTimeOut( void )
{
  // 自動セット
  m_pStaticWork->SetJoinOrderTimeOut();
}


////------------------------------------------------------------------------------
///**
// * @brief   キー操作通知
// * @param   dir  キー方向
// * @retval  true = 操作実行
// * @retval  false = 操作失敗
// */
////------------------------------------------------------------------------------
//bool PokeListJoinFrame::OnKeyAction( u32 dir )
//{
//  SetSubSeq( MM_SEQ_END );
//
//  return true;
//}
//
//bool PokeListJoinFrame::OnKeyActionDecide( ListDraw* pListDraw, MenuWindowDraw* pMenuWindow, CursorDraw* pCursor )
//{
//  GFL_UNUSED( pListDraw );
//  GFL_UNUSED( pMenuWindow );
//  GFL_UNUSED( pCursor );
//
//  StaticWork* pStaticWork = pListDraw->GetStaticWork();
//  CursorPos cpos = pCursor->GetCursorPos();
//
//  switch( m_DispMode )
//  {
//  case DISP_MODE_NORMAL:
//    {
//      // 決定ボタンが選択された
//      if( cpos == CPOS_DECIDE )
//      {
//        u8 join_min = pStaticWork->GetModeContext().join.join_poke_count_min;
//        u8 join_max = pStaticWork->GetModeContext().join.join_poke_count_max;
//        u8 join_cnt = pStaticWork->GetJoinPokeCount();
//
//        if( join_cnt < join_min || join_cnt > join_max )
//        {
//          // NGメッセージ
//          OpenJoinFailedMessage( pMenuWindow, join_min );
//          // カーソルを非表示
//          pCursor->SetVisible( false );
//          break;
//        }
//
//        // レギュレーションチェック
//        if( false )
//        {
//          // NGメッセージ
//          break;
//        }
//
//        m_pContext->end_mode = END_MODE_DECIDE;
//        m_pStaticWork->SetResultJoinPokeIndex();
//        m_pPokeListDraw->PokeListClose();
//        return true;
//      }
//      else {
//        // カーソルを非表示
//        pCursor->SetVisible( false );
//        // メニューウィンドウを開く
//        pMenuWindow->OpenWindow();
//      }
//    } break;
//
//  case DISP_MODE_SUBWINDOW:
//    {
//      // ウィンドウを閉じる
//      CloseSubWindow( pMenuWindow );
//      // カーソルを表示
//      pCursor->SetVisible( true );
//    } break;
//  }
//
//  return false;
//}
//
//bool PokeListJoinFrame::OnKeyActionCancel( ListDraw* pListDraw, MenuWindowDraw* pMenuWindow, CursorDraw* pCursor )
//{
//  GFL_UNUSED( pListDraw );
//  GFL_UNUSED( pMenuWindow );
//  GFL_UNUSED( pCursor );
//
//  switch( m_DispMode )
//  {
//  case DISP_MODE_NORMAL:
//    {
//      ModeContext context = m_pStaticWork->GetModeContext();
//
//      if( !context.join.cancel_enable ) break;
//
//      m_pPokeListDraw->PokeListClose();
//    } break;
//
//  case DISP_MODE_SUBWINDOW:
//    {
//      // ウィンドウを閉じる
//      CloseSubWindow( pMenuWindow );
//      // カーソルを表示
//      pCursor->SetVisible( true );
//    } break;
//  }
//
//  return true;
//}
//
////------------------------------------------------------------------------------
///**
// * @brief   プレートのタッチ通知
// * @param   pListDraw   リスト
// * @param   cpos        カーソル位置
// */
////------------------------------------------------------------------------------
//void PokeListJoinFrame::OnPlateTrigger(
//  gfl2::ui::TouchPanel* pTouchPanel, ListDraw* pListDraw, MenuWindowDraw* pMenuWindow, CursorDraw* pCursor )
//{
//  GFL_UNUSED( pTouchPanel );
//  GFL_UNUSED( pListDraw );
//  GFL_UNUSED( pMenuWindow );
//  GFL_UNUSED( pCursor );
//
//  switch( m_DispMode )
//  {
//  case DISP_MODE_SUBWINDOW:
//    {
//      // ウィンドウを閉じる
//      CloseSubWindow( pMenuWindow );
//      // カーソルを表示
//      pCursor->SetVisible( true );
//    } break;
//  }
//}
//
//
////------------------------------------------------------------------------------
///**
// * @brief   メニューウィンドウの選択通知
// * @param   pListDraw   リスト
// * @param   pMenuWindow メニューウィンドウ
// * @param   pCursor     カーソル
// * @param   id          選択した項目ID
// * @return  true == 入力を有効、false == 入力を無効
// */
////------------------------------------------------------------------------------
//bool PokeListJoinFrame::OnSelectMenuWindow(
//  ListDraw* pListDraw,
//  MenuWindowDraw* pMenuWindow,
//  CursorDraw* pCursor, u32 id )
//{
//  GFL_UNUSED( pListDraw );
//  GFL_UNUSED( pMenuWindow );
//  GFL_UNUSED( pCursor );
//
//  MenuAltID alt_id = pMenuWindow->GetSelectAltID( id );
//  CursorPos cpos = pMenuWindow->GetCursorPos();
//
//  PlateDraw* pPlate = pListDraw->GetPlateDraw( cpos );
//  PlateMessageWindow* pMsgWindow = pPlate->GetPlateMessageWindow();
//
//  u8 join_min = m_pStaticWork->GetModeContext().join.join_poke_count_min;
//  u8 join_max = m_pStaticWork->GetModeContext().join.join_poke_count_max;
//  u8 join_cnt = m_pStaticWork->GetJoinPokeCount();
//
//  FUKUSHIMA_PRINT("JOIN MIN = %d\n", join_min);
//  FUKUSHIMA_PRINT("JOIN MAX = %d\n", join_max);
//  FUKUSHIMA_PRINT("JOIN CNT = %d\n", join_cnt);
//
//  switch( alt_id )
//  {
//  //! さんかする
//  case ALT_JOIN_JOIN:
//    {
//      if( join_cnt < join_max )
//      {
//        u8 poke_index = m_pStaticWork->GetPokeIndex( cpos );
//        m_pStaticWork->AddJoinPoke( poke_index );
//
//        // プレートメッセージウィンドウの更新
//        UpdatePlateMessageWindow( pListDraw, cpos );
//
//        join_cnt = m_pStaticWork->GetJoinPokeCount();
//
//        // 決定ボタンの表示
//        //m_pPokeListDraw->SetVisibleDecideBtn( (join_cnt >= join_min) );
//
//        if( join_max == join_cnt )
//        {
//          pCursor->SetCursorPos( CPOS_DECIDE );
//        }
//      }
//      else {
//        // NGメッセージの表示
//        OpenJoinOverMessage( pMenuWindow, join_max );
//        // カーソルを非表示
//        pCursor->SetVisible( false );
//
//        return true;
//      }
//    } break;
//
//  //! さんかしない
//  case ALT_JOIN_CANCEL:
//    {
//      u8 poke_index = m_pStaticWork->GetPokeIndex( cpos );
//      m_pStaticWork->RemoveJoinPoke( poke_index );
//      pMsgWindow->CloseWindow();
//
//      for( u8 i=0; i<PLATE_NUM; ++i )
//      {
//        CursorPos cur_pos = static_cast<CursorPos>(i);
//        UpdatePlateMessageWindow( pListDraw, cur_pos );
//      }
//
//      join_cnt = m_pStaticWork->GetJoinPokeCount();
//      // 決定ボタンの非表示
//      //m_pPokeListDraw->SetVisibleDecideBtn( (join_cnt >= join_min) );
//    } break;
//  }
//
//  pCursor->SetVisible( true );
//  pMenuWindow->CloseWindow();
//
//  return true;
//}
//
////--------------------------------------------------------------------------
///**
// * @brief   プレートメッセージウィンドウの更新
// * @param   pListDraw   ListDrawクラスポインタ
// * @param   cpos        カーソル位置
// */
////--------------------------------------------------------------------------
//void PokeListJoinFrame::UpdatePlateMessageWindow( ListDraw* pListDraw, CursorPos cpos )
//{
//  PlateDraw* pPlate = pListDraw->GetPlateDraw( cpos );
//  PlateMessageWindow* pMsgWnd = pPlate->GetPlateMessageWindow();
//
//  pMsgWnd->SetupMessageWindow( PLATE_MODE_JOIN );
//}
//
////--------------------------------------------------------------------------
///**
// * @brief   参加条件を満たしていないときのNGメッセージの表示
// * @param   pWindow   ウィンドウ
// * @param   join_min  最小参加数
// */
////--------------------------------------------------------------------------
//void PokeListJoinFrame::OpenJoinFailedMessage( MenuWindowDraw* pWindow, u8 join_min )
//{
//  if( join_min < 2 ) return;
//
//  //MenuSubWindow* pSubWindow = pWindow->GetSubWindow();
//
//  s32 msgID = mes_pokelist_04_31 + join_min - 2;
//
//  pWindow->CloseWindow( false, true );
//  //pSubWindow->OpenWindow( MENU_NULL, CPOS_NULL, false );
//  //pSubWindow->SetMessage( MSG_TYPE_MSG_ONLY, msgID );
//
//  m_DispMode = DISP_MODE_SUBWINDOW;
//}
//
////--------------------------------------------------------------------------
///**
// * @brief   上限以上の参加をしようとしたときのNGメッセージの表示
// * @param   pWindow   ウィンドウ
// * @param   join_max  最大参加数
// */
////--------------------------------------------------------------------------
//void PokeListJoinFrame::OpenJoinOverMessage( MenuWindowDraw* pWindow, u8 join_max )
//{
//  if( join_max == 0 || join_max == 6 ) return;
//
//  //MenuSubWindow* pSubWindow = pWindow->GetSubWindow();
//
//  s32 msgID = mes_pokelist_04_40 + join_max - 1;
//
//  pWindow->CloseWindow( false, true );
//  //pSubWindow->OpenWindow( MENU_NULL, CPOS_NULL, false );
//  //pSubWindow->SetMessage( MSG_TYPE_MSG_ONLY, msgID );
//
//  m_DispMode = DISP_MODE_SUBWINDOW;
//}
//
////--------------------------------------------------------------------------
///**
// * @brief   サブウィンドウを閉じる
// * @param   pWindow   ウィンドウ
// */
////--------------------------------------------------------------------------
//void PokeListJoinFrame::CloseSubWindow( MenuWindowDraw* pWindow )
//{
//  //MenuSubWindow* pSubWindow = pWindow->GetSubWindow();
//  //pSubWindow->CloseWindow();
//
//  m_DispMode = DISP_MODE_NORMAL;
//}
//

GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
