//==============================================================================
/**
 * @file    StrInputUi.cpp
 * @brief   UI関連
 * @author  fukushima_yuya
 * @date    2015.05.22
 * @note    sango_project app strinput_ui.cppから移植
 */
//==============================================================================

// niji
#include "App/StrInput/source/StrInputGraphic.h"
#include "App/StrInput/source/StrInputUi.h"

#include <system/include/HeapDefine.h>

// arc
//#include <niji_conv_header/app/strinput/>
#include <niji_conv_header/app/strinput/strinput_anim_list.h>


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace   App::StrInput
 * @brief       文字入力画面
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )


#define  KEY_REPEAT_INTERVAL    ( 3 )    //!< キーリピートウェイト

#include "strinput_ui_50k.cdat"
#include "strinput_ui_keyboard.cdat"
#include "strinput_ui_50k_usa.cdat"
#include "strinput_ui_keyboard_usa.cdat"
#include "strinput_ui_50k_kor.cdat"
#include "strinput_ui_keyboard_kor.cdat"
#include "StrInputButtonAnimList.cdat"

//! @brief  UIデータ（日本語）
static const UI::UI_DATA UiDataTable[] =
{
  {  // 50音入力
    ButtonData, GFL_NELEMS(ButtonData),
      CursorMoveData, GFL_NELEMS(CursorMoveData),
      GraphicWork::ANMID_KEYMAP_N,
  },
  {  // キーボード入力
    KeyBoard_ButtonData, GFL_NELEMS(KeyBoard_ButtonData),
      KeyBoard_CursorMoveData, GFL_NELEMS(KeyBoard_CursorMoveData),
      GraphicWork::ANMID_KEYMAP_B,
    },
};

//! @brief  UIデータ（英語）
static const UI::UI_DATA UiDataTable_USA[] =
{
  {
    ButtonData_USA, GFL_NELEMS(ButtonData_USA),
      CursorMoveData_USA, GFL_NELEMS(CursorMoveData_USA),
      GraphicWork::ANMID_KEYMAP_N,
  },
  {
    KeyBoard_ButtonData_USA, GFL_NELEMS(KeyBoard_ButtonData_USA),
      KeyBoard_CursorMoveData_USA, GFL_NELEMS(KeyBoard_CursorMoveData_USA),
      GraphicWork::ANMID_KEYMAP_B,
    },
};

//! @brief  UIデータ（韓国語）
static const UI::UI_DATA UiDataTable_KOR[] =
{
  {
    ButtonData_KOR, GFL_NELEMS(ButtonData_KOR),
      CursorMoveData_KOR, GFL_NELEMS(CursorMoveData_KOR),
      GraphicWork::ANMID_KEYMAP_N,
  },
  {
    KeyBoard_ButtonData_KOR, GFL_NELEMS(KeyBoard_ButtonData_KOR),
      KeyBoard_CursorMoveData_KOR, GFL_NELEMS(KeyBoard_CursorMoveData_KOR),
      GraphicWork::ANMID_KEYMAP_B,
    },
};

//! @brief  入力エリアのペインテーブル
static const gfl2::lyt::LytPaneIndex InputHitPaneTable[] = {
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_00,    // 入力フィールド00
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_01,    // 入力フィールド01
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_02,    // 入力フィールド02
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_03,    // 入力フィールド03
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_04,    // 入力フィールド04
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_05,    // 入力フィールド05
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_06,    // 入力フィールド06
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_07,    // 入力フィールド07
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_08,    // 入力フィールド08
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_09,    // 入力フィールド09
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_10,    // 入力フィールド10
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_11,    // 入力フィールド11
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_12,    // 入力フィールド12
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_13,    // 入力フィールド13
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_14,    // 入力フィールド14
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_15,    // 入力フィールド15
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_16,    // 入力フィールド16
  PANENAME_STRINPUT_INP_LOW_000_PANE_INPUT_17,    // 入力フィールド17
};



//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   man     UIデバイスマネージャ
 * @param   heap    ヒープ
 */
