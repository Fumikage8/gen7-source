//======================================================================
/**
 * @file	KawaigariDebugManager.C++
 * @brief	Kawaigari All Debug Mode Manager Code (Camera, CareMode, etc)
 * @author	PETE
 * @data	2015.10.27
 */
//======================================================================
#if defined(GF_PLATFORM_WIN32)
#if PM_DEBUG 

#include "../KawaigariMain.h"
#include "System/include/GflUse.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

// 2016.04.07 tamada追記　ポケモンNo上限のMagicNumber
#include "niji_conv_header/poke_lib/monsno_def.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)



KawaigariDebugManager::KawaigariDebugManager(void)
{      

  mWriteBuffer = NULL;

  SetState(DEBUGMANAGER_STATE_INIT);

  mEnable = false;
  mEffectType = 0;

  

}

KawaigariDebugManager::~KawaigariDebugManager(void)
{    
}

void KawaigariDebugManager::ToggleEnable(void)
{
  if(mEnable && mStateReg!=DEBUGMANAGER_STATE_IDLE) return;

  if(mEnable) 
  {
    mEnable = false;
    coreSys->TextUtil()->ClearDrawText();
  }
  else mEnable = true;
}

void KawaigariDebugManager::SetState(int s) 
{ 
  mStateReg=s; 
  mStateRegSub=0; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;

}
void KawaigariDebugManager::SetSubState(int s) 
{ 
  mStateRegSub=s; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = 0;
}

void KawaigariDebugManager::PrintState(void)
{
  
  GFL_PRINT("DebugManager(%i-%i) -> ",mStateReg,mStateRegSub);

  for(int r=0;r<MAX_STATE_REGS;r++)
  {
    if(r && (r&3)==0) GFL_PRINT("  ");
    GFL_PRINT("%03i ",mStateRegs[r]);    
  }

  GFL_PRINT("\n");

}

void KawaigariDebugManager::SaveState(void)
{ 
  mStateRegBackup=mStateReg; 
  mStateRegSubBackup=mStateRegSub; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegsBackup[r] = mStateRegs[r];  
}

void KawaigariDebugManager::RestoreState(void)
{ 
  mStateReg=mStateRegBackup; 
  mStateRegSub=mStateRegSubBackup; 
  for(int r=0;r<MAX_STATE_REGS;r++) mStateRegs[r] = mStateRegsBackup[r];  
}

bool KawaigariDebugManager::Finalize(void)
{
  if(GetState()==DEBUGMANAGER_STATE_END) return(true);
  SetState(DEBUGMANAGER_STATE_END);
    
  GFL_SAFE_DELETE_ARRAY( mWriteBuffer );    

  mEnable = false;
  return(true);
}


void KawaigariDebugManager::Update(void)
{ 
  if(!mEnable && mStateReg!=DEBUGMANAGER_STATE_INIT) return;

  do 
  {
    mRequireUpdate = false;

    switch(mStateReg)
    {
    case DEBUGMANAGER_STATE_INIT: State_Init(); break;
    case DEBUGMANAGER_STATE_MAIN: State_Main(); break;
    case DEBUGMANAGER_STATE_IDLE: State_Idle(); break;
    case DEBUGMANAGER_STATE_RELOADPOKEMON: State_ReloadPokemon(); break;
    case DEBUGMANAGER_STATE_CAREMODE: State_CareMode(); break;
    case DEBUGMANAGER_STATE_CAMERASET: State_CameraSet(); break;
    case DEBUGMANAGER_STATE_EATVIBRATIONSET: State_EatVibrationSet(); break;

    case DEBUGMANAGER_STATE_END: State_End(); break;

    case DEBUGMANAGER_STATE_CLEANUP: State_Cleanup(); break;
    case DEBUGMANAGER_STATE_FINISH: State_Finish(); break;
    }

    // Print State Debug:
    //    PrintState();

  } while (mRequireUpdate);

}

void KawaigariDebugManager::State_Init(void)
{
  if(coreSys==NULL) return;

  switch(mStateRegSub)
  {
    case 0: // Init Font:

      // PC Write Buffer:
      {
        mWriteBuffer = GFL_NEW( coreSys->GetHeap() ) char[WRITEBUFFER_SIZE];        
      }

      {      
        int n;
        n = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_DIRT);
        mEffect[0] = coreSys->GetCareEffect(n);

        n= coreSys->CreateCareEffect(KAWAIGARI_EFFECT_DUST);
        mEffect[1] = coreSys->GetCareEffect(n);

        n = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_HAIR);
        mEffect[2] = coreSys->GetCareEffect(n);

        n = coreSys->CreateCareEffect(KAWAIGARI_EFFECT_WATER);
        mEffect[3] = coreSys->GetCareEffect(n);

        for(int z=0;z<4;z++)
        {
          mEffect[z]->SetScale(0.0f);          
        }
      }

      SetState(DEBUGMANAGER_STATE_IDLE);
      break;
  }
}

