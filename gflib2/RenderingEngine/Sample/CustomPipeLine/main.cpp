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
	math::Matrix44																						m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	static const u32																					ModelMax = 3;

	scenegraph::resource::ResourceNode												*m_pModelResourceRootNode[ModelMax];
	scenegraph::resource::ResourceNode												*m_pEnvResourceRootNode;

	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[ModelMax];
	MyRenderingPipeLine																				*m_RenderingPipeLine;
	math::Vector																							m_ScrenaSize;
	scenegraph::instance::DrawEnvNode													*m_pDrawEnvNode;
	scenegraph::instance::LightSetNode												*m_pLightSetNode;
	scenegraph::instance::LightNode														*m_pDirectionalLightNode;
	scenegraph::instance::LightNode														*m_pAmbientLightNode;

	s32																												m_FrameCnt;
	u64																												m_PerformanceTime;

	InputDevice::StickPos																			m_Rot;
	math::Vector																							m_ModelRot;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0),
		m_PerformanceTime( 0 ),
		m_pDrawEnvNode( NULL )
	{
		for( s32 i = 0; i < ModelMax; ++i )
		{
			m_pModelResourceRootNode[i] = NULL;
			m_pModelInstanceNode[i] = NULL;
		}
		m_Rot.x = m_Rot.y = 0.0f;

		m_ScrenaSize.Set( 640, 480, 0 );
	}

	virtual ~SimpleDraw(){}

	void CalcFrustumPlanes( math::Matrix *pCullingMatrix, const math::Matrix &viewProjMatrix )
	{
		float m[16];

		for( int i=0; i<16; ++i ) {
			m[i] = viewProjMatrix.GetElem( i/4, i%4 );
		}

		pCullingMatrix[0][0].Set( math::Vector( m[3]-m[0], m[7]-m[4], m[11]-m[8], m[15]-m[12] ).Normalize() );
		pCullingMatrix[0][1].Set( math::Vector( m[3]+m[0], m[7]+m[4], m[11]+m[8], m[15]+m[12] ).Normalize() );
		pCullingMatrix[0][2].Set( math::Vector( m[3]+m[1], m[7]+m[5], m[11]+m[9], m[15]+m[13] ).Normalize() );
		pCullingMatrix[0][3].Set( math::Vector::GetZero() );
		pCullingMatrix[1][0].Set( math::Vector( m[3]-m[1], m[7]-m[5], m[11]-m[9], m[15]-m[13] ).Normalize() );
		pCullingMatrix[1][1].Set( math::Vector( m[3]-m[2], m[7]-m[6], m[11]-m[10], m[15]-m[14] ).Normalize() );
		pCullingMatrix[1][2].Set( math::Vector( m[3]+m[2], m[7]+m[6], m[11]+m[10], m[15]+m[14] ).Normalize() );
		pCullingMatrix[1][3].Set( math::Vector::GetZero() );
		pCullingMatrix[0] = pCullingMatrix[0].Transpose();
		pCullingMatrix[1] = pCullingMatrix[1].Transpose();
		pCullingMatrix[2] = pCullingMatrix[0].AbsPerElem();
		pCullingMatrix[3] = pCullingMatrix[1].AbsPerElem();
	}

	b32 IsIntersectFrustum( const math::Matrix *pCullingMatrices, const math::AABB& rAABB )
	{
		math::Vector c = rAABB.GetCenter();
		math::Vector d = rAABB.GetMax() - c;

		math::Vector c0 = pCullingMatrices[0].TransformCoord(c);
		math::Vector c1 = pCullingMatrices[1].TransformCoord(c);
		math::Vector d0 = pCullingMatrices[2].TransformNormal(d);
		math::Vector d1 = pCullingMatrices[3].TransformNormal(d);

		c = c0 + d0;
		d = c1 + d1;

		return !c.MinPerElem(d).IsAnyMinus();
	}

	virtual bool OnInit( void )
	{
		InputDevice::Pad::Initialize();

		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( m_ScrenaSize.x, m_ScrenaSize.y, false, Format::A8R8G8B8, Format::D24S8, MultiSample::None );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::InitDescription			desc(1024 * 2);
			desc.m_AABBTreeSize.SetMinMax( math::Vector(-1000.0f, -1000.0f, -1000.0f), math::Vector(1000.0f, 1000.0f, 1000.0f) );
			desc.m_AABBDepth = 4;
			scenegraph::SceneGraphManager::Initialize( desc );

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine( m_ScrenaSize.x, m_ScrenaSize.y );
		}

		for( s32 i = 0; i < ModelMax; ++i )
		{
			static const char*			s_pFilePath[ModelMax] = {
				"../data/EnvTestMap/DrawEnvTest.gfmdl",
				//"../data/tr0001_00_hero/old/tr0001_00.gfmdl",
				//"../data/tr0001_00_hero/new/tr0001_00.gfmdl"
				"../data/pm0025_00/pm0025_00.gfmdl",
				//"../data/pm0092_00_ghos/pm0092_00.mb.gfmdl",
				"../data/pm0094/pm0094_51.mb.gfmdl",
			};

			scenegraph::resource::GfMdlData							resourceData;
			resourceData.ReadFile( s_pFilePath[i] );

			m_pModelResourceRootNode[i] = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pModelInstanceNode[i] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelResourceRootNode[i] );
		}

		{
			if ( m_pModelInstanceNode[0] )
				m_pModelInstanceNode[0]->SetLocalTranslate(math::Vector3(0.0f, -139.0f, -546.0f));

			if ( m_pModelInstanceNode[1] )
			{
				math::Quaternion rot1;
				rot1.MatrixToQuaternion(m_World);
				//m_pModelInstanceNode[1]->SetLocalScale(math::Vector(0.3f, 0.3f, 0.3f));
				m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(100.0f, 0.0f, 0.0f));
			}

      m_pModelInstanceNode[2]->SetLocalScale(math::Vector3(0.3f, 0.3f, 0.3f));
      m_pModelInstanceNode[2]->SetLocalTranslate(math::Vector3(-100.0f, 0.0f, 0.0f));
		}

		{
			scenegraph::resource::GfEnvData							resourceData;
			resourceData.ReadFile( "../data/tr0001_00_hero/new/tr0001_00.gfenv" );
			//resourceData.ReadFile( "../data/EnvTestMap/DrawEnvTest.gfenv" );

			m_pEnvResourceRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
			m_pDrawEnvNode = scenegraph::instance::InstanceCreator::CreateDrawEnvNode( m_pEnvResourceRootNode );

			scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
			pCamera->SetLocalTranslate( math::Vector3( 0, 20, 150 ) );
		}

		m_RenderingPipeLine->AddDrawEnv( m_pDrawEnvNode );

		if ( m_pModelInstanceNode[1] )
			m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[1] );
		m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[2] );

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
			math::Vector eyePosition( 0.0f, 50.0f, 80.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 50.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);
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

			math::Matrix34													rotYMat = math::Matrix34::GetRotationY( -m_Rot.x / 20.0f );
			math::Matrix34													rotXMat = math::Matrix34::GetRotationX( -m_Rot.y / 20.0f );
			math::Vector3													  camPos = pCamera->GetLocalTranslate();

			math::Matrix34 mat = rotYMat * rotXMat;

      math::Vector3 axisX = mat.GetElemAxisX();
      math::Vector3 axisZ = mat.GetElemAxisZ();
      camPos += axisX * (pos.x * 2.0f);
      camPos += axisZ * (pos.y * 2.0f);

      pCamera->SetLocalTranslate(camPos);
      math::Quaternion rotQuat;
      rotQuat.MatrixToQuaternion(mat);
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

			math::Matrix			mat;
			math::Matrix			scaleMat = math::Matrix::GetScaling( math::Vector( 0.3f, 0.3f, 0.3f ) );
			math::Matrix			rotYMat = math::Matrix::GetRotationY( m_ModelRot.y );

			mat = scaleMat * rotYMat;

			if ( m_pModelInstanceNode[1] )
			{
				math::Quaternion rot1;
				rot1.MatrixToQuaternion(rotYMat);
				//m_pModelInstanceNode[1]->SetLocalScale(math::Vector(0.3f, 0.3f, 0.3f));
				m_pModelInstanceNode[1]->SetLocalTranslate(math::Vector3(-50.0f, 0.0f, 0.0f));
				m_pModelInstanceNode[1]->SetLocalRotation(rot1);
			}

      math::Quaternion rot2;
      rot2.MatrixToQuaternion(rotYMat);
      m_pModelInstanceNode[2]->SetLocalScale(math::Vector3(0.3f, 0.3f, 0.3f));
      m_pModelInstanceNode[2]->SetLocalTranslate(math::Vector3(50.0f, 0.0f, 0.0f));
      m_pModelInstanceNode[2]->SetLocalRotation(rot2);
		}

		{
			scenegraph::instance::LightSetNode			*pLightSetNode = m_pDrawEnvNode->GetLightSet( 0 );

			for( u32 i = 0; i < pLightSetNode->GetLightNum(); ++i )
			{
				scenegraph::instance::LightNode			*pLightNode = const_cast<scenegraph::instance::LightNode*>( pLightSetNode->GetLightNode( i ) );

				if ( pLightNode->GetLightType() != LightType::Directional )
					continue;

				math::Vector3			dir( 0, 0, 1 );
				math::Matrix34		rotYMat = math::Matrix34::GetIdentity();
				math::Matrix34		rotXMat = math::Matrix34::GetIdentity();

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

		{
			scenegraph::instance::CameraNode*			pCamera = m_pDrawEnvNode->GetCamera( 0 );
			math::Matrix44												viewProjMat44 = m_Projection * pCamera->GetViewMatrix();
			math::AABB														aabb = m_pModelInstanceNode[1]->GetWorldAABB();
			math::Matrix44												cullingMatrix[4];

			math::AABB::Util::CalcFrustumPlanes( cullingMatrix, viewProjMat44 );

			if ( math::AABB::Util::IsIntersectFrustum( cullingMatrix, aabb ) )
			{
				::OutputDebugStringA( "In\n" );
			}
			else
			{
				::OutputDebugStringA( "Out\n" );
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
		if ( m_pDrawEnvNode )
			delete m_pDrawEnvNode;
		//delete m_pLightSetNode;//DrawEnvNodeの子となっているので開放済み
		//delete m_pDirectionalLightNode;//LightSetNodeの子となっているので開放済み
		//delete m_pAmbientLightNode;//LightSetNodeの子となっているので開放済み

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

#if 0
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>

#include <RenderingEngine/Sample/Framework/IApplication.h>

#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "MyRenderingPipeLine.h"

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;

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
	static const u32																					MODEL_MAX = 2;
	math::Matrix																							m_Projection;   // 透視変換行列
	math::Matrix																							m_View;  // ビュー変換行列
	math::Matrix																							m_World;      // ワールド変換行列

	scenegraph::resource::ResourceNode												*m_pRootNode;
	scenegraph::instance::ModelInstanceNode										*m_pModelInstanceNode[MODEL_MAX];
	MyRenderingPipeLine																				*m_RenderingPipeLine;
	math::Vector																							m_ScrenaSize;

	s32																												m_FrameCnt;

	SimpleDraw() : 
		Sample::Framework::IApplication(),
		m_FrameCnt(0)
	{
		m_ScrenaSize.Set( 640, 480, 0 );
	}

	virtual ~SimpleDraw(){}

	virtual bool OnInit( void )
	{
		GFGL::Initialize( &m_Allocator );

		m_Handle = GFGL::CreateDisplay( m_ScrenaSize.x, m_ScrenaSize.y, false, Format::A8R8G8B8, Format::D24S8, MultiSample::MSAA4X );

		{//レンダリングエンジンの初期化
			//全ノードを管理するマネージャ初期化。機能を使わないのであれば必須でない。
			scenegraph::SceneGraphManager::Initialize();

			//リソースノードを生成するマネージャ初期化
			scenegraph::resource::ResourceManager::Initialize();
		
			//描画パイプラインを作成
			m_RenderingPipeLine = new MyRenderingPipeLine( m_ScrenaSize.x, m_ScrenaSize.y );
		}

		{
			s32					size;
			c8*					pGfmdlData;

			//リソースローダー経由でノードは生成されます。
			scenegraph::resource::GfMdlData							resourceData;
			//resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0579_00_lanculus/pm0579_00.gfmdl" );
			resourceData.ReadFile( "C:/home/niji/git_gflib2/RenderingEngine/Sample/data/pm0025_00/pm0025_00.gfmdl" );
			//m_pRootNodeには、様々なリソースがぶら下がっています。
			m_pRootNode = scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
		}

		//同じリソースノードからインスタンスノードを２つ生成
		m_pModelInstanceNode[0] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );
		m_pModelInstanceNode[1] = scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pRootNode );

		{
			// 射影変換行列(パースペクティブ(透視法)射影)
			m_Projection = math::Matrix::GetPerspective(
					math::ConvDegToRad(37.0f),		// 視野角30°
					m_ScrenaSize.x / m_ScrenaSize.y,	// アスペクト比
					1.1f,							// 前方投影面までの距離
					1000.0f);							// 後方投影面までの距離

			m_RenderingPipeLine->SetProjectionMatrix( m_Projection );
		}

		m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[0] );
		m_RenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode[1] );

		return true;
	}

	virtual bool OnUpdate( void )
	{
		{
			// ビュー変換行列
			math::Vector eyePosition( 0.0f, 20.0f, 200.0f, 1.0f );  // 視点(カメラの位置)
			math::Vector focusPosition( 0.0f, 20.0f,  0.0f, 1.0f );  // 注視点
			math::Vector upDirection( 0.0f, 1.0f,  0.0f, 1.0f );  // カメラの上方向

			m_View = math::Matrix::GetLookAt(eyePosition, focusPosition, upDirection);

			m_RenderingPipeLine->SetViewMatrix( m_View );
		}

		{
			math::Matrix		matY, matX;
			f32							rotate;

			rotate = (f32)(PI * (m_FrameCnt % 300)) / 150.0f;
			matY = math::Matrix::GetRotationY(rotate);
			rotate = (f32)(PI * (m_FrameCnt % 150)) / 75.0f;
			matX = math::Matrix::GetRotationX(rotate);

			//m_World = matY * matX;
			m_World = matY;
		}

#if 0
		{// モデルにワールド座標を直接指定する場合。
			m_World[3].Set( -50.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[0]->SetWorldMatrix( m_World );

			m_World[3].Set( 50.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[1]->SetWorldMatrix( m_World );
		}
#else
		{//トラバース処理にてワールド座標を更新する場合。

			//地球の上に、流れる川があって、その上に船があって、その上に、積み荷があって、その上に人が乗っている。
			//といった表現はこちらを用いるとよいです。

			//TransformNode用ビジターを作成
			class TransformNodeVisitor
			{
			public:
				TransformNodeVisitor(){}
				virtual ~TransformNodeVisitor() {}

				void Visit( scenegraph::instance::TransformNode* pNode )
				{
				#if defined(_WINDOWS)
					//::OutputDebugStringA( pNode->GetNodeName() );
					//::OutputDebugStringA( "\n" );
				#endif

					scenegraph::instance::TransformNode			*pParent = pNode->GetParent() ? pNode->GetParent()->SafeCast<scenegraph::instance::TransformNode>() : NULL;

					if ( pParent == NULL )
					{
						pNode->SetWorldMatrix( pNode->GetLocalMatrix() );
						return;
					}

					math::Matrix														world = pParent->GetWorldMatrix() * pNode->GetLocalMatrix();

					pNode->SetWorldMatrix( world );
				}
			};

			m_World[3].Set( 0.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[0]->SetLocalMatrix( m_World );

			m_World[3].Set( 0.0f, 0.0f, 0.0f, 1.0f );
			m_pModelInstanceNode[1]->SetLocalMatrix( m_World );

			scenegraph::Traverser::Visitor<scenegraph::instance::TransformNode, TransformNodeVisitor>				visitor;			

			//TransformNodeに対しトラバース処理
			scenegraph::SceneGraphManager::Traverse<scenegraph::instance::TransformNode, TransformNodeVisitor>( &visitor );
		}
#endif

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

		return true;
	}

	virtual bool OnTerm( void )
	{
		delete m_RenderingPipeLine;

		delete m_pRootNode;
		delete m_pModelInstanceNode[0];
		delete m_pModelInstanceNode[1];

		scenegraph::resource::ResourceManager::Terminate();

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
#endif