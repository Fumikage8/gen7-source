//======================================================================
/**
 * @file  SequenceRenderPipeLine.c++
 * @brief Sequence Rendering PipeLine
 * @author  Pete
 * @data  15/05/08
 */
//======================================================================
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>
#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_DressUpModel.h>
#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "System/include/GflUse.h"
#include <niji_conv_header/demoseq/seq_com_define.h>

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

SequenceRenderingPipeLine::SequenceRenderingPipeLine( SequenceViewSystem *seqCore , f32 screenWidth, f32 screenHeight )
:mSeqCore(seqCore)
,mUseOption()
{
  mEdgeMapPath =  mEdgeMapInframePath[0] = mEdgeMapInframePath[1] =  NULL;
  mOutLinePath = mOutLineInframePath[0] = mOutLineInframePath[1] = NULL;
  mLayoutRenderPath = NULL;
  mEffectRenderPath = NULL;
  mEffectBeforeModelRenderPath = NULL;
  mSkyRenderPath = mModelRenderBeforePath = NULL;
  mModelRenderPath2 = NULL;
  mModelRenderPath = mModelFieldRenderPath = NULL;
  mModelInframeRenderPath[0] = mModelInframeRenderPath[1] = NULL;
  mModelRenderPathAfterEdge = mModelRenderPathAfterEdge_Inframe1 =NULL;
  mScreenEffectRenderPath = NULL;
  mScreenModelRenderPath = NULL;
  mScreenBeforeModelRenderPath = NULL;
  mMotionBlurPath = NULL;
  pEnvNodeBase = NULL;
  pEnvNodeBase2 = NULL;
  pEnvNodeBaseField = NULL;
  m_pDepthStencilTexture = NULL;
  m_pDepthStencilInframeTexture[0] = NULL;
  m_pDepthStencilInframeTexture[1] = NULL;
  mStretchBltFrameBufferPath = NULL;
  mDistortionPostRenderPath = NULL;
  mDistModelCnt = 0;

  mUseInframe1 = mUseInframe2 = false;
}

SequenceRenderingPipeLine::~SequenceRenderingPipeLine()
{
}

void SequenceRenderingPipeLine::Initialize(void)
{
  mUseInframe1 = mUseOption.isUseInframe1;
  mUseInframe2 = mUseOption.isUseInframe2;

  GFL_PRINT("*SEQUENCE RENDERSYSTEM SETUP*\n");
  GFL_PRINT("*INFRAME1:%i  INFRAME2:%i\n",mUseInframe1,mUseInframe2);

	gfl2::heap::HeapBase *heap = mSeqCore->GetMemHeap();
  gfl2::gfx::IGLAllocator *allocator = mSeqCore->GetGLAllocator();
  
  mModelRenderPath = GFL_NEW(heap)System::ModelRenderPath(allocator,128);
  mModelFieldRenderPath = GFL_NEW(heap)System::ModelRenderPath(allocator,128);
  mModelRenderPath2 = GFL_NEW(heap)ModelRenderPath_NoDepthClear(allocator,128);
  mModelRenderBeforePath = GFL_NEW(heap)System::ModelRenderPath(allocator,128);

  if(mUseInframe1)
  {
    mModelInframeRenderPath[0] = GFL_NEW(heap)ModelScreenRenderPath(allocator,128);  
    mEdgeMapInframePath[0]      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,128);  
    mOutLineInframePath[0]      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
  }

  if(mUseInframe2)
  {
    mModelInframeRenderPath[1] = GFL_NEW(heap)ModelScreenRenderPath(allocator,128);
    mEdgeMapInframePath[1]      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,128);
    mOutLineInframePath[1]      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
  }

  if(mUseInframe1 || mUseInframe2)
  {
    mModelRenderPathAfterEdge_Inframe1  = GFL_NEW(heap)System::ModelRenderPath(allocator,128);
  }

  else
  {    
    mModelRenderPathAfterEdge_Inframe1 = mModelRenderPathAfterEdge;
  }

  if(mUseOption.isUseModelRenderPathAfterEdge) mModelRenderPathAfterEdge  = GFL_NEW(heap)System::ModelRenderPath(allocator,128);
  

  mScreenModelRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);
  mScreenBeforeModelRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);

	if(mUseOption.isUseSkyRenderPath)    mSkyRenderPath    = GFL_NEW(heap)System::ModelRenderPath(allocator,8);
	if(mUseOption.isUseEdgeMapPath)      mEdgeMapPath      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,128);
	if(mUseOption.isUseModelOutLinePath) mOutLinePath      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
	if(mUseOption.isUseLayoutRenderPath) mLayoutRenderPath = GFL_NEW(heap)gfl2::lyt::LayoutRenderPath(allocator);

  mEffectRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mEffectBeforeModelRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;

  if(mUseOption.isUseScreenEffectRenderPath) mScreenEffectRenderPath = GFL_NEW(heap)EffectScreenRenderPath;

  if(mUseOption.isUseMotionBlur)
  {  
    GFL_PRINT("*MotionBlur Create*\n");
    mMotionBlurPath = GFL_NEW(heap)System::MotionBlur;
  }

  // Bloom
  if(mUseOption.isUseBloomRenderPath)
  {
    poke_3d::renderer::BloomRenderPath::Description bloomDesc;
    {
      bloomDesc.sourceType                               = poke_3d::renderer::BloomRenderPath::SOURCE_TYPE_FRAME_BUFFER;
      bloomDesc.bloomType                                = poke_3d::renderer::BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3;
      bloomDesc.highLuminanceRenderTargetReciprocalScale = 2;
      bloomDesc.highLuminanceCopyNumMax                  = 1;
      bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::LEFT ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT ] );
      bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::RIGHT] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT] );
      bloomDesc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::DOWN ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN ] );
        
      mBloomRenderPath.SyncCreate(heap,heap->GetLowerHandle(),mSeqCore->GetGLAllocator(),mSeqCore->GetGLAllocator(),mSeqCore->GetAsyncFileManager(),bloomDesc);
    }
  }

  if(mUseOption.isUseWarpModel)
  {
    mStretchBltFrameBufferPath = GFL_NEW(heap)gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath(); 
    mDistortionPostRenderPath = GFL_NEW(heap)poke_3d::renderer::DistortionPostRenderPath();
  }

}

