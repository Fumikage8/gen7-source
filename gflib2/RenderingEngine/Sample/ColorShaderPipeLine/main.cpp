#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>


#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>
#include <system/include/InputDevice/gfl2_Pad.h>
// カラーシェーダ用テクスチャ作成に必要
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include "MyRenderingPipeLine.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::system;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;

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

class SimpleDraw : public Sample::Framework::IApplication
{
public:
	math::Matrix44																							m_Projection;   // 透視変換行列
	math::Matrix34																							m_View;  // ビュー変換行列
	math::Matrix34																							m_World;      // ワールド変換行列

	static const u32																					ModelMax = 1;

	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode[ModelMax];
	scenegraph::resource::ResourceNode												*m_pEnvResourceRootNode;

	scenegraph::resource::ResourceNode												*m_ColorShaderRenderTargetPlaneResource;
	scenegraph::instance::ModelInstanceNode										*m_ColorShaderRenderTargetPlane;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[ModelMax];
	MyRenderingPipeLine																				*m_RenderingPipeLine;
	math::Vector3																							m_ScrenaSize;
	scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	scenegraph::instance::LightSetNode												*m_pLightSetNode;
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	scenegraph::instance::LightNode														*m_pAmbientLightNode;

	s32																												m_FrameCnt;
	u64																												m_PerformanceTime;

	InputDevice::StickPos																			m_Rot;
	math::Vector3																							m_ModelRot;

  //<! @brief ポケモン一体あたりの最大マテリアル数16 + パーティクルマテリアル1つ
  static const s32 MATERIAL_MAX = 16 + 1;
  //<! @brief カラーテクスチャの枚数は1マテリアルにつき3つまで
  static const s32 COLORTEXTURE_MAX = MATERIAL_MAX * gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureMax;
  const renderingengine::scenegraph::resource::TextureResourceNode* m_OriginalColorTextureResourceNodes[COLORTEXTURE_MAX];
  renderingengine::scenegraph::resource::TextureResourceNode* m_ColorShaderTextureResourceNodes[COLORTEXTURE_MAX];
  u32 m_ColorShaderTextureNum;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_PerformanceTime( 0 ),
		m_pDrawEnvNode( NULL ),
    m_ColorShaderTextureNum(0)
	{
		for( s32 i = 0; i < ModelMax; ++i )
		{
			m_pModelResourceRootNode[i] = NULL;
			m_pModelInstanceNode[i] = NULL;
		}
		m_Rot.x = m_Rot.y = 0.0f;

		m_ScrenaSize.Set( 640, 480, 0 );

    for (s32 i = 0; i < COLORTEXTURE_MAX; ++i)
    {
      m_OriginalColorTextureResourceNodes[i] = NULL;
      m_ColorShaderTextureResourceNodes[i] = NULL;
    }
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		InputDevice::Pad::Initialize();

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( 
      static_cast<u32>(m_ScrenaSize.x),
      static_cast<u32>(m_ScrenaSize.y),
      false,
      Format::A8R8G8B8,
      Format::D24S8,
      MultiSample::None );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
			desc.m_AABBTreeSize.SetMinMax( math::Vector(-500.0f, -500.0f, -500.0f), math::Vector(500.0f, 500.0f, 500.0f) );
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine();
		}

