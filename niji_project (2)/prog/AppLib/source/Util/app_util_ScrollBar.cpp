//============================================================================================
/**
 * @file		app_util_ScrollBar.cpp
 * @brief		スクロールバーの簡易処理
 * @author	Hiroyuki Nakamura
 * @date		11.11.10
 */
//============================================================================================
#include "AppLib/include/Util/app_util_ScrollBar.h"

#include <Layout/include/gfl2_LayoutRenderPath.h>
#include <AppLib/include/Tool/app_tool_PaneList.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

static const f32 ALPHA_FADE_SPEED = 0.1f;		//!< アルファ加減値


//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param   param   セットアップパラメータ
 */
//--------------------------------------------------------------------------------------------
ScrollBar::ScrollBar( const SETUP_PARAM * param )
{
  if( param->lytwk != NULL )
  {
    gfl2::lyt::LytParts * parts = param->lytwk->GetPartsPane( param->parts_index );
    pScrollBar = GFL_NEW( param->heap ) app::tool::ScrollBar(
                                          parts,
                                          param->lytwk->GetPane( parts, param->mark_index, &param->res_id ),
                                          param->lytwk->GetPane( parts, param->top_index, &param->res_id ),
                                          param->lytwk->GetPane( parts, param->bottom_index, &param->res_id ),
                                          param->dir );
  }
  else
  {
    pScrollBar = GFL_NEW( param->heap ) app::tool::ScrollBar( param->parts_pane, param->mark_pane, param->top_pane, param->bottom_pane, param->dir );
  }
  ChangeParam( param->max );
  SetFrameMode( GameSys::FRAMEMODE_DEFAULT );
  SetDrawEnable( true );
  SetRequest( REQ_BAR_OFF );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
ScrollBar::~ScrollBar(void)
{
	GFL_DELETE pScrollBar;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		描画
 *
 * @param		val		現在のスクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::Draw( f32 val )
{
  if( is_request && request_num != 0 )
  {
    request = next_request[ request_num-1 ];

    bool is_enable = false;
    switch( request ){
    default:
      GFL_ASSERT(0);
      is_enable = true;
      break;
    case REQ_BAR_IN:
      fadeSpeed = alpha_speed;
      is_enable = true;
      break;
    case REQ_BAR_OUT:
      if( alpha > 0.7f )
      {
        fadeSpeed = -alpha_speed;
        is_enable = true;
      }
      break;
    }

    if( is_enable )
    {
      is_request = false;
      --request_num;
    }
  }
	if( fadeSpeed > 0.0f ){
		alpha += fadeSpeed;
		if( alpha >= 1.0f ){
			SetRequest( REQ_BAR_ON );
		}
	}else if( fadeSpeed < 0.0f ){
		alpha += fadeSpeed;
		if( alpha <= 0.0f ){
			SetRequest( REQ_BAR_OFF );
		}
	}

  if( is_visible )
  {
    if( max == 0.0f )
    {
      pScrollBar->Draw( 0.0f, alpha * 255.0f );
    }
    else
    {
      pScrollBar->Draw( val/max, alpha * 255.0f );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール最大値を変更
 *
 * @param		scroll_max	スクロール最大値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::ChangeParam( u32 scroll_max )
{
  max = static_cast<f32>( scroll_max );
	if( scroll_max == 0 ){
    pScrollBar->SetMarkSize( max );
	}else{
    pScrollBar->SetMarkSize( max+1.0f );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リクエスト発行
 *
 * @param		req		リクエスト
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::SetRequest( REQUEST req )
{
  //REQ_BAR_ONとREQ_BAR_OFFは強制リクエスト
  //他のものはキューに入れてリクエスト処理
  switch( req ){
  case REQ_BAR_ON:
    request = req;
    alpha = 1.0f;
    fadeSpeed = 0.0f;
    request_num = 0;
    is_request  = false;
    break;
  case REQ_BAR_OFF:
    request = req;
    alpha = 0.0f;
    fadeSpeed = 0.0f;
    request_num = 0;
    is_request  = false;
    break;
  default:
    //１つ前のリクエストと同じならば発行しない
    if( (request_num == 0
        || (request_num > 0 && next_request[ request_num-1 ] != req) )
        &&  request_num < next_request_max )
    {
      next_request[ request_num++ ] = req;
      is_request  = true;
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   リクエスト取得
 *
 * @param   none
 *
 * @return  設定されているリクエスト
 */
//--------------------------------------------------------------------------------------------
ScrollBar::REQUEST ScrollBar::GetRequest( void ) const
{
  return request;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   描画判定
 *
 * @param   none
 *
 * @retval  true  リクエストが(REQ_BAR_ON,  REQ_BAR_IN)
 * @retval  false リクエストが(REQ_BAR_OFF, REQ_BAR_OUT)
 */
//--------------------------------------------------------------------------------------------
bool ScrollBar::IsVisible( void ) const
{
  return (REQ_BAR_IN==request) || (REQ_BAR_ON==request);
}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * @brief   自動フェード更新
 *
 * @param   pane_list 参照するペインリスト
 *
 * @li    pane_listUpdateの後に呼んでください
 *
 * @note  pane_listをによるscroll_barのフェード処理を
 *        本関数にまかせることができます。
 *        pane_listを使っている箇所は独自処理がないかぎり
 *        この関数を使用することが推奨です。
 *
 *
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::UpdateAutoFade( const app::tool::PaneList *pane_list )
{
  gfl2::ui::VectorDevice *key = pane_list->GetKey();
  gfl2::ui::Button *button = pane_list->GetButton();

  /*
   *  スクロールバー　フェード仕様
   *
   *  1.十字キー、LR、ドラッグなど、スクロールバーが動くときは
   *  　必ずスクロールバーがフェードインする
   *
   *  2.一度フェードインしたら、一定アルファまでは消えない。
   *    （フェードイン後即フェードアウトしてぱしって見えるのを防止するため）
   *
   */
  //縦方向に関してはフェードを全くしなくなった
  if( dir == app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL )
  {
    this->SetRequest( REQ_BAR_ON );
    return;
  }

  switch( pane_list->GetUpdateResult() )
  {
  default:
    GFL_ASSERT(0);
  case app::tool::PaneList::RET_NONE:						// 動作なし
    if( !key->IsHold(gfl2::ui::BUTTON_UP|gfl2::ui::BUTTON_DOWN)
     && !button->IsHold(gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R)
     )
    {
      this->SetRequest( REQ_BAR_OUT );
    }
    if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      this->SetRequest( REQ_BAR_OFF );
    }
    break;
	case app::tool::PaneList::RET_LIST_UP:						// 上へスクロール中
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_DOWN:					// 下へスクロール中
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_PAGE_UP:				// 上へページ送り
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_PAGE_DOWN:			// 下へページ送り
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_TOUCH:					// タッチ処理中
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_AUTO_SCRILL:		// オートスクロール中
    this->SetRequest( REQ_BAR_IN );
    break;
	case app::tool::PaneList::RET_LIST_SELECT:				// 項目選択
    this->SetRequest( REQ_BAR_OFF );
    break;
	case app::tool::PaneList::RET_LIST_CANCEL:				// キャンセル ( Bボタン )
    this->SetRequest( REQ_BAR_OFF );
    break;
	case app::tool::PaneList::RET_LIST_CATCH:					// 項目キャッチ
    this->SetRequest( REQ_BAR_OUT );
    break;
	case app::tool::PaneList::RET_LIST_PROCESSING:		// なんらかの処理を実行中 ( Aボタンアニメなど )
    this->SetRequest( REQ_BAR_OUT );
    break;
  }
}
#endif

//--------------------------------------------------------------------------------------------
/**
 * @brief 表示設定  （リクエストのON,OFFも効かない完全に表示・表示切り替えです）
 *
 * @param flag  true表示  false非表示
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::SetDrawEnable( bool flag )
{
  is_visible = flag;
  pScrollBar->SetVisible( flag );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief フレームを設定
 *
 * @param frame デフォルトはFRAMEMODE_DEFAULT
 */
//--------------------------------------------------------------------------------------------
void ScrollBar::SetFrameMode( GameSys::FrameMode frame )
{
  if( frame == GameSys::FRAMEMODE_60 )
  {
    alpha_speed = ALPHA_FADE_SPEED;
  }
  else
  {
    alpha_speed = ALPHA_FADE_SPEED * 0.5f;
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 大本のスクロールバーを取得
 *
 * @return スクロールバー
 */
//--------------------------------------------------------------------------------------------
app::tool::ScrollBar * ScrollBar::GetScrollBar( void ) const
{
  return pScrollBar;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
