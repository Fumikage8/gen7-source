#include <clr/include/RenderingEngine/Renderer/Util/gfl2_ViewerRenderingPipeLineNative_CLR.h>
#include <clr/include/gfx/gfl2_GFGL_CLR.h>

#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <gfx/include/gfl2_GFGL.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>
#include <Renderingengine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

struct LocalFunc{
	static void *ReadFile( const char* pFilePath )
  {//自前データロード
#if defined(GF_PLATFORM_CTR)
    nn::fs::FileReader file( pFilePath );
    u32 fileSize = file.GetSize();
    void* pShaderBinary = GLMemory::Malloc(fileSize, 128);
    file.Read(pShaderBinary, fileSize);
		return pShaderBinary;
#else
		FILE		*hFile;
		int			file_size;
		char		*read_buff;

		fopen_s( &hFile, pFilePath, "rb" );

		if ( hFile == NULL ){
			assert(0);
			return NULL;
		}

		fseek( hFile, 0, SEEK_END );
		file_size = ftell( hFile );
		fseek( hFile, 0, SEEK_SET );

		u32		align = 16;
		file_size += align;//必ずNULLが入るようにしたい。
		u32 buf_size = file_size + ((-file_size) & (align-1));

    gfl2::gfx::GLMemory::SetInstanceAllocator(gfl2::clr::gfx::GFGL::GetGLAllocator());
		read_buff = reinterpret_cast<c8*>( gfl2::gfx::GLMemory::Malloc(buf_size) );
		memset( read_buff, 0, buf_size );
		fread( read_buff, file_size, 1, hFile );
		fclose( hFile );

		return read_buff;
#endif
  }
};

namespace gfl2 { namespace renderingengine { namespace renderer { namespace util {

/**
 * @brief ワイヤーフレームシェーダドライバ
 */
WireFrameShaderDriver::WireFrameShaderDriver()
{
  static const u8 s_wirecolor_gffsh[] = {
    #include "../../../../resource/wirecolor.gffsh.ascii"
    0x00
  };
  
  m_pPixelShader = gfx::GFGL::CreatePixelShaderFromSource(gfl2::clr::gfx::GFGL::GetGLAllocator(), reinterpret_cast<const c8*>(s_wirecolor_gffsh), "PS", NULL);

  m_DepthStencilStateObject = gfl2::gfx::GFGL::CreateDepthStencilStateObject(gfl2::clr::gfx::GFGL::GetGLAllocator());
  m_DepthStencilStateObject->SetDepthTestEnable(true);
  m_DepthStencilStateObject->SetDepthFunc(gfl2::gfx::CompareFunc::Less);
  m_DepthStencilStateObject->UpdateState();

  m_RasterizerStateObject = gfl2::gfx::GFGL::CreateRasterizerStateObject(gfl2::clr::gfx::GFGL::GetGLAllocator());
  m_RasterizerStateObject->SetCullMode( gfl2::gfx::CullMode::None );
  m_RasterizerStateObject->SetFillMode( gfl2::gfx::FillMode::Line );
  m_RasterizerStateObject->UpdateState();
}

/**
 * @brief デストラクタ
 */
WireFrameShaderDriver::~WireFrameShaderDriver()
{
  if (m_pPixelShader)
  {
    delete m_pPixelShader;
    m_pPixelShader = NULL;
  }

  if (m_DepthStencilStateObject)
  {
    delete m_DepthStencilStateObject;
    m_DepthStencilStateObject = NULL;
  }

  if (m_RasterizerStateObject)
  {
    delete m_RasterizerStateObject;
    m_RasterizerStateObject = NULL;
  }
}

/**
 *
 */
b32 WireFrameShaderDriver::Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const
{
  pMeshDrawTag->GetShaderDriver()->Apply(rRenderState, pMeshDrawTag);
  gfl2::gfx::GFGL::SetDepthStencilState( m_DepthStencilStateObject );
  gfl2::gfx::GFGL::SetRasterizerState( m_RasterizerStateObject );

  gfl2::gfx::GFGL::SetPixelShader( m_pPixelShader );

  return true;
}

/**
 * @brief コンストラクタ
 */
WireFrameRenderPath::WireFrameRenderPath()
{

}

/**
 * @brief デストラクタ
 */
WireFrameRenderPath::~WireFrameRenderPath()
{
}

/**
 * @brief 上書きするシェーダの取得
 */
const gfl2::renderingengine::renderer::IShaderDriver* WireFrameRenderPath::ShaderOverride()
{
  return &m_WireFrameShaderDriver;
}

void WireFrameRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//  
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
ViewerJointRenderPath::ViewerJointRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL)
{
}

ViewerJointRenderPath::~ViewerJointRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
  }
}

void ViewerJointRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  gfl2::gfx::GLMemory::SetInstanceAllocator(pAllocator);

  // 初期化設定保存
  m_InitDescription = desc;

  // 頂点フォーマットリスト作成
  m_VertexFormatLimit = desc.maxJointCount * 22;
  m_pVertexFormatList = new gfl2::gfx::DrawUtil::VertexFormat[m_VertexFormatLimit];

  gfl2::gfx::GLMemory::ClearInstanceAllocator();
}

void ViewerJointRenderPath::SetRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
  gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void ViewerJointRenderPath::ClearRenderTargetOverride( const gfx::Surface *pRenderTarget, const gfx::Surface *pDepthStencil )
{
  // レンダーターゲットのクリアはしない
}

