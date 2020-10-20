//======================================================================
/**
 * @file	KawaigariDustObject.C++
 * @brief	Kawaigari Dust Object Code
 * @author	PETE
 * @data	2015.1.21
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariDustObject::KawaigariDustObject(void)
: KawaigariTroubleObject()
{
  
}

KawaigariDustObject::~KawaigariDustObject(void)
{  
}



void KawaigariDustObject::State_Idle(void)
{
  const float frames[3] = { 12.0f,12.0f,5.0f };
  const float dec[3] = { 0.5f, 0.25f, 0.25f };
  const int wait[3] = {20, 30, 10 };

  const int DUST_TIMING = 5;

  float speed[3]; 
  
  gfl2::math::Vector3 pos;   
  mEffect->GetCalcPosition(&pos);

  for(int n=0;n<3;n++)
  {
    speed[n] = dec[n] / frames[n];
  }

  if(coreSys->GetPokemonAI()->GetCareToolType()!=CARETOOL_DUST) return;

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

        if(mStateRegs[0]%DUST_TIMING==0) 
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

        if(mStateRegs[0]%DUST_TIMING==0)
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

        if(mStateRegs[0]%DUST_TIMING==0)
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

        if(mStateRegs[0]%DUST_TIMING==0)
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

        if(mStateRegs[0]%DUST_TIMING==0)
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

        if(mStateRegs[0]%DUST_TIMING==0)
        {
          coreSys->CreateEffect(KAWAIGARI_EFFECT_DUST_EFFECT,&pos);
        }

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

void KawaigariDustObject::State_Main(void)
{
  
}

void KawaigariDustObject::State_End(void)
{
  
}


void KawaigariDustObject::State_Cleanup(void)
{

}

void KawaigariDustObject::State_Finish(void)
{

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)