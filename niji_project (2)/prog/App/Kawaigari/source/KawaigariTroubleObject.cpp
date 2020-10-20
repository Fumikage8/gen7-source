//======================================================================
/**
 * @file	KawaigariTroubleObject.C++
 * @brief	Kawaigari Trouble Object Code
 * @author	PETE
 * @data	2015.1.21
 */
//======================================================================
#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <GameSys/include/GameProcManager.h>

#include "PokeTool/include/PokeModel.h"

#include "KawaigariMain.h"
#include "KawaigariRenderSystem.h"

// Sound:
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariTroubleObject::KawaigariTroubleObject(void)
{
  mPlayFinishSound = false;
  mSetupFinished = false;
  mModel = NULL;
  mIndex = 0;
  mEffectID = -1;
  SetState(AI_TROUBLE_STATE_INIT);
  SaveState();
  mBaseScale = 1.0f;
  mScrub = false;
  mShake = false;
  mIsStatusEffect = false;
}

KawaigariTroubleObject::~KawaigariTroubleObject(void)
{  
}

void KawaigariTroubleObject::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;
  
}
void KawaigariTroubleObject::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariTroubleObject::ConvertWorldPointToScreenPoint(f32 &resultX,f32 &resultY,gfl2::math::Vector3 world_point)
{
  gfl2::math::Vector3 world_point_3(world_point.x, world_point.y, world_point.z);

  gfl2::math::Matrix44 proj_mtx;
  gfl2::math::Matrix34 view_mtx;

  coreSys->GetRenderSystem()->GetViewAndProjectionMatrix(view_mtx,proj_mtx);

  gfl2::math::Matrix44 view_matrix(view_mtx);
  gfl2::math::Matrix44 view_projection_matrix  = proj_mtx * view_matrix;

  gfl2::math::Vector3 screen_point_3 = view_projection_matrix.TransformCoord(world_point_3);

  gfl2::math::Vector2 screen_point_2;
  {
    screen_point_2.x =   screen_point_3.x;  // 縦横を入れ替えない
    screen_point_2.y =   - screen_point_3.y;
  }

  // 0.0f<= <=1.0fに変更する(ビューポートで範囲を制限していないレンダーターゲット全体のスクリーン座標)
  resultX = screen_point_2.x / 2.0f + 0.5f;
  resultY = screen_point_2.y / 2.0f + 0.5f;
}

bool KawaigariTroubleObject::CheckDistance(float maxDist)
{
  if(ScrubFinished()) return(false);
  if(GetFixType() !=coreSys->GetPokemonAI()->GetCareToolType()) return(false);
  
  if(IsStatusEffect())
  {      
    if(coreSys->GetPokemonAI()->GetTouchColor()) return(true);
    return(false); // Not touching body
  }

  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );
  u32 touchX = 0,touchY = 0;

  if(touchPanel->IsTouch())
  {
    touchX = touchPanel->GetX();
    touchY = touchPanel->GetY();
  }

  if(GetEffectNum()==KAWAIGARI_EFFECT_WATER)
  {
    touchX -= 44;  
  }

  gfl2::math::Vector3 vec,vecSource;

  f32 fScreenX,fScreenY;
  GetPosition(vecSource);
  ConvertWorldPointToScreenPoint(fScreenX,fScreenY,vecSource);

  fScreenX *= 319.0f;
  fScreenY *= 239.0f;

  vecSource.x = fScreenX;
  vecSource.y = fScreenY;
  vecSource.z = 0.0f;

  vec.x = (float)touchX;
  vec.y = (float)touchY;
  vec.z = 0.0f;
  
  vec-=vecSource;
  GFL_PRINT("*TouchPos: %i,%i  NodePos2D:%i,%i  DIST %f*\n",touchX,touchY,(int)fScreenX,(int)fScreenY,vec.Length());

  float dist = vec.Length();

  return(dist<maxDist);
}

