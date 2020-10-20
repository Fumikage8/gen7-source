#if !defined(JOINFESTAPOKESTATUSEVENT_H_INCLUDED)
#define JOINFESTAPOKESTATUSEVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaPokeStatusEvent.h
 * @date   2016/11/24 Thu. 16:38:30
 * @author muto_yu
 * @brief  JoinFesta(FesCircle/BattleFesCircle)からのポケモンステータス画面呼び出しEvent
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  <GameSys/include/GameManager.h>
#include  <GameSys/include/GameEvent.h>
#include  <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

#include  <App/Status/include/StatusAppParam.h>


// =============================================================================
/**/
//==============================================================================
namespace App {
namespace Status  {
  class Proc;
}
}



namespace NetEvent  {
namespace JoinFestaPokeStatus {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/24 Thu. 16:40:37
  */
//==============================================================================
class JoinFestaPokeStatusEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPokeStatusEvent );
public:

  struct  Param
  {
    struct PartnerInfo
    {
      BattleFesTool::BFCTrainerInfo*  pTrainerInfo;
      pml::pokepara::PokemonParam*    pTarget;
      
      /*  ctor  */
      PartnerInfo(void)
        : pTrainerInfo(NULL)
        , pTarget(NULL)
      {
      }
    }partnerInfos[pml::PokeParty::MAX_MEMBERS];

    u32   focusPokemonIndex;    /**<  Status画面で最初にフォーカスされるPokemonのIndex  */
    bool  bLoadRadarChartCRO;
    
    Param(void) 
      : focusPokemonIndex(0)
      , bLoadRadarChartCRO(true)
    {}
  };

  static void StartEvent(GameSys::GameManager* pGameManager, Param* pParam);

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

  JoinFestaPokeStatusEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~JoinFestaPokeStatusEvent();

private:
  Param*                    m_pEventParam;
  gfl2::heap::HeapBase*     m_pEventHeapBase;         /**<  Event用Heap                   */
  gfl2::heap::HeapBase*     m_pEventLocalHeapBase;    /**<  Event用LocalHeap              */
  
  pml::PokeParty*           m_pTemporaryParty;
  App::Status::APP_PARAM    m_ProcParam;
  App::Status::Proc*        m_pProc;

};


} /*  namespace JoinFestaPokeStatus */
} /*  namespace NetEvent            */
#endif  /*  #if !defined(JOINFESTAPOKESTATUSEVENT_H_INCLUDED)  */
