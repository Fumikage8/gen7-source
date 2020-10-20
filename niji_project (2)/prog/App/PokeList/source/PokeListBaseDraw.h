#ifndef NIJI_PROJECT_APP_POKELIST_BASE_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_BASE_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListBaseDraw.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベースドロー
 */
//==============================================================================

// niji
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListDrawListener;   //!< ポケモンリストリスナークラス
class DrawerBase;
class StaticWork;
class LocalWork;
class PlateDraw;        //!< プレートクラス
class MessageWindow;    //!< メッセージウィンドウ
class PokeListWindow;   //!< ポケモンリストウィンドウ
class CursorDraw;       //!< カーソル
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数宣言
//------------------------------------------------------------------------------
enum Sequence {
  SEQ_INIT,
  SEQ_READY,
  SEQ_IN,
  SEQ_MAIN,
  SEQ_OUT,
  SEQ_END_READY,
  SEQ_END
};

//------------------------------------------------------------------------------
// @brief   ポケモンリストベースドロークラス
//------------------------------------------------------------------------------
class PokeListBaseDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListBaseDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListBaseDraw( app::util::Heap * heap, app::util::AppRenderingManager * renderingManager );

  //------------------------------------------------------------------
  /**
   * @brief	  デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListBaseDraw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( PokeListDrawListener* pListener );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   pStaticWork   スタティックワーク
   * @param   pLocalWork    ローカルワーク
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, StaticWork* pStaticWork, LocalWork* pLocalWork );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のオープン
   */
  //------------------------------------------------------------------
  void PokeListOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のクローズ
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  void PokeListClose( EndMode endMode = END_MODE_CANCEL );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの終了処理
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsEndCursor( void );

protected:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

protected:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool EndFunc( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   */
  //------------------------------------------------------------------
  virtual void UpdateFunc( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   描画処理
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupUI( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   ボタンのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* pBtnMan ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   UIのセットアップ完了待ち
   *
   * @return  "true  = 完了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  virtual bool IsSetupUIReady( void ) { return true; }

protected:
  //------------------------------------------------------------------
  /**
   * @brief   カーソル移動
   * @param   key
   */
  //------------------------------------------------------------------
  void MoveCursor( gfl2::ui::VectorDevice* key );

  //------------------------------------------------------------------
  /**
   * @brief   プレートの有効設定
   */
  //------------------------------------------------------------------
  void SetPlateEnable( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの再起動
   * @note    タッグ戦のみ使用
   */
  //------------------------------------------------------------------
  void RebootPokeList( App::PokeList::CONTEXT_PARAM& rContextParam );

protected:
  app::util::Heap*        m_pHeap;        //!< ヒープ
  
  PokeListDrawListener*   m_uiListener;   //!< UIイベントリスナー

  APP_PARAM*  m_pAppParam;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  bool  m_bCreatedNumFont;

  u32   m_LayoutID;

  u8    m_Seq;

  u8    m_MemberCount;

  DrawerBase*       m_pDrawerBase;

  StaticWork*       m_pStaticWork;

  LocalWork*        m_pLocalWork;

  // ポケモンプレート
  PlateDraw*        m_pPokePlate[pml::PokeParty::MAX_MEMBERS];

  // カーソル
  CursorDraw*       m_pCursor;

  // メッセージウィンドウ
  MessageWindow*    m_pMessageWindow;

  // ポケモンリストウィンドウ
  PokeListWindow*   m_pPokeWindow;

  app::util::KeyRepeatController*   m_pKeyRepeatLeft;
  app::util::KeyRepeatController*   m_pKeyRepeatRight;

  u32     m_CursorPosMaxX;
  u32     m_CursorPosMaxY;

  u32     m_CursorPosMaxRY;

  u32     m_PrevCurPosX;
  u32     m_PrevCurPosY;

private:
  bool    m_isEndCursor;

protected:
  bool    m_PlateEnable[pml::PokeParty::MAX_MEMBERS];
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // NIJI_PROJECT_APP_POKELIST_BASE_DRAW_H_INCLUDED
