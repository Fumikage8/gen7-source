//==============================================================================
/**
 * @file	  BoxUppFolderDraw.h
 * @brief	  フォルダーの描画
 * @author	k.ohno
 * @date	  2015.08.15
 */
// =============================================================================

#if !defined( __BOX_UPP_FOLDER_DRAW_H__ )
#define	__BOX_UPP_FOLDER_DRAW_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "BoxUppFolderDrawListener.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"

#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"

//#include "BoxPartsDraw.h"


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )
class BoxAppSetup;

/// タイトル用ビュー
class BoxUppFolderDraw : public app::ui::UIView,  public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxUppFolderDraw);

private:
  enum
  {
    LYT_WORK_NORMAL,
    LYT_WORK_MAX,
  };

  static const int TEMP_STRBUF_LENGTH = 200;
  
  /// レイアウトID
  typedef enum{
    LAYOUT_ID_UPPER,
    LAYOUT_ID_LOWER,
    LAYOUT_ID_MAX,
  }LAYOUT_ID;

  /// 上画面アニメ
  typedef enum{
    ANMID_UPPER_IN = 0,     ///< タイトルイン
    ANMID_UPPER_LOOP,       ///< 背景ループ
    ANMID_UPPER_OUT,        ///< タイトルアウト

    ANMID_UPPER_MAX,
  }ANIMID_UPPER;

  /// 下画面アニメ
  typedef enum{
    ANMID_LOWER_IN = 0,     ///< タイトルイン
    ANMID_LOWER_LOOP,       ///< 背景ループ
    ANMID_LOWER_OUT,        ///< タイトルアウト

    ANMID_LOWER_MAX,
  }ANIMID_LOWER;



  typedef enum{
    BTN_ID_CANCEL = 0,     ///< タイトルイン
    BTN_ID_Y,           ///< 
    BTN_ID_BOXLIST,        ///< 
  } BUTTON_ID_NUM;


public:

  /**
   * @brief コンストラクタ
   * @param heap  ヒープ
   */
  BoxUppFolderDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  /// デストラクタ
  virtual ~BoxUppFolderDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetUIListener( BoxUppFolderDrawListener *listener );

public:
  /// 更新
  virtual void Update( void );
  /// 描画
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  bool IsExit( void );


public:

  void Init(APP_PARAM* pAppParam, void* pAppLytBuff, BoxAppSetup* pBoxAppSetup);

  bool End(void);
  //----------------------------------------------------------------------
  /**
   *	@brief  テキスト表示
   *  @param messageid
   */
  //----------------------------------------------------------------------
  void SetMessage( int msgid );

  //----------------------------------------------------------------------
  /**
   *	@brief   noにしたがいカーソル表示
   *  @param   no    現在のカーソル位置
   *  @param   udlr  上下左右のいずれか
   *  @return  カーソル位置
   */
  //----------------------------------------------------------------------
  int SetCursorPos(int no, int udlr, int mode );
  
  void SetPokemonPanelVisible(bool bvisible);
private:
  //----------------------------------------------------------------------
  /**
   *	@brief  2D関連作成
   *  @param  heap    ヒープ
   */
  //----------------------------------------------------------------------
  void Initialize2D( app::util::Heap *heap, void* pAppLytBuff );

  //----------------------------------------------------------------------
  /**
   *	@brief  初期テキスト表示
   */
  //----------------------------------------------------------------------
  void InitializeText( app::util::Heap *heap );
  //----------------------------------------------------------------------
  /**
   *	@brief  初期テキスト表示
   */
  //----------------------------------------------------------------------
  void FinalizeText( void );

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectMenu
   * @brief   項目選択時のリスナー
   * @date    2015.03.10
   *
   * @param   id  メニュー項目のID
   */
  //-----------------------------------------------------------------------------
  virtual void SelectMenu( u32 id );



  void MyCreateButtonManager( void );
  void SetDispActive(bool bActive);
  void CarryStart(void);


  
public:
  void DrawMessageLine1(int messageno);
  void MessageLine1Close(void);
  void DrawPokeIcon(int pos, App::Box::PokemonIconDraw* pPara);

  void DrawTitle(int boxpos);
  
private:

  void DrawPokeIconState(int pos, App::Box::PokemonIconDraw* pPkd);


  app::util::Heap*          mpAppHeap;           ///< ヒープ
  BoxUppFolderDrawListener*        m_uiListener;     ///< UIイベントリスナー
  app::util::AppRenderingManager* mpAppRenderingManager;
  gfl2::str::MsgData* mpMsgData;       ///< テストメッセージデータ
  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf2;
	print::WordSet* mpWordSet;

  APP_PARAM* mpAppParam;
	BoxAppSetup* mpBoxAppSetup;

//  BoxPartsDraw mBoxPartsDraw;
  
  int mKeepCursorPosVertical;   //垂直方向で覚えておく位置  
  int mKeepCursorPosHorizontal;  //水平方向で覚えておく位置

  
};



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




#endif	/* __BOX_UPP_FOLDER_DRAW_H__ */

