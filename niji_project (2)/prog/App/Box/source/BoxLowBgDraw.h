//==============================================================================
/**
 * @file	  BoxLowBgDraw.h
 * @brief	  表示のサンプル
 * @author	k.ohno
 * @date	  January 18, 2015
 */
// =============================================================================

#if !defined( __BOX_LOW_BG_DRAW_H__ )
#define	__BOX_LOW_BG_DRAW_H__
#pragma once

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>

#include "BoxLowDrawListener.h"
#include "BoxLowBgDrawListener.h"
#include "BoxDrawPartsListener.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "App/Box/include/BoxAppParam.h"
#include "Print/include/WordSet.h"
#include "str/include/gfl2_Str.h"

#include "AppLib/include/Tool/app_tool_PaneListView.h"

#include "AppLib/include/Tool/app_tool_MenuWindowListener.h"

#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )
class BoxLowBattleInDraw;
class BoxAppSetup;

/// タイトル用ビュー
class BoxLowBgDraw : public app::ui::UIView, public app::ui::UIInputListener, public BoxDrawPartsListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxLowBgDraw);

  friend class BoxLowBattleInDraw;
  
private:
    enum
    {
      LYT_WORK_NORMAL=0,
      LYT_WORK_BG,
      LYT_WORK_PAPERTEX,
      LYT_WORK_MAX,
    };

  static const int TEMP_STRBUF_LENGTH = 400;

  static const int DIFFER_NUM=5;

  enum
  {
    TOUCH_PAIN_NONE,
    TOUCH_PAIN_L1,
    TOUCH_PAIN_L2,
    TOUCH_PAIN_R1,
    TOUCH_PAIN_R2,
    TOUCH_PAIN_FOLDER,
  };

  
  enum
  {
    BOX_MOUSE_MODE_NONE,
    BOX_MOUSE_MODE_NO_TOUCH,
    BOX_MOUSE_MODE_TOUCH,
    BOX_MOUSE_MODE_TUKAMI,
    BOX_MOUSE_MODE_FOLDER,
  };
  
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
    BTN_ID_X,
    BTN_ID_INFO,        ///< 
    BTN_ID_BOXLIST,        ///< 
		BTN_ID_CURTYPE_NORMAL,
		BTN_ID_CURTYPE_SHORTCUT,
		BTN_ID_CURTYPE_RANGE,
    BTN_ID_R1,
    BTN_ID_L1,
    BTN_ID_R2,
    BTN_ID_L2,
    BTN_ID_JUDGE,   ///<ジャッジ画面
    BTN_ID_CHAT_CHANGE,
    BTN_ID_CHAT,
    BTN_ID_BOXBAR,
  } BUTTON_ID_NUM;


public:
  // アイコン配置時に矢印ペインへ移動させるための定義
  enum IconPutArrow
  {
    ICON_PUT_ARROW_NONE = 0,    // なし（通常動作）
    ICON_PUT_ARROW_LEFT,        // 左矢印へ
    ICON_PUT_ARROW_RIGHT,       // 右矢印へ
  };


public:

  /**
   * @brief コンストラクタ
   * @param heap  ヒープ
   */
  BoxLowBgDraw( app::util::Heap * heap,app::util::AppRenderingManager* pAppRanderingManager );

  /// デストラクタ
  virtual ~BoxLowBgDraw();




  
public:
  /// このビュー専用のUIリスナー設定
  void SetUIListener( BoxLowBgDrawListener *listener );
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

  void Init(APP_PARAM* pAppParam, void* pAppLytBuff, BoxAppSetup* pBoxAppSetup, bool TouchPassive);

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
  int SetCursorPos(int no, int udlr );

  
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

  void MyCreateButtonManager( bool reload );
  void SetDispActive(bool bActive);

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   逃がすアニメ
   * @param   pos   位置
   * @retval  true  = 処理中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool PokeFreeAnime( int pos );

public:
  void DrawPokeIconBattle(int traypage);
  void BgChange(int wpNo);
