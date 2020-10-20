//============================================================================================
/**
 * @file		app_tool_panelist.cpp
 * @brief		ペインリスト処理
 * @author	Hiroyuki Nakamura
 * @date		11.10.28
 */
//============================================================================================
#include <AppLib/include/Tool/app_tool_PaneList.h>

#include <AppLib/include/Util/app_util_Common.h>

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

static const u32 CHANGE_POS_PANE_NONE = 0xffffffff;		//!< 入れ替え挿入位置ペインなし

const u32 PaneList::ScrollFrame60[4] = { 8, 6, 3, 1 };		//!< キースクロールフレームテーブル ( 1/60 )
const u32 PaneList::ScrollFrame30[4] = { 4, 3, 1, 1 };		//!< キースクロールフレームテーブル ( 1/30 )

//! ボタンマネージャのアップデート結果
enum {
	BM_UPDATE_NONE = 0,				// 動作なし
	BM_UPDATE_HOLD,						// ボタンを抑えている
	BM_UPDATE_SELECT_ANIME,		// ボタン選択アニメ中
	BM_UPDATE_SELECT,					// ボタン選択
};


//--------------------------------------------------------------------------------------------
/**
  * @brief		リスト外からはみ出している時のスクロールとカーソル位置補正処理
  *
  * @param		pOutput       セットアップに設定したパラメータからlistPos/listScrollを補正して出力
  */