void ViewerJointRenderPath::Execute( const RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  
  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(stateData);

  // 描画開始
  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::ClearScissor();

  // 簡易描画開始
  gfl2::gfx::DrawUtil::BeginRender();

  // プロジェクション行列設定
  gfl2::gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  gfl2::gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());

  // JointInstanceNodeに対してのVisitor
  class JointInstanceNodeVisitor
  {
  public:
    JointInstanceNodeVisitor() {}
    virtual ~JointInstanceNodeVisitor() {}

    void Visit( scenegraph::instance::JointInstanceNode* pNode )
    {
      scenegraph::instance::JointInstanceNode* pParent = pNode->GetParent()->SafeCast<scenegraph::instance::JointInstanceNode>();
      if (pParent == NULL)
      {
        return;
      }

      scenegraph::instance::ModelInstanceNode* pModel = NULL;
      scenegraph::DagNode* pDagNode = pNode;
      while (pModel == NULL)
      {
        pDagNode = pDagNode->GetParent();
        pModel = pDagNode->SafeCast<scenegraph::instance::ModelInstanceNode>();
      }
      if (!pModel->IsVisible())
      {
        return;
      }

      // ノードの行列、親の行列を取得
      const math::Matrix34& nodeMatrix = pNode->GetWorldMatrix();
      const math::Matrix34& parentMatrix = pParent->GetWorldMatrix();

      // スケールが小さ過ぎるノードは描画しない
      static const f32 scale_limit = 0.001f;
      gfl2::math::Vector3 nodeScale = nodeMatrix.GetElemScaling();
      if ((nodeScale.x * nodeScale.x < scale_limit) || (nodeScale.y * nodeScale.y < scale_limit) || (nodeScale.z * nodeScale.z < scale_limit))
      {
        return;
      }

      gfl2::math::Vector3 nodePos = nodeMatrix.GetElemPosition();
      gfl2::math::Vector3 parentPos = parentMatrix.GetElemPosition();

      // 頂点数オーバー確認
      assert(vertexCount + 22 < vertexLimit);

      // 四角錘

      // 親のXYZ軸
      gfl2::math::Vector3 axisX = parentMatrix.GetElemAxisX();
      gfl2::math::Vector3 axisY = parentMatrix.GetElemAxisY();
      gfl2::math::Vector3 axisZ = parentMatrix.GetElemAxisZ();

      axisX *= desc->boneScale;
      axisY *= desc->boneScale;
      axisZ *= desc->boneScale;

      gfl2::math::Vector3 quadPos0 = parentPos + axisY;
      gfl2::math::Vector3 quadPos1 = parentPos + axisZ;
      gfl2::math::Vector3 quadPos2 = parentPos - axisY;
      gfl2::math::Vector3 quadPos3 = parentPos - axisZ;

      // 底辺の四角の部分
      gfl2::gfx::DrawUtil::VertexFormat& vertexQuad0 = vertexList[vertexCount++];
      vertexQuad0.pos.Set(quadPos0.x, quadPos0.y, quadPos0.z);
      vertexQuad0.color = desc->jointColor;

      gfl2::gfx::DrawUtil::VertexFormat& vertexQuad1 = vertexList[vertexCount++];
      vertexQuad1.pos.Set(quadPos1.x, quadPos1.y, quadPos1.z);
      vertexQuad1.color = desc->jointColor;

      vertexList[vertexCount++] = vertexQuad1;

      gfl2::gfx::DrawUtil::VertexFormat& vertexQuad2 = vertexList[vertexCount++];
      vertexQuad2.pos.Set(quadPos2.x, quadPos2.y, quadPos2.z);
      vertexQuad2.color = desc->jointColor;

      vertexList[vertexCount++] = vertexQuad2;

      gfl2::gfx::DrawUtil::VertexFormat& vertexQuad3 = vertexList[vertexCount++];
      vertexQuad3.pos.Set(quadPos3.x, quadPos3.y, quadPos3.z);
      vertexQuad3.color = desc->jointColor;

      vertexList[vertexCount++] = vertexQuad3;

      vertexList[vertexCount++] = vertexQuad0;

      // 子ノードへ繋ぐ
      gfl2::gfx::DrawUtil::VertexFormat& vertexNode = vertexList[vertexCount++];
      vertexNode.pos.Set(nodePos.x, nodePos.y, nodePos.z);
      vertexNode.color = desc->jointColor;

      vertexList[vertexCount++] = vertexQuad0;

      vertexList[vertexCount++] = vertexNode;
      vertexList[vertexCount++] = vertexQuad1;

      vertexList[vertexCount++] = vertexNode;
      vertexList[vertexCount++] = vertexQuad2;

      vertexList[vertexCount++] = vertexNode;
      vertexList[vertexCount++] = vertexQuad3;

      // ここから軸

      // 子のXYZ軸
      gfl2::math::Vector3 childAxisX = nodeMatrix.GetElemAxisX();
      gfl2::math::Vector3 childAxisY = nodeMatrix.GetElemAxisY();
      gfl2::math::Vector3 childAxisZ = nodeMatrix.GetElemAxisZ();

      childAxisX *= desc->xyzScale;
      childAxisY *= desc->xyzScale;
      childAxisZ *= desc->xyzScale;

      // X軸
      gfl2::gfx::DrawUtil::VertexFormat& vertexXBegin = vertexList[vertexCount++];
      gfl2::gfx::DrawUtil::VertexFormat& vertexXEnd = vertexList[vertexCount++];
      vertexXBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
      vertexXBegin.color.Set(1, 0, 0);
      math::Vector3 axisXPos = nodePos + childAxisX;
      vertexXEnd.pos.Set(axisXPos.x, axisXPos.y, axisXPos.z);
      vertexXEnd.color.Set(1, 0, 0);

      // Y軸
      gfl2::gfx::DrawUtil::VertexFormat& vertexYBegin = vertexList[vertexCount++];
      gfl2::gfx::DrawUtil::VertexFormat& vertexYEnd = vertexList[vertexCount++];
      vertexYBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
      vertexYBegin.color.Set(0, 1, 0);
      math::Vector3 axisYPos = nodePos + childAxisY;
      vertexYEnd.pos.Set(axisYPos.x, axisYPos.y, axisYPos.z);
      vertexYEnd.color.Set(0, 1, 0);

      // Z軸
      gfl2::gfx::DrawUtil::VertexFormat& vertexZBegin = vertexList[vertexCount++];
      gfl2::gfx::DrawUtil::VertexFormat& vertexZEnd = vertexList[vertexCount++];
      vertexZBegin.pos.Set(nodePos.x, nodePos.y, nodePos.z);
      vertexZBegin.color.Set(0, 0, 1);
      math::Vector3 axisZPos = nodePos + childAxisZ;
      vertexZEnd.pos.Set(axisZPos.x, axisZPos.y, axisZPos.z);
      vertexZEnd.color.Set(0, 0, 1);
   }

    InitDescription* desc;                // 初期化設定
    gfl2::gfx::DrawUtil::VertexFormat* vertexList;   // 頂点リスト
    u32 vertexLimit;                      // 頂点最大数

    u32 vertexCount;                      // 頂点数
  };
  
  // ノードを巡回して頂点リストを作成する
  scenegraph::Traverser::Visitor<scenegraph::instance::JointInstanceNode, JointInstanceNodeVisitor>				visitor;
  visitor.desc = &m_InitDescription;
  visitor.vertexList = m_pVertexFormatList;
  visitor.vertexLimit = m_VertexFormatLimit;
  visitor.vertexCount = 0;
  scenegraph::SceneGraphManager::Traverse<scenegraph::instance::JointInstanceNode, JointInstanceNodeVisitor>( &visitor );

  // 頂点リスト描画
  gfl2::gfx::DrawUtil::DrawLines(m_pVertexFormatList, visitor.vertexCount, false);

  // 描画終了
  gfl2::gfx::DrawUtil::EndRender();
  gfl2::gfx::GFGL::EndScene();
}

/**
 * @brief コンストラクタ
 */
GridRenderPath::GridRenderPath() :
  IRenderPath(),
  m_pVertexFormatList(NULL)
{
}

/**
 * @brief デストラクタ
 */
GridRenderPath::~GridRenderPath()
{
  if (m_pVertexFormatList != NULL)
  {
    delete[] m_pVertexFormatList;
    m_pVertexFormatList = NULL;
  }
}

void GridRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  gfl2::gfx::GLMemory::SetInstanceAllocator(pAllocator);

  // 初期化設定保存
  m_InitDescription = desc;

  const s32 lineNum = 8;
  const s32 vertexNum = 6 + 4 * (2 * lineNum + 1);
  u32 lineCnt = 0;
  f32 gridInterval = 8.0f;    // 実機ビューアでは4.0f間隔なのでそれに合わせる
  f32 vscale = gridInterval * (float)lineNum;

  m_pVertexFormatList = new gfl2::gfx::DrawUtil::VertexFormat[vertexNum];

  gfl2::gfx::GLMemory::ClearInstanceAllocator();

  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-vscale, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  for (int i = 1; i <= lineNum; ++i)
  {
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4( -vscale, 0.0f, i * gridInterval ), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4( 0.8f, 0.8f, 0.8f ));
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(  vscale, 0.0f, i * gridInterval ), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4( 0.8f, 0.8f, 0.8f ));
  }
  for (int i = 1; i <= lineNum; ++i)
  {
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-vscale, 0.0f, -i * gridInterval), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(vscale, 0.0f, -i * gridInterval), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  }

  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, -vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  for (int i = 1; i <= lineNum; ++i)
  {
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(i * gridInterval, 0.0f, -vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(i * gridInterval, 0.0f,  vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  }
  for (int i = 1; i <= lineNum; ++i)
  {
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-i * gridInterval, 0.0f, -vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
    m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-i * gridInterval, 0.0f, vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.8f, 0.8f, 0.8f));
  }

  // X Axis
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(1.0f, 0.0f, 0.0f));
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(vscale, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(1.0f, 0.0f, 0.0f));
  // Y Axis
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.0f, 1.0f, 0.0f));
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, +vscale, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.0f, 1.0f, 0.0f));
  // Z Axis
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, 0.0f), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.0f, 0.0f, 1.0f));
  m_pVertexFormatList[lineCnt++] = gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.0f, 0.0f, +vscale), gfl2::math::Vector4::GetZero(), gfl2::math::Vector4(0.0f, 0.0f, 1.0f));

  m_VertexFormatLimit = lineCnt;
}

