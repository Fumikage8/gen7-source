#if !defined( __RESORT_LOWER_BASE_DRAW_H_INCLUDED__ )
#define __RESORT_LOWER_BASE_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ResortLowerBaseDraw.h
 * @date    2016/02/08 20:24:34
 * @author  fukushima_yuya
 * @brief   ポケリゾート：下画面Drawのベースクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortDrawListener;
class DrawerBase;
class ResortInitLogic;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( Savedata )
class ResortSave;
class BoxPokemon;
class MyItem;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：下画面Drawのベースクラス
//------------------------------------------------------------------------------
class LowerBaseDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerBaseDraw);

public:
  // 生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                lytHeap;
    app::util::Heap*                iconHeap;
    app::util::AppRenderingManager* renderMan;
    ResortInitLogic*                initLogic;
    Savedata::ResortSave*           resortSave;
    Savedata::BoxPokemon*           boxPokeSave;
    Savedata::MyItem*               itemSave;
    
    CreateParam( void )
      : lytHeap(NULL)
      , iconHeap(NULL)
      , renderMan(NULL)
      , initLogic(NULL)
      , resortSave(NULL)
      , boxPokeSave(NULL)
      , itemSave(NULL)
    { ; }
  };
  // 初期化パラメータ
  struct InitParam
  {
    void*                 lytBuff;    //!< リゾートレイアウト
    gfl2::str::MsgData*   msgSys;     //!< resort_system
    gfl2::str::MsgData*   msgWnd;     //!< resort_window
    gfl2::str::MsgData*   msgItem;    //!< item_info
    print::WordSet*       wordSet;
    
    InitParam( void )
      : lytBuff(NULL)
      , msgSys(NULL)
      , msgWnd(NULL)
      , msgItem(NULL)
      , wordSet(NULL)
    { ; }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerBaseDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerBaseDraw( void ) { ; }


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

public:
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
  
public:
  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   *
   * @param   listener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( ResortDrawListener* listener ) { m_uiListener = listener; }


protected:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param ) = 0;
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param ) = 0;
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void ) { return true; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  virtual void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  virtual void Close( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   リザルトの取得
   *
   * @param   リザルトID：定義 ResortConst.h enum LowerResult
   */
  //------------------------------------------------------------------
  u8 GetResult( void ) const { return m_Result; }

protected:
  //------------------------------------------------------------------
  /**
   * @brief   リザルトの初期化
   */
  //------------------------------------------------------------------
  void InitResult( void );


protected:  // app::ui::UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID )
  {
    return ENABLE_ACCESS;
  }

  //------------------------------------------------------------------
  /**
   * @brief   ペイン選択アニメ開始通知用リスナー
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual void OnLayoutPaneStartingAnimeEvent( const u32 paneID ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   キー入力通知用リスナ
   *
   * @param   btn
   * @param   key
   * @param   stick
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* btn, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick )
  {
    return ENABLE_ACCESS;
  }


protected:   //!< app::tool::ButtonManager
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   *
   * @param   defInfo     ボタンインフォのデフォルトテーブル
   * @param   baseTable   ペインテーブル
   * @param   boundTable  ペインテーブル
   * @param   btnNum      ボタンの数
   */
  //------------------------------------------------------------------
  void CreateButtonManager( const app::ui::ButtonInfoEx* defInfo, const u32* baseTable, const u32* boundTable, const u32 btnNum );
  
  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの生成
   *
   * @param   defInfo     ボタンインフォのデフォルトテーブル
   * @param   baseTable   ペインテーブル
   * @param   partsTable  ペインテーブル
   * @param   boundTable  ペインテーブル
   * @param   btnNum      ボタンの数
   */
  //------------------------------------------------------------------
  void CreateButtonManager( const app::ui::ButtonInfoEx* defInfo, const u32* baseTable, const u32* partsTable, const u32* boundTable, const u32 btnNum );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの個別セットアップ
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* btnMan ) { ; }

protected:
  app::util::Heap*                  m_pLytHeap;

  app::util::Heap*                  m_pIconHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  ResortInitLogic*                  m_pInitLogic;

  Savedata::ResortSave*             m_pResortSave;
  
  Savedata::BoxPokemon*             m_pBoxPokeSave;
  
  Savedata::MyItem*                 m_pItemSave;

  ResortDrawListener*               m_uiListener;

protected:
  DrawerBase*     m_pDrawerBase;

protected:
  u8    m_Result;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_LOWER_BASE_DRAW_H_INCLUDED__