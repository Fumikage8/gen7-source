//======================================================================
/**
 * @file	BgSystem.cpp
 * @brief	戦闘背景システム
 * @author	ariizumi
 * @data	15/04/10
 */
//======================================================================

#include <arc_def_index/arc_def.h>
#include <arc_index/background.gaix>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>

#include <util/include/gfl2_GLHeapAllocator.h>
#include <math/include/gfl2_EulerRotation.h>

#include <Battle/Background/include/BgSystem.h>
#include "BgObject.h"

#include "System/include/SkyBox/Skybox.h"

#include <GameSys/include/GameManager.h>
#include "GameSys/include/TimeZone.h"

#include <model/include/gfl2_BaseModel.h>
#include <model/include/gfl2_BaseCamera.h>

#include <math/include/gfl2_Float.h>

//cdat
#include "niji_conv_header/battle/background/bgsys_near_ground.cdat"
#include "niji_conv_header/battle/background/bgsys_far_ground.cdat"
#include "niji_conv_header/battle/background/bgsys_normal_ground.cdat"


GFL_NAMESPACE_BEGIN(btl)

const f32 BgSystem::SKY_FAR_PLANE = 50000.0f;
const f32 BgSystem::FAR_FAR_PLANE = 15000.0f;


//コンストラクタ
GFL_DLL_EXPORT BgSystem::BgSystem()
{
  mIsEnable = false;
	mIsUseVram = false;
  mIsVisible = false;
  mIsSkyboxSelfCreate = false;
  mIsInitializeSkyBox = false;
}

//デストラクタ
GFL_DLL_EXPORT BgSystem::~BgSystem()
{
}

//システム初期化
GFL_DLL_EXPORT void BgSystem::Initialize(const INIT_PARAM &initParam,s32 hour, s32 minute, s32 second )
{
  mInitParam = initParam;

  mGlAllocator = GFL_NEW(mInitParam.heap) System::nijiAllocator(mInitParam.heap);

  mpSkyBox = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetSkybox();
  
  for( int i=0;i<OBJ_MAX;i++ )
  {
    mObject[i] = GFL_NEW(mInitParam.heap)BgObject();
  }

  mRenderPath = GFL_NEW(mInitParam.heap)System::ModelRenderPath(mGlAllocator,OBJ_MAX);
  mSkyRenderPath = GFL_NEW(mInitParam.heap)System::ModelRenderPath(mGlAllocator,10);
  	// メインとなるシーン描画パスを初期化
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc;
	desc.m_DrawManagerDesc.m_NodeStackSize	  = 64;
	desc.m_DrawManagerDesc.m_DrawTagStackSize = desc.m_DrawManagerDesc.m_NodeStackSize * 16;
	
  mRenderPath->Initialize( mGlAllocator,desc );

  //スカイボックスの描画パスを初期化
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath::InitDescription desc2;
	desc2.m_DrawManagerDesc.m_NodeStackSize	  = 64;
	desc2.m_DrawManagerDesc.m_DrawTagStackSize = desc2.m_DrawManagerDesc.m_NodeStackSize * 16;
  mSkyRenderPath->Initialize( mGlAllocator,desc2 );

  mSkyEnvNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mGlAllocator);
  // ライティング用に環境ノードを追加
  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode		*pSkyLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pSkyDirectionalLightNode;
    gfl2::renderingengine::scenegraph::instance::LightNode			*pSkyAmbientLightNode;

    pSkyLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(mGlAllocator);
    pSkyDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mGlAllocator);
    pSkyAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(mGlAllocator);

    {
  	  pSkyDirectionalLightNode->SetNodeName( "DirectionalLight02" );
  	  gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pSkyDirectionalLightNode->GetNodeName()) );
  	  pSkyDirectionalLightNode->SetNodeNameHash( nameHash.value );
  	  pSkyDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
  	  gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 00000000.0f);
  	  pSkyDirectionalLightNode->SetPosition( position );
  	  gfl2::math::Vector3 direction = -position.Normalize();
  	  pSkyDirectionalLightNode->SetDirection( direction );
  	  pSkyDirectionalLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

    {
  	  pSkyAmbientLightNode->SetNodeName( "AmbientLight02" );
  	  gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pSkyAmbientLightNode->GetNodeName()) );
  	  pSkyAmbientLightNode->SetNodeNameHash( nameHash.value );
  	  pSkyAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
  	  pSkyAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
    }

    pSkyLightSetNode->SetSetNo( 0 );
    pSkyLightSetNode->AddLight( pSkyDirectionalLightNode );//LightSetNodeの子となる
    pSkyLightSetNode->AddLight( pSkyAmbientLightNode );//LightSetNodeの子となる
    mSkyEnvNode->AddLightSet( pSkyLightSetNode );//DrawEnvNodeの子となる
  }

