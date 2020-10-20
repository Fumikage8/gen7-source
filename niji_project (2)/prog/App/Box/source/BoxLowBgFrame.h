#ifndef __BOX_BG_LOW_FRAME_H__
#define __BOX_BG_LOW_FRAME_H__
//=============================================================================
/**
 * @file	 BoxLowBgFrame.h
 * @brief	 BoxBgLowFrame
 * @author k.ohno
 * @date	 2015.7.14
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "BoxLowDrawListener.h"
#include "BoxLowBgDrawListener.h"
#include "App/Box/include/BoxUppStatusListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"


#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

#include "function/BoxPokemonMoveFunction.h"

#include "BoxLowChtDrawListener.h"
//#include "BoxLowFolderDrawListener.h"
#include "BoxStateEndListener.h"

#include "../include/BoxUppStatusListener.h"

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( lyt )
class LayoutRenderPath;
GFL_NAMESPACE_END( lyt )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;
class BoxItemState;
class BoxP2pTradeState;
class BoxNormalState;
class BoxTeamNameState;
class BoxResortState;
class BoxAllBoxState;
class BoxBattleTeamState;
class BoxExitState;
class BoxMarkState;
class BoxEscapeState;
class BoxItemState;
class BoxWallpaperState;

class BoxLowBgFrame : public applib::frame::CellSuper, public App::Box::BoxLowDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxLowBgFrame);


  ///ステートの戻りを得るリスナー

  //ボックス終了時
  class InnerBoxExitEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxExitStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  //全体ボックス終了時
  class InnerBoxAllBoxEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxAllBoxStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  //アイテムモードボックス終了時
  class InnerBoxItemEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxItemStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  //p2p交換終了時
  class InnerBoxP2pTradeEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode );
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  //バトルチーム終了時
  class InnerBoxBattleTeamEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxBattleTeamStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
	//リゾート終了時
	class InnerBoxResortEndListener  : public BoxStateEndListener
	{
	public:
		virtual void OnEnd( u32 retcode ){mpThis->mBoxResortStateRet=retcode; };
		void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
	private:
		BoxLowBgFrame* mpThis;
	};
  
  //壁紙終了時
  class InnerBoxWallPaperEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxWallPaperStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  //チーム名変更時
  class InnerBoxTeamNameEndListener  : public BoxStateEndListener
  {
  public:
    virtual void OnEnd( u32 retcode ){mpThis->mBoxTeamNameStateRet=retcode; };
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };


	//通常終了時
	class InnerBoxNormalEndListener  : public BoxStateEndListener
	{
	public:
		virtual void OnEnd( u32 retcode ){mpThis->mBoxNormalStateRet=retcode; };
		void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
	private:
		BoxLowBgFrame* mpThis;
	};

  ///普通状態の管理リスナー

  //なにもしない時用
  class DInnerNoneListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnTouchFirstHook( bool isTouch ){return true;};
    virtual bool OnKeyAction( u32 dir ){return false;};
    virtual void OnButtonAction( int button_no ){};
    virtual bool GetInputEnableFlg(void){ return false; }
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };

  
  //普通のカーソルの時用
  class DInnerNormalListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void  DebugDisp(void);
#endif
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    virtual void OnMousePokemonStart(int x,int y);
    virtual void OnMousePokemonHand(int x,int y, int cursorno);
    virtual void OnMouseMenuOpen(int cursorno);
    virtual bool OnMousePokemonCurpos(int cursorno);
    virtual bool OnMousePokemonFirstTouch(int cursorno);
    
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };
  

  class BoxInfoWindowBookTypeListener : public app::tool::InfoWindowBookTypeListener
  {
    GFL_FORBID_COPY_AND_ASSIGN(BoxInfoWindowBookTypeListener);
public:
    BoxInfoWindowBookTypeListener(void){}
    virtual ~BoxInfoWindowBookTypeListener() {}

  //! @brief  クローズし切ったときに呼ばれるイベントハンドラ
    virtual void OnClose(void);
    void SetThis( BoxLowBgFrame* pthis ){ mpThis = pthis;}
  private:
    BoxLowBgFrame* mpThis;
  };


  
protected:
  int m_subseq;					      ///< プロセス関数の動作シーケンス
  bool mbEnd;  //Endフラグ

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxLowBgFrame(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxLowBgFrame();



public:

  //--------------------------------------------------------------------------
  /**
   * @brief  マウスモードが終わったとき呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void OnMouseModeEnd(void);
  
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup(APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup);


  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);



public:
  int GetChangeFolderPoke(void);

  void SetFolderCursorPos(int cursor);

  
  void SetNetworkPokemonStatus(int statusType);

  int GetNetChangePoke(void);
  

  bool IsInitEnd(void){ return mbInitEnd; };

  bool IsEndReq(void){ return mbEndReq; };


  void CloseWindow(void);

  bool IsChangePoke(void);
  u16 GetChangePoke(void);// カーソルのポケモン
  void ResetChangePoke(void);


  void RequestChangePoke(u32 param=0);
	void RequestNetChangePoke(u32 param);

  
  int GetMyChatMessage(void);
  
  void reloadResortPoke(void);

  void MarkInitialize(void);
//通信リスナーから受け取る
  void OnFirstTradeSuccess(const pml::pokepara::PokemonParam& pokemonParam);
  void OnTradeSuccess(void);
  void OnTradeComplete(void);
  void OnTradeCancelReceived(void);
  void OnFinalTradeCancel(void);
  void TradeFinalDecide(void);
  void OnFirstTradePokemonInvalid(bool flg);
  void OnFirstTradeValidationError(bool flg);
  void OnTradeExitReceived(void);
  void OnTradeExitSuccess(void);

  bool IsReloadIcon(void);
  u16 GetReloadIcon(void);
  void ResetReloadIcon(void);



  void BoxMenuOpen(void);
  void PaperMenuOpen(void);
  void PaperXMenuOpen(int no);
  void PaperChangeExec( int no );
  void TeamNameChangeOpen(void);
  void SetStatusJump( void );

  void SetListener(void);
  bool IsPokeIconInit(void);  //アイコン初期化完了

public:
  void SetNowCursorNo(int no);
  int GetNowCursorNo(void);
  int GetNowTrayNo(void);
  int* GetNowCursorNoPtr(void);
  
  void SetSelectCursor(void);  //今のカーソル位置をセレクトに設定
  
private:
  void celljumpBeforeFunc(void);
  void celljumpAfterFunc(void);
  BoxNormalState* CreateBoxNormalState(void);
  BoxTeamNameState* CreateBoxTeamNameState(void);
  BoxResortState* CreateBoxResortState(void);
  BoxAllBoxState* CreateBoxAllBoxState(void);
  BoxBattleTeamState* CreateBoxBattleTeamState(void);
  BoxExitState* CreateBoxExitState(void);
  BoxMarkState* CreateBoxMarkState(void);
  BoxEscapeState* CreateBoxEscapeState(void);
  BoxItemState* CreateBoxItemState(void);
  BoxWallpaperState* CreateBoxWallpaperState(void);
  void CreateBoxP2pTradeState(void);

  bool CreatePokeIcon(void);
  bool DeletePokeIcon(void);

  //次のページ
  int nextPage(int changenum);
public:
  void PageChange( int changenum);
private:
  virtual void OnPageScrollEnd(int changenum);

  int getBattleChange(int changenum);

  bool PokemonSet(int selectpage,int selectpos, int nowpage, int nowpos, bool bsingle);

  void CursorActionMark( int udlr);


  void ChangePokemonBoxLinkData(int selectpage,int selectpos,int oldpage,int oldpos);

  void FirstDecide(void);
  void FirstDecideNormal(void);

  void ChatStart(void);
//  void ChatEndReq(void);
  void simpleWarrningMessage(int no, int line, int nextSeq, bool bse=true);
  bool pokeTradeCheck(void);
  void ExitFuncStart(void);
  void EscapeFuncStart(void);
  void EscapeExecStart(void);
  void EscapeExecEnd(void);
  void EscapeExecFinish(void);

  bool pokeEscapeCheck(void);
//  void ItemHandStart(void);
  bool rangeCheck(int pos1,int pos2);
public:
  void CursorAction( int udlr);
private:
  bool AddMenuItem(bool bTemochi);
//  void AButtonMenuItem(int button_no);
  void ItemModeSetup(void);
  void ItemModeShutdown(void);

  void SetEditType(int i);
  void EditTypeChange(void);
  void BattleInCursorAction( int udlr);
	public:
  void BattleInPageChange( int changenum);
  void drawPokeIconAll(void);
private:
  void drawPokeIconBox(void);
  void drawPokeIconBattle(void);
  void reloadTeamPoke(int teamno);
  void InfoDispStart(void);
  void CarryRange(void);

public:
  void RangeSelectStretch(RangeData* pRange);
private:
  void FolderAnimInit(bool brange);
  void MoveModeChange(void);
  void BattleTeamModeSetup(void);
  void BattleTeamShutdown(void);
  void SetPartyPokemon(int partypos, pml::pokepara::PokemonParam * pp);
public:
  void PageChangeDirect( int tray);
  void PageChangeDirectSelectTray(void);
  void PageChangeDirectNowTray(void);
  void BoxBgLowDrawLiftPokeIconEnd(void);
private:
  bool LockCheck_ByItem(void);

  void BattleRegistOrRemove(int selectpage,int selectpos,int oldpage,int oldpos);

public:
  void NetworkSendMessage(int btnno);

  void SetFadeEnd(void){ mbFadeEnd=true; }
  bool IsFadeEnd(void){ return mbFadeEnd; }

  
private:
  static const int MENU_1LINE_POS = -80;
  static const int MENU_2LINE_POS = -60;



  DInnerNoneListener mDInnerNoneListener;
  DInnerNormalListener mDInnerNormalListener;
  
  BoxInfoWindowBookTypeListener mBoxInfoWindowBookTypeListener;
  
  app::util::Heap* mpAppHeap;
  app::util::AppRenderingManager* mpAppRenderingManager;
  app::tool::MenuWindow* mpMenuWindow;
  app::tool::MenuCursor* mpMenuCursor;
  BoxLowBgDraw* mpBoxBgLowDraw;

  BoxLowChtDraw* mpBoxChtLowDrawBk;

  BoxLowFolderDraw* mpBoxLowFolderDraw;

  BoxPokemonMoveFunction* mpBoxPokemonMoveFunction;  //ポケモンの移動、配置にかかわる

  InnerBoxItemEndListener mInnerBoxItemEndListener; //結果リスナー
  u32 mBoxItemStateRet;  //その返

  InnerBoxAllBoxEndListener mInnerBoxAllBoxEndListener;//結果のリスナー
  u32 mBoxAllBoxStateRet;  //その返

  BoxP2pTradeState* mpBoxP2pTradeState;  //ｐ２ｐ交換 通信の為ポインタが必要
  InnerBoxP2pTradeEndListener mInnerBoxP2pTradeEndListener;//結果のリスナー
  u32 mBoxP2pTradeStateRet;  //その返

  InnerBoxBattleTeamEndListener mInnerBoxBattleTeamEndListener; //結果リスナー
  u32 mBoxBattleTeamStateRet;  //その返
  
  InnerBoxWallPaperEndListener mInnerBoxWallPaperEndListener; //結果リスナー
  u32 mBoxWallPaperStateRet;  //その返

  InnerBoxResortEndListener mInnerBoxResortEndListener; //結果リスナー
  u32 mBoxResortStateRet;  //その返

  InnerBoxNormalEndListener mInnerBoxNormalEndListener; //通常の動作用
  u32 mBoxNormalStateRet;

  InnerBoxExitEndListener mInnerBoxExitEndListener; //終了時
  u32 mBoxExitStateRet;

  InnerBoxTeamNameEndListener mInnerBoxTeamNameEndListener; //チームネーム
  u32 mBoxTeamNameStateRet;

  
  
  
  PokePara* mpBoxPokeParaFunc;
  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;
  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用
  pml::PokeParty* mpPartyTemp;  // 交換検査に使用
  RangeData mRangeSelect;

  void* mpArcReadBuff;
  void* mpArcUppReadBuff;
  void* mpMenuCursorLytDataBuffer;
  int mReadSeq;
  int m_subSeq;
  int m_CurSeq;

  int mCursorPos;   //カーソルの位置
  int mFolderCursorPos;
//  int mFolderCursorPosBack;

  int mSelectCursorPos;   // 選んでる、掴んでるカーソルの位置
  int mSelectPagePos;    //選んでる、掴んでるページの位置
	bool mbSelecting;
  bool mbFadeEnd;
  bool mbInitEnd;
	bool mbEndReq;

public:
  void SetBoxUppStatusListener(BoxUppStatusListener* pBoxUppStatusListener){ mpBoxUppStatusListener = pBoxUppStatusListener; };

private:

  BoxUppStatusListener* mpBoxUppStatusListener;  //上画面との掛け渡し

private:
  void GetInputExculusiveKeyMode(void);
  void SetInputExculusiveKeyMode( bool flg );

  bool m_isUIViewInputExclusiveKey;   // UIViewマネージャのボタン監視モード
  

#if PM_DEBUG
  int mDebugSeqBackup;
#endif
  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOX_BG_LOW_FRAME_H__