void KawaigariDebugManager::State_Idle(void)
{
  int selectMax = 6;
  int xp=10,yp=0;
  int ySpace=22;
  wchar_t titleStr[] = L"Debug Main:";

  wchar_t *menuStr[] = { L"Reload Pokemon", L"Toggle Switches", L"CareMode Setting", L"Camera Setting",L"Eat Vibration Setting",L"Play All Motion" };

  switch(mStateRegSub)
  {
      case 0:
        mSelection = 0;
        mStateRegSub++;
        break;

      case 1:
        {
          gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
          gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

          if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ))
          {
            mSelection++;
            if(mSelection>=selectMax) mSelection=0;
          }
          else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ))
          {
            mSelection--;
            if(mSelection<0) mSelection=selectMax-1;
          }

          if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ))
          {
            switch(mSelection)
            {
            case 0: SetState(DEBUGMANAGER_STATE_RELOADPOKEMON); break; // Reload Pokemon:
            case 2: SetState(DEBUGMANAGER_STATE_CAREMODE); break; // Care Mode Settings:
            case 3: SetState(DEBUGMANAGER_STATE_CAMERASET); break; // Camera Setting
            case 4: SetState(DEBUGMANAGER_STATE_EATVIBRATIONSET); break; // Eat Vibration Setting
            
            case 5: 
              if(coreSys->GetPokemonAI()->GetState()!=AI_STATE_PLAY_ALL_MOTION)
                coreSys->GetPokemonAI()->SetState(AI_STATE_PLAY_ALL_MOTION); 
              break;
            }

            break;
          }

        }
        break;
  }

  gfl2::math::Vector4 col;

  coreSys->TextUtil()->ClearDrawText();
  col.Set(0.6f,0.1f,0.5f,1.0f); coreSys->TextUtil()->SetTextColor(col);
  coreSys->TextUtil()->SetTextScale( 1.1f, 1.1f );
  coreSys->TextUtil()->DrawText( xp,   yp, titleStr); yp+=ySpace+10;
  coreSys->TextUtil()->SetTextScale( 0.8f, 0.8f );

  for(int z=0;z<selectMax;z++)
  {
    if(mSelection==z) col.Set(1.0f,1.0f,0.0f,1.0f);
    else col.Set(1.0f,1.0f,1.0f,1.0f);

    coreSys->TextUtil()->SetTextColor(col);
    coreSys->TextUtil()->DrawText( xp,   yp, menuStr[z]); yp+=ySpace;
  }
  
  u8 play,affection,fullness;
  coreSys->GetPokemonAI()->GetParameters(affection,play,fullness);
  
  coreSys->TextUtil()->SetTextScale( 0.5f, 0.5f );
  col.Set(1.0f,1.0f,1.0f,1.0f); coreSys->TextUtil()->SetTextColor(col);

  int curCam = 0;
  curCam = coreSys->GetCamera()->GetCameraType();

  wchar_t *camStr[] = { L"KG_CAMERA_FAR", L"KG_CAMERA_WHOLE_BODY", L"KG_CAMERA_UP",
    L"KG_CAMERA_DO_UP",L"KG_CAMERA_LEFT",L"KG_CAMERA_RIGHT",L"KG_CAMERA_PEEP",L"KG_CAMERA_BEHIND",L"KG_CAMERA_HIT1",L"KG_CAMERA_EAT",L"KG_CAMERA_BUST_UP",L"KG_CAMERA_EFFECT", L"KG_CAMERA_ALLMOTION", L"KG_CAMERA_EAT_FIXED"}; 

  coreSys->TextUtil()->DrawText( xp,   200, L"AF:%i FD:%i ND:%i [%s]",affection,fullness,play,camStr[curCam]); 
  coreSys->TextUtil()->DrawText( xp,   185, L"Memory: %iK / %iK AllocOK:%iK",coreSys->GetHeap()->GetTotalFreeSize()/1024,coreSys->GetHeap()->GetTotalSize()/1024,coreSys->GetHeap()->GetTotalAllocatableSize()/1024); 

  coreSys->GetHeap()->GetTotalFreeSize();

}


void KawaigariDebugManager::State_ReloadPokemon(void)
{
//  const int MAX_MONSNO = 800;
  const int MAX_MONSNO = MonsNo::MONSNO_END;
  const int MAX_FORMNO = MAX_FORM_NUM;
  int selectMax = 4;
  int xp=10,yp=0;
  int ySpace=22;
  wchar_t titleStr[] = L"Load Pokemon:";

  wchar_t *menuStr[] = { L"MonsNo:", L"FormNo:", L"Sex:",L"Reload Pokemon" };

  switch(mStateRegSub)
  {
  case 0:
    mSelection = 0;
    mReloadMonsNo = mMonsNo;
    mReloadFormNo = mFormNo;
    mReloadSexNo = mSexNo;
    mStateRegSub++;
    break;

  case 1:
    {
      gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

      if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ))
      {
        mSelection++;
        if(mSelection>=selectMax) mSelection=0;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ))
      {
        mSelection--;
        if(mSelection<0) mSelection=selectMax-1;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
      {
        int add = 1;

        if( pButton->IsHold( gfl2::ui::BUTTON_R )) add = 10;
        else if( pButton->IsHold( gfl2::ui::BUTTON_L )) add = 100;

        // MonsNo:
        if(mSelection==0) mReloadMonsNo+=add;
        else if(mSelection==1) mReloadFormNo+=add;
        else if(mSelection==2) mReloadSexNo++;

        if(mReloadMonsNo>MAX_MONSNO) mReloadMonsNo=MAX_MONSNO;
        if(mReloadFormNo>MAX_FORMNO) mReloadFormNo=MAX_FORMNO;
        if(mReloadSexNo>2) mReloadSexNo = 0;
      }

      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
      {
        int add = 1;

        if( pButton->IsHold( gfl2::ui::BUTTON_R )) add = 10;
        else if( pButton->IsHold( gfl2::ui::BUTTON_L )) add = 100;

        // MonsNo:
        if(mSelection==0) mReloadMonsNo-=add;
        else if(mSelection==1) mReloadFormNo-=add;
        else if(mSelection==2) mReloadSexNo--;

        if(mReloadMonsNo<0) mReloadMonsNo=1;
        if(mReloadFormNo<0) mReloadFormNo=0;
        if(mReloadSexNo<0) mReloadSexNo=2;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL )) // Cancel back
      {
        SetState(DEBUGMANAGER_STATE_IDLE);
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR )) // Reload!
      {
        coreSys->StartReloadPokemon(mReloadMonsNo,mReloadFormNo,mReloadSexNo);
        SetState(DEBUGMANAGER_STATE_RELOADPOKEMON);
        break;
      }

    }
    break;
  }

  gfl2::math::Vector4 col;

  coreSys->TextUtil()->ClearDrawText();
  col.Set(0.6f,0.1f,0.5f,1.0f); coreSys->TextUtil()->SetTextColor(col);
  coreSys->TextUtil()->SetTextScale( 1.1f, 1.1f );
  coreSys->TextUtil()->DrawText( xp,   yp, titleStr); yp+=ySpace+10;
  coreSys->TextUtil()->SetTextScale( 0.8f, 0.8f );

  for(int z=0;z<selectMax;z++)
  {
    if(mSelection==z) col.Set(1.0f,1.0f,0.0f,1.0f);
    else col.Set(1.0f,1.0f,1.0f,1.0f);

    coreSys->TextUtil()->SetTextColor(col);

    if(z==0) // MonsNO
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"MonsNo: %i",mReloadMonsNo); 
    }
    else if(z==1) // FormNo
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"FormNo: %i",mReloadFormNo); 
    }
    else if(z==2) // Sex
    {
      if(mReloadSexNo == 0) coreSys->TextUtil()->DrawText( xp,   yp, L"Sex: MALE",mReloadFormNo); 
      else if(mReloadSexNo == 1) coreSys->TextUtil()->DrawText( xp,   yp, L"Sex: FEMALE",mReloadFormNo); 
      else if(mReloadSexNo == 2) coreSys->TextUtil()->DrawText( xp,   yp, L"Sex: UNKNOWN",mReloadFormNo); 
    }
    else coreSys->TextUtil()->DrawText( xp,   yp, menuStr[z]); 
    
    yp+=ySpace;
  }


}