#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化する
  if (mInitParam.useHio == false)
#endif
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_BACKGROUND;
    openReq.heapForFile = mInitParam.heap;
    openReq.heapForReq = mInitParam.heap->GetLowerHandle();
    mInitParam.fileManager->AddArcFileOpenReq(openReq);
  }

  m_SkyInitSeq=0;
  mNowAnimType = GetNowTimeAnimLoop(hour, minute, second);
  mWeatherAnimNow = ANM_LOOP;
  mWeatherAnimRequest = ANM_LOOP;
}



//天球のカメラをセットする
GFL_DLL_EXPORT void BgSystem::SetSkyMtx(const gfl2::math::Matrix44 &Proj,	const gfl2::math::Matrix34 &View)
{
  mSkyCamera.m_Projection = Proj;
  mSkyCamera.m_View = View;

  mSkyRenderPath->AddSheneCamera(&mSkyCamera);
}

GFL_DLL_EXPORT void BgSystem::SetFarMtx(const gfl2::math::Matrix44 &Proj, const gfl2::math::Matrix34 &View)
{
  mFarCamera.m_Projection = Proj;
  mFarCamera.m_View = View;

  mRenderPath->AddSheneCamera(&mFarCamera);
}




//スカイボックス初期化
GFL_DLL_EXPORT bool BgSystem::InitializeSkyBox(gfl2::heap::HeapBase *pDevHeap)
{
  if( mSetupParam.useSkyBox == false )
  {
    m_SkyInitSeq = 3;
    return true;
  }
  switch(m_SkyInitSeq){
  case 0:
    if(mpSkyBox->IsCreateResource() == false)
    {
      mpSkyBox->CreateHeap();

      System::Skybox::Skybox::Description desc;
      desc.rotateRadY = 0.0f;
      mpSkyBox->StartCreateResource(desc, pDevHeap);
      mIsSkyboxSelfCreate = true;
      m_SkyInitSeq = 1;
    }
    //break through;
  case 1:
    if(mpSkyBox->UpdateCreateResource() == false)
    {
      break;
    }
    m_SkyInitSeq = 2;
    //break through;
  case 2:
    mpSkyBox->CreateParts( NULL, true, System::nijiAllocator::VRAM_MALLOC_A);
    for(int ps = 0; ps < System::Skybox::Skybox::PARTS_TYPE_MAX; ps++ )
    {
      poke_3d::model::BaseModel* pBax = mpSkyBox->GetPartsModel((System::Skybox::Skybox::PartsType)ps);
      if(pBax)
      {
        pBax->SetPosition(0, 0, 0);
        mSkyRenderPath->AddNode(pBax->GetModelInstanceNode());
      }
    }
    mpSkyBox->Initialize(mSetupParam.skyType);
    mIsInitializeSkyBox = true;
    m_SkyInitSeq++;
    //break through;
  case 3:
    return true;
    //break;
  }
  return false;
}





GFL_DLL_EXPORT bool BgSystem::IsFinishInitialize(void)
{
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == true)
  {
    return true;
  }
#endif
  return mInitParam.fileManager->IsArcFileOpenFinished(ARCID_BACKGROUND);
}

