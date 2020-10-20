//======================================================================
/**
 * @file	KawaigariParamOptionCoreManager.c++
 * @brief	niji Kawaigari Parameter Option Core Manager
 * @author	PETE
 * @data	16/01/12
 */
//======================================================================
#include <Debug/include/gfl2_DebugPrint.h>
#include <PokeTool/include/KawaigariParamOptionCoreManager.h>

// garc FileLoading
#include <arc_def_index/arc_def.h>

// File ID:
#include <arc_index/kawaigari_parameter_resource.gaix>

GFL_NAMESPACE_BEGIN(PokeTool)  

KawaigariParamOptionCoreManager :: KawaigariParamOptionCoreManager(gfl2::heap::HeapBase *dataHeap,gfl2::heap::HeapBase *workHeap,gfl2::fs::AsyncFileManager *fileManager)
{
  mFileManager = fileManager;

  // Parameter GARC:
  {	    
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId				= ARCID_KAWAIGARI_PARAMETER;
    req.heapForFile	= workHeap;
    req.heapForReq	= workHeap;
    fileManager->SyncArcFileOpen(req);
  }

  // Load Parameter Resource:
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = ARCID_KAWAIGARI_PARAMETER;  
  req.datId = GARC_kawaigari_parameter_resource_kw_param_data_option_core_BIN;
  req.align = 128;
  req.heapForBuf = dataHeap;
  req.heapForReq = workHeap;
  req.heapForCompressed = NULL;
  
  req.pBufSize = NULL;
  req.ppBuf = &dataBuf;

  fileManager->AddArcFileLoadDataReq(req);
  dataCount = 0;
  isLoaded = false;
}

KawaigariParamOptionCoreManager :: ~KawaigariParamOptionCoreManager(void)
{
  if(isLoaded) GflHeapFreeMemory(dataBuf); // Forgot to Finalize()!
}

bool KawaigariParamOptionCoreManager :: IsLoaded(void)
{
  if(isLoaded) return(true);

  isLoaded = mFileManager->IsArcFileLoadDataFinished( &dataBuf );  

  if(isLoaded)
  {
    u32 *val = (u32*)dataBuf;
    dataCount = *val;    

    // Parameter GARC:
    {	    
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= ARCID_KAWAIGARI_PARAMETER;
      mFileManager->SyncArcFileClose(req);
    }
  }

   return isLoaded;
}

int KawaigariParamOptionCoreManager :: GetAdjustLookAngle(int monsNo,int formNo,int sex)
{
  if(!isLoaded || dataCount == 0) return(0);

  int result = 0;
  bool isSet = false;

  for(u32 z=0;z<dataCount;z++)
  {
    KWParamData_OptionCore *data = GetData(z);

    if(data->idNum == monsNo) // ID matches MonsterNo
    {
      // Use first result or exact result:
      if( (data->formNum == formNo && (data->sex==sex || data->sex==0)) || !isSet) 
      {
        result = data->adjustLookAngle;
        isSet=true;
      }
    }

    if(data->idNum>monsNo) break; // Can't find!
  }
 
  return(result);
}

KWParamData_OptionCore* KawaigariParamOptionCoreManager :: GetData(u32 index)
{
  // No data to access?
  if(!isLoaded || dataCount==0 || index>=dataCount) return(NULL);

  index++;

  // Use table index to calculate pointer to data:
  char *ptr = static_cast<char*>(dataBuf);

  u32 *off = reinterpret_cast<u32*>(ptr+(index*4)); 
  ptr += *off;

  return( reinterpret_cast<KWParamData_OptionCore*>(ptr));
}

bool KawaigariParamOptionCoreManager :: Finalize(void)
{
  GflHeapFreeMemory(dataBuf);
  isLoaded = false;
  dataCount = 0;
  return(true);
}
  
GFL_NAMESPACE_END(PokeTool)
  