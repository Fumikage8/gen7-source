#if !defined( __APP_BOX_SEARCH_LOWER_DRAW_H_INCLUDED__ )
#define __APP_BOX_SEARCH_LOWER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchLowerDraw.h
 * @date    2015/07/27 12:58:33
 * @author  fukushima_yuya
 * @brief   ボックス検索：下画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/BoxSearch/include/BoxSearchAppParam.h>
#include <App/BoxSearch/source/BoxSearchConst.h>

#include <App/BoxSearch/source/parts/BoxSearchListBase.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )
class BoxSearchDrawListener;
class DrawerBase;
class ItemList;
class ContentsList;
class MenuCursor;
GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れ下画面描画
//------------------------------------------------------------------------------
class BoxSearchLowerDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
  , public SearchListListener
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSearchLowerDraw );

public:
  struct InitParam
  {
    APP_PARAM*          appParam;
    void*               lytBuff;
    gfl2::str::MsgData* msgData_Main;
    gfl2::str::MsgData* msgData_Initial;
    print::WordSet*     wordSet;
    MenuCursor*         cursor_item;
    MenuCursor*         cursor_cont;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxSearchLowerDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxSearchLowerDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief	  このビュー専用のUIリスナー設定
   *
   * @param   listener    UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( BoxSearchDrawListener* listener ) { m_uiListener = listener; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Init( InitParam& param );

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
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void BoxSearchOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void BoxSearchClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void InitializeClass( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの生成
   *
   * @param   initParam   初期化パラメータ
   */
  //------------------------------------------------------------------
  void CreatePaneList( InitParam& initParam );

public:
  //------------------------------------------------------------------
  /**
   * @brief   画面モードのセット
   * @param   dispMode    画面モード
   */
  //------------------------------------------------------------------
  void SetDispMode( DispMode dispMode );

  //------------------------------------------------------------------
  /**
   * @brief   検索条件選択の開始
   * @param   pos    検索条件項目ID
   */
  //------------------------------------------------------------------
  void StartSearchCondition( u32 pos );

private:
  //------------------------------------------------------------------
  /**
   * @brief   キーアクション
   */
  //------------------------------------------------------------------
  void KeyAction_Top( gfl2::ui::Button* button );
  void KeyAction_Cond( gfl2::ui::Button* button );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストの有効設定
   */
  //------------------------------------------------------------------
  void SetPaneListEnable_ItemList( bool enable );
  void SetPaneListEnable_ContentsList( bool enable );

  //------------------------------------------------------------------
  /**
   * @brief   ペインリストのリザルトの取得
   */
  //------------------------------------------------------------------
  app::tool::PaneList::Result GetPaneListResult( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンアクション
   */
  //------------------------------------------------------------------
  void ButtonSelectAction( u32 btnID );

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
   * @brief   ペイン選択アニメーションの開始の検知
   * @param   paneId
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneId );

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
   * @brief   リソース生成前に必要なデータを登録。フォントリソースなど
   */
  //------------------------------------------------------------------
  virtual void OnPreLoadResource( void );

public:   //!< 検索リストリスナー
  //------------------------------------------------------------------
  /**
   * @brief   検索項目リストの表示
   */
  //------------------------------------------------------------------
  virtual void OpenItemList( void );

  //------------------------------------------------------------------
  /**
   * @brief   検索内容リストの表示
   *
   * @param   srcID   検索項目ID
   */
  //------------------------------------------------------------------
  virtual void OpenContentsList( u32 srcID );

private:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   */
  //------------------------------------------------------------------
  void CreateButtonManager( void );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  BoxSearchDrawListener*            m_uiListener;   //!< UIイベントリスナー

  gfl2::Fade::FadeManager*          m_pFadeManager;

  APP_PARAM*                        m_pAppParam;
private:
  DrawerBase*         m_pDrawerBase;

  MenuCursor*         m_pCursorItem;

  MenuCursor*         m_pCursorContents;

  ItemList*           m_pItemList;

  ContentsList*       m_pContentsList;

  DispMode            m_DispMode;

  u8                  m_Seq;

  bool                m_InputExclusive;
};


GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __APP_BOX_SEARCH_LOWER_DRAW_H_INCLUDED__