void KawaigariTroubleObject::PrintState(void)
{

  switch(mStateReg)
  {    
  case AI_TROUBLE_STATE_INIT: GFL_PRINT("STATE_INIT "); break;
  case AI_TROUBLE_STATE_MAIN: GFL_PRINT("STATE_MAIN ");  break;
  case AI_TROUBLE_STATE_IDLE: GFL_PRINT("STATE_IDLE "); break;  
  case AI_TROUBLE_STATE_END: GFL_PRINT("STATE_END "); break;
  case AI_TROUBLE_STATE_CLEANUP: GFL_PRINT("STATE_CLEANUP "); break;
  }

  GFL_PRINT("(%i) -> ",mStateRegSub);

  for(int r=0;r<MAX_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");

}

void KawaigariTroubleObject::SaveState(void)
{ 
  mStateRegBackup=mStateReg; 
  mStateRegSubBackup=mStateRegSub; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegsBackup[r] = mStateRegs[r];  
}

void KawaigariTroubleObject::RestoreState(void)
{ 
  mStateReg=mStateRegBackup; 
  mStateRegSub=mStateRegSubBackup; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = mStateRegsBackup[r];  
}

void KawaigariTroubleObject::Setup(int index,bool isStatusEffect)
{
  mIsStatusEffect = isStatusEffect;
 
  int jIndex = 0;

  if(isStatusEffect)
  {    
    index = 0;
  }
  else  mNode = coreSys->GetDataManager()->GetJointID(index);

  mNode = coreSys->ConvertCareModeJointToPokeJoint(mNode,jIndex); // Convert to actual joint
  if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) mNode = PokeTool::MODEL_NODE_OVERHEAD;
  else if(mIsStatusEffect) mNode = PokeTool::MODEL_NODE_CENTER;

  int n = coreSys->CreateCareEffect(GetEffectNum());

  if(n==-1) return;

  mEffectID = n;
  mIndex = index;

  mEffect = coreSys->GetCareEffect(n);

  gfl2::math::Vector3 pos;
  mModel->GetEffectNodePosition((PokeTool::MODEL_NODE)mNode,jIndex,&pos);  
    
  //mStartScale = 0.1f;
 // float zOff = 50.0f * mModel->GetAdjustScale();
//  mEffect->SetOffset(0.0f,10.0f,18.0f+zOff);
 // mEffect->SetScale(mStartScale,mStartScale,mStartScale);

 
  if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) coreSys->UpdateSleepEffectPos(n);
  else mEffect->SetPosition(pos);

  // Get Position & Scale from Data:
  {  
    coreSys->GetDataManager()->GetJointPos(mIndex,pos);

    if(isStatusEffect)
    {
      pos.x = pos.y = 0.0f;
      pos.z = 0;//10.0f;
    }

    float zOff = 50.0f * mModel->GetAdjustScale();

    if(!isStatusEffect)
      pos.z += zOff;
    
    mEffect->SetOffset(pos);
    if(isStatusEffect && GetEffectNum()!=KAWAIGARI_EFFECT_SLEEPY)
    {
      gfl2::math::Vector3 finalPos = mEffect->GetPosition();

      gfl2::math::Vector3 camPos,tgtPos,upVec,camVec;
      coreSys->GetCamera()->GetPosition(camPos,tgtPos,upVec);

      camVec = finalPos;
      camVec -= camPos;
      camVec.Normalize();

      if(coreSys->GetPokemonAI()->CheckGroup(GRP_GIANT) || coreSys->GetPokemonAI()->CheckGroup(GRP_WHALEOH) )
      {      
        finalPos.x -= camVec.x*0.0f;
        finalPos.y -= camVec.y*0.0f;
        finalPos.z -= camVec.z*0.0f;
      }
      else
      {      
        finalPos.x -= camVec.x*2.0f;
        finalPos.y -= camVec.y*2.0f;
        finalPos.z -= camVec.z*2.0f;
      }

      mEffect->SetPosition(finalPos);

      finalPos.z += mEffect->GetOffset().GetZ();      
      mStartScale = 1.1f;

      if(coreSys->GetPokemonAI()->CheckGroup(GRP_GIANT) || coreSys->GetPokemonAI()->CheckGroup(GRP_WHALEOH) )
      {        
        mStartScale = 2.5f;
      }

      mStartScale *= coreSys->GetCalculatedEffectScale(finalPos);
    }
    else mStartScale = coreSys->GetDataManager()->GetScale(mIndex);  
  }

  if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) mStartScale = 1.0f;

  mEffect->SetScale(mStartScale,mStartScale,mStartScale);

  mSetupFinished = true;
}

