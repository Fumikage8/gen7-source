#if !defined( __APP_STATUS_WAZA_EXCHANGE_H_INCLUDED__ )
#define __APP_STATUS_WAZA_EXCHANGE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusWazaExchange.h
 * @date    2015/12/09 15:35:48
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：技入れ替え(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <Layout/include/gfl2_Layout.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <ui/include/gfl2_UI.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class StatusDrawListener;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   構造体
//------------------------------------------------------------------------------
//! @brief  プレートデータ
struct PlateData
{
  gfl2::lyt::LytParts*  parts;    //!< プレート部品パーツ
  gfl2::lyt::LytPane*   bound;    //!< 境界ペイン
  u32                   defIdx;   //!< 現在のプレートの位置
  gfl2::math::VEC3      defPos;   //!< 初期位置
  bool                  excFlg;   //!< 入れ替えフラグ
  bool                  dragFlg;  //!< ドラッグフラグ
  gfl2::math::VEC3      moveVec;  //!< 移動量
  bool                  moveSet;  //!< 移動量の設定フラグ
};


//------------------------------------------------------------------------------
// @brief   技入れ替え(下)リスナー
//------------------------------------------------------------------------------
class WazaExchangeListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   プレートデータの設定
   *
   * @param   plateDataTbl    プレートデータ配列
   */
  //------------------------------------------------------------------
  virtual void SetPlateData( PlateData* plateDataTbl ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  virtual void SetVisibleCursor( bool visible ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示位置の設定
   *
   * @param   pos   カーソル位置
   */
  //------------------------------------------------------------------
  virtual void SetPosCursor( u32 pos ) { ; }
};

//------------------------------------------------------------------------------
// @brief   技入れ替え(下)説明表示更新リスナー
//------------------------------------------------------------------------------
class WazaExchangeExplainListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   説明画面の表示を更新
   *
   * @param   wazaNo    技No
   */
  //------------------------------------------------------------------
  virtual void SetWazaExplain( WazaNo wazaNo ) { ; }
};

//------------------------------------------------------------------------------
// @brief   技入れ替え(下)メッセージリスナー
//------------------------------------------------------------------------------
class WazaExchangeMsgListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージIDの設定
   *
   * @param   msgID   メッセージID
   */
  //------------------------------------------------------------------
  virtual void SetMessageID( const u32 msgID ) { ; }
};