void GridRenderPath::SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
  gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void GridRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットのクリアはしない
}

void GridRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  SetRenderTargetOverride(rDrawContext.m_pRenderTarget, rDrawContext.m_pDepthStencil);

  // シザー、カリング、フィルの設定
  gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);

  // 深度テストの設定
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = true;
  gfl2::gfx::DrawUtil::SetDepthTestState(stateData);

  // 描画開始
  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::ClearScissor();

  // 簡易描画開始
  gfl2::gfx::DrawUtil::BeginRender();

  // プロジェクション行列設定
  gfl2::gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  gfl2::gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());

  // 頂点リスト描画
  gfl2::gfx::DrawUtil::DrawLines(m_pVertexFormatList, m_VertexFormatLimit, false);
  //gfx::DrawUtil::DrawCircle(math::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 40.0f, math::Vector4(1.0f, 1.0f, 0.0f, 1.0f));
  //gfx::DrawUtil::DrawTube(math::Vector4(0.0f, 0.0f, 0.0f, 1.0f), 40.0f, 100.0f, math::Vector4(1.0f, 1.0f, 0.0f, 0.5f));

  // 簡易描画終了
  gfl2::gfx::DrawUtil::EndRender();

  gfl2::gfx::GFGL::EndScene();

  // 簡易描画の深度テストを元に戻す
  stateData.m_DepthTestEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(stateData);
}

/**
 * @brief コンストラクタ
 */
GaugeRenderPath::GaugeRenderPath() :
  IRenderPath(),
  m_IsGaugeVisible(true)
{
}

/**
 * @brief デストラクタ
 */
GaugeRenderPath::~GaugeRenderPath()
{
}

void GaugeRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  // 初期化設定保存
  m_InitDescription = desc;

  // FPS設定
  gfl2::gfx::GFGL::SetTargetFPS(29.92);
}

void GaugeRenderPath::SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
  gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void GaugeRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットのクリアはしない
}

void GaugeRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // 1秒経ったら平均値をゲージに反映
  ++m_InitDescription.frameCount;
  if (m_InitDescription.frameCount % FRAME_COUNT_RESOLUTION == 0)
  {
    m_InitDescription.cpuTimeAverage = 0;
    for (s32 i = 0; i < FRAME_COUNT_RESOLUTION; ++i)
    {
      m_InitDescription.cpuTimeAverage += m_InitDescription.cpuExecutionTime[i];
    }
    m_InitDescription.cpuTimeAverage /= FRAME_COUNT_RESOLUTION;

    m_InitDescription.gpuTimeAverage = 0;
    for (s32 i = 0; i < FRAME_COUNT_RESOLUTION; ++i)
    {
      m_InitDescription.gpuTimeAverage += m_InitDescription.gpuExecutionTime[i];
    }
    m_InitDescription.gpuTimeAverage /= FRAME_COUNT_RESOLUTION;
  }

  if (m_IsGaugeVisible)
  {
    // シザー、カリング、フィルの設定
    gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
    gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

    // マテリアル設定
    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);

    // 描画開始
    gfl2::gfx::GFGL::BeginScene();
    gfl2::gfx::DrawUtil::ClearScissor();

    // 簡易描画開始
    gfl2::gfx::DrawUtil::BeginRender();

    float resolution = 1000000.0f / static_cast<float>(30);
    f32 length;
    length = m_InitDescription.maxGaugeLength * (float)m_InitDescription.cpuTimeAverage / resolution;
    gfl2::gfx::DrawUtil::DrawRect(0.0f, 0.0f, static_cast<f32>(m_InitDescription.maxGaugeLength), 20.0f, m_InitDescription.cpuGaugeColor / 2.0f);
    gfl2::gfx::DrawUtil::DrawRect(0.0f, 0.0f, length, 20.0f, m_InitDescription.cpuGaugeColor);

    length = m_InitDescription.maxGaugeLength * (float)m_InitDescription.gpuTimeAverage / resolution;
    gfl2::gfx::DrawUtil::DrawRect(0.0f, 20.0f, static_cast<f32>(m_InitDescription.maxGaugeLength), 20.0f, m_InitDescription.gpuGaugeColor / 2.0f);
    gfl2::gfx::DrawUtil::DrawRect(0.0f, 20.0f, length, 20.0f, m_InitDescription.gpuGaugeColor);
  
    gfl2::gfx::DrawUtil::DrawRect(static_cast<f32>(m_InitDescription.maxGaugeLength), 0.0f, 5.0f, 40.0f, gfl2::math::Vector4(1.0f, 1.0f, 1.0f));

    // 簡易描画終了
    gfl2::gfx::DrawUtil::EndRender();

    gfl2::gfx::GFGL::EndScene();
  }
}

void GaugeRenderPath::SetCpuFrame(s32 frame)
{
  for (s32 i = 0; i < FRAME_COUNT_RESOLUTION - 1; ++i)
  {
    m_InitDescription.cpuExecutionTime[i + 1] = m_InitDescription.cpuExecutionTime[i];
  }
  m_InitDescription.cpuExecutionTime[0] = frame;
}

void GaugeRenderPath::SetGpuFrame(s32 frame)
{
  for (s32 i = 0; i < FRAME_COUNT_RESOLUTION - 1; ++i)
  {
    m_InitDescription.gpuExecutionTime[i + 1] = m_InitDescription.gpuExecutionTime[i];
  }
  m_InitDescription.gpuExecutionTime[0] = frame;
}

void GaugeRenderPath::ChangeVisibility(b32 enable)
{
  m_IsGaugeVisible = enable;
}

s32 GaugeRenderPath::GetCpuTimeAverage()
{
  return m_InitDescription.cpuTimeAverage;
}

s32 GaugeRenderPath::GetGpuTimeAverage()
{
  return m_InitDescription.gpuTimeAverage;
}





/**
 * @brief コンストラクタ
 */
DebugFigureRenderPath::DebugFigureRenderPath() :
  IRenderPath(),
  m_IsInnerTubeVisible(false),
  m_IsOuterTubeVisible(false),
  m_PokemonSize(gfl2::renderingengine::PokemonSize::S),
  m_IsFocusPointVisible(false),
  m_FocusPosition(gfl2::math::Vector4(0.0f, 0.0f, 0.0f)),
  m_IsInterestPositionVisible(false),
  m_InterestPosition(gfl2::math::Vector4(0.0f, 0.0f, 0.0f)),
  m_IsModelAABBVisible(false),
  m_ModelAABBColor(gfl2::math::Vector4(0.0f, 0.0f, 0.0f)),
  m_LightPositionVisible(false),
  m_LightPosition(gfl2::math::Vector4(0.0f, 0.0f, 0.0f))
{
}

/**
 * @brief デストラクタ
 */
DebugFigureRenderPath::~DebugFigureRenderPath()
{
}

