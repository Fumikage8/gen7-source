//======================================================================
/**
* @file	ResourceManager.h
* @brief	リソース管理システム
* @author	ariizumi
* @data	15/04/29
 */
//======================================================================
#include <fs/include/gfl2_Fs.h>
#include <util/include/gfl2_std_string.h>
#include <Debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

#include "DemoLib/Object/include/ResourceManager.h"
#include "DemoLib/Object/include/ObjectUtil.h"

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

ResourceManager::ResourceManager(gfl2::fs::AsyncFileManager *fileMng,gfl2::heap::HeapBase* heap, u32 bufferNum )
:mList(heap,bufferNum)
,mFileManager(fileMng)
{

}

ResourceManager::~ResourceManager()
{


}

void ResourceManager::LoadRequest( gfl2::heap::HeapBase *bufHeap , 
                                  gfl2::heap::HeapBase *workHeap ,
                                  gfl2::fs::ArcFile::ARCID arcId , 
                                  gfl2::fs::ArcFile::ARCDATID datId , 
                                  bool isComp ,
                                  RESOURCE_TYPE type ,
                                  bool isUseGasc )
{	
  //重複チェック
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      //同一のデータを見つけました
      return;
    }
    ++it;
  }

	ResourceData *resData = GFL_NEW(bufHeap)ResourceData();

	resData->arcId = arcId;
	resData->datId = datId;
	resData->type = type;
  resData->resState = RESSTATE_LOADING;
  resData->bufHeap = bufHeap;
  resData->workHeap = workHeap;
  resData->isComp = isComp;
  resData->isUseGasc = isUseGasc;
  mList.PushBack(resData);
   
  LoadDataCore(resData);
}


bool ResourceManager::IsFinishLoad(void)
{
  //ファイルごとのチェックが必要な場合は処理を入れる
  if( mFileManager->IsAllReqFinished() )
  {
    gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
    while( (*it) != NULL )
    {
      ResourceData *data = (*it);
      data->resState = RESSTATE_LOADEND;
      ++it;
    }
    return true;
  }
  return false;
}

//読み込みリクエスト発行(最大読み込み個数制限
void ResourceManager::LoadRequestStep( gfl2::heap::HeapBase *bufHeap , 
                     gfl2::heap::HeapBase *workHeap ,
                     gfl2::fs::ArcFile::ARCID arcId , 
                     gfl2::fs::ArcFile::ARCDATID datId , 
                     bool isComp ,
                     RESOURCE_TYPE type ,
                     bool isUseGasc ) //デバッグ用のgarcから読むか設定
{
  //重複チェック
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      //同一のデータを見つけました
      return;
    }
    ++it;
  }

  ResourceData *resData = GFL_NEW(bufHeap)ResourceData();

  resData->arcId = arcId;
  resData->datId = datId;
  resData->type = type;
  resData->resState = RESSTATE_LOADWAIT;
  resData->bufHeap = bufHeap;
  resData->workHeap = workHeap;
  resData->isComp = isComp;
  resData->isUseGasc = isUseGasc;
  mList.PushBack(resData);
}

bool ResourceManager::IsFinishLoadStep(int loadNum)
{
  //データの先頭から必ず読み込みが開始される前提。

  int loadLimit = loadNum;

  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->resState == RESSTATE_LOADWAIT )
    {
      LoadDataCore(data);
      data->resState = RESSTATE_LOADING;
      loadLimit--;
      if( loadLimit <= 0 )
      {
        return false;
      }
    }
    else
    if( data->resState == RESSTATE_LOADING )
    {
      //読み込み中
      if( mFileManager->IsArcFileLoadDataFinished(&data->buf) )
      {
        data->resState = RESSTATE_LOADEND;
      }
      else
      {
        loadLimit--;
        if( loadLimit <= 0 )
        {
          return false;
        }
      }
    }
    //LOADENDは見ない
    ++it;
  }
  //全部LOADENDになると来る
  return true;
}

void ResourceManager::LoadDataCore(ResourceData *data)
{
  //読み込み前なので読み込みを開始する
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  req.arcId = data->arcId;  
  req.datId = data->datId;
  req.align = 128;
  req.heapForBuf = data->bufHeap;
  req.heapForReq = data->workHeap;

  if( data->isComp == true )
  {
    req.heapForCompressed = data->workHeap;
  }
  else 
  {
    req.heapForCompressed = NULL;
  }
  if( data->isUseGasc == true )
  {
    req.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
  }

  req.pBufSize = &data->size;
  req.ppBuf = &data->buf;

  mFileManager->AddArcFileLoadDataReq(req);
}