//システム開放
GFL_DLL_EXPORT void BgSystem::Terminate(void)
{
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == false)
#endif
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_BACKGROUND;
    closeReq.heapForReq = mInitParam.heap->GetLowerHandle();
    mInitParam.fileManager->AddArcFileCloseReq(closeReq);
  }

  if( mIsInitializeSkyBox == true )
  {
    mpSkyBox->Terminate();
    if( mIsSkyboxSelfCreate)
    {
      mpSkyBox->DestroyAll();
      mpSkyBox->DeleteHeap();
    }
    else
    {
      mpSkyBox->DestroyParts();
    }
    mIsInitializeSkyBox = false;
  }
  
  for( int i=0;i<OBJ_MAX;i++ )
  {
    GFL_SAFE_DELETE( mObject[i] );
  }
  GFL_SAFE_DELETE(mSkyEnvNode);
  GFL_SAFE_DELETE(mSkyRenderPath);
  GFL_SAFE_DELETE(mRenderPath);

  GFL_SAFE_DELETE(mGlAllocator);
}

GFL_DLL_EXPORT bool BgSystem::IsFinishTerminate(void)
{
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == true)
  {
    return true;
  }
#endif
  return mInitParam.fileManager->IsArcFileCloseFinished(ARCID_BACKGROUND);
}

//システム読み込み
GFL_DLL_EXPORT void BgSystem::StartLoad(const SETUP_PARAM &setupParam)
{
  mSetupParam = setupParam;
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化する
  if (mInitParam.useHio == true)
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_BACKGROUND;
    openReq.heapForFile = mInitParam.heap;
    openReq.heapForReq = mInitParam.heap->GetLowerHandle();
    openReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_HIO;
    openReq.arcSrcCreateSetting = gfl2::fs::AsyncFileManager::ArcSrcCreateSetting::ALWAYS_CREATE;
    mInitParam.fileManager->AddArcFileOpenReq(openReq);
  }
#endif

  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = ARCID_BACKGROUND;
  loadReq.align = 128;
  loadReq.heapForBuf = mInitParam.heap;
  if( mInitParam.workHeap )
  {
    loadReq.heapForReq = mInitParam.workHeap->GetLowerHandle();
    loadReq.heapForCompressed = mInitParam.workHeap->GetLowerHandle();
  }
  else
  {
    loadReq.heapForReq = mInitParam.heap->GetLowerHandle();
    loadReq.heapForCompressed = mInitParam.heap->GetLowerHandle();
  }
#if PM_DEBUG
  if( mInitParam.useHio )
  {
    loadReq.arcSrcUseSetting = gfl2::fs::AsyncFileManager::ArcSrcUseSetting::ALWAYS_USE;
  }
#endif

  if( mSetupParam.groundType < elementof(btl::bg::bgsys_normal_ground_filetbl) ){
    loadReq.datId = btl::bg::bgsys_normal_ground_filetbl[ mSetupParam.groundType ];
  }
  else{
    loadReq.datId = btl::bg::bgsys_normal_ground_filetbl[ 0 ];
  }
  if(mSetupParam.groundType!=0){
    loadReq.ppBuf = mObject[OBJ_GROUND]->GetBufPointer();
    mInitParam.fileManager->AddArcFileLoadDataReq(loadReq);
       mObject[OBJ_GROUND]->SetRead(true);
  }

  if( mSetupParam.nearType < elementof(btl::bg::bgsys_near_ground_filetbl) ){
    loadReq.datId = btl::bg::bgsys_near_ground_filetbl[ mSetupParam.nearType ];
  }
  else{
    loadReq.datId = btl::bg::bgsys_near_ground_filetbl[ 0 ];
  }
  if(loadReq.datId!=0){
    loadReq.ppBuf = mObject[OBJ_NEAR]->GetBufPointer();
    mInitParam.fileManager->AddArcFileLoadDataReq(loadReq);
     mObject[OBJ_NEAR]->SetRead(true);
  }

  if( mSetupParam.farType < elementof(btl::bg::bgsys_far_ground_filetbl) ){
    loadReq.datId = btl::bg::bgsys_far_ground_filetbl[ mSetupParam.farType ];
  }
  else{
    loadReq.datId = btl::bg::bgsys_far_ground_filetbl[ 0 ];
  }
  if(loadReq.datId!=0){
    loadReq.ppBuf = mObject[OBJ_FAR]->GetBufPointer();
    mInitParam.fileManager->AddArcFileLoadDataReq(loadReq);
     mObject[OBJ_FAR]->SetRead(true);
  }

  if( mSetupParam.groundType < elementof(btl::bg::bgsys_normal_ground_b_filetbl) &&
      btl::bg::bgsys_normal_ground_b_filetbl[ mSetupParam.groundType ] != -1 ){
    loadReq.datId = btl::bg::bgsys_normal_ground_b_filetbl[ mSetupParam.groundType ];
    loadReq.ppBuf = mObject[OBJ_BUILD]->GetBufPointer();
    mInitParam.fileManager->AddArcFileLoadDataReq(loadReq);
       mObject[OBJ_BUILD]->SetRead(true);
  }
  
  //ライトの読み込み
  if( mSetupParam.groundType < elementof(btl::bg::bgsys_normal_ground_l_filetbl) )
  {
    loadReq.datId = btl::bg::bgsys_normal_ground_l_filetbl[ mSetupParam.groundType ];
    loadReq.ppBuf = &mLightBuf;
    GFL_PRINT("[[[%d]]]\n", btl::bg::bgsys_normal_ground_l_filetbl[mSetupParam.groundType]);
    mInitParam.fileManager->AddArcFileLoadDataReq(loadReq);
  }
  else
  {
    GFL_ASSERT_MSG(0,"ライト指定は必ず必要\n");
  }
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == true)
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_BACKGROUND;
    closeReq.heapForReq = mInitParam.heap->GetLowerHandle();
    mInitParam.fileManager->AddArcFileCloseReq(closeReq);
  }
