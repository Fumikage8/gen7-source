//===================================================================
/**
 * @file    AppRenderingPipeline.h
 * @brief   アプリレンダリングパイプライン
 * @author  k.ohno
 */
//===================================================================
#include "../include/AppRenderingPipeLine.h"

#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

#include <util/include/gfl2_GLHeapAllocator.h>


#if defined(GF_PLATFORM_CTR)
#include <nn/fs.h>
#include <nn/hio.h>
#endif

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;



//----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
AppRenderingPipeLine::AppRenderingPipeLine( gfl2::heap::HeapBase* heap, f32 screanWidth, f32 screanHeight, u32 lytWkMax )
  : m_pGLHeapAllocator(NULL)
  , m_RenderPathCnt(0)
  , m_LayoutRenderPath(NULL)
 // , m_OutLinePostRenderPath( screanWidth, screanHeight )
{
	// メインとなるシーン描画パスを初期化
	SceneRenderPath::InitDescription desc;
//	m_MainRenderPath.Initialize( desc );

  m_pGLHeapAllocator = GFL_NEW( heap ) gfl2::util::GLHeapAllocator( heap );

  m_LayoutRenderPath = GFL_NEW( heap ) gfl2::lyt::LayoutRenderPath( m_pGLHeapAllocator );
  m_LayoutRenderPath->Init( heap, lytWkMax );
}

//----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
AppRenderingPipeLine::~AppRenderingPipeLine(void)
{
  GFL_SAFE_DELETE(m_LayoutRenderPath);
  GFL_SAFE_DELETE(m_pGLHeapAllocator);
}

//----------------------------------------------------------------------------
/**
*  @brief  最初のパスから描画できる状態にする
*/
//-----------------------------------------------------------------------------
b32 AppRenderingPipeLine::StartRenderPath(void)
{
	m_RenderPathCnt = 0;
	return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  現状のパスにあったシーンを返す
*  @return シーン
*/
//-----------------------------------------------------------------------------
RenderingPipeLine::IRenderPath* AppRenderingPipeLine::GetRenderPath(void)
{
	switch( m_RenderPathCnt )
	{
	case 0:
		return m_LayoutRenderPath;
	}
	return NULL;
}

//----------------------------------------------------------------------------
/**
*  @brief  次のパスへ遷移
*  @return trueなら次のパス、falseなら描画終了
*/
//-----------------------------------------------------------------------------
b32 AppRenderingPipeLine::NextRenderPath(void)
{
	m_RenderPathCnt++;
	if ( m_RenderPathCnt == APP_MAX_PATH_NUM )
	{
		return false;
	}
	return true;
}