void KawaigariDebugManager::State_CareMode(void)
{
  int jIndex = 0;
  static int cIndex = 0;
  static int px=0,py=0,pz=0;
  static int scale = 100;
  static int jointIndex = 0;
  float moveSpeed = 1.0f;
  int selectMax = 8;
  int xp=10,yp=0;
  int ySpace=22;
  const int jointMax = 10;
  wchar_t titleStr[] = L"CareMode Effect Position:";

  wchar_t *menuStr[] = { L"Index:", L"Joint:", L"Position:", L"Scale:", L"EffectType:", L"Add to Pokemon",L"Export to PC",L"Reload from PC" };
  wchar_t *jointStr[] = { L"NONE", L"WAIST", L"HEAD", L"MOUTH", L"HAND", L"FOOT",L"FOOT2",L"HAND2",L"TAIL",L"TAIL2" };


  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);  

  f32 stickValueX = pVecDevice->GetX();
  f32 stickValueY = pVecDevice->GetY();

  if(stickValueX>-0.3f && stickValueX<0.3f) stickValueX=0;
  if(stickValueY>-0.3f && stickValueY<0.3f) stickValueY=0;

  if(!jointIndex) selectMax = 2;

  // Turn Off All effects:
  for(int z=0;z<4;z++)
  {
    mEffect[z]->SetScale(0.0f);          
  }

  switch(mStateRegSub)
  {
  case 0:
    coreSys->GetTroubleManager()->TurnOff();
    coreSys->GetPokemonAI()->SetState(AI_STATE_CAREWAIT);
    px = py = pz = 0;
    scale = 100;
    cIndex = 0;
    jointIndex = 0;
    mSelection = 0;

    // Transfer Data from DataManager:
    {
      KWParamData_Care *data = coreSys->GetDataManager()->GetCareData();
      if(data!=NULL) 
      {
        memcpy(&careData,data,sizeof(careData));      

        scale = careData.scale[cIndex];
        jointIndex = careData.jointID[cIndex];
        px = careData.xPos[cIndex];
        py = careData.yPos[cIndex];
        pz = careData.zPos[cIndex];
      }
    }

    mReloadMonsNo = mMonsNo;
    mReloadFormNo = mFormNo;
    mStateRegSub++;

    break;

  case 1:
    {
      px += (int)(stickValueX*moveSpeed);
      py += (int)( (stickValueY*1.2f)*moveSpeed);

      gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

      if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ))
      {
        mSelection++;
        if(mSelection>=selectMax) mSelection=0;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ))
      {
        mSelection--;
        if(mSelection<0) mSelection=selectMax-1;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
      {        
        // Index:
        if(mSelection==0) cIndex++;        
        else if(mSelection==1) jointIndex++;        
        else if(mSelection==3) scale++;        
        else if(mSelection==4) mEffectType++;

        if(cIndex>=MAX_CARE_JOINT) cIndex=0;
        if(jointIndex>=jointMax) jointIndex = 0;
        if(mEffectType>=4) mEffectType=0;

        if(mSelection==0)
        {
          KWParamData_Care *data = coreSys->GetDataManager()->GetCareData();
          if(data!=NULL) 
          {
            memcpy(&careData,data,sizeof(careData));      

            scale = careData.scale[cIndex];
            jointIndex = careData.jointID[cIndex];
            px = careData.xPos[cIndex];
            py = careData.yPos[cIndex];
            pz = careData.zPos[cIndex];
          }
        }

        
        
        
        
      }

      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
      {        
        // Index:
        if(mSelection==0) cIndex--;        
        else if(mSelection==1) jointIndex--;        
        else if(mSelection==4) mEffectType--;
              
        
        if(cIndex<0) cIndex=MAX_CARE_JOINT-1;                
        if(jointIndex<0) jointIndex = jointMax-1;
        if(mEffectType<0) mEffectType = 3;

        if(mSelection==0)
        {
          KWParamData_Care *data = coreSys->GetDataManager()->GetCareData();
          if(data!=NULL) 
          {
            memcpy(&careData,data,sizeof(careData));      

            scale = careData.scale[cIndex];
            jointIndex = careData.jointID[cIndex];
            px = careData.xPos[cIndex];
            py = careData.yPos[cIndex];
            pz = careData.zPos[cIndex];
          }
        }

      }

      else if( pButton->IsHold( gfl2::ui::BUTTON_L ))
      {   
        pz++;
      }
      else if( pButton->IsHold( gfl2::ui::BUTTON_R ))
      {   
        pz--;
      }

      else if( pButton->IsHold( gfl2::ui::BUTTON_LEFT ))
      {   
        if(mSelection==3 && scale>2) scale--;          
      }
      else if( pButton->IsHold( gfl2::ui::BUTTON_RIGHT ))
      {   
        if(mSelection==3) scale++;  
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL )) // Cancel back
      {
        coreSys->GetTroubleManager()->TurnOn();
        coreSys->GetPokemonAI()->SetState(AI_STATE_IDLE);
        SetState(DEBUGMANAGER_STATE_IDLE);
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==5) // Add to Pokemon
      {
        int effectID[4] = { CARETROUBLE_DIRT,CARETROUBLE_DUST,CARETROUBLE_HAIR,CARETROUBLE_WATER};
        coreSys->GetTroubleManager()->AddObject(effectID[mEffectType],cIndex); 
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==selectMax-2 && jointIndex!=0) // Export to PC!
      {
        ExportCareDataToPC();
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==selectMax-1 && jointIndex!=0) // Reload from PC!
      {
        coreSys->GetDataManager()->HIOReload_Care();

        KWParamData_Care *data = coreSys->GetDataManager()->GetCareData();
        if(data!=NULL) 
        {
          memcpy(&careData,data,sizeof(careData));      

          scale = careData.scale[cIndex];
          jointIndex = careData.jointID[cIndex];
          px = careData.xPos[cIndex];
          py = careData.yPos[cIndex];
          pz = careData.zPos[cIndex];
        }

        break;
      }
      

      // Copy Data Into Structure:
      careData.jointID[cIndex] = jointIndex;
      careData.scale[cIndex] = scale;
      careData.xPos[cIndex] = px;
      careData.yPos[cIndex] = py;
      careData.zPos[cIndex] = pz;

      // Copy Into Data Manager:
      {      
        KWParamData_Care *data = coreSys->GetDataManager()->GetCareData();
        if(data!=NULL) 
        {
          memcpy(data,&careData,sizeof(careData));
        }
      }

      // Make sure current effect is visible:
      {      
        mEffect[mEffectType]->SetScale((float)(scale/100.0f));          
        gfl2::math::Vector3 pos;
        coreSys->GetDataManager()->GetJointPos(cIndex,pos);

        float zOff = 50.0f * coreSys->GetPokeModel()->GetAdjustScale();
        pos.z += zOff;
        mEffect[mEffectType]->SetOffset(pos);
        
        int node = coreSys->ConvertCareModeJointToPokeJoint(jointIndex,jIndex); // Convert to actual joint
        coreSys->GetPokeModel()->GetEffectNodePosition((PokeTool::MODEL_NODE)node,jIndex,&pos);  

        mEffect[mEffectType]->SetPosition(pos);
      }
    }
    break;
  }

  gfl2::math::Vector4 col;

  coreSys->TextUtil()->ClearDrawText();
  col.Set(0.6f,0.1f,0.5f,1.0f); coreSys->TextUtil()->SetTextColor(col);
  coreSys->TextUtil()->SetTextScale( 1.1f, 1.1f );
  coreSys->TextUtil()->DrawText( xp,   yp, titleStr); yp+=ySpace+10;
  coreSys->TextUtil()->SetTextScale( 0.8f, 0.8f );

  for(int z=0;z<selectMax;z++)
  {
    if(mSelection==z) col.Set(1.0f,1.0f,0.0f,1.0f);
    else col.Set(1.0f,1.0f,1.0f,1.0f);

    coreSys->TextUtil()->SetTextColor(col);

    if(z==0) // Index
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"Index: %i / 5",cIndex+1); 
    }
    else if(z==1) // Joint
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"Joint: %s",jointStr[jointIndex]); 
    }
    else if(z==2) // Position
    {
      if(jointIndex)
        coreSys->TextUtil()->DrawText( xp,   yp, L"Position: %i,%i,%i",px,py,pz);       
    }
    else if(z==3) // Scale
    {
      if(jointIndex)
        coreSys->TextUtil()->DrawText( xp,   yp, L"Scale: %0.2f",scale/100.0f);       
    }
    else if(z==4) // Effect Type
    {
      switch(mEffectType)
      {
      case 0: coreSys->TextUtil()->DrawText( xp,   yp, L"EffectType: DIRT"); break;
      case 1: coreSys->TextUtil()->DrawText( xp,   yp, L"EffectType: DUST"); break;
      case 2: coreSys->TextUtil()->DrawText( xp,   yp, L"EffectType: HAIR"); break;
      case 3: coreSys->TextUtil()->DrawText( xp,   yp, L"EffectType: WATER"); break;
      }
    }
    else if(jointIndex) coreSys->TextUtil()->DrawText( xp,   yp, menuStr[z]); 

    yp+=ySpace;
  }

  //coreSys->TextUtil()->DrawText( xp,   yp, L"Stick: %f,%f",stickValueX,stickValueY); 
}