#endif
}



GFL_DLL_EXPORT void BgSystem::LoadUpdate(void)
{
  InitializeSkyBox(mInitParam.workHeap?mInitParam.workHeap:mInitParam.heap);
}


GFL_DLL_EXPORT bool BgSystem::IsFinishLoad(void)
{
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == true)
  {
    if (mInitParam.fileManager->IsArcFileOpenFinished(ARCID_BACKGROUND) == false)
    {
      return false;
    }
  }
#endif
  for( int i=0;i<OBJ_MAX;i++ )
  {
    if(mObject[i]->GetRead())
    {
      if( mInitParam.fileManager->IsArcFileLoadDataFinished(mObject[i]->GetBufPointer()) == false )
      {
        return false;
      }
    }
  }
  if( mInitParam.fileManager->IsArcFileLoadDataFinished(&mLightBuf) == false )
  {
    return false;
  }
#if PM_DEBUG
  //HIOモードの時は読み込むときにARCを初期化・開放する
  if (mInitParam.useHio == true)
  {
    if (mInitParam.fileManager->IsArcFileCloseFinished(ARCID_BACKGROUND) == false)
    {
      return false;
    }
  }
#endif
  if(m_SkyInitSeq!=3)
  {
    return false;
  }
  return true;
}

//読み込み後にモデル作成
GFL_DLL_EXPORT void BgSystem::Create(void)
{
	// テクスチャリソースファクトリを取得する
	gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory =
		static_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(
		gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
		gfl2::renderingengine::scenegraph::resource::IResourceData::Type(gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName())
		));
	gfl2::gfx::Pool oldPool = pTextureResourceNodeFactory->GetPool();

	

  for( int i=0;i<OBJ_MAX;i++ )
  {
#if PM_DEBUG
		System::nijiAllocator::VramArea vramArea;
		static_cast<System::nijiAllocator*>(mGlAllocator)->GetApplication(&vramArea);
		int beforeSizeA = vramArea.vramA->GetContiniusFreeBlockNum() * System::nijiAllocator::AREA_MAN_BLOCK_SIZE;
		int beforeSizeB = vramArea.vramB->GetContiniusFreeBlockNum() * System::nijiAllocator::AREA_MAN_BLOCK_SIZE;
#endif

		if (mIsUseVram)
		{
			if ((i == OBJ_GROUND)/* || (i == OBJ_FAR) || (i == OBJ_NEAR)*/)
			{
				// vramから確保に変更
				pTextureResourceNodeFactory->SetPool(gfl2::gfx::Pool::LocalA);
			}
		}

    if(mObject[i]->Create(mGlAllocator))
    {
      mRenderPath->AddNode(mObject[i]->GetModelNode());			
    }


		if (mIsUseVram)
		{
			if ((i == OBJ_GROUND)/* || (i == OBJ_FAR) || (i == OBJ_NEAR)*/)
			{
#if PM_DEBUG
				int afterSizeA = vramArea.vramA->GetContiniusFreeBlockNum() * System::nijiAllocator::AREA_MAN_BLOCK_SIZE;
				int afterSizeB = vramArea.vramB->GetContiniusFreeBlockNum() * System::nijiAllocator::AREA_MAN_BLOCK_SIZE;
				switch (i)
				{
				case OBJ_GROUND:
					GFL_RELEASE_PRINT("BSGROUND,%d,", mSetupParam.groundType);
					break;
					/*case OBJ_FAR:
						GFL_RELEASE_PRINT("BSFAR,%d,", mSetupParam.farType);
						break;
						case OBJ_NEAR:
						GFL_RELEASE_PRINT("BSNEAR,%d,", mSetupParam.nearType);
						break;*/
				}

				GFL_RELEASE_PRINT("sizeA,0x%x,", beforeSizeA - afterSizeA);
				GFL_RELEASE_PRINT("sizeB,0x%x\n", beforeSizeB - afterSizeB);
				
				System::nijiAllocator::DebugVramMemoryInfoDump();
#endif // PM_DEBUG

				// vramから確保に変更
				pTextureResourceNodeFactory->SetPool(oldPool);
			}
		}
  }


  //ライトの設定
  mLightBinData.Initialize(mLightBuf);

  //Envの作成
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
    envData.SetBinaryEnvData(static_cast<c8*>(mLightBinData.GetData( 0 )));
    mEnvResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(mGlAllocator,&envData);
    mEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(mGlAllocator,mEnvResNode);

    //Envの適用
    mRenderPath->AddDrawEnv(mEnvNode);
    mSkyRenderPath->AddDrawEnv(mEnvNode);

    //アニメーションがらみ
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmResData;
    anmResData.SetMotionData(static_cast<c8*>(mLightBinData.GetData( 1 )));
    mEnvAnmNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( mGlAllocator,&anmResData );		

    mEnvAnmController.Initialize(mInitParam.heap,mEnvNode);
    mEnvAnmController.GetLightSlot()->SetAnimationResource(mEnvAnmNode);
    mEnvAnmController.GetFogSlot()->SetAnimationResource(mEnvAnmNode);

    mEnvNode->SetFogEnable(true);
  }



  mIsEnable = true;
  mIsVisible = true;

  PlayAnime(ANM_LOOP);
  PlayAnime(ANM_1DAY);
  if( mNowAnimType != 0 )
  {
    PlayAnime(mNowAnimType);
  }
}