//  void DrawPokeIcon(int pos, PokeTool::SimpleParam* pPara);
//  void DrawPokeIcon2(int pos, App::Box::PokemonIconDraw* pPkd);
  void DelPokeIcon(int pos);
  void IconClear(void);

  void DrawBoxNameBefore(int no);
  void DrawBoxName(void);
  void DrawMessageLine2(int messageno);
  void DrawMessageLine2Upper(int messageno);
  void MessageLine2Close(void);
  void DrawMessageLine1(int messageno);
  void ExpandPokemon(int no, const pml::pokepara::CoreParam * pp );
  void ExpandItem(int no, int itemno);

  void SetTimerIcon( bool flg );
  void ResetCursorPos(void);

  void DrawJudgeName(void);
  int GetCursorPosType(int curpos);

  void ResetLiftPokeIconClear(void);
  void LiftPokeIconClear(void);

  void LiftPokeIconReady(int page, int pos);
  void LiftPokeIconShortcut(int page, int pos);
  void LiftPokeIconReadyReturn(int page, int pos);
  void LiftPokeIconReadyReturn2(int page, int pos);
  void LiftPokeIconStart(int page,int pos,bool bteam=false);
  void LiftPokeIconMove( gfl2::math::VEC3* pMove);
  void LiftPokeIconEnd(int page,int traypage);
  gfl2::lyt::LytParts* MovePokeIconReady(int page, int pos);

  void LiftItemIconReady(int page, int pos, int item, bool bTouch);
//  void LiftItemIconReadyReturn(void);
  void LiftItemIconStart(int page, int pos, int item, bool bTouch);
  void LiftItemIconAfter(bool bTouch);

  void LiftItemIconEnd(int page);

  void ResetMainCursorVisible(void);
  void ResetMainCursor(void);
  void PassiveMainCursor(void);

  void PassiveBoxTrayChangeCursor(void);
  void ResetBoxTrayChangeCursor(void);
  void SetVisibleBoxTrayChangeCursor( bool flg );
  
  void PassiveBoxChangeCursor(void);
  void ResetBoxChangeCursor(void);
  void SetBoxBarInputEnable(bool benable);

  void PassiveUpperButton(void);
  void ResetUpperButton(void);

  void PassiveUnderButton(void);
  void ResetUnderButton(void);

  void PassiveRightUnderButton(void);
  void ResetRightUnderButton(void);
  
  void PassiveCancelButton(void);
  void ResetCancelButton(void);


  void PassiveChatChangeButton(void);
  void ResetChatChangeButton(void);

  void WarrningMessageBefore(BoxLowBgDrawListener *listener);
  void WarrningMessageAfter(int nowDispTray, int selectTray, int battleTray);

  
  void PassiveFolderButton(void);
  void ResetFolderButton(void);
  void AnimRevCheck( void );
  void PokeIconWhite(int page,int x, bool bWhite);

  
  void MarkWinStart(int i);
  void MarkWinEnd(void);
  
  void DrawTeamName(int i);

  void ResetRangeSelect(void);
  void DispRangeNum(int no ,gfl2::math::VEC3* pvec);
  void ResetRangeNum(void );

  bool RangeSelectStart(int inx,int iny);
  void RangeSelectEnd(void);
  void CarryRangeSelectMove(int cursor,gfl2::math::VEC3* pvec);
  bool RangeSelectStretch(int inx,int iny,int outx, int outy);
  void CarryPokeIconRangeStart(int page, int startx, int starty, int sizex, int sizey, int topcur, bool skipselect);
  void CarryPokeIconRangeEnd(int page);
  void CarryPokeIconPassive(int page,int cursor );

	void CarryRangeSelectVisible(bool bVisible );

  
  void CreateMarkButtonManager( void );
  int SetCursorPosMk(int no,int udlr );
  void DirectCursorPosMk(int no );

  void InfoDispStart(app::tool::InfoWindowBookTypeListener* pListener);
  void InfoDispClose(void);
  void StartMarkAnim(int no,int mark);


  void PokeIconActive(int page,int x = -1);
  void PokeIconPassive(int page,int x = -1);

  void DrawPokeIconAll(int page,int traypage);
  void DrawPokeIconStateAll(int page,int traypage);
//  void RedrawTemochi(void);

  void ResetL2R2Key(void);
  void VisibleL2R2Key(bool bvisible);

private:
  void _pokeIconAllOff(int pos);
  void PokeIconVisible(int pos, bool visible);
	void PokeIconTeam(int pos, bool visible);
	void PokeIconLock(int pos, bool visible);

