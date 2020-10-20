#if !defined( __RESORT_LOWER_UI_DRAW_H__ )
#define __RESORT_LOWER_UI_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerUIDraw.h
 * @date    2015/09/08 20:00:50
 * @author  fukushima_yuya
 * @brief   ポケリゾート：UI画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortSequenceListener;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class FingerCursor;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   UI画面(下)の描画クラス
//------------------------------------------------------------------------------
class LowerUIDraw
  : public LowerBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerUIDraw);

public:
  // 表示タイプ
  enum DispType
  {
    DISP_TYPE_MAIN,
    DISP_TYPE_BOX_POKEMON_EVENT,    //!< ポケモンイベント
    DISP_TYPE_WILD_POKEMON_EVENT,   //!< ポケモンイベント
    DISP_TYPE_MASK,                 //!< 下画面マスクのみ
    DISP_TYPE_INPUT_WAIT_NO_MASK,   //!< 入力待ち(マスクなし)
    DISP_TYPE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerUIDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerUIDraw( void ) { ; }


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ数の更新
   */
  //------------------------------------------------------------------
  void UpdateBeansCount( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの更新
   */
  //------------------------------------------------------------------
  void UpdateButton( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   一時的にボタンを無効化
   *
   * @param   except    除外ボタンID
   */
  //------------------------------------------------------------------
  void TempDisableBtn( u32 except );
  
  //------------------------------------------------------------------
  /**
   * @brief   無効化ボタンを復帰
   */
  //------------------------------------------------------------------
  void ReturnDisableBtn( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   表示タイプの設定
   * @param   type    表示タイプ
   */
  //------------------------------------------------------------------
  void SetDisplayType( u8 type );

public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートの状態を確認
   */
  //------------------------------------------------------------------
  void CheckResort( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   チュートリアル矢印の表示
   *
   * @param   progID    進行度ID
   * @param   x         X座標
   * @param   y         Y座標
   */
  //------------------------------------------------------------------
  void VisibleTutorialArrow( u8 progID, int x = 0, int y = 0 );

  //------------------------------------------------------------------
  /**
   * @brief   チュートリアル矢印を非表示
   */
  //------------------------------------------------------------------
  void InvisibleTurorialArrow( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   シーケンスリスナーのセット
   *
   * @param   listener    リスナー
   */
  //------------------------------------------------------------------
  void SetSequenceListener( ResortSequenceListener* listener ) { m_pSeqListener = listener; }

private:   //!< app::ui::UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );

  //------------------------------------------------------------------
  /**
   * @brief   ペイン選択アニメーションの開始通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneID );


private:   //!< app::tool::ButtonManager
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの個別セットアップ
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* btnMan );


public:
  enum BtnID
  {
    BTN_ID_CANCEL,
    BTN_ID_MENU,
    BTN_ID_ARROW_L,
    BTN_ID_ARROW_R,
    BTN_ID_EVENT_STATUS,
    BTN_ID_NUM,
  };

private:
  static const u8 BEANS_KIND_NUM = 3;   //!< ポケマメの種類数

private:
  app::tool::FingerCursor*      m_pFingerCursor;

  ResortSequenceListener*       m_pSeqListener;

private:
  u8      m_DispType;

  u32     m_BeansCount[BEANS_KIND_NUM];

  void*   m_pCursorBuff;

  bool    m_ProgressFlag_TouchBox;

  bool    m_ProgressFlag_Menu;

  u32     m_TempDisableBtnID[BTN_ID_NUM];

  bool    m_TempDisableBtn;

private:
  gfl2::lyt::LytPane*         m_pBrackBG;

  gfl2::lyt::LytPane*         m_pPaneVisible;

  gfl2::lyt::LytTextBox*      m_pBeansCountText[BEANS_KIND_NUM];
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_LOWER_UI_DRAW_H__