GFL_DLL_EXPORT void BgSystem::Delete(void)
{
  mDeleteSeq = 0;
  mIsEnable = false;
}

GFL_DLL_EXPORT bool BgSystem::IsFinishDelete(void)
{
  switch( mDeleteSeq )
  {
  case 0:
    for( int i=0;i<OBJ_MAX;i++ )
    {
      if(mObject[i]->GetRead())
      {
        mRenderPath->RemoveNode(mObject[i]->GetModelNode());
      }
    }
    mSkyRenderPath->RemoveDrawEnv(mEnvNode);
    mRenderPath->RemoveDrawEnv(mEnvNode);
    mEnvAnmController.Finalize();
    mDeleteSeq++;
    break;
  case 1:
    for( int i=0;i<OBJ_MAX;i++ )
    {
      mObject[i]->Delete();
    }
    GFL_SAFE_DELETE(mEnvAnmNode);
    GFL_SAFE_DELETE(mEnvNode);
    GFL_SAFE_DELETE(mEnvResNode);
    GflHeapFreeMemory(mLightBuf);

    mDeleteSeq++;
    break;
  case 2:
    return true;
    //break;
  }
  return false;
}



/**
 * @brief 時間からアニメーションフレームに変換
 * @param hour 時間
 * @param minute 分
 * @param second 秒
 */
