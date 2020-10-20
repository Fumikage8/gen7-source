#ifndef NIJI_PROJECT_APP_LANG_SELECT_DRAWER_BASE_H_INCLUDED
#define NIJI_PROJECT_APP_LANG_SELECT_DRAWER_BASE_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file LangSelectDrawerBase.h
 * @date 2015/07/23 15:41:30
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Tool/app_tool_PokeIcon.h>
#include <AppLib/include/Tool/AppToolItemIcon.h>

// gflib2
#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_LytSys.h>
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class ButtonManager;
class MenuCursor;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_BEGIN( util )
class G2DUtil;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_BEGIN( ui )
class TouchPanel;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( LangSelect )

//------------------------------------------------------------------------------
// @brief   描画共通処理
//------------------------------------------------------------------------------
class DrawerBase
{
  GFL_FORBID_COPY_AND_ASSIGN( DrawerBase );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  DrawerBase( app::ui::UIView* pUIView, app::util::Heap* pHeap, u32 layout_id, print::WordSet* pWordSet, gfl2::str::MsgData* pMsgData );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~DrawerBase( void );


public: //!< ペイン関連
  //------------------------------------------------------------------
  /**
   * @brief   ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPaneクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytPane* GetPane( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytTextBoxクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytTextBox* GetTextBox( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   境界ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytBoundingクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytBounding* GetBounding( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   ピクチャーペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPictureクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytPicture* GetPicture( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   部品ペインの取得
   * @param   index   ペインインデックス
   * @param   pParts  部品ペイン(デフォルト = NULL)
   * @return  gfl2::lyt::LytPartsクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytParts* GetParts( gfl2::lyt::LytPaneIndex index, gfl2::lyt::LytParts* pParts = NULL );


  //------------------------------------------------------------------
  /**
   * @brief   位置の設定
   * @param   pPane   対象ペイン
   * @param   pos     更新座標値
   */
  //------------------------------------------------------------------
  void SetPanePos( gfl2::lyt::LytPane* pPane, gfl2::math::Vector3 pos );

  //------------------------------------------------------------------
  /**
   * @brief   位置の取得
   * @param   pPane   対象ペイン
   * @return  現在座標値
   */
  //------------------------------------------------------------------
  gfl2::math::Vector3 GetPanePos( gfl2::lyt::LytPane* pPane );


  //------------------------------------------------------------------
  /**
   * @brief   ペインのアルファを変更
   * @param   pPane     変更対象ペイン
   * @param   alpha     アルファ値
   */
  //------------------------------------------------------------------
  void SetAlpha( gfl2::lyt::LytPane* pPane, u8 alpha );


  //------------------------------------------------------------------
  /**
   * @brief   ペインの表示切替
   * @param   pPane     切り替え対象ペイン
   * @param   viisible  表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisiblePane( gfl2::lyt::LytPane* pPane, bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   ペインの表示切替フラグの取得
   * @param   pPane     取得対象ペイン
   * @return  true = 表示中、false = 非表示
   */
  //------------------------------------------------------------------
  bool IsVisiblePane( gfl2::lyt::LytPane* pPane ) const;


  //------------------------------------------------------------------
  /**
   * @brief   ペインのタッチ判定
   */
  //------------------------------------------------------------------
  bool IsHoldPane( gfl2::ui::TouchPanel* pTouchPanel, gfl2::lyt::LytPane* pPane ) const;


public: //!< アニメーション関連
  //------------------------------------------------------------------
  /**
   * @brief   アニメーションの再生
   * @param   anmID         アニメーションID
   * @param   auto_anim     オートアニメーションするか(デフォルト = true)
   * @param   auto_unbind   終了時に自動でアンバインドするか(デフォルト = true)
   */
  //------------------------------------------------------------------
  void StartAnim( u32 anmID, bool auto_anim = true, bool auto_unbind = true );

  //------------------------------------------------------------------
  /**
   * @brief   アニメーションの停止
   * @param   anmID   アニメーションID
   * @param   clear   アニメの初期化フラグ(デフォルト = false)
   */
  //------------------------------------------------------------------
  void StopAnim( u32 anmID, bool clear = false );

  //------------------------------------------------------------------
  /**
   * @brief   アニメーションの終了判定
   * @param   anmID   アニメーションID
   */
  //------------------------------------------------------------------
  bool IsAnimEnd( u32 anmID );

  //------------------------------------------------------------------
  /**
   * @brief   アニメーションのフレームを設定
   * @param   anmID   アニメーションID
   * @param   frame   アニメフレーム
   */
  //------------------------------------------------------------------
  void SetAnimFrame( u32 anmID, f32 frame );

  //------------------------------------------------------------------
  /**
   * @brief   現在のアニメーションのフレームを取得
   *
   * @param   anmID   アニメーションID
   *
   * @return  現在のアニメーションフレーム
   */
  //------------------------------------------------------------------
  f32 GetAnimFrame( u32 anmID ) const;