//--------------------------------------------------------------------------------------------
void PaneList::CommonAdjustmentListScrollAndPos( SETUP_PARAM* pOutput )
{
  if( pOutput == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  u32 scroll_max = 0;
  if( pOutput->valMax >= pOutput->listPosMax )
  {
    scroll_max = pOutput->valMax - pOutput->listPosMax;
  }

  if( pOutput->listScroll > scroll_max )
  {
    pOutput->listScroll = scroll_max;
  }
        
  u32 pos_max = 0;
  if( scroll_max != 0 )
  {
    pos_max = pOutput->listPosMax - 1;
  }
  else if( pOutput->valMax != 0 )
  {
    pos_max = pOutput->valMax - 1;
  }
  if( pOutput->listPos > pos_max )
  {
    pOutput->listPos = pos_max;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		setup				セットアップデータ
 * @param		frameMode		動作フレームレート
 */
//--------------------------------------------------------------------------------------------
PaneList::PaneList( const SETUP_PARAM * setup, bool is_effect, GameSys::FrameMode frameMode )
  : m_skipMode( PAGE_SKIP_LR_KEY )
  , m_loopFlag( true )
  , m_catchFlag( false )
  , m_keySelectFlag( true )
  , m_keyCancelFlag( false )
  , m_moveSE( SE_NONE )
  , m_selectSE( SE_NONE )
  , m_cancelSE( SE_NONE )
  , trgCount( 0 )
  , trgSpeed( 0 )
  , updateSeq( PANELIST_SEQ_INPUT )
  , pScrollValue( NULL )
  , chgFlag( false )
//  , m_chgOnPane( NULL )
//  , m_chgOffPane( NULL )
  , crossKeyEnable( true )
  , input_enabled( true )
  , input_touch_only( false )
  , cancelListPos( -1 )
  , useDrag( true )
  , useTouch( true )
  , m_isEffect( is_effect )
{
	param = *setup;
  m_frameMode = frameMode;

  if( m_frameMode == GameSys::FRAMEMODE_60 )
  {
    m_catchFrame = app::util::DEFAULT_GET_COUNT_60F;
  }
  else
  {
    m_catchFrame = app::util::DEFAULT_GET_COUNT_30F;
  }

	if( param.valMax <= param.listPosMax ){
		listScrollMax = 0.0f;
		param.listPosMax = param.valMax;
	}else{
		listScrollMax = static_cast<f32>( param.valMax - param.listPosMax );
	}

	tmpScroll = param.listScroll;

	tmpPaneListTable = GFL_NEW_ARRAY( param.heap ) s32[param.paneListMax];

	CreateButtonManager();

	InitListPut();
	MakeTouchArea();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
PaneList::~PaneList(void)
{
	SetVisibleCursor( false );
  // CB_CursorOff( param.listPos );  //->スクロール中に消すとアサートになるため。
	DeleteScrollValue();

	DeleteButtonManager();

	GFL_DELETE_ARRAY tmpPaneListTable;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetConfig
 * @brief   動作関連を設定
 * @date    2015.03.17
 *
 * @param   skip            リストのスキップモード
 * @param   is_loop         true = リストがループする
 * @param   is_catch        true = 項目をキャッチ可能
 * @param   is_key_select   true = Aボタンで決定可能
 * @param   is_key_cancel   true = Bボタンでキャンセル可能
 */
//-----------------------------------------------------------------------------
void PaneList::SetConfig( PageSkipMode skip, b8 is_loop, b8 is_catch, b8 is_key_select, b8 is_key_cancel )
{
  m_skipMode      = skip;
  m_loopFlag      = is_loop;
  m_catchFlag     = is_catch;
  m_keySelectFlag = is_key_select;
  m_keyCancelFlag = is_key_cancel;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSE
 * @brief   SEを設定
 * @date    2015.03.17
 *
 * @param   move    カーソル移動時のSE
 * @param   select  決定時のSE
 * @param   cancel  キャンセル時のSE
 */
//-----------------------------------------------------------------------------
void PaneList::SetSE( u32 move, u32 select, u32 cancel )
{
  m_moveSE   = move;
  m_selectSE = select;
  m_cancelSE = cancel;

  // 各ボタンに選択SEを設定
  for( u32 i=0; i<param.paneListMax; i++ ) {
    if( m_selectSE != SE_NONE ) {
      pButtonManager->SetButtonSelectSE( i, m_selectSE );
    }

    seTbl[i] = m_selectSE;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		十字キー有効/無効設定
 *
 * @param		flg		true = 有効
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetCrossKeyEnable( bool flg )
{
	crossKeyEnable = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンマネージャ作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CreateButtonManager(void)
{
	pButtonManager = GFL_NEW( param.heap ) app::tool::ButtonManager( param.heap, NULL, param.paneListMax );
	seTbl = GFL_NEW_ARRAY( param.heap ) u32[param.paneListMax];

	for( u32 i=0; i<param.paneListMax; i++ ){
		pButtonManager->AddButton(
			param.heap,
			i,
			param.pLytWk,
			param.paneList[i].base,
			param.paneList[i].bound,
			param.paneList[i].holdAnime,
			param.paneList[i].releaseAnime,
			param.paneList[i].cancelAnime,
			param.paneList[i].selectAnime,
			app::tool::ButtonManager::ANIMATION_NULL,
			app::tool::ButtonManager::ANIMATION_NULL,
			NULL );

		if( m_selectSE != SE_NONE ){
			pButtonManager->SetButtonSelectSE( i, m_selectSE );
		}
		seTbl[i] = m_selectSE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンマネージャ削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::DeleteButtonManager(void)
{
	GFL_DELETE_ARRAY seTbl;
	GFL_DELETE pButtonManager;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンマネージャアップデート
 *
 * @param		none
 *
 * @return	アップデート結果
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::UpdateButtonManager(void)
{
  if( input_enabled ){
    pButtonManager->UpdateInput( GetButton(), GetTouchPanel() );
  }

	switch( pButtonManager->Update() ){
	case app::tool::ButtonManager::UPDATE_STATE_IDLE:			// 何もしていない
		break;

	case app::tool::ButtonManager::UPDATE_STATE_HOLDING:	// ボタンを押さえている
		return BM_UPDATE_HOLD;

	case app::tool::ButtonManager::UPDATE_STATE_SELECT_ANIME_PLAYING:	// ボタン選択アニメ再生中
		return BM_UPDATE_SELECT_ANIME;

	case app::tool::ButtonManager::UPDATE_STATE_SELECTED:	// ボタンが選択された
		return BM_UPDATE_SELECT;

	case app::tool::ButtonManager::UPDATE_STATE_CANCEL:		// ボタンがキャンセルされた
		break;

	default:
		GFL_ASSERT( 0 );
	}

	return BM_UPDATE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ中のボタンを取得
 *
 * @param		none
 *
 * @return	ボタンID
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::BM_GetHoldButton(void)
{
	return pButtonManager->GetHoldingButtonId();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		選択されたボタンを取得
 *
 * @param		none
 *
 * @return	ボタンID
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::BM_GetSelectButton(void)
{
	return pButtonManager->GetSelectedButtonId();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ中のボタンをリセット
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CancelButtonHold(void)
{
	pButtonManager->ResetHoldState();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンのSEを設定
 *
 * @param		pos		カーソル位置
 * @param		se		SE番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetButtonSE( u32 pos, u32 se )
{
	u32	id = GetPosPaneIndex( pos );
	pButtonManager->SetButtonSelectSE( id, se );
	seTbl[id] = se;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンのSEを設定（ペインデータインデックス指定）
 *
 * @param   index ペインデータインデックス
 * @param   se    SE番号
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetButtonSEPaneIndex( u32 index, u32 se )
{
  pButtonManager->SetButtonSelectSE( index, se );
  seTbl[index] = se;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル配置
 *
 * @param		pos		配置位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::PutCursor( u32 pos )
{
	if( param.menuCursor == NULL ){
		return;
	}
  param.menuCursor->Put( param.pLytWk, param.paneList[ GetPosPaneIndex(pos) ].cursor_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		現在のカーソル位置にカーソルを配置
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::PutCursor(void)
{
	PutCursor( param.listPos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル表示切り替え
 *
 * @param		flg		true = 表示、false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetVisibleCursor( bool flg )
{
	if( param.menuCursor == NULL ){
		return;
	}
	param.menuCursor->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		入力許可フラグ設定
 *
 * @param		is_enable		true = 許可、false = 不許可
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetInputEnabled( bool is_enable )
{
  input_enabled = is_enable;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   入力許可フラグ取得
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//--------------------------------------------------------------------------------------------
bool PaneList::IsInputEnabled(void)
{
  return input_enabled;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ制御オンリーフラグ設定
 *
 * @param		is_touch_only		true = タッチのみ、false = タッチとボタン
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetIsTouchOnly( bool is_touch_only )
{
  input_touch_only = is_touch_only;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルデータ取得
 *
 * @param		pos			カーソル位置格納場所
 * @param		scroll	スクロール値格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::GetCursorData( u32 * pos, f32 * scroll ) const
{
	if( pos != NULL ){
		*pos = param.listPos;
	}
	if( scroll != NULL ){
/*
		if( pScrollValue != NULL ){
			*scroll = pScrollValue->GetCurrentValue();
		}else{
			*scroll = param.listScroll;
		}
*/
		*scroll = tmpScroll;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルデータ設定
 *
 * @param		pos			カーソル位置格納場所
 * @param		scroll	スクロール値格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetCursorData( u32 pos, f32 scroll )
{
	param.listPos = pos;
  param.listScroll = scroll;
	tmpScroll = scroll;
	PutCursor();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目位置取得 ( カーソル位置＋スクロール値 )
 *
 * @param		none
 *
 * @return	項目位置
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetItemPos(void) const
{
	u32	pos;
	f32	scroll;
	GetCursorData( &pos, &scroll );
	return ( pos + static_cast<u32>(scroll) );
//	return ( param.listPos + static_cast<u32>( param.listScroll ) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート処理
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::Update(void)
{
	Result ret = RET_NONE;

#if PM_DEBUG
//	DebugPrintPanePos();
#endif // PM_DEBUG

	switch( updateSeq ){
	case PANELIST_SEQ_INPUT:
		ret = UpdateInput();
		tmpScroll = param.listScroll;
		break;

	case PANELIST_SEQ_KEY_SCROLL:
		ret = UpdateKeyScroll();
		tmpScroll = param.listScroll;
		break;

	case PANELIST_SEQ_DRAG:
		ret = UpdateDrag();
//		tmpScroll = GetUpdateDragScroll();
		tmpScroll = param.listScroll;
		break;

	case PANELIST_SEQ_DRAG_SCROLL:
		ret = UpdateDragScroll();
		tmpScroll = GetUpdateDragScroll();
		break;

	case PANELIST_SEQ_AUTO_SCROLL:
		ret = UpdateAutoScroll();
		if( pScrollValue != NULL ){
			tmpScroll = pScrollValue->GetCurrentValue();
		}else{
			tmpScroll = param.listScroll;
		}
		break;

	case PANELIST_SEQ_KEY_SELECT_ANIME:
		ret = UpdateKeySelectAnime();
		tmpScroll = param.listScroll;
		break;
	}
  result  = ret;

  if( updateSeq != PANELIST_SEQ_KEY_SCROLL )
  {
    SetKeyEnable(false);
  }
  SetButtonEnable(false);

  if( result == RET_NONE )
  {
    SetTpReleaseEnable(false);
    SetTpEnable(false);
  }
  
	return GetUpdateResult();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		外部スクロール処理初期化
 *
 * @param		mv	スクロール方向
 *
 * @retval	"true = スクロール可"
 * @retval	"false = スクロール不可"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::InitExScroll( int mv )
{
	if( mv == -1 ){
		if( param.listScroll == 0.0f ){
			return false;
		}
	}else if( mv == 1 ){
		if( param.listScroll >= listScrollMax ){
			return false;
		}
	}
	InitKeyScroll( mv );
	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		外部スクロール処理
 *
 * @param		none
 *
 * @retval	"true = スクロールした"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::MainExScroll(void)
{
	if( scrollVec == -1 ){
		if( param.listScroll == 0.0f ){
			return false;
		}
	}else if( scrollVec == 1 ){
		if( param.listScroll >= listScrollMax ){
			return false;
		}
	}

	if( scrollCount == static_cast<s8>(GetScrollFrame(trgSpeed)) ){
		ReviseInitScrollList();
		param.listScroll += ( 1.0f * scrollVec );
		tmpScroll = param.listScroll;
		CB_CursorMove( param.listPos );

		if( scrollVec == -1 ){
			if( param.listScroll == 0.0f ){
				return false;
			}
		}else if( scrollVec == 1 ){
			if( param.listScroll >= listScrollMax ){
				return false;
			}
		}

		if( trgCount == SCROLL_FRAME_CHG_COUNT ){
			trgCount = 0;
			if( trgSpeed < (GFL_NELEMS(ScrollFrame60)-1) ){
				trgSpeed++;
			}
		}else{
			trgCount++;
		}
		basePane = GetDefaultPositionPane();
		baseListPos = param.listScroll;
		scrollCount = 0;
		return false;
	}

	if( scrollCount == 0 ){
		CB_CursorOff( param.listPos );
	}
	ScrollListPutKey( scrollVec );
	scrollCount++;

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール方向取得
 *
 * @param		none
 *
 * @return	スクロール方向
 */
//--------------------------------------------------------------------------------------------
int PaneList::GetExScrollVec(void)
{
	return scrollVec;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：カーソル移動
 *
 * @param		pos		カーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CB_CursorMove( u32 pos )
{
	u32	pane;

	if( param.valMax == 0 ){
		pane = CALLBACK_PANE_NONE;
	}else{
		pane = GetPosPaneIndex( pos );
	}

	param.cbFunc->CallBack_CursorMove(
		param.cbWork, param.paneList, pane, param.listPos+static_cast<u32>(param.listScroll) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：カーソル非表示
 *
 * @param		pos		カーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CB_CursorOff( u32 pos )
{
	u32	pane;

	if( param.valMax == 0 ){
		pane = CALLBACK_PANE_NONE;
	}else{
		pane = GetPosPaneIndex( pos );
	}

	param.cbFunc->CallBack_CursorOff(
		param.cbWork, param.paneList, pane, param.listPos+static_cast<u32>(param.listScroll) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コールバック：入れ替え
 *
 * @param		pos1	入れ替え位置１
 * @param		pos2	入れ替え位置２
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CB_ItemChange( u32 pos1, u32 pos2 )
{
	if( chgFlag == true ){
		param.cbFunc->CallBack_ItemChange( param.cbWork, pos1, pos2 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		十字キー取得
 *
 * @param		none
 *
 * @return	十字キー
 */
//--------------------------------------------------------------------------------------------
gfl2::ui::VectorDevice * PaneList::GetKey(void) const
{
  if(!m_bKey){
    return NULL;
  }
  if( input_enabled && !input_touch_only){
    return param.uiDevMan->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  }else{
    return NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタン取得
 *
 * @param		none
 *
 * @return	ボタン
 */
//--------------------------------------------------------------------------------------------
gfl2::ui::Button * PaneList::GetButton(void) const
{
  if(!m_bButton){
    return NULL;
  }
  if( input_enabled && !input_touch_only )
  {
	  return param.uiDevMan->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  }
  else
  {
    return NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチパネル取得
 *
 * @param		none
 *
 * @return	タッチパネル
 */
//--------------------------------------------------------------------------------------------
gfl2::ui::TouchPanel * PaneList::GetTouchPanel(void)
{
  if(m_bTpRelease){
    m_bTpRelease = false;
    if( input_enabled ) {
      return param.uiDevMan->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
    }
  }
  if(!m_bTp){
    return NULL;
  }
  if( ! useTouch ) {
    return NULL;
  }
  if( input_enabled ) {
	  return param.uiDevMan->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  }else{
    return NULL;
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペイン座標取得
 *
 * @param		pane  ペイン
 *
 * @return	座標
 */
//--------------------------------------------------------------------------------------------
gfl2::math::VEC3 PaneList::GetPanePos( gfl2::lyt::LytPane * pane ) const
{
  gfl2::lyt::LytPane * base = pane;
  gfl2::lyt::LytPane * root = param.pLytWk->GetLayoutCore()->GetRootPane();
  gfl2::math::VEC3 pos( 0.0f, 0.0f, 0.0f );

  // 自身がルートペインじゃない
  if( base != root )
  {
    pos = base->GetTranslate();
    for ever{
      gfl2::lyt::LytPane * parent = base->GetParent();
      // 親がいない or 親がルートペイン
      if( parent == NULL || parent == root )
      {
        break;
      }
      pos += parent->GetTranslate();
      base = parent;
    }
  }

  return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペイン座標設定
 *
 * @param		pane  ペイン
 * @param		pos   ペインの座標
 *
 * @return	nonw
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetPanePos( gfl2::lyt::LytPane * pane, gfl2::math::VEC3 & pos )
{
  gfl2::lyt::LytPane* parentPane = pane->GetParent();
  if(parentPane)
  {
    gfl2::math::VEC3 parentPos = this->GetPanePos(parentPane);
    pane->SetTranslate( pos - parentPos ); 
  }
  else
  {
    pane->SetTranslate( pos );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペイン表示切り替え
 *
 * @param		pane  ペイン
 * @param		flg   true = 表示, false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetPaneVisible( gfl2::lyt::LytPane * pane, bool flg )
{
  pane->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペイン表示状態取得
 *
 * @param		pane  ペイン
 *
 * @retval	"true = 表示"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::GetPaneVisible( gfl2::lyt::LytPane * pane )
{
  return pane->IsVisible();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ範囲エリア作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::MakeTouchArea(void)
{
	gfl2::math::VEC3	pos = GetPanePos( param.paneList[0].base );
	touchAreaLX = pos.x - param.defSX/2.0f;
	touchAreaRX = pos.x + param.defSX/2.0f;
	touchAreaUY = param.defPY + param.defSY/2.0f - 1.0f;
	touchAreaDY = touchAreaUY - param.defSY * static_cast<f32>(param.listPosMax) + 1.0f;
/*
	BTS[4746]: ( touchAreaDY )
		ボタン同士の境界線上をタッチするとプライオリティの高いボタンを拾ってくるが、
		ボタンがループ移動すると下のボタンのプライオリティが高くなり、範囲外のボタンをタッチできてしまう。
		そのため、タッチ有効範囲を上下１ドット小さくする
*/

//	GFL_PRINT( "area: (u, d, l, r) = (%f, %f, %f, %f)\n", touchAreaUY, touchAreaDY, touchAreaLX, touchAreaRX );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ範囲エリア作成
 *
 * @param		lx	左X座標
 * @param		rx	右X座標
 * @param		uy	上Y座標
 * @param		dy	下Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::MakeTouchArea( f32 lx, f32 rx, f32 uy, f32 dy )
{
	touchAreaLX = lx;
	touchAreaRX = rx;
	touchAreaUY = uy-1.0f;
	touchAreaDY = dy+1.0f;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストエリアチェック
 *
 * @param		x		X座標
 * @param		y		Y座標
 *
 * @retval	"true = 範囲内"
 * @retval	"false = 範囲外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckListArea( f32 x, f32 y )
{
	if( x >= touchAreaLX && x < touchAreaRX &&
			y > touchAreaDY && y <= touchAreaUY ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ範囲エリアチェック
 *
 * @param		x		X座標
 * @param		y		Y座標
 *
 * @retval	"true = 範囲内"
 * @retval	"false = 範囲外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckTouchArea( s32 x, s32 y )
{
	f32	tpx = static_cast<f32>( x-160 );
	f32	tpy = static_cast<f32>( 120-y );
	return CheckListArea( tpx, tpy );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ範囲エリアチェック
 *
 * @param		none
 *
 * @retval	"true = 範囲内"
 * @retval	"false = 範囲外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckTouchArea(void)
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();

  if( tp ){
    u16	x, y;
    tp->GetXY( &x, &y );
    
    return CheckTouchArea( x, y );
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ座標セット
 *
 * @param		init	初期化時 = true
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetTouchPos( bool init )
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();
	u16	x, y;

	if( tp ){
		if( tp->IsTouch() == true ){
			tp->GetXY( &x, &y );
		}else{
			x = nowTPX;
			y = nowTPY;
		}

    if( init == true ){
      oldTPX = x;
      oldTPY = y;
      nowTPX = x;
      nowTPY = y;
    }else{
      oldTPX = nowTPX;
      oldTPY = nowTPY;
      nowTPX = x;
      nowTPY = y;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ドラッグスクロール開始チェック
 *
 * @param		none
 *
 * @retval	"true = 開始"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckDragScroll(void)
{
	if( ! useDrag ){
		return false;
	}
	if( gfl2::math::FAbs(startTPY-nowTPY) >= 3.0f ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ScrollValue作成
 *
 * @param		level		速度レベル
 * @param		mv			移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CreateScrollValue( u32 level, s32 mv )
{
	app::tool::ScrollValue::Settings	dat;

	dat.init_value = param.listScroll;	// 初期値[value]
	dat.min_value  = 0.0f;							// 最小値[value]
	dat.max_value  = listScrollMax;			// 最大値[value]

	static const f32 decel[] = {
		// 最大速度	0.01 mv = 50
		//					0.02 mv = 25
		//					0.03 mv = 16
		//					0.04 mv = 12
		//					0.05 mv = 10
		//					0.06 mv = 8
		//					0.07 mv = 7
		//					0.08 mv = 6
		0.04f, 0.03f, 0.02f, 0.01f,
	};
	// 減速度[value/frame*frame]
	if( level >= GFL_NELEMS(decel) ){
		dat.default_decel = decel[GFL_NELEMS(decel)-1];
	}else{
		dat.default_decel = decel[level];
	}

	dat.overrun_enable  = false;					  // 限界値突破を許すかどうか
	dat.overrun_frame = 0;							    // 限界値突破時の強制スクロールのフレーム数

	pScrollValue = GFL_NEW( param.heap ) app::tool::ScrollValue( dat );

	// 初速度
	static const f32 speed_tbl[] = {
		0.5f, 0.6f, 0.7f, 0.8f
	};
	f32	speed;
	if( level >= GFL_NELEMS(speed_tbl) ){
		speed = speed_tbl[GFL_NELEMS(speed_tbl)-1];
	}else{
		speed = speed_tbl[level];
	}

	if( mv < 0 ){
		pScrollValue->StartScroll( -speed, 1.0f );
	}else{
		pScrollValue->StartScroll( speed, 1.0f );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ScrollValue削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::DeleteScrollValue(void)
{
	if( pScrollValue != NULL ){
		GFL_DELETE	pScrollValue;
		pScrollValue = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デフォルト位置(カーソル0の位置)のペインのインデックスを取得
 *
 * @param		none
 *
 * @return	ペインのインデックス
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetDefaultPositionPane(void)
{
	return GetPosPaneIndex( 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト位置のペインを取得
 *
 * @param		pos		リスト位置
 *
 * @return	ペインのインデックス
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetPosPaneIndex( int pos )
{
	f32	y = param.defPY - param.defSY * static_cast<f32>( pos );

	for( u32 i=0; i<param.paneListMax; i++ ){
		gfl2::math::VEC3	p1 = GetPanePos( param.paneList[i].base );
//    GFL_PRINT("%d: y=%f \n",i, p1.y);
		if( p1.y == y ){
			return i;
		}
	}

//  GFL_PRINT("pos=%f \n", y);
	GFL_ASSERT( 0 );
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   リスト位置のペインを取得
 *
 * @param   pos   リスト位置
 *
 * @retval  -1  = なし
 * @retval  -1 != ペインのインデックス
 */
//--------------------------------------------------------------------------------------------
int PaneList::GetPosPaneIndexEz( int pos )
{
  f32 y = param.defPY - param.defSY * static_cast<f32>( pos );

  for( u32 i=0; i<param.paneListMax; i++ )
  {
    gfl2::math::VEC3 p1 = GetPanePos( param.paneList[i].base );
    if( p1.y == y )
    {
      return static_cast<int>( i );
    }
  }
  return -1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート処理結果を取得
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::GetUpdateResult( void ) const
{
  return result;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンマネージャのアップデート結果を取得
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
app::tool::ButtonManager::UpdateState PaneList::GetButtonUpdateState( void ) const
{
  return pButtonManager->GetUpdateState();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペインからリスト位置を取得
 *
 * @param		index		ペインのインデックス
 *
 * @return	リスト位置
 */
//--------------------------------------------------------------------------------------------
s32 PaneList::GetPaneListPos( u32 index )
{
	gfl2::math::VEC3	pos = GetPanePos( param.paneList[index].base );
	return static_cast<u32>( (param.defPY-pos.y)/param.defSY );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ中のペインを取得
 *
 * @param		none
 *
 * @return	ペインのインデックス
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetTouchPane(void)
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();

	if( tp && tp->IsTouch() == true ){
		u16	tpx, tpy;
		tp->GetXY( &tpx, &tpy );

		f32	x = static_cast<f32>( tpx ) - 160.0f;
		f32	y = 120.0f - static_cast<f32>( tpy );

		f32	half_sx = param.defSX / 2.0f;
		f32	half_sy = param.defSY / 2.0f;

		for( u32 i=0; i<param.paneListMax; i++ ){
			if( GetPaneVisible( param.paneList[i].base ) == false ){
				continue;
			}

			gfl2::math::VEC3	pos = GetPanePos( param.paneList[i].bound );
			if( CheckListArea( pos.x, pos.y ) == false ){
				continue;
			}

			if( x >= (pos.x-half_sx) && x < (pos.x+half_sx) &&
					y >= (pos.y-half_sy) && y < (pos.y+half_sy) ){
				return i;
			}
		}
	}

	return TOUCH_PANE_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定ペインをタッチしているか
 *
 * @param		pane  ペイン
 *
 * @retval	"true = タッチしている"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckTouchPane( gfl2::lyt::LytPane * pane )
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();

	if( tp && tp->IsTouch() == true ){
		u16	tpx, tpy;
		tp->GetXY( &tpx, &tpy );

		f32	x = static_cast<f32>( tpx ) - 160.0f;
		f32	y = 120.0f - static_cast<f32>( tpy );

		f32	half_sx = param.defSX / 2.0f;
		f32	half_sy = param.defSY / 2.0f;

		gfl2::math::VEC3	pos = GetPanePos( pane );

		if( x >= (pos.x-half_sx) && x < (pos.x+half_sx) &&
				y >= (pos.y-half_sy) && y < (pos.y+half_sy) ){
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ座標のカーソル位置を取得
 *
 * @param		y		Y座標
 *
 * @return	カーソル位置
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetTouchListPos( f32 y )
{
	f32	uy = param.defPY + param.defSY / 2.0f - 1.0f;
	f32	dy = uy - param.defSY;
	for( u32 i=0; i<param.listPosMax; i++ ){
		if( y <= uy && y > dy ){
			return i;
		}
		uy = dy;
		dy -= param.defSY;
	}
/*
  @fix niji GFNMCat[801]: 
        部品ペインの場合、タッチ範囲とペインの座標の整合性がとれない場合があるため、
        取得できない場合の値を返し、その場合は処理を無効化する
*/
//	GFL_ASSERT( 0 );
//	return 0;
  return param.listPosMax;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト初期配置
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::InitListPut(void)
{
	gfl2::math::VEC3	pos = GetPanePos( param.paneList[0].base );
	s32	lp;

	lp = static_cast<u32>( param.listScroll ) + param.writeOffsetY;
	pos.y = param.defPY - param.defSY * param.writeOffsetY;

	for( u32 i=0; i<param.paneListMax; i++ ){
		SetPanePos( param.paneList[i].base, pos );
		if( lp >= 0 && lp < static_cast<s32>(param.valMax) ){
			SetPaneVisible( param.paneList[i].base, true );
			param.cbFunc->CallBack_ListWrite( param.cbWork, param.paneList, i, lp );
		}else{
			SetPaneVisible( param.paneList[i].base, false );
		}
		pos.y -= param.defSY;
		lp++;
	}

  ChangePutListPriority();
  SetEffect();

	CB_CursorMove( param.listPos );

	if( param.valMax == 0 ){
		SetVisibleCursor( false );
	}else{
		PutCursor( param.listPos );
		SetVisibleCursor( true );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストスクロール
 *
 * @param		mv_y		移動値
 * @param		mv			移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::ScrollListPut( f32 mv_y, int mv )
{
	s32 * tbl = tmpPaneListTable;
	f32	ry;
	bool	se = false;

	MakePabeListRowTable( tbl, basePane, &ry );

	f32	def_y   = param.defPY - param.defSY * param.writeOffsetY;		// 画面に表示されるペインに最上部Y座標
	f32	half_sy = param.defSY / 2.0f;																// ペインYサイズの1/2
	f32	uy = def_y + half_sy;																				// 最上部の画面外座標
#if 0   // @fix writeOffsetYの値でuy-dy間の差分が変わってしまう(sangoはスクロールするリストではwriteOffsetY=-1になっていたので表面化しなかった)
	f32	dy = def_y - param.defSY * ( param.paneListMax + param.writeOffsetY ) - half_sy;	// 最下部の画面外座標
#else
	f32	dy = uy - param.defSY * param.paneListMax;	// 最下部の画面外座標
#endif

	gfl2::math::VEC3	drag_pos = GetPanePos( param.paneList[basePane].base );
	drag_pos.y += mv_y;
	SetPanePos( param.paneList[basePane].base, drag_pos );

	for( u32 i=0; i<param.paneListMax; i++ ){
		if( tbl[i] != static_cast<s32>(basePane) ){
			gfl2::math::VEC3	pos = GetPanePos( param.paneList[ tbl[i] ].base );
			pos.y = drag_pos.y + ry;
			if( mv < 0 ){
				if( pos.y <= dy ){
					pos.y += ( param.defSY * static_cast<f32>(param.paneListMax) );
					f32	sy = ReviseFloat( drag_pos.y - pos.y );
					s32	list_pos = static_cast<s32>( baseListPos + sy / param.defSY );
					if( list_pos < 0 ){
						SetPaneVisible( param.paneList[ tbl[i] ].base, false );
					}else{
						SetPaneVisible( param.paneList[ tbl[i] ].base, true );
						param.cbFunc->CallBack_ListWrite( param.cbWork, param.paneList, tbl[i], list_pos );
//						GFL_PRINT( "ds = %f, dpy = %f, py = %f, list_pos = %d(%f), sy = %f\n", baseListPos, drag_pos.y, pos.y, list_pos, baseListPos + sy / param.defSY, sy );
					}
					se = true;
				}
			}else{
				if( pos.y > uy ){
					pos.y -= ( param.defSY * static_cast<f32>(param.paneListMax) );
					f32	sy = ReviseFloat( drag_pos.y - pos.y );
					s32	list_pos = static_cast<s32>( baseListPos + sy / param.defSY );
					if( list_pos >= static_cast<s32>(param.valMax) ){
						SetPaneVisible( param.paneList[ tbl[i] ].base, false );
					}else{
						SetPaneVisible( param.paneList[ tbl[i] ].base, true );
						param.cbFunc->CallBack_ListWrite( param.cbWork, param.paneList, tbl[i], list_pos );
//						GFL_PRINT( "ds = %f, dpy = %f, py = %f, list_pos = %d(%f), sy = %f\n", baseListPos, drag_pos.y, pos.y, list_pos, baseListPos + sy / param.defSY, sy );
					}
					se = true;
				}
			}
			SetPanePos( param.paneList[ tbl[i] ].base, pos );
		}
		ry -= param.defSY;
	}

  // プライオリティを変更
  ChangePutListPriority();
  SetEffect();

	if( se == true ){
		PlayCursorMoveSE();
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キーでのリストスクロール
 *
 * @param		mv			移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::ScrollListPutKey( int mv )
{
	ScrollListPut( param.defSY / GetScrollFrame(trgSpeed) * mv, mv );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		浮動小数点を補正
 *
 * @param		p		補正する値
 *
 * @return	補正した値
 *
 * @li	0.9999以上を繰り上げ、0.0001未満を破棄
 */
//--------------------------------------------------------------------------------------------
f32 PaneList::ReviseFloat( f32 p )
{
	f32	pa = static_cast<f32>( static_cast<s32>(p) );
	f32	pb = gfl2::math::FAbs( p - pa );
	if( pb >= 0.9999f ){
#if 1   // kujira BTS[7057] ペインリストの小数点計算により本来+1の位置を返すところで-1の位置を返している不具合の修正
		if( pa >= 0.0f ){
#else
		if( pa > 0.0f ){
#endif
			return ( pa + 1.0f );
		}else{
			return ( pa - 1.0f );
		}
	}else if( pb < 0.0001f ){
		return pa;
	}
	return p;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール後のリスト位置を補正
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::ReviseInitScrollList(void)
{
	s32 * tbl = tmpPaneListTable;
	u32 def_pane;
	f32	def_y;

	def_pane = MakePabeListRowTable( tbl, DEFAULT_PANE_NONE, &def_y );

	for( u32 i=0; i<param.paneListMax; i++ ){
		gfl2::math::VEC3	pos = GetPanePos( param.paneList[ tbl[i] ].base );
		if( tbl[i] == static_cast<s32>(def_pane) ){
			pos.y = param.defPY;
		}else{
			pos.y = param.defPY + def_y;
		}
		SetPanePos( param.paneList[ tbl[i] ].base, pos );
		def_y -= param.defSY;
	}

  SetEffect();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストペインのY座標順のテーブル作成
 *
 * @param		tbl					テーブル作成場所
 * @param		def_index		基準となるペインのインデックス
 * @param		def_y				配置開始Y座標作成場所
 *
 * @return	基準となるペインのインデックス
 *
 * @li	"def_index = DEFAULT_PANE_NONE"の場合、カーソル0の位置を基準とする
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::MakePabeListRowTable( s32 * tbl, u32 def_index, f32 * def_y )
{
	// テーブル初期化
	for( u32 i=0; i<param.paneListMax; i++ ){
		tbl[i] = i;
	}
	// Y座標の大きい順に並び替え
	for( u32 i=0; i<param.paneListMax-1; i++ ){
		gfl2::math::VEC3	p1 = GetPanePos( param.paneList[ tbl[i] ].base );
		for( u32 j=i+1; j<param.paneListMax; j++ ){
			gfl2::math::VEC3	p2 = GetPanePos( param.paneList[ tbl[j] ].base );
			if( p1.y < p2.y ){
				s32	tmp = tbl[i];
				tbl[i] = tbl[j];
				tbl[j] = tmp;
				p1 = p2;
			}
		}
	}
	// 基準位置設定
	// 基準ペインなしの場合は、カーソル０の位置を基準とする
	if( def_index == DEFAULT_PANE_NONE ){
		gfl2::math::VEC3	p1 = GetPanePos( param.paneList[0].base );
		def_index = 0;
		for( u32 i=1; i<param.paneListMax; i++ ){
			gfl2::math::VEC3	p2 = GetPanePos( param.paneList[i].base );
			if( gfl2::math::FAbs(p1.y-param.defPY) > gfl2::math::FAbs(p2.y-param.defPY) ){
				def_index = i;
				p1 = p2;
			}
		}
	}
	*def_y = 0.0f;
	for( u32 i=0; i<param.paneListMax; i++ ){
		if( tbl[i] == static_cast<s32>(def_index) ){
			break;
		}
		*def_y += param.defSY;
	}

	return def_index;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キースクロール初期設定
 *
 * @param		mv		移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::InitKeyScroll( int mv )
{
	if( chgFlag == true ){
		InitKeyScrollChange( mv );
	}else{
		scrollVec = mv;
		scrollCount = 0;
		trgCount = 0;
		trgSpeed = 0;

		basePane = GetDefaultPositionPane();
		baseListPos = param.listScroll;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル移動
 *
 * @param		val		移動値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::CursorMove( int val )
{
  u32	tmp = param.listPos;

  // 通常
  if( chgFlag == false )
  {
    CB_CursorOff( param.listPos );
    param.listPos += val;
    CB_CursorMove( param.listPos );
  }
  // 入れ替え
  else
  {
    u32 pane1_index = GetPosPaneIndex( param.listPos );
    u32 pane2_index = GetPosPaneIndex( param.listPos+val );
    gfl2::math::VEC3 pos1 = GetPanePos( param.paneList[pane1_index].base );
    gfl2::math::VEC3 pos2 = GetPanePos( param.paneList[pane2_index].base );
    SetPanePos( param.paneList[pane1_index].base, pos2 );
    SetPanePos( param.paneList[pane2_index].base, pos1 );
    CB_ItemChange(
      static_cast<u32>(param.listScroll)+param.listPos,
      static_cast<u32>(param.listScroll)+param.listPos+val );
    param.listPos += val;
    SetEffect();
  }

  if( tmp != param.listPos )
  {
    PlayCursorMoveSE();
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ページ送り：上
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::PageUp(void)
{
	if( param.listScroll != 0 ){
		CB_CursorOff( param.listPos );
		f32	tmp = param.listScroll;
		if( param.listScroll < param.listPosMax ){
			param.listScroll = 0;
		}else{
			param.listScroll -= param.listPosMax;
		}
		CB_ItemChange(
			static_cast<u32>(tmp)+param.listPos,
			static_cast<u32>(param.listScroll)+param.listPos );
		InitListPut();
    ChangeCursorPosItemPriority();
		PlayCursorMoveSE();
		return RET_LIST_PAGE_UP;
	}else if( param.listPos != 0 ){
		CB_CursorOff( param.listPos );
		CB_ItemChange(
			static_cast<u32>(param.listScroll)+param.listPos, static_cast<u32>(param.listScroll) );
		param.listPos = 0;
		InitListPut();
    ChangeCursorPosItemPriority();
		PlayCursorMoveSE();
		return RET_LIST_PAGE_UP;
	}
	return RET_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ページ送り：下
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::PageDown(void)
{
	if( param.listScroll != listScrollMax ){
		CB_CursorOff( param.listPos );
		f32	tmp = param.listScroll;
		if( (param.listScroll+param.listPosMax) > listScrollMax ){
			param.listScroll = listScrollMax;
		}else{
			param.listScroll += param.listPosMax;
		}
		CB_ItemChange(
			static_cast<u32>(tmp)+param.listPos,
			static_cast<u32>(param.listScroll)+param.listPos );
		InitListPut();
    ChangeCursorPosItemPriority();
		PlayCursorMoveSE();
		return RET_LIST_PAGE_DOWN;
	}else if( param.listPos != (param.listPosMax-1) ){
		CB_CursorOff( param.listPos );
		u32	tmp = param.listPos;
		param.listPos = param.listPosMax - 1;
		CB_ItemChange(
			static_cast<u32>(param.listScroll)+tmp, static_cast<u32>(param.listScroll)+param.listPos );
		InitListPut();
    ChangeCursorPosItemPriority();
		PlayCursorMoveSE();
		return RET_LIST_PAGE_DOWN;
	}
	return RET_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ドラッグ処理継続チェック
 *
 * @param		none
 *
 * @retval	"true = ドラッグ中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckDragContinue(void)
{
/*
	if( CheckCatchItem() == true ){
		return false;
	}
*/

	gfl2::ui::TouchPanel * tp = GetTouchPanel();

	if( tp && tp->IsTouch() == true ){
		return CheckTouchArea();
	}

	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キャッチムーブ可能か
 *
 * @param		none
 *
 * @retval	"true = 可"
 * @retval	"false = 不可"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckCatchItem(void)
{
	if( GFL_BOOL_CAST(m_catchFlag) == true && m_catchCount == m_catchFrame ){
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		基準ペインのリスト位置取得
 *
 * @param		none
 *
 * @return	リスト位置
 */
//--------------------------------------------------------------------------------------------
f32 PaneList::GetBaseListPos(void)
{
	gfl2::math::VEC3	pos = GetPanePos( param.paneList[basePane].base );
	return ReviseFloat( param.listScroll-(pos.y-param.defPY)/param.defSY );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ドラッグ処理中のスクロール値
 *
 * @param		none
 *
 * @return	スクロール値
 */
//--------------------------------------------------------------------------------------------
f32 PaneList::GetUpdateDragScroll(void)
{
	// オートスクロールに遷移したとき
	if( updateSeq == PANELIST_SEQ_AUTO_SCROLL ){
		return pScrollValue->GetCurrentValue();
	// 入力待ちに遷移したとき
	}else if( updateSeq == PANELIST_SEQ_INPUT ){
		return param.listScroll;
	}
	// 上記以外
	gfl2::math::VEC3 pos = GetPanePos( param.paneList[basePane].base );
	return ( param.listScroll + ( static_cast<f32>(param.listPos) - (param.defPY-pos.y) / param.defSY ) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		入れ替えフラグ設定
 *
 * @param		flg		true = 入れ替え状態、false = 通常選択
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetChangeFlag( bool flg )
{
	chgFlag = flg;
	if( flg == false ){
		CB_CursorOff( param.listPos );
		InitListPut();
	}else{
		CB_CursorOff( param.listPos );
		CB_CursorMove( param.listPos );
  	SetItemChangePanePriority( GetPosPaneIndex(param.listPos), true );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ドラッグ使用フラグ設定
 *
 * @param		flg		true = ドラッグする、false = ドラッグしない
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetUseDrag( bool flg )
{
	useDrag = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ使用フラグ設定
 *
 * @param		flg		true = タッチ可能、false = タッチ不可能
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetUseTouch( bool flg )
{
	useTouch = flg;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キー操作での入れ替えスクロール処理初期化
 *
 * @param		mv	スクロール方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::InitKeyScrollChange( int mv )
{
	scrollVec = mv;
	scrollCount = 0;
	trgCount = 0;
	trgSpeed = 0;

	baseListPos = param.listScroll;

	basePane = GetPosPaneIndex( param.listPos );
	chgPane  = GetPosPaneIndex( param.listPos+mv );

	SetItemChangePanePriority( basePane, true );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストスクロール（入れ替え用）
 *
 * @param		mv_y		移動値
 * @param		mv			移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::ScrollListPutChange( f32 mv_y, int mv )
{
	f32	def_y   = param.defPY - param.defSY * param.writeOffsetY;
	f32	half_sy = param.defSY / 2.0f;
	f32	uy = def_y + half_sy;
#if 0   // @fix writeOffsetYの値でuy-dy間の差分が変わってしまう(sangoはスクロールするリストではwriteOffsetY=-1になっていたので表面化しなかった)
	f32	dy = def_y - param.defSY * ( param.paneListMax + param.writeOffsetY ) - half_sy;
#else
	f32	dy = uy - param.defSY * param.paneListMax;
#endif

	bool se = false;

	for( u32 i=0; i<param.paneListMax; i++ ){
		if( i == basePane ){
			continue;
		}
		gfl2::math::VEC3	pos = GetPanePos( param.paneList[i].base );
		pos.y += mv_y;

		if( mv < 0 ){
			if( pos.y <= dy ){
				pos.y += ( param.defSY * static_cast<f32>(param.paneListMax) );
				s32	list_pos = baseListPos - 1 + param.writeOffsetY;
				if( list_pos < 0 ){
					SetPaneVisible( param.paneList[i].base, false );
				}else{
					SetPaneVisible( param.paneList[i].base, true );
					param.cbFunc->CallBack_ListWrite( param.cbWork, param.paneList, i, list_pos );
//					GFL_PRINT( "ds = %f, dpy = %f, py = %f, list_pos = %d(%f), sy = %f\n", baseListPos, drag_pos.y, pos.y, list_pos, baseListPos + sy / param.defSY, sy );
				}
				se = true;
			}
		}else{
			if( pos.y > uy ){
				pos.y -= ( param.defSY * static_cast<f32>(param.paneListMax) );
				s32	list_pos = baseListPos + ( param.paneListMax + param.writeOffsetY );
				if( list_pos >= static_cast<s32>(param.valMax) ){
					SetPaneVisible( param.paneList[i].base, false );
				}else{
					SetPaneVisible( param.paneList[i].base, true );
					param.cbFunc->CallBack_ListWrite( param.cbWork, param.paneList, i, list_pos );
//					GFL_PRINT( "ds = %f, dpy = %f, py = %f, list_pos = %d(%f), sy = %f\n", baseListPos, drag_pos.y, pos.y, list_pos, baseListPos + sy / param.defSY, sy );
				}
				se = true;
			}
		}

		SetPanePos( param.paneList[i].base, pos );
	}

	gfl2::math::VEC3	pos = GetPanePos( param.paneList[chgPane].base );
	pos.y += mv_y;
	SetPanePos( param.paneList[chgPane].base, pos );

  ChangePutListPriority();
  SetEffect();
  SetItemChangePanePriority( basePane, true );

	if( se == true ){
		PlayCursorMoveSE();
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		入れ替え処理用ペイン表示優先度切り替え
 *
 * @param		index		ペインデータインデックス
 * @param		flg			true = 取得、false = 配置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetItemChangePanePriority( u32 index, bool flg )
{
/*
	if( m_chgOnPane == NULL || m_chgOffPane == NULL ){
		return;
	}

	gfl2::lyt::LytPane * pane   = param.paneList[index].base;
	gfl2::lyt::LytPane * parent = pane->GetParent();
	gfl2::lyt::LytPane * insert;

  if( parent != NULL ){
    parent->RemoveChild( pane );
	}else{
		parent = param.pLytWk->GetLayoutCore()->GetRootPane();
	}

	if( flg == true ){
		insert = m_chgOnPane;
	}else{
		insert = m_chgOffPane;
	}
	parent->InsertChild( insert, pane );
*/


  if( param.listStartPane == NULL )
  {
    return;
  }

  if( flg != false )
  {
    gfl2::lyt::LytPane * parent = param.paneList[index].base->GetParent();
    if( parent != NULL ){
      parent->RemoveChild( param.paneList[index].base );
	  }
    param.listStartPane->AppendChild( param.paneList[index].base );
  }
/*
  else
  {
    ChangePutListPriority();
  }
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		入れ替え処理用ペイン表示優先度基準ペイン設定
 *
 * @param		on		取得時の基準ペイン
 * @param		off		配置時の基準ペイン
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void PaneList::SetupChangePosPane( gfl2::lyt::LytPaneIndex on, gfl2::lyt::LytPaneIndex off )
{
  m_chgOnPane  = param.pLytWk->GetPane( on );
	m_chgOffPane = param.pLytWk->GetPane( off );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		入れ替え処理用ペイン表示優先度基準ペイン設定
 *
 * @param		on		取得時の基準ペイン
 * @param		off		配置時の基準ペイン
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void PaneList::SetupChangePosPane( gfl2::lyt::LytPane * on, gfl2::lyt::LytPane * off )
{
  m_chgOnPane  = on;
	m_chgOffPane = off;
}
*/

//--------------------------------------------------------------------------------------------
/**
 * @brief		複数のpanelistを同期するための処理
 *
 * @param		src		同期Src側
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::Sync( PaneList* src )
{
  param.listPos		= src->param.listPos;
  param.listScroll	= src->param.listScroll;
  tmpScroll			= src->tmpScroll;
  PutCursor( param.listPos );
  InitListPut();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		Bボタンを押した際に反応するペインの設定
 *
 * @param		list_pos		該当ペインの位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::SetCancelListPos(s32 list_pos)
{
	//現状ではスクロールを未考慮
	//m_keyCancelFlag = falseにしておかないとそちらが優先されてアニメの再生前にCANCELが返る
	cancelListPos = list_pos;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル移動SE再生
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void PaneList::PlayCursorMoveSE(void)
{
	if( m_moveSE != SE_NONE )
  {
    Sound::PlaySE( m_moveSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キー選択アニメ再生
 *
 * @param		none
 *
 * @retval	"true = 再生"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::StartKeySelectAnime(void)
{
	if( param.valMax == 0 ){
		return false;
	}

	u32	id = GetPosPaneIndex( param.listPos );
	u32	anm = param.paneList[id].selectAnime;

	if( seTbl[id] != SE_NONE )
  {
    Sound::PlaySE( seTbl[id] );
	}

	// いれかえ時はアニメをしないで決定
	if( chgFlag == true ){
		return false;
	}

	if( anm == app::tool::ButtonManager::ANIMATION_NULL ){
		return false;
	}

	if( param.pLytWk->IsBindAnime( anm ) == false ){
		param.pLytWk->BindAnime( anm );
	}
	param.pLytWk->SetAutoAnime( anm, true );
	param.pLytWk->SetAnimeFrame( anm, 0 );

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キー選択アニメ再生チェック
 *
 * @param		none
 *
 * @retval	"true = 再生中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool PaneList::CheckKeySelectAnime(void)
{
	u32	anm = param.paneList[ GetPosPaneIndex(param.listPos) ].selectAnime;
	if( param.pLytWk->IsAnime( anm ) == false ||
			param.pLytWk->IsBindAnime( anm ) == false ||
			param.pLytWk->IsAnimeEnd( anm ) == true ){
		return false;
	}
	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @func    ListLoop
 * @brief   リストループ処理
 * @date    2015.10.17
 *
 * @param   mv  カーソル移動方向
 *
 * @return  処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::ListLoop( s32 mv )
{
  if( GFL_BOOL_CAST(m_loopFlag) == false )
  {
    return RET_NONE;
  }

  if( mv < 0 )
  {
    if( param.listScroll == 0 && param.listPos == 0 )
    {
      CB_CursorOff( param.listPos );
      CB_ItemChange(
        static_cast<u32>(param.listScroll)+param.listPos,
        static_cast<u32>(listScrollMax)+param.listPosMax-1 );
      u32	tmp_pos    = param.listPos;
      f32	tmp_scroll = param.listScroll;
      param.listPos = param.listPosMax - 1;
      param.listScroll = listScrollMax;
      if( tmp_pos != param.listPos || tmp_scroll != param.listScroll )
      {
        if( listScrollMax != 0.0f )
        {
          InitListPut();
        }
        else
        {
          if( chgFlag == false )
          {
            CB_CursorMove( param.listPos );
            PutCursor( param.listPos );
          }
          else
          {
            InitListPut();
          }
        }
        ChangeCursorPosItemPriority();
        PlayCursorMoveSE();
      }
      return RET_LIST_UP;
    }
  }
  else
  {
    if( param.listScroll == listScrollMax && param.listPos == (param.listPosMax-1) )
    {
      CB_CursorOff( param.listPos );
      CB_ItemChange( static_cast<u32>(param.listScroll)+param.listPos, 0 );
      u32	tmp_pos    = param.listPos;
      f32	tmp_scroll = param.listScroll;
      param.listPos = 0;
      param.listScroll = 0;
      if( tmp_pos != param.listPos || tmp_scroll != param.listScroll )
      {
        if( listScrollMax != 0.0f )
        {
          InitListPut();
        }
        else
        {
          if( chgFlag == false )
          {
            CB_CursorMove( param.listPos );
            PutCursor( param.listPos );
          }
          else
          {
            InitListPut();
          }
        }
        ChangeCursorPosItemPriority();
        PlayCursorMoveSE();
      }
      return RET_LIST_DOWN;
    }
  }

  return RET_NONE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：入力待ち
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateInput(void)
{
	if( param.valMax != 0 ){
		gfl2::ui::TouchPanel * tp = GetTouchPanel();

		if( tp && tp->IsTouchTrigger() == true ){
			if( CheckTouchArea() == true ){
				if( chgFlag == true ){
					// いれかえ時はここで決定
					u32	tmp = param.listPos;
					param.listPos = GetPaneListPos( GetTouchPane() );
					CB_ItemChange(
						static_cast<u32>(param.listScroll)+tmp,
						static_cast<u32>(param.listScroll)+param.listPos );
					PutCursor( param.listPos );
					if( m_selectSE != SE_NONE )
          {
            Sound::PlaySE( m_selectSE );
					}
					return RET_LIST_SELECT;
				}

				if( UpdateButtonManager() == BM_UPDATE_HOLD ){
					basePane = BM_GetHoldButton();

					SetVisibleCursor( false );
					CB_CursorOff( param.listPos );
					param.listPos = GetPaneListPos( basePane );
//					CB_CursorMove( param.listPos );
					baseListPos = GetBaseListPos();
					curPane = basePane;
					SetTouchPos( true );
					reviseFlag = false;
					startTPX = nowTPX;
					startTPY = nowTPY;
					m_catchCount = 0;
					updateSeq = PANELIST_SEQ_DRAG;
					return RET_LIST_TOUCH;
				}
			}else{
				if( chgFlag == true ){
					return RET_NONE;	// いれかえ時はここで戻す
				}
			}
		}
	}

  if( param.valMax != 0 )
  {
    if( crossKeyEnable == true )
    {
      gfl2::ui::VectorDevice * key = GetKey();

      if( key )
      {
        // ループ処理
        if( key->IsTrigger(gfl2::ui::DIR_UP) || ( m_skipMode == PAGE_SKIP_LR_KEY && key->IsTrigger(gfl2::ui::DIR_LEFT) ) )
        {
          Result result = ListLoop( -1 );
          if( result != RET_NONE )
          {
            return result;
          }
        }
        if( key->IsTrigger(gfl2::ui::DIR_DOWN) || ( m_skipMode == PAGE_SKIP_LR_KEY && key->IsTrigger(gfl2::ui::DIR_RIGHT) ) )
        {
          Result result = ListLoop( 1 );
          if( result != RET_NONE )
          {
            return result;
          }
        }
        // 上下動作
        if( key->IsRepeat(gfl2::ui::DIR_UP) )
        {
          if( param.listScroll != 0.0f && param.listPos <= (param.listPosMax/2-((param.listPosMax&1)^1)) )
          {
            InitKeyScroll( -1 );
            updateSeq = PANELIST_SEQ_KEY_SCROLL;
            return RET_LIST_UP;
          }
          if( param.listPos != 0 ){
            CursorMove( -1 );
            PutCursor( param.listPos );
            return RET_LIST_UP;
          }
          return RET_NONE;
        }
        if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
        {
          if( param.listScroll < listScrollMax && param.listPos >= (param.listPosMax/2) )
          {
            InitKeyScroll( 1 );
            updateSeq = PANELIST_SEQ_KEY_SCROLL;
            return RET_LIST_DOWN;
          }
          if( param.listPos < (param.listPosMax-1) )
          {
            CursorMove( 1 );
            PutCursor( param.listPos );
            return RET_LIST_DOWN;
          }
          return RET_NONE;
        }
        // ページ送り：LRキー
        if( m_skipMode == PAGE_SKIP_LR_KEY )
        {
          if( key->IsRepeat( gfl2::ui::DIR_LEFT ) )
          {
            return PageUp();
          }
          if( key->IsRepeat( gfl2::ui::DIR_RIGHT ) )
          {
            return PageDown();
          }
        }
      }
		}

    gfl2::ui::Button * button = GetButton();
    if( button )
    {
      // 決定
      if( GFL_BOOL_CAST(m_keySelectFlag) == true )
      {
        if( button->IsTrigger(gfl2::ui::BUTTON_A) )
        {
          if( param.valMax > 0 )
          {
            u32	paneIndex = GetPosPaneIndex( param.listPos );
            if( pButtonManager && pButtonManager->IsButtonInputEnable( paneIndex ) )
            {
              if( StartKeySelectAnime() == false )
              {
                return RET_LIST_SELECT;
              }
              updateSeq = PANELIST_SEQ_KEY_SELECT_ANIME;
              return RET_LIST_PROCESSING;
            }
          }
        }
      }

      if( m_skipMode == PAGE_SKIP_LR_BUTTON )
      {
        // ループ処理：LRボタン
        if( button->IsTrigger(gfl2::ui::BUTTON_L) )
        {
          Result result = ListLoop( -1 );
          if( result != RET_NONE )
          {
            return result;
          }
        }
        if( button->IsTrigger(gfl2::ui::BUTTON_R) )
        {
          Result result = ListLoop( 1 );
          if( result != RET_NONE )
          {
            return result;
          }
        }
        // ページ送り：LRボタン
        if( button->IsRepeat(gfl2::ui::BUTTON_L) )
        {
          return PageUp();
        }
        if( button->IsRepeat(gfl2::ui::BUTTON_R) )
        {
          return PageDown();
        }
      }

      // キャンセル
      if( GFL_BOOL_CAST(m_keyCancelFlag) == true )
      {
        if( button->IsTrigger(gfl2::ui::BUTTON_B) )
        {
          if( m_cancelSE != SE_NONE )
          {
            Sound::PlaySE( m_cancelSE );
          }
          return RET_LIST_CANCEL;
        }
      }

      // キャンセル：特定のペインを動作させる時用
      if( 0 <= cancelListPos )
      {
        if( button->IsTrigger(gfl2::ui::BUTTON_B) )
        {
          CB_CursorOff( param.listPos );
          param.listPos = cancelListPos;
          InitListPut();
          // @fix GFSBTS【1418】：写真保存でBボタンを押した際にキャンセル音ではなく決定音が鳴っている件の修正
          //SEを一時的にキャンセルSEに書き換えてStartKeySelectAnimeを呼び、そのあとにSEを戻す
          u32 old_se = seTbl[ GetPosPaneIndex(param.listPos) ];
          seTbl[ GetPosPaneIndex(param.listPos) ] = m_cancelSE;
          if( StartKeySelectAnime() == false )
          {
            seTbl[ GetPosPaneIndex(param.listPos) ] = old_se;
            return RET_LIST_SELECT;
          }
          seTbl[ GetPosPaneIndex(param.listPos) ] = old_se;
          updateSeq = PANELIST_SEQ_KEY_SELECT_ANIME;
          return RET_LIST_PROCESSING;
        }
      }
    }
  }

  return RET_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：キースクロール
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateKeyScroll(void)
{
	if( chgFlag == true ){
		return UpdateChangeKeyScroll();
	}

	Result ret;
	if( scrollVec < 0 ){
		ret = RET_LIST_UP;
	}else{
		ret = RET_LIST_DOWN;
	}

	if( scrollCount == static_cast<s8>(GetScrollFrame(trgSpeed)) ){
		ReviseInitScrollList();
		param.listScroll += ( 1.0f * scrollVec );
		CB_CursorMove( param.listPos );

		if( crossKeyEnable == true ){
			gfl2::ui::VectorDevice * key = GetKey();

			if( scrollVec == -1 ){
				if( !key || !key->IsHold(gfl2::ui::DIR_UP) || param.listScroll == 0.0f ){
					updateSeq = PANELIST_SEQ_INPUT;
					return RET_NONE;
				}
			}else if( scrollVec == 1 ){
				if( !key || !key->IsHold(gfl2::ui::DIR_DOWN) || param.listScroll >= listScrollMax ){
					updateSeq = PANELIST_SEQ_INPUT;
					return RET_NONE;
				}
			}
		}

		if( trgCount == SCROLL_FRAME_CHG_COUNT ){
			trgCount = 0;
			if( trgSpeed < (GFL_NELEMS(ScrollFrame60)-1) ){
				trgSpeed++;
			}
		}else{
			trgCount++;
		}
		basePane = GetDefaultPositionPane();
		baseListPos = param.listScroll;
		scrollCount = 0;
		return ret;
	}

	if( scrollCount == 0 ){
		CB_CursorOff( param.listPos );
//		PlayCursorMoveSE();
	}
	ScrollListPutKey( scrollVec );
	scrollCount++;

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：入れ替え用キースクロール
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateChangeKeyScroll(void)
{
	Result ret;
	if( scrollVec < 0 ){
		ret = RET_LIST_UP;
	}else{
		ret = RET_LIST_DOWN;
	}

	if( scrollCount == static_cast<s8>(GetScrollFrame(trgSpeed)) ){
		CB_ItemChange(
			static_cast<u32>(param.listScroll)+param.listPos,
			static_cast<u32>(param.listScroll)+param.listPos+scrollVec );
		ReviseInitScrollList();
		param.listScroll += ( 1.0f * scrollVec );
//		CB_CursorMove( param.listPos );

		if( crossKeyEnable == true ){
			gfl2::ui::VectorDevice * key = GetKey();

			if( scrollVec == -1 ){
				if( !key || !key->IsHold(gfl2::ui::DIR_UP) || param.listScroll == 0.0f ){
					SetItemChangePanePriority( basePane, false );
					updateSeq = PANELIST_SEQ_INPUT;
					return RET_NONE;
				}
			}else if( scrollVec == 1 ){
				if( !key || !key->IsHold(gfl2::ui::DIR_DOWN) || param.listScroll >= listScrollMax ){
					SetItemChangePanePriority( basePane, false );
					updateSeq = PANELIST_SEQ_INPUT;
					return RET_NONE;
				}
			}
		}

		if( trgCount == SCROLL_FRAME_CHG_COUNT ){
			trgCount = 0;
			if( trgSpeed < (GFL_NELEMS(ScrollFrame60)-1) ){
				trgSpeed++;
			}
		}else{
			trgCount++;
		}
//		basePane = GetDefaultPositionPane();
		chgPane  = GetPosPaneIndex( param.listPos+scrollVec );
		baseListPos = param.listScroll;
		scrollCount = 0;
		return ret;
	}

	if( scrollCount == 0 ){
//		CB_CursorOff( param.listPos );
//		PlayCursorMoveSE();
	}
//	ScrollListPutKey( scrollVec );
	ScrollListPutChange( param.defSY / GetScrollFrame(trgSpeed) * scrollVec, scrollVec );
	scrollCount++;

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：ドラッグ処理
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateDrag(void)
{
	switch( UpdateButtonManager() ){
	case BM_UPDATE_NONE:					// 動作なし
		m_catchCount = 0;
		{
			gfl2::ui::TouchPanel * tp = GetTouchPanel();
			if( tp == NULL || ( tp && tp->IsTouch() == false ) ){
				PutCursor( param.listPos );
				CB_CursorMove( param.listPos );
				SetVisibleCursor( true );
				updateSeq = PANELIST_SEQ_INPUT;
			}
		}
		break;

	case BM_UPDATE_HOLD:					// ボタンを抑えている
		SetTouchPos( false );
		if( CheckDragScroll() == true ){
			CancelButtonHold();
			m_catchCount = 0;
			updateSeq = PANELIST_SEQ_DRAG_SCROLL;
			break;
		}

		if( CheckTouchPane( param.paneList[basePane].bound ) == true ){
			if( GFL_BOOL_CAST(m_catchFlag) == true ){
				m_catchCount++;
				if( CheckCatchItem() == true ){
					CancelButtonHold();
					PutCursor( param.listPos );
					SetVisibleCursor( false );  // PutCursorで表示されるので消す 2015/05/11 by nakahiro
					updateSeq = PANELIST_SEQ_INPUT;
					return RET_LIST_CATCH;
				}
			}
		}else{
			m_catchCount = 0;
		}
		break;

	case BM_UPDATE_SELECT_ANIME:	// ボタン選択アニメ中
		break;

	case BM_UPDATE_SELECT:				// ボタン選択
		PutCursor( param.listPos );
		SetVisibleCursor( true );
		updateSeq = PANELIST_SEQ_INPUT;
		return RET_LIST_SELECT;
	}

	return RET_LIST_TOUCH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：ドラッグスクロール
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateDragScroll(void)
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();

	if( reviseFlag == false ){
		// ドラッグ中
		if( tp && tp->IsTouch() == true ){

			SetTouchPos( false );

			if( CheckTouchArea( nowTPX, nowTPY ) == false ){
				return RET_LIST_TOUCH;
			}

			f32	mv = static_cast<f32>( oldTPY - nowTPY );		// タッチ座標の差分

			bool baseChange = false;

			// +方向
			if( mv > 0.0f ){
				gfl2::math::VEC3 pos = GetPanePos( param.paneList[basePane].base );
				// 座標限界値補正
				if( ( pos.y + mv ) > param.defPY ){
//					GFL_PRINT( "[+]座標限界\n" );
					mv = param.defPY - pos.y;
				}
				// スクロール値限界補正
				f32	ds = baseListPos - static_cast<f32>(param.listPos);
				f32	lp = static_cast<f32>(param.listPos)*param.defSY - ( param.defPY - ( pos.y + mv ) );
				if( ( ds*param.defSY+lp) > (listScrollMax*param.defSY) ){
//					GFL_PRINT( "[+]スクロール限界\n" );
					mv = listScrollMax*param.defSY - ds*param.defSY - static_cast<f32>(param.listPos)*param.defSY + param.defPY - pos.y;
				}
				if( mv != 0.0f ){
					ScrollListPut( mv, 1 );
				}else{
					baseChange = true;
				}
			// -方向
			}else if( mv < 0.0f ){
				gfl2::math::VEC3 pos = GetPanePos( param.paneList[basePane].base );
				// 座標限界値補正
				f32	dpy = param.defPY - param.defSY * static_cast<f32>( param.listPosMax-1 );
				if( ( pos.y + mv ) < dpy ){
//					GFL_PRINT( "[-]座標限界\n" );
					mv = dpy - pos.y;
				}
				// スクロール値限界補正
				if( ( baseListPos*param.defSY-(param.defPY-(pos.y+mv)) ) < 0.0f ){
//					GFL_PRINT( "[-]スクロール限界\n" );
					mv = -baseListPos*param.defSY + param.defPY - pos.y;
				}
				if( mv != 0.0f ){
					ScrollListPut( mv, -1 );
				}else{
					baseChange = true;
				}
			}

			if( baseChange == true ){
				u32	tmp = GetTouchPane();
				if( tmp != TOUCH_PANE_NONE && tmp != basePane ){
					f32	base_pos = baseListPos - param.listScroll;
					param.listScroll += static_cast<f32>( static_cast<s32>(base_pos) - GetPaneListPos(basePane) );
					basePane = tmp;
					param.listPos = GetPaneListPos( basePane );
					baseListPos = GetBaseListPos();
					curPane = tmp;
				}
			}

		}else{

			// オートスクロールチェック
			if( CheckTouchArea( oldTPX, oldTPY ) == true ){
				s32	as_dot = gfl2::math::Abs( nowTPY - oldTPY );
				if( as_dot >= 3 ){
					s32	mv = 0;
					// -
					if( nowTPY > oldTPY ){
						mv = -1;
					// +
					}else if( nowTPY < oldTPY ){
						mv = 1;
					}
					gfl2::math::VEC3 pos = GetPanePos( param.paneList[basePane].base );
					f32	ry = param.defPY - static_cast<f32>(static_cast<s32>((param.defPY-pos.y)/param.defSY)) * param.defSY;
					if( pos.y != ry ){
						ReviseInitScrollList();
					}
					f32	base_pos = baseListPos - param.listScroll;
					param.listScroll += static_cast<f32>( static_cast<s32>(base_pos) - GetPaneListPos(basePane) );
					s32	lp = GetPaneListPos( basePane );
					if( lp < 0 ){
						autoScrollListPos = 0;	// オートスクロール後に戻す位置
					}else if( lp >= static_cast<s32>(param.listPosMax) ){
						autoScrollListPos = param.listPosMax - 1;	// オートスクロール後に戻す位置
					}else{
						autoScrollListPos = lp;	// オートスクロール後に戻す位置
					}
					if( mv > 0 ){
						param.listPos = 0;
					}else{
						param.listPos = param.listPosMax - 1;
					}
					basePane = GetPosPaneIndex( param.listPos );
					curPane = basePane;
					baseListPos = GetBaseListPos();
					CreateScrollValue( (as_dot-3)/8, mv );
					autoScrollValue = 0;
					autoScrollStopPos = AUTO_SCROLL_END;
					updateSeq = PANELIST_SEQ_AUTO_SCROLL;
					return RET_LIST_TOUCH;
				}
			}

			// 終了準備
			gfl2::math::VEC3 pos = GetPanePos( param.paneList[basePane].base );
			reviseIndex = basePane;
			revisePY = param.defPY - static_cast<f32>(static_cast<s32>((param.defPY-pos.y)/param.defSY)) * param.defSY;
			f32	abs = gfl2::math::FAbs( revisePY - pos.y );

			if( abs == 0.0f ){
				param.listScroll += ( static_cast<f32>(param.listPos) - (param.defPY-pos.y) / param.defSY );
				param.listPos = GetPaneListPos( curPane );
				ReviseInitScrollList();
				CB_CursorMove( param.listPos );
				PutCursor( param.listPos );
				SetVisibleCursor( true );
				updateSeq = PANELIST_SEQ_INPUT;
				return RET_NONE;
			}else if( abs < (param.defSY/2.0f) ){
				reviseParam = param.defSY / 8;
			}else{
				reviseParam = -param.defSY / 8;
				revisePY -= param.defSY;
			}
			reviseFlag = true;
		}

	}else{

		gfl2::math::VEC3 pos = GetPanePos( param.paneList[reviseIndex].base );
		bool	end = false;
		if( ( reviseParam < 0.0f && (pos.y+reviseParam) <= revisePY ) ||
				( reviseParam > 0.0f && (pos.y+reviseParam) >= revisePY ) ){
			reviseParam = revisePY - pos.y;
			end = true;
		}
		if( reviseParam > 0.0f ){
			ScrollListPut( reviseParam, 1 );
		}else if( reviseParam < 0.0f ){
			ScrollListPut( reviseParam, -1 );
		}

		if( end == true ){
			pos = GetPanePos( param.paneList[reviseIndex].base );
			param.listScroll += ( static_cast<f32>(param.listPos) - (param.defPY-pos.y) / param.defSY );
			param.listPos = GetPaneListPos( curPane );
			ReviseInitScrollList();
			PutCursor( param.listPos );
			SetVisibleCursor( true );
			CB_CursorMove( param.listPos );
			updateSeq = PANELIST_SEQ_INPUT;
			return RET_NONE;
		}

	}

	return RET_LIST_TOUCH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：オートスクロール
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateAutoScroll(void)
{
	gfl2::ui::TouchPanel * tp = GetTouchPanel();

	f32	val = pScrollValue->GetCurrentValue();

	pScrollValue->UpdateScroll();

	// 停止チェック
	if( tp ){
		if( tp->IsTouchTrigger() == true ){
			SetTouchPos( true );

			f32	tpx = static_cast<f32>( nowTPX-160 );
			f32	tpy = static_cast<f32>( 120-nowTPY );

			if( CheckListArea( tpx, tpy ) == true ){
//				GFL_PRINT( "y = %d, tpy = %f\n", y, tpy );
/*
        @fix niji GFNMCat[801]: 
              部品ペインの場合、タッチ範囲とペインの座標の整合性がとれない場合があるため、
              取得できない場合の値を返し、その場合は処理を無効化する
*/
//        pScrollValue->StopRequest( 1 );
//        autoScrollStopPos = GetTouchListPos( tpy );
        u32 list_pos = GetTouchListPos( tpy );
        if( list_pos != param.listPosMax )
        {
          pScrollValue->StopRequest( 1 );
          autoScrollStopPos = list_pos;
        }
			}
		}else if( tp->IsTouch() == true ){
			SetTouchPos( false );
		}
	}

	f32	scroll = pScrollValue->GetCurrentValue();
//	GFL_PRINT( "val = %f, scroll = %f, calc = %f\n", val, scroll, ( scroll - val ) * param.defSY );
	val = ( scroll - val ) * param.defSY;

	autoScrollValue += gfl2::math::FAbs( val );

	// Y座標順のテーブルを作成
	s32 * tbl = tmpPaneListTable;
	{
		f32	tmp_def_y;
		MakePabeListRowTable( tbl, basePane, &tmp_def_y );
	}
	// basePaneのテーブル位置を取得
	s32	bp_tbl_pos = -1;
	for( u32 i=0; i<param.paneListMax; i++ ){
		if( tbl[i] == static_cast<s32>(basePane) ){
			bp_tbl_pos = i;
			break;
		}
	}

	if( val > 0.0f ){
		// スクロールが項目分を超えたら基準位置を変更
		for ever{
			if( autoScrollValue >= param.defSY ){
				autoScrollValue -= param.defSY;
				if( bp_tbl_pos == (static_cast<s32>(param.paneListMax)-1) ){
					bp_tbl_pos = 0;
				}else{
					bp_tbl_pos += 1;
				}
			}else{
				break;
			}
		}
		basePane = tbl[bp_tbl_pos];
		baseListPos = GetBaseListPos();
		ScrollListPut( val, 1 );
		param.listScroll = scroll;
//		return RET_LIST_AUTO_SCRILL;
	}else if( val < 0.0f ){
		// スクロールが項目分を超えたら基準位置を変更
		for ever{
			if( autoScrollValue >= param.defSY ){
				autoScrollValue -= param.defSY;
				if( bp_tbl_pos == 0 ){
					bp_tbl_pos = param.paneListMax - 1;
				}else{
					bp_tbl_pos -= 1;
				}
			}else{
				break;
			}
		}
		basePane = tbl[bp_tbl_pos];
		baseListPos = GetBaseListPos();
		ScrollListPut( val, -1 );
		param.listScroll = scroll;
//		return RET_LIST_AUTO_SCRILL;
	}

	if( pScrollValue->IsScroll() == false ){
		param.listScroll = pScrollValue->GetCurrentValue();
		DeleteScrollValue();
		ReviseInitScrollList();

		u32	tp_pane = GetTouchPane();
		// ドラッグ処理へ
		if( tp_pane != TOUCH_PANE_NONE ){
			basePane = tp_pane;
			param.listPos = GetPaneListPos( tp_pane );
			baseListPos = GetBaseListPos();
			curPane = basePane;
//			SetTouchPos( true );
			reviseFlag = false;
			updateSeq = PANELIST_SEQ_DRAG_SCROLL;
			m_catchCount = 0;
			return RET_LIST_TOUCH;
		}else{
			if( autoScrollStopPos != AUTO_SCROLL_END ){
				param.listPos = autoScrollStopPos;
			}else{
				param.listPos = autoScrollListPos;
			}
			PutCursor( param.listPos );
			SetVisibleCursor( true );
			CB_CursorMove( param.listPos );
			updateSeq = PANELIST_SEQ_INPUT;
			return RET_NONE;
		}
	}

	return RET_LIST_AUTO_SCRILL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート：キー選択アニメ
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
PaneList::Result PaneList::UpdateKeySelectAnime(void)
{
	if( CheckKeySelectAnime() == false ){
		updateSeq = PANELIST_SEQ_INPUT;
		return RET_LIST_SELECT;
	}

	return RET_LIST_PROCESSING;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールフレーム速度取得
 *
 * @param		speed	キースクロール速度テーブルインデックス
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
u32 PaneList::GetScrollFrame( int speed )
{
	if( m_frameMode == GameSys::FRAMEMODE_60 ){
		return ScrollFrame60[speed];
	}
	return ScrollFrame30[speed];
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangePutListPriority
 * @brief	  項目の表示プライオリティを配置順に変更
 * @date    2015.03.17
 */
//-----------------------------------------------------------------------------
void PaneList::ChangePutListPriority(void)
{
  if( param.listStartPane == NULL )
  {
    return;
  }

	s32 * tbl = tmpPaneListTable;
	f32	y;

  MakePabeListRowTable( tbl, DEFAULT_PANE_NONE, &y );
  for( s32 i=param.paneListMax-1; i>=0; i-- ){
    gfl2::lyt::LytPane * parent = param.paneList[tbl[i]].base->GetParent();
    if( parent != NULL ){
      parent->RemoveChild( param.paneList[tbl[i]].base );
	  }
    param.listStartPane->PrependChild( param.paneList[tbl[i]].base );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeCursorPosItemPriority
 * @brief	  カーソル位置の項目の表示プライオリティを最高にする
 * @date    2015.04.30
 *
 * @note    入れ替え用
 */
//-----------------------------------------------------------------------------
void PaneList::ChangeCursorPosItemPriority(void)
{
  if( chgFlag != false )
  {
    SetItemChangePanePriority( GetPosPaneIndex(param.listPos), true );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SetAlphaEffect
 * @brief	  範囲外の項目ペインへのエフェクト設定
 * @date    2015.08.07
 */
//-----------------------------------------------------------------------------
/*
void PaneList::SetAlphaEffect(void)
{
  m_isEffect = true;
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    SetEffect
 * @brief	  項目ペインにエフェクトをかける
 * @date    2015.08.07
 */
//-----------------------------------------------------------------------------
void PaneList::SetEffect(void)
{
  if( m_isEffect == false )
  {
    return;
  }

  for( u32 i=0; i<param.paneListMax; i++ )
  {
    param.paneList[i].base->SetAlpha( CalcEffectParam(i) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CalcEffectParam
 * @brief	  項目ペインのエフェクト値取得
 * @date    2015.08.07
 *
 * @param   index   ペインデータテーブルのインデックス
 *
 * @return  エフェクト値
 */
//-----------------------------------------------------------------------------
u32 PaneList::CalcEffectParam( u32 index )
{
  f32 end_y = param.defPY - static_cast<f32>(param.listPosMax-1) * param.defSY;
  gfl2::math::VEC3 trans = GetPanePos( param.paneList[index].base );

  if( trans.y > param.defPY )
  {
    return GetEffectParam( trans.y, param.defPY );
  }
  else if( trans.y < end_y )
  {
    return GetEffectParam( trans.y, end_y );
  }
  return 255;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetEffectParam
 * @brief	  項目ペインのエフェクト値計算
 * @date    2015.08.07
 *
 * @param   p1  座標１
 * @param   p2  座標２
 *
 * @return  エフェクト値
 */
//-----------------------------------------------------------------------------
u32 PaneList::GetEffectParam( f32 p1, f32 p2 )
{
  f32 prm = 255.0f;
  f32 val = gfl2::math::FAbs( p1 - p2 );
  f32 space = param.defSY * 2.0f;

  // @fix GFMMCat[24]: 範囲外の上限をなくした
  prm = prm - prm / space * val;
  if( prm < 0.0f )
  {
    prm = 0.0f;
  }

  return static_cast<u32>(prm);
}


//-----------------------------------------------------------------------------
/**
 * @func    MakeListPos
 * @brief   カーソル位置とスクロール値を生成
 * @date    2015.09.07
 *
 * @param   list_max    リスト項目数
 * @param   move_max    カーソル移動最大値
 * @param   list_pos    リスト項目上のカーソル位置
 * @param   cursor_pos  表示上のカーソル位置
 * @param   scroll_val  スクロール値取得場所
 */
//-----------------------------------------------------------------------------
void PaneList::MakeListPos( s32 list_max, s32 move_max, s32 list_pos, s32 * cursor_pos, s32 * scroll_val )
{
  GFL_ASSERT( move_max > *cursor_pos );

  s32 scroll_max = list_max - move_max;

  if( scroll_max <= 0 )
  {
    *cursor_pos = list_pos;
    *scroll_val = 0;
  }
  else
  {
    *scroll_val = list_pos - *cursor_pos;
    if( *scroll_val > scroll_max )
    {
      s32 tmp = *scroll_val - scroll_max;
      *scroll_val = *scroll_val - tmp;
      *cursor_pos = *cursor_pos + tmp;
    }
  }
  GFL_ASSERT( (*scroll_val+*cursor_pos) == list_pos );
}


//-----------------------------------------------------------------------------
/**
  * @brief ボタンを有効な状態にする
  * @param buttonID  有効にするボタン
  * @param isAnime   true指定時にアニメーションを行う
  */
//-----------------------------------------------------------------------------
void PaneList::SetButtonActive( app::tool::ButtonManager::ButtonId buttonID, bool isAnime )
{
  if( pButtonManager )
  {
    pButtonManager->SetButtonActive( buttonID, isAnime );
  }
}


//-----------------------------------------------------------------------------
/**
  * @brief ボタンを無効な状態にする
  * @param buttonID  無効にするボタン
  * @param isAnime   true指定時にアニメーションを行う
  */
//-----------------------------------------------------------------------------
void PaneList::SetButtonPassive( app::tool::ButtonManager::ButtonId buttonID, bool isAnime )
{
  if( pButtonManager )
  {
    pButtonManager->SetButtonPassive( buttonID, isAnime );
  }
}


//-----------------------------------------------------------------------------
/**
  * @brief ボタンの動作をさせなくする
  *
  * @param is_enable trueで動作  falseで動作しない
  *        SetInputEnableのボタン指定版です
  */
//-----------------------------------------------------------------------------
void PaneList::SetButtonInputEnable( app::tool::ButtonManager::ButtonId buttonID, bool is_enable )
{
  if( pButtonManager )
  {
    pButtonManager->SetButtonInputEnable( buttonID, is_enable );
  }
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
#if PM_DEBUG
void PaneList::DebugPrintPanePos(void)
{
	gfl2::ui::Button * b = param.uiDevMan->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
	if( b->IsTrigger( gfl2::ui::BUTTON_START ) ){
		for( u32 i=0; i<param.paneListMax; i++ ){
			gfl2::math::VEC3	pos = GetPanePos( param.paneList[i].base );
			GFL_PRINT( "base = %f, %f   ", pos.x, pos.y );
			pos = GetPanePos( param.paneList[i].bound );
			GFL_PRINT( "bound = %f, %f   ", pos.x, pos.y );
			GFL_PRINT( "visible = %d\n", GetPaneVisible( param.paneList[i].base ) );
		}
	}
}
#endif // PM_DEBUG


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