void KawaigariDebugManager::State_CameraSet(void)
{
  static int cIndex = 0;
  float moveSpeed = 1.0f;
  int selectMax = 7;
  int xp=10,yp=0;
  static int yAppear = 0;
  static s16 px=0,py=0,pz=0;
  static s16 tx=0,ty=0,tz=0;
  int ySpace=22;
  const int camMax = 10;
  wchar_t titleStr[] = L"Camera Position Setting:";

  wchar_t *menuStr[] = { L"Type:", L"Camera Pos:", L"Target Pos:", L"Appear YPos:", L"Export to PC",L"Reload from PC",L"Set Appear State:" };
  wchar_t *camStr[] = { L"CAM_FAR", L"CAM_WHOLE_BODY", L"CAM_UP", L"CAM_DO_UP", L"CAM_LEFT", L"CAM_RIGHT", L"CAM_PEEP", L"CAM_BEHIND", L"CAM_HIT1", L"CAM_EAT" };

  wchar_t *appearStr[] = {    
//   L"AI_SUBSTATE_APPEAR_SICK",      
    L"APPEAR_UPPER",    
    L"APPEAR_LOWER",    
    L"APPEAR_BEHIND",    
    L"APPEAR_LEFT",    
    L"APPEAR_RIGHT",    
    L"APPEAR_DO_UP",    
    L"APPEAR_BACK",    
    L"APPEAR_DROWSEY",    
    L"APPEAR_SLEEP",    
    L"APPEAR_LOOKAROUND",    
    L"APPEAR_PLAYMOTION" };
    static int appearState = 0;
    int appearMax = 11;
    

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);  

  f32 stickValueX = pVecDevice->GetX();
  f32 stickValueY = pVecDevice->GetY();

  if(stickValueX>-0.3f && stickValueX<0.3f) stickValueX=0;
  if(stickValueY>-0.3f && stickValueY<0.3f) stickValueY=0;
  
  switch(mStateRegSub)
  {
  case 0:
    appearState = 0;
    coreSys->GetPokemonAI()->SetState(AI_STATE_CAMERAWAIT);
    px = py = pz = 0;
    yAppear = 0;
    cIndex = 0;
    mSelection = 0;

    // Transfer Data from DataManager:
    {
      KWParamData_Camera *data = coreSys->GetDataManager()->GetCameraData();
      
      if(data!=NULL) 
      {
        memcpy(&cameraData,data,sizeof(cameraData));      

        yAppear = cameraData.yPos;        
        px = cameraData.camPos[ (cIndex*6)+0];
        py = cameraData.camPos[ (cIndex*6)+1];
        pz = cameraData.camPos[ (cIndex*6)+2];
        tx = cameraData.camPos[ (cIndex*6)+3];
        ty = cameraData.camPos[ (cIndex*6)+4];
        tz = cameraData.camPos[ (cIndex*6)+5];
      }

      coreSys->GetCamera()->SetLocationDirect(cIndex);
    }
    
    mStateRegSub++;

    break;

  case 1:
    {

      if(mSelection==1)
      {
        px += (int)(stickValueX*moveSpeed);
        py += (int)( (stickValueY*1.2f)*moveSpeed);
      }
      else if(mSelection==2)
      {
        tx += (int)(stickValueX*moveSpeed);
        ty += (int)( (stickValueY*1.2f)*moveSpeed);
      }


      gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

      if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ))
      {
        mSelection++;
        if(mSelection>=selectMax) mSelection=0;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ))
      {
        mSelection--;
        if(mSelection<0) mSelection=selectMax-1;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
      {        
        // Index:
        if(mSelection==0) cIndex++;        
        
        if(cIndex>=camMax) cIndex=0;
        if(cIndex==7) cIndex=8; // Skip Behind Camera
        
        if(mSelection==0) // Changing Camera:
        {
          KWParamData_Camera *data = coreSys->GetDataManager()->GetCameraData();
          if(data!=NULL) 
          {
            memcpy(&cameraData,data,sizeof(cameraData));      

            yAppear = cameraData.yPos;
            px = cameraData.camPos[ (cIndex*6)+0];
            py = cameraData.camPos[ (cIndex*6)+1];
            pz = cameraData.camPos[ (cIndex*6)+2];
            tx = cameraData.camPos[ (cIndex*6)+3];
            ty = cameraData.camPos[ (cIndex*6)+4];
            tz = cameraData.camPos[ (cIndex*6)+5];
          }

          coreSys->GetCamera()->SetLocationDirect(cIndex);
        }





      }

      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
      {        
        // Index:
        if(mSelection==0) cIndex--;        
        
        if(cIndex<0) cIndex=camMax-1;      
        if(cIndex==7) cIndex=6; // Skip Behind Camera
        
        if(mSelection==0)
        {
          KWParamData_Camera *data = coreSys->GetDataManager()->GetCameraData();
          if(data!=NULL) 
          {
            memcpy(&cameraData,data,sizeof(cameraData));      

            yAppear = cameraData.yPos;
            px = cameraData.camPos[ (cIndex*6)+0];
            py = cameraData.camPos[ (cIndex*6)+1];
            pz = cameraData.camPos[ (cIndex*6)+2];
            tx = cameraData.camPos[ (cIndex*6)+3];
            ty = cameraData.camPos[ (cIndex*6)+4];
            tz = cameraData.camPos[ (cIndex*6)+5];
          }
          coreSys->GetCamera()->SetLocationDirect(cIndex);
        }

      }

      else if( pButton->IsHold( gfl2::ui::BUTTON_L ))
      {   
        if(mSelection==1) pz++;
        else if(mSelection==2) tz++;
      }
      else if( pButton->IsHold( gfl2::ui::BUTTON_R ))
      {   
        if(mSelection==1) pz--;
        else if(mSelection==2) tz--;
      }

      else if( pButton->IsHold( gfl2::ui::BUTTON_LEFT ))
      {   
        if(mSelection==3) yAppear--;                
      }
      else if( pButton->IsHold( gfl2::ui::BUTTON_RIGHT ))
      {   
        if(mSelection==3) yAppear++;          
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
      {       
        if(mSelection==6) appearState--;
        if(appearState<0) appearState=appearMax-1;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
      {   
        if(mSelection==6) appearState++;
        if(appearState>=appearMax) appearState=0;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL )) // Cancel back
      {
        coreSys->GetPokeModel()->SetPosition(0.0f,0.0f,0.0f);
        coreSys->GetPokemonAI()->SetState(AI_STATE_IDLE);
        SetState(DEBUGMANAGER_STATE_IDLE);
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==4) // Export to PC!
      {
        ExportCameraDataToPC();
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==6) // Test Appear
      {
        coreSys->GetPokemonAI()->SetForceAppearState(appearState);
        coreSys->GetPokemonAI()->SetState(AI_STATE_APPEAR);
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) && mSelection==5) // Import from PC!
      {
        coreSys->GetDataManager()->HIOReload_Camera();

        KWParamData_Camera *data = coreSys->GetDataManager()->GetCameraData();
        if(data!=NULL) 
        {
          memcpy(&cameraData,data,sizeof(cameraData));      

          yAppear = cameraData.yPos;
          px = cameraData.camPos[ (cIndex*6)+0];
          py = cameraData.camPos[ (cIndex*6)+1];
          pz = cameraData.camPos[ (cIndex*6)+2];
          tx = cameraData.camPos[ (cIndex*6)+3];
          ty = cameraData.camPos[ (cIndex*6)+4];
          tz = cameraData.camPos[ (cIndex*6)+5];
        }
        coreSys->GetCamera()->SetLocationDirect(cIndex);
        break;
      }

      if(mSelection!=6) // Dont set camera on appear test:
      {      
        // Copy Data Into Structure:      
        cameraData.yPos = yAppear;
        cameraData.camPos[ (cIndex*6)+0] = px;
        cameraData.camPos[ (cIndex*6)+1] = py;
        cameraData.camPos[ (cIndex*6)+2] = pz;
        cameraData.camPos[ (cIndex*6)+3] = tx;
        cameraData.camPos[ (cIndex*6)+4] = ty;
        cameraData.camPos[ (cIndex*6)+5] = tz;
      
        // Copy Into Data Manager:
        {      
          KWParamData_Camera *data = coreSys->GetDataManager()->GetCameraData();
          if(data!=NULL)  memcpy(data,&cameraData,sizeof(cameraData));        

          // Update Camera:
          coreSys->GetCamera()->SetLocationDirect(cIndex);
        }

      }
    }
    break;
  }

  gfl2::math::Vector4 col;

  coreSys->TextUtil()->ClearDrawText();
  col.Set(0.6f,0.1f,0.5f,1.0f); coreSys->TextUtil()->SetTextColor(col);
  coreSys->TextUtil()->SetTextScale( 1.1f, 1.1f );
  coreSys->TextUtil()->DrawText( xp,   yp, titleStr); yp+=ySpace+10;
  coreSys->TextUtil()->SetTextScale( 0.8f, 0.8f );

  for(int z=0;z<selectMax;z++)
  {
    if(mSelection==z) col.Set(1.0f,1.0f,0.0f,1.0f);
    else col.Set(1.0f,1.0f,1.0f,1.0f);

    coreSys->TextUtil()->SetTextColor(col);

    if(z==0) // Cam Type
    {
      coreSys->TextUtil()->DrawText( xp,   yp, camStr[cIndex]); 
    }
    else if(z==1) // Cam Pos
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"CamPos: %i,%i,%i",px,py,pz); 
    }
    else if(z==2) // Target Pos
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"TargetPos: %i,%i,%i",tx,ty,tz); 
    }
    else if(z==3) // Y Appear Pos
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"YAppear: %i",yAppear);       
    }
    else if(z==6) // Appear State
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"Set Appear: %s",appearStr[appearState]);       
    }
    
    else coreSys->TextUtil()->DrawText( xp,   yp, menuStr[z]); 

    yp+=ySpace;
  }


}

