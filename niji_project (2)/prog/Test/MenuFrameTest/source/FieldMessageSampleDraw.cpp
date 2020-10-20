//==============================================================================
/**
 * @file	  MenuSampleDraw.cpp
 * @brief	  描画サンプル
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================
#if PM_DEBUG

#include "FieldMessageSampleDraw.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/parts_test.gaix>
//#include <niji_conv_header/app/test/parts_test/ctr_winmsg_test/res2d/msgwin_test.h>
//#include <niji_conv_header/app/test/parts_test/ctr_winmsg_test/res2d/msgwin_test_pane.h>
//#include <niji_conv_header/app/test/parts_test/ctr_winmsg_test/res2d/msgwin_test_anim_list.h>

//fieldwindow.gaix

#include "arc_index/message.gaix"
#include <arc_index/debug_message.gaix>
#include <arc_index/script_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>


#include "niji_conv_header/app/window/res2d/fieldwindow.h"
#include "niji_conv_header/app/window/res2d/fieldwindow_anim_list.h"
#include "niji_conv_header/app/window/res2d/fieldwindow_pane.h"



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldMessageTest )

//--------------------------------------------------------------------------



//  アニメIDテーブル
static const gfl2::lyt::LytArcIndex layoutAnimDataTable[] =
{
  LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01,
};



// メッセージウィンドウ用ペイン
static const gfl2::lyt::LytPaneIndex messageWindowPane[] =
{
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_00,
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_01,
};



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
FieldMessageSampleDraw::FieldMessageSampleDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,m_heap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;
  mpTrNameMsgData=NULL;

}

/// デストラクタ
FieldMessageSampleDraw::~FieldMessageSampleDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void FieldMessageSampleDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff)
{


  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_heap, false );


  Initialize2D( m_heap,pAppLytBuff );

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  InitializeText(m_heap);

  pG2DUtil->CreateMessageWindow(  app::util::G2DUtil::SETUP_UPPER, messageWindowPane,true, m_heap  );




  //  m_g2dUtil->CreateMessageWindow( m_uiDeviceManager, LAYOUT_ID_UPPER, MESSAGE_WINDOW_PANE, m_heap, true, 3 );


  //void G2DUtil::CreateLayoutWork( u32 lytMax, Heap * heap )



  SetInputListener( this );
}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool FieldMessageSampleDraw::End(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->DeleteMessageWindow();

  if(mpTrNameMsgData)
  {
    GFL_DELETE mpTrNameMsgData;
  }
  FinalizeText();
  Delete2D();
  DeleteGraphicsSystem();
  return true;
}


//----------------------------------------------------------------------
/**
 *  @brief  UIイベント用リスナーの登録
 */
//----------------------------------------------------------------------
void FieldMessageSampleDraw::SetTitleViewUIListener( FieldMessageSampleDrawListener *listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void FieldMessageSampleDraw::Update(void)
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();

}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void FieldMessageSampleDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, displayNo, 0 );
}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool FieldMessageSampleDraw::IsExit( void )
{
  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief  タッチパネルイベントの検知
 *
 * @param  pTouchPanel タッチパネル
 * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
 *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//--------------------------------------------------------------------------------------------
// overwride
::app::ui::UIInputListener::ListenerResult FieldMessageSampleDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() )
  {
    //    m_uiListener->OnButtonAction(0);
    return DISABLE_ACCESS;
  }
  return ENABLE_ACCESS;
}

//------------------------------------------------------------------
/**
 * @brief   ペインイベントの検知
 *
 * @param  layoutId 通知を受けるレイアウトＩＤ
 * @param  painId   通知を受けるペインＩＤ
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//------------------------------------------------------------------
// overwride
app::ui::UIInputListener::ListenerResult FieldMessageSampleDraw::OnLayoutPaneEvent( const u32 painId )
{
  return ENABLE_ACCESS;
}


//------------------------------------------------------------------
/**
 * @brief   キー入力の検知
 *
 * @param  pKey    十字キー
 * @param  pStick  アナログスティック
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//------------------------------------------------------------------
// overwride
app::ui::UIInputListener::ListenerResult FieldMessageSampleDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  // 決定
  if( pButton->IsTrigger(gfl2::ui::BUTTON_B) || pButton->IsTrigger(gfl2::ui::BUTTON_START) )
  {
    m_uiListener->OnButtonAction(0);
    return DISABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    m_uiListener->OnButtonAction(1);
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//==========================================================================
/**
 *        private関数
 */
//==========================================================================
//----------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   heap    ヒープ
 */
//----------------------------------------------------------------------
void FieldMessageSampleDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  //  static const u32 COMMON_RES_ADD_MAX = LYTRES_TURTLESALMON_RES_END + LYTRES_GRPFONT_TURTLESALMON_END;

  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[] = {
    {//上
      0,
      LYTRES_FIELDWINDOW_WINDOW_MSG_UPP_000_BFLYT,
      LA_FIELDWINDOW_WINDOW_MSG_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
      },
  };


  Create2D( heap, NULL, 1,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            UNUSE_MESSAGE_DATA );


  //	mpScriptMsgData = GFL_NEW(heap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Script(),
  //                       GARC_script_message_test_event_0010_DAT,
  //													heap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
  //	mpScriptWordSet = GFL_NEW( heap->GetDeviceHeap() ) print::WordSet( heap->GetDeviceHeap() );


  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  pG2DUtil->SetMessageData( mpScriptMsgData, mpScriptWordSet );

}

