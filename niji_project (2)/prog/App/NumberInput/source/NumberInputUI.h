#if !defined( __NUMBER_INPUT_UI_H_INCLUDED__ )
#define __NUMBER_INPUT_UI_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    NumberInputUI.h
 * @date    2015/11/25 11:11:28
 * @author  fukushima_yuya
 * @brief   英数字入力アプリ：入力関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "NumberInputDef.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class ButtonManager;
class CursorManager;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(ui)
class DeviceManager;
class VectorDevice;
class Button;
class TouchPanel;
#ifdef NUMBER_INPUT_USE_UI_GYROSCOPE
class Gyroscope;
#endif // NUMBER_INPUT_USE_UI_GYROSCOPE
#ifdef NUMBER_INPUT_USE_UI_ACCELEROMETER
class Accelerometer;
#endif // NUMBER_INPUT_USE_UI_ACCELEROMETER
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( NumberInput )

//------------------------------------------------------------------------------
// @brief   UIクラス
//------------------------------------------------------------------------------
class UI
{
  GFL_FORBID_COPY_AND_ASSIGN( UI ); //!< コピーコンストラクタ＋代入禁止

public:
  //! @brief    アップデートの戻り値
  enum {
    RET_DELETE = BUTTON_DATA_MAX - 2,   //!< 削除
    RET_END,                            //!< 終了

    RET_CURSOR_MOVE,      //!< カーソル移動
    RET_TOUCH,            //!< ボタンをタッチしている
    RET_DECIDE_KEY,       //!< Aボタン
    RET_DELETE_KEY,       //!< Bボタン

    RET_NONE,             //!< 入力なし
  };

  //! @brief    ボタンデータ
  typedef struct {
    u32 id;
    gfl2::lyt::LytPaneIndex parent;
    gfl2::lyt::LytPaneIndex bounding;
    u32 topAnim;
  } BUTTON_DATA;

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   man     UIデバイスマネージャ
   * @param   heap    ヒープ
   * @param   full    true = 数値+アルファベット, false = 数値のみ
   * @param   lytwk   レイアウトワーク
   * @param   resID   リソースID
   */
  //------------------------------------------------------------------
  UI( gfl2::ui::DeviceManager * man, app::util::Heap* heap, bool full, gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytMultiResID resID );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~UI( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   IDからカーソル位置を設定
   *
   * @param   id    カーソル位置
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void SetCursorPosID( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置を取得
   *
   * @param   none
   *
   * @return  カーソル位置
   */
  //------------------------------------------------------------------
  u32 GetCursorPos( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置のID取得
   *
   * @param   none
   *
   * @return  カーソル位置のID
   */
  //------------------------------------------------------------------
  u32 GetCursorPosID( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置から部品ペインを取得
   *
   * @param   none
   *
   * @return  部品ペイン
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytParts* GetButtonParts( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置から境界ペインを取得
   *
   * @param   none
   *
   * @return  境界ペイン
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytPane* GetButtonPane( void );

  //------------------------------------------------------------------
  /**
   * @brief   押さえているボタンIDを取得
   *
   * @param   none
   *
   * @return  ボタンID
   */
  //------------------------------------------------------------------
  u32 GetHoldButtonID( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   アップデート
   *
   * @param   none
   *
   * @return  入力結果
   */
  //------------------------------------------------------------------
  u32 Update( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   UI関連作成
   *
   * @param   man     UIデバイスマネージャ
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void CreateUI( gfl2::ui::DeviceManager* man, app::util::Heap* heap );

  //------------------------------------------------------------------
  /**
   * @brief   UI関連削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void DeleteUI( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルマネージャ作成
   *
   * @param   full    true = 数値+アルファベット, false = 数値のみ
   * @param   heap    ヒープ
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void CreateCursorManager( bool full, app::util::Heap* heap );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルマネージャ削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void DeleteCursorManager( void );

  //------------------------------------------------------------------
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
  //------------------------------------------------------------------
  void CreateButtonManager( gfl2::lyt::LytWk* lytwk, gfl2::lyt::LytMultiResID resID, bool full, app::util::Heap* heap );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャ削除
   *
   * @param   none
   *
   * @return  none
   */
  //------------------------------------------------------------------
  void DeleteButtonManager( void );

private:
  gfl2::ui::DeviceManager*    m_pUIDeviceManager;     //!< 入力デバイス管理オブジェクト

  gfl2::ui::VectorDevice*     m_pKey;                 //!< 十字キー

  gfl2::ui::VectorDevice*     m_pStick;               //!< アナログスティック

  gfl2::ui::Button*           m_pButton;              //!< ボタン

  gfl2::ui::TouchPanel*       m_pTouchPanel;          //!< タッチパネル

#ifdef NUMBER_INPUT_USE_UI_GYROSCOPE
  gfl2::ui::Gyroscope*        m_pGyroscope;           //!< ジャイロ
#endif // NUMBER_INPUT_USE_UI_GYROSCOPE
#ifdef NUMBER_INPUT_USE_UI_ACCELEROMETER
  gfl2::ui::Accelerometer*    m_pAccelerometer;       //!< 加速度センサー
#endif // NUMBER_INPUT_USE_UI_ACCELEROMETER

  app::tool::ButtonManager*   m_pButtonManager;       //!< ボタンマネージャ

  app::tool::CursorManager*   m_pCursorManager;       //!< カーソルマネージャ

  gfl2::lyt::LytParts*        m_pBtnParts[BUTTON_DATA_MAX];           //!< ボタンパーツ

  gfl2::lyt::LytPane*         m_pBtnBoundPane[BUTTON_DATA_MAX];       //!< ボタン境界ペイン
};

GFL_NAMESPACE_END( NumberInput )
GFL_NAMESPACE_END( App )

#endif // __NUMBER_INPUT_UI_H_INCLUDED__