//------------------------------------------------------------------------------
UI::UI( gfl2::ui::DeviceManager * man, app::util::Heap * heap )
{
  pHeap = GFL_CREATE_HEAP(
    gfl2::heap::Manager::GetHeapByHeapId(HEAPID_STRINPUT_APP), HEAPID_STRINPUT_UI, 0x5000, gfl2::heap::HEAP_TYPE_EXP, false );
    //gfl2::heap::Manager::GetHeapByHeapId(HEAPID_STRINPUT_APP), HEAPID_STRINPUT_UI, 0x3000, gfl2::heap::HEAP_TYPE_EXP, false );
  CreateUI( man, heap );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
UI::~UI( void )
{
  DeleteUI();
  gfl2::heap::Manager::DeleteHeap( pHeap );
}

//------------------------------------------------------------------------------
/**
 * @brief   UIデータ作成（ボタン＋カーソル）
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 * @param   mode    処理モード
 * @param   lang    言語ID
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateUiData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, u32 mode, u8 lang )
{
  if( pCursorManager != NULL || pButtonManager != NULL ){
    GFL_ASSERT( 0 );
    return;
  }

  const u32 * pAnimArray = (mode == 0) ? KeyMap50onBtnTopAnm : KeyMapKeyboardBtnTopAnm;

  switch( lang )
  {
  case POKEMON_LANG_NULL:    //!< 言語コード：無効な値
    GFL_ASSERT( 0 );
  case POKEMON_LANG_JAPAN:  //!< 言語コード：日本
  case POKEMON_LANG_CHINA:   //!< 言語コード：簡体字（中国）
  case POKEMON_LANG_TAIWAN:  //!< 言語コード：繁体字（台湾、香港）
    pUiData = &UiDataTable[mode];
    break;
  case POKEMON_LANG_KOREA:  //!< 言語コード：韓国
    pUiData = &UiDataTable_KOR[mode];
    break;
  default:                  //!< その他
    pUiData = &UiDataTable_USA[mode];
    break;
  };

  CreateButtonManager( lytwk, resID, pUiData->button, pUiData->button_size, pUiData->anmNo, pAnimArray );
  CreateCursorManager( pUiData->cursor, pUiData->cursor_size );
}


//------------------------------------------------------------------------------
/**
 * @brief   UIデータ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::DeleteUiData( void )
{
  DeleteCursorManager();
  DeleteButtonManager();
}

//------------------------------------------------------------------------------
/**
 * @brief   矢印ボタン設定
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::SetArrowButtonData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID )
{
  gfl2::lyt::LytParts* pParts;
  gfl2::lyt::LytPane*  pPane;
  u32 anmID;

  pParts = lytwk->GetPartsPane( PANENAME_STRINPUT_BGR_LOW_000_PANE_CHANGE_00 );
  pPane  = lytwk->GetPane( pParts, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resID );
  anmID  = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_00_TOUCH;
  pButtonManager->AddButton(
    pHeap,          //!< 管理ワーク確保に使用するヒープ
    BTNID_L,        //!< ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
    lytwk,          //!< ボタンが所属するレイアウトワーク
    pParts,
    pPane,          //!< ボタンを表すペイン( 当たり判定に使用 )
    anmID + GraphicWork::ANMID_CHANGE_ARROW_TOUCH,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_RELEASE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_CANSEL,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_TOUCH_RELEASE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_ACTIVE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_PASSIVE,
    NULL );         //!< ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )

  pParts = lytwk->GetPartsPane( PANENAME_STRINPUT_BGR_LOW_000_PANE_CHANGE_01 );
  pPane  = lytwk->GetPane( pParts, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &resID );
  anmID  = LA_STRINPUT_STRINPUT_BGR_LOW_000__CHANGE_01_TOUCH;
  pButtonManager->AddButton(
    pHeap,          //!< 管理ワーク確保に使用するヒープ
    BTNID_R,        //!< ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
    lytwk,          //!< ボタンが所属するレイアウトワーク
    pParts,
    pPane,          //!< ボタンを表すペイン( 当たり判定に使用 )
    anmID + GraphicWork::ANMID_CHANGE_ARROW_TOUCH,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_RELEASE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_CANSEL,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_TOUCH_RELEASE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_ACTIVE,
    anmID + GraphicWork::ANMID_CHANGE_ARROW_PASSIVE,
    NULL );         //!< ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンボタン設定
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::SetIconButtonData( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID )
{
  gfl2::lyt::LytParts* pParts = lytwk->GetPartsPane( PANENAME_STRINPUT_INP_LOW_000_PANE_TARGETICON );
  gfl2::lyt::LytPane*  pPane  = lytwk->GetPane( pParts, PANENAME_STRINPUT_BTN_LOW_004_PANE_ICON_BOUND, &resID );
  u32  anmNo = GraphicWork::ANMID_INPUT_ICON;

  pButtonManager->AddButton(
    pHeap,          //!< 管理ワーク確保に使用するヒープ
    BTNID_ICON,     //!< ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
    lytwk,          //!< ボタンが所属するレイアウトワーク
    pParts,
    pPane,          //!< ボタンを表すペイン( 当たり判定に使用 )
    anmNo+GraphicWork::BTNANM_TOUCH,
    anmNo+GraphicWork::BTNANM_RELEASE,
    anmNo+GraphicWork::BTNANM_CANCEL,
    anmNo+GraphicWork::BTNANM_TOUCH_RELEASE,
    anmNo+GraphicWork::BTNANM_ACTIVE,
    anmNo+GraphicWork::BTNANM_PASSIVE,
    NULL );         //!< ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンの選択通知タイミングを設定
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::SetButtonNotifyTimming( void )
{
  pButtonManager->SetNotifyTimming( app::tool::ButtonManager::NOTIFY_START_ANIME );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力エリア作成
 *
 * @param   heap    ヒープ
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 * @param   str     入力文字列
 * @param   siz     入力文字数
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateInputArea( app::util::Heap * heap, gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, const gfl2::str::STRCODE * str, u32 siz )
{
  pInputArea = GFL_NEW( heap->GetDeviceHeap() ) InputArea( lytwk, resID, InputHitPaneTable, str, siz );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力エリア削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::DeleteInputArea( void )
{
  GFL_DELETE pInputArea;
}

//------------------------------------------------------------------------------
/**
 * @brief   入力エリア取得
 *
 * @param   start  入力開始位置格納場所
 * @param   end    入力終了位置格納場所
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::GetInputAreaPos( u16 * start, u16 * end )
{
  pInputArea->GetAreaPos( start, end );
}

//------------------------------------------------------------------------------
/**
 * @brief   入力エリアサイズ取得
 *
 * @param   none
 *
 * @return  入力エリアサイズ
 */
//------------------------------------------------------------------------------
u32 UI::GetInputAreaSize( void )
{
  return pInputArea->GetAreaSize();
}

//------------------------------------------------------------------------------
/**
 * @brief   メイン処理
 *
 * @param   none
 *
 * @return  入力結果
 */
//------------------------------------------------------------------------------
u32 UI::Main( void )
{
  // 入力エリア
  if( pInputArea->CheckInputArea(pTouchPanel) == true )
  {
    return RET_INPUT;
  }

  // タッチ
  switch( pButtonManager->Update() )
  {
  case app::tool::ButtonManager::UPDATE_STATE_IDLE:      // 何もしていない
    break;

  case app::tool::ButtonManager::UPDATE_STATE_HOLDING:  // ボタンを押さえている
    {
      u32  ret = GetTouchID( TP_GET_MODE_HOLD );
      if( ret == BTNID_L || ret == BTNID_R )
      {
        return RET_SCROLL_TOUCH;
      }
      else if( ret == BTNID_ICON )
      {
        return RET_NONE;
      }
    }
    return RET_CURSOR_MOVE_TOUCH;
  /*
  case app::tool::ButtonManager::UPDATE_STATE_SELECT_ANIME_PLAYING:  // ボタン選択アニメ再生中
  case app::tool::ButtonManager::UPDATE_STATE_CANCEL:    // ボタンがキャンセルされた
  return RET_NONE;
  */

  case app::tool::ButtonManager::UPDATE_STATE_SELECTED:  // ボタンが選択された
    return GetUpdateReturn( GetTouchID(TP_GET_MODE_RELEASE) );
  }

  // A: 決定
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    return RET_DECIDE;
  }
  // B: 削除
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return RET_DELETE_KEY;
  }
  // X: 文字切り替え
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    return RET_CHG_CODE;
  }
  // Y: 入力切替
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    return RET_CHG_INCREMENT;
  }
  // L: 入力モード切替
  if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
  {
    return RET_L_SCROLL_KEY;
  }
  // R: 入力モード切替
  if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
  {
    return RET_R_SCROLL_KEY;
  }
  // START:「おわり」へジャンプ
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    return RET_END_JUMP;
  }

  app::tool::CursorManager::MOVE_RET cmret = pCursorManager->MainRepeat();

  if( cmret == app::tool::CursorManager::RET_MOVE )
  {
    return RET_CURSOR_MOVE;
  }

  return RET_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置のボタンに対応した入力結果を取得
 *
 * @param   none
 *
 * @return  入力結果
 */