void KawaigariDebugManager::State_EatVibrationSet(void)
{
  static int cFrame = 0;
  static int cIndex = 0;
  static int cObj = 0;
  float moveSpeed = gfl2::math::ConvDegToRad(0.01f);
  int selectMax = 13;
  int xp=10,yp=0;
  static int yAppear = 0;
  int ySpace=15; 
  wchar_t titleStr[] = L"Eat Vibration Setting:";

  wchar_t *menuStr[] = { L"Index:", L"Frame1:", L"Frame2:", L"Frame3:", L"Frame4",L"Frame5",L"Frame6",L"Frame7",L"Frame8",L"Frame9",L"Scale",L"Reset Hunger",L"Write Debug" };


  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);  

  f32 stickValueX = pVecDevice->GetX();
  f32 stickValueY = pVecDevice->GetY();

  if(stickValueX>-0.3f && stickValueX<0.3f) stickValueX=0;
  if(stickValueY>-0.3f && stickValueY<0.3f) stickValueY=0;

  switch(mStateRegSub)
  {
  case 0:       
    coreSys->LoadFoodResource(0);
    coreSys->LoadFoodResource(1);
    coreSys->LoadFoodResource(2);
    coreSys->LoadFoodResource(3);
    coreSys->LoadFoodResource(4);
    coreSys->LoadFoodResource(5);
    mStateRegSub++;
    // BREAK-THOUGH

  case 1:
    if(coreSys->WaitFoodResource()) mStateRegSub++;
    else break;

  case 2:
    coreSys->LoadFoodObject(0);
    coreSys->LoadFoodObject(1);
    coreSys->LoadFoodObject(2);
    coreSys->LoadFoodObject(3);
    coreSys->LoadFoodObject(4);
    {    
      gfl2::math::Vector3 *pos = coreSys->GetFoodPosition();
      coreSys->Get3DTouchPositionByXY(pos,160,120+60,TPT_MAME);        
    }

    coreSys->ShowFoodObject(cObj,true);
    coreSys->GetUIManager()->ChangeToMainMenu();
    coreSys->GetPokemonAI()->SetState(AI_STATE_CAREWAIT);
    cIndex = 0;
    cObj = 0;
    mSelection = 0;
    mStateRegSub++;
    break;

  case 3:
    {
      // Auto Play Shake:
      if(mSelection==0 || mSelection==10)
      {
        gfl2::math::Vector3 *rot = coreSys->GetFoodRotation();
        
        float xv,zv,sxv,szv;
        float scl = coreSys->GetPokemonAI()->GetFoodRotScale(cIndex);
        coreSys->GetPokemonAI()->GetFoodRotVal(cFrame,xv,zv);
        sxv = xv * scl;
        szv = zv * scl;
        rot->x = gfl2::math::ConvRadToDeg(sxv);
        rot->z = gfl2::math::ConvRadToDeg(szv);
        
        cFrame++; if(cFrame==20) cFrame=0;
      }

      gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

      // Adjust X or Z:
      if(mSelection>0 && mSelection<10)
      {
        float xv,zv;
        coreSys->GetPokemonAI()->GetFoodRotVal(mSelection-1,xv,zv);
        
        xv += (stickValueX*moveSpeed);
        zv += (stickValueY*1.2f)*moveSpeed;

        coreSys->GetPokemonAI()->SetFoodRotVal(xv,zv,mSelection-1);
      }
      
      if( pButton->IsTrigger( gfl2::ui::BUTTON_DOWN ))
      {
        mSelection++;
        if(mSelection>=selectMax) mSelection=0;
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_UP ))
      {
        mSelection--;
        if(mSelection<0) mSelection=selectMax-1;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ))
      {        
        // Index:
        

        if(mSelection==0) cIndex++;        
        if(cIndex>=5) cIndex=0;              

        
      }

      else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ))
      {        
        
        // Index:
        if(mSelection==0) cIndex--;        
        if(cIndex<0) cIndex=4;                      
        
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_R ))
      {   
        coreSys->ShowFoodObject(cObj,false);
        cObj++; if(cObj>=5) cObj = 0;
        coreSys->ShowFoodObject(cObj,true);
      }
      else if( pButton->IsTrigger( gfl2::ui::BUTTON_L ))
      {   
        coreSys->ShowFoodObject(cObj,false);
        cObj--; if(cObj<0) cObj = 4;
        coreSys->ShowFoodObject(cObj,true);
      }
      
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && mSelection==11) // Reset Hunger Zero
      {
        coreSys->GetPokemonAI()->SetFullness(0);
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL )) // Cancel back
      {
        coreSys->ShowFoodObject(cObj,false);
        coreSys->GetPokeModel()->SetPosition(0.0f,0.0f,0.0f);
        coreSys->GetPokemonAI()->SetState(AI_STATE_IDLE);
        SetState(DEBUGMANAGER_STATE_IDLE);
        coreSys->UnloadFoodObjects();
        break;
      }

      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR )) // Action
      {
        if(mSelection==11) // Reset Hunger:
        {
          coreSys->GetPokemonAI()->SetFullness(130);
        }
#if defined(GF_PLATFORM_WIN32)
        else if(mSelection==12) // Export
        {
          ExportEatVibrationDataToPC();
        }
#endif
      }

      if( pButton->IsHold( gfl2::ui::BUTTON_LEFT ))
      {   
        if(mSelection==10) 
        {
          float scl = coreSys->GetPokemonAI()->GetFoodRotScale(cIndex);
          scl -=0.01f;       
          coreSys->GetPokemonAI()->SetFoodRotScale(scl,cIndex);
        }
      }
      else if( pButton->IsHold( gfl2::ui::BUTTON_RIGHT ))
      {   
        if(mSelection==10)
        {
          float scl = coreSys->GetPokemonAI()->GetFoodRotScale(cIndex);
          scl +=0.01f;       
          coreSys->GetPokemonAI()->SetFoodRotScale(scl,cIndex);
        }
      }

      break;
    }
  }

  // Set Food Rot:
  {    
    gfl2::math::Vector3 *rot = coreSys->GetFoodRotation();

    if(mSelection>0 && mSelection<=9)
    {
      float xv,zv,sxv,szv;
      float scl = coreSys->GetPokemonAI()->GetFoodRotScale(cIndex);
      coreSys->GetPokemonAI()->GetFoodRotVal(mSelection-1,xv,zv);
      sxv = xv * scl;
      szv = zv * scl;
      rot->x = gfl2::math::ConvRadToDeg(sxv);
      rot->z = gfl2::math::ConvRadToDeg(szv);
    }
    else if(mSelection!=10 && mSelection!=0)
    {
      rot->x = rot->y = rot->z = 0.0f;
    }
  }

  gfl2::math::Vector4 col;

  coreSys->TextUtil()->ClearDrawText();
  col.Set(0.6f,0.1f,0.5f,1.0f); coreSys->TextUtil()->SetTextColor(col);
  coreSys->TextUtil()->SetTextScale( 1.0f, 1.0f );
  coreSys->TextUtil()->DrawText( xp,   yp, titleStr); yp+=ySpace+10;
  coreSys->TextUtil()->SetTextScale( 0.6f, 0.6f );

  for(int z=0;z<selectMax;z++)
  {
    if(mSelection==z) col.Set(1.0f,1.0f,0.0f,1.0f);
    else col.Set(1.0f,1.0f,1.0f,1.0f);

    coreSys->TextUtil()->SetTextColor(col);
    if(z==0)
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"Index %i / 5",cIndex+1); 
    }
    else if(z>0 && z<=9) // VibrationRotation
    {
      float xv,zv,sxv,szv;
      float scl = coreSys->GetPokemonAI()->GetFoodRotScale(cIndex);
      coreSys->GetPokemonAI()->GetFoodRotVal(z-1,xv,zv);
      sxv = xv * scl;
      szv = zv * scl;
      
      coreSys->TextUtil()->DrawText( xp,   yp, L"Frame(%i) X:%0.2f  Z:%0.2f -> %0.2f,%0.2f",z,gfl2::math::ConvRadToDeg(xv),gfl2::math::ConvRadToDeg(zv),gfl2::math::ConvRadToDeg(sxv),gfl2::math::ConvRadToDeg(szv)); 
    }
    else if(z==10)
    {
      coreSys->TextUtil()->DrawText( xp,   yp, L"Scale: %0.2f",coreSys->GetPokemonAI()->GetFoodRotScale(cIndex)); 
    }
    else coreSys->TextUtil()->DrawText( xp,   yp, menuStr[z]); 

    yp+=ySpace;
  }


}


