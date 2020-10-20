#if !defined(EVOLUTIONEVENT_H_INCLUDED)
#define EVOLUTIONEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionEvent.h
 * @date   2015/08/28 Fri. 19:54:08
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameEvent.h"
#include  "NetApp/Evolution/include/EvolutionProcParam.h"

#include  "NetStatic/NetAppLib/include/Util/NetAppClearSettingUtility.h"

/*  save  */
#include  "Savedata/include/MyItemSave.h"

/*  zukan  */
#include  "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"

/*  wazaoshie  */
#include  "App/WazaOshie/include/WazaOshieAppParam.h"
#include  "App/WazaOshie/include/WazaOshieProc.h"

/*
  forward declare
*/
namespace NetAppLib  {
namespace System {
  class ApplicationSystemBase;
}
}
namespace NetApp {
namespace Evolution {
  class EvolutionProc;
}
}

namespace NetApp  {
namespace Evolution {
  class EvolutionProc;
}
}

namespace NetEvent  {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================
/**
  結果
*/
typedef struct  EvolutionEventResult
{
  bool    m_IsEvolved;      /**<  進化した  */


  /*  ctor  */
  EvolutionEventResult(void)
    : m_IsEvolved(false)
  {}
}EvolutionEventResult;




/**
  進化Event起動パラメータ
  ポインタ参照先はEvent実行中は保持されること
*/
typedef struct  EvolutionEventParam
{
  /*  進化関連  */
  pml::pokepara::PokemonParam*    m_pTargetPokemon;       /**<  [in/out]  対象となるポケモンのパラメータ                                                  */
  MonsNo                          m_MonsNoEvolve;         /**<  [in]      進化先MonsNo                                                                    */
  u32                             m_EvolveRootNum;        /**<  [in]      進化Root  ※「進化ルート」とあったのでrootではなくrouteらしい                   */

  /**  進化の要因  */
  NetApp::Evolution::Reason       m_Reason;

  /*  所持品関連  */
  pml::PokeParty*                 m_pPokeParty;           /**<  [in/out]  現在の手持ちポケモン（ヌケニン発生確認用、また発生時には追加される）            */
  Savedata::MyItem*               m_pMyItem;              /**<  [in/out]  手持ちのアイテム（ヌケニン発生確認用、また発生時には条件アイテムが消費される）  */
  u32                             m_PokePartyReserveNum;  /**<  [in]      手持ちの空きのうち、呼び出し元で後に必要となるため予約する必要のある数
                                                                          空きがこれ未満の場合はヌケニンが発生しない
                                                                          default: 0
                                                          */
  /*
    2016/03/15 Yu Muto
    ポケモンを捕獲して戦闘を終了、ツチニンがレベルアップの結果進化しヌケニンが発生した場合、
    nijiではその後に捕獲ポケモンが手持ち/BOXに追加されるため、ヌケニンの発生により空きがなくなると破綻する
    この問題を回避するため、呼び出し元で手持ちの空きを予約するm_PokePartyReserveNumを追加
  */

  /*  結果  */
  EvolutionEventResult*           m_pResult;               /**<  [out]     結果  */


#if 0
  /*  ヌケニン確認用  */
  pml::PokeParty * party;               ///<手持ちパーティ
  savedata::MYITEM * m_myitem;                ///<手持ちどうぐセーブワークへのポインタ

    //ヌケニン生成チェック
    //（１）進化したポケモンがテッカニンで、
    //（２）モンスターボールを持っていて、
    //（３）手持ちに空きがあるとき、
    // ヌケニンが生成される
    if ( msys->m_app_param->new_monsno == MONSNO_TEKKANIN
      && msys->m_myitem->Check( ITEM_MONSUTAABOORU, 1, msys->heap.pHeap ) == true
      && msys->m_app_param->party->GetMemberCount() < pml::PokeParty::MAX_MEMBERS )
    {
      //テッカニンからヌケニンを生成、手持ちに加える
      pml::pokepara::PokemonParam * nukeninp;
      nukeninp = pml::pokepara::Factory::CreateNukenin( msys->heap.pHeap, msys->m_pokep);
      msys->m_app_param->party->AddMember( *nukeninp );
      //ずかん登録
      if(!msys->m_app_param->is_net)  // 通信交換のときは外で登録する
      {
        msys->m_zukansave->SetPokeGet(*nukeninp);
        msys->m_zukansave->SetPokeSee(*nukeninp);
        msys->m_zukansave->IncrementPokeSeeAmount(nukeninp->GetMonsNo());
      }
      GFL_DELETE nukeninp;
      //手持ちのモンスターボールを減らす
      msys->m_myitem->Sub( ITEM_MONSUTAABOORU, 1, msys->heap.pHeap );
    }


#endif
  /*    */
  EvolutionEventParam(void)
    : m_pTargetPokemon(NULL)
    , m_MonsNoEvolve((MonsNo)0)
    , m_EvolveRootNum(0)
    /*  進化の要因  */
    , m_Reason(NetApp::Evolution::REASON_LevelUp)
    /*  所持品関連  */
    , m_pPokeParty(NULL)
    , m_pMyItem(NULL)
    , m_PokePartyReserveNum(0)
    /*  結果  */
    , m_pResult(NULL)
  {}
}EvolutionEventParam;



//==============================================================================
/**
  *  @class  EvolutionEvent
  */
//==============================================================================
/*
  2016/01/05
  通信エラーハンドリングが不要のため、指示によりBaseClassをNetEventBaseからGameEventに変更
*/
class EvolutionEvent
  : public    GameSys::GameEvent
  , protected NetAppLib::Util::NetAppClearSettingUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( EvolutionEvent );

public:
  /*  イベント起動  */
  static void StartEvent(GameSys::GameManager* pGameManager);
  static void StartEvent(GameSys::GameManager* pGameManager, EvolutionEventParam& rEventParam);




  EvolutionEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~EvolutionEvent();

  /*
      GameEvent I/F
  */
  virtual bool  BootChk( GameSys::GameManager* pGameManager );
  virtual void  InitFunc( GameSys::GameManager* pGameManager );
  virtual void  EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  /*
      End GameEvent I/F
  */

private:
  /*
    funcs
  */
  void  ResultHandler(GameSys::GameManager* pGameManager);


  /*
      members
  */
  EvolutionEventParam                   m_EventParam;
  NetApp::Evolution::APP_PARAM          m_ProcParam;
  NetApp::Evolution::EvolutionProc*     m_pProc;

  /*  図鑑  */
  app::event::ZukanRegisterEventParam   m_ZukanParam;
  pml::pokepara::PokemonParam*          m_pNukeninTempParam;      /**<  ヌケニンが発生する場合は図鑑呼び出しようにここで保持  */

  /*  waza  */
  pml::PokeParty*                       m_pDummyPartyForWazaProc; /**<  wazaOshie呼び出し用ダミーパーティ */
  App::WazaOshie::APP_PARAM             m_WazaParam;
  App::WazaOshie::Proc*                 m_pWazaProc;

  /*  others  */
  bool                                  m_IsNeedBGMResume;
};  /*  class EvolutionEvent  */


} /*  namespace Evolution  */
} /*  namespace NetApp       */
#endif  /*  #if !defined(EVOLUTIONEVENT_H_INCLUDED)  */
