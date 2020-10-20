//======================================================================
/**
 * @file	KawaigariParamMotionAffectionManager.c++
 * @brief	niji Kawaigari Parameter Motion Affection Manager
 * @author	PETE
 * @data	16/01/22
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include "KawaigariParamMotionAffectionManager.h"

// garc FileLoading
#include <arc_def_index/arc_def.h>

// File ID:
#include <arc_index/kawaigari_parameter_resource.gaix>

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariParamMotionAffectionManager :: KawaigariParamMotionAffectionManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  mFileManager = fileManager;

  // Load Parameter Resource:
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_KAWAIGARI_PARAMETER;  
  req.datId = GARC_kawaigari_parameter_resource_kw_param_data_motion_affection_BIN;
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

KawaigariParamMotionAffectionManager :: ~KawaigariParamMotionAffectionManager(void)
{
  if(isLoaded) GflHeapFreeMemory(dataBuf); // Forgot to Finalize()!
}

bool KawaigariParamMotionAffectionManager :: IsLoaded(void)
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

int KawaigariParamMotionAffectionManager :: GetIndex(int monsNo,int formNo,int sex)
{
  if(checkMonsNo == monsNo && checkFormNo == formNo && checkSexNo == sex) return(lastResult);

  if(!isLoaded || dataCount == 0) return(0);

  int result = -1;

  for(u32 z=0;z<dataCount;z++)
  {
    KWParamData_MotionAffection *data = GetData(z);

    if(data->idNum == monsNo) // ID matches MonsterNo
    {
      // Use first result or exact result:
      if( (formNo == data->form && (data->sex==sex || data->sex==0)) || result==-1)
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

int KawaigariParamMotionAffectionManager :: GetAffectionRequired(int monsNo,int formNo,int sex,int type,int defaultVal)
{
  KWParamData_MotionAffection *data = GetData(GetIndex(monsNo,formNo,sex));
  if(data==NULL || type<0 || type>=MOT_AFFECTION_MAX) return(defaultVal);  
  return(data->loveAmount[type]);
}


KWParamData_MotionAffection* KawaigariParamMotionAffectionManager :: GetData(int monsNo,int formNo,int sex)
{
  int index = GetIndex(monsNo,formNo,sex);
  return(GetData(index));
}

#if PM_DEBUG
void KawaigariParamMotionAffectionManager :: LoadHIO(char *path,gfl2::heap::HeapBase *dataHeap)
{
  if(!isLoaded) return;
  GflHeapFreeMemory(dataBuf);

  // Load Local File:
  size_t size;  
  gfl2::fs::PcUtil::ReadFile(&dataBuf,&size,NULL,path,dataHeap,8);

}
#endif

KWParamData_MotionAffection* KawaigariParamMotionAffectionManager :: GetData(u32 index)
{
  // No data to access?
  if(!isLoaded || dataCount==0 || index>=dataCount || index<0) return(NULL);

  index++;

  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(dataBuf);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;

  return( reinterpret_cast<KWParamData_MotionAffection*>(ptr));
}

bool KawaigariParamMotionAffectionManager :: Finalize(void)
{
  GflHeapFreeMemory(dataBuf);
  isLoaded = false;
  dataCount = 0;
  return(true);
}

  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)

