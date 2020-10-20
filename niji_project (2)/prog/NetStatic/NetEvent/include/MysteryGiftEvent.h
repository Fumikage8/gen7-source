
#if     !defined(MYSTERY_GIFT_EVENT_H_INCLUDED)
#define MYSTERY_GIFT_EVENT_H_INCLUDED
#pragma once
// ============================================================================
/**
  * @file   MysteryGiftEvent.h
  * @brief  ふしぎなおくりものイベント
  * @date   2015.05.12
  */
// ============================================================================


// ============================================================================
/**/
// ============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameEvent.h"

#include  "Savedata/include/MysteryGiftSave.h"
#include  "NetApp/MysteryGift/include/GiftAppParam.h"

/*
  forward declare
*/
namespace NetAppLib {
  namespace System {
    class ApplicationSystemBase;
  }
}

namespace NetEvent  {
namespace MysteryGift {
// ============================================================================
/**/
// ============================================================================


#if 0
  ../source/MysteryGiftEvent.cpp
  

    //! 状態
    enum{
      STATE_BOOT_CHECK,   ///< 起動チェック
      STATE_MAIN,         ///< メイン
      STATE_END,          ///< 終了
    };

    GameEvent* mParent;	///< 親（呼び出し元）のイベントへのポインタ
    unsigned int mSeq;				///< シーケンスワーク
    gfl2::heap::HeapBase *mWorkHeap;		///<制御関数毎に固有ワークを作るためのHEAP
    u32         mState;
#ifdef RO_VALID    
    nn::ro::Module* mpModule; ///< DLL
#endif //RO_VALID
#endif


//==============================================================================
/**
  *  @class  MysteryGiftEvent
  *  @brief  ふしぎなおくりものイベント
  *  @date   2015.05.12
  */
//==============================================================================
/*
  2016/01/05
  通信エラーハンドリングが不要のため、指示によりBaseClassをNetEventBaseからGameEventに変更
*/
class MysteryGiftEvent  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( MysteryGiftEvent );

public:
  //------------------------------------------------------------------
  /**
    *  @func     StartEvent
    *  @brief    MysteryGiftEventの開始
    */
  //------------------------------------------------------------------
  static void StartEvent(GameSys::GameManager* pGameManager);

  //------------------------------------------------------------------
  /**
    *  @func     MysteryGiftEvent
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  MysteryGiftEvent( gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
    *  @func     ~MysteryGiftEvent
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~MysteryGiftEvent();


  /*
      GameEvent I/F
  */
  //------------------------------------------------------------------
  /**
    *  @func    BootChk
    *  @brief   起動判定
    */
  //------------------------------------------------------------------
  virtual bool  BootChk( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    InitFunc
    *  @brief   初期化
    */
  //------------------------------------------------------------------
  virtual void  InitFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    EndFunc
    *  @brief   終了
    */
  //------------------------------------------------------------------
  virtual void  EndFunc( GameSys::GameManager* pGameManager );

  //------------------------------------------------------------------
  /**
    *  @func    MainFunc
    *  @brief   更新
    */
  //------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  /*
      End GameEvent I/F
  */

private:
  /*
      members
  */
  gfl2::heap::HeapBase*         m_pEventHeapBase;

  NetApp::Gift::APP_PARAM       m_AppParam;  /**<  アプリケーションパラメータ    */

  pml::pokepara::PokemonParam*  m_pPokemonParamForReceiveDemo;    /**<  */

};  /*  class MysteryGiftEvent  */




#if PM_DEBUG

//==============================================================================
/**
  *  @class  MysteryGiftEvent_ForDebugMenu
  *  @brief  ふしぎなおくりものイベント
             DebugMenuからの呼び出し用、終了時にDebugMenuへ遷移する
  *  @date   2015.05.12
  */
//==============================================================================
class MysteryGiftEvent_ForDebugMenu : public MysteryGiftEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( MysteryGiftEvent_ForDebugMenu );

public:
  MysteryGiftEvent_ForDebugMenu( gfl2::heap::HeapBase* pHeap ) :
    MysteryGiftEvent( pHeap )
  {}
  virtual ~MysteryGiftEvent_ForDebugMenu()  {}

  static void StartEvent(GameSys::GameManager* pGameManager)
  {
    GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, MysteryGiftEvent_ForDebugMenu>(pGameManager->GetGameEventManager());
  }
};  /*  class MysteryGiftEvent_ForDebugMenu  */


#endif


} /*  namespace MysteryGift  */
} /*  namespace NetEvent       */
#endif  /*  #define MYSTERY_GIFT_EVENT_H_INCLUDED */