void DebugFigureRenderPath::Initialize(gfl2::gfx::IGLAllocator* pAllocator, const InitDescription& desc)
{
  // 初期化設定保存
  m_InitDescription = desc;

  {// ポケモンサイズに合わせた半径の設定
    m_InnerTubeRadiuses[0] = 93.0f;
    m_InnerTubeRadiuses[1] = 130.0f;
    m_InnerTubeRadiuses[2] = 150.0f;
    m_OuterTubeRadiuses[0] = 100.0f;
    m_OuterTubeRadiuses[1] = 150.0f;
    m_OuterTubeRadiuses[2] = 190.0f;
  }
}

void DebugFigureRenderPath::SetRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットの設定
  gfl2::gfx::GFGL::SetRenderTarget( pRenderTarget );
  gfl2::gfx::GFGL::SetDepthStencil( pDepthStencil );
}

void DebugFigureRenderPath::ClearRenderTargetOverride( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil )
{
  // レンダーターゲットのクリアはしない
}

void DebugFigureRenderPath::Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  // シザー、カリング、フィルの設定
  gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
  gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);

  // マテリアル設定
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);

  // 簡易描画のデプステストを無しに設定する
  gfl2::gfx::DepthStencilStateObject::StateData stateData;
  stateData.m_DepthTestEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(stateData);

  // 描画開始
  gfl2::gfx::GFGL::BeginScene();
  gfl2::gfx::DrawUtil::ClearScissor();

  // 簡易描画開始
  gfl2::gfx::DrawUtil::BeginRender();

  // プロジェクション行列設定
  gfl2::gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
  // ビュー行列設定
  gfl2::gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
  // モデル行列設定
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());

  // カメラの注視点の表示
  if (m_IsFocusPointVisible)
  {
    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
    gfl2::math::Vector4 diff = gfl2::math::Vector4(1.5f, 1.5f, 1.5f);
    gfl2::math::Vector4 pos = gfl2::math::Vector4(m_FocusPosition);
    gfl2::gfx::DrawUtil::DrawAABB(pos - diff, pos + diff, gfl2::math::Vector4(0.9f, 0.9f, 1.0f));
  }

  // ライトの位置の表示
  if (m_LightPositionVisible)
  {
    gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
    gfl2::math::Vector4 diff = gfl2::math::Vector4(1.5f, 1.5f, 1.5f);
    gfl2::math::Vector4 pos = gfl2::math::Vector4(m_LightPosition);
    gfl2::gfx::DrawUtil::DrawAABB(pos - diff, pos + diff, gfl2::math::Vector4(1.0f, 1.0f, 0.1f));
  }
  
  //if (m_ModelDataManager != null)
  {
    // モデルデータの視線制御用の注目点の表示
    if (m_IsInterestPositionVisible)
    {
      gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
      gfl2::math::Vector4 diff = gfl2::math::Vector4(1.5f, 1.5f, 1.5f);
      gfl2::math::Vector4 pos = gfl2::math::Vector4(m_InterestPosition);
      gfl2::gfx::DrawUtil::DrawAABB(pos - diff, pos + diff, gfl2::math::Vector4(0.4f, 1.0f, 0.4f));
    }
  
    if (m_IsInnerTubeVisible)
    {
      gfl2::gfx::DrawUtil::SetMaterialAlpha(m_InnerTubeColor.w);
      gfl2::gfx::DrawUtil::DrawTube(
        m_ModelPosition,
        m_InnerTubeRadiuses[m_PokemonSize],
        500.0f,
        m_InnerTubeColor);
    }
  
    if (m_IsOuterTubeVisible)
    {
      gfl2::gfx::DrawUtil::SetMaterialAlpha(m_OuterTubeColor.w);
      gfl2::gfx::DrawUtil::DrawTube(
        m_ModelPosition,
        m_OuterTubeRadiuses[m_PokemonSize],
        500.0f,
        m_OuterTubeColor);
    }
  
    // AABBの描画
    if (m_IsModelAABBVisible)
    {
      gfl2::gfx::DrawUtil::SetMaterialAlpha(m_ModelAABBColor.w);
      gfl2::gfx::DrawUtil::DrawAABB(m_ModelAABB, m_ModelAABBColor);
    }
  }
  // 簡易描画終了
  gfl2::gfx::DrawUtil::EndRender();

  gfl2::gfx::GFGL::EndScene();

  // 簡易描画の深度テストを元に戻す
  stateData.m_DepthTestEnable = false;
  gfl2::gfx::DrawUtil::SetDepthTestState(stateData);
}

ViewerRenderingPipeLine::ViewerRenderingPipeLine(
  const c8* colorShaderPlaneFilePath,
  const c8* colorShaderPlaneVertexShaderPath,
  const c8* colorShaderPlanePixelShaderPath,
  const c8* colorShaderPlaneTexturePath,
  const c8* edgeCheckPlaneFilePath,
  const c8* edgeCheckPlaneVertexShaderPath,
  const c8* edgeCheckPlanePixelShaderPath,
  const c8* edgeCheckPlaneTexturePath,
  const c8*   edgeGfbmdlPath,
  const c8**  edgeShaderPaths,
  s32   edgeShaderNum,
  const c8**  edgeTexturePaths,
  s32   edgeTexNum,
  const c8* gfbmdlPathNormal,
  const c8** shaderPathsNormal,
  s32 shaderNumNormal,
  const c8* texturePathNormal,
  const c8* gfbmdlPathId,
  const c8** shaderPathsId,
  s32 shaderNumId,
  const c8* texturePathId,
  const c8* dofPlaneFilePath,
  const c8* dofPlaneVertexShaderPath,
  const c8* dofFrontPlanePixelShaderPath,
  const c8* dofBackPlanePixelShaderPath,
  const c8* dofPlaneTexturePath,
  u32 screenWidth,
  u32 screenHeight,
  gfl2::gfx::Surface* pSurface) :
