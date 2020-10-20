//======================================================================
/**
 * @file	KawaigariDirtObject.C++
 * @brief	Kawaigari Dirt Object Code
 * @author	PETE
 * @data	2015.12.15
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariDirtObject::KawaigariDirtObject(void)
: KawaigariTroubleObject()
{
  
}

KawaigariDirtObject::~KawaigariDirtObject(void)
{  
}

void KawaigariDirtObject::State_Idle(void)
{
  const float frames[3] = { 12.0f,12.0f,5.0f };
  const float dec[3] = { 0.5f, 0.25f, 0.25f };
  const int wait[3] = {20, 30, 10 };
 
  float speed[3]; 

  for(int n=0;n<3;n++)
  {
    speed[n] = dec[n] / frames[n];
  }

  if(coreSys->GetPokemonAI()->GetCareToolType()!=CARETOOL_DIRT) return;

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
    DoFinishEffect();
    SetState(AI_TROUBLE_STATE_END);
    mScrub = false;    
  }

}

void KawaigariDirtObject::State_Main(void)
{
  
}

void KawaigariDirtObject::State_End(void)
{
  
}


void KawaigariDirtObject::State_Cleanup(void)
{

}

void KawaigariDirtObject::State_Finish(void)
{

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)