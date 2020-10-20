//======================================================================
/**
 * @file  KawaigariRenderPipeLine.c++
 * @brief Kawaigari Rendering PipeLine
 * @author  Pete
 * @data  15/10/13
 * 
 *  2016.4.13    ishiguro  SetViewProjectionDirect, SetSkyMtxDirect, SetFarMtxDirect, MinimalUpdate関数追加
 * 
 */
//======================================================================
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_DressUpModel.h>
#include "KawaigariRenderPipeline.h"
#include "KawaigariMain.h"
#include "System/include/GflUse.h"

#include "KawaigariEdgeIDRenderPath.h"

#include <gfx/include/gfl2_PCDebugUtil.h>
#include <niji_conv_header/demoseq/seq_com_define.h>

GFL_NAMESPACE_BEGIN(App)
	GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariRenderingPipeLine::KawaigariRenderingPipeLine( KawaigariMain *core , f32 screenWidth, f32 screenHeight )
:mCore(core)
{
  mEdgeMapPath = NULL;
  mMameEdgeMapPath = NULL;

  mOutLinePath = NULL;
  mMameOutLinePath = NULL;
  
  mLayoutRenderPath = NULL;
  
  mEffectRenderPath = NULL;
  mEffectFixedSizeRenderPath = NULL;

  mSkyRenderPath = NULL;
  
  mModelRenderPath = NULL;
  mMameModelRenderPath = NULL;
  mModelRenderPathShadow = NULL;
  
  mIDTextureRenderPath = NULL;
  mModelRenderPathAfterEdge = NULL;
  mScreenEffectRenderPath = NULL;
  mScreenModelRenderPath = NULL;  
  pEnvNodeBase = NULL;
  mDBCnt = 0;
  mOffscreenBuffer[0] = NULL;
  mOffscreenBuffer[1] = NULL;
  mDepthStencilTexture = NULL;
  mClearDepthPath = NULL;
  mIsBottomScreenRender = false;
  mIsUseTouchID = false;
}

KawaigariRenderingPipeLine::~KawaigariRenderingPipeLine()
{
}

void KawaigariRenderingPipeLine::Dump(void)
{
  /*
  gfl2::heap::HeapBase *heap = mCore->GetHeap();
  if(mOffscreenBuffer==NULL) return;

///  gfl2::gfx::PCDebugUtil::PrintImage(mOffscreenBuffer, "helloworld.bmp", 512, 256 , 3, heap);

  u32 tx,ty;
  mCore->GetUIManager()->GetTouchRawPos(tx,ty);

  u8 *buf = (u8*)mOffscreenBuffer[mDBCnt];

#if defined(GF_PLATFORM_CTR) // 3DS:  
  u32 off = (512-320)*256*3;
  off += (tx*256*3) + (ty*3);
#else // Win32
  // Reverse Y:
  ty = 239 - ty;
  u32 off = (ty*512*3) + (tx*3);
#endif

  u8 r,g,b;

  b = buf[off];
  g = buf[off+1];
  r = buf[off+2];

  mCore->GetUIManager()->GetTouchRawPos(tx,ty);
  ///GFL_PRINT("Touched Pos: %i,%i   RGB: 0x%02X 0x%02X 0x%02X\n",tx,ty,r,g,b);
  if(r!=0xFF) GFL_PRINT("*** TOUCHED PICKACHU!***  RGB: 0x%02X 0x%02X 0x%02X\n\n",tx,ty,r,g,b);

  mIDTextureRenderPath->Dump();
  */
}

