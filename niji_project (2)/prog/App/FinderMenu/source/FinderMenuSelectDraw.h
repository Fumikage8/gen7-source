#if !defined( __FINDER_MENU_SELECT_DRAW_H__ )
#define __FINDER_MENU_SELECT_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuSelectDraw.h
 * @date    2015/11/02 19:29:42
 * @author  fukushima_yuya
 * @brief   選択画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/FinderMenu/include/FinderMenuAppParam.h>
#include <App/FinderMenu/source/parts/FinderMenuWindow.h>

#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class FinderMenuDrawListener;
class DrawerBase;
class MenuCursor;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class KeyRepeatController;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)
struct FinderPhotoSnap;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Savedata)
class PokeFinderSave;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   写真選択画面の描画
//------------------------------------------------------------------------------
class FinderMenuSelectDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public app::tool::MenuWindowListener
  , public MessageWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuSelectDraw );

public:
  enum SaveRes {
    SAVE_RES_SUCCESS,
    SAVE_RES_SHORTAGE,
    SAVE_RES_NO_SPACE,
    SAVE_RES_ERROR,
    SAVE_RES_NUM,
  };

public:
  struct CreateParam
  {
    app::util::Heap*                    heap;
    app::util::AppRenderingManager*     renderMan;
    MenuCursor*                         cursor;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuSelectDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuSelectDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( FinderMenuDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   appParam      外部設定パラメータ
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* appParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

private:
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
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルのセットアップ
   */
  //------------------------------------------------------------------
  void SetupCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルのセットアップ解除
   */
  //------------------------------------------------------------------
  void UnsetupCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   写真データのセットアップ
   *
   * @param   photoIdx    写真データインデックス
   */
  //------------------------------------------------------------------
  void SetupPhotoData( u32 photoIdx );

private:
  //------------------------------------------------------------------
  /**
   * @brief   SDカードへの保存処理
   *
   * @return  SaveRes
   */
  //------------------------------------------------------------------
  SaveRes SaveForSDCard( void );

  //------------------------------------------------------------------
  /**
   * @brief   SDカードへの保存結果に応じたメッセージの表示
   *
   * @param   result    保存結果
   */
  //------------------------------------------------------------------
  void SetSaveResultMessage( SaveRes );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   * @param   pKey
   * @param   pStick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  //------------------------------------------------------------------
  /**
   * @brief   項目選択時のリスナー
   * @param   id  メニュー項目のID
   */
  //------------------------------------------------------------------
  virtual void SelectMenu( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力設定
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  virtual void SetButtonEnable( bool enable );

public:   //!< ボタンマネージャ関連
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  app::util::Heap*                    m_pHeap;        //!< ヒープ

  FinderMenuDrawListener*             m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*     m_pAppRenderingManager;

  gfl2::str::MsgData*                 m_pMsgData;

  print::WordSet*                     m_pWordSet;

  APP_PARAM*                          m_pAppParam;

  Savedata::PokeFinderSave*           m_pFinderSave;

  u8    m_Seq;

private:
  DrawerBase*       m_pDrawerBase;

  MenuCursor*       m_pCursor;

  MessageWindow*    m_pMsgWindow;

  app::util::KeyRepeatController*           m_pKeyRepeat;

  Field::TrialModel::FinderPhotoSnap*       m_pPhotoSnap;

  f32               m_WaitFrame;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_SELECT_DRAW_H__