  //------------------------------------------------------------------
  /**
   * @brief   アニメーションの切り替え
   * @param   stop    停止するアニメID
   * @param   start   再生するアニメID
   */
  //------------------------------------------------------------------
  void ChangeAnim( u32 stop, u32 start );


public: //!< 取得関連
  //------------------------------------------------------------------
  /**
   * @brief   UIViewの取得
   * return   app::ui::UIViewクラスポインタ
   */
  //------------------------------------------------------------------
  app::ui::UIView* GetUIView( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトリソースの取得
   * return   gfl2::lyt::LytResクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytRes* GetLytRes( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   レイアウトワークの取得
   * return   gfl2::lyt::LytWkクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::lyt::LytWk* GetLytWk( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ヒープの取得
   * return   app::util::Heapクラスポインタ
   */
  //------------------------------------------------------------------
  app::util::Heap* GetHeap( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   システムヒープの取得
   * return   gfl2::heap::HeapBaseクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::heap::HeapBase* GetSystemHeap( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   デバイスヒープの取得
   * return   gfl2::heap::HeapBaseクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::heap::HeapBase* GetDeviceHeap( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   G2DUtilの取得
   * @return  app::util::G2DUtilクラスポインタ
   */
  //------------------------------------------------------------------
  app::util::G2DUtil* GetG2D( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ボタンマネージャの取得
   * @return  app::tool::ButtonManagerクラスポインタ
   */
  //------------------------------------------------------------------
  app::tool::ButtonManager* GetButtonManager( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ワードセットの取得
   * @return  print::WordSetクラスポインタ
   */
  //------------------------------------------------------------------
//  print::WordSet* GetWordSet( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの取得
   * @return  gfl2::str::StrBufクラスポインタ
   */
  //------------------------------------------------------------------
//  gfl2::str::MsgData* GetMsgData( void ) const;

private:
  app::ui::UIView*    m_pUIView;
  app::util::Heap*    m_pHeap;
  u32   m_LayoutID;

private:
  gfl2::lyt::LytWk*         m_pLytWk;
  gfl2::lyt::LytMultiResID  m_ResID;

//  print::WordSet*       m_pWordSet;
//  gfl2::str::MsgData*   m_pMsgData;



//------------------------------------------------------------------------------
// @brief   ポケモンアイコン
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの生成
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  void CreatePokeIcon( app::tool::PokeIconToolEx::PokeIconToolParam param );
  
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの破棄
   */
  //------------------------------------------------------------------
  void DeletePokeIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンの取得
   * @return  app::tool::PokeIconToolのポインタ
   */
  //------------------------------------------------------------------
  app::tool::PokeIconToolEx* GetPokeIcon( void );


private:
  app::tool::PokeIconToolEx*    m_pPokeIcon;


//------------------------------------------------------------------------------
// @brief   アイテムアイコン
//------------------------------------------------------------------------------
public:
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの生成
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  void CreateItemIcon( app::tool::ItemIconToolEx::ItemIconToolParam param );
  
  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの破棄
   */
  //------------------------------------------------------------------
  void DeleteItemIcon( void );

  //------------------------------------------------------------------
  /**
   * @brief   アイテムアイコンの取得
   * @return  app::tool::ItemIconToolのポインタ
   */
  //------------------------------------------------------------------
  app::tool::ItemIconToolEx* GetItemIcon( void );


private:
  app::tool::ItemIconToolEx*    m_pItemIcon;
};



//------------------------------------------------------------------------------
// @brief   メニューカーソル
//------------------------------------------------------------------------------
class MenuCursor
{
  GFL_FORBID_COPY_AND_ASSIGN( MenuCursor );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap        ヒープ
   * @param   renderMan   レンダリングマネージャ
   */
  //------------------------------------------------------------------
  MenuCursor( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~MenuCursor( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの生成
   */
  //------------------------------------------------------------------
  bool CreateMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの破棄
   */
  //------------------------------------------------------------------
  bool DeleteMenuCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo no, u8 order = 128, u32 layerNo = 0 );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルのセットアップ
   * @param   g2d       G2DUtil
   * @param   paneTbl   カーソル配置ペイン配列
   * @param   paneCount カーソル配置ペイン数
   * @param   dispType  描画画面
   */
  //------------------------------------------------------------------
  void SetupCursor( app::util::G2DUtil* g2d, gfl2::lyt::LytPane* paneTbl[], const u32 paneCount, gfl2::lyt::DisplayType dispType );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルのアンセットアップ
   *
   * @note    レイアウトを破棄する前に呼ぶこと
   */
  //------------------------------------------------------------------
  void UnsetupCursor( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の設定
   * @param   lytwk   レイアウトワーク
   * @param   pos     位置
   */
  //------------------------------------------------------------------
  void SetCursorPos( gfl2::lyt::LytWk* lytwk, u32 pos );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の取得
   * @return  位置
   */
  //------------------------------------------------------------------
  u32 GetCursorPos( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルバッファの取得
   * @return  カーソルバッファ
   */
  //------------------------------------------------------------------
  void* GetCursorBuffer( void ) const;

private:
  app::util::Heap*                  m_pHeap;

  app::util::AppRenderingManager*   m_pAppRenderMan;

  app::tool::MenuCursor*      m_pMenuCursor;

  gfl2::lyt::LytPane**        m_pCursorPosPane;

  gfl2::lyt::DisplayType      m_DisplayType;

  void*     m_pCursorBuffer;

  u8        m_CreateSeq;

  u8        m_DeleteSeq;

  u32       m_CursorPosMax;

  u32       m_CursorPos;

  bool      m_bUnsetUp;
};

GFL_NAMESPACE_END( LangSelect )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_LANG_SELECT_DRAWER_BASE_H_INCLUDED