f32 BgSystem::CalculateTimeToAllTimeAnimationFrame( s32 hour, s32 minute, s32 second )
{
  const f32 ALL_TIME_ANIMATION_FRAME_MAX        = 480.0f;  // アニメーションリソースの最大フレーム数と合わせる
  const f32 ALL_TIME_ANIMATION_ONE_HOUR_FRAME   = ALL_TIME_ANIMATION_FRAME_MAX / 24.0f;     // 24 = 1日の合計時間数
  const f32 ALL_TIME_ANIMATION_ONE_MINUTE_FRAME = ALL_TIME_ANIMATION_FRAME_MAX / 1440.0f;   // 1440 = 1日の合計分数
  const f32 ALL_TIME_ANIMATION_ONE_SECOND_FRAME = ALL_TIME_ANIMATION_FRAME_MAX / 86400.0f;  // 86400 = 1日の合計秒数

  f32 hourFrame   = static_cast<f32>(hour) * ALL_TIME_ANIMATION_ONE_HOUR_FRAME;
  f32 minuteFrame = static_cast<f32>(minute) * ALL_TIME_ANIMATION_ONE_MINUTE_FRAME;
  f32 secondFrame = static_cast<f32>(second) * ALL_TIME_ANIMATION_ONE_SECOND_FRAME;
  f32 allTimeAnimationFrame = hourFrame + minuteFrame + secondFrame;

  return gfl2::math::Float::Floor( allTimeAnimationFrame, 2);
}



//時間の解釈を行ってアニメ再生を決める部分
GFL_DLL_EXPORT void BgSystem::TimeUpdate(s32 hour, s32 minute, s32 second, BgObject* pObject)
{
  if( !pObject->IsAnimPlaying( ANM_1DAY ) ){  //1dayanim処理
    return;
  }
  {
    pObject->SetAnimationFrame(ANM_1DAY, CalculateTimeToAllTimeAnimationFrame(hour, minute, second));
  }
//    SetAnimationFrame
}


GFL_DLL_EXPORT int BgSystem::GetNowTimeAnimLoop(s32 hour, s32 minute, s32 second)
{
  int changeAnim = 0;

  GameSys::TimeZone::Zone timeZone = GameSys::TimeZone::ConvertHourToTimeZone(hour);
  switch( timeZone )
  {
  case GameSys::TimeZone::MORNING: ///< 朝
    changeAnim = ANM_MORNING_LOOP;
    break;
  case GameSys::TimeZone::NOON:    ///< 昼
    changeAnim = ANM_NOON_LOOP;
    break;
  case GameSys::TimeZone::EVENING: ///< 夕
    changeAnim = ANM_EVENING_LOOP;
    break;
  case GameSys::TimeZone::NIGHT:   ///< 夜
  case GameSys::TimeZone::MIDNIGHT:///< 深夜
    changeAnim = ANM_NIGHT_LOOP;
    break;
  }
  return changeAnim;
}


GFL_DLL_EXPORT void BgSystem::TimeInOutControl(s32 hour, s32 minute, s32 second)
{
  int changeAnim = GetNowTimeAnimLoop( hour, minute, second);

  switch(mNowAnimType){
  case ANM_NIGHT_OUT:
  case ANM_EVENING_OUT:
  case ANM_NOON_OUT:
  case ANM_MORNING_OUT:
    if( IsAnimeEnd(mNowAnimType) ){
      mNowAnimType = changeAnim - 1; // @caution -1 がインアニメで無いと矛盾してしまう
      OHNO_PRINT("----- %d\n",mNowAnimType);
      PlayAnime(mNowAnimType);
    }
    return;
  case ANM_MORNING_IN:
  case ANM_NOON_IN:
  case ANM_EVENING_IN:
  case ANM_NIGHT_IN:
    //インアニメ実行中はなにも処理しない
    if( IsAnimeEnd(mNowAnimType) ){
      mNowAnimType++;
      PlayAnime(mNowAnimType);
    }
    return;
  case ANM_MORNING_LOOP:
  case ANM_NOON_LOOP:
  case ANM_EVENING_LOOP:
  case ANM_NIGHT_LOOP:
    if( mNowAnimType != changeAnim ){
      mNowAnimType++;     // @caution +1 がアウトアニメで無いと矛盾してしまう
      OHNO_PRINT("----- %d\n",mNowAnimType);
      PlayAnime(mNowAnimType);
    }
    break;
  }
}


