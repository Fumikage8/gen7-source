#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_McnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <math/include/gfl2_QuaternionNative.h>
#include <math/include/gfl2_Quaternion.h>

#include <system/include/InputDevice/gfl2_Mouse.h>

#include <gfx/include/gfl2_Texture.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Texture.h>


#include "rendering_pipeline.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::animation;

using namespace gfl2::system;

// アニメーションは30FPSで作られているが実行環境は60FPS
static const f32 SC_STEP_FRAME = 30.0f / 60.0f;

class GLAllocator : public IGLAllocator
{
public:
	GLAllocator():IGLAllocator(){}
	virtual ~GLAllocator(){}

	virtual void* SystemMemoryMalloc(u32 size, u32 alignment){
		return malloc(size);
	}

	virtual void SystemMemoryFree(void* const address)
	{
		free( address );
	}

	virtual void* LocalMemoryMalloc(u32 size, u32 alignment){
		return NULL;
	}

	virtual void LocalMemoryFree(void* const address)
	{
		
	}
};

enum POKE_ID
{
  POKE_ID_PIKACHU,
  POKE_ID_YADON,
  POKE_ID_THUNDERS,
  POKE_ID_TORNELOS,
  POKE_ID_KOIKING,
  POKE_ID_MAGGYO,
  POKE_ID_LATIAS,
  POKE_ID_GANGAR,
  POKE_ID_STARMIE,
  POKE_ID_NIDORINO,
  POKE_ID_GRAENA,

  POKE_ID_COUNT
};

struct PokeParam
{
  const c8* pModelFilePath;
  const c8* pMotionConfigFilePath;
  const c8* pAnimationFilePath;
  const f32 scale;
  math::Vector3 cameraPosition;
  math::Vector3 cameraAt;
};