int KawaigariRenderingPipeLine::GetTouchColor(void)
{
  // Win32:
  u32 tx,ty;
  ///mCore->GetUIManager()->GetTouchRawPos(tx,ty);
  gfl2::ui::TouchPanel * touchPanel;
  touchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

  if(!touchPanel->IsTouch()) return(0);

  //Dump();
  
   tx = touchPanel->GetX();
   ty = touchPanel->GetY();
  
  u8 *buf = (u8*)mOffscreenBuffer[mDBCnt];

#if defined(GF_PLATFORM_CTR) // 3DS:  
  u32 off = (512-320)*256*3;
  off += (tx*256*3) + ( (239-ty)*3);
#else // Win32
  // Reverse Y:
  ty = 239 - ty;
  u32 off = (ty*512*3) + (tx*3);
#endif
  
  
  u8 r = buf[off+2];

  if(r!=0xFF)
  {    
    GFL_PRINT("*TouchID: %i  %i  %i\n",buf[off+2],buf[off+1],buf[off]);
      return(r); 
  }
  return(0); // Not touching...
}

void KawaigariRenderingPipeLine::Initialize(void)
{
	gfl2::heap::HeapBase *heap = mCore->GetHeap();
  gfl2::gfx::IGLAllocator *allocator = mCore->GetGLAllocator();
  
  for( int i = 0; i < 2; ++i )
  {
    mOffscreenBuffer[i] = (u32*)GflHeapAllocMemoryAlign(heap, 3 * 256 * 512, 16);
  }
  

  mModelRenderPath = GFL_NEW(heap)System::ModelRenderPath(allocator,256);
  mMameModelRenderPath = GFL_NEW(heap)System::ModelRenderPath(allocator,256);
  mModelRenderPathShadow = GFL_NEW(heap)System::ModelRenderPath(allocator,256);

  mIDTextureRenderPath = GFL_NEW(heap)KawaigariEdgeIDRenderPath(allocator,128);

  mModelRenderPathAfterEdge  = GFL_NEW(heap)System::ModelRenderPath(allocator,128);

  mScreenModelRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);

	mSkyRenderPath    = GFL_NEW(heap)System::ModelRenderPath(allocator,8);
	
  mEdgeMapPath      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,256);
  mMameEdgeMapPath      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,256);
	
  mOutLinePath      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
  mMameOutLinePath      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
	
  mLayoutRenderPath = GFL_NEW(heap)gfl2::lyt::LayoutRenderPath(allocator);

  mEffectRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mEffectFixedSizeRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;

  mScreenEffectRenderPath = GFL_NEW(heap)EffectScreenRenderPath;

  mClearDepthPath = GFL_NEW(heap)System::ClearDepthRenderPath;
  
}

void KawaigariRenderingPipeLine::Terminate(void)
{
  for( int i = 0; i < 2; ++i )
  {
    GFL_SAFE_DELETE(mOffscreenBuffer[i]);
  }
  GFL_SAFE_DELETE(mScreenEffectRenderPath);
  GFL_SAFE_DELETE(mEffectRenderPath);
  GFL_SAFE_DELETE(mEffectFixedSizeRenderPath);
	
  GFL_SAFE_DELETE(mOutLinePath);
  GFL_SAFE_DELETE(mMameOutLinePath);

  GFL_SAFE_DELETE(mEdgeMapPath);
  GFL_SAFE_DELETE(mMameEdgeMapPath);

	GFL_SAFE_DELETE(mSkyRenderPath);
	
  GFL_SAFE_DELETE(mModelRenderPath);
  GFL_SAFE_DELETE(mMameModelRenderPath);
  
  GFL_SAFE_DELETE(mModelRenderPathShadow);  
  GFL_SAFE_DELETE(mIDTextureRenderPath);
  GFL_SAFE_DELETE(mModelRenderPathAfterEdge);

  GFL_SAFE_DELETE(mScreenModelRenderPath);

  GFL_SAFE_DELETE(mLayoutRenderPath);

  GFL_SAFE_DELETE(mDepthStencilTexture);

  GFL_SAFE_DELETE(mClearDepthPath);
}