GFL_DLL_EXPORT void BgSystem::Update(s32 hour, s32 minute, s32 second)
{
  if( mIsEnable )
  {
    for( int i=0;i<OBJ_MAX;i++ )
    {
      TimeUpdate( hour, minute, second,  mObject[i]);
      mObject[i]->Update();
    }
    TimeInOutControl( hour, minute, second );
    WeatherControl();

    mEnvAnmController.GetLightSlot()->SetFrame(CalculateTimeToAllTimeAnimationFrame(hour, minute, second));
    mEnvAnmController.GetFogSlot()->SetFrame(CalculateTimeToAllTimeAnimationFrame(hour, minute, second));
    mEnvAnmController.Update();

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam fogParam;
    fogParam.projMatrix = mSkyCamera.m_Projection;
    mEnvNode->UpdateFog(fogParam);
  }

  if( mIsEnable && mSetupParam.useSkyBox == true )
  {
    gfl2::math::Vector3 pos(0,200,0);
    mpSkyBox->Update( pos, hour, minute, second );
  }

}

GFL_DLL_EXPORT void BgSystem::SetVisible(bool flg)
{
  if( mIsEnable )
  {
    for( int i=0;i<OBJ_MAX;i++ )
    {
      mObject[i]->SetVisible(flg);
    }
    if( mSetupParam.useSkyBox == true )
    {
      mpSkyBox->SetEnable(flg);
    }
    mIsVisible = flg;
  }
}

GFL_DLL_EXPORT void BgSystem::PlayAnime(int anmType)
{
  if( mIsEnable )
  {
    GFL_PRINT("BgSystem ChangeAnime[%d]\n",anmType);
    //一回そのスロットのアニメを止めておく
    StopAnime(anmType);
    for( int i=0;i<OBJ_MAX;i++ )
    {
      mObject[i]->PlayAnimeCore(anmType);
    }
    //mAnimType = anmType;
  }
}

GFL_DLL_EXPORT void BgSystem::StopAnime(int anmType)
{
  if( mIsEnable )
  {
    for( int i=0;i<OBJ_MAX;i++ )
    {
      mObject[i]->StopAnimeCore(anmType);
    }
    //mAnimType = 0;
  }
}



GFL_DLL_EXPORT bool BgSystem::IsAnimeEnd(int anmType)
{
  if( mIsEnable )
  {
    for( int i=0;i<OBJ_MAX;i++ )
    {
      if(!mObject[i]->IsAnimeEnd(anmType)){  //終わってないOBJがある
        return false;
      }
    }
  }
  return true;  //実行していない+全完了ならTRUE
}



GFL_DLL_EXPORT void BgSystem::RequestAnime(int anmType)
{
  if( mIsEnable )
  {
    switch(anmType){
    case ANM_LOOP:
    case ANM_RAIN_LOOP:
    case ANM_SUNLIGHT_LOOP:
    case ANM_CLOUD_LOOP:
    case ANM_WIND_LOOP:
    case ANM_SNOW_LOOP:
      mWeatherAnimRequest = anmType;
      OHNO_PRINT("--mWeatherAnimNow start --- %d\n",mWeatherAnimRequest);

      break;
    default:
      mWeatherAnimRequest = ANM_LOOP;  //他のラベルでのリクエストは対応していない
      GFL_PRINT("WARRINIG このラベルでは動きません %d\n",anmType); 
      break;
    }
  }
}
  