m_OutLinePostRenderPath(),
m_ColorShaderRenderTargetPlaneResourceNode(NULL),
m_ColorShaderRenderTargetPlaneInstanceNode(NULL),
m_pColorShaderModelData(NULL),
m_pColorShaderVertexShaderData(NULL),
m_pColorShaderPixelShaderData(NULL),
m_pColorShaderTextureData(NULL),
m_EdgeCheckRenderTargetPlaneResourceNode(NULL),
m_EdgeCheckRenderTargetPlaneInstanceNode(NULL),
m_pEdgeCheckModelData(NULL),
m_pEdgeCheckVertexShaderData(NULL),
m_pEdgeCheckPixelShaderData(NULL),
m_pEdgeCheckTextureData(NULL),
m_pDofFrontPixelShaderData(NULL),
m_pDofBackPixelShaderData(NULL),
m_IsBackgroundVisible(false),
m_IsJointVisible(false),
m_IsEffectLocatorVisible(true),
m_IsGridVisible(true),
m_IsInterestRangeVisible(false),
m_IsOutLineVisible(true),
m_IsDofEnable(false),
m_RenderPathCnt(0),
m_CpuStartFrame(0),
m_CpuEndFrame(0),
m_EdgeMapSceneRenderPath(gfl2::clr::gfx::GFGL::GetGLAllocator(), DRAWABLE_MODEL_MAX)
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode = NULL; 

  // カラーシェーダ用のリソースを作る
  m_pColorShaderModelData = new gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
  m_pColorShaderModelData->SetModelData((c8*)LocalFunc::ReadFile(colorShaderPlaneFilePath));

  m_ColorShaderRenderTargetPlaneResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pColorShaderModelData);

  m_pColorShaderVertexShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pColorShaderVertexShaderData->SetShaderData((c8*)LocalFunc::ReadFile(colorShaderPlaneVertexShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pColorShaderVertexShaderData);
  m_ColorShaderRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pColorShaderPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pColorShaderPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(colorShaderPlanePixelShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pColorShaderPixelShaderData);
  m_ColorShaderRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pColorShaderTextureData = new gfl2::renderingengine::scenegraph::resource::GfBinaryTexData();
  m_pColorShaderTextureData->SetTextureData((c8*)LocalFunc::ReadFile(colorShaderPlaneTexturePath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pColorShaderTextureData);
  m_ColorShaderRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_ColorShaderRenderTargetPlaneInstanceNode = 
    gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_ColorShaderRenderTargetPlaneResourceNode);
  
  // カラーシェーダ用モデルをシーングラフから外す
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_ColorShaderRenderTargetPlaneInstanceNode);

  // エッジカラー表示用のリソースを作る
  m_pEdgeCheckModelData = new gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
  m_pEdgeCheckModelData->SetModelData((c8*)LocalFunc::ReadFile(edgeCheckPlaneFilePath));

  m_EdgeCheckRenderTargetPlaneResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pEdgeCheckModelData);

  m_pEdgeCheckVertexShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pEdgeCheckVertexShaderData->SetShaderData((c8*)LocalFunc::ReadFile(edgeCheckPlaneVertexShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pEdgeCheckVertexShaderData);
  m_EdgeCheckRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pEdgeCheckPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pEdgeCheckPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(edgeCheckPlanePixelShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pEdgeCheckPixelShaderData);
  m_EdgeCheckRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pEdgeCheckTextureData = new gfl2::renderingengine::scenegraph::resource::GfBinaryTexData();
  m_pEdgeCheckTextureData->SetTextureData((c8*)LocalFunc::ReadFile(edgeCheckPlaneTexturePath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pEdgeCheckTextureData);
  m_EdgeCheckRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_EdgeCheckRenderTargetPlaneInstanceNode = 
    gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_EdgeCheckRenderTargetPlaneResourceNode);

    // エッジカラー用モデルをシーングラフから外す
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_EdgeCheckRenderTargetPlaneInstanceNode);

  // 被写界深度用のリソースを作る
  m_pDofModelData = new gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData();
  m_pDofModelData->SetModelData((c8*)LocalFunc::ReadFile(dofPlaneFilePath));

  m_DofRenderTargetPlaneResourceNode = 
    gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pDofModelData);

  m_pDofVertexShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofVertexShaderData->SetShaderData((c8*)LocalFunc::ReadFile(dofPlaneVertexShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pDofVertexShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofFrontPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofFrontPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(dofFrontPlanePixelShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pDofFrontPixelShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofBackPixelShaderData = new gfl2::renderingengine::scenegraph::resource::GfBinaryShdData();
  m_pDofBackPixelShaderData->SetShaderData((c8*)LocalFunc::ReadFile(dofBackPlanePixelShaderPath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pDofBackPixelShaderData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_pDofTextureData = new gfl2::renderingengine::scenegraph::resource::GfBinaryTexData();
  m_pDofTextureData->SetTextureData((c8*)LocalFunc::ReadFile(dofPlaneTexturePath));
  pResourceNode = scenegraph::resource::ResourceManager::CreateResourceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_pDofTextureData);
  m_DofRenderTargetPlaneResourceNode->AddChild(pResourceNode);

  m_DofRenderTargetPlaneInstanceNode = 
    gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(gfl2::clr::gfx::GFGL::GetGLAllocator(), m_DofRenderTargetPlaneResourceNode);
  
  // 被写界深度用モデルをシーングラフから外す
  gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild(m_DofRenderTargetPlaneInstanceNode);


  m_ColorShaderTextureRenderPaths.CreateBuffer(DRAWABLE_MODEL_MAX, gfl2::clr::gfx::GFGL::GetGLAllocator());

  {// 背景描画パス初期化
    for (s32 i = 0; i < DRAWABLE_BACKGROUND_MAX; ++i)
    {
      ModelRenderPath::InitDescription desc;
      m_BackgroundRenderPath[i].Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    }
  }
  {
    // シーンレンダーパス初期化
    for (s32 i = 0; i < DRAWABLE_MODEL_MAX; ++i)
    {
      ModelRenderPath::InitDescription desc;
      m_ModelRenderPath[i].Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    }

		for (s32 i = 0; i < DRAWABLE_MODEL_MAX; ++i)
    {
			SceneRenderPath::InitDescription desc;
			m_DistortionPostRenderPath[i].Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
      m_DistortionPostRenderPath[i].CreateBuffer(gfl2::clr::gfx::GFGL::GetGLAllocator(), DRAWABLE_MODEL_MAX);
    }
  }

  {
    // シーンレンダーパス初期化
    SceneRenderPath::InitDescription desc;

    // ワイヤーフレームパスはシーンと同じ初期化
    m_WireFrameRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    m_EdgeMapSceneRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
		c8* edgeGfbmdlBinary = (c8*)LocalFunc::ReadFile(edgeGfbmdlPath);
    c8** edgeShaderBinaries = new c8*[edgeShaderNum];
    for (s32 i = 0; i < edgeShaderNum; ++i)
    {
      edgeShaderBinaries[i] = (c8*)LocalFunc::ReadFile(edgeShaderPaths[i]);
    }
    c8** edgeTextureBinaries = new c8*[edgeTexNum];
    for (s32 i = 0; i < edgeTexNum; ++i)
    {
      edgeTextureBinaries[i] = (c8*)LocalFunc::ReadFile(edgeTexturePaths[i]);
    }

    // エッジマップのパスに必要なリソースの読み込み
    m_EdgeMapSceneRenderPath.LoadResources(gfl2::clr::gfx::GFGL::GetGLAllocator(), edgeGfbmdlBinary, edgeShaderBinaries, edgeShaderNum, edgeTextureBinaries, edgeTexNum);

    delete edgeShaderBinaries;
    delete edgeTextureBinaries;
  }

  {// アウトラインの初期化
    c8* gfbmdlBinaryNormal = (c8*)LocalFunc::ReadFile(gfbmdlPathNormal);
    c8** shaderBinariesNormal = new c8*[shaderNumNormal];
    for (s32 i = 0; i < shaderNumNormal; ++i)
    {
      shaderBinariesNormal[i] = (c8*)LocalFunc::ReadFile(shaderPathsNormal[i]);
    }
    c8* textureBinaryNormal = (c8*)LocalFunc::ReadFile(texturePathNormal);

    c8* gfbmdlBinaryId = (c8*)LocalFunc::ReadFile(gfbmdlPathId);
    c8** shaderBinariesId = new c8*[shaderNumId];
    for (s32 i = 0; i < shaderNumId; ++i)
    {
      shaderBinariesId[i] = (c8*)LocalFunc::ReadFile(shaderPathsId[i]);
    }
    c8* textureBinaryId = (c8*)LocalFunc::ReadFile(texturePathId);

    m_OutLinePostRenderPath.Initialize(
      gfl2::clr::gfx::GFGL::GetGLAllocator(), 
      gfbmdlBinaryNormal,
      shaderBinariesNormal,
      shaderNumNormal,
      textureBinaryNormal,
      gfbmdlBinaryId,
      shaderBinariesId,
      shaderNumId,
      textureBinaryId);
  }

  {// アウトライン描画用のテクスチャの初期化
    m_EdgeMapSceneRenderPath.CreateEdgeMapTexture(
      gfl2::clr::gfx::GFGL::GetGLAllocator(),
      static_cast<float>(screenWidth),
      static_cast<float>(screenHeight));

    const gfx::Texture		*pTexture;

    pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal );
    m_OutLinePostRenderPath.SetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Normal, pTexture );

    pTexture = m_EdgeMapSceneRenderPath.GetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID );
    m_OutLinePostRenderPath.SetEdgeMapTexture( poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::ID, pTexture );
  }

  {
    // ブラー用描画パス初期化
    {
      gfl2::renderingengine::renderer::util::MakeBlurImagePath::InitDescription desc;
      desc.m_ScreenWidth = screenWidth;
      desc.m_ScreenHeight = screenHeight;
      m_MakeBlurImagePath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    }

    {
      gfl2::renderingengine::renderer::util::MakeReductionImagePath::InitDescription desc;
      desc.m_ScreenWidth = screenWidth / 2;
      desc.m_ScreenHeight = screenHeight / 2;
      desc.m_pSourceTexture = m_MakeBlurImagePath.GetDepthWrittenTexture();
      m_MakeReductionImagePath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    }

    {
      gfl2::renderingengine::renderer::util::DofRenderPath::InitDescription desc;
      desc.m_pTargetModelInstanceNode = m_DofRenderTargetPlaneInstanceNode;
      desc.m_pSourceTexture = m_MakeReductionImagePath.GetDestinationTexture();
      m_DofRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
    }

    // ブラー用の焦点距離を設定
    f32 focusLength = 200.0f;
    f32 blurredCoef = 0.75f;
    m_MakeBlurImagePath.SetAvailableLayerNum(1);
    m_MakeBlurImagePath.SetFocusRange(0, focusLength, blurredCoef);
    m_DofRenderPath.SetFocusLength(focusLength);
  }

  {// メガヤミラミ/屈折を使用したエフェクト用の描画パスの初期化
    m_StretchBltFrameBufferPath.CreateFrameBufferTexture(gfl2::clr::gfx::GFGL::GetGLAllocator(), pSurface);
  }

  {
    // ジョイントレンダリングパス初期化
    gfl2::renderingengine::renderer::util::ViewerJointRenderPath::InitDescription desc;
    m_JointRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // エフェクトロケータレンダリングパス初期化
    gfl2::renderingengine::renderer::util::EffectLocatorRenderPath::InitDescription desc;
    m_EffectLocatorRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // グリッドレンダリングパス初期化
    GridRenderPath::InitDescription desc;
    m_GridRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // 負荷ゲージレンダリングパス初期化
    GaugeRenderPath::InitDescription desc;
    m_GaugeRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // 判定図形レンダリングパス初期化
    DebugFigureRenderPath::InitDescription desc;
    m_DebugFigureRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // Mcnfレンダリングパス初期化
    McnfRenderPath::InitDescription desc;
    m_McnfRenderPath.Initialize(gfl2::clr::gfx::GFGL::GetGLAllocator(), desc);
  }

  {
    // 画面全体描画パス初期化
    m_FillScreenRenderPath.SetColorShaderRenderTargetPlane(m_EdgeCheckRenderTargetPlaneInstanceNode);
    m_FillScreenRenderPath.SetFillScreenTexture(NULL);
  }

  // カラーシェーダ以外のパスを全て配列に格納
  s32 pathCount = 0;

  m_PathStartIndex[PathType::Clear] = pathCount;
  m_AllPaths[pathCount] = &m_ClearRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Background] = pathCount;
  for (s32 i = 0; i < DRAWABLE_BACKGROUND_MAX; ++i)
  {
    m_AllPaths[pathCount] = &m_BackgroundRenderPath[i];
    ++pathCount;
  }

  m_PathStartIndex[PathType::Edge] = pathCount;
  m_AllPaths[pathCount] = &m_EdgeMapSceneRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Model] = pathCount;
  for (s32 i = 0; i < DRAWABLE_MODEL_MAX; ++i)
  {
    m_AllPaths[pathCount] = &m_ModelRenderPath[i];
    ++pathCount;
  }

  m_PathStartIndex[PathType::OutLine] = pathCount;
  m_AllPaths[pathCount] = &m_OutLinePostRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::StretchBltFrameBuffer] = pathCount;
  m_AllPaths[pathCount] = &m_StretchBltFrameBufferPath;
  ++pathCount;

  m_PathStartIndex[PathType::Distortion] = pathCount;
  gfl2::gfx::Texture* pFrameBufferTexture = m_StretchBltFrameBufferPath.GetFrameBufferTexture();
  for (s32 i = 0; i < DRAWABLE_MODEL_MAX; ++i)
  {
		m_DistortionPostRenderPath[i].SetOverrideTexture(pFrameBufferTexture);
    m_AllPaths[pathCount] = &m_DistortionPostRenderPath[i];
    ++pathCount;
  }

  m_PathStartIndex[PathType::WireFrame] = pathCount;
  m_AllPaths[pathCount] = &m_WireFrameRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Grid] = pathCount;
  m_AllPaths[pathCount] = &m_GridRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::MakeBlurImage] = pathCount;
  m_AllPaths[pathCount] = &m_MakeBlurImagePath;
  ++pathCount;

  m_PathStartIndex[PathType::MakeReductionImage] = pathCount;
  m_AllPaths[pathCount] = &m_MakeReductionImagePath;
  ++pathCount;

  m_PathStartIndex[PathType::Dof] = pathCount;
  m_AllPaths[pathCount] = &m_DofRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Joint] = pathCount;
  m_AllPaths[pathCount] = &m_JointRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::EffectLocator] = pathCount;
  m_AllPaths[pathCount] = &m_EffectLocatorRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Gauge] = pathCount;
  m_AllPaths[pathCount] = &m_GaugeRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::DebugFigure] = pathCount;
  m_AllPaths[pathCount] = &m_DebugFigureRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Mcnf] = pathCount;
  m_AllPaths[pathCount] = &m_McnfRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::FillScreen] = pathCount;
  m_AllPaths[pathCount] = &m_FillScreenRenderPath;
  ++pathCount;

  m_PathStartIndex[PathType::Present] = pathCount;
  m_AllPaths[pathCount] = &m_PresentPath;
  ++pathCount;

  m_PathStartIndex[PathType::NumberOf] = pathCount;
  m_AllPaths[pathCount] = NULL;
  ++pathCount;
}