void SequenceRenderingPipeLine::Terminate(void)
{
  GFL_SAFE_DELETE(mScreenEffectRenderPath);
  GFL_SAFE_DELETE(mEffectRenderPath);
  GFL_SAFE_DELETE(mEffectBeforeModelRenderPath);
	GFL_SAFE_DELETE(mLayoutRenderPath);

	GFL_SAFE_DELETE(mOutLinePath);
  GFL_SAFE_DELETE(mOutLineInframePath[0]);
  GFL_SAFE_DELETE(mOutLineInframePath[1]);
	
  GFL_SAFE_DELETE(mEdgeMapPath);
  GFL_SAFE_DELETE(mEdgeMapInframePath[0]);
  GFL_SAFE_DELETE(mEdgeMapInframePath[1]);

	GFL_SAFE_DELETE(mSkyRenderPath);
	
  GFL_SAFE_DELETE(mModelRenderPath);
  GFL_SAFE_DELETE(mModelFieldRenderPath);
  GFL_SAFE_DELETE(mModelRenderPath2);
  GFL_SAFE_DELETE(mModelRenderBeforePath);
  GFL_SAFE_DELETE(mModelInframeRenderPath[0]);
  GFL_SAFE_DELETE(mModelInframeRenderPath[1]);

  GFL_SAFE_DELETE(mModelRenderPathAfterEdge);

  if(mUseInframe1 || mUseInframe2)
    GFL_SAFE_DELETE(mModelRenderPathAfterEdge_Inframe1);

  GFL_SAFE_DELETE(mScreenModelRenderPath);
  GFL_SAFE_DELETE(mScreenBeforeModelRenderPath);

  GFL_SAFE_DELETE(m_pDepthStencilTexture);
  GFL_SAFE_DELETE(m_pDepthStencilInframeTexture[0]);
  GFL_SAFE_DELETE(m_pDepthStencilInframeTexture[1]);

  GFL_PRINT("*Destroy Motion Blur*\n");
  GFL_SAFE_DELETE(mMotionBlurPath);

  GFL_SAFE_DELETE(mDistortionPostRenderPath);
  GFL_SAFE_DELETE(mStretchBltFrameBufferPath);
}

void SequenceRenderingPipeLine::UpdateBloom(void)
{
  if(mUseOption.isUseBloomRenderPath)
    mBloomRenderPath.Update();
}

gfl2::renderingengine::scenegraph::instance::DrawEnvNode *SequenceRenderingPipeLine::GetDrawEnvNode(int opt)
{ 
  if(opt==1) return(pEnvNodeBase2);
  else if(opt==2) return(pEnvNodeBaseField);
  return(pEnvNodeBase);
}

void SequenceRenderingPipeLine::UpdateFog(void)
{
  if(pEnvNodeBase!=NULL)
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = mScreenCamera.m_Projection;

    pEnvNodeBase->UpdateFog(param);
  }

  if(pEnvNodeBase2!=NULL)
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = mScreenCamera.m_Projection;

    pEnvNodeBase2->UpdateFog(param);
  }

  if(pEnvNodeBaseField!=NULL)
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = mScreenCamera.m_Projection;

    pEnvNodeBaseField->UpdateFog(param);
  }

}

void SequenceRenderingPipeLine::SetBloomMaskTexture(const gfl2::gfx::Texture *tex)
{
  if(mUseOption.isUseBloomRenderPath)
    mBloomRenderPath.SetAlphaMaskTexture(tex);
}

bool SequenceRenderingPipeLine::IsEnabledBloom(void)
{
  if(!mUseOption.isUseBloomRenderPath) return(false);
  if(pEnvNodeBase==NULL) return(false);
  return(pEnvNodeBase->GetGlareEnable());
  
}

void SequenceRenderingPipeLine::Setup(void *edgeRes,void *outLineRes)
{
  gfl2::gfx::IGLAllocator *allocator = mSeqCore->GetGLAllocator();
  
	if(mEdgeMapPath) mEdgeMapPath->SetResource(edgeRes);
	if(mEdgeMapPath) mEdgeMapPath->CreateEdgeMapTexture( allocator,512,256 );

  if(mUseInframe1 && mEdgeMapInframePath[0]) mEdgeMapInframePath[0]->SetResource(edgeRes);  
  if(mUseInframe1 && mEdgeMapInframePath[0]) mEdgeMapInframePath[0]->CreateEdgeMapTexture( allocator,512,256 );

  if(mUseInframe2 && mEdgeMapInframePath[1]) mEdgeMapInframePath[1]->SetResource(edgeRes);
  if(mUseInframe2 && mEdgeMapInframePath[1]) mEdgeMapInframePath[1]->CreateEdgeMapTexture( allocator,512,256 );


  if(mOutLinePath) mOutLinePath->SetResource(outLineRes);
  if(mUseInframe1 && mOutLineInframePath[0]) mOutLineInframePath[0]->SetResource(outLineRes);
  if(mUseInframe2 && mOutLineInframePath[1]) mOutLineInframePath[1]->SetResource(outLineRes);
  
	{
		// メインとなるシーン描画パスを初期化
		gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
		desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
		desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    if( mModelRenderPath ) mModelRenderPath->Initialize( allocator,desc );

    desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    if( mModelFieldRenderPath ) mModelFieldRenderPath->Initialize( allocator,desc );
    if( mModelRenderPath2 ) mModelRenderPath2->Initialize( allocator,desc );
    if( mModelRenderBeforePath ) mModelRenderBeforePath->Initialize( allocator,desc );
    if( mUseInframe1 && mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->Initialize( allocator,desc );
    if( mUseInframe2 && mModelInframeRenderPath[1]  ) mModelInframeRenderPath[1]->Initialize( allocator,desc );

    if( mModelRenderPathAfterEdge ) mModelRenderPathAfterEdge->Initialize( allocator,desc );
    if( (mUseInframe1 || mUseInframe2) && mModelRenderPathAfterEdge_Inframe1 ) mModelRenderPathAfterEdge_Inframe1->Initialize( allocator,desc );
    if( mEdgeMapPath ) mEdgeMapPath->Initialize( allocator,desc );
    if( mUseInframe1 && mEdgeMapInframePath[0] ) mEdgeMapInframePath[0]->Initialize( allocator,desc );
    if( mUseInframe2 && mEdgeMapInframePath[1] ) mEdgeMapInframePath[1]->Initialize( allocator,desc );
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

    if( mScreenModelRenderPath ) mScreenModelRenderPath->Initialize( allocator,desc );
    if( mScreenBeforeModelRenderPath ) mScreenBeforeModelRenderPath->Initialize( allocator,desc );
  }

  if( mEdgeMapPath && mOutLinePath )
  {
	  for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
	  {
  		const gfl2::gfx::Texture *tex = mEdgeMapPath->GetEdgeMapTexture( i );
		  mOutLinePath->SetEdgeMapTexture(i,tex);
	  }
  }

  if(mUseInframe1 && mEdgeMapInframePath[0] && mOutLineInframePath[0] )
  {
    for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
    {
      const gfl2::gfx::Texture *tex = mEdgeMapInframePath[0]->GetEdgeMapTexture( i );
      mOutLineInframePath[0]->SetEdgeMapTexture(i,tex);
    }
  }

  if(mUseInframe2 && mEdgeMapInframePath[1] && mOutLineInframePath[1] )
  {
    for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
    {
      const gfl2::gfx::Texture *tex = mEdgeMapInframePath[1]->GetEdgeMapTexture( i );
      mOutLineInframePath[1]->SetEdgeMapTexture(i,tex);
    }
  }
 

  if(mScreenModelRenderPath) mScreenModelRenderPath->SetCameraParam(&mScreenCamera);
  if(mScreenBeforeModelRenderPath) mScreenBeforeModelRenderPath->SetCameraParam(&mScreenCamera);
  if(mUseInframe1 && mModelInframeRenderPath[0]) mModelInframeRenderPath[0]->SetCameraParam(&inFrameCam[0]);
  if(mUseInframe2 && mModelInframeRenderPath[1]) mModelInframeRenderPath[1]->SetCameraParam(&inFrameCam[1]);


  if(mScreenEffectRenderPath) mScreenEffectRenderPath->SetCameraParam(&mEffectScreenCamera);
  
  if( mUseOption.isUseMotionBlur && mMotionBlurPath ) mMotionBlurPath->InitializeMotionBlur(mSeqCore->GetGLAllocator(), mSeqCore->GetMemHeap(), mSeqCore->GetAsyncFileManager(),gfl2::Fade::FadeManager::COLOR_BUFFER_LOWER);

#if defined(GF_PLATFORM_CTR)
  if(mOutLinePath)
  {  
    m_pDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(mSeqCore->GetGLAllocator(), gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    mOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilTexture);
  }

  if(mUseInframe1 && mOutLineInframePath[0])
  {  
    m_pDepthStencilInframeTexture[0] = gfl2::gfx::GFGL::CreateTextureFromSurface(mSeqCore->GetGLAllocator(), gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    mOutLineInframePath[0]->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilInframeTexture[0]);
  }

  if(mUseInframe2 && mOutLineInframePath[1])
  {  
    m_pDepthStencilInframeTexture[1] = gfl2::gfx::GFGL::CreateTextureFromSurface(mSeqCore->GetGLAllocator(), gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
    mOutLineInframePath[1]->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, m_pDepthStencilInframeTexture[1]);
  }
#endif

  //歪み用
  if(mUseOption.isUseWarpModel)
  {
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    mStretchBltFrameBufferPath->CreateFrameBufferTexture(allocator,pBackBuffer);

    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 4;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
    mDistortionPostRenderPath->Initialize(allocator, desc);
    mDistortionPostRenderPath->CreateBuffer(allocator, 4);
    mDistortionPostRenderPath->SetOverrideTexture( mStretchBltFrameBufferPath->GetFrameBufferTexture() );

    mDistModelCnt = 0;
  }
  
}

void SequenceRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  if(mModelRenderPath) mModelRenderPath->AddDrawEnv( pNode ); 
  if(mModelRenderBeforePath) mModelRenderBeforePath->AddDrawEnv( pNode ); 
  if(mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->AddDrawEnv( pNode ); 
  if(mModelInframeRenderPath[1] ) mModelInframeRenderPath[1] ->AddDrawEnv( pNode );   
  if(mDistortionPostRenderPath) mDistortionPostRenderPath->AddDrawEnv( pNode );
  
  if(mUseOption.isUseBloomRenderPath)
  {
    mBloomRenderPath.SetDrawEnvNode(pNode);
  }
  
  pEnvNodeBase = pNode;

}

void SequenceRenderingPipeLine::AddDrawEnv2( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  if(mModelRenderPath2) mModelRenderPath2->AddDrawEnv( pNode );    
  pEnvNodeBase2 = pNode;
}

void SequenceRenderingPipeLine::AddDrawEnvField( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  if(mModelFieldRenderPath) mModelFieldRenderPath->AddDrawEnv( pNode );    
  if(mSkyRenderPath) mSkyRenderPath->AddDrawEnv( pNode );    
  pEnvNodeBaseField = pNode;
}

void SequenceRenderingPipeLine::EnableBloom(void)
{
  if(pEnvNodeBase==NULL) return;
  pEnvNodeBase->SetGlareEnable(true);
}

void SequenceRenderingPipeLine::DisableBloom(void)
{
  if(pEnvNodeBase==NULL) return;
  pEnvNodeBase->SetGlareEnable(false);
}

void SequenceRenderingPipeLine::SetBloomParameters(const gfl2::math::Vector4 weightRGB,const f32 intensity,const f32 range,const f32 strength)
{
  if(pEnvNodeBase==NULL) return;

  pEnvNodeBase->SetGlareWeight(weightRGB);
  pEnvNodeBase->SetGlareIntensity(intensity);
  pEnvNodeBase->SetGlareRange(range);
  pEnvNodeBase->SetGlareStrength(strength);
}

void SequenceRenderingPipeLine::EnableFog(void)
{
  if(pEnvNodeBase!=NULL)  
    pEnvNodeBase->SetFogEnable(true);

  if(pEnvNodeBase2!=NULL)  
    pEnvNodeBase2->SetFogEnable(true);

  if(pEnvNodeBaseField!=NULL)  
    pEnvNodeBaseField->SetFogEnable(true);
}

void SequenceRenderingPipeLine::DisableFog(void)
{
  if(pEnvNodeBase!=NULL)
    pEnvNodeBase->SetFogEnable(false);

  if(pEnvNodeBase2!=NULL)
    pEnvNodeBase2->SetFogEnable(false);

  if(pEnvNodeBaseField!=NULL)
    pEnvNodeBaseField->SetFogEnable(false);
}

void SequenceRenderingPipeLine::SetFogParameters(const gfl2::math::Vector4 colorRGB,const f32 nearV,const f32 farV,const f32 strength)
{
  if(pEnvNodeBase!=NULL)
  {
    pEnvNodeBase->SetFogColor(colorRGB);
    pEnvNodeBase->SetFogNearLength(nearV);
    pEnvNodeBase->SetFogFarLength(farV);
    pEnvNodeBase->SetFogStrength(strength);  
  }

  if(pEnvNodeBase2!=NULL)
  {
    pEnvNodeBase2->SetFogColor(colorRGB);
    pEnvNodeBase2->SetFogNearLength(nearV);
    pEnvNodeBase2->SetFogFarLength(farV);
    pEnvNodeBase2->SetFogStrength(strength);  
  }

  if(pEnvNodeBaseField!=NULL)
  {
    pEnvNodeBaseField->SetFogColor(colorRGB);
    pEnvNodeBaseField->SetFogNearLength(nearV);
    pEnvNodeBaseField->SetFogFarLength(farV);
    pEnvNodeBaseField->SetFogStrength(strength);  
  }
}


void SequenceRenderingPipeLine::SetInframeCamera( int index, const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{ 
  if(index<0 || index>=2) return;

  if(index==0 && !mUseInframe1) return;
  if(index==1 && !mUseInframe2) return;

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

  inFrameCam[index].m_Projection = tempProj[0];
  inFrameCam[index].m_View = tempView[0];
  
#else
  inFrameCam[index].m_Projection = projection;
  inFrameCam[index].m_View = view;
#endif

  mModelInframeRenderPath[index]->SetCameraParam(&inFrameCam[index]);
    
}

void SequenceRenderingPipeLine::SetBaseCamera(const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{ 
  baseCam.m_Projection = projection;
  baseCam.m_View = view;
}

void SequenceRenderingPipeLine::GetViewAndProjectionMatrixPivot(gfl2::math::Matrix34 &view,gfl2::math::Matrix44 &proj)
{ 
#if defined(GF_PLATFORM_CTR)
  gfl2::math::Matrix44 tempProj[2];   // 透視変換行列
  gfl2::math::Matrix34 tempView[2];   // ビュー変換行列
  f32 depthLevel = 4.0f;
  f32 factor	   = 1.0f;
  gfl2::gfx::GFGL::GetStereoMatrix(	baseCam.m_Projection,
    baseCam.m_View,
    depthLevel,
    factor, 
    &tempProj[0],
    &tempView[0],
    &tempProj[1],
    &tempView[1]);

  proj = tempProj[0];
  view = tempView[0];
#else
  proj = baseCam.m_Projection; view=baseCam.m_View; 
#endif
}

void SequenceRenderingPipeLine::SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
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
  mScreenBeforeModelRenderPath->SetCameraParam(&mScreenCamera);
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 SequenceRenderingPipeLine::StartRenderPath(void)
{
  mRenderPathCnt = 0;
  return true;
}

enum
{
  PATH_SKY,
  PATH_EDGE,  
  PATH_MODEL_BEFORE,  
  PATH_MODEL_FIELD,
  PATH_MODEL,    

  PATH_EFFECT_BEFORE,  

  PATH_MODEL2,  
  
  PATH_OUTLINE,

  PATH_AFTEREDGE,

  PATH_SCREEN_MODEL_BEFORE,
  
  PATH_EDGE_INFRAME1,    
  PATH_MODEL_INFRAME1,      
  PATH_OUTLINE_INFRAME1,

  PATH_AFTEREDGE_INFRAME1,

  PATH_EDGE_INFRAME2,    
  PATH_MODEL_INFRAME2,    
  PATH_OUTLINE_INFRAME2,
  
  PATH_EFFECT,  

  PATH_COPY_SCREEN,
  PATH_DISTORTION,

  PATH_SCREEN_MODEL,
  PATH_SCREEN_EFFECT,
  
  PATH_LAYOUT,
  PATH_BLOOM,
  PATH_BLUR,
  PATH_MAX
};

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* SequenceRenderingPipeLine::GetRenderPath(void)
{
  u32 pokePathNum = 0;
  if( mSeqCore->GetPokeModelSys() )
  {
    pokePathNum = mSeqCore->GetPokeModelSys()->GetPokeShaderPathNum();

    if( mRenderPathCnt < pokePathNum )
    {
      return mSeqCore->GetPokeModelSys()->GetPokeShaderPath(mRenderPathCnt);
    }
  }

	switch( mRenderPathCnt-pokePathNum )
	{
		case PATH_SKY:          
			return mSkyRenderPath;

    case PATH_MODEL_FIELD:
      return mModelFieldRenderPath;
    case PATH_MODEL:
			return mModelRenderPath;
    case PATH_MODEL2:
      return mModelRenderPath2;

    case PATH_MODEL_BEFORE:
      return mModelRenderBeforePath;
    
    case PATH_MODEL_INFRAME1:                              
      return mModelInframeRenderPath[0];
     
    case PATH_MODEL_INFRAME2:       
      return mModelInframeRenderPath[1];
      		
    case PATH_EDGE:
		return mEdgeMapPath;
    
    case PATH_EDGE_INFRAME1:
      if(mUseInframe1 && mSeqCore->UseInframeSystem())
      {              
        SetProjectionMatrix(inFrameCam[0].m_Projection);
        SetViewMatrix(inFrameCam[0].m_View);              
        return mEdgeMapInframePath[0];        
      }
      else break;

    case PATH_EDGE_INFRAME2:
      if(mUseInframe2 && mSeqCore->UseInframeSystem())
      {   
        SetProjectionMatrix(inFrameCam[1].m_Projection);
        SetViewMatrix(inFrameCam[1].m_View);           
        return mEdgeMapInframePath[1];        
      }
      else break;
      
    case PATH_AFTEREDGE:       
      SetProjectionMatrix(baseCam.m_Projection);
      SetViewMatrix(baseCam.m_View);      
      return mModelRenderPathAfterEdge;

    case PATH_AFTEREDGE_INFRAME1:       
      if(mUseInframe1 || mUseInframe2)
      {
        SetProjectionMatrix(baseCam.m_Projection);
        SetViewMatrix(baseCam.m_View);      
        return mModelRenderPathAfterEdge_Inframe1;
      }
      else return(NULL);
		
    case PATH_OUTLINE:
			return mOutLinePath;
      
    case PATH_OUTLINE_INFRAME1:
      if(mUseInframe1 && mSeqCore->UseInframeSystem())
      {   
        SetProjectionMatrix(inFrameCam[0].m_Projection);
        SetViewMatrix(inFrameCam[0].m_View);    
        return mOutLineInframePath[0];
      }
      else break;

    case PATH_OUTLINE_INFRAME2:
      if(mUseInframe2 && mSeqCore->UseInframeSystem())
      {   
        SetProjectionMatrix(inFrameCam[1].m_Projection);
        SetViewMatrix(inFrameCam[1].m_View);            
        return mOutLineInframePath[1];
      }
      else break;
		
    case PATH_EFFECT:
      if(mUseInframe1 || mUseInframe2)
      {   
        SetProjectionMatrix(baseCam.m_Projection);
        SetViewMatrix(baseCam.m_View);         
      }
			return mEffectRenderPath;


    case PATH_EFFECT_BEFORE:
      if(mUseInframe1 || mUseInframe2)
      {   
        SetProjectionMatrix(baseCam.m_Projection);
        SetViewMatrix(baseCam.m_View);         
      }
      return mEffectBeforeModelRenderPath;

    case PATH_LAYOUT:
			return mLayoutRenderPath;
   
   case PATH_SCREEN_MODEL:
      return mScreenModelRenderPath;

   case PATH_SCREEN_MODEL_BEFORE:
     return mScreenBeforeModelRenderPath;

    case PATH_SCREEN_EFFECT:
      return mScreenEffectRenderPath;

    case PATH_BLOOM:
      if(mUseOption.isUseBloomRenderPath)
      {
        return(&mBloomRenderPath);
      }
      break;

    case PATH_BLUR:
      if(mUseOption.isUseMotionBlur)
        return mMotionBlurPath;
      else return(NULL);

    case PATH_COPY_SCREEN:
      if( mDistModelCnt > 0 )
      {
        return mStretchBltFrameBufferPath;
      }
      break;
    case PATH_DISTORTION:
      if( mDistModelCnt > 0 )
      {
        return mDistortionPostRenderPath;
      }
      break;
			
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 SequenceRenderingPipeLine::NextRenderPath(void)
{
  mRenderPathCnt++;

  u32 pokePathNum = 0;
  if( mSeqCore->GetPokeModelSys() )
  {
    pokePathNum = mSeqCore->GetPokeModelSys()->GetPokeShaderPathNum();
  }

  if ( mRenderPathCnt == (u32)(PATH_MAX+pokePathNum) )
  {
    return false;
  }
  return true;
}

//ポケモデルを追加
void SequenceRenderingPipeLine::AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  switch(opt)
  {

    case 1: // Inframe1:
    case 4:
      if(mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->AddNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapInframePath[0]) mEdgeMapInframePath[0]->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
      break;

    case 2: // Inframe2:
    case 5:
      if(mModelInframeRenderPath[1] ) mModelInframeRenderPath[1] ->AddNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapInframePath[1]) mEdgeMapInframePath[1]->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
      break;

    default:
      if(mModelRenderPath) mModelRenderPath->AddNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
      break;
  }
  
  




  if( pokeModel->GetShadowModel() )
  {

    switch(opt)
    {
    case 0:
        if(mModelRenderPath) mModelRenderPath->AddNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
        break;

    case 1: // Inframe1:    
      if(mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->AddNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
      break;

    case 2: // Inframe2:    
      if(mModelInframeRenderPath[1] ) mModelInframeRenderPath[1] ->AddNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
      break;    
    }    
  }

  //ライトも追加
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {      
      // TEMP Light Dir Setting:      
      gfl2::math::Vector3 lightDir;      
      lightDir.Set(0.0f,-1.0f,-1.0f);
      lightSetNode->GetLightNode(0)->SetDirection(lightDir);

      pEnvNodeBase->AddLightSet(lightSetNode);
      
    }
  }

}
void SequenceRenderingPipeLine::RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
    
    switch(opt)
    {
    case 1: // Inframe1:
    case 4:
      if(mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->RemoveNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapInframePath[0]) mEdgeMapInframePath[0]->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());  
      break;

    case 2: // Inframe2:
    case 5:
      if(mModelInframeRenderPath[1] ) mModelInframeRenderPath[1] ->RemoveNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapInframePath[1]) mEdgeMapInframePath[1]->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());  
      break;

    default:
      if(mModelRenderPath) mModelRenderPath->RemoveNode(pokeModel->GetModelInstanceNode());
      if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());  
      break;
    }
  

  

  if( pokeModel->GetShadowModel() )
  {

    switch(opt)
    {
    case 0:
      if(mModelRenderPath) mModelRenderPath->RemoveNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
      break;

    case 1: // Inframe1:
      if(mModelInframeRenderPath[0] ) mModelInframeRenderPath[0] ->RemoveNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
      break;

    case 2: // Inframe2:
      if(mModelInframeRenderPath[1] ) mModelInframeRenderPath[1] ->RemoveNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
      break;    
    }
    
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


//モデルを追加
void SequenceRenderingPipeLine::AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{	

  switch(opt)
  {
    
  case SEQ_DEF_DRAWTYPE_FIELD: // FIELD
    if(mModelFieldRenderPath) mModelFieldRenderPath->AddNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
    
  case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
    if(mModelRenderPath) mModelRenderPath->AddNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
    if(mModelRenderPath2) mModelRenderPath2->AddNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;


  case SEQ_DEF_DRAWTYPE_NORMAL_BEFORE:  //通常描画 BEFORE
    if(mModelRenderBeforePath) mModelRenderBeforePath->AddNode(mdlNode);
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;

  case SEQ_DEF_DRAWTYPE_NO_EDGE:  //エッジ無し
    if(mModelRenderPath) mModelRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_NO_EDGE2:  //エッジ無し
    if(mModelRenderPath2) mModelRenderPath2->AddNode(mdlNode);
    break;
  ///case SEQ_DEF_DRAWTYPE_FIELD: // FIELD
  case SEQ_DEF_DRAWTYPE_NO_EDGE_FIELD:
    if(mModelFieldRenderPath) mModelFieldRenderPath->AddNode(mdlNode);
    break;

  case SEQ_DEF_DRAWTYPE_SCREEN:  //スクリーン貼り付け
    if(mScreenModelRenderPath) mScreenModelRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTEREDGE:    
    if(mModelRenderPathAfterEdge) mModelRenderPathAfterEdge->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTEREDGE_INFRAME1:    
    if(mModelRenderPathAfterEdge_Inframe1) mModelRenderPathAfterEdge_Inframe1->AddNode(mdlNode);
    break;

  case SEQ_DEF_DRAWTYPE_SCREEN_BEFORE: // SCREEN BEFORE INFRAME
    if(mScreenBeforeModelRenderPath) mScreenBeforeModelRenderPath->AddNode(mdlNode);
    break;

  case SEQ_DEF_DRAWTYPE_WARP: //歪み
    mDistModelCnt++;
		mDistortionPostRenderPath->AddDistortionRenderingTarget( mdlNode );
    break;

  }
}

void SequenceRenderingPipeLine::RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{
  switch(opt)
  {
    case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
      if(mModelFieldRenderPath) mModelFieldRenderPath->RemoveNode(mdlNode);
      if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
    case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
      if(mModelRenderPath) mModelRenderPath->RemoveNode(mdlNode);
      if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
      break;
    case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
      if(mModelRenderPath2) mModelRenderPath2->RemoveNode(mdlNode);
      if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
      break;

    case SEQ_DEF_DRAWTYPE_NORMAL_BEFORE:  //通常描画BEFORE
      if(mModelRenderBeforePath) mModelRenderBeforePath->RemoveNode(mdlNode);
      if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
      break;

    case SEQ_DEF_DRAWTYPE_NO_EDGE:  //エッジ無し
      if(mModelRenderPath) mModelRenderPath->RemoveNode(mdlNode);
      break;
    case SEQ_DEF_DRAWTYPE_NO_EDGE2:  //エッジ無し
      if(mModelRenderPath2) mModelRenderPath2->RemoveNode(mdlNode);
      break;
    case SEQ_DEF_DRAWTYPE_NO_EDGE_FIELD:
    //case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
      if(mModelFieldRenderPath) mModelFieldRenderPath->RemoveNode(mdlNode);
      break;

    case SEQ_DEF_DRAWTYPE_SCREEN:  //スクリーン貼り付け
      if(mScreenModelRenderPath) mScreenModelRenderPath->RemoveNode(mdlNode);
      break;  
    case SEQ_DEF_DRAWTYPE_AFTEREDGE:    
      if(mModelRenderPathAfterEdge) mModelRenderPathAfterEdge->RemoveNode(mdlNode);
      break;
    case SEQ_DEF_DRAWTYPE_AFTEREDGE_INFRAME1:    
      if(mModelRenderPathAfterEdge_Inframe1) mModelRenderPathAfterEdge_Inframe1->RemoveNode(mdlNode);
      break;
    case SEQ_DEF_DRAWTYPE_SCREEN_BEFORE:  //スクリーン貼り付け INFRAME前
      if(mScreenBeforeModelRenderPath) mScreenBeforeModelRenderPath->RemoveNode(mdlNode);
      break;  

    case SEQ_DEF_DRAWTYPE_WARP: //歪み
      mDistModelCnt--;
      mDistortionPostRenderPath->RemoveDistortionRenderingTarget( mdlNode );
      break;
  }
	
}


//キャラモデルを追加
void SequenceRenderingPipeLine::AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt )
{
  switch(opt)
  {
  case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
    if(mModelFieldRenderPath) mModelFieldRenderPath->AddNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
    if(mModelRenderPath) mModelRenderPath->AddNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
    if(mModelRenderPath2) mModelRenderPath2->AddNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  default:
    if(mModelRenderPath) mModelRenderPath->AddNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());    
    break;
  }
}

void SequenceRenderingPipeLine::RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt )
{

  switch(opt)
  {
  case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
    if(mModelFieldRenderPath) mModelFieldRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
    if(mModelRenderPath) mModelRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
    if(mModelRenderPath2) mModelRenderPath2->RemoveNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;

  default:
    if(mModelRenderPath) mModelRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
    if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
    break;
  }
  
}

void SequenceRenderingPipeLine::AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      switch(opt)
      {
        case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
          mModelFieldRenderPath->AddNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pParts);
          break;
        case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
          mModelRenderPath->AddNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pParts);
          break;

          case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
            mModelRenderPath2->AddNode(pParts);
            if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pParts);
            break;

        default:
          mModelRenderPath->AddNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->AddEdgeRenderingTarget(pParts);
          break;
      }     
    }
  }
}

void SequenceRenderingPipeLine::RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      switch(opt)
      {
        case SEQ_DEF_DRAWTYPE_FIELD:  //FIELD
          mModelFieldRenderPath->RemoveNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
          break;
        case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
          mModelRenderPath->RemoveNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
          break;

        case SEQ_DEF_DRAWTYPE_NORMAL2:  //通常描画2
          mModelRenderPath2->RemoveNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
          break;

        default:
          mModelRenderPath->RemoveNode(pParts);
          if(mEdgeMapPath) mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
          break;
      }     
    }
  }
}

//Light
void SequenceRenderingPipeLine::AddObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt )
{
  GFL_PRINT("Adding LightSetNodes to RenderPath...\n");

  // Attach all lights one by one:    
  for (u32 i = 0; i < node->LightSetMax; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = node->GetLightSet(i);
    if (pLightSetNode == NULL) continue;

    GFL_PRINT("\n\n*Checking LightSet %i\n",i);

    bool ambientFound = false,directionalFound = false;

    int numLights = pLightSetNode->GetLightNum();
    for(int n=0;n<numLights;n++)
    {
      switch(pLightSetNode->GetLightNode(n)->GetLightType())
      {
      case gfl2::renderingengine::LightType::Ambient: 
        GFL_PRINT("Found Ambient Light!\n");
        ambientFound = true;
        break;
      case gfl2::renderingengine::LightType::Directional: 
        GFL_PRINT("Found Directional Light!\n");
        directionalFound = true;
        break;
      }
    }

    // Add Default Ambient?
    if(!ambientFound)
    {
      GFL_PRINT("Adding Default Ambient Light!\n");

      gfl2::renderingengine::scenegraph::instance::LightNode			*pAmbientLightNode;
      pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mSeqCore->GetGLAllocator());

      pAmbientLightNode->SetNodeName( "AmbientLight02" );
      gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()) );
      pAmbientLightNode->SetNodeNameHash( nameHash.value );
      pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
      pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 0.0f, 0.0f ) );
      pLightSetNode->AddLight( pAmbientLightNode );
    }
    if(!directionalFound)
    {
      GFL_PRINT("Adding Default Directional Light!\n");

      gfl2::renderingengine::scenegraph::instance::LightNode			*pDirectionalLightNode;
      pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mSeqCore->GetGLAllocator());

      pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
      gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()) );
      pDirectionalLightNode->SetNodeNameHash( nameHash.value );
      pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );			

      gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 0.0f, 0.0f);
      pDirectionalLightNode->SetPosition( position );
      gfl2::math::Vector3 direction = gfl2::math::Vector3(-0.390278f, -0.833886, -0.390278);      
      pDirectionalLightNode->SetDirection( direction );
      pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) );

      pLightSetNode->AddLight( pDirectionalLightNode );

    }

    GFL_PRINT("Adding LightSet Node..\n");
    if(opt==0 && pEnvNodeBase) pEnvNodeBase->AddLightSet(pLightSetNode);
    else if(opt==1 && pEnvNodeBase2) pEnvNodeBase2->AddLightSet(pLightSetNode);
    else if(opt==2 && pEnvNodeBaseField) pEnvNodeBaseField->AddLightSet(pLightSetNode);
  }
}

void SequenceRenderingPipeLine::RemoveObject_Light( gfl2::renderingengine::scenegraph::instance::DrawEnvNode *node,int opt )
{
  GFL_PRINT("Removing LightSetNodes from RenderPath...\n");
  
  // Remove all lights one by one:    
  for (u32 i = 0; i < node->LightSetMax; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode* pLightSetNode = node->GetLightSet(i);
    if (pLightSetNode == NULL) continue;    
    GFL_PRINT("Removing LightSet Node..\n");
    if(opt==0 && pEnvNodeBase) pEnvNodeBase->RemoveLightSet(pLightSetNode);
    else if(opt==1 && pEnvNodeBase2) pEnvNodeBase2->RemoveLightSet(pLightSetNode);
    else if(opt==2 && pEnvNodeBaseField) pEnvNodeBaseField->RemoveLightSet(pLightSetNode);

    GFL_SAFE_DELETE(pLightSetNode);
    
  }
}

void SequenceRenderingPipeLine::AddObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode )
{
  GFL_PRINT("*AddSkybox RenderObject\n");
  if(mSkyRenderPath) mSkyRenderPath->AddNode(mdlNode);
}

void SequenceRenderingPipeLine::RemoveObject_Skybox( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode )
{
  GFL_PRINT("*RemoveSkybox RenderObject\n");
  if(mSkyRenderPath) mSkyRenderPath->RemoveNode(mdlNode);
}




	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