GFL_DLL_EXPORT void BgSystem::WeatherControl(void)
{
  switch(mWeatherAnimNow){
  case ANM_RAIN_OUT:
  case ANM_SUNLIGHT_OUT:
  case ANM_CLOUD_OUT:
  case ANM_WIND_OUT:
  case ANM_SNOW_OUT:
    if( IsAnimeEnd(mWeatherAnimNow) ){
      if(mWeatherAnimRequest==ANM_LOOP){// 元に戻したい時
        mWeatherAnimNow = ANM_LOOP; // 完了
        break;
      }
      else{
        mWeatherAnimNow = mWeatherAnimRequest - 1;
        OHNO_PRINT("--mWeatherAnimNow --- %d\n",mWeatherAnimNow);
        PlayAnime(mWeatherAnimNow);
      }
    }
    break;
  case ANM_SNOW_IN:
  case ANM_WIND_IN:
  case ANM_CLOUD_IN:
  case ANM_SUNLIGHT_IN:
  case ANM_RAIN_IN: 
    if( IsAnimeEnd(mWeatherAnimNow) ){
      mWeatherAnimNow++;
      OHNO_PRINT("--mWeatherAnimloopNow --- %d\n",mWeatherAnimNow);
      PlayAnime(mWeatherAnimNow); //LOOPに移行
    }
    break;
  case ANM_LOOP: //何も動いていない時
    if( mWeatherAnimNow != mWeatherAnimRequest ){
      mWeatherAnimNow = mWeatherAnimRequest - 1; // INに変更
    }
    break;
  case ANM_SNOW_LOOP:
  case ANM_WIND_LOOP:
  case ANM_CLOUD_LOOP:
  case ANM_SUNLIGHT_LOOP:
  case ANM_RAIN_LOOP:
    if( mWeatherAnimNow != mWeatherAnimRequest ){
      mWeatherAnimNow++;
      OHNO_PRINT("--mWeatherAnimNow--- %d\n", mWeatherAnimNow);
      PlayAnime( mWeatherAnimNow );
    }
    break;
  }
}

//バトルでロイヤル背景をまわす対応
GFL_DLL_EXPORT void BgSystem::RotateY_Deg(f32 rot)
{
  gfl2::math::Vector3 vecRot(0,gfl2::math::ConvDegToRad(rot),0);
  gfl2::math::EulerRotation eulerRot;
  eulerRot.Set(vecRot,gfl2::math::EulerRotation::RotationOrderZYX);
  for( int i=0;i<OBJ_MAX;i++ )
  {
    mObject[i]->RotateY_Deg(eulerRot.ToQuaternion());
  }
}

// アニメーションフレームを設定
GFL_DLL_EXPORT void BgSystem::SetAnimationFrame(BG_OBJ_TYPE objType,int anmType,u32 frame)
{
  if(objType >= 0 && objType < OBJ_MAX)
  {
    if(mIsEnable)
    {
      if(mObject[objType]->GetModelNode())
      {
        mObject[objType]->SetAnimationFrame(anmType,frame);
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"Object Array Over!\n");
  }
}

// アニメーションフレームを取得
GFL_DLL_EXPORT int BgSystem::GetAnimationFrame(BG_OBJ_TYPE objType,int anmType)
{
  if(objType >= 0 && objType < OBJ_MAX)
  {
    if(mIsEnable)
    {
      if(mObject[objType]->GetModelNode())
      {
        return mObject[objType]->GetAnimationFrame(anmType);
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"Object Array Over!\n");
  }

  return -1;
}

// 最終フレームを取得
GFL_DLL_EXPORT int BgSystem::GetLastAnimationFrame(BG_OBJ_TYPE objType,int anmType)
{
  if(objType >= 0 && objType < OBJ_MAX)
  {
    if(mIsEnable)
    {
      if(mObject[objType]->GetModelNode())
      {
        return mObject[objType]->GetAnimationLastFrame(anmType);
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"Object Array Over!\n");
  }

  return -1;
}

// 再生速度を取得
GFL_DLL_EXPORT int BgSystem::GetAnimationStepFrame(BG_OBJ_TYPE objType,int anmType)
{
  if(objType >= 0 && objType < OBJ_MAX)
  {
    if(mIsEnable)
    {
      if(mObject[objType]->GetModelNode())
      {
        return mObject[objType]->GetAnimationStepFrame(anmType);
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"Object Array Over!\n");
  }

  return -1;
}

// アニメーションの更新のオンオフを切り替える
GFL_DLL_EXPORT void BgSystem::SetAnimationUpdateFrameEnable(bool isUpdateAnimation)
{
  if(mIsEnable)
  {
    for(int i = 0;i<OBJ_MAX;i++)
    {
      mObject[i]->SetAnimationUpdateFrameEnable(isUpdateAnimation);
    }
  }
}

GFL_NAMESPACE_END(btl)
