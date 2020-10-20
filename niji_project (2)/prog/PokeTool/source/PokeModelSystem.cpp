//======================================================================
/**
 * @file	PokeModelSystem.cpp
 * @brief	niji ポケモンモデルシステム
 * @author	ariizumi
 * @data	15/05/07
 */
//======================================================================
#include <debug/include/gfl2_DebugPrint.h>

#include <arc_def_index/arc_def.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNodeFactory.h>

#include "PokeTool/include/PokeModelSystem.h"
#include "./PokeModelLocal.h"

#include <arc_index/NijiRenderPath.gaix>
#include <arc_index/pokemon_model.gaix>

GFL_NAMESPACE_BEGIN(PokeTool)

//----------------------------------------------------------------------------
/**
  *  @brief  コンストラクタ
  */
//-----------------------------------------------------------------------------
PokeModelSystem::PokeModelSystem()
{
  mIsEnable = false;
  mIsUseHeap = NULL;
  mModelHeapArr = NULL;
}

//----------------------------------------------------------------------------
/**
  *  @brief  デストラクタ
  */
//-----------------------------------------------------------------------------
PokeModelSystem::~PokeModelSystem()
{

}

//----------------------------------------------------------------------------
/**
  *  @brief  システムの初期化(非同期版)
  *
  *  @param  gfl::heap::HeapBase *heap             ヒープ
  *  @param  gfl::heap::HeapBase *deviceHheap      デバイスヒープ
  *  @param  gfl::heap::HeapBase *fileReadHeap     ファイル読み込み用Heap
  *  @param  gfl::fs::FileReadManager* readMng     非同期読み込み用マネージャ
  *
  *  InitSystemAsyncをした後、WaitInitSystemAsyncを待ってから、CreateSystemを読んでください。
  */
//-----------------------------------------------------------------------------
void PokeModelSystem::InitSystemAsync( gfl2::heap::HeapBase *heap , gfl2::heap::HeapBase *workHeap , gfl2::fs::AsyncFileManager* fileMng , int modelNum )
{
  mFileReadManager = fileMng;
  mHeap = heap;
  mWorkHeap = workHeap;
  mModelNum = modelNum;

  mList = GFL_NEW(mHeap)gfl2::util::List<PokeModel*>(mHeap,modelNum);

  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
    req.arcId = ARCID_POKEMON_MODEL;
    req.heapForFile = mHeap;
    req.heapForReq = mWorkHeap->GetLowerHandle();
    mFileReadManager->AddArcFileOpenReq(req);

    //シェーダー用の処理 
    //@todo 将来的にはARCID_POKEMON_MODELに入れたい
    req.arcId = ARCID_RENDERPATH;
    mFileReadManager->AddArcFileOpenReq(req);

  }
  //ポケモン用シェーダーバイナリ
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = ARCID_RENDERPATH;
    req.datId = GARC_NijiRenderPath_PokeShader_BIN;
    req.align = 128;
    req.heapForBuf = mHeap;
    req.heapForReq = mWorkHeap->GetLowerHandle();
    req.heapForCompressed = NULL;
    req.ppBuf = &mPokeShaderResourceBuf;

    mFileReadManager->AddArcFileLoadDataReq(req);

    //IDテクスチャ用バイナリ
    req.datId = GARC_NijiRenderPath_IDTextureResource_BIN;
    req.ppBuf = &mIdTextureShaderResourceBuf;
    mFileReadManager->AddArcFileLoadDataReq(req);

  }
  //ポケモンGARC管理データ
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = ARCID_POKEMON_MODEL;
    req.datId = GARC_pokemon_model__poke_mng_data_DAT;
    req.align = 128;
    req.heapForBuf = mHeap;
    req.heapForReq = mWorkHeap->GetLowerHandle();
    req.heapForCompressed = NULL;
    req.ppBuf = &mPokeMngDataBuf;

    mFileReadManager->AddArcFileLoadDataReq(req);
  }
}

//----------------------------------------------------------------------------
/**
  *  @brief  システムの初期化待ち(非同期版)
  */
