//==============================================================================
/**
 * @file	  MenuSampleDraw.cpp
 * @brief	  描画サンプル
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================
#if PM_DEBUG

#include "MenuSampleDraw.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/parts_test.gaix>
#include <niji_conv_header/app/test/parts_test/ctr_up_lyt_test/res2d/parts_test.h>
#include <niji_conv_header/app/test/parts_test/ctr_up_lyt_test/res2d/parts_test_pane.h>
#include <niji_conv_header/app/test/parts_test/ctr_up_lyt_test/res2d/parts_test_anim_list.h>

#include "arc_index/message.gaix"
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_menuparts.h>

#include <Layout/include/gfl2_LayoutRenderPath.h>



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )

//--------------------------------------------------------------------------



    //  アニメIDテーブル
static const gfl2::lyt::LytArcIndex layoutAnimDataTable[] =
{
   LYTRES_PARTS_TEST_OHNO_PARTS_PANE_TEST_BFLAN,
};



// メッセージウィンドウ用ペイン
static const gfl2::lyt::LytPaneIndex messageWindowPane[] =
{
  PANENAME_OHNO_PARTS_PANE_TEST_PANE_TEXTBOX_00,
  PANENAME_OHNO_PARTS_PANE_TEST_PANE_TEXTBOX_01,
};



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
MenuSampleDraw::MenuSampleDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,m_heap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;

}

/// デストラクタ
MenuSampleDraw::~MenuSampleDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void MenuSampleDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff)
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

bool MenuSampleDraw::End(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->DeleteMessageWindow();
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
void MenuSampleDraw::SetTitleViewUIListener( MenuSampleDrawListener *listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void MenuSampleDraw::Update(void)
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
void MenuSampleDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager,displayNo,0 );


}


//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool MenuSampleDraw::IsExit( void )
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
// overwride::
::app::ui::UIInputListener::ListenerResult MenuSampleDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  // 決定
  if( pTouchPanel->IsTouchTrigger() ){
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
::app::ui::UIInputListener::ListenerResult MenuSampleDraw::OnLayoutPaneEvent( const u32 painId )
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
::app::ui::UIInputListener::ListenerResult MenuSampleDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ){
    return ENABLE_ACCESS;
  }

  // 決定
  if( pButton->IsTrigger(gfl2::ui::BUTTON_B) || pButton->IsTrigger(gfl2::ui::BUTTON_START) ){
    m_uiListener->OnButtonAction(0);
    return DISABLE_ACCESS;
  }
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) ){
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
void MenuSampleDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
//  static const u32 COMMON_RES_ADD_MAX = LYTRES_TURTLESALMON_RES_END + LYTRES_GRPFONT_TURTLESALMON_END;

  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

	 app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_PARTS_TEST_OHNO_PARTS_PANE_TEST_BFLYT,
        LA_PARTS_TEST_OHNO_PARTS_PANE_TEST___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };


  Create2D( heap, NULL, 1, 
            resTbl, GFL_NELEMS(resTbl), 
            setupData, GFL_NELEMS(setupData), 
            GARC_message_btl_ui_DAT );


}

//----------------------------------------------------------------------
/**
 *	@brief  初期テキスト表示
 */
//----------------------------------------------------------------------
void MenuSampleDraw::InitializeText( app::util::Heap * pheap )
{

	mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
                                                        GARC_debug_message_d_menuparts_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  
	mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );

}


void MenuSampleDraw::FinalizeText(void)
{
	GFL_DELETE mpWordSet;
  GFL_DELETE mpMsgData;
  GFL_DELETE mpTempStrBuf2;
  GFL_DELETE mpTempStrBuf;
}

void MenuSampleDraw::SetMessage( int msgid )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  mpMsgData->GetString(msgid, *mpTempStrBuf);


//  WordSet
  //mpMsgData->Expand();
	{
		int outNum;
		registerWords(mpTempStrBuf,&outNum);
	}
	mpWordSet->Expand(mpTempStrBuf2, mpTempStrBuf);

  //Expand
  

  pG2DUtil->SetMessage( mpTempStrBuf2 );
}


void MenuSampleDraw::ExpandPokemon(int no, int monsno )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->SetRegisterMonsNameNo(no, (MonsNo)monsno );
}


void MenuSampleDraw::ExpandItem(int no, int itemno)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->SetRegisterItemName(no, itemno);
}



//----------------------------------------------------------------------------------
/**
 * 文字列中のタブIDから必要な情報を判別してWORDSETに登録する
 *
 * @param   buf
 * @param   args
 * @param   wset
 */
//----------------------------------------------------------------------------------
void MenuSampleDraw::registerWords( const gfl2::str::StrBuf* buf, int* args )
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
							u16 bufIdx = print::GetTagParam( sp, 0 );
							GFL_PRINT("ここにきたらOK %d\n",bufIdx);   //トレーナIDを取得
              args[0] = bufIdx;
            }
            break;
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
}







GFL_NAMESPACE_END( MenuFrameTest )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