public:
  int TouchLRButtonHit(u16 x, u16 y);
  void TouchLRButtonHit2(u16 x, u16 y);

  void DrawPageChangeAnim(int changenum);
  void DrawTeamAnim(int changenum);
  void DrawPokeIconBefore(int no);

  void DrawTeamNameBefore(int i);
  void DrawTeamIconBefore(int no);
	void SetMenuCursor(app::tool::MenuCursor* pMenuCursor);
  void StopMainCursor(void);
  void StartMainCursor(void);


  void BeforeBgChange(int wpNo);
  int TouchPokemonHit(u16 x, u16 y);

  void ActiveRangeButton(bool bactive);
  void PokemonTouchEventFunc(gfl2::ui::TouchPanel* pTouchPanel, bool isTouch,u16 x,u16 y);

  bool GetFolderMode(void){ return (mMousePokemonMode==BOX_MOUSE_MODE_FOLDER); };
  bool GetTukamiMode(void){ return (mMousePokemonMode==BOX_MOUSE_MODE_TUKAMI); };//GFNMCat1783
  void SetTukamiMode(void);
  void MouseModeEnd(void);


  void ChangePokemonAnimStart(int selectpage,int selectpos, int nowpage,int nowpos,IconPutArrow put_arrow=ICON_PUT_ARROW_NONE);
//  void ChangePokemonLandingAnimStart(int selectpage,int selectpos, int nowpage,int nowpos);
//  void ChangePokemonRangeAnimStart(void);

public:
  virtual gfl2::lyt::LytParts* GetIconParts(int pos);
  virtual gfl2::lyt::LytParts* GetCarryIconParts(int pos);
  virtual gfl2::lyt::LytParts* GetTemotiIconParts(int pos);
  virtual void TemotiIconPartsReady(int page,int pos);

  
  bool IsEditTypeVisible(void);
  void AllPassiveStart(void);
  void AllPassiveEnd(void);

  void ChangeItemAnimStart(int selectpage,int selectpos, int nowpage,int nowpos,IconPutArrow put_arrow=ICON_PUT_ARROW_NONE);

  void ChangeItemIcon(int pos, int itemno);

  void ReloadCursorPos(void);

  bool IsPokeIconInit(void){ return mbPokeIconInit; }  //アイコン初期化完了

  void TemotiPokeIconAnimStart(void);
  void TemotiPokeIconAnimEnd(void);
  
private:
  void _animStart(int no, int quick);
 void localBgChange(int wpNo, int partlabel, int bglabel);
  void PushButtonTypePos(int type, int onoff, int quick = false);
  
  void StartTemotiSelect(void);
  void EndTemotiSelect(void);

  void DrawPokeIcon3(int pos, App::Box::PokemonIconDraw* pPkd);
  void DrawPokeIconState(int pos, App::Box::PokemonIconDraw* pPkd);
  void SetPokeIconColorAndAlpha( gfl2::lyt::LytParts* pParts, App::Box::PokemonIconDraw* pPkd );
  void DrawPokeFunc(int page,int traypage);

  void TemotiTumeruPokeIconRegist(PokeTool::SimpleParam* pPara,u32 pos,int cursor,App::Box::PokemonIconDraw* pPkd, int painno);

  
  void CarryPokeIconRegist(PokeTool::SimpleParam* pPara,u32 pos,int cursor,App::Box::PokemonIconDraw* pPkd, int painno);
  void localDrawTeamName(int i,int partsno, int color);

  void CursorChange(bool byoko );
  void TemotiSelectCheckFunc(void);

  bool TeamTextBoxFunc( u16 x, u16 y );

  void _iconPoke30Backup(void);

  
  
  
