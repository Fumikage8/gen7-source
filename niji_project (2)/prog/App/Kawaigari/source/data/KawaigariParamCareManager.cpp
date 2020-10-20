//======================================================================
/**
 * @file	KawaigariParamCareManager.c++
 * @brief	niji Kawaigari Parameter Care Manager
 * @author	PETE
 * @data	16/01/28
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include "KawaigariParamCareManager.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

// File ID:
#include <arc_index/kawaigari_parameter_resource.gaix>

#define TYPE_ENABLE_DIRT (1)
#define TYPE_ENABLE_DUST (2)
#define TYPE_ENABLE_HAIR (4)
#define TYPE_ENABLE_WATER (8)

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariParamCareManager :: KawaigariParamCareManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  mFileManager = fileManager;

  // Load Parameter Resource:
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_KAWAIGARI_PARAMETER;  
  req.datId = GARC_kawaigari_parameter_resource_kw_param_data_caremode_BIN;
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

KawaigariParamCareManager :: ~KawaigariParamCareManager(void)
{
  if(isLoaded) GflHeapFreeMemory(dataBuf); // Forgot to Finalize()!
}

#if PM_DEBUG
void KawaigariParamCareManager :: LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap)
{
  if(!isLoaded) return;
  GflHeapFreeMemory(dataBuf);

  // Load Local File:
  size_t size;  
  gfl2::fs::PcUtil::ReadFile(&dataBuf,&size,NULL,path,dataHeap,8);

}
#endif

bool KawaigariParamCareManager :: IsLoaded(void)
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

int KawaigariParamCareManager :: GetIndex(int monsNo,int formNo,int sex)
{
  if(checkMonsNo == monsNo && checkFormNo == formNo && checkSexNo == sex) return(lastResult);

  if(!isLoaded || dataCount == 0) return(0);

  int result = -1;

  for(u32 z=0;z<dataCount;z++)
  {
    KWParamData_Care *data = GetData(z);

    if(data->idNum == monsNo) // ID matches MonsterNo
    {
      // Use first result or exact result:
      if( (formNo == data->formNum && (data->sex==sex || data->sex==0)) || result==-1)
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

int KawaigariParamCareManager :: GetJointCount(int monsNo,int formNo,int sex,int defaultVal)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultVal);  
  return(data->jointCount);
}

void KawaigariParamCareManager :: GetJointPos(int monsNo,int formNo,int sex,int index,gfl2::math::Vector3 &pos)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL || index<0 || index>=MAX_CARE_JOINT)
  {
    pos.x = pos.y = pos.z = 0.0f;
    return;
  }

  pos.x = (float)data->xPos[index];
  pos.y = (float)data->yPos[index];
  pos.z = (float)data->zPos[index];
  
}

float KawaigariParamCareManager :: GetScale(int monsNo,int formNo,int sex,int index,float defaultVal)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL || index<0 || index>=MAX_CARE_JOINT)
  {    
    return(defaultVal);
  }
  return((float)data->scale[index]/100.0f);
}

int KawaigariParamCareManager :: GetJointID(int monsNo,int formNo,int sex,int index,int defaultVal)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL || index<0 || index>=MAX_CARE_JOINT)
  {    
    return(defaultVal);
  }
  return(data->jointID[index]);
}

bool KawaigariParamCareManager :: TypeEnabled_Dirt(int monsNo,int formNo,int sex,bool defaultValue)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultValue);
  
  return( (data->typeCanUse & TYPE_ENABLE_DIRT) != 0 );
}

bool KawaigariParamCareManager :: TypeEnabled_Dust(int monsNo,int formNo,int sex,bool defaultValue)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultValue);

  return( (data->typeCanUse & TYPE_ENABLE_DUST) != 0 );
}

bool KawaigariParamCareManager :: TypeEnabled_Hair(int monsNo,int formNo,int sex,bool defaultValue)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultValue);

  return( (data->typeCanUse & TYPE_ENABLE_HAIR) != 0 );
}

bool KawaigariParamCareManager :: TypeEnabled_Water(int monsNo,int formNo,int sex,bool defaultValue)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL) return(defaultValue);

  return( (data->typeCanUse & TYPE_ENABLE_WATER) != 0 );
}

KWParamData_Care *KawaigariParamCareManager :: GetCareData(int monsNo,int formNo,int sex)
{
  KWParamData_Care *data = GetData(GetIndex(monsNo,formNo,sex));  
  return(data);
}

KWParamData_Care* KawaigariParamCareManager :: GetData(int monsNo,int formNo,int sex)
{
  int index = GetIndex(monsNo,formNo,sex);
  return(GetData(index));
}

KWParamData_Care* KawaigariParamCareManager :: GetData(u32 index)
{
  // No data to access?
  if(!isLoaded || dataCount==0 || index>=dataCount || index<0) return(NULL);

  index++;

  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(dataBuf);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;

  return( reinterpret_cast<KWParamData_Care*>(ptr));
}

bool KawaigariParamCareManager :: Finalize(void)
{
  GflHeapFreeMemory(dataBuf);
  isLoaded = false;
  dataCount = 0;
  return(true);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