ViewerRenderingPipeLine::~ViewerRenderingPipeLine()
{
  if (m_ColorShaderRenderTargetPlaneInstanceNode)
  {
    delete m_ColorShaderRenderTargetPlaneInstanceNode;
    m_ColorShaderRenderTargetPlaneInstanceNode = NULL;
  }

  if (m_pColorShaderModelData)
  {
    delete m_pColorShaderModelData;
    m_pColorShaderModelData = NULL;
  }

  if (m_pColorShaderVertexShaderData)
  {
    delete m_pColorShaderVertexShaderData;
    m_pColorShaderVertexShaderData = NULL;
  }

  if (m_pColorShaderPixelShaderData)
  {
    delete m_pColorShaderPixelShaderData;
    m_pColorShaderPixelShaderData = NULL;
  }

  if (m_pColorShaderTextureData)
  {
    delete m_pColorShaderTextureData;
    m_pColorShaderTextureData = NULL;
  }

  if (m_EdgeCheckRenderTargetPlaneInstanceNode)
  {
    delete m_EdgeCheckRenderTargetPlaneInstanceNode;
    m_EdgeCheckRenderTargetPlaneInstanceNode = NULL;
  }

  if (m_pEdgeCheckModelData)
  {
    delete m_pEdgeCheckModelData;
    m_pEdgeCheckModelData = NULL;
  }

  if (m_pEdgeCheckVertexShaderData)
  {
    delete m_pEdgeCheckVertexShaderData;
    m_pEdgeCheckVertexShaderData = NULL;
  }

  if (m_pEdgeCheckPixelShaderData)
  {
    delete m_pEdgeCheckPixelShaderData;
    m_pEdgeCheckPixelShaderData = NULL;
  }

  if (m_pEdgeCheckTextureData)
  {
    delete m_pEdgeCheckTextureData;
    m_pEdgeCheckTextureData = NULL;
  }

  if (m_DofRenderTargetPlaneInstanceNode)
  {
    delete m_DofRenderTargetPlaneInstanceNode;
    m_DofRenderTargetPlaneInstanceNode = NULL;
  }

  if (m_pDofModelData)
  {
    delete m_pDofModelData;
    m_pDofModelData = NULL;
  }

  if (m_pDofVertexShaderData)
  {
    delete m_pDofVertexShaderData;
    m_pDofVertexShaderData = NULL;
  }

  if (m_pDofFrontPixelShaderData)
  {
    delete m_pDofFrontPixelShaderData;
    m_pDofFrontPixelShaderData = NULL;
  }

  if (m_pDofBackPixelShaderData)
  {
    delete m_pDofBackPixelShaderData;
    m_pDofBackPixelShaderData = NULL;
  }

  if (m_pDofTextureData)
  {
    delete m_pDofTextureData;
    m_pDofTextureData = NULL;
  }
}