void KawaigariDebugManager :: ResetWriteBuffer(void)
{
   memset(mWriteBuffer,0,WRITEBUFFER_SIZE); // Clear Buffer
   mWritePos = 0;
}

void KawaigariDebugManager :: AddWriteBufferString(char *str)
{  
  memcpy(mWriteBuffer+mWritePos,str,strlen(str));
  mWritePos += strlen(str);
  mWriteBuffer[mWritePos++] = 13;
  mWriteBuffer[mWritePos++] = 10;
}

void KawaigariDebugManager :: ExportCameraDataToPC(void)
{
  ResetWriteBuffer();

  char bufStr[128];
  const int maxCam = 10;

  // MonsNo:
  AddWriteBufferString("id");
  #if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mMonsNo); AddWriteBufferString(bufStr);
  #else
  sprintf_s(bufStr,128,"%i",mMonsNo); AddWriteBufferString(bufStr);
  #endif

  // FormNo:
  AddWriteBufferString("form");

#if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mFormNo); AddWriteBufferString(bufStr);
#else
  sprintf_s(bufStr,128,"%i",mFormNo); AddWriteBufferString(bufStr);
#endif

  // SexNo:
  AddWriteBufferString("sex");

#if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mSexNo); AddWriteBufferString(bufStr);
#else
  sprintf_s(bufStr,128,"%i",mSexNo); AddWriteBufferString(bufStr);
#endif

  char *camStr[] = { "CAM_FAR", "CAM_WHOLE_BODY", "CAM_UP", "CAM_DO_UP", "CAM_LEFT", "CAM_RIGHT", "CAM_PEEP", "CAM_BEHIND", "CAM_HIT1", "CAM_EAT" };

  for(int z=0;z<maxCam;z++)
  {
    // Joint
    #if defined(GF_PLATFORM_CTR)
    sprintf(bufStr,camStr[z]); AddWriteBufferString(bufStr);
    #else
    sprintf_s(bufStr,128,camStr[z]); AddWriteBufferString(bufStr);
    #endif
    
    // Cam Pos
    #if defined(GF_PLATFORM_CTR)
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+0] ); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+1] ); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+2] ); AddWriteBufferString(bufStr);

    // Cam Target:
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+3] ); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+4] ); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",cameraData.camPos[ (z*6)+5] ); AddWriteBufferString(bufStr);
    #else
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+0] ); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+1] ); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+2] ); AddWriteBufferString(bufStr);

    // Cam Target:
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+3] ); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+4] ); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",cameraData.camPos[ (z*6)+5] ); AddWriteBufferString(bufStr);
    #endif

  }

