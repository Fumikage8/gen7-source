#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__
//======================================================================
/**
 * @file	ResourceManager.h
 * @brief	リソース管理システム
 * @author	ariizumi
 * @data	15/04/29
 */
//======================================================================

#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_List.h>
#include <fs/include/gfl2_Fs.h>
#include <effect/include/gfl2_EffectSystem.h>

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( scenegraph )
      GFL_NAMESPACE_BEGIN( resource )
        class ResourceNode;
        class TextureResourceNode;
      GFL_NAMESPACE_END( resource )
    GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_END( renderingengine )
  GFL_NAMESPACE_BEGIN( gfx )
    class IGLAllocator;
  GFL_NAMESPACE_END( gfx )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class EffectIndexManager;

typedef enum
{
  RESOURCE_NORMAL,
  RESOURCE_ANIME,
  RESOURCE_EFFECT,
  RESOURCE_TEXTURE,
  RESOURCE_BINPACK,
}RESOURCE_TYPE;

typedef enum
{
  RESSTATE_LOADWAIT,
  RESSTATE_LOADING,
  RESSTATE_LOADEND,
  RESSTATE_CREATE,
}RESOURCE_STATE;

struct ResourceData
{
  gfl2::fs::ArcFile::ARCID    arcId;
  gfl2::fs::ArcFile::ARCDATID datId;
  u32 size;
  void *buf;
  RESOURCE_TYPE type;
  //CreateResourceでデータを作る用(ANIME・TEXTUREで使用
  gfl2::renderingengine::scenegraph::resource::ResourceNode *resNode;
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode *texNode;
  //BinLinkerでパックされたリソース RESOURCE_BINPACKで使用
  gfl2::fs::BinLinkerAccessor *mBinResouce;
  //Effect用リソースインデックス
  int effResIdx;

  RESOURCE_STATE resState;

  //後から読むのに必要
  gfl2::heap::HeapBase *bufHeap;
  gfl2::heap::HeapBase *workHeap;
  bool isComp;
  bool isUseGasc;

  ResourceData()
    :arcId(gfl2::fs::ArcFile::ARCID_NULL)
    ,datId(gfl2::fs::ArcFile::ARCDATID_NULL)
    ,size(0)
    ,buf(NULL)
    ,type(RESOURCE_NORMAL)
    ,resNode(NULL)
    ,effResIdx(-1)
    ,resState(RESSTATE_LOADWAIT)
    ,bufHeap(NULL)
    ,workHeap(NULL)
    ,isComp(false)
    ,isUseGasc(false)
  {}
};


class ResourceManager
{
public:
  ResourceManager(gfl2::fs::AsyncFileManager *fileMng,gfl2::heap::HeapBase* heap, u32 bufferNum );
  ~ResourceManager();

  //読み込みリクエスト発行
  void LoadRequest( gfl2::heap::HeapBase *bufHeap , 
                    gfl2::heap::HeapBase *workHeap ,
                    gfl2::fs::ArcFile::ARCID arcId , 
                    gfl2::fs::ArcFile::ARCDATID datId , 
                    bool isComp ,
                    RESOURCE_TYPE type = RESOURCE_NORMAL ,
                    bool isUseGasc = false ); //デバッグ用のgarcから読むか設定

  //読み込み終了確認(FileManagerすべてのリクエストの完了をチェックしています。)
  bool IsFinishLoad(void);

  //読み込みリクエスト発行(最大読み込み個数制限
  void LoadRequestStep( gfl2::heap::HeapBase *bufHeap , 
    gfl2::heap::HeapBase *workHeap ,
    gfl2::fs::ArcFile::ARCID arcId , 
    gfl2::fs::ArcFile::ARCDATID datId , 
    bool isComp ,
    RESOURCE_TYPE type = RESOURCE_NORMAL ,
    bool isUseGasc = false ); //デバッグ用のgarcから読むか設定
  //読み込み終了確認(FileManagerすべてのリクエストの完了をチェックしています。)

  //@param loadNum 同時読み込み数
  bool IsFinishLoadStep(int loadNum = 1);

  //Bufからデータを作る
  void CreateResource( gfl2::heap::HeapBase *heap , 
                       gfl2::gfx::IGLAllocator *allocator , 
                       gfl2::Effect::System *effSys = NULL, 
                       gfl2::Effect::GFHeap *effHeap = NULL ,
                       EffectIndexManager *effIdxMng = NULL );

  //データの取得
  void* GetDataBuf(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size = NULL);
  //RESOURCE_ANIMEで初期化したリソースの取得
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetDataBuf_Anime(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size = NULL);
  //RESOURCE_EFFECTで初期化したリソースのインデックス取得
  int GetEffectResIdx(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId);
  //RESOURCE_TEXTUREで初期化したリソースの取得
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetDataBuf_Texture(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size = NULL);
  //RESOURCE_BINPACKで初期化したリソースの取得
  gfl2::fs::BinLinkerAccessor *GetDataBuf_BinPack(gfl2::fs::ArcFile::ARCID arcId , gfl2::fs::ArcFile::ARCDATID datId , u32 *size = NULL);

  void DeleteResource(gfl2::fs::ArcFile::ARCID arcId ,gfl2::fs::ArcFile::ARCDATID datId, gfl2::Effect::System *effSys = NULL,EffectIndexManager *effIdxMng=NULL);
  void DeleteAllResource(gfl2::Effect::System *effSys = NULL,EffectIndexManager *effIdxMng=NULL);

  //リソースが開放されているか？
  bool IsReleaseAllResouce(void);

  void Dump(void);
private:

  void LoadDataCore(ResourceData *data);

  gfl2::util::List<ResourceData*> mList;

  gfl2::fs::AsyncFileManager *mFileManager;
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
#endif // __TASKSYSTEM_H__