void KawaigariRenderingPipeLine::Setup(void *edgeRes,void *outLineRes)
{

  gfl2::gfx::IGLAllocator *allocator = mCore->GetGLAllocator();
  
	if(mEdgeMapPath) mEdgeMapPath->SetResource(edgeRes);
	if(mEdgeMapPath) mEdgeMapPath->CreateEdgeMapTexture(allocator, 512,256 );
	if(mOutLinePath) mOutLinePath->SetResource(outLineRes);

  if(mMameEdgeMapPath) mMameEdgeMapPath->SetResource(edgeRes);
  if(mMameEdgeMapPath) mMameEdgeMapPath->CreateEdgeMapTexture(allocator, 512,256 );
  if(mMameOutLinePath) mMameOutLinePath->SetResource(outLineRes);
  
	{
		// メインとなるシーン描画パスを初期化
		gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
		desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
		desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    if( mModelRenderPath ) mModelRenderPath->Initialize(allocator, desc );
    if( mMameModelRenderPath ) mMameModelRenderPath->Initialize(allocator, desc );
    if( mModelRenderPathShadow ) mModelRenderPathShadow->Initialize(allocator, desc );    
    
    if( mIDTextureRenderPath )
    {
      mIDTextureRenderPath->Initialize(allocator, desc );
      mIDTextureRenderPath->SetupRenderTargetTexture(512,256);
      mIDTextureRenderPath->SetCopyBuffer(mOffscreenBuffer[mDBCnt]);
    }

    if( mModelRenderPathAfterEdge ) mModelRenderPathAfterEdge->Initialize(allocator, desc );
    if( mEdgeMapPath ) mEdgeMapPath->Initialize(allocator, desc );
    if( mMameEdgeMapPath ) mMameEdgeMapPath->Initialize(allocator, desc );
	}

	{
		// メインとなるシーン描画パスを初期化
		gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
		desc.m_DrawManagerDesc.m_NodeStackSize	  = 8;
		desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 8;

		if( mSkyRenderPath ) mSkyRenderPath->Initialize(allocator, desc );
	}


  {
    // スクリーン貼り付けシーン描画パスを初期化
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 16;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    if( mScreenModelRenderPath ) mScreenModelRenderPath->Initialize(allocator, desc );
  }

  if( mEdgeMapPath && mOutLinePath )
  {
	  for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
	  {
  		const gfl2::gfx::Texture *tex = mEdgeMapPath->GetEdgeMapTexture( i );
		  mOutLinePath->SetEdgeMapTexture(i,tex);
	  }
  }

  if( mMameEdgeMapPath && mMameOutLinePath )
  {
    for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
    {
      const gfl2::gfx::Texture *tex = mMameEdgeMapPath->GetEdgeMapTexture( i );
      mMameOutLinePath->SetEdgeMapTexture(i,tex);
    }
  }
 

  if(mScreenModelRenderPath) mScreenModelRenderPath->SetCameraParam(&mScreenCamera);
  if(mScreenEffectRenderPath) mScreenEffectRenderPath->SetCameraParam(&mEffectScreenCamera);
  
#if defined(GF_PLATFORM_CTR)
  if(mOutLinePath)
  {  
    mDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(allocator, gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    mOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, mDepthStencilTexture);
  }

  if(mMameOutLinePath)
  {  
    //mDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(allocator, gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    mMameOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, mDepthStencilTexture);
  }
#endif
 
}

void KawaigariRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  if(mModelRenderPath) mModelRenderPath->AddDrawEnv( pNode );  
  if(mMameModelRenderPath) mMameModelRenderPath->AddDrawEnv( pNode );  
  if(mModelRenderPathShadow) mModelRenderPathShadow->AddDrawEnv( pNode );    
  pEnvNodeBase = pNode;
}