void ViewerRenderingPipeLine::ChangeBackgroundVisibility(b32 enable)
{
  m_IsBackgroundVisible = enable;
}

void ViewerRenderingPipeLine::ChangeModelVisibility(b32 enable)
{
  m_IsModelVisible = enable;
}

void ViewerRenderingPipeLine::ChangeWireFrameVisibility(b32 enable)
{
  m_IsWireFrameVisible = enable;
}

void ViewerRenderingPipeLine::ChangeJointVisibility(b32 enable)
{
  m_IsJointVisible = enable;
}

void ViewerRenderingPipeLine::ChangeEffectLocatorVisibility(b32 enable)
{
  m_IsEffectLocatorVisible = enable;
}

void ViewerRenderingPipeLine::ChangeGridVisibility(b32 enable)
{
  m_IsGridVisible = enable;
}

void ViewerRenderingPipeLine::ChangeGaugeVisibility(b32 enable)
{
  m_GaugeRenderPath.ChangeVisibility(enable);
}

void ViewerRenderingPipeLine::ChangeInterestRangeVisibility(b32 enable)
{
  m_IsInterestRangeVisible = enable;
}

void ViewerRenderingPipeLine::ChangeOutLineVisibility(b32 enable)
{
  m_IsOutLineVisible = enable;
}

void ViewerRenderingPipeLine::SetEnabledDof(b32 enable)
{
  m_IsDofEnable = enable;
}

void ViewerRenderingPipeLine::SetClearColor(const gfl2::gfx::Color& color)
{
  m_ClearRenderPath.SetClearColor(color);
}

void ViewerRenderingPipeLine::AddEdgeRenderingTarget(poke_3d::model::BaseModel* pModel)
{
  m_EdgeMapSceneRenderPath.AddEdgeRenderingTarget(pModel->GetModelInstanceNode());
}

void ViewerRenderingPipeLine::RemoveEdgeRenderingTarget(poke_3d::model::BaseModel* pModel)
{
  m_EdgeMapSceneRenderPath.RemoveEdgeRenderingTarget(pModel->GetModelInstanceNode());
}

void ViewerRenderingPipeLine::SetCpuFrame(s32 frame)
{
  m_GaugeRenderPath.SetCpuFrame(frame);
}

void ViewerRenderingPipeLine::SetGpuFrame(s32 frame)
{
  m_GaugeRenderPath.SetGpuFrame(frame);
}

s32 ViewerRenderingPipeLine::GetCpuTimeAverage()
{
  return m_GaugeRenderPath.GetCpuTimeAverage();
}

s32 ViewerRenderingPipeLine::GetGpuTimeAverage()
{
  return m_GaugeRenderPath.GetGpuTimeAverage();
}

b32 ViewerRenderingPipeLine::StartRenderPath()
{
  m_RenderPathCnt = 0;
  return true;
}

void ViewerRenderingPipeLine::AddColorShaderTexture(
  const gfl2::gfx::Texture* pOriginalColorTexture,
  const gfl2::gfx::Texture* pColorShaderTexture)
{
  ColorShaderTextureRenderPath* path = new ColorShaderTextureRenderPath();
  path->SetColorShaderRenderTargetPlane(m_ColorShaderRenderTargetPlaneInstanceNode);
  path->SetOriginalColorTexture(pOriginalColorTexture);
  path->SetColorShaderTexture(pColorShaderTexture);
  m_ColorShaderTextureRenderPaths.push_back(path);
}

void ViewerRenderingPipeLine::RemoveColorShaderTexture(u32 index)
{
  renderer::util::ColorShaderTextureRenderPath* path = m_ColorShaderTextureRenderPaths[index];
  m_ColorShaderTextureRenderPaths.remove(path);
  delete path;
}

void ViewerRenderingPipeLine::SetColorShaderTextureColor(
  s32 startPassIndex,
  u32 passNum,
  const math::Vector4& color)
{
  s32 size = m_ColorShaderTextureRenderPaths.size();
  s32 passNumFromStart = startPassIndex + static_cast<s32>(passNum);
  assert(passNumFromStart <= size);

  for (s32 i = startPassIndex; i < passNumFromStart; ++i)
  {
    m_ColorShaderTextureRenderPaths[i]->SetColorShaderTextureColor(color);
  }
}

/**
 * @brief 該当のノードを所有しているModelRenderPath検索
 * @param pDrawEnvNode 検索対象のDrawEnvNode
 * @param pModelInstanceNode 検索対象のModelInstanceNode
 */
s32 ViewerRenderingPipeLine::FindModelIndexOfModelRenderPath(
  poke_3d::model::PokemonModel* pModel,
  gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
  s32 renderPathArrayLength)
{
  if (!renderPathArray)
  {
    return -1;
  }

  if (renderPathArrayLength <= 0)
  {
    return -1;
  }

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pDrawEnvNode = NULL;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = NULL;

  if (pModel != NULL)
  {
    pDrawEnvNode = pModel->GetDrawEnvNode();
    pModelInstanceNode = pModel->GetModelInstanceNode();
  }

  for (s32 i = 0; i < renderPathArrayLength; ++i)
  {
    const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pModelPathDrawEnvNode = renderPathArray[i].GetDrawEnvNode();
    const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelPathModelInstanceNode = renderPathArray[i].GetModelInstanceNode();
    if ((pDrawEnvNode == pModelPathDrawEnvNode) && (pModelInstanceNode == pModelPathModelInstanceNode))
    {
      return i;
    }
  }

  return -1;
}

b32 ViewerRenderingPipeLine::AddDrawableModel(poke_3d::model::PokemonModel* pModel)
{
  s32		index = AddDrawableModelToRenderPath(pModel, m_ModelRenderPath, DRAWABLE_MODEL_MAX);

	if ( index >= 0 )
	{
		m_DistortionPostRenderPath[index].AddDrawEnv( pModel->GetDrawEnvNode() );
		m_DistortionPostRenderPath[index].AddDistortionRenderingTarget( pModel->GetModelInstanceNode() );
	}

	return (index >= 0);
}

