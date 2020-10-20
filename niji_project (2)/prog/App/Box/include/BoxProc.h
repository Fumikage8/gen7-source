#ifndef __BOX_PROC_H__
#define __BOX_PROC_H__
//=============================================================================================
/**
 * @file   BoxProc.h
 * @brief  呼び出しのProcから
 * @author k.ohno
 * @date   2015.2.16
 */
//=============================================================================================
#pragma once

//#include "BoxUppStatusListener.h"

#include "GameSys/include/GameProc.h"
#include "AppLib/include/Frame/AppFrameManager.h"
//#include <System/include/MyRenderingPipeLine.h>
#include "AppLib/include/Util/app_util_heap.h"

#include "AppLib/include/Util/AppRenderingManager.h"

#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeResponseListener.h"

#include "BoxAppParam.h"

#include "App/BoxSearch/include/BoxSearchAppParam.h"

#if PM_DEBUG

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( debug )

  class DebugWinGroup;

GFL_NAMESPACE_END( debug )
GFL_NAMESPACE_END( gfl2 )

#endif

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN(tool)
class PokeIcon;
class ItemIcon;
class AppToolTrainerIconRendering;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

class BoxBaseFrame;
class BoxLowBgFrame;
class BoxUppPokeStatusFrame;
class BoxAppSetup;
class BoxSimpleAccesser;

class Proc : public GameSys::GameProc {
  GFL_FORBID_COPY_AND_ASSIGN(Proc); //コピーコンストラクタ＋代入禁止


  class NetworkResponce : public NetAppLib::P2PTrade::P2PTradeResponseListener
  {
  public:
    NetworkResponce(void){};
    ~NetworkResponce(){};

    //------------------------------------------------------------------
    /**
     *  @brief    メッセージIDを受信した時に呼ばれるハンドラです
     *  @param    eMessageID 受信したメッセージID
     *  @param    monsno 欲しいポケモンの種族番号
     *  @note     関連関数 AddSendMessageIDRequest
     */
    //------------------------------------------------------------------
    virtual void OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer );
    //------------------------------------------------------------------
    /**
     *  @brief    ポケモンのパラメータを受信した時に呼ばれるハンドラです
     *  @param    pokemonParam 受信したポケモンのパラメータ
     *  @note     関連関数 AddSendPokemonParamRequest
     */
    //------------------------------------------------------------------
    virtual void OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam );
    //------------------------------------------------------------------
    /**
     *  @brief    最初の交換が成立した時に呼ばれるハンドラです
     *  @param    pokemonParam 交換相手のポケモンのパラメータ
     *  @note     関連関数 AddFirstTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam );
    //------------------------------------------------------------------
    /**
     *  @brief    交換相手のポケモンが不正だった時に呼ばれるハンドラです
     *  @param    bMyPokemonInvalid [ true : 自分のポケモンが不正 / false : 相手のポケモンが不正 ]
     *  @note     関連関数 AddFirstTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFirstTradePokemonInvalid( bool bMyPokemonInvalid ); //GFNMCat4378
    //------------------------------------------------------------------
    /**
     *  @brief    不正チェックサーバーへ接続出来なかった時などに呼ばれるハンドラです
     *  @param    bMyValidationError [ true : 自分の通信エラー / false : 相手の通信エラー ]
     *  @note     関連関数 AddFirstTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFirstTradeValidationError(bool bMyValidationError); //GFNMCat4378
    //------------------------------------------------------------------
    /**
     *  @brief    最初の交換リクエストがキャンセルされた時に呼ばれるハンドラです
     *  @note     関連関数 CancelFirstTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFirstTradeCancel();
    //------------------------------------------------------------------
    /**
     *  @brief    最後の交換が成立した時に呼ばれるハンドラです
     *  @note     関連関数 AddFinalTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFinalTradeSuccess();
    //------------------------------------------------------------------
    /**
     *  @brief    最後の交換が完了した時に呼ばれるハンドラです
     *  @note     関連関数 AddFinalTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFinalTradeComplete();
    //------------------------------------------------------------------
    /**
     *  @brief    最後の交換リクエストがキャンセルされた時に呼ばれるハンドラです
     *  @note     関連関数 CancelFinalTradeRequest
     */
    //------------------------------------------------------------------
    virtual void OnFinalTradeCancel();
    //------------------------------------------------------------------
    /**
     *  @brief    交換キャンセルを受信した時に呼ばれるハンドラです
     *  @note     関連関数 AddSendTradeCancelRequest
     */
    //------------------------------------------------------------------
    virtual void OnTradeCancelReceived();
    //------------------------------------------------------------------
    /**
      *  @brief    交換終了を受信した時に呼ばれるハンドラです
      *  @note     関連関数 AddTradeExitRequest
      */
    //------------------------------------------------------------------
    virtual void OnTradeExitReceived();
    //------------------------------------------------------------------
    /**
      *  @brief    交換終了が成立した時に呼ばれるハンドラです
      *  @note     関連関数 AddTradeExitRequest
      */
    //------------------------------------------------------------------
    virtual void OnTradeExitSuccess();

  public:
    inline void SetThis(Proc* pThis){ mpThis = pThis; }
    
  private:
    Proc* mpThis;
    
  };

  
