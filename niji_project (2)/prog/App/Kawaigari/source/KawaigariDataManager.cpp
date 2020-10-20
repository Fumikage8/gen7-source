//======================================================================
/**
 * @file	KawaigariDataManager.C++
 * @brief	Kawaigari All Data Loader / Manager Code (Camera, Parameters, etc)
 * @author	PETE
 * @data	2015.10.22
 */
//======================================================================

#include "KawaigariDataManager.h"
#include "KawaigariCamera.h" // For Camera Types
#include "KawaigariMain.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>


GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariDataManager::KawaigariDataManager(void)
{  
  cameraData = NULL;
  optionData = NULL;
  faceData = NULL;
  snackAffectionData = NULL;
  groupData = NULL;
  affectionData = NULL;
  motionAffectionData = NULL;
  soundData = NULL;
  careData = NULL;

  initSeq = 0;
}

KawaigariDataManager::~KawaigariDataManager(void)
{    
}

#if PM_DEBUG
void KawaigariDataManager::HIOReload(void)
{
  optionData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_option.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED OPTIONS DATA*\n");

  careData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_caremode.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED CARE DATA*\n");

  soundData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_sound.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED SOUND DATA*\n");

  snackAffectionData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_snackaffection.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED SNACK AFFECTION DATA*\n");

  motionAffectionData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_motion_affection.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED MOTION AFFECTION DATA*\n");

  groupData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_grouping.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED GROUP DATA*\n");

  faceData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_face.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED FACE DATA*\n");

  cameraData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_camera.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED CAMERA DATA*\n");

  affectionData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_affection.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED AFFECTION DATA*\n");

}

void KawaigariDataManager::HIOReload_Camera(void)
{  
  cameraData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_camera.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED CAMERA DATA*\n"); 
}

void KawaigariDataManager::HIOReload_Care(void)
{  
  careData->LoadHIO("R:/home/momiji/resource/app_resource/resource/kawaigari/param/kw_param_data_caremode.bin",coreSys->GetHeap());
  GFL_PRINT("*RELOADED CARE DATA*\n");
}


#endif

bool KawaigariDataManager::Initalize(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  switch(initSeq)
  {
    case 0:
      // Parameter GARC:
      {	    
        gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
        req.arcId				= ARCID_KAWAIGARI_PARAMETER;
        req.heapForFile	= workHeap;
        req.heapForReq	= workHeap;
        fileManager->SyncArcFileOpen(req);
      }

      cameraData = GFL_NEW(dataHeap)KawaigariParamCameraManager(dataHeap,workHeap,fileManager);
      optionData = GFL_NEW(dataHeap)KawaigariParamOptionManager(dataHeap,workHeap,fileManager);
      faceData = GFL_NEW(dataHeap)KawaigariParamFaceManager(dataHeap,workHeap,fileManager);
      snackAffectionData = GFL_NEW(dataHeap)KawaigariParamSnackAffectionManager(dataHeap,workHeap,fileManager);
      groupData = GFL_NEW(dataHeap)KawaigariParamGroupManager(dataHeap,workHeap,fileManager);
      affectionData = GFL_NEW(dataHeap)KawaigariParamAffectionManager(dataHeap,workHeap,fileManager);
      motionAffectionData = GFL_NEW(dataHeap)KawaigariParamMotionAffectionManager(dataHeap,workHeap,fileManager);
      soundData = GFL_NEW(dataHeap)KawaigariParamSoundManager(dataHeap,workHeap,fileManager);
      careData = GFL_NEW(dataHeap)KawaigariParamCareManager(dataHeap,workHeap,fileManager);

      initSeq++;
      break;
    case 1:
      if(!cameraData->IsLoaded()) break;
      if(!optionData->IsLoaded()) break;
      if(!faceData->IsLoaded()) break;
      if(!snackAffectionData->IsLoaded()) break;
      if(!groupData->IsLoaded()) break;
      if(!affectionData->IsLoaded()) break;
      if(!motionAffectionData->IsLoaded()) break;
      if(!soundData->IsLoaded()) break;
      if(!careData->IsLoaded()) break;

      initSeq++;
      break;

    case 2:
      // Parameter GARC Close:
      {	    
        gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
        req.arcId				= ARCID_KAWAIGARI_PARAMETER;
        fileManager->SyncArcFileClose(req);
      }
      return(true);      

  }
  return(false);
}