void KawaigariRenderingPipeLine::SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{ 
  if(!mScreenModelRenderPath) return;

#if defined(GF_PLATFORM_CTR)
  gfl2::math::Matrix44 tempProj[2];   // 透視変換行列
  gfl2::math::Matrix34 tempView[2];   // ビュー変換行列
  f32 depthLevel = 4.0f;
  f32 factor	   = 1.0f;
  gfl2::gfx::GFGL::GetStereoMatrix(	projection,
    view,
    depthLevel,
    factor, 
    &tempProj[0],
    &tempView[0],
    &tempProj[1],
    &tempView[1]);

  mScreenCamera.m_Projection = tempProj[0];
  mScreenCamera.m_View = tempView[0];
  mEffectScreenCamera.m_Projection = tempProj[0];
  mEffectScreenCamera.m_View = tempView[0];
#else
  mScreenCamera.m_Projection = projection;
  mScreenCamera.m_View = view;
  mEffectScreenCamera.m_Projection = projection;
  mEffectScreenCamera.m_View = view;
#endif

  mScreenModelRenderPath->SetCameraParam(&mScreenCamera);
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 KawaigariRenderingPipeLine::StartRenderPath(void)
{
  mRenderPathCnt = 0;
  return true;
}

enum
{
  PATH_SKY,
  PATH_FAR_BG,
  PATH_BG,
  PATH_BG_NEAR,
  PATH_CLEAR_DEPTH,
  PATH_EDGE,  
  PATH_MODEL_SHADOW,  
  PATH_MODEL,          
  PATH_OUTLINE,
  PATH_EFFECT,  
  PATH_EDGE_MAME,
  PATH_MODEL_MAME,
  PATH_OUTLINE_MAME,  
  PATH_EFFECT_FIXED_SIZE,  
  PATH_IDTEXTURE,      
  PATH_MAX
};

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* KawaigariRenderingPipeLine::GetRenderPath(void)
{
  u32 pokePathNum = 0;
  if( mCore->GetPokeModelSys() )
  {
    pokePathNum = mCore->GetPokeModelSys()->GetPokeShaderPathNum();

    if( mRenderPathCnt < pokePathNum )
    {
      if(mIsBottomScreenRender || mCore->GetPokemonAI()->IsDualScreenMonster())
        return mCore->GetPokeModelSys()->GetPokeShaderPath(mRenderPathCnt);
      else return NULL;
    }
  }

  //@caution ここにパスを追加する際は、KawaigariRenderingPipeLine::SetViewProjectionDirect関数にマトリックスの更新を足してください。
	switch( mRenderPathCnt-pokePathNum )
	{
  
  case PATH_SKY:      
      return mCore->GetBgSystem()->GetSkyRenderPath();  
    
      
  case PATH_FAR_BG:
    //if(mIsBottomScreenRender || mCore->GetPokemonAI()->IsDualScreenMonster())
      return mCore->GetBgSystem()->GetFarRenderPath();
   // else return NULL;
    
  case PATH_BG:
   // if(mIsBottomScreenRender)
    {      
      return mCore->GetBgSystem()->GetRenderPath();
    }
  //  else return NULL;
    
  /*case PATH_BG_NEAR:
    if(!mIsBottomScreenRender)
    {
      return mCore->GetBgSystemNear()->GetRenderPath();
    }
    else return NULL;
    */
  case PATH_CLEAR_DEPTH:    
     if(mIsBottomScreenRender)
      return mClearDepthPath;
     else return NULL;
  
  case PATH_MODEL:
    if(mIsBottomScreenRender || mCore->GetPokemonAI()->IsDualScreenMonster())
			return mModelRenderPath;
    else return NULL;

  case PATH_MODEL_MAME:
    SetProjectionMatrix(mEatProjMtx);
    SetViewMatrix(mEatViewMtx);         
    return mMameModelRenderPath;
    
  case PATH_MODEL_SHADOW:
    if(mIsBottomScreenRender)
      return mModelRenderPathShadow;
    else return NULL;

     case PATH_IDTEXTURE:
       SetProjectionMatrix(mBaseProjMtx);
       SetViewMatrix(mBaseViewMtx);       

      if(mIsBottomScreenRender && mIsUseTouchID)
        return mIDTextureRenderPath;
      else return NULL;   

		
    case PATH_EDGE:
      if(mIsBottomScreenRender || mCore->GetPokemonAI()->IsDualScreenMonster())
			  return mEdgeMapPath;
      else return NULL;

    case PATH_EDGE_MAME:
      SetProjectionMatrix(mEatProjMtx);
      SetViewMatrix(mEatViewMtx);         
        return mMameEdgeMapPath;
      
    case PATH_OUTLINE:
      if(mIsBottomScreenRender || mCore->GetPokemonAI()->IsDualScreenMonster())
			  return mOutLinePath;
      else return NULL;
		
    case PATH_OUTLINE_MAME:
      SetProjectionMatrix(mEatProjMtx);
      SetViewMatrix(mEatViewMtx);         
        return mMameOutLinePath;

    case PATH_EFFECT:
      SetProjectionMatrix(mBaseProjMtx);
      SetViewMatrix(mBaseViewMtx);       
			return mEffectRenderPath;

    case PATH_EFFECT_FIXED_SIZE:
      SetProjectionMatrix(mEffectProjMtx);
      SetViewMatrix(mEffectViewMtx);       
      return mEffectFixedSizeRenderPath;
			
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 KawaigariRenderingPipeLine::NextRenderPath(void)
{
  mRenderPathCnt++;

  u32 pokePathNum = 0;
  if( mCore->GetPokeModelSys() )
  {
    pokePathNum = mCore->GetPokeModelSys()->GetPokeShaderPathNum();
  }

  if ( mRenderPathCnt == (u32)(PATH_MAX+pokePathNum) )
  {
    return false;
  }
  return true;
}

//ポケモデルを追加
void KawaigariRenderingPipeLine::AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  if(mModelRenderPath) mModelRenderPath->AddNode(pokeModel->GetModelInstanceNode());
 
  if(mIDTextureRenderPath)
  {
      mIDTextureRenderPath->AddNode(pokeModel->GetIdModel()->GetModelInstanceNode());
      mIDTextureRenderPath->AddDrawEnv(pokeModel->GetIdModel()->GetDrawEnvNode());
  }
 /// if(mIDTextureRenderPath) mIDTextureRenderPath->AddNode(pokeModel->GetModelInstanceNode());

  if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
  
  if( pokeModel->GetShadowModel() )
  {
    mModelRenderPathShadow->AddNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
  }

  //ライトも追加
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      pEnvNodeBase->AddLightSet(lightSetNode);
      gfl2::math::Vector3 lightDir;      
      lightDir.Set(0.0f,-1.0f,-1.0f);
      pEnvNodeBase->GetLightSet(opt)->GetLightNode(0)->SetDirection(lightDir);
    }
  }
  
}
void KawaigariRenderingPipeLine::RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  if(mModelRenderPath) mModelRenderPath->RemoveNode(pokeModel->GetModelInstanceNode());

  if(mIDTextureRenderPath)
  {
    mIDTextureRenderPath->RemoveNode(pokeModel->GetIdModel()->GetModelInstanceNode());
    mIDTextureRenderPath->RemoveDrawEnv(pokeModel->GetIdModel()->GetDrawEnvNode());
  }