#if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"YAppear"); AddWriteBufferString(bufStr);
  sprintf(bufStr,"%i",cameraData.yPos); AddWriteBufferString(bufStr);
#else
  sprintf_s(bufStr,128,"YAppear"); AddWriteBufferString(bufStr);
  sprintf_s(bufStr,128,"%i",cameraData.yPos); AddWriteBufferString(bufStr);
#endif

  gfl2::fs::PcUtil::WriteFile("R:/home/momiji/resource/app_resource/resource/kawaigari/param/hio/cameradata_patch.txt",mWriteBuffer,mWritePos-2);
}

void KawaigariDebugManager :: ExportCareDataToPC(void)
{
  ResetWriteBuffer();
  
  char bufStr[128];

  // MonsNo:
  AddWriteBufferString("id");
  #if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mMonsNo); AddWriteBufferString(bufStr);
  #else
  sprintf_s(bufStr,128,"%i",mMonsNo); AddWriteBufferString(bufStr);
  #endif

  // FormNo:
  AddWriteBufferString("form");

  #if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mFormNo); AddWriteBufferString(bufStr);
  #else
  sprintf_s(bufStr,128,"%i",mFormNo); AddWriteBufferString(bufStr);
  #endif

  // SexNo:
  AddWriteBufferString("sex");

#if defined(GF_PLATFORM_CTR)
  sprintf(bufStr,"%i",mSexNo); AddWriteBufferString(bufStr);
#else
  sprintf_s(bufStr,128,"%i",mSexNo); AddWriteBufferString(bufStr);
#endif

  char *jointStr[] = { "NONE", "WAIST", "HEAD", "MOUTH", "HAND", "FOOT","FOOT2","HAND2","TAIL","TAIL2" };

  for(int z=0;z<MAX_CARE_JOINT;z++)
  {
#if defined(GF_PLATFORM_CTR)
    // Joint
    sprintf(bufStr,"Joint_%i",z); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%s",jointStr[careData.jointID[z]] ); AddWriteBufferString(bufStr);

    // Pos
    sprintf(bufStr,"XOff_%i",z); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",careData.xPos[z]); AddWriteBufferString(bufStr);

    sprintf(bufStr,"YOff_%i",z); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",careData.yPos[z]); AddWriteBufferString(bufStr);

    sprintf(bufStr,"ZOff_%i",z); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",careData.zPos[z]); AddWriteBufferString(bufStr);

    // scale
    sprintf(bufStr,"Scale_%i",z); AddWriteBufferString(bufStr);
    sprintf(bufStr,"%i",careData.scale[z]); AddWriteBufferString(bufStr);

#else

    // Joint
    sprintf_s(bufStr,128,"Joint_%i",z); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%s",jointStr[careData.jointID[z]] ); AddWriteBufferString(bufStr);

    // Pos
    sprintf_s(bufStr,128,"XOff_%i",z); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",careData.xPos[z]); AddWriteBufferString(bufStr);

    sprintf_s(bufStr,128,"YOff_%i",z); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",careData.yPos[z]); AddWriteBufferString(bufStr);

    sprintf_s(bufStr,128,"ZOff_%i",z); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",careData.zPos[z]); AddWriteBufferString(bufStr);

    // scale
    sprintf_s(bufStr,128,"Scale_%i",z); AddWriteBufferString(bufStr);
    sprintf_s(bufStr,128,"%i",careData.scale[z]); AddWriteBufferString(bufStr);

#endif
  }

  gfl2::fs::PcUtil::WriteFile("R:/home/momiji/resource/app_resource/resource/kawaigari/param/hio/caredata_patch.txt",mWriteBuffer,mWritePos-2);
}