//-----------------------------------------------------------------------------
bool PokeModelSystem::IsFinishInitSystemAsync(void)
{
  if( mFileReadManager->IsArcFileOpenFinished(ARCID_POKEMON_MODEL) &&
      mFileReadManager->IsArcFileOpenFinished(ARCID_RENDERPATH) &&
      mFileReadManager->IsArcFileLoadDataFinished(&mPokeMngDataBuf) &&
      mFileReadManager->IsArcFileLoadDataFinished(&mPokeShaderResourceBuf)&&
      mFileReadManager->IsArcFileLoadDataFinished(&mIdTextureShaderResourceBuf) )
  {
    /*
    u32 num;
    mFileReadManager->GetArcFile(ARCID_POKEMON_MODEL)->GetDataCount(&num);
    for( int i=0;i<num;i++ )
    {
      size_t size;
      mFileReadManager->GetArcFile(ARCID_POKEMON_MODEL)->GetDataSize(&size,i,gfl2::fs::ToolDefine::ArcLangType::JPN,mWorkHeap);
      GFL_PRINT("[%2d][%6d]\n",i,size);
    }
    mIsEnable = true;
    */
    return true;
  }
  return false;
}

void PokeModelSystem::CreateSystem(gfl2::gfx::IGLAllocator *allocator,bool useColorShader,bool useIdMode)
{
  mPokeMngData = static_cast<POKE_MNG_DATA*>(mPokeMngDataBuf);
  mPokeFlgData = (POKE_FLG_DATA*)((int)(mPokeMngData) + (MONSNO_END+1)*sizeof(POKE_MNG_DATA));

#if 0
  for( int i=0;i<MONSNO_MAX;i++ )
  {
    ARIIZUMI_PRINT("[%3d]:[%3d][%3d][%3d]\n",i+1,mPokeMngData[i].mDataTop,mPokeMngData[i].mDataNum,mPokeMngData[i].mFlgData);
  }
  ARIIZUMI_PRINT("-----------------------------------------\n");
  for( int i=0;i<mPokeMngData[MONSNO_MAX-1].mDataTop + mPokeMngData[MONSNO_MAX-1].mDataNum;i++ )
  {
    //ARIIZUMI_PRINT("[%3d]:[%3d][%3d]\n",i+1,mPokeFlgData[i].mFlgData,mPokeFlgData[i].mSubFrom);
  }
#endif

  if( useColorShader )
  {
    mPokeShaderResourceBin.Initialize(mPokeShaderResourceBuf);

    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData mdlRes;
    mdlRes.SetModelData((c8*)mPokeShaderResourceBin.GetData(0));
    mPokeShaderResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator, &mdlRes );

    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode		*texResNode;
      texRes.SetTextureData((c8*)mPokeShaderResourceBin.GetData(1));
      texResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator , &texRes );
      mPokeShaderResourceNode->AddChild(texResNode);
    }
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shaderRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode *shaderResNode;
      shaderRes.SetShaderData( (c8*)mPokeShaderResourceBin.GetData(2) );

      shaderResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator , &shaderRes );
      mPokeShaderResourceNode->AddChild(shaderResNode);
    }
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData shaderRes;
      gfl2::renderingengine::scenegraph::resource::ResourceNode *shaderResNode;
      shaderRes.SetShaderData( (c8*)mPokeShaderResourceBin.GetData(3) );

      shaderResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator , &shaderRes );
      mPokeShaderResourceNode->AddChild(shaderResNode);
    }

    mPokeShaderModel = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(allocator,mPokeShaderResourceNode);

    //@todo 今は内部でnewなので、HEAP渡しできたら
    mPokeShaderRenderPaths.CreateBuffer(mHeap, poke_3d::model::PokemonModel::COLORTEXTURE_MAX*mModelNum);

    mViewportResetPath = GFL_NEW(mHeap)ViewportResetPath();

    mIsSetupShader = true;
  }
  else
  {
    mPokeShaderResourceNode = NULL;
    mViewportResetPath = NULL;

    mIsSetupShader = false;
  }

  if( useIdMode )
  {
    mIdTextureShaderResourceBin.Initialize(mIdTextureShaderResourceBuf);

#if 0
    gfl2::renderingengine::scenegraph::resource::GfBinaryShdData binShaderData;
    binShaderData.SetShaderData( (c8*)mIdTextureShaderResourceBin.GetData(0) );
    mIdTextureShaderResourceNode = 
      reinterpret_cast<gfl2::renderingengine::scenegraph::resource::ShaderResourceNode*>
        (gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( allocator, &binShaderData ));
#else
    // シェーダー生成用のファクトリを取得
    gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
      gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
      gfl2::renderingengine::scenegraph::resource::IResourceData::Type("shader")
      );
		gfl2::renderingengine::scenegraph::resource::ShaderResourceNodeFactory* pShaderResourceNodeFactory = 
			reinterpret_cast<gfl2::renderingengine::scenegraph::resource::ShaderResourceNodeFactory*>(pFactory);

    gfl2::renderingengine::scenegraph::resource::GfBinaryShdData binShaderData;
    binShaderData.SetShaderData( (c8*)mIdTextureShaderResourceBin.GetData(0) );

    mIdTextureShaderResourceNode = reinterpret_cast<gfl2::renderingengine::scenegraph::resource::ShaderResourceNode*>(
        pShaderResourceNodeFactory->CreateResourceNode(allocator,binShaderData.GetData(0),binShaderData.GetDataSize(0)));