private:
//  app::tool::MenuCursor* mpMenuCursor;
  app::util::Heap*          mpAppHeap;           ///< ヒープ
  BoxLowBgDrawListener*        m_uiListener;     ///< UIイベントリスナー
  BoxLowDrawListener*        m_uiDrawListener;     ///< UIDrawイベントリスナー
  app::util::AppRenderingManager* mpAppRenderingManager;
  gfl2::str::MsgData* mpMsgData;       ///< テストメッセージデータ
  gfl2::str::StrBuf*   mpTempStrBuf;
  gfl2::str::StrBuf*   mpTempStrBuf1;
  gfl2::str::StrBuf*   mpTempStrBuf2;
	gfl2::str::StrBuf*   mpTempStrBuf3;
	gfl2::str::StrBuf*   mpTempStrBuf4;
	gfl2::str::StrBuf*   mpTempStrBuf5;
	gfl2::str::StrBuf*   mpTempStrBuf6;
	print::WordSet* mpWordSet;
  app::tool::InfoWindowBookType* mpInfoWindowBookType;

  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;
  gfl2::math::VEC3 mVecMoveCursorBackup;

  int mKeepCursorPosVertical;   //垂直方向で覚えておく位置  
  int mKeepCursorPosHorizontal;  //水平方向で覚えておく位置
  
  int mButtonPos;
  int mBkButtonPos;
  int mMarkButtonPos;
  int mBkMarkButtonPos;
  int mBattleInSetButtonPos;

  int mMousePokemonNo;
  int mMousePokemonMode;
  u16 mMousePokemonX;
  u16 mMousePokemonY;
  int mMouseX;  //アイコン表示位置に使用
  int mMouseY;
  
  //消しているものを覚えておく
  int mMarkAnimType;

  int mInfoRemove;  //インフォウインドウ出し入れ
  
  int mKeepCursorPosVerticalMk;   //マークつけるとき覚える値
  int mbCarryNum;
  int mPageChangeSpeed;
  int mTeamChangeSpeed;

  bool mbPokemonTouchOn;
  bool mbMarkWin;
  bool mbFolderButtonPassive;
  bool mbTemotiTrayCenter;   //手持ちトレイを真ん中に移動した
  bool mbLR1Passive;    //RL１をパッシブにした
  bool mbCursorPassive;  //カーソルをパッシブにした
  bool mbTouchPassive;  //ポケモン移動モードにした
  bool mbTouchPassiveF;  //ポケモン移動モードにした
  bool mbCarryPokemon;
  bool mbPageChangeAnim;
  bool mbTeamChangeAnim;
	bool mbActiveRangeButton;
  bool mbTemotiCursorDisp;
  bool mbTemotiCursorDisp2;
  bool mbStopCursor;
  bool mbPokeIconInit;  //アイコン初期化完了
  bool mbCancelButtonPassive;  //キャンセルボタン
  bool mbRightUnderButtonPassive;  //右下 ジャッジ、インフォメーション、チャット、検索
  bool mbTemotiChangeCursor;  //てもちのきりかえカーソル
  bool mbChatChangeButtonPassive;//チャットステータス上画面切り替え

  
public:
  bool WallPaperChange(int no);
private:
  bool ChangeWallpaperColor(void);
  void InitChangeWallPaper( s32 mv );
	u16	wpChgCount;	//!< 壁紙切り替えカウンタ
	s16	wpChgMove;	//!< 壁紙切り替え方向（フェードインかアウトか）
  u16 mAnimSeq;

  u8 _delayTimer;
  u8 _bFookFolderButton;   //!< フォルダー切り替えが連続で起きないようにするための抑制フラグ
  
///------------------------------------------
public:
  int BtlInSetCursorPos(int no,int udlr );
//  void BtlCursorPosEnd(int cursor);
	bool MarkSrideEndFunc(void);

  void PushButtonMode(int mode, int quick = false);
  void PushButtonModePos(int type, int onoff, int quick);
  void PushButtonType(int type,int quick=false);
  void SetPokeIconActive(int page,int pos, bool bActive);
  void ResetFunc(void);
  void ResetTemotiTray(void);

  bool IsTemochiFrameMove(void);
  bool IsTeamFrameMove(void);
  bool IsPageFrameMove(void);

private:
  bool IsAnimeEnd( u32 anm );

private:

  int mBtlInKeepButtonPos;

public:
  void InitTouchPos(void);
  int GetNearScrollInterval( int now, int chg );
  IconPutArrow GetIconPutArrow( int now_tray, int get_tray , int get_pos );

private:
  void SetCatchPokeIconTranslate(void);
  int GetScrollInterval( int start, int end, int max, int mv );
  
};



GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )




#endif	/* __BOX_LOW_BG_DRAW_H__ */