b32 ViewerRenderingPipeLine::RemoveDrawableModel(poke_3d::model::PokemonModel* pModel)
{
  s32		index = RemoveDrawableModelToRenderPath(pModel, m_ModelRenderPath, DRAWABLE_MODEL_MAX);

	if (index >= 0)
	{
		m_DistortionPostRenderPath[index].RemoveDrawEnv( pModel->GetDrawEnvNode() );
		m_DistortionPostRenderPath[index].RemoveDistortionRenderingTarget( pModel->GetModelInstanceNode() );
	}

	return (index >= 0);
}

b32 ViewerRenderingPipeLine::AddBackgroundModel(poke_3d::model::PokemonModel* pModel)
{
  s32 index = AddDrawableModelToRenderPath(pModel, m_BackgroundRenderPath, DRAWABLE_BACKGROUND_MAX);
  if (index < 0)
  {
    return false;
  }

  return true;
}

b32 ViewerRenderingPipeLine::RemoveBackgroundModel(poke_3d::model::PokemonModel* pModel)
{
  s32			index = RemoveDrawableModelToRenderPath(pModel, m_BackgroundRenderPath, DRAWABLE_BACKGROUND_MAX);
	return (index >= 0);
}

void ViewerRenderingPipeLine::SetCameraDataToBackgroundPath(
  b32 useCameraData,
  gfl2::math::Matrix44 projection,
  gfl2::math::Matrix34 view)
{
  for (s32 i = 0; i < DRAWABLE_BACKGROUND_MAX; ++i)
  {
    ModelRenderPath::Camera camera;
    camera.m_UseCamera = useCameraData;
    camera.m_Projection = projection;
    camera.m_View = view;
    m_BackgroundRenderPath[i].SetCameraData(camera);
  }
}

void ViewerRenderingPipeLine::ChangeFillTexture(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Enum edgeColor)
{
  u32 type = static_cast<u32>(edgeColor);
  if (type < static_cast<u32>(poke_3d::renderer::EdgeMapSceneRenderPath::EdgeColor::Enum::Max))
  {
    m_FillScreenRenderPath.SetFillScreenTexture(m_EdgeMapSceneRenderPath.GetEdgeMapTexture(type));
    return;
  }

  m_FillScreenRenderPath.SetFillScreenTexture(NULL);
}

void ViewerRenderingPipeLine::ChangeFillScreenType(gfl2::renderingengine::FillScreenConstantType::Enum fillScreenType)
{
  u32 type = static_cast<u32>(fillScreenType);
  for (u32 i = 0; i < gfl2::renderingengine::scenegraph::resource::CombinerAttribute::ConstantMax; ++i)
  {
    m_FillScreenRenderPath.SetConstantColorToRenderTargetPlane(i, gfl2::math::Vector4::GetZero());
  }
  m_FillScreenRenderPath.SetConstantColorToRenderTargetPlane(type, gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
}

void ViewerRenderingPipeLine::ChangeDofType(gfl2::renderingengine::DofType::Enum dofType)
{
  m_MakeBlurImagePath.ChangeDofType(dofType);
  m_DofRenderPath.ChangeDofType(dofType);
}

s32 ViewerRenderingPipeLine::AddDrawableModelToRenderPath(
  poke_3d::model::PokemonModel* pModel,
  gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
  s32 renderPathArrayLength)
{
  if (pModel == NULL)
  {// 追加対象のモデルが指定されてないので終了
    return -1;
  }

  if (renderPathArrayLength <= 0)
  {
    return -1;
  }

  // 空いている領域を探す
  s32 index = FindModelIndexOfModelRenderPath(NULL, renderPathArray, renderPathArrayLength);
  if (index < 0)
  {
    return -1;
  }

  renderPathArray[index].SetDrawEnvNode(pModel->GetDrawEnvNode());
  renderPathArray[index].SetModelInstanceNode(pModel->GetModelInstanceNode());
  return index;
}

s32 ViewerRenderingPipeLine::RemoveDrawableModelToRenderPath(
  poke_3d::model::PokemonModel* pModel,
  gfl2::renderingengine::renderer::util::ModelRenderPath* renderPathArray,
  s32 renderPathArrayLength)
{
  if (pModel == NULL)
  {// 削除対象のモデルが指定されてないので終了
    return -1;
  }

  if (renderPathArrayLength <= 0)
  {
    return -1;
  }

  // モデルが存在する領域を探す
  s32 index = FindModelIndexOfModelRenderPath(pModel, renderPathArray, renderPathArrayLength);
  if (index < 0)
  {
    return -1;
  }

  // パスから外す(外すだけ. リソースは削除しない. 削除は作った人の仕事)
  renderPathArray[index].SetDrawEnvNode(NULL);
  renderPathArray[index].SetModelInstanceNode(NULL);
  return index;
}

gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* ViewerRenderingPipeLine::GetRenderPath()
{
  // カラーシェーダ用の描画パスを画面のレンダリングの前に実行
  u32 colorShaderRenderPathNum = m_ColorShaderTextureRenderPaths.size();
  if (m_RenderPathCnt < colorShaderRenderPathNum)
  {
    return m_ColorShaderTextureRenderPaths[m_RenderPathCnt];
  }

  // カラーシェーダ用の描画パスの更新が終わっていたら, 描画パスの更新数だけ引いて画面描画開始
  s32 renderPathCnt = static_cast<s32>(m_RenderPathCnt - colorShaderRenderPathNum);
  if (renderPathCnt == m_PathStartIndex[PathType::Clear])
  {
    m_CpuStartFrame = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
  }
  else if (renderPathCnt == m_PathStartIndex[PathType::Gauge])
  {
    // CPU時間計測終了
    m_GaugeRenderPath.SetCpuFrame(gfl2::gfx::GFGL::GetTotalCPUTimeMicroSecond());
    m_GaugeRenderPath.SetGpuFrame(gfl2::gfx::GFGL::GetTotalGPUTimeMicroSecond());
  }

  s32 limit = sizeof(m_AllPaths) / sizeof(m_AllPaths[0]);
  if (renderPathCnt < limit)
  {
    return m_AllPaths[renderPathCnt];
  }

  return NULL;
}

b32 ViewerRenderingPipeLine::NextRenderPath()
{
  ++m_RenderPathCnt;
  u32 colorShaderRenderPathNum = m_ColorShaderTextureRenderPaths.size();
  if (m_RenderPathCnt < colorShaderRenderPathNum)
  {
    return true;
  }

  s32 renderPathCnt = static_cast<s32>(m_RenderPathCnt - colorShaderRenderPathNum);

  if ((renderPathCnt == m_PathStartIndex[PathType::Background]) && !m_IsBackgroundVisible)
  {
    renderPathCnt += DRAWABLE_BACKGROUND_MAX;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::Edge]) && !m_IsOutLineVisible)
  {
    ++renderPathCnt;
  }

  if (renderPathCnt == m_PathStartIndex[PathType::Model] && !m_IsModelVisible)
  {
    renderPathCnt += DRAWABLE_MODEL_MAX;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::OutLine]) && !m_IsOutLineVisible)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::WireFrame]) && !m_IsWireFrameVisible)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::Grid]) && !m_IsGridVisible)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::MakeBlurImage]) && !m_IsDofEnable)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::MakeReductionImage]) && !m_IsDofEnable)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::Dof]) && !m_IsDofEnable)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::Joint]) && !m_IsJointVisible)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::EffectLocator]) && !m_IsEffectLocatorVisible)
  {
    ++renderPathCnt;
  }

  if ((renderPathCnt == m_PathStartIndex[PathType::Mcnf]) && !m_IsInterestRangeVisible)
  {
    ++renderPathCnt;
  }

  if (renderPathCnt == m_PathStartIndex[PathType::NumberOf])
  {
    return false;
  }

  m_RenderPathCnt = renderPathCnt + colorShaderRenderPathNum;
  return true;
}

}}}}
