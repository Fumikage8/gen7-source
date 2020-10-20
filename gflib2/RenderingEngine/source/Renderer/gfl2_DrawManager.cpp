//=============================================================================
/**
 * @file    gfl2_DrawManager.cpp
 * @brief   描画マネージャクラス(描画ノード管理、描画順コントロールなどを行う。)
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.5.11   ishiguro  重複機能関数の削除
 *  2016.5.10   ishiguro  ワークサイズ取得系関数追加
 *  2016.5.5    ishiguro  デバッグ掲示関数追加
 *
 */
//=============================================================================
#include <RenderingEngine/include/renderer/gfl2_DrawManager.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>
#include <algorithm>    // レンダーキューのソートに使用する.


#include <RenderingEngine/include/Renderer/gfl2_MeshDrawTag.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::scenegraph;
using namespace gfl2::renderingengine::scenegraph::instance;

namespace gfl2 { namespace renderingengine { namespace renderer { 

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
/*DrawManager* DrawManager::Create( InitDescription desc )
{
  return Create( gfx::GLMemory::GetAllocator(), desc );
}*/

DrawManager* DrawManager::Create( gfx::IGLAllocator* pAllocator, InitDescription desc )
{
  gfx::GLMemory::SetInstanceAllocator( pAllocator ); 
	DrawManager			*pDrawManager = new DrawManager( desc );
  gfx::GLMemory::ClearInstanceAllocator();
	return pDrawManager;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DrawManager::DrawManager( InitDescription desc ):
	m_Desc(desc),
	m_DrawLayerBit( 0 ),
  m_pOTOverride( NULL )
{
	GFL_ASSERT( m_Desc.m_NodeStackSize );
	GFL_ASSERT( m_Desc.m_DrawTagStackSize );
	GFL_ASSERT( m_Desc.m_DrawEnvStackSize );

	m_DrawableNodeContainer.CreateBuffer( m_Desc.m_NodeStackSize );
	m_DrawEnvNodeContainer.CreateBuffer( m_Desc.m_DrawEnvStackSize );

	m_OTInfo.m_OrderingTable = reinterpret_cast<DrawTag**>( this->GetInstanceAllocator()->SystemMemoryMalloc( sizeof(DrawTag*) * m_Desc.m_DrawTagStackSize * EXECUTABLE_LAYER_MAX ) );

	//以下デフォルト設定
	DrawLayerEnable( scenegraph::resource::DrawLayer::Layer0 );
	DrawLayerEnable( scenegraph::resource::DrawLayer::Layer1 );
	DrawLayerEnable( scenegraph::resource::DrawLayer::Layer2 );
	DrawLayerEnable( scenegraph::resource::DrawLayer::Layer3 );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
DrawManager::~DrawManager()
{
	if ( m_OTInfo.m_OrderingTable )
	{
		this->GetInstanceAllocator()->SystemMemoryFree( m_OTInfo.m_OrderingTable );
		m_OTInfo.m_OrderingTable = NULL;
	}

	m_DrawableNodeContainer.clear();
	m_DrawEnvNodeContainer.clear();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::AddObject( scenegraph::instance::DrawableNode *pNode )
{
	GFL_ASSERT( pNode );

#if GFL_DEBUG
  //m_DrawableNodeContainerの使用状況チェック
  DebugSizeCheckDump( "DrawableNodeContainerの空きが少なくなっています。DrawManager初期化時のNodeStackSizeを増やしてください。", 
                      m_DrawableNodeContainer.size(),
                      m_DrawableNodeContainer.buffer_size() );
#endif

	m_DrawableNodeContainer.push_back( pNode );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::RemoveObject( scenegraph::instance::DrawableNode *pNode )
{
	GFL_ASSERT( pNode );
	m_DrawableNodeContainer.remove( pNode );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::RemoveAllObject()
{
	m_DrawableNodeContainer.clear();
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
u32 DrawManager::ObjectCnt()
{
  return m_DrawableNodeContainer.size();
}

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
//-----------------------------------------------------------------------------
/// @brief    描画オブジェクトコンテナ最大サイズ取得関数
/// @return   描画オブジェクトコンテナ最大サイズ
//-----------------------------------------------------------------------------
u32 DrawManager::GetDrawableNodeContainerMaxSize() const
{
  return m_DrawableNodeContainer.buffer_size();
}
#endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::AddDrawEnv( scenegraph::instance::DrawEnvNode *pNode )
{
	GFL_ASSERT( pNode );

#if GFL_DEBUG
  //m_DrawEnvNodeContainerの使用状況チェック
  DebugSizeCheckDump( "DrawEnvNodeContainerの空きが少なくなっています。DrawManager初期化時のDrawEnvStackSizeを増やしてください。", 
    m_DrawEnvNodeContainer.size(),
    m_DrawEnvNodeContainer.buffer_size() );
#endif

	m_DrawEnvNodeContainer.push_back( pNode );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::RemoveDrawEnv( scenegraph::instance::DrawEnvNode *pNode )
{
	GFL_ASSERT( pNode );
	m_DrawEnvNodeContainer.remove( pNode );
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::RemoveAllDrawEnv()
{
	m_DrawEnvNodeContainer.clear();
}

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)

//-----------------------------------------------------------------------------
/// @brief    DrawEnvNodeコンテナ使用サイズ取得関数
/// @return   DrawEnvNodeコンテナ使用サイズ
//-----------------------------------------------------------------------------
u32 DrawManager::GetDrawEnvNodeContainerUsedSize() const
{
  return m_DrawEnvNodeContainer.size();
}

//-----------------------------------------------------------------------------
/// @brief    DrawEnvNodeコンテナ最大サイズ取得関数
/// @return   DrawEnvNodeコンテナ最大サイズ
//-----------------------------------------------------------------------------
u32 DrawManager::GetDrawEnvNodeContainerMaxSize() const
{
  return m_DrawEnvNodeContainer.buffer_size();
}

#endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
b32 DrawManager::IsDrawLayerEnable( const scenegraph::resource::DrawLayer::Enum type )
{
	u8					bit = 1 << type;
	return m_DrawLayerBit & bit;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::DrawLayerEnable( const scenegraph::resource::DrawLayer::Enum type )
{
	u8					bit = 1 << type;
	m_DrawLayerBit |= bit;

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)
  {
    u32       layerNum = 0;
    for( u32 i = 0; i < scenegraph::resource::DrawLayer::NumberOf; ++i )
    {
      if ( this->IsDrawLayerEnable( static_cast<scenegraph::resource::DrawLayer::Enum>(i) ) )
      {
        ++layerNum;
      }
    }

    GFL_ASSERT_STOP_MSG( layerNum <= EXECUTABLE_LAYER_MAX, "DrawManagerのDrawLayerは最大４つまでしか有効にできません。\n" );
  }
#endif//  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::DrawLayerDisable( const scenegraph::resource::DrawLayer::Enum type )
{
	u8					bit = ~(1 << type);
	m_DrawLayerBit &= bit;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::Update()
{
  if ( m_pOTOverride == NULL )
  {
    m_OTInfo.m_VisibleTagNum = 0;
	  {//描画タグを積む。
		  u32	nodeNum = m_DrawableNodeContainer.size();
      DrawableNode::UpdateDrawTagInfo updateDrawTagInfo;
    
  #if defined(GF_PLATFORM_CTR)
      math::Vector4					screanPos( 0.0f, 0.0f, -1.0f, 1.0f );
  #else
		  math::Vector4					screanPos( 0.0f, 0.0f, 1.0f, 1.0f );
  #endif

		  screanPos = m_Projection.Inverse() * screanPos;

		  updateDrawTagInfo.m_Far = -(screanPos.z / screanPos.w);//演算コスト削減のためニアは考慮しない。
		  updateDrawTagInfo.m_ViewMatrix = m_View;//プロジェクションマトリックスは、ＣＴＲではピボット回転しているので計算結果が一致しない。ビューのみを用いる。

		  for( u32 nodeNo = 0; nodeNo < nodeNum; ++nodeNo )
		  {
			  if ( m_DrawableNodeContainer[nodeNo]->IsVisible() == false )
				  continue;

			  u32 tagNum = m_DrawableNodeContainer[nodeNo]->GetDrawTagNum();

			  m_DrawableNodeContainer[nodeNo]->UpdateDrawTag(updateDrawTagInfo);

			  for( u32 tagNo = 0; tagNo < tagNum; ++tagNo )
			  {
				  DrawTag			*pTag = m_DrawableNodeContainer[nodeNo]->GetDrawTag( tagNo );
				  u32					drawLayerNo = pTag->GetDrawLayerNo();
				  u8					checkBit = 1 << drawLayerNo;

          // 非表示ならタグを積まない
          if (!pTag->IsVisible())
          {
            continue;
          }

				  if ( (m_DrawLayerBit & checkBit) == 0 )
				  {
            continue;
          }

				  m_RenderState.SetWorldMatrix(pTag->GetWorldMatrix());

				  GFL_ASSERT( m_OTInfo.m_VisibleTagNum < (m_Desc.m_DrawTagStackSize * EXECUTABLE_LAYER_MAX) );

				  if ( m_OTInfo.m_VisibleTagNum < (m_Desc.m_DrawTagStackSize * EXECUTABLE_LAYER_MAX) )
				  {
					  m_OTInfo.m_OrderingTable[m_OTInfo.m_VisibleTagNum] = pTag;
					  ++m_OTInfo.m_VisibleTagNum;
				  }
			  }
		  }
	  }

  #if GFL_DEBUG
    //m_OTInfo.m_OrderingTableの使用状況チェック
    DebugSizeCheckDump( "OrderingTableの空きが少なくなっています。DrawManager初期化時のDrawTagStackSizeを増やしてください。",
                        m_OTInfo.m_VisibleTagNum,
                        m_Desc.m_DrawTagStackSize * EXECUTABLE_LAYER_MAX );
  #endif

    // レンダーキーでソートを行う
    std::sort(&m_OTInfo.m_OrderingTable[0], &m_OTInfo.m_OrderingTable[m_OTInfo.m_VisibleTagNum], RenderElementCompare());
  }

	m_RenderState.SetProjectionMatrix( &m_Projection );
	m_RenderState.SetViewMatrix( &m_View );

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  // 初回は必ず違うフォグになるようにしておく
  m_RenderState.ResetCurrentFog();
  
  // もしもUpdateFogをユーザにやらせないなら、コメントアウトしているようなことをここで行うといいと思う。
  //for( u32 envNo = 0; envNo < m_DrawEnvNodeContainer.size(); ++envNo )
  //{
  //  DrawEnvNode *pEnv = m_DrawEnvNodeContainer[ envNo ];
  //  DrawEnvNode::FogOuterParam param;
  //  param.projMatrix = m_Projection;
  //  pEnv->UpdateFog(param);
  //}
#endif  // GF_PLATFORM_CTR

}

void DrawManager::Draw( b32 fUpdate )
{
	if ( fUpdate )
		Update();

  OrderingTableInfo   *pOT = &m_OTInfo;

  if ( m_pOTOverride )
    pOT = m_pOTOverride;

	for (u32 i = 0; i < pOT->m_VisibleTagNum; ++i)
  {
    SetDrawEnvState(pOT->m_OrderingTable[i]);
    pOT->m_OrderingTable[i]->PreDraw(m_RenderState);
    pOT->m_OrderingTable[i]->Draw(m_RenderState);
    pOT->m_OrderingTable[i]->PostDraw(m_RenderState);
  }
}

class PickUpLight{
public:

  PickUpLight()
  {
    for( u32 i = 0; i < RenderState::LightMax; ++i )
      m_pNode[i] = NULL;
  }

  const LightNode				*m_pNode[RenderState::LightMax];

  void PickUp( const LightSetNode *pLightSetNode, const math::AABB &rAABB )
  {
    if ( pLightSetNode == NULL )				return;

    for( u32 lightNo = 0; lightNo < pLightSetNode->GetLightNum(); ++lightNo )
    {
      const LightNode	*pLightNode = pLightSetNode->GetLightNode( lightNo );

      if ( pLightNode->GetLightType() == LightType::Point || 
            pLightNode->GetLightType() == LightType::Spot )
      {
        math::AABB			lightAABB = pLightNode->GetWorldAABB();
        f32							len = lightAABB.GetDifference().Length3();

        if ( len > 0.0f )
        {
          if ( lightAABB.IsCross(rAABB) == false )
            continue;
        }
      }

      for( u32 i = 0; i < RenderState::LightMax; ++i )
      {
        if ( m_pNode[i] == NULL )
        {
          m_pNode[i] = pLightNode;
          pLightNode = NULL;//詰めた印にNULL
          break;
        }
      }

      if ( pLightNode == NULL )
        continue;

      //重複チェック
      for( u32 i = 0; i < RenderState::LightMax; ++i )
      {
        if( m_pNode[i]->GetPriority() == pLightNode->GetPriority() && 
          m_pNode[i]->GetLightType() == pLightNode->GetLightType() && 
          m_pNode[i]->GetNodeNameHash() == pLightNode->GetNodeNameHash() )
        {
          pLightNode = NULL;//詰めた印にNULL
          break;
        }
      }

      if ( pLightNode == NULL )
        continue;

      for( u32 i = 0; i < RenderState::LightMax; ++i )
      {
        if( m_pNode[i]->GetPriority() < pLightNode->GetPriority()  )
        {
          m_pNode[i] = pLightNode;
          break;
        }
      }
    }
  }
};

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void DrawManager::SetDrawEnvState( const DrawTag* pTarget )
{
  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  DrawEnvNode *pCurrEnv = NULL;
#endif  // GF_PLATFORM_CTR

	const math::AABB		*pAABB = pTarget->GetCullingWorldAABB();

	for( u32 envNo = 0; envNo < m_DrawEnvNodeContainer.size(); ++envNo )
	{
		DrawEnvNode					*pEnv = m_DrawEnvNodeContainer[ envNo ];

    if (!this->SetDrawEnvState_IsCrossAABB( pAABB, pEnv ))
    {
      continue;
    }
    this->SetDrawEnvState_LightPickUp( pTarget, pAABB, pEnv );

    //kawa20150507
#if defined(GF_PLATFORM_CTR)
    pCurrEnv = pEnv;
#endif  // GF_PLATFORM_CTR

		break;
	}

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  // フォグの設定
  // m_RenderStateにpEnvからフォグを取得し(フォグ番号があるときは番号の一致しているフォグを取得し)フォグノードを設定する。
  // 同じフォグなら、コマンド発行する必要があるフラグを立てない。
  // 違うフォグなら、コマンド発行する必要があるフラグを立てる。
  // 初回は必ず違うフォグになるようにしておく。
  // 初回とは「毎フレームのはじめ(Updateで初期化すればいい)」か「m_RenderState生存のはじめ(コンストラクタで初期化すればいい)」かどちらか。
  // どちらにすればいいかはコマンドの初期化をどう行っているかを見てそれに合わせればいい。
  // 今回はUpdateでm_RenderState.ResetCurrentFog();を呼んでいるので前者。
  if(pCurrEnv != m_RenderState.GetCurrentFogNode())
  {
    m_RenderState.SetCurrentFogNode(pCurrEnv);
    m_RenderState.SetCurrentFogCommandPutFlag(false);
  }
#endif  // GF_PLATFORM_CTR
}

bool DrawManager::SetDrawEnvState_IsCrossAABB( const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv )
{
  const math::AABB		envAABB = pEnv->GetWorldAABB();
  return envAABB.IsCross(*pAABB);
}

void DrawManager::SetDrawEnvState_LightPickUp( const DrawTag* pTarget, const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv )
{
  s32			lightSetNo = pTarget->GetLightSetNo();
  if ( lightSetNo < 0 )
  {
    return;
  }

  PickUpLight						pickUp;

  this->SetDrawEnvState_LightPickUp_PickUp( pickUp, pAABB, pEnv, lightSetNo );
  this->SetDrawEnvState_LightPickUp_AddLightNode( pickUp );
}

void DrawManager::SetDrawEnvState_LightPickUp_PickUp( class PickUpLight& pickUp, const math::AABB* pAABB, scenegraph::instance::DrawEnvNode* pEnv, s32 lightSetNo )
{
  const LightSetNode		*pLightSetNode = pEnv->GetLightSet( lightSetNo );

  if ( pLightSetNode )
  {
    pickUp.PickUp( pLightSetNode, *pAABB );
  }
  else
  {//補間AABBの場合、周囲のEnvよりLightを設定
    for( u32 linkEnvNo = 0; linkEnvNo< pEnv->GetLinkEnvNum(); ++linkEnvNo )
    {
      
      pLightSetNode = pEnv->GetLightSet( lightSetNo );
      pickUp.PickUp( pLightSetNode, *pAABB );
    }
  }
}

void DrawManager::SetDrawEnvState_LightPickUp_AddLightNode( class PickUpLight& pickUp )
{
  m_RenderState.RemoveAllLightNode();

  for( int i = 0; i < RenderState::LightMax; ++i )
  {
    if ( pickUp.m_pNode[i] == NULL )
      break;
    m_RenderState.SetLightNode( i, pickUp.m_pNode[i] );
  }
}

void DrawManager::ViewSpaceRenderEnable( b32 enable )
{
  RenderState::SetUpEnable      setUpEnable = m_RenderState.GetSetUpEnable();

  setUpEnable.VsIsWorldSpace = !enable;

  m_RenderState.SetSetUpEnable( setUpEnable );
}

bool DrawManager::IsViewSpaceRenderEnable() const
{
  return !m_RenderState.GetSetUpEnable().VsIsWorldSpace;
}

u32 DrawManager::GetAllTagNum( void ) const
{
  u32       allTagNum = 0;

  for( u32 nodeNo = 0; nodeNo < m_DrawableNodeContainer.size(); ++nodeNo )
  {
    allTagNum += m_DrawableNodeContainer[nodeNo]->GetDrawTagNum();
  }

  return allTagNum;
}

u32 DrawManager::GetVisibleTagNum( void ) const
{ 
  if( m_pOTOverride )
  {
    return m_pOTOverride->m_VisibleTagNum;
  }

  return m_OTInfo.m_VisibleTagNum; 
}

#if defined(GF_CONFIG_DEBUG) || defined(GF_CONFIG_DEVELOP)

//-----------------------------------------------------------------------------
/// @brief       OrderingTableコンテナ最大サイズ取得関数
/// @return      OrderingTableコンテナ最大サイズ
/// @attention   DrawManager::Update関数呼出し後に取得できます。
//-----------------------------------------------------------------------------
u32 DrawManager::GetOrderingTableMaxSize() const
{
  return m_Desc.m_DrawTagStackSize * EXECUTABLE_LAYER_MAX;
}

#endif  //  GF_CONFIG_DEBUG || GF_CONFIG_DEVELOP

#if GFL_DEBUG
//----------------------------------------------------------------------------
/**
  *   @brief デバッグ掲示関数(ワークサイズの使用状況報告)
  *
  *   @param pMessage         警告メッセージ
  *   @param usedBufferSize   使用しているサイズ
  *   @param maxBufferSize    ワーク最大サイズ
*/
//-----------------------------------------------------------------------------
void DrawManager::DebugSizeCheckDump( const c8* pMessage, u32 usedBufferSize, u32 maxBufferSize )
{
  #define   WARNING_PERCENTAGE    (0.9f)//使用率が９割を超えたら警告
  f32       usedPercentage = static_cast<f32>(usedBufferSize) / static_cast<f32>(maxBufferSize);

  if ( usedPercentage > WARNING_PERCENTAGE )
  {
    GFL_RELEASE_PRINT( "[gfx] warning : %s 使用サイズ %d / %d\n", pMessage, usedBufferSize, maxBufferSize );
  }
}
#endif

}}}// gfl2::renderingengine::renderer