#if defined(GF_PLATFORM_WIN32)
void KawaigariDebugManager :: ExportEatVibrationDataToPC(void)
{
  ResetWriteBuffer();

  char bufStr[128];
  
  AddWriteBufferString("float foodRotMotion_1[10][3]= {");
  AddWriteBufferString("{ 0.0f,0.0f,0.0f }, // Frame 0");

  for(int z=0;z<9;z++)
  {  
    float xv,zv;    
    coreSys->GetPokemonAI()->GetFoodRotVal(z,xv,zv);    

    sprintf_s(bufStr,128,"{ %f,0.0f,%f }, // Frame %i",xv,zv,z+1); AddWriteBufferString(bufStr);
  }

  AddWriteBufferString("};");

  AddWriteBufferString("float foodRotScale[5] = {");

  for(int z=0;z<5;z++)
  {  
      
    float scl = coreSys->GetPokemonAI()->GetFoodRotScale(z);    

    sprintf_s(bufStr,128,"%f, ",scl); AddWriteBufferString(bufStr);
   
  }

  AddWriteBufferString("};");

  gfl2::fs::PcUtil::WriteFile("R:/home/momiji/eatdata.txt",mWriteBuffer,mWritePos-2);
}
#endif

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

#endif // PM_DEBUG

#endif // WIN32