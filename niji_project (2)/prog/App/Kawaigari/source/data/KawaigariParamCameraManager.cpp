//======================================================================
/**
 * @file	KawaigariParamCameraManager.c++
 * @brief	niji Kawaigari Parameter Camera Manager
 * @author	PETE
 * @data	16/01/12
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include "KawaigariParamCameraManager.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

// File ID:
#include <arc_index/kawaigari_parameter_resource.gaix>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariParamCameraManager :: KawaigariParamCameraManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  mFileManager = fileManager;

  // Load Parameter Resource:
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_KAWAIGARI_PARAMETER;  
  req.datId = GARC_kawaigari_parameter_resource_kw_param_data_camera_BIN;
  req.align = 128;
  req.heapForBuf = dataHeap;
  req.heapForReq = workHeap;
  req.heapForCompressed = NULL;
  
  req.pBufSize = NULL;
  req.ppBuf = &dataBuf;

  fileManager->AddArcFileLoadDataReq(req);
  dataCount = 0;
  isLoaded = false;

  checkMonsNo = -1;
  checkFormNo = -1;
  checkSexNo = -1;
  lastResult = 0;

}

KawaigariParamCameraManager :: ~KawaigariParamCameraManager(void)
{
  if(isLoaded) GflHeapFreeMemory(dataBuf); // Forgot to Finalize()!
}

bool KawaigariParamCameraManager :: IsLoaded(void)
{
  if(isLoaded) return(true);

  isLoaded = mFileManager->IsArcFileLoadDataFinished( &dataBuf );  

  if(isLoaded)
  {
    u32 *val = (u32*)dataBuf;
    dataCount = *val;    
  }

   return isLoaded;
}

int KawaigariParamCameraManager :: GetIndex(int monsNo,int formNo,int sex)
{
  if(checkMonsNo == monsNo && checkFormNo == formNo && checkSexNo == sex) return(lastResult);

  if(!isLoaded || dataCount == 0) return(0);

  int result = -1;

  for(u32 z=0;z<dataCount;z++)
  {
    KWParamData_Camera *data = GetData(z);

    if(data->idNum == monsNo) // ID matches MonsterNo
    {
      // Use first result or exact result:
      if( (formNo == data->formNo && (data->sex==sex || data->sex==0)) || result==-1)
        result = z;
    }

    if(data->idNum>monsNo) break; // Can't find!
  }
  
  checkMonsNo = monsNo;
  checkFormNo = formNo;
  checkSexNo = sex;
  lastResult = result;

  return(result);
}

void KawaigariParamCameraManager :: GetCameraData(int monsNo,int formNo,int sex,int type,gfl2::math::Vector3 &eyePos,gfl2::math::Vector3 &focusPos)
{  
  if(!isLoaded || dataCount == 0) return; // No Data Loaded!

  int index = GetIndex(monsNo,formNo,sex);
  if(index == -1) return; // No Matching Pokemon Found!

  KWParamData_Camera *data = GetData(index);
  
  if(data==NULL) return;

  eyePos.x = (float)data->camPos[ (type*6)+0 ];
  eyePos.y = (float)data->camPos[ (type*6)+1 ];
  eyePos.z = (float)data->camPos[ (type*6)+2 ];

  focusPos.x = (float)data->camPos[ (type*6)+3 ];
  focusPos.y = (float)data->camPos[ (type*6)+4 ];
  focusPos.z = (float)data->camPos[ (type*6)+5 ];


}

float KawaigariParamCameraManager :: GetAppearYPos(int monsNo,int formNo,int sex,float defaultVal)
{
  if(!isLoaded || dataCount == 0) return(defaultVal); // No Data Loaded!

  int index = GetIndex(monsNo,formNo,sex);
  if(index == -1) return(defaultVal); // No Matching Pokemon Found!

  KWParamData_Camera *data = GetData(index);
  if(data==NULL) return(defaultVal);
  return(data->yPos);
}

KWParamData_Camera* KawaigariParamCameraManager :: GetCameraData(int monsNo,int formNo,int sex)
{
  if(!isLoaded || dataCount == 0) return(NULL); // No Data Loaded!

  int index = GetIndex(monsNo,formNo,sex);
  if(index == -1) return(NULL); // No Matching Pokemon Found!

  KWParamData_Camera *data = GetData(index);
  return(data);
}

#if PM_DEBUG
void KawaigariParamCameraManager :: LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap)
{
  if(!isLoaded) return;
  GflHeapFreeMemory(dataBuf);

  // Load Local File:
  size_t size;  
  gfl2::fs::PcUtil::ReadFile(&dataBuf,&size,NULL,path,dataHeap,8);

}
#endif

KWParamData_Camera* KawaigariParamCameraManager :: GetData(u32 index)
{
  // No data to access?
  if(!isLoaded || dataCount==0 || index>=dataCount) return(NULL);

  index++;

  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(dataBuf);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;

  return( reinterpret_cast<KWParamData_Camera*>(ptr));
}

bool KawaigariParamCameraManager :: Finalize(void)
{
  GflHeapFreeMemory(dataBuf);
  isLoaded = false;
  dataCount = 0;
  return(true);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