//Bufからデータを作る
void ResourceManager::CreateResource( 
  gfl2::heap::HeapBase *heap , 
  gfl2::gfx::IGLAllocator *allocator , 
  gfl2::Effect::System *effSys , 
  gfl2::Effect::GFHeap *effHeap ,
  EffectIndexManager *effIdxMng )
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    ++it;

    if( data->resState == RESSTATE_CREATE )
    {
      continue;
    }
    data->resState = RESSTATE_CREATE;

    if( data->type == RESOURCE_ANIME )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
      anmRes.SetMotionData((c8*)data->buf);

      data->resNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator,&anmRes );		
    }
    else
    if( data->type == RESOURCE_EFFECT )
    {
      data->effResIdx = effIdxMng->GetIndex();
      effSys->EntryResource( effHeap , data->buf , data->effResIdx );
    }
    else
    if( data->type == RESOURCE_TEXTURE )
    {
      // テクスチャ生成用のファクトリを取得
      gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
        gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
        gfl2::renderingengine::scenegraph::resource::IResourceData::Type("texture")
        );
			gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
				reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

      gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texRes;
      texRes.SetTextureData((c8*)data->buf);
			data->texNode = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNode*>(
          pTextureResourceNodeFactory->CreateResourceNode(allocator,texRes.GetData(0),texRes.GetDataSize(0)));
    }
    else
    if( data->type == RESOURCE_BINPACK )
    {
      data->mBinResouce = GFL_NEW(heap)gfl2::fs::BinLinkerAccessor(data->buf);
    }

  }
}

//データの取得
void* ResourceManager::GetDataBuf(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size )
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      if( size )
      {
        *size = data->size;
      }
      return data->buf;
    }
    ++it;
  }
  return NULL;
}
gfl2::renderingengine::scenegraph::resource::ResourceNode* ResourceManager::GetDataBuf_Anime(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size )
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      if( size )
      {
        *size = data->size;
      }
      return data->resNode;
    }
    ++it;
  }
  return NULL;
}

//RESOURCE_EFFECTで初期化したリソースのインデックス取得
int ResourceManager::GetEffectResIdx(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId)
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      return data->effResIdx;
    }
    ++it;
  }
  return -1;
}

//RESOURCE_TEXTUREで初期化したリソースの取得
gfl2::renderingengine::scenegraph::resource::TextureResourceNode* ResourceManager::GetDataBuf_Texture(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size )
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      if( size )
      {
        *size = data->size;
      }
      return data->texNode;
    }
    ++it;
  }
  return NULL;
}

//RESOURCE_BINPACKで初期化したリソースの取得
gfl2::fs::BinLinkerAccessor* ResourceManager::GetDataBuf_BinPack(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size )
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    if( data->arcId == arcId && data->datId == datId )
    {
      if( size )
      {
        *size = data->size;
      }
      return data->mBinResouce;
    }
    ++it;
  }
  return NULL;
}

void ResourceManager::DeleteResource(gfl2::fs::ArcFile::ARCID arcId ,gfl2::fs::ArcFile::ARCDATID datId, gfl2::Effect::System *effSys,EffectIndexManager *effIdxMng)
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    ++it;

    if( data->arcId == arcId && data->datId == datId )
    {
      if( data->effResIdx != -1 )
      {
        effSys->ClearResource( NULL, data->effResIdx);
        effIdxMng->ReturnIndex(data->effResIdx);
      }

      GFL_SAFE_DELETE( data->resNode );
      GFL_SAFE_DELETE( data->texNode );
      GFL_SAFE_DELETE( data->mBinResouce );

      mList.Remove(data);
      GflHeapFreeMemory(data->buf);
      GFL_SAFE_DELETE(data);

      ARIIZUMI_PRINT("ResourceManager DeleteResource[%d]\n",datId);
    }
  }
}

void ResourceManager::DeleteAllResource(gfl2::Effect::System *effSys,EffectIndexManager *effIdxMng)
{
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    ++it;

    if( data->effResIdx != -1 )
    {
      effSys->ClearResource( NULL, data->effResIdx);
      effIdxMng->ReturnIndex(data->effResIdx);
    }

    GFL_SAFE_DELETE( data->resNode );
    GFL_SAFE_DELETE( data->texNode );
    GFL_SAFE_DELETE( data->mBinResouce );

    mList.Remove(data);
    GflHeapFreeMemory(data->buf);
    GFL_SAFE_DELETE(data);
  }

}

//リソースが開放されているか？
bool ResourceManager::IsReleaseAllResouce(void)
{
  if (mList.Size() == 0)
  {
    return true;
  }
  return false;
}

void ResourceManager::Dump(void)
{
  GFL_PRINT("ResourceManager Dump\n");
  gfl2::util::List<ResourceData*>::Iterator it = mList.Begin();
  while( (*it) != NULL )
  {
    ResourceData *data = (*it);
    ++it;

    GFL_PRINT("Buf[0x%8x]Type[%d]Eff[%4d]ResNode[0x%8x]Bin[0x%8x]\n",data->buf,data->type,data->effResIdx,data->resNode,data->mBinResouce);
  }

}

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

