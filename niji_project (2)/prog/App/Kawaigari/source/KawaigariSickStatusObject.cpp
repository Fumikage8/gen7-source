//======================================================================
/**
 * @file	KawaigariSickStatusObject.C++
 * @brief	Kawaigari Sick Status Object Code (Fire, Poison, Ice, Etc)
 * @author	PETE
 * @data	2015.2.19
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"

// Sound:
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariSickStatusObject::KawaigariSickStatusObject(int effectNum)
: KawaigariTroubleObject()
{
  mEffectNum = effectNum;
}

KawaigariSickStatusObject::~KawaigariSickStatusObject(void)
{  
}

int KawaigariSickStatusObject::GetEffectNum(void)
{ 
  return(mEffectNum); 
}

void KawaigariSickStatusObject::State_Idle(void)
{
  const float frames[3] = { 12.0f*3.0f,12.0f*3.0f,5.0f*3.0f };
  const float dec[3] = { 0.5f, 0.25f, 0.25f };
  const int wait[3] = {20*3.0f, 30*3.0f, 10*3.0f };

  float speed[3]; 

  for(int n=0;n<3;n++)
  {
    speed[n] = dec[n] / frames[n];
  }

  if(coreSys->GetPokemonAI()->GetCareToolType()!=CARETOOL_BANDAGE) return;

  switch(mStateRegSub)
  {
    case 0:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);
      mStateRegSub++;
      break;

    case 1:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)wait[0]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
        }
      }
      break;

    case 2:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mBaseScale-=speed[0];
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)frames[0]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
        }
      }
      break;

    case 3:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)wait[1]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
        }
      }
      break;

    case 4:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mBaseScale-=speed[1];
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)frames[1]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
       //   mStateRegSub=7;
       //   mBaseScale=0.0f;
        }
      }
      break;      
    case 5:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)wait[2]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
        }
      }
      break;

    case 6:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);

      if(mScrub)
      {
        mScrub=false;
        mBaseScale-=speed[2];
        mStateRegs[0]++;

        if(mStateRegs[0]>=(int)frames[2]) 
        {
          mStateRegs[0]=0;
          mStateRegSub++;
        }
      }
      break;
      
    case 7:
      if(ScrubFinished()) SetState(AI_TROUBLE_STATE_END);
      break;
  }


  if(ScrubFinished())
  {
    coreSys->CreateEffect(KAWAIGARI_EFFECT_CLEANSUCCESS);
    coreSys->GetPokemonAI()->PlaySE( SEQ_SE_KW_OTEIRE_CURED );
    if(IsFreezeStatus()) coreSys->GetPokeModel()->SetAnimationStepFrame(1.0f);
    SetState(AI_TROUBLE_STATE_END);
    mScrub = false;
  }
}

void KawaigariSickStatusObject::State_Main(void)
{
  
}

void KawaigariSickStatusObject::State_End(void)
{
  
}


void KawaigariSickStatusObject::State_Cleanup(void)
{

}

void KawaigariSickStatusObject::State_Finish(void)
{

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)