bool KawaigariDataManager::Finalize(void)
{
  if(cameraData) cameraData->Finalize();
  GFL_SAFE_DELETE(cameraData);

  if(optionData) optionData->Finalize();
  GFL_SAFE_DELETE(optionData);

  if(faceData) faceData->Finalize();
  GFL_SAFE_DELETE(faceData);

  if(snackAffectionData) snackAffectionData->Finalize();
  GFL_SAFE_DELETE(snackAffectionData);

  if(groupData) groupData->Finalize();
  GFL_SAFE_DELETE(groupData);

  if(affectionData) affectionData->Finalize();
  GFL_SAFE_DELETE(affectionData);
  
  if(motionAffectionData) motionAffectionData->Finalize();
  GFL_SAFE_DELETE(motionAffectionData);

  if(soundData) soundData->Finalize();
  GFL_SAFE_DELETE(soundData);

  if(careData) careData->Finalize();
  GFL_SAFE_DELETE(careData);

  return(true);
}

void KawaigariDataManager::GetIdleCamera(int &xMin,int &xMax,int &yMin,int &yMax)
{
  xMin = optionData->GetIdleCameraXMin(mMonsNo,mFormNo,mSexNo);
  xMax = optionData->GetIdleCameraXMax(mMonsNo,mFormNo,mSexNo);
  yMin = optionData->GetIdleCameraYMin(mMonsNo,mFormNo,mSexNo);
  yMax = optionData->GetIdleCameraYMax(mMonsNo,mFormNo,mSexNo);
}

void KawaigariDataManager::GetCameraData(int type,gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos)
{
  if(type>=KG_CAMERA_MAX) return; // Invalid Camera Type!

  switch(type)
  {
    case KG_CAMERA_FAR:    
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 67.0f, 250.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 30.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;


    case KG_CAMERA_WHOLE_BODY:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 36.0f, 144.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 24.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_UP:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 23.0f, 105.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 24.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_DO_UP:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 22.0f, 74.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 26.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_LEFT:
      {
        gfl2::math::Vector3 eyePosition( 24.0f, 39.0f, 135.0f );  
        gfl2::math::Vector3 focusPosition( 32.0f, 33.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;
    case KG_CAMERA_RIGHT:
      {
        gfl2::math::Vector3 eyePosition( -25.0f, 42.0f, 136.0f );  
        gfl2::math::Vector3 focusPosition( -30.0f, 35.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_PEEP:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 31.0f, 103.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 41.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_BEHIND:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 52.0f, 155.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 24.0f, 310.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;

    case KG_CAMERA_HIT1:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 35.0f, 158.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 24.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;
    case KG_CAMERA_EAT:
      {
        gfl2::math::Vector3 eyePosition( 0.0f, 35.0f, 105.0f );  
        gfl2::math::Vector3 focusPosition( 0.0f, 27.0f, 0.0f );
        cameraData->GetCameraData(mMonsNo,mFormNo,mSexNo,type,eyePosition,focusPosition);
        eyePos = eyePosition; focusPos = focusPosition;
      }
      break;
    
    case KG_CAMERA_BUST_UP: // Special! Average of CAM_WHOLE_BODY & CAM_UP
      {
        gfl2::math::Vector3 eyeA( 0.0f, 0.0f, 0.0f );  
        gfl2::math::Vector3 focusA( 0.0f, 0.0f, 0.0f );
        gfl2::math::Vector3 eyeB( 0.0f, 0.0f, 0.0f );  
        gfl2::math::Vector3 focusB( 0.0f, 0.0f, 0.0f );
                
        GetCameraData(KG_CAMERA_WHOLE_BODY,eyeA,focusA);
        GetCameraData(KG_CAMERA_UP,eyeB,focusB);
        
        eyePos.x = (eyeA.x+eyeB.x) / 2.0f;
        eyePos.y = (eyeA.y+eyeB.y) / 2.0f;
        eyePos.z = (eyeA.z+eyeB.z) / 2.0f;

        focusPos.x = (focusA.x+focusB.x) / 2.0f;
        focusPos.y = (focusA.y+focusB.y) / 2.0f;
        focusPos.z = (focusA.z+focusB.z) / 2.0f;
      }
      break;

    case KG_CAMERA_ALL_MOTION: // Special! CAM_WHOLE_BODY + CAM_FAR/2
      {
        gfl2::math::Vector3 eyeA( 0.0f, 0.0f, 0.0f );  
        gfl2::math::Vector3 focusA( 0.0f, 0.0f, 0.0f );
        gfl2::math::Vector3 eyeB( 0.0f, 0.0f, 0.0f );  
        gfl2::math::Vector3 focusB( 0.0f, 0.0f, 0.0f );

        GetCameraData(KG_CAMERA_WHOLE_BODY,eyeA,focusA);
        GetCameraData(KG_CAMERA_FAR,eyeB,focusB);

        eyePos.x = (eyeA.x+eyeB.x) / 2.0f;
        eyePos.y = (eyeA.y+eyeB.y) / 2.0f;
        eyePos.z = (eyeA.z+eyeB.z) / 2.0f;

        focusPos.x = (focusA.x+focusB.x) / 2.0f;
        focusPos.y = (focusA.y+focusB.y) / 2.0f;
        focusPos.z = (focusA.z+focusB.z) / 2.0f;
      }
      break;
    
  }
  
}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)