#ifndef __BOXP2PTRADESTATE_H__
#define __BOXP2PTRADESTATE_H__
//=============================================================================
/**
 * @file	 BpxP2pTradeState.h
 * @brief	 P2p交換のシーケンス
 * @author k.ohno
 * @date	 2016.2.27
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
//#include "BoxLowDrawListener.h"
//#include "BoxLowBgDrawListener.h"
#include "App/Box/include/BoxUppStatusListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "AppLib/include/Util/AppRenderingManager.h"


#include <AppLib/include/Tool/app_tool_MenuWindow.h>
#include <AppLib/include/Tool/app_tool_MenuWindowListener.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookType.h>
#include <AppLib/include/Tool/InfoWindow/app_tool_InfoWindowBookTypeListener.h>

//#include "BoxLowChtDrawListener.h"
//#include "BoxLowFolderDrawListener.h"


#include "../BoxStateBasis.h"



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class PokePara;
class BoxLowBgDraw;
class BoxLowChtDraw;
class BoxAppSetup;
class BoxLowFolderDraw;

class BoxP2pTradeState : public BoxStateBasis , public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxP2pTradeState);


public:




  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxP2pTradeState(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxP2pTradeState();

public:


  void Setup(void);

  void Release(void);

  virtual u32 StateFunc(void);

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

  ////外部呼出し

  //通信Aボタン選択開始
	void ModeStart(int tray,int cursor);
  //チャット開始
	void ChatStart(void);

  void BoxP2pTradeLocalSetup( BoxLowChtDraw* mpBoxChtLowDraw);

  
private:

  void LiftEndFunc(void);
  
  //交換に出していいかどうか
  int pokeTradeCheck(void);
  void pokeTradeStartMenu(void);
  void funcMessageBefore(void);
  void funcMessageAfter(void);


public:
  void OnFirstTradeSuccess(const pml::pokepara::PokemonParam& pokemonParam);
  void OnTradeSuccess(void);
  void OnTradeExitReceived(void);
  void OnTradeExitSuccess(void);
  void OnTradeComplete(void);
  void OnTradeCancelReceived(void);
  void OnFinalTradeCancel(void);
  //交換２回目の決定時の処理
  void TradeFinalDecide(void);
  void OnFirstTradePokemonInvalid(bool bMyValidationError);
  void OnFirstTradeValidationError(bool bMyValidationError);
private:
  void ChatEndReq(void);
	void SetStatusJump( void );
	void SetNetworkPokemonStatus(int statusType);
  void ItemPeaceCheckMenu(void);

    //チャット結果を受け取るインナークラス
  class InnerBoxLowChtDrawListener : public App::Box::BoxLowChtDrawListener
  {
  public:
    //--------------------------------------------------------------------------
    /**
     * @brief   ボタン押下通知
     */
    //--------------------------------------------------------------------------
    virtual void OnButtonAction( int button_no );
    //--------------------------------------------------------------------------
    /**
     * @brief   キー操作通知
     * @param		dir  キー方向
     * @retval  true = 操作実行
     * @retval  false = 操作失敗
     */
    //--------------------------------------------------------------------------
    virtual bool OnKeyAction( u32 dir );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };


  //交換最初確認
  class DInnerMessageTradeFirstListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void DebugDisp(void);
#endif
    virtual bool OnTouchFirstHook( bool isTouch );
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //交換最終確認
  class DInnerMessageTradeFinalListener : public BoxLowBgDrawListener
  {
  public:
#if PM_DEBUG
    virtual void DebugDisp(void);
#endif
    virtual bool OnTouchFirstHook( bool isTouch );
    virtual bool OnKeyAction( u32 dir );
    virtual void OnButtonAction( int button_no );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
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
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };


  //P2P最終決定用
  class InnerP2PTradeListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //P2P用
  class InnerP2PListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //GTS用
  class InnerGTSListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //Miracle用
  class InnerMiracleListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //はいいいえを受け取るインナークラス
  class InnerYesNoListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };
  //ピース外れるよのはいいいえを受け取るインナークラス
  class InnerPeaceListener : public app::tool::MenuWindowListener
  {
  public:
    virtual void SelectMenu( u32 id );
    void SetThis( BoxP2pTradeState* pthis ){ mpThis = pthis;}
  private:
    BoxP2pTradeState* mpThis;
  };


  

  InnerP2PListener mInnerP2PListener;
  InnerP2PTradeListener mInnerP2PTradeListener;
  InnerGTSListener mInnerGTSListener;
  InnerMiracleListener mInnerMiracleListener;
  InnerBoxLowChtDrawListener mBoxChtLowDrawListener;
  InnerYesNoListener mInnerYesNoListener;
  InnerPeaceListener mInnerPeaceListener;

  DInnerNoneListener mDInnerNoneListener;
  DInnerMessageTradeFinalListener mDInnerMessageTradeFinalListener;
  DInnerMessageTradeFirstListener mDInnerMessageTradeFirstListener;

  pml::pokepara::CoreParam* mpCoreTemp;  //マークつける時 交換検査に使用
	pml::PokeParty* mpPartyTemp;  // 交換検査に使用

  BoxLowChtDraw* mpBoxChtLowDraw;
	BoxLowChtDraw* mpBoxChtLowDrawBk;

  int mItemno;
  
  u32 mSelectTray;
  u32 mSelectCursor;

#if PM_DEBUG
  int mDebugSeqBackup;
#endif

  
public:
  void SetBoxUppStatusListener(BoxUppStatusListener* pBoxUppStatusListener){ mpBoxUppStatusListener = pBoxUppStatusListener; };

private:
  BoxUppStatusListener* mpBoxUppStatusListener;  //上画面との掛け渡し

};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOXP2PTRADESTATE_H__




