//======================================================================
/**
 * @file	KawaigariRandomActionSelector.C++
 * @brief	Random Action Selector for Pokemon AI
 * @author	PETE
 * @data	2015.10.26
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <heap/include/gfl2_Heap.h>
#include "KawaigariRandomActionSelector.h"

#include "KawaigariMain.h"
#include "KawaigariPokemon.h" // For SubStates

#include "data/KawaigariParamAffectionManager.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariRandomActionSelector::KawaigariRandomActionSelector(void)
{  
  coreSys = NULL;  
  Reset();  
}

KawaigariRandomActionSelector::~KawaigariRandomActionSelector(void)
{  
}

void KawaigariRandomActionSelector::Reset(void)
{
  numCommands = 0;
  curSelection = -1;
  memset(comTable,0,sizeof(comTable));
  memset(motionTable,0,sizeof(motionTable));
}

bool KawaigariRandomActionSelector::SelectRandomAction(int &action)
{
  if(numCommands==0) return(false);
  if(curSelection != -1) return(false);

  curSelection = coreSys->GetRandom(numCommands);
  action = comTable[curSelection];
  return(true);
}

bool KawaigariRandomActionSelector::GetSelectedAction(int &action)
{
  if(numCommands==0) return(false);
  if(curSelection == -1) return(false);
  action = comTable[curSelection];
  return(true);
}


bool KawaigariRandomActionSelector::GetSelectedMotion(int &motion)
{
  if(numCommands==0) return(false);
  if(curSelection == -1) return(false);
  if(motionTable[curSelection] == -1) return(false);
  motion = motionTable[curSelection];
  return(true);
}

void KawaigariRandomActionSelector::AddAppearCom(int com,int motion)
{
  if(numCommands>=MAX_ACTIONS) return; // Too many in list already

  // Check for valid motion and valid command here: (By Affection Param)   
  if(!CanUseAppearCommand(com)) return;
  if(!CanUseMotion(motion)) return;

  comTable[numCommands] = com;
  motionTable[numCommands] = motion;
  numCommands++;
}

bool KawaigariRandomActionSelector::CanUseAppearCommand(int command)
{
  
  switch(command)
  {
  //  case AI_SUBSTATE_APPEAR_UPPER:
  //    if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_UPPER) ) return(false);
  //    break;

    case AI_SUBSTATE_APPEAR_LOWER:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_LOWER) ) return(false);
      break;

   // case AI_SUBSTATE_APPEAR_BEHIND:
    //  if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_BEHIND) ) return(false);
    //  break;

    case AI_SUBSTATE_APPEAR_LEFT:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_LEFT) ) return(false);
      break;

    case AI_SUBSTATE_APPEAR_RIGHT:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_RIGHT) ) return(false);
      break;

    case AI_SUBSTATE_APPEAR_DO_UP:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_DO_UP) ) return(false);
      break;

    case AI_SUBSTATE_APPEAR_BACK:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_BACK) ) return(false);
      break;

    case AI_SUBSTATE_APPEAR_DROWSEY:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_ROW) ) return(false);
      break;

    case AI_SUBSTATE_APPEAR_SLEEP:
      if(coreSys->GetPokemonAI()->GetAffectionLevel() < coreSys->GetDataManager()->GetAffectionRequired(COM_APPEAR_SLEEP) ) return(false);
      break;

      /*
    case AI_SUBSTATE_APPEAR_LOOKAROUND:
      break;
    case AI_SUBSTATE_APPEAR_PLAYMOTION:
      break;
    case AI_SUBSTATE_APPEAR_NONE:
      break;*/

    default:
      break;
  }
  return(true);
}


bool KawaigariRandomActionSelector::CanUseIdleCommand(int command)
{  
  switch(command)
  {   
    case AI_SUBSTATE_IDLE_INIT:
      break;
    case AI_SUBSTATE_IDLE_MAIN:
      break;
    case AI_SUBSTATE_IDLE_PLAYMOTION:
      break;
    case AI_SUBSTATE_IDLE_LOOKAROUND:
      break;
    case AI_SUBSTATE_IDLE_SLEEP:
      break;
    default:
      break;
  }
  return(true);
}

void KawaigariRandomActionSelector::AddIdleCom(int com,int motion)
{
  if(numCommands>=MAX_ACTIONS) return; // Too many in list already

  // Check for valid motion and valid command here: (By Affection Param)   
  if(!CanUseIdleCommand(com)) return;
  if(!CanUseMotion(motion)) return;

  comTable[numCommands] = com;
  motionTable[numCommands] = motion;
  numCommands++;
}

bool KawaigariRandomActionSelector::CanUseMotion(int motion)
{
  if(motion!=-1)
  {
    // Exception for MonsNo:597: (Check directly for motion)
    if(coreSys->GetPokemonAI()->IsCharacterID(MONSNO_TESSIIDO))
    {
      return(coreSys->GetPokemonAI()->CanSetMotion(motion));
    }

    switch(motion)
    {
      case PokeTool::MODEL_ANIME_QUESTION: return(true);

      case PokeTool::MODEL_ANIME_HAPPY_A:
      case PokeTool::MODEL_ANIME_HAPPY_C:
      case PokeTool::MODEL_ANIME_PLAY_A:
      case PokeTool::MODEL_ANIME_PLAY_B:
        if(coreSys->GetPokemonAI()->CanSetMotion(motion)==true) return(true); 
        return(coreSys->GetPokemonAI()->CanSetMotion(PokeTool::MODEL_ANIME_HAPPY_B));

      case PokeTool::MODEL_ANIME_TURN_B:
        if(coreSys->GetPokemonAI()->CanSetMotion(motion)==true) return(true); 
        return(coreSys->GetPokemonAI()->CanSetMotion(PokeTool::MODEL_ANIME_TURN_A));
        

      default:
        if(coreSys->GetPokemonAI()->CanSetMotion(motion)==false) return(false); 
    }
  }
  return(true);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)