public:
  Proc(void);  //コンストラクタ
  virtual ~Proc(void);  //デストラクタ

  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);
  
  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no);

  //-----------------------------------------------------------------------------
  /**
   * @func    PreDrawFunc
   * @brief   Proc描画下準備
   * @date    2015.07.09
   *
   * @param   pManager    プロセス管理マネージャー
   */
  //-----------------------------------------------------------------------------
  virtual void PreDrawFunc( gfl2::proc::Manager * pManager );
  
  //------------------------------------------------------------------
  /**
   * @brief   PROCのメインを行う関数
   *  1/30フレームの②フレーム目の処理です。
   *  1/60フレーム動作時には、使われません。
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);


  //--------------------------------------------------------------------------
  /**
   * @brief   ポケモンの描画を引数で渡したステートを表示するように変更する
   */
  //--------------------------------------------------------------------------
//  virtual void SetPokemonStatus( const pml::pokepara::PokemonParam& pokemonParam );


  void SetAppParam(APP_PARAM* pAppParam, BoxAppSetup* pBoxAppSetup, App::BoxSearch::APP_PARAM* pBoxSearch )
  {
    mpAppParam = pAppParam;
    mpBoxAppSetup = pBoxAppSetup;
    mpBoxSearch = pBoxSearch;

#if PM_DEBUG
    // デバッグ用APP_PARAM保存
    m_debug_appparam = mpAppParam;
#endif
  };
  
  //applib::frame::Manager* GetFrameManager(void){ return mpFrameManager; };

  void SetAsyncFileManager(gfl2::fs::AsyncFileManager* pFM){mpAsyncFileManager = pFM;};

  app::util::AppRenderingManager* GetAppRenderingManager(void){  return mpAppRenderingManager;  }

  app::util::Heap* GetAppHeap(void){ return mpAppHeap; }

  bool CreatePokeIcon(void);
  bool DeletePokeIcon(void);
  
  bool DeleteItemIcon(void);

#if PM_DEBUG
  // デバッグ表示用
  typedef struct{
    int value[144];
    gfl2::str::STRCODE poke_nickname[24];
    gfl2::str::STRCODE parent_name[24];
    gfl2::str::STRCODE other_parent_name[24];
    
  }DebugPokeParaDump;
#endif

private:
#if PM_DEBUG
  // ポケモン情報表示用
  void DebugMenu_Init( gfl2::heap::HeapBase *heap );
  void DebugMenu_Exit( void );
  void DebugMenu_Update( void );
  pml::pokepara::CoreParam *GetCoreParamPointer( int tray, int pos );

  int  old_tray, old_pos;
  DebugPokeParaDump             m_dump;
  gfl2::debug::DebugWinGroup*   m_debug_topGroup;
  gfl2::debug::DebugWinGroup*   m_debug_LockGroup;
  gfl2::ui::DeviceManager*      m_debug_DeviceManager;
  pml::pokepara::CoreParam      *m_coreParam;
  APP_PARAM                     *m_debug_appparam;
  
#endif

  void CreateMainPipeline(void);

  void CreateTrainerPicture(int index);


  static const int SIZE_PROC_MANAGER = 0x1000;   /// 4Kバイト  マネージャーのヒープは荒れるので別途管理

  Proc::NetworkResponce *mpNetworkResponceListener;


  app::util::AppRenderingManager* mpAppRenderingManager;

  app::util::Heap* mpAppHeap;
  app::util::Heap* mpAppHeapUpper;
  
  applib::frame::CellSuper* mpMenuCellFrame;

  BoxSimpleAccesser* mpBoxSimpleAccesser;

  gfl2::heap::HeapBase *mpUpManagerHeap;
  gfl2::heap::HeapBase *mpLowManagerHeap;
  applib::frame::Manager *mpUpFrameManager;
  applib::frame::Manager *mpLowFrameManager;

  
  APP_PARAM* mpAppParam;
  BoxAppSetup* mpBoxAppSetup;
  App::BoxSearch::APP_PARAM* mpBoxSearch;

  BoxUppPokeStatusFrame* mpBoxUppPokeStatusFrame;
  BoxUppPokeStatusFrame* mpBoxUppPokeStatusFrameFinish;

  BoxLowBgFrame* mpBoxLowBgFrame;

  gfl2::fs::AsyncFileManager* mpAsyncFileManager;   //ファイルマネージャー

  bool mbTIRTerminate;
  bool mbOnTradeExitReceived;

  u32 mEndSeq;
  u32   mSeq;
  u32 m_CurSeq;
  
  //  3Dモデル描画用
  System::nijiAllocator*            m_pNijiAllocatorForEdgeMapTexture;  // VRAM限定。

  gfl2::gfx::IGLAllocator* mpOldAllocator;   //セットされているアロケータを記憶するもの
  gfl2::heap::HeapBase* m_pLocalDeviceHeap;                   // 専用のヒープ、寿命も管理する　
  System::nijiAllocator* m_pLocalDeviceHeapAllocator;      // 専用のアロケーター、寿命も管理する　
  System::nijiAllocator* m_pNijiAllocator;  //レンダリングに必要なアロケータ

  
};

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_PROC_H__
