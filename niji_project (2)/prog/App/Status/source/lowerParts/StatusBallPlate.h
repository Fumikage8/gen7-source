#if !defined( __APP_STATUS_BALL_PLATE_H_INCLUDED__ )
#define __APP_STATUS_BALL_PLATE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusBallPlate.h
 * @date    2015/12/03 16:44:41
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ボールプレート(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Status/include/StatusAppParam.h>

// pml
#include <pml/include/pmlib.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>
#include <ui/include/gfl2_UI.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
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
// @brief   ボールプレート(下)リスナー
//------------------------------------------------------------------------------
class BallPlateListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   タッチされたボールのインデックスを設定
   *
   * @param   index   タッチされたボールのインデックス
   */
  //------------------------------------------------------------------
  virtual void SetTouchedBallIndex( u32 index ) { ; }
};


//------------------------------------------------------------------------------
// @brief   ボールプレート(下)
//------------------------------------------------------------------------------
class BallPlate
{
  GFL_FORBID_COPY_AND_ASSIGN( BallPlate );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  BallPlate( DrawerBase* base );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~BallPlate( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   listener    リスナー
   * @param   mode        ステータス画面の表示モード
   */
  //------------------------------------------------------------------
  void Initialize( BallPlateListener* listener, Mode mode );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool Terminate( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   party   ポケパーティ
   */
  //------------------------------------------------------------------
  void Setup( const pml::PokeParty* party );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ：捕獲のボール
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup_GetBall( const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボールプレートのセットアップ
   *
   * @param   index     手持ちのインデックス
   * @param   pp        ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetupBallPlate( u32 index, const pml::pokepara::PokemonParam* pp );

private:
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの生成
   */
  //------------------------------------------------------------------
  void CreateItemIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの破棄
   */
  //------------------------------------------------------------------
  bool DeleteItemIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの差し替え
   */
  //------------------------------------------------------------------
  void ReplaceItemIcon( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   ボールインデックスの設定
   *
   * @param   index   ボールインデックス
   * @param   playSE  SEの再生フラグ(デフォルトはtrue)
   */
  //------------------------------------------------------------------
  void SetBallIndex( u32 index, bool playSE = true );

  //------------------------------------------------------------------
  /**
   * @brief   タッチ入力
   *
   * @param   touch   タッチパネル
   */
  //------------------------------------------------------------------
  void TouchBall( gfl2::ui::TouchPanel* touch );

private:
  // タッチ入力：トリガー
  void TouchBall_Trigger( gfl2::ui::TouchPanel* touch );
  // タッチ入力：ホールド
  void TouchBall_Hold( gfl2::ui::TouchPanel* touch );
  // タッチ入力：リリース
  void TouchBall_Release( gfl2::ui::TouchPanel* touch );
  // タッチ入力
  u32 _TouchBall( gfl2::ui::TouchPanel* touch );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボールアニメの開始
   *
   * @param   index   ボールインデックス
   */
  //------------------------------------------------------------------
  void StartBallAnim( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ボールアニメの更新
   */
  //------------------------------------------------------------------
  void UpdateBallAnim( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボールアイコンの変更
   */
  //------------------------------------------------------------------
  void UpdateBallIcon( void );


public:
  //! @brief  ボールの状態
  enum BallState {
    BALL_STATE_SELECT,
    BALL_STATE_UNSELECT,
    BALL_STATE_KEEP,
    BALL_STATE_NUM,
    BALL_STATE_NULL,
  };

  //! @brief  ボールの最大数
  static const u32 BALL_MAX = pml::PokeParty::MAX_MEMBERS;

private:
  DrawerBase*             m_pBase;

  BallPlateListener*      m_pListener;

  Mode                    m_StatusMode;

  u32                     m_MemberCount;

  u32                     m_SelectIndex;

  u32                     m_TriggerIndex;

  bool                    m_Disable;

  bool                    m_InputEnable;

  BallState               m_BallState[BALL_MAX];

private:
  u8      m_IconSeq;

  u32     m_BallID;

private:
  gfl2::lyt::LytParts*        m_pPlateParts;

  gfl2::lyt::LytPicture*      m_pBallPane[BALL_MAX];

  gfl2::lyt::LytPane*         m_pBallBound[BALL_MAX];

  gfl2::lyt::LytPicture*      m_pPicGetBallIcon;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_BALL_PLATE_H_INCLUDED__