//------------------------------------------------------------------------------
u32 UI::GetDecideID( void )
{
  return GetUpdateReturn( pCursorManager->GetID() );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置取得
 *
 * @param   none
 *
 * @return  カーソル位置
 */
//------------------------------------------------------------------------------
u32 UI::GetCursorPos( void )
{
  return pCursorManager->GetPos();
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置設定
 *
 * @param   pos    カーソル位置
 *
 * @retval  "true = 位置が移動した"
 * @retval  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool UI::SetCursorPos( u32 pos )
{
  return pCursorManager->SetPos( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   指定位置のカーソルID取得
 *
 * @param   pos    カーソル位置
 *
 * @return  カーソルID
 */
//------------------------------------------------------------------------------
u32 UI::GetCursorID( u32 pos )
{
  return pCursorManager->GetID( pos );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンIDから位置を取得
 *
 * @param   id  ボタンID
 *
 * @return  ボタンデータインデックス
 */
//------------------------------------------------------------------------------
u32 UI::GetButtonPosID( u32 id )
{
  for( u32 i=0; i<pUiData->button_size; i++ )
  {
    if( pUiData->button[i].id == id )
    {
      return i;
    }
  }

  GFL_ASSERT( 0 );
  return 0;
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置からアニメを取得
 *
 * @param   none
 *
 * @return  アニメオフセット
 */
//------------------------------------------------------------------------------
u32 UI::GetCursorPosAnmOffset( void )
{
  return GetButtonPosIDAnmOffset( pCursorManager->GetID() );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンIDからアニメを取得
 *
 * @param   id  ボタンID1
 *
 * @return  アニメオフセット
 */
//------------------------------------------------------------------------------
u32 UI::GetButtonPosIDAnmOffset( u32 id )
{
  return pUiData->button[GetButtonPosID(id)].anmOffs;
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチしているボタンのIDを取得
 *
 * @param   mode  取得モード
 *
 * @return  ボタンID
 */
//------------------------------------------------------------------------------
u32 UI::GetTouchID( u32 mode )
{
  if( mode == TP_GET_MODE_HOLD )
  {
    return pButtonManager->GetHoldingButtonId();
  }

  return pButtonManager->GetSelectedButtonId();
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの当たり判定ペインを取得（カーソル位置指定）
 *
 * @param   pos    カーソル位置
 *
 * @return  当たり判定ペイン
 */
//------------------------------------------------------------------------------
void UI::GetButtonBoundingPane( u32 pos, gfl2::lyt::LytPaneIndex * pPartsIdx, gfl2::lyt::LytPaneIndex * pPaneIdx )
{
  GetButtonIDBoundingPane( GetCursorID(pos), pPartsIdx, pPaneIdx );
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンの当たり判定ペインを取得（ID指定）
 *
 * @param   id  ボタンID（ = カーソルID）
 *
 * @return  当たり判定ペイン
 */
//------------------------------------------------------------------------------
void UI::GetButtonIDBoundingPane( u32 id, gfl2::lyt::LytPaneIndex * pPartsIdx, gfl2::lyt::LytPaneIndex * pPaneIdx )
{
  for( u32 i=0; i<pUiData->button_size; i++ )
  {
    if( pUiData->button[i].id == id )
    {
      *pPartsIdx = pUiData->button[i].parts;
      *pPaneIdx  = pUiData->button[i].bound;
      return;
    }
  }

  GFL_ASSERT( 0 );
}

//------------------------------------------------------------------------------
/**
 * @brief   UI関連作成
 *
 * @param   man   UIデバイスマネージャ
 * @param   haap  ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateUI( gfl2::ui::DeviceManager * man, app::util::Heap * heap )
{
  pUiDeviceManager = man;
  pButton     = pUiDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  pKey        = pUiDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  pStick      = pUiDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  pTouchPanel = pUiDeviceManager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  static const gfl2::ui::Device::RepeatParam rep = {
    gfl2::ui::Device::DEFAULT_REPEAT_START_WAIT,
    KEY_REPEAT_INTERVAL
  };
  pKey->SetRepeatParam( rep );

 #ifdef  STRINPUT_USE_UI_GYROSCOPE
  // ジャイロ
  pUiDeviceManager->BootGyroscope( heap->GetDeviceHeap(), gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
  pGyroscope  = pUiDeviceManager->GetGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
  pGyroscope->SetZeroDriftMode( gfl2::ui::Gyroscope::ZERO_DRIFT_MODE_TIGHT );
  pGyroscope->DisableRevisionByAcceleration();
  pGyroscope->DisableZeroDrift();
#endif  // STRINPUT_USE_UI_GYROSCOPE

#ifdef  STRINPUT_USE_UI_ACCELEROMETER
  // 加速度センサー
  pUiDeviceManager->BootAccelerometer( heap->GetDeviceHeap(), gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
  pAccelerometer = pUiDeviceManager->GetAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
#endif  // STRINPUT_USE_UI_ACCELEROMETER
}

//------------------------------------------------------------------------------
/**
 * @brief   UI関連削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::DeleteUI( void )
{
#ifdef  STRINPUT_USE_UI_ACCELEROMETER
  pUiDeviceManager->StopAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
#endif  // STRINPUT_USE_UI_ACCELEROMETER
#ifdef  STRINPUT_USE_UI_GYROSCOPE
  pUiDeviceManager->StopGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
#endif  // STRINPUT_USE_UI_GYROSCOPE

  // キーリピートリセット
  pKey->SetDefaultRepeatParam();
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルマネージャ作成
 *
 * @param   dat   カーソルデータ
 * @param   size  データ数
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateCursorManager( const app::tool::CursorManager::MOVE_DATA * dat, u32 size )
{
  pCursorManager = GFL_NEW( pHeap ) app::tool::CursorManager( dat, size );
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルマネージャ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::DeleteCursorManager( void )
{
  if( pCursorManager != NULL )
  {
    GFL_DELETE pCursorManager;
    pCursorManager = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャ作成
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 * @param   dat     ボタンデータ
 * @param   anm_no  アニメ番号
 * @param   anm_array   先頭アニメ番号配列
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateButtonManager( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytMultiResID resID, const UI::BUTTON_DATA * dat, int size, u32 anm_no, const u32 * anm_array )
{
  GFL_PRINT( "%s\n", __FUNCTION__ );
  pButtonManager = GFL_NEW( pHeap ) app::tool::ButtonManager( pHeap, pUiDeviceManager, size+3 );  //!< +左右矢印 +アイコン
  pButtonManager->ChangeButtonProcessMode( app::tool::ButtonManager::BUTTON_PROCESS_MULTIPLE, pHeap );

  for( int i=0; i<size; i++ )
  {
    gfl2::lyt::LytParts* pParts = lytwk->GetPartsPane( dat[i].parts );
    gfl2::lyt::LytPane*  pPane  = lytwk->GetPane( pParts, dat[i].bound, &resID );
    //u32 anm = anm_no + dat[i].anmOffs * (GraphicWork::BTNANM_MAX+1);    //!< キー選択分＋
    u32 anm = anm_array[ dat[i].anmOffs ];
    GFL_PRINT( "ANIM_ID[%d], ANIM_OFFSET[%d]\n", anm, dat[i].anmOffs );

    pButtonManager->AddButton(
      pHeap,        //!< 管理ワーク確保に使用するヒープ
      dat[i].id,    //!< ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
      lytwk,        //!< ボタンが所属するレイアウトワーク
      pParts,
      pPane,        //!< ボタンを表すペイン( 当たり判定に使用 )
      anm+GraphicWork::BTNANM_TOUCH,
      anm+GraphicWork::BTNANM_RELEASE,
      anm+GraphicWork::BTNANM_CANCEL,
      anm+GraphicWork::BTNANM_TOUCH_RELEASE,
      anm+GraphicWork::BTNANM_ACTIVE,
      anm+GraphicWork::BTNANM_PASSIVE,
      NULL );       //!< ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャ削除
 *
 * @param   none
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::DeleteButtonManager( void )
{
  if( pButtonManager != NULL )
  {
    GFL_DELETE pButtonManager;
    pButtonManager = NULL;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   各ボタンに対応した入力結果を取得
 *
 * @param   id  ボタンID
 *
 * @return  入力結果
 */
//------------------------------------------------------------------------------
u32 UI::GetUpdateReturn( u32 id )
{
  // かな
  if( id == BTNID_HIRAGANA )
  {
    return RET_CHG_HIRAGANA;
    // カナ
  }
  else if( id == BTNID_KATAKANA )
  {
    return RET_CHG_KATAKANA;
    // ABC
  }
  else if( id == BTNID_ABC )
  {
    return RET_CHG_ABC;
    // 記号
  }
  else if( id == BTNID_MARK )
  {
    return RET_CHG_MARK;
    // 消す
  }
  else if( id == BTNID_DELETE )
  {
    return RET_DELETE;
    // 終わり
  }
  else if( id == BTNID_END )
  {
    return RET_END;
    // SPACE
  }
  else if( id == BTNID_SPACE )
  {
    return RET_SPACE;
    // SHIFT
  }
  else if( id == BTNID_SHIFT )
  {
    return RET_SHIFT;
    // 左矢印
  }
  else if( id == BTNID_L )
  {
    return RET_L_SCROLL;
    // 右矢印
  }
  else if( id == BTNID_R )
  {
    return RET_R_SCROLL;
    // アイコン
  }
  else if( id == BTNID_ICON )
  {
    return RET_ICON;
  }

  return ( RET_KEYBOARD + id );
}

GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )
