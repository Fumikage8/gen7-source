#ifndef NIJI_PROJECT_APP_POKELIST_DRAWER_BASE_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_DRAWER_BASE_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListDrawerBase.h
 * @author  fukushima_yuya
 * @date    2015.03.09
 * @brief   共通データのアクセス用の基底クラス
 *          sango_project app_pokelist_DrawerBase
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/Tool/app_tool_PokeIcon.h>
#include <AppLib/include/Tool/AppToolItemIcon.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

// クラス宣言
class StaticWork;
class LocalWork;


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
  DrawerBase( app::ui::UIView* pUIView, app::util::Heap* pHeap, u32 layout_id, StaticWork* pStaticWork, LocalWork* pLocalWork );

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
   * @brief   レイアウトIDの取得
   * return   レイアウトID
   */
  //------------------------------------------------------------------
  u32 GetLytID( void ) const;

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
   * @brief   スタティックワークの取得
   * @return  StatickWorkクラスポインタ
   */
  //------------------------------------------------------------------
  StaticWork* GetStaticWork( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   ローカルワークの取得
   * @return  LocalWorkクラスポインタ
   */
  //------------------------------------------------------------------
  LocalWork* GetLocalWork( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   メッセージデータの取得
   * @return  gfl2::str::MsgDataクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::str::MsgData* GetMsgData( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   単語セットの取得
   * @return  print::WordSetクラスポインタ
   */
  //------------------------------------------------------------------
  print::WordSet* GetWordSet( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   StrBufの取得
   * @param   id  バッファID
   * @return  gfl2::str::StrBufクラスポインタ
   */
  //------------------------------------------------------------------
  gfl2::str::StrBuf* GetStrBuf( u8 id ) const;



private:
  app::ui::UIView*    m_pUIView;
  app::util::Heap*    m_pHeap;
  u32   m_LayoutID;

  StaticWork*   m_pStaticWork;
  LocalWork*    m_pLocalWork;

private:
  gfl2::lyt::LytWk*         m_pLytWk;
  gfl2::lyt::LytMultiResID  m_ResID;

private:
  static const u8   STR_BUF_NUM = 4;
  static const u16  CHAR_COUNT = 512;
  gfl2::str::StrBuf*    m_pStrBuf[STR_BUF_NUM];



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
// @brief   プロトタイプ宣言
//------------------------------------------------------------------------------
//--------------------------------------------------------------------
/**
 * @brief   プレートの取得
 * @param   pBase   DrawerBaseクラスポインタ
 * @param   cpos    プレートのインデックス(カーソル位置基準)
 */
//--------------------------------------------------------------------
gfl2::lyt::LytParts* GetPlateParts( DrawerBase* pBase, CursorPos cpos );



GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_DRAWER_BASE_H_INCLUDED
