#ifndef __BOX_UPP_POKE_STATUS_FRAME_H__
#define __BOX_UPP_POKE_STATUS_FRAME_H__
//=============================================================================
/**
 * @file	 BoxUppPokeStatusFrame.h
 * @brief	 BoxUppPokeStatusFrame
 * @author k.ohno
 * @date	 2015.7.14
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "BoxLowBgDrawListener.h"

#include "App/Box/include/BoxAppParam.h"

#include "Test/MenuFrameTest/include/FieldMessageSampleAppParam.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "../include/BoxUppStatusListener.h"

//ポケモン交換の送信量を抑える為
#define SENDPOKE_SENDQ_ 1


GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( lyt )
class LayoutRenderPath;
GFL_NAMESPACE_END( lyt )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )



GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxUppFolderDraw;
class BoxUppPokeStatusDraw;
class BoxUppChatDraw;
class BoxUppBgDraw;
class BoxAppSetup;

class BoxUppPokeStatusFrame : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(BoxUppPokeStatusFrame);



#if SENDPOKE_SENDQ_
  
private:
  enum SENDPOKEMON_REQ{
    SENDPOKEMON_NONE,
    SENDPOKEMON_REQ,
    SENDPOKEMON_REQ_CALL,
    SENDPOKEMON_REQ_TIME=45,
  };
  struct SendPokemon{
    int downCount;
    int request;
  };

#endif
  
protected:
  int m_subseq;					      ///< プロセス関数の動作シーケンス
  bool mbEnd;  //Endフラグ

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxUppPokeStatusFrame(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxUppPokeStatusFrame();

public:

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

  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンの描画を引数で渡したステートを表示するように変更する
   */
  //--------------------------------------------------------------------------
//  void SetPokemonStatus( const pml::pokepara::PokemonParam& pokemonParam );


  void CloseWindow(void);

  bool SetDisp(u16 boxno);
  bool SetDispNetwork(u16 stateno,u16 boxno);//ネットワーク用

  void ChangeJudgeMode(void);
  void ChangeChatMode(void);

  //------------------------------------------------------------------
  /**
   * @brief	  フォルダーモードへの切り替え
   */
  //------------------------------------------------------------------
  void ChangeFolderMode(bool bFolder);
  void PokemonStateReloadOpen(bool btarget);

  void SetOtherPokemon(const pml::pokepara::PokemonParam& pokemonParam);

  void DelayInitLayout(void);
  void DispOtherMessage( void );
  int DispOtherMessage2( void );
  void DispSelfMessage( void );

  
  bool SetPokemonFolderDisp(u16 boxno);
  bool ChangeIcon(u16 boxno);

  void ResetBackupBoxNo(void){ mBackupBoxNo = -1; };

  void ChangeForceChat(void);



  BoxUppStatusListener* GetBoxUppStatusListener(void){ return &mBoxUppStatusListener; }
  
  
private:

#if SENDPOKE_SENDQ_
  void SetSendPokeReq(void);
  void UpdateSendPoke(void);
  void SendPokemonParam(void);
  
#endif


  bool ChangeForceState(bool btarget);
  bool StateDrawEnd(void);
  bool StateDrawEnd2(void);

  void ExpandPokemon(const pml::pokepara::CoreParam * pp);

  void ReDispOtherStatus(void);



  class DBoxUppStatusListener : public BoxUppStatusListener
  {
  public:
    virtual void Init(void);
    virtual void Request(UPPDISP_TYPE no, u32 param);
    virtual void Request(UPPDISP_TYPE no, u32 param, u32 param2);
    virtual void PokeChangeRequest(UPPDISP_TYPE no, u32 param, u32 param2);
    virtual void Update(void);
    virtual void UpdatePokemon(void);

    void reqin(UppDispChange* pUDC);
    void reqout(UppDispChange* pUDC);

    void reqoutpoke( UppDispChange* pUDC  );
    void getpoke( UppDispChange* pUDC  );
    void reqinpoke(UppDispChange* pUDC);


    
    void SetThis( BoxUppPokeStatusFrame* pthis ){ mpThis = pthis;}
  private:
    BoxUppPokeStatusFrame* mpThis;
  };


  
private:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;  // カメラを含む環境
  poke_3d::model::BaseCamera*                               m_camera;       // カメラ

  app::tool::AppCommonGrpIconData* mpAppCommonGrpIconData;



  
private:
  pml::pokepara::PokemonParam* mpTargetParam;
  pml::pokepara::PokemonParam* mpMyParam;
  
  app::util::Heap* mpAppHeap;
  app::util::AppRenderingManager* mpAppRenderingManager;
  App::Box::BoxUppPokeStatusDraw*     m_pView;
  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;

  BoxUppChatDraw*  mpBoxUppChatDraw;
  BoxUppBgDraw* mpBoxUppBgDraw;
  BoxUppFolderDraw* mpBoxUppFolderDraw;
  BoxUppFolderDraw* mpBoxUppFolderDrawBk;


#if PM_DEBUG
  int mDebugSeqBackup;
#endif
  
  void* mpArcReadBuff;
  void* mpArcReadBuffTrd;
  int mReadSeq;
  int m_subSeq;
  int mBackupBoxNo;
  bool mbJudge;
  bool mbChat;
  bool mbLock;
  bool mbTeam;
//  bool mbReDispOtherStatus;
  
  DBoxUppStatusListener mBoxUppStatusListener;

#if SENDPOKE_SENDQ_
  SendPokemon mSendPokeSt;
#endif
  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )


#endif // __BOX_UPP_POKE_STATUS_FRAME_H__