		{
			static const char*			s_pFilePath[ModelMax] = {
				"../data/pm0025_00/pm0025_00.gfmdl",
			};

			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( s_pFilePath[0] );

			m_pModelResourceRootNode[0] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pModelInstanceNode[0] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[0] );
      m_pModelInstanceNode[0]->SetLocalScale(math::Vector3(0.3f, 0.3f, 0.3f));
      m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(0.0f, 0.0f, 0.0f));

      scenegraph::resource::GfMdlData							resourceDataPlane;
      resourceDataPlane.ReadFile("../data/ColorShaderGfmdl/colorshaderplane.gfmdl");
      m_ColorShaderRenderTargetPlaneResource = scenegraph::resource::ResourceManager::CreateResourceNode(&resourceDataPlane);
      m_ColorShaderRenderTargetPlane = scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_ColorShaderRenderTargetPlaneResource);
      // カラーシェーダ変更用モデルをシーングラフから外す
      scenegraph::SceneGraphManager::RemoveChild(m_ColorShaderRenderTargetPlane);
      m_RenderingPipeLine->SetColorShaderRenderTargetPlane(m_ColorShaderRenderTargetPlane);

      // 各種カラーシェーダ用テクスチャの作成
      u32 materialNum = m_pModelInstanceNode[0]->GetMaterialNum();
      for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
      {
        renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode;
        pMaterialInstanceNode = m_pModelInstanceNode[0]->GetMaterialInstanceNode(materialNo);
        const renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam& rAttributeParam = *pMaterialInstanceNode->GetAttributeParam();
        const renderingengine::scenegraph::resource::MaterialResourceNode::UserData& rUserData = pMaterialInstanceNode->GetMaterialResourceNode()->GetUserData();

        u32 textureNum = pMaterialInstanceNode->GetTextureNum();
        if (textureNum == 0)
        {
          break;
        }

        for (u32 textureNo = 0; textureNo <= renderingengine::scenegraph::resource::CombinerAttribute::TextureNode::Texture2; ++textureNo)
        {
          renderingengine::scenegraph::resource::TextureBakeType::Enum bakeType = 
            static_cast<renderingengine::scenegraph::resource::TextureBakeType::Enum>(rUserData.m_BakeTextures[textureNo]);
          if (bakeType != renderingengine::scenegraph::resource::TextureBakeType::AtAll)
          {// "AtAll"以外は未使用なので無視
            continue;
          }

          const renderingengine::scenegraph::resource::TextureResourceNode* pOriginalTextureResourceNode;
          pOriginalTextureResourceNode = pMaterialInstanceNode->GetTextureResourceNode(textureNo);
          const gfx::Texture* pTexture = pOriginalTextureResourceNode->GetTexture();
          if (!pTexture)
          {
            continue;
          }
          const gfx::Texture::TextureDesc& desc = pTexture->GetTextureDesc();
          // カラーシェーダ用テクスチャを, 元のテクスチャの大きさに合わせて作成, それを元にノードを作成して差し替え
          gfx::Texture* pColorShaderTexture = gfx::GFGL::CreateTexture(
            desc.width,
            desc.height,
            desc.mipLevels,
            gfl2::gfx::Usage::RenderTarget,  // テクスチャはレンダーターゲットとして使う.
            desc.format,
            desc.pool);
          m_OriginalColorTextureResourceNodes[m_ColorShaderTextureNum] = pOriginalTextureResourceNode;
          renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
              renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
              renderingengine::scenegraph::resource::IResourceData::Type("texture")
              );

          renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
            reinterpret_cast<renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

          m_ColorShaderTextureResourceNodes[m_ColorShaderTextureNum] = 
            reinterpret_cast<renderingengine::scenegraph::resource::TextureResourceNode*>(
            pTextureResourceNodeFactory->CreateResourceNode(
              pOriginalTextureResourceNode->GetNodeName(),
              pColorShaderTexture)
            );

          // カラーシェーダ用にテクスチャを差し替え
          pMaterialInstanceNode->SetTextureResourceNode(textureNo, m_ColorShaderTextureResourceNodes[m_ColorShaderTextureNum]);

          // パイプラインにテクスチャを追加
          m_RenderingPipeLine->AddColorShaderTexture(pTexture, pColorShaderTexture);

          ++m_ColorShaderTextureNum;
        }
      }

      // 描画する色を指定
      m_RenderingPipeLine->SetColorShaderTextureColor(0, m_ColorShaderTextureNum, math::Vector(0.0f, 0.5f, 1.0f, 0.5f));

		}

		{
			scenegraph::resource::GfEnvData							resourceData;
			resourceData.ReadFile( "../data/tr0001_00_hero/new/tr0001_00.gfenv" );

			m_pEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pEnvResourceRootNode );

			scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
			pCamera->SetLocalTranslate( math::Vector3( 0, 20, 150 ) );
		}

		m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix44::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					m_ScrenaSize.x / m_ScrenaSize.y,	// アスペクト比
					0.1f,							// 前方投影面までの距離
					10000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		{
			// ビュー変換行列
			math::Vector3 eyePosition( 0.0f, 50.0f, 80.0f );  // 視点(カメラの位置)
			math::Vector3 focusPosition( 0.0f, 50.0f,  0.0f );  // 注視点
			math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向

			m_View = math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
		}

		return true;
	}

	virtual bool OnUpdate( void )
	{
		InputDevice::Pad::Update();

		{
			scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
			const InputDevice::StickPos&					pos = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Left );
			const InputDevice::StickPos&					rot = InputDevice::Pad::GetStick( 0, InputDevice::Stick::Right );

			m_Rot.x += rot.x;
			m_Rot.y += rot.y;

			math::Matrix44													rotYMat = math::Matrix44::GetRotationY( -m_Rot.x / 20.0f );
			math::Matrix44													rotXMat = math::Matrix44::GetRotationX( -m_Rot.y / 20.0f );
			math::Vector3													camPos = pCamera->GetLocalTranslate();

			math::Matrix44 mat = rotYMat * rotXMat;

			mat[3] = camPos;
			mat[3] += mat[0] * (pos.x * 2.0f);
			mat[3] += mat[2] * (pos.y * 2.0f);

      pCamera->SetLocalTranslate(math::Vector3(mat[3].x, mat[3].y, mat[3].z));
      math::Quaternion rotQuat;
      math::Matrix34 mat34;
      mat34[0] = mat[0];
      mat34[1] = mat[1];
      mat34[2] = mat[2];
      rotQuat.MatrixToQuaternion(mat34);
      pCamera->SetLocalRotation(rotQuat);

			m_RenderingPipeLine->SetViewMatrix( pCamera->GetViewMatrix() );	
		}

		{
			if( InputDevice::Pad::IsPress( 0, InputDevice::Button::R2 ) )
			{
				m_ModelRot.y += math::ConvDegToRad(1.0f);
			}
			else if( InputDevice::Pad::IsPress( 0, InputDevice::Button::L2 ) )
			{
				m_ModelRot.y -= math::ConvDegToRad(1.0f);
			}

			math::Matrix34			mat;
			math::Matrix34			scaleMat = math::Matrix34::GetScaling( math::Vector3( 1.0f, 1.0f, 1.0f ) );
			math::Matrix34			rotYMat = math::Matrix34::GetRotationY( m_ModelRot.y );

			mat = scaleMat * rotYMat;

      math::Quaternion rot2;
      rot2.MatrixToQuaternion(rotYMat);
      m_pModelInstanceNode[0]->SetLocalScale(math::Vector3(1.0f, 1.0f, 1.0f));
      m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(0.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[0]->SetLocalRotation(rot2);
		}

		{
			scenegraph::instance::LightSetNode			*pLightSetNode = m_pDrawEnvNode->GetLightSet( 0 );

			for( u32 i = 0; i < pLightSetNode->GetLightNum(); ++i )
			{
				scenegraph::instance::LightNode			*pLightNode = const_cast<scenegraph::instance::LightNode*>( pLightSetNode->GetLightNode( i ) );

				if ( pLightNode->GetLightType() != LightType::Directional )
					continue;

				math::Vector3			dir( 0, 0, 1 );
				math::Matrix34			rotYMat = math::Matrix34::GetIdentity();
				math::Matrix34			rotXMat = math::Matrix34::GetIdentity();

				if ( GetAsyncKeyState(VK_NUMPAD7) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(70.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD8) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(70.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD9) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(70.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(-45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}

				if ( GetAsyncKeyState(VK_NUMPAD4) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(50.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD5) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(50.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD6) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(50.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(-45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}

				if ( GetAsyncKeyState(VK_NUMPAD1) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(30.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD2) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(30.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				else if ( GetAsyncKeyState(VK_NUMPAD3) & 0x8000 )
				{
					rotXMat = math::Matrix34::GetRotationX( math::ConvDegToRad(30.0f) );
					rotYMat = math::Matrix34::GetRotationY( math::ConvDegToRad(-45.0f) );
					dir = rotYMat * rotXMat * dir;
					dir.z *= -1;
					pLightNode->SetDirection( dir );
				}
				
			}
		}

		{

			{
				u64				start = Timer::PerformanceCounter::GetMicroSecond();
        scenegraph::SceneGraphManager::TraverseModel();
				u64				end = Timer::PerformanceCounter::GetMicroSecond();

				m_PerformanceTime += end - start;
			}
		}

		if ( m_FrameCnt % 60 == 0 )
		{
			char		buff[32];
			memset(buff, 0, sizeof(buff));
			sprintf_s( buff, sizeof(buff), "%d\n", static_cast<s32>(m_PerformanceTime / 60) );
			::OutputDebugStringA( buff );
			m_PerformanceTime = 0;
		}

		++m_FrameCnt;

		return true;
	}

	virtual bool OnDraw( void )
	{
		GFGL::SetActiveDisplay( m_Handle );

		Surface*			pBackBuffer = GFGL::GetBackBuffer( m_Handle );
		Surface*			pDepthBuffer = GFGL::GetDepthStencilBuffer( m_Handle );

		m_RenderingPipeLine->SetRenderTarget( pBackBuffer );
		m_RenderingPipeLine->SetDepthStencil( pDepthBuffer );

		m_RenderingPipeLine->Execute();

		{//! @note フラグの一斉クリアは後日用意
			class TransformNodeVisitor
			{
			public:
				TransformNodeVisitor(){}
				virtual ~TransformNodeVisitor() {}

				void Visit( scenegraph::instance::TransformNode* pNode )
				{
					pNode->SetTransformDirty( false );
				}
			};

			scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitor>				visitor;
			scenegraph::SceneGraphManager::Traverse<scenegraph::instance::TransformNode, TransformNodeVisitor>( &visitor );
		}

		return true;
	}

	virtual bool OnTerm( void )
	{
    if (m_ColorShaderRenderTargetPlane)
    {
      delete m_ColorShaderRenderTargetPlane;
      m_ColorShaderRenderTargetPlane = NULL;
    }
		if ( m_pDrawEnvNode )
    {
			delete m_pDrawEnvNode;
		//delete m_pLightSetNode;//DrawEnvNodeの子となっているので開放済み
		//delete m_pDirectionalLightNode;//LightSetNodeの子となっているので開放済み
		//delete m_pAmbientLightNode;//LightSetNodeの子となっているので開放済み
    }

		delete m_RenderingPipeLine;
		delete m_pEnvResourceRootNode;
		
		for( s32 i = 0; i < ModelMax; ++i )
		{
			if ( m_pModelResourceRootNode[i] )
				delete m_pModelResourceRootNode[i];
			if ( m_pModelInstanceNode[i] )
				delete m_pModelInstanceNode[i];
		}

		scenegraph::resource::ResourceManager::Terminate();
		scenegraph::SceneGraphManager::Terminate();

		GFGL::Finish();

		GFGL::SetActiveDisplay(NULL);
		GFGL::DestroyDisplay(m_Handle);
		GFGL::Terminate();

		InputDevice::Pad::Terminate();

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