#endif
  }
  else
  {
    mIdTextureShaderResourceNode = NULL;
  }

  mIsEnable = true;
}

//----------------------------------------------------------------------------
/**
  *  @brief  システムの開放(非同期版)
  */
//-----------------------------------------------------------------------------
void PokeModelSystem::TermSystemAsync(void)
{
  mIsEnable = false;

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.arcId = ARCID_POKEMON_MODEL;
  req.heapForReq = mWorkHeap->GetLowerHandle();
  mFileReadManager->AddArcFileCloseReq(req);

  req.arcId = ARCID_RENDERPATH;
  mFileReadManager->AddArcFileCloseReq(req);

  if( mIsSetupShader )
  {
    GFL_SAFE_DELETE(mViewportResetPath);
    GFL_SAFE_DELETE(mPokeShaderModel);
    GFL_SAFE_DELETE(mPokeShaderResourceNode);
  }

  GFL_SAFE_DELETE(mIdTextureShaderResourceNode);

  GflHeapFreeMemory(mIdTextureShaderResourceBuf);
  GflHeapFreeMemory(mPokeShaderResourceBuf);
  GflHeapFreeMemory(mPokeMngDataBuf);

  GFL_SAFE_DELETE(mList);
}
//----------------------------------------------------------------------------
/**
  *  @brief  システムの開放(非同期版)
  */
