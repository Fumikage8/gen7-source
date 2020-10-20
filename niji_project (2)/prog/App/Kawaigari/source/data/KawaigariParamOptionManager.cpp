//======================================================================
/**
 * @file	KawaigariParamOptionManager.c++
 * @brief	niji Kawaigari Parameter Option Manager
 * @author	PETE
 * @data	16/01/14
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include "KawaigariParamOptionManager.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

// File ID:
#include <arc_index/kawaigari_parameter_resource.gaix>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariParamOptionManager :: KawaigariParamOptionManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  mFileManager = fileManager;

  // Load Parameter Resource:
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_KAWAIGARI_PARAMETER;  
  req.datId = GARC_kawaigari_parameter_resource_kw_param_data_option_BIN;
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

KawaigariParamOptionManager :: ~KawaigariParamOptionManager(void)
{
  if(isLoaded) GflHeapFreeMemory(dataBuf); // Forgot to Finalize()!
}

#if PM_DEBUG
void KawaigariParamOptionManager :: LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap)
{
  if(!isLoaded) return;
  GflHeapFreeMemory(dataBuf);

  // Load Local File:
  size_t size;  
  gfl2::fs::PcUtil::ReadFile(&dataBuf,&size,NULL,path,dataHeap,8);

}
#endif

bool KawaigariParamOptionManager :: IsLoaded(void)
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

int KawaigariParamOptionManager :: GetIndex(int monsNo,int formNo,int sex)
{
  if(checkMonsNo == monsNo && checkFormNo == formNo && checkSexNo == sex) return(lastResult);

  if(!isLoaded || dataCount == 0) return(0);

  int result = -1;

  for(u32 z=0;z<dataCount;z++)
  {
    KWParamData_Option *data = GetData(z);

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

float KawaigariParamOptionManager :: GetFoodDist(int monsNo,int formNo,int sex,float defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->foodDist);
}

float KawaigariParamOptionManager :: GetFoodScale(int monsNo,int formNo,int sex,float defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)(data->foodScale/100.0f));
}

float KawaigariParamOptionManager :: GetAdjustAngleEat(int monsNo,int formNo,int sex,float defaultVal)
{    
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->adjustAngleEat);
}

float KawaigariParamOptionManager :: GetAdjustAngleLook(int monsNo,int formNo,int sex,float defaultVal)
{    
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->adjustAngleLook);
}

float KawaigariParamOptionManager :: GetAdjustAngleUpper(int monsNo,int formNo,int sex,float defaultVal)
{    
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->adjustAngleUpper);
}

float KawaigariParamOptionManager :: GetAdjustAngleLower(int monsNo,int formNo,int sex,float defaultVal)
{    
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->adjustAngleLower);
}


int KawaigariParamOptionManager :: GetFoodAnimMax(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->foodAnimMax);
}

bool KawaigariParamOptionManager :: UseSlowMotion(int monsNo,int formNo,int sex,bool defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->slowMotion!=0);
}

int KawaigariParamOptionManager :: GetIdleCameraXMin(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->idleCamXMin);
}

int KawaigariParamOptionManager :: GetIdleCameraXMax(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->idleCamXMax);
}

int KawaigariParamOptionManager :: GetIdleCameraYMin(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->idleCamYMin);
}

int KawaigariParamOptionManager :: GetIdleCameraYMax(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->idleCamYMax);
}

float KawaigariParamOptionManager :: GetOffsetEar(int monsNo,int formNo,int sex,float defaultVal)
{    
  KWParamData_Option *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return((float)data->offsetEar);
}

KWParamData_Option* KawaigariParamOptionManager :: GetData(int monsNo,int formNo,int sex)
{
  int index = GetIndex(monsNo,formNo,sex);
  return(GetData(index));
}

KWParamData_Option* KawaigariParamOptionManager :: GetData(u32 index)
{
  // No data to access?
  if(!isLoaded || dataCount==0 || index>=dataCount || index<0) return(NULL);

  index++;

  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(dataBuf);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;

  return( reinterpret_cast<KWParamData_Option*>(ptr));
}

bool KawaigariParamOptionManager :: Finalize(void)
{
  GflHeapFreeMemory(dataBuf);
  isLoaded = false;
  dataCount = 0;
  return(true);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

