#if !defined( __APP_STATUS_RIBBON_H_INCLUDED__ )
#define __APP_STATUS_RIBBON_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusRibbon.h
 * @date    2015/12/04 13:21:01
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：リボン(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusInformation.h"

#include <AppLib/include/Tool/app_tool_ScrollPaneManager.h>
#include <PokeTool/include/PokeToolRibbon.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <ui/include/gfl2_UI.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
class MenuCursor;
class StatusDrawListener;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class ScrollBar;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   リボンリスナー
//------------------------------------------------------------------------------
class RibbonListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   タブのパッシブ
   */
  //------------------------------------------------------------------
  virtual void TabPassive( void ) { ; }

  //------------------------------------------------------------------
  /**
   * @brief   キャンセルボタンの有効設定
   *
   * @param   enable    有効フラグ
   */
  //------------------------------------------------------------------
  virtual void SetCancelBtnEnable( bool enable ) { ; }
};

//------------------------------------------------------------------------------
// @brief   リボン(下)
//------------------------------------------------------------------------------
class Ribbon
  : public app::tool::ScrollPaneManager::Listener
  , public InformationRibbonListener
{
  GFL_FORBID_COPY_AND_ASSIGN( Ribbon );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  Ribbon( DrawerBase* base, MenuCursor* cursor, gfl2::ui::DeviceManager* devMan, RibbonListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~Ribbon( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   listener    リスナー
   */
  //------------------------------------------------------------------
  void Initialize( StatusDrawListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void Setup( const pml::pokepara::PokemonParam* pp );

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
  void Draw( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   開く
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   閉じる
   */
  //------------------------------------------------------------------
  void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   スクロールペインのセット
   */
  //------------------------------------------------------------------
  void SetScrollPane( void );

  //------------------------------------------------------------------
  /**
   * @brief   所持リボンのセットアップ
   *
   * @param   pp      ポケモンパラメータ
   *
   * @return  所持リボンの数
   */
  //------------------------------------------------------------------
  u32 SetupHaveRibbon( const pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールペインマネージャの生成
   *
   * @param   ribbonNum   所持リボン数
   */
  //------------------------------------------------------------------
  void CreateScrollPaneManager( u32 ribbonNum );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールペインマネージャの破棄
   */
  //------------------------------------------------------------------
  void DeleteScrollPaneManager( void );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールペインマネージャ生成パラメータのセットアップ
   *
   * @param   desc        生成パラメータ
   * @param   ribbonNum   所持リボン数
   */
  //------------------------------------------------------------------
  void SetupDescription( app::tool::ScrollPaneManager::Description* desc, u32 ribbonNum );

private:
  //------------------------------------------------------------------
  /**
   * @brief   スクロールバーの生成
   */
  //------------------------------------------------------------------
  void CreateScrollBar( void );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールバーの破棄
   */
  //------------------------------------------------------------------
  void DeleteScrollBar( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   テクスチャの差し替え
   *
   * @param   paneIndex   ペインインデックス
   * @param   dataID      データID
   */
  //------------------------------------------------------------------
  void ReplaceTexture( u32 paneIndex, u32 dataID );

public:
  //------------------------------------------------------------------
  /**
   * @brief   入力切替
   *
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  void SetInputEnable( bool enable );

private:
  //------------------------------------------------------------------
  /**
   * @brief   入力切替
   *
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  void _SetInputEnable( bool enable );

public:
  //------------------------------------------------------------------
  /**
   * @brief   アナログキーの入力切替
   * 
   * @param   enable    入力フラグ
   */
  //------------------------------------------------------------------
  void SetInputKeyEnabled( bool enable );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を元に戻す
   */
  //------------------------------------------------------------------
  void SetEnableInput( void );

  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力を無効にする
   */
  //------------------------------------------------------------------
  void SetDisableInput( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisibleCursor( bool visible );

  //------------------------------------------------------------------
  /**
   * @brief   リボンIDの取得
   *
   * @param   index   インデックス
   *
   * @return  リボンID
   */
  //------------------------------------------------------------------
  u32 GetRibbonID( u32 index ) const { return m_RibbonKind[index]; }

  //------------------------------------------------------------------
  /**
   * @brief   最後に選択したリボンIDの取得
   *
   * @return  リボンID
   */
  //------------------------------------------------------------------
  u32 GetLastRibbonID( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   リボン説明関連
   */
  //------------------------------------------------------------------
  void UpdateRibbonExplain( u32 dataID );
  bool _OpenRibbonExplain( u32 ribbonID );
  void _SetRibbonExplain( u32 ribbonID );


  void _SetDispMode( u8 mode );
  u8 _GetDispMode( void ) const;

  void _SetInputExclusive( bool exclusive );
  bool _GetInputExclusive( void );

public:
  // 更新ステートの取得
  app::tool::ScrollPaneManager::State GetUpdateState( void );

private:  //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   リストの項目を描画通知
   *
   * @param   pane_index    選択されたボタンのインデックス
   * @param   dataID        選択されたデータのインデックス
   * @param   is_end_async  非同期の読み込み完了フラグ
   */
  //------------------------------------------------------------------
  virtual void WriteTexture( u32 pane_index, u32 dataID, bool is_end_async );

  //------------------------------------------------------------------
  /**
   * @brief   決定ボタンの押下通知
   *
   * @param   pane_index  選択されたボタンのインデックス
   * @param   dataID      選択されたデータのインデックス
   */
  //------------------------------------------------------------------
  virtual void Decide( u32 pane_index, u32 dataID );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの選択通知
   *
   * @param   pane_index  選択されたボタンのインデックス
   * @param   dataID      選択されたデータのインデックス
   */
  //------------------------------------------------------------------
  virtual void OnCursor( u32 pane_index, u32 dataID );

  //------------------------------------------------------------------
  /**
   * @brief   スクロールの通知
   *
   * @param   move    移動量
   */
  //------------------------------------------------------------------
  virtual void OnScroll( f32 move );

private:
  //------------------------------------------------------------------
  /**
   * @brief   リボンリストを開いているかどうか
   *
   * @return  "true  = 開いている"
   * @return  "false = 閉じている"
   */
  //------------------------------------------------------------------
  virtual bool IsRibbonListOpen( void ) const;

private:
  static const u32 RIBBON_BTN_NUM = 28;
  static const u32 BUTTON_NUM = BTN_ID_NUM;

private:
  DrawerBase*                               m_pBase;

  MenuCursor*                               m_pCursor;

  gfl2::ui::DeviceManager*                  m_pDevMan;

  app::tool::ScrollPaneManager*             m_pScrollPaneManager;

  app::tool::ScrollPaneManager::PaneInfo    m_InfoArray[RIBBON_BTN_NUM];

  app::util::ScrollBar*                     m_pScrollBar;

  StatusDrawListener*                       m_pListener;

  const pml::pokepara::PokemonParam*        m_pPokeParam;

  RibbonListener*                           m_pRibbonListener;

private:
  u32     m_RibbonKind[PokeTool::RIBBON_NO_MAX];

  u32     m_CursorPos;

  u32     m_LastRibbonIndex;

  u32     m_HaveRibbonNum;

  bool    m_isOpen;

  bool    m_InputExclusive;

  bool    m_BtnEnable[BUTTON_NUM];

private:
  gfl2::lyt::LytPicture*        m_pRibbonPane[RIBBON_BTN_NUM];
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_RIBBON_H_INCLUDED__