//-----------------------------------------------------------------------------
bool PokeModelSystem::IsFinishTermSystemAsync(void)
{
  if( mFileReadManager->IsArcFileCloseFinished(ARCID_POKEMON_MODEL) &&
      mFileReadManager->IsArcFileCloseFinished(ARCID_RENDERPATH) )
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
  *  @brief  PokeModelの各種更新
  */
//-----------------------------------------------------------------------------
void PokeModelSystem::Update(void)
{
  if( mIsEnable )
  {
    gfl2::util::List<PokeModel*>::Iterator it = mList->Begin();
    while( (*it) != NULL )
    {
      PokeModel *mdl = (*it);
      if( mdl->IsEnable() == true )
      {
        mdl->Update();
        mdl->UpdateAnimation();
        mdl->UpdateAfter();
      }
      ++it;
    }
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  PokeModelの各種更新
*/
//-----------------------------------------------------------------------------
void PokeModelSystem::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if( mIsEnable )
  {
    gfl2::util::List<PokeModel*>::Iterator it = mList->Begin();
    while( (*it) != NULL )
    {
      //PokeModel *mdl = (*it);
      //現状処理無し
      ++it;
    }
  }
}

//----------------------------------------------------------------------------
/**
  *  @brief  PokeModel生成(簡易パラメータ版)
  *
  *  @param  const SimpleParam *param                    シンプルパラム
  *  @param  const PokeModel::SetupOption *opt           セットアップオプション
  *  @return PokeModel*                                  ポケモンモデル
  */
//-----------------------------------------------------------------------------
PokeModel* PokeModelSystem::CreateModelSimple( const SimpleParam *param , const PokeModel::SetupOption &opt )
{
  gfl2::heap::HeapBase *heap = opt.dataHeap;
  void *vramAddr = NULL;
  if( mModelHeapArr )
  {
    for( int i=0;i<mModelNum;i++ )
    {
      if( mIsUseHeap[i] == false )
      {
        mIsUseHeap[i] = true;
        heap = mModelHeapArr[i];
        vramAddr = mModelVramArr[i];
        break;
      }
    }
  }
  else
  {
    GFL_PRINT("■□■□■□■□■□■□■□■□■□■□■□■□■\n");
    GFL_PRINT("□PokeModelSystem:専用ヒープが作られていません！□\n");
    GFL_PRINT("■□■□■□■□■□■□■□■□■□■□■□■□■\n");
#if defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko)
    //GFL_ASSERT_MSG(0,"■□■□■□■□■□■□■□■□■□■□■□■□■\n");
    //GFL_ASSERT_MSG(0,"□PokeModelSystem:専用ヒープが作られていません！□\n");
    //GFL_ASSERT_MSG(0,"■□■□■□■□■□■□■□■□■□■□■□■□■\n");
#endif
  }

  PokeModel *mdl = GFL_NEW(heap)PokeModel();

  mdl->CreateSimple(this,param,opt,heap,vramAddr);

  mList->PushBack(mdl);
  return mdl;
}

//----------------------------------------------------------------------------
/**
  *  @brief  PokeModel生成(PokeParam版)
  *
  *  @param  const pml::pokepara::PokemonParam *pp       ポケモンパラム
  *  @param  const PokeModel::SetupOption *opt           セットアップオプション
  *  @return PokeModel*                                  ポケモンモデル
  */
//-----------------------------------------------------------------------------
PokeModel* PokeModelSystem::CreateModelPP( const pml::pokepara::PokemonParam *pp , const PokeModel::SetupOption &opt )
{
  SimpleParam param;
  GetSimpleParam(&param,pp);
  return CreateModelSimple(&param,opt);
}

//----------------------------------------------------------------------------
/**
  *  @brief  PokeModel破棄
  */
//-----------------------------------------------------------------------------
void PokeModelSystem::DeleteModel( PokeModel *mdl )
{
  if( mModelHeapArr )
  {
    int i;
    for( i=0;i<mModelNum;i++ )
    {
      if( mIsUseHeap[i] == true && 
          mdl->GetDataHeap() == mModelHeapArr[i] )
      {
        mIsUseHeap[i] = false;
        break;
      }
    }
    if( i == mModelNum )
    {
      GFL_ASSERT_MSG(0,"PokeModelSys:Heap開放失敗!\n");
    }
  }

  mList->Remove(mdl);
  mdl->Delete();
  GFL_SAFE_DELETE(mdl);
}

//----------------------------------------------------------------------------
/**
  *  @brief  管理用ヒープの作成
  */
//-----------------------------------------------------------------------------
void PokeModelSystem::CreatePokeModelHeap( gfl2::heap::HeapBase *heap , const HeapOption *heapOption )
{
  mGLAllocator = GFL_NEW(mHeap)System::nijiAllocator(mHeap);

  mIsUseHeap = GFL_NEW_ARRAY(mHeap)bool[mModelNum];
  mModelHeapArr = GFL_NEW_ARRAY(mHeap)gfl2::heap::HeapBase*[mModelNum];
  mModelVramArr = GFL_NEW_ARRAY(mHeap)void*[mModelNum];

  const int size = CalcPokeModelHeapSize(heapOption);

  char heapName[16] = "PokeModel0\0";
  for( int i=0;i<mModelNum;i++ )
  {
    heapName[9] = '0'+i;
    mIsUseHeap[i] = false;
    mModelHeapArr[i] = GFL_CREATE_LOCAL_HEAP_NAME( heap ,  size , gfl2::heap::HEAP_TYPE_EXP , false , heapName );
    if( heapOption->useColorShader )
    {
#if defined(GF_PLATFORM_CTR)
      mModelVramArr[i] = mGLAllocator->LocalMemoryMalloc(heapOption->vramPool,POKEMODEL_VRAM_SIZE,128);
#elif defined(GF_PLATFORM_WIN32)
      //WinはVRAM無いので対応しない
      mModelVramArr[i] = NULL;
#endif  //プラットフォーム分岐
    }
    else
    {
      mModelVramArr[i] = NULL;
    }
  }
}

void PokeModelSystem::DeletePokeModelHeap(void)
{
  for( int i=0;i<mModelNum;i++ )
  {
    if( mModelVramArr[i] )
    {
      mGLAllocator->LocalMemoryFree(mModelVramArr[i]);
    }
    GFL_DELETE_HEAP( mModelHeapArr[i] );
  }
  GFL_DELETE_ARRAY(mModelVramArr);
  GFL_DELETE_ARRAY(mModelHeapArr);
  GFL_DELETE_ARRAY(mIsUseHeap);

  GFL_SAFE_DELETE(mGLAllocator);
}

//----------------------------------------------------------------------------
/**
  *  @brief  ポケモンモデルの表示に必要なヒープサイズの計算
  */
//-----------------------------------------------------------------------------
u32 PokeModelSystem::CalcPokeModelHeapSize( const HeapOption *heapOption )
{
  //前回MAX バトル　：ピカチュウ(たくましい)2.74MB+インスタンス
  //前回MAX 可愛がり：ピカチュウ(かっこいい)2.88MB+インスタンス
  //MemHeap側のサイズが0x38000
  
  if( heapOption->allocSize != 0 )
  {
    return heapOption->allocSize;
  }

  //なので3MB＋0x40000にしておく(3.25MB)
  int HEAP_SIZE = 0x340000;
  if( heapOption->useIdModel )
  {
    HEAP_SIZE += 0x4C0000;  //デスカーン(0x481500)
  }
  return HEAP_SIZE;
}

int PokeModelSystem::GetPokeShaderPathNum(void)const
{
  return mPokeShaderRenderPaths.buffer_size() + 1;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* PokeModelSystem::GetPokeShaderPath(u32 idx)
{
  if( idx < mPokeShaderRenderPaths.size() )
  {
    return mPokeShaderRenderPaths[idx];
  }
  if( idx == mPokeShaderRenderPaths.size() )
  {
    //このパスでViewportがリセットされる
    return mViewportResetPath;
  }
  return NULL;
}


gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath* PokeModelSystem::CreatePokeShaderPath(gfl2::heap::HeapBase *heap)
{

  gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath* path = GFL_NEW(heap) gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath();
  path->SetColorShaderRenderTargetPlane(mPokeShaderModel);

  mPokeShaderRenderPaths.push_back(path);
  return path;
}

void PokeModelSystem::DeletePokeShaderPath(gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath *path)
{
  if( path )
  {
    mPokeShaderRenderPaths.remove(path);
    GFL_DELETE(path);
  }
}

const PokeModelSystem::POKE_MNG_DATA *PokeModelSystem::GetMngData( int monsNo )
{
  GFL_ASSERT_MSG(mIsEnable,"PokeModelSystem:システムが初期化されていません！\n");
  return &mPokeMngData[monsNo-1];
}

const PokeModelSystem::POKE_FLG_DATA *PokeModelSystem::GetFlgData( int monsNo , int formNo , int sex )
{
  GFL_ASSERT_MSG(mIsEnable,"PokeModelSystem:システムが初期化されていません！\n");
  //const POKE_MNG_DATA *mngData = GetMngData( monsNo );

  int dataIdx = GetDataIdx(monsNo,formNo,sex);
  return &mPokeFlgData[dataIdx];
  
}

int PokeModelSystem::GetDataIdx( int monsNo , int formNo , int sex )
{
  GFL_ASSERT_MSG(mIsEnable,"PokeModelSystem:システムが初期化されていません！\n");
  const POKE_MNG_DATA *mngData = GetMngData( monsNo );
  
  int dataIdx = 0;
  //データはオス、メス、フォルム違いの順で入る。
  //メガ進化がフォルム違いなので、雌雄差ありでフォルム違いがある場合がある。
  //フォルム違いの分まで雌雄差があるやつは今のところいない。
  
  if( mngData->mFlgData & POKE_MNG_FLG_EXIST_FORM_CHANGE &&
      formNo > 0 )
  {
    if( mngData->mFlgData & POKE_MNG_FLG_EXIST_FEMALE )
    {
      dataIdx = formNo+1;
    }
    else
    {
      dataIdx = formNo;
    }
  }
  else
  if( sex == pml::SEX_FEMALE && (mngData->mFlgData & POKE_MNG_FLG_EXIST_FEMALE) )
  {
    dataIdx = 1;
  }
  else
  {
    dataIdx = 0;
  }

  if( dataIdx >= mngData->mDataNum )
  {
    GFL_PRINT("Poke[%d][%d][%d] dataIdx over![Top:%d][Num:%d]\n",monsNo , formNo , sex , mngData->mDataTop , mngData->mDataNum );
    dataIdx = 0;
  }
  return mngData->mDataTop+dataIdx;
}

//カラーシェーダー描画後のViewportのリセットだけを行うパス
ViewportResetPath::ViewportResetPath()
  :gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath()
{
}

ViewportResetPath::~ViewportResetPath()
{
}

void ViewportResetPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
	gfl2::gfx::GFGL::BeginScene();
  if ( gfl2::gfx::GFGL::GetRenderTarget() )
	{
		if ( rDrawContext.m_DisplayWidth && rDrawContext.m_DisplayHeight )
		{
			gfl2::gfx::GFGL::SetViewport(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
			gfl2::gfx::GFGL::SetScissor(0, 0, rDrawContext.m_DisplayWidth, rDrawContext.m_DisplayHeight );
		}
		else
		{
			const gfl2::gfx::Surface::SurfaceDesc			&rDesc = gfl2::gfx::GFGL::GetRenderTarget()->GetSurfaceDesc();
			gfl2::gfx::GFGL::SetViewport(0, 0, rDesc.width, rDesc.height);
			gfl2::gfx::GFGL::SetScissor(0, 0, rDesc.width, rDesc.height);
		}
	}
#if defined(GF_PLATFORM_CTR)
  gfl2::gfx::GFGL::FlushAllDirtyCommand();
#endif  //プラットフォーム分岐
	gfl2::gfx::GFGL::EndScene();
}

//PokeModel専用アロケーター
PokeModelAllocator::PokeModelAllocator( gfl2::heap::HeapBase *devHeap , void* vramTop ,u32 vramSize)
  :gfl2::util::GLHeapAllocator(devHeap)
  ,mVramTop(vramTop)
  ,mVramOffset(0)
  ,mVramSize(vramSize)
{

}

PokeModelAllocator::~PokeModelAllocator()
{

}

void* PokeModelAllocator::LocalMemoryMalloc(u32 size, u32 alignment)
{
  void* ret =  reinterpret_cast<void*>(reinterpret_cast<size_t>(mVramTop) + mVramOffset);
  mVramOffset += size;
  GFL_ASSERT_MSG(mVramOffset<=mVramSize,"PokeModelAllocator VramSize is ove!![%x:%x]\n",mVramOffset,mVramSize);
  //ARIIZUMI_PRINT("PokeModelVram [%x][%x->%x]\n",ret,mVramSize-mVramOffset+size,mVramSize-mVramOffset);
  return ret;
}

void PokeModelAllocator::LocalMemoryFree(void* const address)
{
  //nofunc
}

GFL_NAMESPACE_END(PokeTool)