//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------
void FieldMessageSampleDraw::InitializeText( app::util::Heap * pheap )
{

  //	mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
  //                                                        GARC_debug_message_d_menuparts_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
  mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Script(),
                                                                  GARC_script_message_test_event_0010_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTrainerStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );

}


void FieldMessageSampleDraw::FinalizeText(void)
{
  GFL_DELETE mpWordSet;
  GFL_DELETE mpMsgData;
  GFL_DELETE mpTrainerStrBuf;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf;
}

void FieldMessageSampleDraw::SetMessage( int msgid )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  mpMsgData->GetString(msgid, *mpTempStrBuf);

  // mpWordSet->Expand(mpTempStrBuf2, mpTempStrBuf);

  pG2DUtil->SetMessage( mpTempStrBuf );

  pG2DUtil->StartAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01 );

  DispFieldName();
}




//----------------------------------------------------------------------------------
/**
 *  文字列中のタブIDから名前欄のトレーナーmsg番号を取り出す
 *
 * @param   buf    文字列
 * @param   args   取り出せたらトレーナーの名前msgのインデックス順番
 * @return  取り出せたらtrue なければfalse
 */
//----------------------------------------------------------------------------------
bool FieldMessageSampleDraw::GetTrainerNo( const gfl2::str::StrBuf* buf, int* args )
{
  const gfl2::str::STRCODE* sp;
  u8  argIdxDec[print::WordSet::DEFAULT_WORD_MAX];
  const gfl2::str::STRCODE eomCode = gfl2::str::EOM_CODE;
  const gfl2::str::STRCODE tagCode = gfl2::str::TAG_START_CODE;

  // タグを走査して、bufIdxから減算すべき値argIdxDecのテーブルを作成。
  // タグが bufIdx の順に出現するとは限らないため（特に、言語によってはトレーナータイプ名がトレーナー名よりも後に来る点が挙げられる）。
  for ( int i = 0; i < GFL_NELEMS(argIdxDec); i ++ )
  {
    argIdxDec[i] = 0;
  }
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u8  tagGrp = print::GetTagGroup( sp );

        if( tagGrp == print::TAGGROUP_WORD )
        {
          u8 tagIdx = print::GetTagIndex( sp );
          switch( tagIdx ){
          case print::TAGIDX_WORD_TRAINER_TYPE:
          case print::TAGIDX_WORD_TRAINER_TYPE_AND_NAME:
            // [Name:TrainerType ] の bufIdx より大きい bufIdx を持つWORDは、取得するargIdxを1減らす
            for ( int i = bufIdx+1; i < GFL_NELEMS(argIdxDec); i ++ )
            {
              argIdxDec[i] ++;
            }
            break;
          default:
            break;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else {
      ++sp;
    }
  }

  // 文字列中に出現するタグに対応するWORDをWordSetに設定
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u32 argIdx = bufIdx;
        u8  tagGrp = print::GetTagGroup( sp );
        if( argIdx >= argIdxDec[bufIdx] ){
          argIdx -= argIdxDec[bufIdx];
        }
        //   BTL_N_Printf( DBGSTR_PRINT_BufIdx, bufIdx);

        if( tagGrp == print::TAGGROUP_NUMBER )
        {
//          u8 keta = print::GetTagIndex( sp ) + 1;
          //  SysWork.wset->RegisterNumber( bufIdx, args[argIdx], keta, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        }
        else
        {
          u8 tagIdx = print::GetTagIndex( sp );

          GFL_PRINT("[BTLSTR] TagIdx=%d\n", tagIdx);

          switch( tagIdx ){
          case print::TAGIDX_WORD_TRAINER_NAME_FIELD:
            {
//              u8 clientID = args[ argIdx ];
              u16 bufIdx2 = print::GetTagParam( sp, 0 );
              GFL_PRINT("ここにきたらOK %d\n",bufIdx2);   //トレーナIDを取得
              args[0] = bufIdx2;
            }
            return true;
//            break;
          default:
            GFL_ASSERT_MSG(0, "unknown tagIdx = %d\n", tagIdx);
            break;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else{
      ++sp;
    }
  }
  return false;
}


void FieldMessageSampleDraw::DispFieldName(void )
{
  int trno = 0;

  if(GetTrainerNo(mpTempStrBuf,&trno))
  {
    GFL_PRINT("FieldMessageSampleDraw  %d \n",trno);

    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

    mpTrNameMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(),
                                                                           GARC_message_trname_DAT, m_heap->GetDeviceHeap(),
                                                                           gfl2::str::MsgData::LOAD_FULL );

    mpTrNameMsgData->GetString( trno, *mpTrainerStrBuf );

    pG2DUtil->SetTextBoxPaneString(LAYOUT_ID_UPPER, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXT_NAME, mpTrainerStrBuf);
  }
}




GFL_NAMESPACE_END( FieldMessageTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG
