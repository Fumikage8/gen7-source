//======================================================================
/**
 * @file  btlv_RenderPipeLine.cpp
 * @brief バトル描画 RenderingPipeLine
 * @author  ariizumi
 * @data  15/03/07
 */
//======================================================================

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_StretchBltFrameBufferPath.h>
#include <renderer/include/gfl2_DistortionPostRenderPath.h>

#include <model/include/gfl2_CharaModel.h>

#include <model/include/gfl2_DressUpModel.h>
#include "System/include/GflUse.h"

#include "../btlv_core.h"
#include "../btlv_object_poke.h"
#include "btlv_RenderPipeLine.h"

#include "../btlv_seq_com_define.h"

GFL_NAMESPACE_BEGIN(btl)


BattleRenderingPipeLine::BattleRenderingPipeLine( BattleViewSystem *btlvCore , f32 screanWidth, f32 screanHeight )
:mBtlvCore(btlvCore)
{

}

BattleRenderingPipeLine::~BattleRenderingPipeLine()
{
}

void BattleRenderingPipeLine::Initialize(void)
{
  gfl2::heap::HeapBase *heap = mBtlvCore->GetResidentHeap();
  gfl2::gfx::IGLAllocator *allocator = mBtlvCore->GetGLAllocator();

  mModelRenderPath  = GFL_NEW(heap)System::ModelRenderPath(allocator,256);
  mModelBeforeRenderPath  = GFL_NEW(heap)System::ModelRenderPath(allocator,32);
  mModelAfterRenderPath  = GFL_NEW(heap)System::ModelRenderPath(allocator,32);
  mModelBallRenderPath  = GFL_NEW(heap)System::ModelRenderPath(allocator,256);
  mModelOrthoRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,8);
  mModelBgRenderPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,8);
  mModelBgAfterRenderPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,8);
  mModelAfterOrthoRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,8);
  mFloorMaskPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,1);
  mShadowRenderPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,8);
  mScreenModelRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);
  mScreenModelAfterLayoutRenderPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);
  mScreenModelBeforeRenderPath = GFL_NEW(heap)ModelScreenRenderPath(allocator,4);
  // mSkyRenderPath    = GFL_NEW(heap)System::ModelRenderPath(allocator,4);
  mEdgeMapPath      = GFL_NEW(heap)System::NijiEdgeMapSceneRenderPath(allocator,256);
  mOutLinePath      = GFL_NEW(heap)System::NijiOutLinePostSceneRenderPath(allocator);
  mLayoutRenderPath = GFL_NEW(heap)gfl2::lyt::LayoutRenderPath(allocator);
  mEffectRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mEffectLowRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mEffectHighRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mEffectBeforeRenderPath = GFL_NEW(heap)gfl2::Effect::EffectRenderPath;
  mScreenEffectRenderPath = GFL_NEW(heap)EffectScreenRenderPath;
  mFillRenderPath  = GFL_NEW(heap)ModelScreenRenderPath(allocator,1);
  mStretchBltFrameBufferPath = GFL_NEW(heap)gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath(); 
  mStretchBltPokeFrameBufferPath = GFL_NEW(heap)gfl2::renderingengine::renderer::util::StretchBltFrameBufferPath(); 
  mDistortionPostRenderPath = GFL_NEW(heap)poke_3d::renderer::DistortionPostRenderPath();
  mDistortionPokePostRenderPath = GFL_NEW(heap)poke_3d::renderer::DistortionPostRenderPath();

  //mFillRenderPath   = GFL_NEW(heap)System::FillRenderPath();
#if USE_AURA_SHADER
  //こいつらのアロケーターは演出中に取られるのでTempにしておく
  mFullPowerEffectRenderPath_Rim = GFL_NEW(heap)FullPowerEffectRenderPath0(mBtlvCore->GetFullPowerGLAllocator());
  mFullPowerEffectRenderPath_Aura = GFL_NEW(heap)FullPowerEffectRenderPath1(mBtlvCore->GetFullPowerGLAllocator());
#endif //USE_AURA_SHADER

  mFPDeleteCnt = 0;
  mFPDeleteReserve = NULL;

  mIsDraw = false;
  mIsAfterDraw = false;
}

void BattleRenderingPipeLine::Terminate(void)
{
  if( mFPDeleteReserve )
  {
    DeletePoke_FullPowerShader( mFPDeleteReserve );
    mFPDeleteReserve = NULL;
  }
  GFL_SAFE_DELETE(mDepthStencilTexture);
#if USE_AURA_SHADER
  GFL_SAFE_DELETE(mFullPowerEffectRenderPath_Rim);
  GFL_SAFE_DELETE(mFullPowerEffectRenderPath_Aura);
#endif //USE_AURA_SHADER
  GFL_SAFE_DELETE(mDistortionPostRenderPath);
  GFL_SAFE_DELETE(mDistortionPokePostRenderPath);
  GFL_SAFE_DELETE(mStretchBltFrameBufferPath);
  GFL_SAFE_DELETE(mStretchBltPokeFrameBufferPath);
  //GFL_SAFE_DELETE(mFillRenderPath);
  GFL_SAFE_DELETE(mFillRenderPath);
  GFL_SAFE_DELETE(mScreenEffectRenderPath);
  GFL_SAFE_DELETE(mEffectRenderPath);
  GFL_SAFE_DELETE(mEffectLowRenderPath);
  GFL_SAFE_DELETE(mEffectHighRenderPath);
  GFL_SAFE_DELETE(mEffectBeforeRenderPath);
  GFL_SAFE_DELETE(mLayoutRenderPath);
  GFL_SAFE_DELETE(mOutLinePath);
  GFL_SAFE_DELETE(mEdgeMapPath);
//  GFL_SAFE_DELETE(mSkyRenderPath);
  GFL_SAFE_DELETE(mShadowRenderPath);
  GFL_SAFE_DELETE(mFloorMaskPath);
  GFL_SAFE_DELETE(mModelRenderPath);
  GFL_SAFE_DELETE(mModelBeforeRenderPath);
  GFL_SAFE_DELETE(mModelAfterOrthoRenderPath);
  GFL_SAFE_DELETE(mModelBallRenderPath);
  GFL_SAFE_DELETE(mModelOrthoRenderPath);
  GFL_SAFE_DELETE(mModelAfterRenderPath);
  GFL_SAFE_DELETE(mModelBgAfterRenderPath);
  GFL_SAFE_DELETE(mModelBgRenderPath);
  GFL_SAFE_DELETE(mScreenModelBeforeRenderPath);
  GFL_SAFE_DELETE(mScreenModelAfterLayoutRenderPath);
  GFL_SAFE_DELETE(mScreenModelRenderPath);

}

void BattleRenderingPipeLine::Setup(void *edgeRes,void *outLineRes,void *FullPowerRes)
{
  gfl2::gfx::IGLAllocator *allocator = mBtlvCore->GetGLAllocator();

  mEdgeMapPath->SetResource(edgeRes);
  mEdgeMapPath->CreateEdgeMapTexture(allocator, 512,256 );
  mOutLinePath->SetResource(outLineRes);

  {
    // メインとなるシーン描画パスを初期化
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 256;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 32;

    mModelRenderPath->Initialize( allocator,desc );
    mEdgeMapPath->Initialize( allocator,desc );
#if USE_AURA_SHADER
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 16;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 32;
    mFullPowerEffectRenderPath_Rim->Initialize( mBtlvCore->GetFullPowerGLAllocator(),desc );
    mFullPowerEffectRenderPath_Rim->SetResource( mBtlvCore->GetFullPowerGLAllocator(),FullPowerRes );
    mFullPowerEffectRenderPath_Aura->Initialize( mBtlvCore->GetFullPowerGLAllocator(),desc );
    mFullPowerEffectRenderPath_Aura->SetResource( mBtlvCore->GetFullPowerGLAllocator(),FullPowerRes );
#endif //USE_AURA_SHADER

    desc.m_DrawManagerDesc.m_NodeStackSize	  = 32;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
    mModelBeforeRenderPath->Initialize( allocator,desc );
    mModelAfterRenderPath->Initialize( allocator,desc );
    mModelBallRenderPath->Initialize( allocator,desc );
    mModelOrthoRenderPath->Initialize( allocator,desc );
    mModelAfterOrthoRenderPath->Initialize( allocator,desc );
    mModelBgRenderPath->Initialize( allocator,desc );
    mModelBgAfterRenderPath->Initialize( allocator,desc );

    desc.m_DrawManagerDesc.m_NodeStackSize	  = 8;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 32;
    mShadowRenderPath->Initialize( allocator,desc );

    desc.m_DrawManagerDesc.m_NodeStackSize	  = 1;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
    mFloorMaskPath->Initialize( allocator,desc );
  }
  {
    // スクリーン貼り付けシーン描画パスを初期化
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 16;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;

    mScreenModelRenderPath->Initialize( allocator,desc );
    mScreenModelAfterLayoutRenderPath->Initialize( allocator,desc );
    mScreenModelBeforeRenderPath->Initialize( allocator,desc );
  }
  mScreenModelRenderPath->SetCameraParam(&mScreenCamera);
  mScreenModelAfterLayoutRenderPath->SetCameraParam(&mScreenCamera);
  mScreenModelBeforeRenderPath->SetCameraParam(&mScreenCamera);
  mScreenEffectRenderPath->SetCameraParam(&mEffectScreenCamera);

  {
    //正射影カメラ
    //スクリーン貼り付けカメラ
    gfl2::math::Matrix44 projMtx;   // 透視変換行列
    gfl2::math::Matrix34 viewMtx;   // ビュー変換行列
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );

    // 射影変換行列(パースペクティブ(透視法)射影)
	  mOrthoCamera.m_Projection = gfl2::math::Matrix44::GetOrthographic(-200.0f,200.0f,-120.0f,120.0f,32.0f,512.0f,true);

    // ビュー変換行列
	  gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, 0.0f );  // 視点(カメラの位置)
	  gfl2::math::Vector3 focusPosition( 0.0f, 0.0f,-200.0f );  // 注視点
	  gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
	  mOrthoCamera.m_View = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    mModelOrthoRenderPath->SetCameraParam(&mOrthoCamera);
    mModelAfterOrthoRenderPath->SetCameraParam(&mOrthoCamera);
  }
  {
    // メインとなるシーン描画パスを初期化
   // gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
   // desc.m_DrawManagerDesc.m_NodeStackSize	  = 4;
   // desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;

  //  mSkyRenderPath->Initialize( desc );
  }

  
  for( int i=0;i<poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Max;i++ )
  {
    const gfl2::gfx::Texture *tex = mEdgeMapPath->GetEdgeMapTexture( i );
    mOutLinePath->SetEdgeMapTexture(i,tex);
  }

#if defined(GF_PLATFORM_CTR)
  mDepthStencilTexture = gfl2::gfx::GFGL::CreateTextureFromSurface(allocator, gfl2::gfx::GFGL::GetDepthStencilBuffer(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]));
  mOutLinePath->SetEdgeMapTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, mDepthStencilTexture);
#else
  mDepthStencilTexture = NULL;
#endif

  {
    // 背景塗りつぶし描画パスを初期化
    gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
    desc.m_DrawManagerDesc.m_NodeStackSize	  = 1;
    desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 4;
    mFillRenderPath->Initialize( allocator,desc );

    //スクリーン貼り付けカメラ
    gfl2::math::Matrix44 projMtx;   // 透視変換行列
    gfl2::math::Matrix34 viewMtx;   // ビュー変換行列
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );

    // 射影変換行列(パースペクティブ(透視法)射影)
	  mBgFillCamera.m_Projection = gfl2::math::Matrix44::GetPerspective(
			  gfl2::math::ConvDegToRad(30.0f),		// 視野角30°
        (f32)displayWidth / (f32)displayHeight,
			  1.0f,							// 前方投影面までの距離
			  1024.0f);							// 後方投影面までの距離

    // ビュー変換行列
	  gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, 0.0f );  // 視点(カメラの位置)
	  gfl2::math::Vector3 focusPosition( 0.0f, 0.0f, 200.0f );  // 注視点
	  gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
	  mBgFillCamera.m_View = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    mFillRenderPath->SetCameraParam(&mBgFillCamera);
  }

  //歪み用
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

    mStretchBltPokeFrameBufferPath->CreateFrameBufferTexture(allocator,pBackBuffer);
    mDistortionPokePostRenderPath->Initialize(allocator, desc);
    mDistortionPokePostRenderPath->CreateBuffer(allocator, 4);
    mDistortionPokePostRenderPath->SetOverrideTexture( mStretchBltPokeFrameBufferPath->GetFrameBufferTexture() );
    mDistPokeModelCnt = 0;
  }
}

void BattleRenderingPipeLine::Update(const gfl2::math::Matrix34 &viewMtx )
{
  gfl2::renderingengine::renderer::RenderingPipeLine::Update();
#if USE_AURA_SHADER
  nn::math::VEC3 axisY = nn::math::VEC3( 0,1,0 );

  gfl2::math::Vector3 rot = viewMtx.GetElemRotationZYX_fix();

  nn::math::MTX34 mtx;
  nn::math::MTX34RotAxisRad( &mtx, &axisY, -rot.y );
 
  nn::math::VEC4 r0 = mtx.GetRow(0);
  nn::math::VEC4 r1 = mtx.GetRow(1);
  nn::math::VEC4 r2 = mtx.GetRow(2);

  gfl2::math::Vector4 r0_1 = gfl2::math::Vector4( r0.x, r0.y, r0.z, r0.w );
  gfl2::math::Vector4 r1_1 = gfl2::math::Vector4( r1.x, r1.y, r1.z, r1.w );
  gfl2::math::Vector4 r2_1 = gfl2::math::Vector4( r2.x, r2.y, r2.z, r2.w );

  gfl2::math::Matrix34 outMtx;
  outMtx = gfl2::math::Matrix34( r0_1, r1_1, r2_1 );

  //mFullPowerEffectRenderPath_Aura->SetYBillboardMtx( &outMtx );
#endif //USE_AURA_SHADER
  if( mFPDeleteCnt > 0 )
  {
    mFPDeleteCnt--;
    if( mFPDeleteCnt <= 0 )
    {
      DeletePoke_FullPowerShader( mFPDeleteReserve );
      mFPDeleteReserve = NULL;
    }
  }
}

void BattleRenderingPipeLine::AddDrawEnv( gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode )
{ 
  mModelRenderPath->AddDrawEnv( pNode ); 
  mShadowRenderPath->AddDrawEnv( pNode ); 
  mModelBallRenderPath->AddDrawEnv( pNode ); 
  mModelBeforeRenderPath->AddDrawEnv( pNode ); 
  mModelAfterRenderPath->AddDrawEnv( pNode ); 
  mModelOrthoRenderPath->AddDrawEnv( pNode ); 
  mModelAfterOrthoRenderPath->AddDrawEnv( pNode ); 
  mModelBgRenderPath->AddDrawEnv( pNode ); 
  mModelBgAfterRenderPath->AddDrawEnv( pNode ); 
	mDistortionPostRenderPath->AddDrawEnv( pNode );
  mDistortionPokePostRenderPath->AddDrawEnv( pNode );
}


void BattleRenderingPipeLine::SetScreenCameraParam( const gfl2::math::Matrix44	&projection , const gfl2::math::Matrix34 &view)
{ 
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
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 BattleRenderingPipeLine::StartRenderPath(void)
{
  mRenderPathCnt = 0;
  return true;
}

enum
{
  PATH_SKY,
  PATH_BG_FAR,
  PATH_BG,
  PATH_BG_MODEL,
  PATH_FILL,
  PATH_BG_MODEL_AFTER,
  PATH_SHADOW,
  PATH_FLOOR_MASK,
  PATH_BEFORE_SCREEN,
  PATH_BEFORE_MODEL,
  PATH_BEFORE_EFFECT,
  PATH_EDGE,
  PATH_MAIN,
  PATH_MONSTER_BALL,  //エッジのため後描き
  PATH_FP_RIM,
  PATH_FP_AURA,
  PATH_OUTLINE,
  PATH_COPY_SCREEN_POKE,
  PATH_DISTORITON_POKE,
  PATH_ORTHO_MODEL,
  PATH_AFTER_MODEL,
  PATH_EFFECT_LOW,
  PATH_EFFECT,
  PATH_EFFECT_HIGH,
  PATH_COPY_SCREEN,
  PATH_DISTORITON,
  PATH_SCREEN_MODEL,
  PATH_SCREEN_EFFECT,
  PATH_AFTER_ORTHO_MODEL,
  PATH_LAYOUT,
  PATH_SCREEN_MODEL_AFTER_LAYOUT,
  PATH_MAX,
};
//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* BattleRenderingPipeLine::GetRenderPath(void)
{
  //分割描画の２回目はこれのみ
  if( mIsDraw == true && mIsAfterDraw == true )
  {
    return mScreenModelAfterLayoutRenderPath;
  }

  const int pokePathNum = mBtlvCore->GetPokeModelSys()->GetPokeShaderPathNum();

  if( mRenderPathCnt < pokePathNum )
  {
    return mBtlvCore->GetPokeModelSys()->GetPokeShaderPath(mRenderPathCnt);
  }
#if PM_DEBUG
  if( mBtlvCore->mDebugRenderPathCheck != -1 &&
      mBtlvCore->mDebugRenderPathCheck != mRenderPathCnt-pokePathNum )
  {
    return NULL;
  }
#endif
  switch( mRenderPathCnt-pokePathNum )
  {
  case PATH_SKY:
    return mBtlvCore->GetBgSystem()->GetSkyRenderPath();
    //    return mSkyRenderPath;
    //break;
  case PATH_BG_FAR:
    return mBtlvCore->GetBgSystem()->GetFarRenderPath();
  case PATH_BG:
    return mBtlvCore->GetBgSystem()->GetRenderPath();
    //break;
  case PATH_BG_MODEL:
    return mModelBgRenderPath;
  case PATH_SHADOW:
    return mShadowRenderPath;
  case PATH_FLOOR_MASK:
    return mFloorMaskPath;
    //break;
  case PATH_BG_MODEL_AFTER:
    return mModelBgAfterRenderPath;
  case PATH_FILL:
    return mFillRenderPath;
    //break;
  case PATH_EDGE:
    return mEdgeMapPath;
    //break;
  case PATH_BEFORE_SCREEN:
    return mScreenModelBeforeRenderPath;
    //break;
  case PATH_BEFORE_MODEL:
    return mModelBeforeRenderPath;
    //break;
  case PATH_BEFORE_EFFECT:
    return mEffectBeforeRenderPath;
    //break;
  case PATH_MAIN: 
    return mModelRenderPath;
    //break;
  case PATH_MONSTER_BALL: //エッジのため後描き
    return mModelBallRenderPath;
    //break;
#if USE_AURA_SHADER
  case PATH_FP_RIM:
    return mFullPowerEffectRenderPath_Rim;
    //break;
  case PATH_FP_AURA:
    return mFullPowerEffectRenderPath_Aura;
    //break;
#endif //USE_AURA_SHADER

  case PATH_OUTLINE:
    return mOutLinePath;
    //break;
  case PATH_COPY_SCREEN_POKE:
    if( mDistPokeModelCnt > 0 )
    {
      return mStretchBltPokeFrameBufferPath;
    }
    break;
  case PATH_DISTORITON_POKE:
    if( mDistPokeModelCnt > 0 )
    {
      return mDistortionPokePostRenderPath;
    }
    break;
  case PATH_ORTHO_MODEL:
    return mModelOrthoRenderPath;
    //break;
  case PATH_AFTER_MODEL:
    return mModelAfterRenderPath;
    //break;
  case PATH_EFFECT_LOW:
    return mEffectLowRenderPath;
    //break;
  case PATH_EFFECT:
    return mEffectRenderPath;
    //break;
  case PATH_EFFECT_HIGH:
    return mEffectHighRenderPath;
    //break;
  case PATH_COPY_SCREEN:
    if( mDistModelCnt > 0 )
    {
      return mStretchBltFrameBufferPath;
    }
    break;
  case PATH_DISTORITON:
    if( mDistModelCnt > 0 )
    {
      return mDistortionPostRenderPath;
    }
    break;
  case PATH_AFTER_ORTHO_MODEL:
    return mModelAfterOrthoRenderPath;
    //break;
  case PATH_LAYOUT:
    return mLayoutRenderPath;
    //break;
  case PATH_SCREEN_MODEL:
    return mScreenModelRenderPath;
    //break;
  case PATH_SCREEN_EFFECT:
    return mScreenEffectRenderPath;
    //break;
  case PATH_SCREEN_MODEL_AFTER_LAYOUT:
    if( mIsDraw == false )
    {
      return mScreenModelAfterLayoutRenderPath;
    }
    //break;
  }
  return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 BattleRenderingPipeLine::NextRenderPath(void)
{
  mRenderPathCnt++;

  //分割描画の２回目はPathは１個のみ
  if( mIsDraw == true && mIsAfterDraw == true )
  {
    return false;
  }

  const int pokePathNum = mBtlvCore->GetPokeModelSys()->GetPokeShaderPathNum();
  if ( mRenderPathCnt == PATH_MAX+pokePathNum )
  {
    return false;
  }
  return true;
}

//エッジだけ制御
void BattleRenderingPipeLine::AddEdge_Poke( PokeTool::PokeModel *pokeModel )
{
  mEdgeMapPath->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
}
void BattleRenderingPipeLine::RemoveEdge_Poke( PokeTool::PokeModel *pokeModel )
{
  mEdgeMapPath->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());
}
void BattleRenderingPipeLine::AddEdge_Chara( poke_3d::model::CharaModel *charaModel )
{
  mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
}
void BattleRenderingPipeLine::RemoveEdge_Chara( poke_3d::model::CharaModel *charaModel )
{
  mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
}

void BattleRenderingPipeLine::AddEdge_DressUpChara( poke_3d::model::DressUpModel *dressModel )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからADD
      // 生成されていないパーツはNULL
      mEdgeMapPath->AddEdgeRenderingTarget(pParts);
    }
  }
}

void BattleRenderingPipeLine::RemoveEdge_DressUpChara( poke_3d::model::DressUpModel *dressModel )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからADD
      // 生成されていないパーツはNULL
      mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
    }
  }
}


//モデルを追加
void BattleRenderingPipeLine::AddObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{
  switch(opt)
  {
  case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
    mModelRenderPath->AddNode(mdlNode);
    mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_NO_EDGE:  //エッジ無し
    {
      //ポケモンに負けないようにプライオリティをあげておく
      static const u8 defaultPriority = gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::s_DefaultPriority;
      mdlNode->SetSortPriority(defaultPriority + 32);
    }
    mModelRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_SCREEN:  //スクリーン貼り付け
    mScreenModelRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BEFORE:  //前描画
    mModelBeforeRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTER:  //後描画
    mModelAfterRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_ORTHO:  //正射影
    mModelOrthoRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTER_ORTHO:  //正射影
    mModelAfterOrthoRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BALL:
    mModelBallRenderPath->AddNode(mdlNode);
    mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    mdlNode->SetSortPriority(192);  //エッジのプライオリティもいじる。
    break;
  case SEQ_DEF_DRAWTYPE_BG: //背景
    mModelBgRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTER_LYT:  //スクリーン貼り付け(レイアウト後
    mScreenModelAfterLayoutRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_DISTORTION: //歪み
    mDistModelCnt++;
		mDistortionPostRenderPath->AddDistortionRenderingTarget( mdlNode );
    break;
  case SEQ_DEF_DRAWTYPE_SCREEN_BEFORE://スクリーン貼り付け(前描画)
    mScreenModelBeforeRenderPath->AddNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BEFORE_EDGE:
    mModelBeforeRenderPath->AddNode(mdlNode);
    mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BG_AFTER: //背景(後
    mModelBgAfterRenderPath->AddNode(mdlNode);
    break;

  case RENDER_OPT_SHADOW_MODEL: //影用(サイズぎりぎりなので注意
    mShadowRenderPath->AddNode(mdlNode);
    break;
  case RENDER_OPT_EDGE_MASK:  //地面のエッジマスク
    mEdgeMapPath->AddEdgeRenderingTarget(mdlNode);
    break;
  case RENDER_OPT_FLOOR_MASK: //地面のモデルマスク
    mFloorMaskPath->AddNode(mdlNode);
    break;
  case RENDER_OPT_BGFILL://背景塗りつぶし
    mFillRenderPath->AddNode(mdlNode);
    break;
  case RENDER_OPT_SKYBOX://仮用空
    GFL_ASSERT(0);
    //mSkyRenderPath->AddNode(mdlNode);
    break;
  }
}

void BattleRenderingPipeLine::RemoveObject_GfMdl( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mdlNode , int opt )
{
  switch(opt)
  {
  case SEQ_DEF_DRAWTYPE_NORAML:  //通常描画
    mModelRenderPath->RemoveNode(mdlNode);
    mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_NO_EDGE:  //エッジ無し
    mModelRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_SCREEN:  //スクリーン貼り付け
    mScreenModelRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BEFORE:  //前描画
    mModelBeforeRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTER:  //後描画
    mModelAfterRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_ORTHO:  //正射影
    mModelOrthoRenderPath->RemoveNode(mdlNode);
    break;;
  case SEQ_DEF_DRAWTYPE_AFTER_ORTHO:  //正射影
    mModelAfterOrthoRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BALL:
    mModelBallRenderPath->RemoveNode(mdlNode);
    mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BG: //背景
    mModelBgRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_AFTER_LYT:  //スクリーン貼り付け(レイアウト後
    mScreenModelAfterLayoutRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_DISTORTION: //歪み
    mDistModelCnt--;
		mDistortionPostRenderPath->RemoveDistortionRenderingTarget( mdlNode );
    break;
  case SEQ_DEF_DRAWTYPE_SCREEN_BEFORE://スクリーン貼り付け(前描画)
    mScreenModelBeforeRenderPath->RemoveNode(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BEFORE_EDGE:
    mModelBeforeRenderPath->RemoveNode(mdlNode);
    mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
  case SEQ_DEF_DRAWTYPE_BG_AFTER: //背景(後
    mModelBgAfterRenderPath->RemoveNode(mdlNode);
    break;


  case RENDER_OPT_SHADOW_MODEL: //影用(サイズぎりぎりなので注意
    mShadowRenderPath->RemoveNode(mdlNode);
    break;
  case RENDER_OPT_EDGE_MASK:  //地面のエッジマスク
    mEdgeMapPath->RemoveEdgeRenderingTarget(mdlNode);
    break;
  case RENDER_OPT_FLOOR_MASK: //地面のモデルマスク
    mFloorMaskPath->RemoveNode(mdlNode);
    break;
  case RENDER_OPT_BGFILL://背景塗りつぶし
    mFillRenderPath->RemoveNode(mdlNode);
    break;
  case RENDER_OPT_SKYBOX://仮用空
    GFL_ASSERT(0);
//    mSkyRenderPath->RemoveNode(mdlNode);
    break;
  }
}

//全力シェーダー用
void BattleRenderingPipeLine::AddPoke_FullPowerShader( BtlvObject_PokeModel *pokeModel , bool isZenryoku)
{
#if USE_AURA_SHADER
  const PokeExtraInfoStruct *data = pokeModel->GetPokeInfoData();

  // 全力技エフェクト(Rim+Aura)の外部設定
  FullPowerEffectRimMdlManager::mdlStatusSetting rimStatus;
  FullPowerEffectAuraMdlManager::mdlStatusSetting auraStatus;

#if PM_DEBUG & 0
  if( mBtlvCore->mDebugFPEnableSetting )
  {
    rimStatus.rimColorR = mBtlvCore->mDebugFPRimColorR;//0.0f;
    rimStatus.rimColorG = mBtlvCore->mDebugFPRimColorG;
    rimStatus.rimColorB = mBtlvCore->mDebugFPRimColorB;//0.5f;
    rimStatus.rimPow = mBtlvCore->mDebugFPRimPow;//4.0f;
    rimStatus.rimWidth = mBtlvCore->mDebugFPRimWidth;//0.4f;
    rimStatus.rimMoveSpeed = mBtlvCore->mDebugFPRimMoveSpeed;//0x400;
    rimStatus.rimMoveWidth = mBtlvCore->mDebugFPRimMoveWidth;

    auraStatus.auraColorR = mBtlvCore->mDebugFPAuraColorR;
    auraStatus.auraColorG = mBtlvCore->mDebugFPAuraColorG;
    auraStatus.auraColorB = mBtlvCore->mDebugFPAuraColorB;
    auraStatus.auraScaleX = mBtlvCore->mDebugFPAuraScaleX*mBtlvCore->mDebugFPAuraBaseScaleX;
    auraStatus.auraScaleY = mBtlvCore->mDebugFPAuraScaleY*mBtlvCore->mDebugFPAuraBaseScaleY;
    auraStatus.density = mBtlvCore->mDebugFPAuraDensity;
  }
  else
#endif //PM_DEBUG
  if( data )
  {
    rimStatus.rimColorR = data->mRimR;
    rimStatus.rimColorG = data->mRimG;
    rimStatus.rimColorB = data->mRimB;
    rimStatus.rimPow = data->mRimPow;
    rimStatus.rimWidth = data->mRimWidth;
    rimStatus.rimMoveSpeed = data->mRimSpeed;
    rimStatus.rimMoveWidth = data->mRimMove;

    auraStatus.auraColorR = data->mAuraR;
    auraStatus.auraColorG = data->mAuraG;
    auraStatus.auraColorB = data->mAuraB;
    auraStatus.auraScaleX = data->mAuraX*1.2f;
    auraStatus.auraScaleY = data->mAuraY*1.0f;
    auraStatus.density = data->mAuraDensity;
  }
  else
  {
    rimStatus.rimColorR = 1.0f;
    rimStatus.rimColorG = 1.0f;
    rimStatus.rimColorB = 0.4f;
    rimStatus.rimPow = 4.0f;
    rimStatus.rimWidth = 0.4f;
    rimStatus.rimMoveSpeed = 2048;
    rimStatus.rimMoveWidth = 0.2f;

    auraStatus.auraColorR = 1.0f;
    auraStatus.auraColorG = 1.0f;
    auraStatus.auraColorB = 0.4f;
    auraStatus.auraScaleX = 1.0f*1.2f;
    auraStatus.auraScaleY = 1.0f*1.0f;
    auraStatus.density = 9024;
  }

  if( isZenryoku == false )
  {
    //全力技ではないので外部からのパラメータを参照
    if( mBtlvCore->IsBeastBattle() )
    {
      //代表ビースト戦は固定指定
      rimStatus.rimColorR = 1.0f;
      rimStatus.rimColorG = 0.6f;
      rimStatus.rimColorB = 0.1f;
      auraStatus.auraColorR = 1.0f;
      auraStatus.auraColorG = 0.6f;
      auraStatus.auraColorB = 0.1f;
    }
    else
    {
      const PartyParam *param = mBtlvCore->GetPartySetupParam( mBtlvCore->GetEnemyClientID(0) );
      if( param )
      {
        ARIIZUMI_PRINT("AuraColor [%f:%f:%f:]\n",param->defaultPowerUpDesc->aura_color.x,param->defaultPowerUpDesc->aura_color.y,param->defaultPowerUpDesc->aura_color.z);
        //0-255で来る
        auraStatus.auraColorR = param->defaultPowerUpDesc->aura_color.x/255.0f;
        auraStatus.auraColorG = param->defaultPowerUpDesc->aura_color.y/255.0f;
        auraStatus.auraColorB = param->defaultPowerUpDesc->aura_color.z/255.0f;
        rimStatus.rimColorR = param->defaultPowerUpDesc->aura_color.x/255.0f;
        rimStatus.rimColorG = param->defaultPowerUpDesc->aura_color.y/255.0f;
        rimStatus.rimColorB = param->defaultPowerUpDesc->aura_color.z/255.0f;
      }
    }
  }

  //マテリアル例外設定
  int dataSize;
  const void *disableMaterialBuf = pokeModel->GetPokeModel()->GetFullPowerDisableMaterialData(&dataSize);
  if( disableMaterialBuf )
  {
    const u8 *disableMaterialData = static_cast<const u8*>(disableMaterialBuf);
    //サイズが128でパディングされているのであまりあてにならない
    u32 dataNum = dataSize/(PokeTool::POKE_MATNAME_LEN);  

    //マテリアルの書き換え処理
    for( u32 datIdx = 0 ;datIdx < dataNum ; datIdx++ )
    {
      const u8* matName = &disableMaterialData[datIdx*(PokeTool::POKE_MATNAME_LEN)];
      if( matName[0] != '\0' )
      {
        gfl2::util::FnvHash	hash( matName );
        rimStatus.m_disableMaterialInfo.materialHashList[rimStatus.m_disableMaterialInfo.targetNum] = hash.value;
        rimStatus.m_disableMaterialInfo.targetNum++;
        auraStatus.m_disableMaterialInfo.materialHashList[auraStatus.m_disableMaterialInfo.targetNum] = hash.value;
        auraStatus.m_disableMaterialInfo.targetNum++;
      }
    }
  }

  mFullPowerEffectRenderPath_Rim->AddRenderingTarget(mBtlvCore->GetFullPowerHeap(), pokeModel->GetPokeModel()->GetModelInstanceNode(),&rimStatus);
  mFullPowerEffectRenderPath_Aura->AddRenderingTarget(mBtlvCore->GetFullPowerHeap(),pokeModel->GetPokeModel()->GetModelInstanceNode(),&auraStatus);
#endif
}

void BattleRenderingPipeLine::RemovePoke_FullPowerShader( BtlvObject_PokeModel *pokeModel )
{
#if USE_AURA_SHADER
  mFullPowerEffectRenderPath_Rim->RemoveRenderingTarget(pokeModel->GetPokeModel()->GetModelInstanceNode());
  mFullPowerEffectRenderPath_Aura->RemoveRenderingTarget(pokeModel->GetPokeModel()->GetModelInstanceNode());
#endif
  mFPDeleteCnt = 2;
  mFPDeleteReserve = pokeModel;
}

void BattleRenderingPipeLine::DeletePoke_FullPowerShader( BtlvObject_PokeModel *pokeModel )
{
#if USE_AURA_SHADER
  mFullPowerEffectRenderPath_Rim->DeleteManagedModel(pokeModel->GetPokeModel()->GetModelInstanceNode());
  mFullPowerEffectRenderPath_Aura->DeleteManagedModel(pokeModel->GetPokeModel()->GetModelInstanceNode());
#endif
}


//ポケモデルを追加
void BattleRenderingPipeLine::AddObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  mModelRenderPath->AddNode(pokeModel->GetModelInstanceNode());
  mEdgeMapPath->AddEdgeRenderingTarget(pokeModel->GetModelInstanceNode());

  if( pokeModel->GetShadowModel() )
  {
    mShadowRenderPath->AddNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
  }
  if( pokeModel->GetDistortionModel() )
  {
    mDistPokeModelCnt++;
    mDistortionPokePostRenderPath->AddDistortionRenderingTarget( pokeModel->GetDistortionModel()->GetModelInstanceNode() );
  }

  //ライトも追加
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      mBtlvCore->GetRenderSys()->GetDrawEnvNode()->AddLightSet(lightSetNode);
    }
  }
}
void BattleRenderingPipeLine::RemoveObject_Poke( PokeTool::PokeModel *pokeModel , int opt )
{
  mModelRenderPath->RemoveNode(pokeModel->GetModelInstanceNode());
  mEdgeMapPath->RemoveEdgeRenderingTarget(pokeModel->GetModelInstanceNode());

  if( pokeModel->GetShadowModel() )
  {
    mShadowRenderPath->RemoveNode(pokeModel->GetShadowModel()->GetModelInstanceNode());
  }
  if( pokeModel->GetDistortionModel() )
  {
    mDistortionPokePostRenderPath->RemoveDistortionRenderingTarget( pokeModel->GetDistortionModel()->GetModelInstanceNode() );
    mDistPokeModelCnt--;
  }

  //ライトも削除
  for( int i=0;i<gfl2::renderingengine::scenegraph::instance::DrawEnvNode::LightSetMax;i++ )
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *lightSetNode = pokeModel->GetDrawEnvNode()->GetLightSet(i);
    if( lightSetNode )
    {
      mBtlvCore->GetRenderSys()->GetDrawEnvNode()->RemoveLightSet(lightSetNode);
      pokeModel->GetDrawEnvNode()->AddLightSet(lightSetNode); //親が切れてるから・・・
    }
  }
}

//キャラモデルを追加
void BattleRenderingPipeLine::AddObject_Chara( poke_3d::model::CharaModel *charaModel , int opt )
{
  if( opt >= 100 )
  {
    //ポケモンの身代わり人形
    mModelRenderPath->AddNode(charaModel->GetModelInstanceNode());
    mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
  }
  else
  if( opt == RENDER_OPT_SHADOW_MODEL )
  {
    //影モデル
    mShadowRenderPath->AddNode(charaModel->GetModelInstanceNode());
  }
  else
  {
    mModelRenderPath->AddNode(charaModel->GetModelInstanceNode());
    mEdgeMapPath->AddEdgeRenderingTarget(charaModel->GetModelInstanceNode());
  }
}

void BattleRenderingPipeLine::RemoveObject_Chara( poke_3d::model::CharaModel *charaModel , int opt )
{
  if( opt >= 100 )
  {
    //ポケモンの身代わり人形
    mModelRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
    mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
  }
  else
  if( opt == RENDER_OPT_SHADOW_MODEL )
  {
    //影モデル
    mShadowRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
  }
  else
  {
    mModelRenderPath->RemoveNode(charaModel->GetModelInstanceNode());
    mEdgeMapPath->RemoveEdgeRenderingTarget(charaModel->GetModelInstanceNode());
  }
}

void BattleRenderingPipeLine::AddObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからADD
      // 生成されていないパーツはNULL
      mModelRenderPath->AddNode(pParts);
      mEdgeMapPath->AddEdgeRenderingTarget(pParts);
    }
  }
}

void BattleRenderingPipeLine::RemoveObject_DressUpChara( poke_3d::model::DressUpModel *dressModel , int opt )
{
  for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pParts = dressModel->GetPartsModelInstanceNode( static_cast<poke_3d::model::DressUpModel::PartsModel>(i));
    if (pParts)
    {
      // NULLチェックしてからADD
      // 生成されていないパーツはNULL
      mModelRenderPath->RemoveNode(pParts);
      mEdgeMapPath->RemoveEdgeRenderingTarget(pParts);
    }
  }
}

GFL_NAMESPACE_END(btl)