void KawaigariTroubleObject::GetPosition(gfl2::math::Vector3 &pos)
{
  gfl2::math::Vector3 v;
  mEffect->GetCalcPosition(&v);
  pos = v;
}

float KawaigariTroubleObject::GetFinalScale(void) 
{ 
  float scale = mStartScale*mBaseScale;

  scale *= coreSys->GetPokeModel()->GetScale().GetX();

  if(coreSys->GetPokeModel()->GetScale().GetX()!=1.0f)
  {
    mEffect->SetVisible(false);
    return(0.00f);
  }
  
  if(scale>0.01f) return(scale);  
  else mEffect->SetVisible(false);

  return(0.00f);
}

void KawaigariTroubleObject::State_Init(void)
{  
  SetState(AI_TROUBLE_STATE_IDLE);
  UpdateAgain();
}

void KawaigariTroubleObject::DoFinishEffect(void)
{
  gfl2::math::Vector3 finalPos;

  mEffect->GetCalcPosition(&finalPos);
  coreSys->CreateEffect(KAWAIGARI_EFFECT_CLEANSPOT,&finalPos);
  mPlayFinishSound = true;
}

void KawaigariTroubleObject::Update(void)
{ 
  if(mModel==NULL) return; // Need a valid pokemon model to proceed!

  int jIndex = 0;

  // Update Model Position:
  {
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

    // Get Position & Scale from Data:
    {  
      gfl2::math::Vector3 pos;
      coreSys->GetDataManager()->GetJointPos(mIndex,pos);

      if(mIsStatusEffect)
      {
        pos.x = pos.y = 0.0f;
        pos.z = 0;//10.0f;
      }

      float zOff = 50.0f * mModel->GetAdjustScale();
      
      if(!mIsStatusEffect) pos.z += zOff;
      
      mEffect->SetOffset(pos);
      mFinalOffset = pos;

      if(!mIsStatusEffect)
        mStartScale = coreSys->GetDataManager()->GetScale(mIndex);  
      

      

      mNode = coreSys->GetDataManager()->GetJointID(mIndex);
      mNode = coreSys->ConvertCareModeJointToPokeJoint(mNode,jIndex); // Convert to actual joint
      if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) { mNode = PokeTool::MODEL_NODE_OVERHEAD; jIndex = 0; }
      else if(mIsStatusEffect)
      {
        jIndex = 0;
        mNode = PokeTool::MODEL_NODE_CENTER;        
      }


    }

    gfl2::math::Vector3 pos;
    mModel->GetEffectNodePosition((PokeTool::MODEL_NODE)mNode,jIndex,&pos);  

    if(mShake && coreSys->GetRandom(2)==0)
    {
      float xoff,yoff;
      xoff = (float)(coreSys->GetRandom(4) - 2.0f);
      yoff = (float)(coreSys->GetRandom(4) - 2.0f);
      xoff *= 0.5f;
      yoff *= 0.5f;
      pos.x+=xoff;
      pos.y+=yoff;
      mShake = false;
    }

    if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) coreSys->UpdateSleepEffectPos(mEffectID);
    else mEffect->SetPosition(pos);    

    mFinalPosition = pos;


    if(mIsStatusEffect)
    {
      gfl2::math::Vector3 finalPos = mFinalPosition;
    
      gfl2::math::Vector3 camPos,tgtPos,upVec,camVec;
      coreSys->GetCamera()->GetPosition(camPos,tgtPos,upVec);

      camVec = finalPos;
      camVec -= camPos;
      camVec = camVec.Normalize();      

      float MOVEDIST = 5.0f;

      if(coreSys->GetPokemonAI()->IsCharacterID(MONSNO_KUWAGATA2)) MOVEDIST = -35.0f;
       
      if(coreSys->GetPokemonAI()->CheckGroup(GRP_WHALEOH))
      {        
        if(GetEffectNum()!=KAWAIGARI_EFFECT_SLEEPY) MOVEDIST = -50.0f;
        finalPos.x += camVec.x*MOVEDIST;
        finalPos.y += camVec.y*MOVEDIST;
        finalPos.z += camVec.z*MOVEDIST;
      }

      else if(coreSys->GetPokemonAI()->CheckGroup(GRP_GIANT)  )
      {
        if(GetEffectNum()!=KAWAIGARI_EFFECT_SLEEPY) MOVEDIST = 0.0f;
        finalPos.x += camVec.x*MOVEDIST;
        finalPos.y += camVec.y*MOVEDIST;
        finalPos.z += camVec.z*MOVEDIST;
      }
      else if(coreSys->GetPokemonAI()->IsCharacterID(MONSNO_MAYU)) 
      {        
        MOVEDIST = 0.0f;
        finalPos.x += camVec.x*MOVEDIST;
        finalPos.y += camVec.y*MOVEDIST;
        finalPos.z += camVec.z*MOVEDIST;
      }
      else
      {
        finalPos.x += camVec.x*MOVEDIST;
        finalPos.y += camVec.y*MOVEDIST;
        finalPos.z += camVec.z*MOVEDIST;
      }
      
      if(GetEffectNum()!=KAWAIGARI_EFFECT_SLEEPY)
      {
        mEffect->SetPosition(finalPos);
        mFinalPosition = finalPos;  
      }
           
      mStartScale = 1.1f;

      if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY)
      {
        mStartScale = 1.1f;
      }
      else if(coreSys->GetPokemonAI()->CheckGroup(GRP_GIANT) || coreSys->GetPokemonAI()->CheckGroup(GRP_WHALEOH))
      {        
        mStartScale = 2.5f;
      }

      mStartScale *= coreSys->GetCalculatedEffectScale(finalPos,true);

      // EXCEPTION FOR MONSNO_790            
      if(coreSys->GetPokemonAI()->IsCharacterID(MONSNO_MAYU)) mStartScale = 0.175f;
    }
    
  }

  if(GetEffectNum()==KAWAIGARI_EFFECT_SLEEPY) 
  {    
    mStartScale = 0.75;
  }

  mEffect->SetScale(GetFinalScale(),GetFinalScale(),GetFinalScale());

  
  if(!mSetupFinished)
  {
    Setup(mIndex);
    return;
  }

  if(ScrubFinished() && GetState()==AI_TROUBLE_STATE_END)
  {
    switch(GetEffectNum())
    {
      case KAWAIGARI_EFFECT_POISON:
      case KAWAIGARI_EFFECT_FIRE:
      case KAWAIGARI_EFFECT_ICE:
      case KAWAIGARI_EFFECT_PARALYSIS:
      case KAWAIGARI_EFFECT_SLEEPY:
        coreSys->CurePokemon(); break;

      default: coreSys->CleanPokemonPart(mIndex); break;
    }
    
  }

  do 
  {
    mRequireUpdate = false;
  
    switch(mStateReg)
    {
      case AI_TROUBLE_STATE_INIT: State_Init(); break;
      case AI_TROUBLE_STATE_MAIN: State_Main(); break;
    
      case AI_TROUBLE_STATE_IDLE: State_Idle(); break;
      
      case AI_TROUBLE_STATE_END: State_End(); break;
  
      case AI_TROUBLE_STATE_CLEANUP: State_Cleanup(); break;
      case AI_TROUBLE_STATE_FINISH: State_Finish(); break;
    }

    // Print State Debug:
//    PrintState();

  } while (mRequireUpdate);


  if(mPlayFinishSound)
  {
    Sound::StopSE(SEQ_SE_KW_STROKE,0,1);
    Sound::StopSE(SEQ_SE_KW_OTEIRE_OMNISHEET,0,1);
    Sound::StopSE(SEQ_SE_KW_OTEIRE_COMB,0,1);
    Sound::StopSE(SEQ_SE_KW_OTEIRE_TOWEL,0,1);
    Sound::StopSE(SEQ_SE_KW_OTEIRE_BRUSH,0,1);        
    Sound::PlaySE(SEQ_SE_KW_OTEIRE_CURED);    
    mPlayFinishSound = false;
  }
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)