static PokeParam s_pokeParamList[] =
{
  // POKE_ID_PIKACHU, XY X
  {
    "../data/pm0025_00/pm0025_00.gfmdl",
    "../data/pm0025_00/pm0025_00_motion_kw.mcnf",
    "../data/pm0025_00/pm0025_00_kw01_wait01.gfmot",
    1.65f,
    math::Vector3(2.0f, 48.0f, 204.0f),
    math::Vector3(0.0f, 39.0f,  0.0f),
  },
  // POKE_ID_YADON, YZ
  {
    "../data/pm0079_00_yadon/pm0079_00.gfmdl",
    "../data/pm0079_00_yadon/pm0079_00_motion_kw.mcnf",
    "../data/pm0079_00_yadon/pm0079_00_ba10_waitA01.gfmot",
    0.7894f,
    math::Vector3(2.0f, 48.0f, 204.0f),
    math::Vector3(0.0f, 39.0f,  0.0f),
  },
  // POKE_ID_THUNDERS, ZY
  {
    "../data/pm0135_00_thunders/pm0135_00.gfmdl",
    "../data/pm0135_00_thunders/pm0135_00_motion_kw.mcnf",
    "../data/pm0135_00_thunders/pm0135_00_kw01_wait01.gfmot",
    1.1f,
    math::Vector3(2.0f, 48.0f, 204.0f),
    math::Vector3(0.0f, 39.0f,  0.0f),
  },
  // POKE_ID_TORNELOS, XZ X
  {
    "../data/pm0641_12_tornelos/pm0641_12.gfmdl",
    "../data/pm0641_12_tornelos/pm0641_12_motion_kw.mcnf",
    "../data/pm0641_12_tornelos/pm0641_12_kw01_wait01.gfmot",
    0.913f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_KOIKING, ZY
  {
    "../data/pm0129_00_koiking/pm0129_00.gfmdl",
    "../data/pm0129_00_koiking/pm0129_00_motion_kw.mcnf",
    "../data/pm0129_00_koiking/pm0129_00_kw01_wait01.gfmot",
    0.913f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_MAGGYO, Y
  {
    "../data/pm0618_00_maggyo/pm0618_00.gfmdl",
    "../data/pm0618_00_maggyo/pm0618_00_motion_kw.mcnf",
    "../data/pm0618_00_maggyo/pm0618_00_kw01_wait01.gfmot",
    1.157f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_LATIAS, ZY, Z, Z
  {
    "../data/pm0380_00_latias/pm0380_00.gfmdl",
    "../data/pm0380_00_latias/pm0380_00_motion_kw.mcnf",
    "../data/pm0380_00_latias/pm0380_00_kw01_wait01.gfmot",
    1.093f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_GANGAR, X_M90
  {
    "../data/pm0094_00_gangar/pm0094_00.gfmdl",
    "../data/pm0094_00_gangar/pm0094_00_motion_kw.mcnf",
    "../data/pm0094_00_gangar/pm0094_00_kw01_wait01.gfmot",
    0.74f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_STARMIE, Y_90
  {
    "../data/pm0121_00_starmie/pm0121_00.gfmdl",
    "../data/pm0121_00_starmie/pm0121_00_motion_kw.mcnf",
    "../data/pm0121_00_starmie/pm0121_00_ba10_waitA01.gfmot",
    0.74f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_NIDORINO, Z_180
  {
    "../data/pm0033_00_nidorino/pm0033_00.gfmdl",
    "../data/pm0033_00_nidorino/pm0033_00_motion_kw.mcnf",
    "../data/pm0033_00_nidorino/pm0033_00_kw01_wait01.gfmot",
    1.0f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
  // POKE_ID_GRAENA,
  {
    "../data/pm0262_00_graena/pm0262_00.gfmdl",
    "../data/pm0262_00_graena/pm0262_00_motion_kw.mcnf",
    "../data/pm0262_00_graena/pm0262_00_kw01_wait01.gfmot",
    1.0f,
    math::Vector3(2.0f, 48.0f, 254.0f),
    math::Vector3(0.0f, 70.0f,  0.0f),
  },
};

static PokeParam* s_pokeParam = &s_pokeParamList[POKE_ID_PIKACHU];

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	math::Matrix44                              m_Projection;                         ///< 射影行列
	math::Matrix34                              m_View;                               ///< ビュー行列
	math::Matrix34                              m_World;                              ///< ワールド行列

	KawaigariRenderingPipeLine*               m_RenderingPipeLine;                  ///< レンダリングパイプライン
	scenegraph::resource::ResourceNode*       m_pModelResourceRootNode;             ///< モデルリソース
	scenegraph::resource::ResourceNode*       m_pIdTextureResourceRootNode;         ///< IDテクスチャリソース
	scenegraph::resource::ResourceNode*       m_pAnimationResourceRootNode;         ///< アニメーションリソース
	scenegraph::resource::ResourceNode*       m_pMotionConfigResourceRootNode;      ///< モーション設定リソース
	scenegraph::instance::ModelInstanceNode*  m_pModelInstanceNode;                 ///< モデルインスタンス
	scenegraph::instance::ModelInstanceNode*  m_pIdModelInstanceNode;               ///< ID用モデルインスタンス
	scenegraph::instance::MotionConfigInstanceNode*  m_pMotionConfigInstanceNode;   ///< モーションコンフィグインスタンス

  scenegraph::instance::DrawEnvNode*        m_pDrawEnvNode; 
	scenegraph::instance::LightSetNode*       m_pLightSetNode;
	scenegraph::instance::LightNode*          m_pDirectionalLightNode;
	scenegraph::instance::LightNode*          m_pAmbientLightNode;

	AnimationController                       m_AnimationController;                ///< アニメーションコントローラー
  JointController                           m_JointController;                    ///< ジョイントコントローラー

	u32                                       m_FrameCnt;                           ///< フレームカウント
  u32                                       m_AnimationNo;                        ///< アニメーション番号

  f32                                       m_InterestWeight;                     ///< 注視ウェイト
  math::Vector3                             m_InterestPosition;                   ///< 注視位置

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
    m_AnimationNo(0)
	{
	
	}

	virtual ~SimpleDraw(){}

  scenegraph::resource::ResourceNode* LoadIDTextures(const std::string& modelPath)
  {
    struct ImageHeader
    {
      s32						size;
      s32						dumy[3];
      c8						path[64];
    };

    scenegraph::resource::TextureResourceNodeFactory textureResourceNodeFactory;

    std::size_t lastIndex = modelPath.find_last_of("/");
    std::string dirtmp = modelPath.substr(0, lastIndex);
    dirtmp += "/Texture/";

    // *Id.tgaを全検索
    std::wstring dir;
    dir.resize(dirtmp.length());

    for (u32 i = 0; i < dir.length(); ++i)
    {
      dir[i] = dirtmp[i];
    }

    WIN32_FIND_DATA win32fd;
    HANDLE hFind = FindFirstFile((dir + L"*Id.tga").c_str(), &win32fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
      return NULL;
    }

    scenegraph::resource::ResourceNode* pTopNode = NULL;// new scenegraph::resource::ResourceNode();

    do {
      void* buf = malloc(sizeof(ImageHeader) + win32fd.nFileSizeLow);
      DWORD readSize = 0;
      HANDLE hFile = CreateFile((dir + win32fd.cFileName).c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      if (::ReadFile(hFile, reinterpret_cast<u8*>(buf) + sizeof(ImageHeader), win32fd.nFileSizeLow, &readSize, NULL))
      {
        ImageHeader* header = reinterpret_cast<ImageHeader*>(buf);
        header->size = readSize;
        for (s32 i = 0; i < sizeof(header->path); ++i)
        {
          header->path[i] = (c8)win32fd.cFileName[i];
        }
        scenegraph::resource::ResourceNode* pResourceNode = textureResourceNodeFactory.CreateResourceNode(buf, sizeof(ImageHeader) + readSize);
        if (pTopNode == NULL)
        {
          pTopNode = pResourceNode;
        }
        else
        {
          pTopNode->AddChild(pResourceNode);
        }
      }
      CloseHandle(hFile);

      free(buf);
    } while (FindNextFile(hFind, &win32fd));

    FindClose(hFind);

    return pTopNode;
  }

	virtual bool OnInit( void )
	{
		math::Matrix				lerpMat;
		math::Vector				col0(0, 0, 0, 0);

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( DisplayMode::HD720P, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );
    DrawUtil::Initialize(&m_Allocator, &m_Handle);

    InputDevice::Mouse::Initialize();

		{
			scenegraph::SceneGraphManager::Initialize();
			scenegraph::resource::ResourceManager::Initialize();
			m_RenderingPipeLine = new KawaigariRenderingPipeLine();
		}

		{
      // モデル読み込み
			scenegraph::resource::GfMdlData modelResourceData;
			modelResourceData.ReadFile(s_pokeParam->pModelFilePath);
			m_pModelResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&modelResourceData);

      // ID用のテクスチャ読み込み
      m_pIdTextureResourceRootNode = this->LoadIDTextures(s_pokeParam->pModelFilePath);

      // モーション設定
      scenegraph::resource::McnfData motionConfigResourceData;
      motionConfigResourceData.ReadFile(s_pokeParam->pMotionConfigFilePath);
      m_pMotionConfigResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);

      // アニメーション
			scenegraph::resource::GfMotData animationResourceData;
			animationResourceData.ReadFile(s_pokeParam->pAnimationFilePath);
			m_pAnimationResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode(&animationResourceData);
		}

		{
      // モデルインスタンス作成
			m_pModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );
			m_pIdModelInstanceNode = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode );

      m_pIdModelInstanceNode->SetVisible(false);
      m_RenderingPipeLine->AddIDObject(m_pIdModelInstanceNode);

      for (u32 i = 0; i < m_pIdModelInstanceNode->GetMaterialNum(); ++i)
      {
        scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode = m_pIdModelInstanceNode->GetMaterialInstanceNode(i);

        
        for  (u32 j = 0; j < pMaterialInstanceNode->GetTextureNum(); ++j)
        {
          // テクスチャインフォを書き換え
          scenegraph::resource::MaterialResourceNode::TextureInfo* pInfo = pMaterialInstanceNode->GetTextureInfo(j);
          pInfo->m_Attribute.m_ScaleU = 0;
          pInfo->m_Attribute.m_ScaleV = 0;
          pInfo->m_Attribute.m_Rotate = 0;
          pInfo->m_Attribute.m_TranslateU = 0;
          pInfo->m_Attribute.m_TranslateV = 0;

          pInfo->m_Attribute.m_RepeatU = 0;
          pInfo->m_Attribute.m_RepeatV = 0;

          pInfo->m_Attribute.m_MagFilter = 0;
          pInfo->m_Attribute.m_MinFilter = 0;

          // IDテクスチャに差し替え
          const scenegraph::resource::TextureResourceNode* pTextureResouceNode = pMaterialInstanceNode->GetTextureResourceNode(j);

          if (pTextureResouceNode == NULL)
          {
            continue;
          }

          if (m_pIdTextureResourceRootNode != NULL)
          {
            for (u32 k = 0; k < m_pIdTextureResourceRootNode->GetChildCount(); ++k)
            {
              const scenegraph::resource::TextureResourceNode* pIdTextureResourceNode = m_pIdTextureResourceRootNode->GetChild(k)->SafeCast<scenegraph::resource::TextureResourceNode>();
              std::string idName = pIdTextureResourceNode->GetName();
              idName = idName.substr(0, idName.size() - 6);
              idName += ".tga";
              if (strcmp(pTextureResouceNode->GetName(), idName.c_str()) == 0)
              {
                pMaterialInstanceNode->SetTextureResourceNode(j, pIdTextureResourceNode);

                pInfo->m_Attribute.m_ScaleU = 1;
                pInfo->m_Attribute.m_ScaleV = 1;
                break;
              }
            }
          }
        }

        // コンバイナ書き換え
        scenegraph::resource::MaterialResourceNode::AttributeParam* pAttributeParam = pMaterialInstanceNode->GetAttributeParam();
        scenegraph::resource::MaterialResourceNode::TextureCombinerAttribute* pCombinerRgbList = pAttributeParam->m_TextureCombinerAttribute[scenegraph::resource::CombinerColorType::RGB];
        scenegraph::resource::MaterialResourceNode::TextureCombinerAttribute* pCombinerAlphaList = pAttributeParam->m_TextureCombinerAttribute[scenegraph::resource::CombinerColorType::A];

        for (u32 j = 0; j < scenegraph::resource::CombinerAttribute::CombinerMax; ++j)
        {
          scenegraph::resource::MaterialResourceNode::TextureCombinerAttribute* pCombinerRgb = &pCombinerRgbList[j];

          pCombinerRgb->m_MathFormula = 0;
          pCombinerRgb->m_Scale = 1.0f;

          pCombinerRgb->m_InputSource[0] = 0;
          pCombinerRgb->m_InputSource[1] = 0;
          pCombinerRgb->m_InputSource[2] = 0;

          pCombinerRgb->m_Operand[0] = 0;
          pCombinerRgb->m_Operand[1] = 0;
          pCombinerRgb->m_Operand[2] = 0;

          pCombinerRgb->m_OutPutBuffer = false;
        }
        for (u32 j = 0; j < scenegraph::resource::CombinerAttribute::CombinerMax; ++j)
        {
          scenegraph::resource::MaterialResourceNode::TextureCombinerAttribute* pCombinerAlpha = &pCombinerAlphaList[j];

          pCombinerAlpha->m_MathFormula = 0;
          pCombinerAlpha->m_Scale = 1.0f;

          pCombinerAlpha->m_InputSource[0] = 0;
          pCombinerAlpha->m_InputSource[1] = 0;
          pCombinerAlpha->m_InputSource[2] = 0;

          pCombinerAlpha->m_Operand[0] = 2;
          pCombinerAlpha->m_Operand[1] = 2;
          pCombinerAlpha->m_Operand[2] = 2;

          pCombinerAlpha->m_OutPutBuffer = false;
        }

        pAttributeParam->m_VsLightingEnable = false;
        pAttributeParam->m_PsLightingEnable = false;
        pAttributeParam->m_LightSetNo = 1;
      }

      // アニメーション再生
      AnimationController::InitDescription desc;
      desc.jointSlotCount = 1;
			m_AnimationController.Initialize( m_pModelInstanceNode, desc );

      // ジョイントアニメーション
      JointAnimationSlot* pSlot = m_AnimationController.GetJointSlot();
      pSlot->SetAnimationResource(m_pAnimationResourceRootNode);
      pSlot->SetLoop(true);
      pSlot->SetStepFrame(SC_STEP_FRAME);
		}

    {
      // ジョイントコントローラー
      m_pMotionConfigInstanceNode = scenegraph::instance::InstanceCreator::CreateMotionConfigInstanceNode(m_pMotionConfigResourceRootNode);
      m_JointController.Initialize(m_pModelInstanceNode, m_pMotionConfigInstanceNode);

      //JointFunction* pJointFunction = m_JointController.GetJointFunction(0);
      //pJointFunction->SetJointNode(m_pModelInstanceNode->GetJointInstanceNode(4));
    }

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					(float)GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) / (float)GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ),	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

    // シーン環境初期化
    {
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode();
			m_pLightSetNode = scenegraph::instance::InstanceCreator::CreateLightSetNode();
			m_pDirectionalLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();
			m_pAmbientLightNode = scenegraph::instance::InstanceCreator::CreateLightNode();

			{
				m_pDirectionalLightNode->SetNodeName( "DirectionalLight01" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pDirectionalLightNode->GetNodeName()) );
				m_pDirectionalLightNode->SetNodeNameHash( nameHash.value );
				m_pDirectionalLightNode->SetLightType( LightType::Directional );
				math::Vector3 position = math::Vector3(0.0f, 10000000.0f, 10000000.0f);
				m_pDirectionalLightNode->SetPosition( position );
				math::Vector3 direction = -position.Normalize();
				m_pDirectionalLightNode->SetDirection( direction );
				m_pDirectionalLightNode->SetColor( math::Vector3( 0.7f, 0.7f, 0.7f ) );
			}

			{
				m_pAmbientLightNode->SetNodeName( "AmbientLight01" );
				util::FnvHash				nameHash( reinterpret_cast<const u8*>(m_pAmbientLightNode->GetNodeName()) );
				m_pAmbientLightNode->SetNodeNameHash( nameHash.value );
				m_pAmbientLightNode->SetLightType( LightType::Ambient );
				m_pAmbientLightNode->SetColor( math::Vector3( 1.0f, 1.0f, 1.0f ) );
			}

      m_pLightSetNode->SetSetNo(0);
			m_pLightSetNode->AddLight( m_pDirectionalLightNode );//LightSetNodeの子となる
			m_pLightSetNode->AddLight( m_pAmbientLightNode );//LightSetNodeの子となる
			m_pDrawEnvNode->AddLightSet( m_pLightSetNode );//DrawEnvNodeの子となる

			math::AABB				envSpace( math::Vector(-500000.0f, -500000.0f, -500000.0f), math::Vector(500000.0f, 500000.0f, 500000.0f) );
			m_pDrawEnvNode->SetAABB( envSpace );
			scenegraph::SceneGraphManager::AddChild( m_pDrawEnvNode );

			m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );
		}

    m_InterestWeight = 0.0f;

		return true;
	}

	virtual bool OnUpdate( void )
	{
#if 0
    static int updateFlag = 1;
    updateFlag ^= 1;
    if (updateFlag)
    {
      return true;
    }
#endif

    static f32 s_rot = 0.0f;
    math::Quaternion rot;
    //s_rot += 0.005f;
    rot.RadianYToQuaternion(s_rot);
    m_pModelInstanceNode->SetLocalRotation(rot);
    //m_pModelInstanceNode->SetLocalTranslate(math::Vector(0, -60, 0));

    InputDevice::Mouse::Update();

		{
			// ビュー変換行列
			//math::Vector eyePosition( 2.0f, 48.0f, 204.0f, 1.0f );  // 視点(カメラの位置)
			//math::Vector focusPosition( 0.0f, 39.0f,  0.0f, 1.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

      static f32 s_camRot = 0.0f;
      //s_camRot += 0.005f;
      math::Matrix34 camRotMat = math::Matrix34::GetRotationY(s_camRot);
      math::Vector3 camPos = camRotMat.TransformCoord(s_pokeParam->cameraPosition);

			m_View = math::Matrix34::GetLookAt(camPos, s_pokeParam->cameraAt, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

    // ターゲット移動
    math::Vector3 targetPos(0.0f, 100.0f, 50.0f);

    InputDevice::MousePos mousePos = InputDevice::Mouse::GetLeftUpOriginPosRatio();
    b32 isMouseClicked = InputDevice::Mouse::IsPress(InputDevice::MouseButton::Left);
    u32 touchedId = (isMouseClicked) 
      ? this->GetTouchedId(mousePos.x, mousePos.y)
      : 0;

    if (touchedId != 0)
    {
      c8 buf[256];
      sprintf_s(buf, "ID : 0x%08X\n", touchedId);
      OutputDebugStringA(buf);
    }

    mousePos = InputDevice::Mouse::GetLeftDownOriginPosRatio();
    mousePos.x = mousePos.x * 2.0f - 1.0f;
    mousePos.y = mousePos.y * 2.0f - 1.0f;

    targetPos.x = mousePos.x * 360;
    targetPos.y = mousePos.y * 240;

    // モデル行列更新
    f32 scl = s_pokeParam->scale;
    m_pModelInstanceNode->SetLocalScale(math::Vector3(scl, scl, scl));

    // アニメーション更新
    m_AnimationController.Update();

    // ワールド行列更新
    scenegraph::SceneGraphManager::TraverseModel();

    // 耳タッチしているか？
    bool earTouch = false;
    if ((s_pokeParam == &s_pokeParamList[POKE_ID_PIKACHU]) || (s_pokeParam == &s_pokeParamList[POKE_ID_THUNDERS]) || (s_pokeParam == &s_pokeParamList[POKE_ID_GRAENA]))
    {
      SpringController* pSpringController = m_JointController.GetSpringController();

      // 右耳タッチ
      static f32 downForceR = 0.0f;
      f32 TouchForce = 0.0f;
      f32 forceRate = 0.05f;
      if (touchedId == 0x000078)
      {
        earTouch = true;
        TouchForce = -8.0f;
        forceRate = 0.05f;
      }
      downForceR += (TouchForce - downForceR) * forceRate;
      // バネの外力設定
      pSpringController->SetSpringDownForceR(downForceR);

      // 左耳タッチ
      TouchForce = 0.0f;
      static f32 downForceL = 0.0f;
      if (touchedId == 0xff0078)
      {
        earTouch = true;
        TouchForce = -8.0f;
        forceRate = 0.05f;
      }
      downForceL += (TouchForce - downForceL) * forceRate;
      // バネの外力設定
      pSpringController->SetSpringDownForceL(downForceL);

      if (!earTouch)
      {
        earTouch = (downForceL * downForceL > 0.5f) || (downForceR * downForceR > 0.5f);
      }
    }

    // 注目位置設定
    InterestController* pInterestController = m_JointController.GetInterestController();
    m_InterestPosition += (targetPos - m_InterestPosition) * 0.1f;
    //pInterestController->SetInterestPosition((!earTouch && InputDevice::Mouse::IsPress(InputDevice::MouseButton::Left)) ? &m_InterestPosition : NULL);

    static f32 s_posRot = 0.0f;
    s_posRot += 0.01f;
    m_InterestPosition.Set(sin(s_posRot) * 2000, 0, 1000);
    pInterestController->SetInterestPosition(&m_InterestPosition);
    //pInterestController->SetInterestPosition(NULL);

    // ワールド行列更新後にジョイントコントローラーを更新
    m_JointController.Update();

    // ID用モデルに行列をコピー
    m_pIdModelInstanceNode->SetWorldMatrix(m_pModelInstanceNode->GetWorldMatrix());
    for (u32 i = 0; i < m_pModelInstanceNode->GetJointNum(); ++i)
    {
      scenegraph::instance::JointInstanceNode* pJointOriginal = m_pModelInstanceNode->GetJointInstanceNode(i);
      scenegraph::instance::JointInstanceNode* pJointId = m_pIdModelInstanceNode->GetJointInstanceNode(i);

      pJointId->SetWorldMatrix(pJointOriginal->GetWorldMatrix());
    }

		return true;
	}

  bool IsToutch(const math::Vector3& touchPos, const math::Vector3& bonePos)
  {
    math::Vector3 tmp = m_View.TransformCoord(bonePos);
    tmp = m_Projection.TransformCoord(tmp);

    tmp.x *= 360;
    tmp.y *= 240;

    f32 downForce = 0.0f;
    if (InputDevice::Mouse::IsPress(InputDevice::MouseButton::Left))
    {
      f32 subx = tmp.x - touchPos.x;
      f32 suby = tmp.y - touchPos.y;
      if ((subx * subx) + (suby * suby) < 500)
      {
        return true;
      }
    }
    return false;
  }

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

		return true;
	}

	virtual bool OnTerm( void )
	{
    if (m_pAmbientLightNode != NULL) delete m_pAmbientLightNode;
    if (m_pDirectionalLightNode != NULL) delete m_pDirectionalLightNode;
    if (m_pLightSetNode != NULL) delete m_pLightSetNode;
    if (m_pDrawEnvNode != NULL) delete m_pDrawEnvNode;

    m_JointController.Finalize();
    m_AnimationController.Finalize();

    if (m_pAnimationResourceRootNode != NULL) delete m_pAnimationResourceRootNode;
    if (m_pModelResourceRootNode != NULL) delete m_pModelResourceRootNode;
    if (m_pModelInstanceNode != NULL) delete m_pModelInstanceNode;
    if (m_pIdTextureResourceRootNode != NULL) delete m_pIdTextureResourceRootNode;
    if (m_pIdModelInstanceNode != NULL) delete m_pIdModelInstanceNode;

    if (m_pMotionConfigResourceRootNode != NULL) delete m_pMotionConfigResourceRootNode;

		if (m_RenderingPipeLine != NULL) delete m_RenderingPipeLine;

		scenegraph::resource::ResourceManager::Terminate();
    scenegraph::SceneGraphManager::Terminate();

    InputDevice::Mouse::Terminate();

    DrawUtil::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();
		return true;
	}

	//いずれＩＯクラスができたら置き換える。
	static c8* ReadFile( const char* filename, s32* p_size, s32 align = 16 )
	{
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, filename, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		*p_size = file_size;
		fseek( hFile, 0, SEEK_SET );

		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

		read_buff = reinterpret_cast<c8*>( GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

	
		return( read_buff );
	}

  u32 GetTouchedId(f32 screenX, f32 screenY)
  {
#if 0
    const gfx::windx11::WinDX11Texture* pTexture = dynamic_cast<const gfx::windx11::WinDX11Texture*>(m_RenderingPipeLine->GetIdTexture());

    // コピー用のバッファを作成
    D3D11_TEXTURE2D_DESC stagingBufferDesc;
    pTexture->GetTargetTexture()->GetDesc(&stagingBufferDesc);
    stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
    stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingBufferDesc.BindFlags = 0;
    ID3D11Texture2D* pStagingBuffer = NULL;
    HRESULT hr = gfx::GFGL::GetDevice()->CreateTexture2D(&stagingBufferDesc, 0, &pStagingBuffer);
    if (FAILED(hr)) assert( 0 );

    // コピー用のバッファにコピーする
    gfx::GFGL::GetDeviceContext()->CopyResource(pStagingBuffer, pTexture->GetTargetTexture().get());

    // CPU側から参照できるように展開
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    hr = gfx::GFGL::GetDeviceContext()->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
    if (FAILED(hr)) assert( 0 );

    u32* pIdData = static_cast<u32*>(mappedSubresource.pData);

    u32 x = static_cast<u32>(screenX * stagingBufferDesc.Width);
    u32 y = static_cast<u32>(screenY * stagingBufferDesc.Height);

    if (x >= stagingBufferDesc.Width) x = stagingBufferDesc.Width - 1;
    if (y >= stagingBufferDesc.Height) y = stagingBufferDesc.Height - 1;
    u32 id = pIdData[x + (y * stagingBufferDesc.Width)];

    // アルファ部分は要らない
    id &= 0x00FFFFFF;

    // 展開終了
    gfx::GFGL::GetDeviceContext()->Unmap(pStagingBuffer, 0);

    // コピー用バッファ開放
    pStagingBuffer->Release();

    return id;
#endif

    return 0;
  }

private:
	DisplayDeviceHandle					m_Handle;
	GLAllocator									m_Allocator;
};

SimpleDraw	*g_pApp = NULL;

Sample::Framework::IApplication* GetApplication()
{
	if( g_pApp == NULL ) {
		g_pApp = new SimpleDraw();
	}
	return g_pApp;
}

void TerminateApplication()
{
	delete g_pApp;
}