///  if(mIDTextureRenderPath) mIDTextureRenderPath->RemoveNode(pokeModel->GetModelInstanceNode());
  
  if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());  
  
  if( pokeModel->GetShadowModel() )
  {
    mModelRenderPathShadow->RemoveNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
  }

  //ライトも削除
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      pEnvNodeBase->RemoveLightSet(lightSetNode);
      pokeModel->GetDrawEnvNode()->AddLightSet(lightSetNode); //親が切れてるから・・・
    }
  }
  
}

//Light
void KawaigariRenderingPipeLine::AddObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt)
{
  GFL_PRINT("Adding LightSetNodes to RenderPath...\n");
 
  // Attach all lights one by one:    
  for (u32 i = 0; i < node->LightSetMax; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = node->GetLightSet(i);
    if (pLightSetNode == NULL) continue;
    GFL_PRINT("Adding LightSet Node..\n");
    if(pEnvNodeBase) pEnvNodeBase->AddLightSet(pLightSetNode);
  }
}

void KawaigariRenderingPipeLine::RemoveObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt)
{
  GFL_PRINT("Removing LightSetNodes from RenderPath...\n");
  
  // Remove all lights one by one:    
  for (u32 i = 0; i < node->LightSetMax; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = node->GetLightSet(i);
    if (pLightSetNode == NULL) continue;    
    GFL_PRINT("Removing LightSet Node..\n");
    if(pEnvNodeBase) pEnvNodeBase->RemoveLightSet(pLightSetNode);

    GFL_SAFE_DELETE(pLightSetNode);
    
  }
}

