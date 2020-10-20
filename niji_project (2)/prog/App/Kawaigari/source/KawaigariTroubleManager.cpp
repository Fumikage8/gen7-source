//======================================================================
/**
 * @file	KawaigariTroubleManager.C++
 * @brief	Kawaigari Trouble Manager Code
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

KawaigariTroubleManager::KawaigariTroubleManager(void)
{
  mCanUpdate = true;
  mEnable = true;
  for(int z=0;z<MAX_CAREPROBLEMS;z++) objs[z] = NULL;
}

KawaigariTroubleManager::~KawaigariTroubleManager(void)
{  
}

bool KawaigariTroubleManager::HasFreezeStatus(void) 
{ 
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]==NULL) continue;
    if(objs[z]->IsFreezeStatus() && !objs[z]->ScrubFinished()) return(true);
  }
  return(false); 
}

bool KawaigariTroubleManager::SuccessSoundPlay(void) 
{ 
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]==NULL) continue;
    if(objs[z]->PlayFinishSound()) return(true);
  }
  return(false); 
}


bool KawaigariTroubleManager::HasSleepyStatus(void) 
{ 
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]==NULL) continue;
    if(objs[z]->IsSleepyStatus() && !objs[z]->ScrubFinished()) return(true);
  }
  return(false); 
}

bool KawaigariTroubleManager::HasStatusEffect(void) 
{ 
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]==NULL) continue;
    if(objs[z]->IsStatusEffect() && !objs[z]->ScrubFinished()) return(true);
  }
  return(false); 
}

void KawaigariTroubleManager::Finalize(void) 
{
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    GFL_SAFE_DELETE(objs[z]);
  }
}

void KawaigariTroubleManager::Update(void)
{ 
  if(coreSys==NULL) return; // Need a valid core
  if(!mEnable) return;
  if(!mCanUpdate) return;

  // Update all Care Objects:
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {
      objs[z]->Update();
    }
  }

}

bool KawaigariTroubleManager::ScrubFinished(void)
{
  bool result = true;

  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {
      result &= objs[z]->ScrubFinished();
    }
  }

  return(result);
}

bool KawaigariTroubleManager::HasObjects(void)
{
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL) 
    {
        if(!objs[z]->IsFinished()) return(true);
    }
  }
  return(false);
}

void KawaigariTroubleManager::Scrub(float dist)
{
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {
      if(objs[z]->CheckDistance(dist)) objs[z]->Scrub();
    }
  }
}

bool KawaigariTroubleManager::CheckDistance(float dist)
{
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {
      if(objs[z]->CheckDistance(dist)) return(true);
    }
  }
  return(false);
}

#if PM_DEBUG  
void KawaigariTroubleManager::TurnOff(void)
{
  mEnable = false;
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {      
      gfl2::math::Vector3 scl = objs[z]->mEffect->GetScale();
      scaleBackup[z] = scl.x;
      objs[z]->mEffect->SetScale(0.0f,0.0f,0.0f);      
      break;
    }
    else scaleBackup[z] = 1.0f;
  }
}

void KawaigariTroubleManager::TurnOn(void)
{
  mEnable = true;
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL && objs[z]->mEffect!=NULL)
    {            
      objs[z]->mEffect->SetScale(scaleBackup[z],scaleBackup[z],scaleBackup[z]);            
    }
  }
}
#endif

void KawaigariTroubleManager::AddObject(int type,int index)
{
  int source = -1;

  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]!=NULL)
    {
      if(objs[z]->GetIndex()==index) 
      {
        if(!objs[z]->ScrubFinished())        
          return; // Already Object at index!

        else
        {
          GFL_SAFE_DELETE(objs[z]);
          objs[z] = NULL;
        }
      }
    }
  }

  // Find Available Slot:
  for(int z=0;z<MAX_CAREPROBLEMS;z++)
  {
    if(objs[z]==NULL)
    {
      source = z;
      break;
    }
  }

  if(source==-1) return; // Too many objects already created

  bool isStatusEffect = false;

  switch(type)
  {
    case CARETROUBLE_DIRT:
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariDirtObject();      
      break;

    case CARETROUBLE_HAIR:
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariHairObject();
      break;

    case CARETROUBLE_WATER:
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariWaterObject();
      break;

    case CARETROUBLE_DUST:
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariDustObject();
      break;

    case CARETROUBLE_FIRE:
      isStatusEffect = true;
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariSickStatusObject(KAWAIGARI_EFFECT_FIRE);
      break;

    case CARETROUBLE_ICE:
      isStatusEffect = true;
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariSickStatusObject(KAWAIGARI_EFFECT_ICE);
      break;

    case CARETROUBLE_POISON:
      isStatusEffect = true;
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariSickStatusObject(KAWAIGARI_EFFECT_POISON);
      break;

    case CARETROUBLE_PARALYSIS:
      isStatusEffect = true;
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariSickStatusObject(KAWAIGARI_EFFECT_PARALYSIS);
      break;

    case CARETROUBLE_SLEEPY:
      isStatusEffect = true;
      objs[source] = GFL_NEW(coreSys->GetHeap())KawaigariSickStatusObject(KAWAIGARI_EFFECT_SLEEPY);
      break;
      
  }
  if(objs[source] == NULL) return; // Not allocated properly!

  objs[source]->SetCoreSys(coreSys);
  objs[source]->SetPokeModel(coreSys->GetPokeModel());
  objs[source]->Setup(index,isStatusEffect);

}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)