//------------------------------------------------------------------------------
// @brief   技入れ替え(下)
//------------------------------------------------------------------------------
class WazaExchange
{
  GFL_FORBID_COPY_AND_ASSIGN( WazaExchange );

public:
  //! @brief  初期化パラメータ
  struct InitParam
  {
    WazaExchangeListener*           curListener;
    WazaExchangeMsgListener*        msgListner;
    WazaExchangeExplainListener*    expListener;
    StatusDrawListener*             uiListener;
  };

public:
  // シーケンス
  enum {
    SEQ_INIT,
    SEQ_IDLE,
    SEQ_DEST,
    SEQ_EXCHANGE,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  WazaExchange( DrawerBase* base );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~WazaExchange( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   initParam   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize( InitParam& initParam );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   対象のプレートをセット
   *
   * @param   plateIndex    初期のプレートインデックス
   * @param   drag_flag     ドラッグでの入れ替えかどうか
   */
  //------------------------------------------------------------------
  void SetPlate_Init( u32 plateIndex, bool dragFlag = false );

  //------------------------------------------------------------------
  /**
   * @brief   対象のプレートをセット
   *
   * @param   plateIndex    現在のプレートインデックス
   * @param   drag_flag     ドラッグでの入れ替えかどうか
   */
  //------------------------------------------------------------------
  void SetPlate_Now( u32 plateIndex, bool dragFlag = false );

  //------------------------------------------------------------------
  /**
   * @brief   入れ替えの終了
   */
  //------------------------------------------------------------------
  void End( void );

  //------------------------------------------------------------------
  /**
   * @brief   パラメータのリセット
   */
  //------------------------------------------------------------------
  void Reset( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   対象のプレートをセット
   *
   * @param   plateIndex    プレートインデックス
   * @param   drag_flag     ドラッグでの入れ替えかどうか
   */
  //------------------------------------------------------------------
  void SetPlate( u32 plateIndex, bool dragFlag );

private:
  //------------------------------------------------------------------
  /**
   * @brief   入れ替え
   */
  //------------------------------------------------------------------
  void Exchange( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   入れ替えの終了判定
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsExchange( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期位置へバインド
   */
  //------------------------------------------------------------------
  void UpdateBind( void );

  //------------------------------------------------------------------
  /**
   * @brief   初期位置へバインド
   *
   * @param   plateIndex    プレートインデックス
   */
  //------------------------------------------------------------------
  void _UpdateBind( u32 plateIndex );

private:
  //------------------------------------------------------------------
  /**
   * @brief   親ビューの入力切替
   *
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  void SetParentInputEnable( bool enable );

private:
  //------------------------------------------------------------------
  /**
   * @brief   プレート選択アニメの開始
   *
   * @param   plateIndex    対象プレートインデックス
   */
  //------------------------------------------------------------------
  void StartPlateSelectAnim( u32 plateIndex );

  //------------------------------------------------------------------
  /**
   * @brief   プレート選択解除アニメの開始
   *
   * @param   plateIndex    対象プレートインデックス
   */
  //------------------------------------------------------------------
  void StartPlateUnselectAnim( u32 plateIndex );

private:
  //------------------------------------------------------------------
  /**
   * @brief   子ペインの追加
   *
   * @param   plateIndex   対象のプレートインデックス
   * @param   appendPos    追加する場所の指定
   */
  //------------------------------------------------------------------
  void AppendChildPane( u32 plateIndex, u8 appendPos );

public:
  //------------------------------------------------------------------
  /**
   * @brief   技の取得
   *
   * @param   plateIndex    現在のプレートインデックス
   *
   * @return  技No
   */
  //------------------------------------------------------------------
  WazaNo GetWazaNo( u32 plateIndex );

  //------------------------------------------------------------------
  /**
   * @brief   初期プレートインデックスの取得
   *
   * @param   plateIndex    現在のプレートインデックス
   *
   * @return  初期インデックス
   */
  //------------------------------------------------------------------
  u32 GetPlateIndex_Init( u32 plateIndex ) const;

  //------------------------------------------------------------------
  /**
   * @brief   現在のプレートインデックスの取得
   *
   * @param   plateIndex    初期プレートインデックス
   *
   * @return  現在のインデックス
   */
  //------------------------------------------------------------------
  u32 GetPlateIndex_Now( u32 plateIndex ) const;


//------------------------------------------------------------------------------
// @brief   タッチ処理関連
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief   タッチパネルのセット
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  "PLARE_MAX未満 = 入れ替え開始"
   * @return  "PLATE_MAX以上 = それ以外"
   */
  //------------------------------------------------------------------
  u32 SetTouchPanel( gfl2::ui::TouchPanel* touchPanel );

  //------------------------------------------------------------------
  /**
   * @brief   ドラッグ中かどうか
   *
   * @return  "true  = ドラッグ中"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsPlateDrag( void ) const { return m_isDrag; }

  //------------------------------------------------------------------
  /**
   * @brief   タッチしているプレートのインデックスの取得
   *
   * @param   touchPanel    タッチパネル
   *
   * @return  タッチしているプレートのインデックス
   */
  //------------------------------------------------------------------
  u32 GetTouchPlateIndex( gfl2::ui::TouchPanel* touchPanel );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ドラッグによるプレートの座標更新
   */
  //------------------------------------------------------------------
  void UpdateDragPlate( void );


// @fix NMCat[363] ----- Begin
private:
  void StartExchange_Drag( u32 plateID );
  void EndExchange_Drag( void );

public:
  u32 TouchEvent( gfl2::ui::TouchPanel* touchPanel );

private:
  u32 TouchEvent_MoveSelect( gfl2::ui::TouchPanel* touchPanel );
  u32 TouchEvent_DestSelect( gfl2::ui::TouchPanel* touchPanel );

private:
  bool BasicTouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel );
  bool BasicTouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel );
  bool BasicTouchEvent_Release( gfl2::ui::TouchPanel* touchPanel );

private:
  bool PlateTouchEvent_Trigger( gfl2::ui::TouchPanel* touchPanel, u32* destID );
  bool PlateTouchEvent_Hold( gfl2::ui::TouchPanel* touchPanel, u32* destID );

private:
  gfl2::math::VEC3 GetTouchPos( gfl2::ui::TouchPanel* touchPanel );
  gfl2::math::VEC3 GetTouchPos_Layout( gfl2::ui::TouchPanel* touchPanel );

private:
  u32 GetTouchPlate( gfl2::ui::TouchPanel* touchPanel, u32 except = PLATE_MAX );
// @fix NMCat[363] ----- End

private:
  static const u8   PLATE_MAX = 4;

private:
  DrawerBase*                     m_pBase;

  WazaExchangeListener*           m_pCursorListener;

  WazaExchangeMsgListener*        m_pMsgWndListener;

  WazaExchangeExplainListener*    m_pExplainListener;

  StatusDrawListener*             m_uiListener;

  pml::pokepara::PokemonParam*    m_pPokeParam;

private:
  u8          m_Seq;

  PlateData   m_PlateData[PLATE_MAX];

  PlateData   m_DefPlateData[PLATE_MAX];

  u32         m_MoveIndex;

  u32         m_DestIndex;

  bool        m_isDrag;

  bool        m_isRelease;

  WazaNo      m_WazaNo;

  u32         m_SelectAnim;

  u32         m_TouchPlate;

private:
  gfl2::math::VEC3      m_TouchPos;

  gfl2::math::VEC3      m_TouchMarginPos;

private:  // @fix NMCat[363]
  u32   m_TriggerPlateID;

  u32   m_HoldPlateID;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_WAZA_EXCHANGE_H_INCLUDED__