//モデルを追加
void KawaigariRenderingPipeLine::AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{	

  switch(opt)
  {
  case 0:  //通常描画
    if(mModelRenderPath) mModelRenderPath->AddNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  case 1:  //マメ
    if(mMameModelRenderPath) mMameModelRenderPath->AddNode(mdlNode);
    if(mMameEdgeMapPath) mMameEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  }
}

void KawaigariRenderingPipeLine::RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{
  switch(opt)
  {
  case 0:  //通常描画
    if(mModelRenderPath) mModelRenderPath->RemoveNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
  case 1:  //マメ
    if(mMameModelRenderPath) mMameModelRenderPath->RemoveNode(mdlNode);
    if(mMameEdgeMapPath) mMameEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break; 
  }

}

// --------------------------------------------------------------------------
/**
  * @brief  モデル描画パスへのViewProjectionMatrix書き換え関数
  * @param  projection       プロジェクションマトリックス
  * @param  view             ビューマトリックス
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SetViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{
  //KawaigariRenderingPipeLine::GetRenderPath関数にて使用されているパスが処理対象

  mEdgeMapPath->GetDrawManager()->SetProjectionMatrix( projection );
  mEdgeMapPath->GetDrawManager()->SetViewMatrix( view );
  
  mModelRenderPath->GetDrawManager()->SetProjectionMatrix( projection );
  mModelRenderPath->GetDrawManager()->SetViewMatrix( view );
  
  mModelRenderPathShadow->GetDrawManager()->SetProjectionMatrix( projection );
  mModelRenderPathShadow->GetDrawManager()->SetViewMatrix( view );
 
  {//現在描画に使われていないが、設定しておきます。
    mSkyRenderPath->GetDrawManager()->SetProjectionMatrix( projection );
    mSkyRenderPath->GetDrawManager()->SetViewMatrix( view );

    mIDTextureRenderPath->GetDrawManager()->SetProjectionMatrix( projection );
    mIDTextureRenderPath->GetDrawManager()->SetViewMatrix( view );

    mModelRenderPathAfterEdge->GetDrawManager()->SetProjectionMatrix( projection );
    mModelRenderPathAfterEdge->GetDrawManager()->SetViewMatrix( view );

    mScreenModelRenderPath->GetDrawManager()->SetProjectionMatrix( projection );
    mScreenModelRenderPath->GetDrawManager()->SetViewMatrix( view );
  }

  mBaseProjMtx = projection;
  mBaseViewMtx = view;

  //mClearDepthPathは設定不要。クリアするだけのパス。
  //mCore->GetPokeModelSys()->GetPokeShaderPath(mRenderPathCnt);//テクスチャにシェーディングをかけるパス。
}

// --------------------------------------------------------------------------
/**
  * @brief  MAMEモデル描画パスへのViewProjectionMatrix書き換え関数
  * @param  projection       プロジェクションマトリックス
  * @param  view             ビューマトリックス
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SetMameViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{
  //KawaigariRenderingPipeLine::GetRenderPath関数にて使用されているパスが処理対象 (MAME ONLY)
  
  mMameEdgeMapPath->GetDrawManager()->SetProjectionMatrix( projection );
  mMameEdgeMapPath->GetDrawManager()->SetViewMatrix( view );

  mMameModelRenderPath->GetDrawManager()->SetProjectionMatrix( projection );
  mMameModelRenderPath->GetDrawManager()->SetViewMatrix( view );

  mEatProjMtx = projection;
  mEatViewMtx = view;
 
}

// --------------------------------------------------------------------------
/**
  * @brief  EFFECTモデル描画パスへのViewProjectionMatrix書き換え関数
  * @param  projection       プロジェクションマトリックス
  * @param  view             ビューマトリックス
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SetEffectViewProjectionDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{  
  mEffectProjMtx = projection;
  mEffectViewMtx = view;  
}

// --------------------------------------------------------------------------
/**
  * @brief  天球描画パスへのViewProjectionMatrix書き換え関数(Near, Farが他のパスと異なるため別途用意)
  * @param  projection       プロジェクションマトリックス
  * @param  view             ビューマトリックス
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SetSkyMtxDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{
  //mCore->GetBgSystem()->GetFarRenderPath();//現在NULLを返すので除外。
  //mCore->GetBgSystemNear()->GetFarRenderPath();//現在NULLを返すので除外。
  mCore->GetBgSystem()->GetSkyRenderPath()->GetDrawManager()->SetProjectionMatrix( projection );
  mCore->GetBgSystemNear()->GetSkyRenderPath()->GetDrawManager()->SetProjectionMatrix( projection );
  mCore->GetBgSystem()->GetSkyRenderPath()->GetDrawManager()->SetViewMatrix( view );
  mCore->GetBgSystemNear()->GetSkyRenderPath()->GetDrawManager()->SetViewMatrix( view );
}

// --------------------------------------------------------------------------
/**
  * @brief  遠景描画パスへのViewProjectionMatrix書き換え関数(Near, Farが他のパスと異なるため別途用意)
  * @param  projection       プロジェクションマトリックス
  * @param  view             ビューマトリックス
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SetFarMtxDirect( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{
  //mCore->GetBgSystem()->GetFarRenderPath();//現在NULLを返すので除外。
  //mCore->GetBgSystemNear()->GetFarRenderPath();//現在NULLを返すので除外。
  mCore->GetBgSystem()->GetRenderPath()->GetDrawManager()->SetProjectionMatrix( projection );
  mCore->GetBgSystemNear()->GetRenderPath()->GetDrawManager()->SetProjectionMatrix( projection );
  mCore->GetBgSystem()->GetRenderPath()->GetDrawManager()->SetViewMatrix( view );
  mCore->GetBgSystemNear()->GetRenderPath()->GetDrawManager()->SetViewMatrix( view );
}

// --------------------------------------------------------------------------
/**
  * @brief  特定パスのアップデート（最小限のアップデート）
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::MinimalUpdate()
{
  //全てのパスがDrawManager経由の描画ではありません。以下３つについてはUpdate関数を直接呼びます。
  //ちなみに以下３つのパスのUpdateは基底クラスの空実装関数が呼ばれています。処理負荷はほとんどないです。
  mOutLinePath->Update( m_DrawContext );
  mEffectRenderPath->Update( m_DrawContext );
  
 // mEffectFixedSizeRenderPath->Update( m_DrawContext ); // OK??
  
  mScreenEffectRenderPath->Update( m_DrawContext );  
}

// --------------------------------------------------------------------------
/**
  * @brief  ＩＤモデル描画オフスクリーンバッファコピー先切り替え
  */
// --------------------------------------------------------------------------
void KawaigariRenderingPipeLine::SwapIDCopyBuffer()
{
  mIDTextureRenderPath->SetCopyBuffer(mOffscreenBuffer[mDBCnt]);
  mDBCnt ^= 1;
}



	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

