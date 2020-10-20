//==============================================================================
/**
 * @file	  BoxLowFolderDraw.h
 * @brief	  フォルダーの描画
 * @author	k.ohno
 * @date	  2015.08.15
 */
// =============================================================================

#if !defined( __BOX_LOW_FOLDER_DRAW_H__ )
#define	__BOX_LOW_FOLDER_DRAW_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "BoxLowDrawListener.h"
#include "BoxLowFolderDrawListener.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"

#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )
class BoxAppSetup;

/// タイトル用ビュー
class BoxLowFolderDraw : public app::ui::UIView, public app::ui::UIInputListener, public app::tool::MenuWindowListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxLowFolderDraw);

private:
  enum
  {
    LYT_WORK_NORMAL,
    LYT_WORK_BG,
    LYT_WORK_MAX,
  };

  
  enum{
    FOLDER_MOUSE_MODE_NONE = 0,
    FOLDER_MOUSE_MODE_MOVE,
    FOLDER_MOUSE_MODE_POKEMON_DRAG,
		FOLDER_MOUSE_MODE_TUKAMI,
  };

  
  static const int TEMP_STRBUF_LENGTH = 200;
  static const int DIFFER_NUM=3;
  
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
  BoxLowFolderDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  /// デストラクタ
  virtual ~BoxLowFolderDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetUIListener( BoxLowFolderDrawListener *listener );
  //BOX下画面UIリスナー
  void SetUIDrawListener( BoxLowDrawListener *listener ){m_uiDrawListener=listener;};

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
  /// タッチパネルイベントの検知
  virtual ::app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  /// ペインアクション通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  /// キー入力通知用リスナ
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick);

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


  void AnimFolder( int boxno , int num , int active, int lv);

  void CarryStart(int page, int startx, int starty, int sizex, int sizey, int topcur, int num);
	void CarryEnd(void);
  void CursorChange(bool byoko );

  void SetFolderActive(int pos,bool bActive);

  void FolderIconMoveActive(int pos,bool bActive);
  
private:
  void FolderIconMove(void);
  void FolderIconDelete(void);
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

  int TouchFolderHit(int mode, u16 x, u16 y);


  void MyCreateButtonManager( void );
  void SetDispActive(bool bActive);

  void DispRangeNum(gfl2::math::VEC3* pvec);
  void ResetRangeNum(void );



public:
  bool GetTukamiMode(void){ return (mMousePokemonMode==FOLDER_MOUSE_MODE_POKEMON_DRAG); };
  void SetTukamiMode(void){ mMousePokemonMode = FOLDER_MOUSE_MODE_POKEMON_DRAG; }
  void MouseModeEnd(void);
  void DrawMessageLine1(int messageno);
  void MessageLine1Close(void);
  void InitBoxAnim(int cursortype);

  void LiftPokeIconStart0(void);  // kawa_box_modify  使用中のペインのテクスチャを貼り替えるので処理を2フレームに分ける
  void LiftPokeIconStart(PokeTool::SimpleParam* pPara);
  void LiftPokeIconEnd(void);
  void SetCursorVisible(bool bvisible );

  void SetSearchButtonPassive(void);
  void SetSearchButtonActive(void);

  void SetBoxListButtonPassive(void);
  void SetBoxListButtonActive(void);

  void SetButtonPos(int buttonpos);

  void StopMainCursor(void);
  void StartMainCursor(void);

  void ButtonReset(void);
  
private:
  void LiftPokeIconMove( gfl2::math::VEC3* pvec);
  gfl2::lyt::LytParts* GetFolderPane(int nextbutton);

  void InitTouchPos(void);
  int getFolderAnimNo(int num, int active);

  
  bool mbCarryPokemon;
  int mbCarryNum;
	int mButtonPos;

  int mMoveX;
  int mMoveY;
  int mMousePokemonMode;
	int mMouseIndex;
  int mMouseFirstX;
  int mMouseFirstY;

  bool mbDrawOn;
  
  app::util::Heap*          mpAppHeap;           ///< ヒープ
  BoxLowFolderDrawListener*        m_uiListener;     ///< UIイベントリスナー
  BoxLowDrawListener*        m_uiDrawListener;     ///< UIDrawイベントリスナー
  app::util::AppRenderingManager* mpAppRenderingManager;
  gfl2::str::MsgData* mpMsgData;       ///< テストメッセージデータ
  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf2;
	print::WordSet* mpWordSet;

  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;

  int mKeepCursorPosVertical;   //垂直方向で覚えておく位置  
  int mKeepCursorPosHorizontal;  //水平方向で覚えておく位置

  
};



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




#endif	/* __BOX_LOW_FOLDER_DRAW_H__ */

