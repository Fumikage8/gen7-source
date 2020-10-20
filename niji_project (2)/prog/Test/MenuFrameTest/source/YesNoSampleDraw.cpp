//==============================================================================
/**
 * @file	  YesNoSampleDraw.cpp
 * @brief	  描画サンプル
 * @author	ohno
 * @data	  February 17, 2015
 */
// =============================================================================
#if PM_DEBUG

#include "YesNoSampleDraw.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/yesno_test.gaix>
#include <niji_conv_header/app/test/parts_test/ctr_yesno_test/res2d/yesno_test.h>
#include <niji_conv_header/app/test/parts_test/ctr_yesno_test/res2d/yesno_test_pane.h>
#include <niji_conv_header/app/test/parts_test/ctr_yesno_test/res2d/yesno_test_anim_list.h>


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
  0,
};






//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap  ヒープ
 */
//----------------------------------------------------------------------
YesNoSampleDraw::YesNoSampleDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
,m_heap( heap )
,m_uiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;

}

/// デストラクタ
YesNoSampleDraw::~YesNoSampleDraw( void )
{
}

//----------------------------------------------------------------------
/**
 *  @brief  初期化
 */
//----------------------------------------------------------------------

void YesNoSampleDraw::Init(APP_PARAM* pAppParam, void* pAppLytBuff)
{


	SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
	CreateGraphicsSystem( m_heap, false );


  Initialize2D( m_heap,pAppLytBuff );

//  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  InitializeText(m_heap);

  SetInputListener( this );
}

//----------------------------------------------------------------------
/**
 *  @brief  終了処理
 */
//----------------------------------------------------------------------

bool YesNoSampleDraw::End(void)
{
  //app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
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
void YesNoSampleDraw::SetTitleViewUIListener( YesNoSampleDrawListener *listener )
{
  m_uiListener = listener;
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void YesNoSampleDraw::Update(void)
{

//  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  app::ui::UIView::Update();

}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void YesNoSampleDraw::Draw(gfl2::gfx::CtrDisplayNo displayNo)
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
bool YesNoSampleDraw::IsExit( void )
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
::app::ui::UIInputListener::ListenerResult YesNoSampleDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
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
::app::ui::UIInputListener::ListenerResult YesNoSampleDraw::OnLayoutPaneEvent( const u32 painId )
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
::app::ui::UIInputListener::ListenerResult YesNoSampleDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_uiListener == NULL ){
    return MYSUBVIEW_ONLY_ACCESS;
  }

  // 決定
  if( pButton->IsTrigger(gfl2::ui::BUTTON_X) ){
    m_uiListener->OnButtonAction(0);
    return MYSUBVIEW_ONLY_ACCESS;
  }

  return MYSUBVIEW_ONLY_ACCESS;
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
void YesNoSampleDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
//  static const u32 COMMON_RES_ADD_MAX = LYTRES_TURTLESALMON_RES_END + LYTRES_GRPFONT_TURTLESALMON_END;

  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  pAppLytBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_YESNO_TEST_OHNO_YESNOTEST_BFLYT,
        LA_YESNO_TEST_OHNO_YESNOTEST___NUM,
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
void YesNoSampleDraw::InitializeText( app::util::Heap * pheap )
{

	mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(),
                                                        GARC_debug_message_d_menuparts_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  

}


void YesNoSampleDraw::FinalizeText(void)
{
  GFL_DELETE mpMsgData;
  GFL_DELETE mpTempStrBuf;
}

void YesNoSampleDraw::SetMessage( int msgid )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  mpMsgData->GetString(msgid, *mpTempStrBuf);

  pG2DUtil->SetMessage( mpTempStrBuf );
}





GFL_NAMESPACE_END( MenuFrameTest )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
