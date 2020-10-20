//======================================================================
/**
 * @file    NumberInputUI.cpp
 * @date    2015/11/25 11:36:35
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：入力関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "NumberInputUI.h"

#include <AppLib/include/Tool/app_tool_ButtonManager.h>
#include <AppLib/include/Tool/app_tool_CursorManager.h>
#include <AppLib/include/Util/app_util_heap.h>

// gflib2
#include <ui/include/gfl2_UI.h>

// arc
#include <niji_conv_header/app/number_input/number_input_pane.h>
#include <niji_conv_header/app/number_input/number_input_anim_list.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(NumberInput)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
//! @brief    カーソル移動データ
static const app::tool::CursorManager::MOVE_DATA c_CursorMoveData[CURSOR_DATA_MAX] = {
  {  0, { 36, 10,  9,  1 } },   //!< 00: 1
  {  1, { 37, 11,  0,  2 } },   //!< 01: 2
  {  2, { 38, 12,  1,  3 } },   //!< 02: 3
  {  3, { 39, 13,  2,  4 } },   //!< 03: 4
  {  4, { 40, 14,  3,  5 } },   //!< 04: 5
  {  5, { 41, 15,  4,  6 } },   //!< 05: 6
  {  6, { 42, 16,  5,  7 } },   //!< 06: 7
  {  7, { 43, 17,  6,  8 } },   //!< 07: 8
  {  8, { 44, 18,  7,  9 } },   //!< 08: 9
  {  9, { 45, 19,  8,  0 } },   //!< 09: 0

  { 10, {  0, 20, 19, 11 } },   //!< 10: A
  { 11, {  1, 21, 10, 12 } },   //!< 11: B
  { 12, {  2, 22, 11, 13 } },   //!< 12: C
  { 13, {  3, 23, 12, 14 } },   //!< 13: D
  { 14, {  4, 24, 13, 15 } },   //!< 14: E
  { 15, {  5, 25, 14, 16 } },   //!< 15: F
  { 16, {  6, 26, 15, 17 } },   //!< 16: G
  { 17, {  7, 27, 16, 18 } },   //!< 17: H
  { 18, {  8, 28, 17, 19 } },   //!< 18: I
  { 19, {  9, 29, 18, 10 } },   //!< 19: J

  { 20, { 10, 30, 29, 21 } },   //!< 20: K
  { 21, { 11, 31, 20, 22 } },   //!< 21: L
  { 22, { 12, 32, 21, 23 } },   //!< 22: M
  { 23, { 13, 33, 22, 24 } },   //!< 23: N
  { 24, { 14, 34, 23, 25 } },   //!< 24: O
  { 25, { 15, 35, 24, 26 } },   //!< 25: P
  { 26, { 16, 42, 25, 27 } },   //!< 26: Q    // @fix NMCat[2488] ↓への移動先を変更
  { 27, { 17, 43, 26, 28 } },   //!< 27: R    // @fix NMCat[2488] ↓への移動先を変更
  { 28, { 18, 44, 27, 29 } },   //!< 28: S    // @fix NMCat[2488] ↓への移動先を変更
  { 29, { 19, 45, 28, 20 } },   //!< 29: T    // @fix NMCat[2488] ↓への移動先を変更

  { 30, { 20, 36, 35, 31 } },   //!< 30: U
  { 31, { 21, 37, 30, 32 } },   //!< 31: V
  { 32, { 22, 38, 31, 33 } },   //!< 32: W
  { 33, { 23, 39, 32, 34 } },   //!< 33: X
  { 34, { 24, 40, 33, 35 } },   //!< 34: Y
  { 35, { 25, 41, 34, 30 } },   //!< 35: Z

  { UI::RET_DELETE, { 30,  0, 41, 41 } },   //!< 36: けす
  { UI::RET_DELETE, { 31,  1, 41, 41 } },   //!< 37: けす
  { UI::RET_DELETE, { 32,  2, 41, 41 } },   //!< 38: けす
  { UI::RET_DELETE, { 33,  3, 41, 41 } },   //!< 39: けす
  { UI::RET_DELETE, { 34,  4, 41, 41 } },   //!< 40: けす

  { UI::RET_END,    { 35,  5, 36, 36 } },   //!< 36: おわり
  { UI::RET_END,    { 26,  6, 36, 36 } },   //!< 37: おわり
  { UI::RET_END,    { 27,  7, 36, 36 } },   //!< 38: おわり
  { UI::RET_END,    { 28,  8, 36, 36 } },   //!< 39: おわり
  { UI::RET_END,    { 29,  9, 36, 36 } },   //!< 40: おわり
};

//! @brief    ボタンアニメ
enum {
  BTN_ANIM_TOUCH,
  BTN_ANIM_RELEASE,
  BTN_ANIM_CANCEL,
  BTN_ANIM_PASSIVE,
  BTN_ANIM_ACTIVE,
  BTN_ANIM_TOUCH_RELEASE,
  BTN_ANIM_NUM,
};

//! @brief    ボタンデータ
static const UI::BUTTON_DATA c_ButtonData[BUTTON_DATA_MAX] = {
  {  0, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_00, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_00_TOUCH },
  {  1, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_01, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_01_TOUCH },
  {  2, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_02, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_02_TOUCH },
  {  3, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_03, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_03_TOUCH },
  {  4, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_04, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_04_TOUCH },
  {  5, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_05, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_05_TOUCH },
  {  6, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_06, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_06_TOUCH },
  {  7, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_07, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_07_TOUCH },
  {  8, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_08, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_08_TOUCH },
  {  9, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_09, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_09_TOUCH },

  { 10, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_10, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_10_TOUCH },
  { 11, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_11, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_11_TOUCH },
  { 12, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_12, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_12_TOUCH },
  { 13, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_13, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_13_TOUCH },
  { 14, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_14, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_14_TOUCH },
  { 15, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_15, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_15_TOUCH },
  { 16, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_16, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_16_TOUCH },
  { 17, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_17, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_17_TOUCH },
  { 18, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_18, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_18_TOUCH },
  { 19, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_19, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_19_TOUCH },

  { 20, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_20, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_20_TOUCH },
  { 21, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_21, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_21_TOUCH },
  { 22, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_22, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_22_TOUCH },
  { 23, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_23, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_23_TOUCH },
  { 24, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_24, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_24_TOUCH },
  { 25, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_25, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_25_TOUCH },
  { 26, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_26, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_26_TOUCH },
  { 27, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_27, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_27_TOUCH },
  { 28, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_28, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_28_TOUCH },
  { 29, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_29, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_29_TOUCH },

  { 30, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_30, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_30_TOUCH },
  { 31, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_31, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_31_TOUCH },
  { 32, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_32, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_32_TOUCH },
  { 33, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_33, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_33_TOUCH },
  { 34, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_34, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_34_TOUCH },
  { 35, PANENAME_NUMBER_INPUT_LOW_PANE_WORD_35, PANENAME_STRINPUT_BTN_LOW_000_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__WORD_35_TOUCH },

  { 36, PANENAME_NUMBER_INPUT_LOW_PANE_BUTTON1_00, PANENAME_STRINPUT_BTN_LOW_002_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__BUTTON1_00_TOUCH },
  { 37, PANENAME_NUMBER_INPUT_LOW_PANE_BUTTON1_01, PANENAME_STRINPUT_BTN_LOW_002_PANE_BOUNDING_00, LA_NUMBER_INPUT_NUMBER_INPUT_LOW__BUTTON1_01_TOUCH },
};


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   man     UIデバイスマネージャ
 * @param   heap    ヒープ
 * @param   full    true = 数値+アルファベット, false = 数値のみ
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 */
//------------------------------------------------------------------------------
UI::UI( gfl2::ui::DeviceManager * man, app::util::Heap* heap, bool full, gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytMultiResID resID )
{
  CreateUI( man, heap );
  CreateCursorManager( full, heap );
  CreateButtonManager( lytwk, resID, full, heap );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
UI::~UI( void )
{
  DeleteButtonManager();
  DeleteCursorManager();
  DeleteUI();
}


//------------------------------------------------------------------------------
/**
 * @brief   IDからカーソル位置を設定
 *
 * @param   id    カーソル位置
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::SetCursorPosID( u32 id )
{
  for( u32 i=0; i<CURSOR_DATA_MAX; ++i )
  {
    if( c_CursorMoveData[i].id == id )
    {
      m_pCursorManager->SetPos( i );
      FUKUSHIMA_PRINT( "%s(%d) : pos[%d]\n", __FUNCTION__, id, i );
      return;
    }
  }

  GFL_ASSERT(0);
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置を取得
 *
 * @param   none
 *
 * @return  カーソル位置
 */
//------------------------------------------------------------------------------
u32 UI::GetCursorPos( void ) const
{
  return m_pCursorManager->GetPos();
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置のID取得
 *
 * @param   none
 *
 * @return  カーソル位置のID
 */
//------------------------------------------------------------------------------
u32 UI::GetCursorPosID( void ) const
{
  return m_pCursorManager->GetID();
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置から部品ペインを取得
 *
 * @param   none
 *
 * @return  部品ペイン
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytParts* UI::GetButtonParts( void )
{
  u32 pos = m_pCursorManager->GetPos();

  if( pos >= CURSOR_DATA_MAX )
  {
    GFL_ASSERT(0);
    pos = 0;
  }

  for( u32 i=0; i<BUTTON_DATA_MAX; ++i )
  {
    if( c_ButtonData[i].id == c_CursorMoveData[pos].id )
    {
      return m_pBtnParts[i];
    }
  }

  GFL_ASSERT(0);
  return m_pBtnParts[0];
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソル位置から境界ペインを取得
 *
 * @param   none
 *
 * @return  境界ペイン
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytPane* UI::GetButtonPane( void )
{
  u32 pos = m_pCursorManager->GetPos();

  if( pos >= CURSOR_DATA_MAX )
  {
    GFL_ASSERT(0);
    pos = 0;
  }

  for( u32 i=0; i<BUTTON_DATA_MAX; ++i )
  {
    if( c_ButtonData[i].id == c_CursorMoveData[pos].id )
    {
      return m_pBtnBoundPane[i];
    }
  }

  GFL_ASSERT(0);
  return m_pBtnBoundPane[0];
}


//------------------------------------------------------------------------------
/**
 * @brief   押さえているボタンIDを取得
 *
 * @param   none
 *
 * @return  ボタンID
 */
//------------------------------------------------------------------------------
u32 UI::GetHoldButtonID( void ) const
{
  return m_pButtonManager->GetHoldingButtonId();
}

//------------------------------------------------------------------------------
/**
 * @brief   アップデート
 *
 * @param   none
 *
 * @return  入力結果
 */
//------------------------------------------------------------------------------
u32 UI::Update( void )
{
  switch( m_pButtonManager->Update() )
  {
  case app::tool::ButtonManager::UPDATE_STATE_IDLE:       //!< 何もしない
    break;

  case app::tool::ButtonManager::UPDATE_STATE_HOLDING:    //!< ボタンを押さえている
    return RET_TOUCH;

  case app::tool::ButtonManager::UPDATE_STATE_SELECT_ANIME_PLAYING:   //!< ボタン選択アニメ再生中
  case app::tool::ButtonManager::UPDATE_STATE_CANCEL:     //!< ボタンがキャンセルされた
    return RET_NONE;

  case app::tool::ButtonManager::UPDATE_STATE_SELECTED:       //!< ボタンが選択された
    return m_pButtonManager->GetSelectedButtonId();
  }

  app::tool::CursorManager::MOVE_RET ret = m_pCursorManager->MainRepeat();

  // 移動
  if( ret == app::tool::CursorManager::RET_MOVE )
  {
    return RET_CURSOR_MOVE;
  }

  if( m_pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    return RET_DECIDE_KEY;
  }

  if( m_pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    return RET_DELETE_KEY;
  }

  return RET_NONE;
}


//------------------------------------------------------------------------------
/**
 * @brief   UI関連作成
 *
 * @param   man     UIデバイスマネージャ
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateUI( gfl2::ui::DeviceManager* man, app::util::Heap* heap )
{
  m_pUIDeviceManager = man;
  m_pButton     = m_pUIDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
  m_pKey        = m_pUIDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );
  m_pStick      = m_pUIDeviceManager->GetStick( gfl2::ui::DeviceManager::STICK_STANDARD );
  m_pTouchPanel = m_pUIDeviceManager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

#ifdef NUMBER_INPUT_USE_UI_GYROSCOPE
  // ジャイロ
  m_pUIDeviceManager->BootGyroscope( heap->GetDeviceHeap(), gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
  m_pGyroscope = m_pUIDeviceManager->GetGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
  m_pGyroscope->SetZeroDriftMode( gfl2::ui::Gyroscope::ZERO_DRIFT_MODE_TIGHT );
  m_pGyroscope->DisableRevisionByAcceleration();
  m_pGyroscope->DisableZeroDrift();
#endif // NUMBER_INPUT_USE_UI_GYROSCOPE

#ifdef NUMBER_INPUT_USE_UI_ACCELEROMETER
  // 加速度センサー
  m_pUIDeviceManager->BootAccelerometer( heap->GetDeviceHeap(), gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
  m_pAccelerometer = m_pUIDeviceManager->GetAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
#endif // NUMBER_INPUT_USE_UI_ACCELEROMETER
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
#ifdef NUMBER_INPUT_USE_UI_ACCELEROMETER
m_pUIDeviceManager->StopAccelerometer( gfl2::ui::DeviceManager::ACCELEROMETER_STANDARD );
#endif // NUMBER_INPUT_USE_UI_ACCELEROMETER

#ifdef NUMBER_INPUT_USE_UI_GYROSCOPE
m_pUIDeviceManager->StopGyroscope( gfl2::ui::DeviceManager::GYROSCOPE_STANDARD );
#endif // NUMBER_INPUT_USE_UI_GYROSCOPE
}

//------------------------------------------------------------------------------
/**
 * @brief   カーソルマネージャ作成
 *
 * @param   full    true = 数値+アルファベット, false = 数値のみ
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateCursorManager( bool full, app::util::Heap* heap )
{
  gfl2::heap::HeapBase* sysHeap = heap->GetSystemHeap();

  m_pCursorManager = GFL_NEW(sysHeap) app::tool::CursorManager( c_CursorMoveData, GFL_NELEMS(c_CursorMoveData) );

  if( !full )
  {
    const u32 offset    = 10;
    const u32 loopCount = offset + 26;

    // アルファベット部分を移動不可に
    for( u32 i=offset; i<loopCount; ++i )
    {
      m_pCursorManager->SetMoveEnable( i, false );
    }
  }
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
  GFL_DELETE m_pCursorManager;
}

//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャ作成
 *
 * @param   lytwk   レイアウトワーク
 * @param   resID   リソースID
 * @param   full    true = 数値+アルファベット, false = 数値のみ
 * @param   heap    ヒープ
 *
 * @return  none
 */
//------------------------------------------------------------------------------
void UI::CreateButtonManager( gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytMultiResID resID, bool full, app::util::Heap* heap )
{
  gfl2::heap::HeapBase* sysHeap = heap->GetSystemHeap();
  gfl2::heap::HeapBase* devHeap = heap->GetDeviceHeap();

  const u32 btnNum = GFL_NELEMS(c_ButtonData);
  const u32 numCnt = 10;
  const u32 alpCnt = 26;

  m_pButtonManager = GFL_NEW(sysHeap) app::tool::ButtonManager( devHeap, m_pUIDeviceManager, btnNum );
  m_pButtonManager->ChangeButtonProcessMode( app::tool::ButtonManager::BUTTON_PROCESS_MULTIPLE, devHeap );

  for( u32 i=0; i<btnNum; ++i )
  {
    m_pBtnParts[i]     = lytwk->GetPartsPane( c_ButtonData[i].parent );
    m_pBtnBoundPane[i] = lytwk->GetPane( m_pBtnParts[i], c_ButtonData[i].bounding, &resID );

    u32 anmID = c_ButtonData[i].topAnim;

    m_pButtonManager->AddButton(
      devHeap,
      c_ButtonData[i].id,
      lytwk,
      m_pBtnParts[i],
      m_pBtnBoundPane[i],
      anmID + BTN_ANIM_TOUCH,
      anmID + BTN_ANIM_RELEASE,
      anmID + BTN_ANIM_CANCEL,
      anmID + BTN_ANIM_TOUCH_RELEASE,
      anmID + BTN_ANIM_ACTIVE,
      anmID + BTN_ANIM_PASSIVE,
      NULL );

    if( !full )
    {
      if( i >= numCnt && i < (numCnt + alpCnt) )
      {
        m_pButtonManager->SetButtonPassive( c_ButtonData[i].id, true );
      }
    }
  }

  m_pButtonManager->SetNotifyTimming( app::tool::ButtonManager::NOTIFY_START_ANIME );
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
  GFL_DELETE m_pButtonManager;
}

GFL_NAMESPACE_END(NumberInput)
GFL_NAMESPACE_END(App)
