//===================================================================
/**
 * @file    FieldKusaModel.h
 * @brief   草モデルクラス
 * @author  ishiguro_masateru
 * @date    2015.05.23
 */
//===================================================================

#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

using namespace gfl2::gfx;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::renderer;
using namespace gfl2::renderingengine::scenegraph;

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( KusaModel )


const f32			FieldKusaModel::GrassSizeX = 75.0f;
const f32			FieldKusaModel::GrassSizeY = 75.0f;
const f32			FieldKusaModel::GrassXOffSet = 45.0f;
const f32			FieldKusaModel::KusaErrY = -5000.0f;

void FieldKusaModel::FieldKusaShaderDriver::FieldKusaConstants::Set( const renderer::RenderState& rRenderState, const scenegraph::resource::MaterialResourceNode *pMaterial )
{

}

FieldKusaModel::FieldKusaShaderDriver::FieldKusaShaderDriver() : 
  gfl2::gfx::GLMemory(),
  renderer::IShaderDriver(),
  m_pOrigDriver(NULL)
{
  m_FieldKusaConstantsBuffer = GFGL::CreateConstantBuffer( this->GetInstanceAllocator(), FieldKusaConstants::GetConstantBufferElement(), Usage::WriteOnly, Pool::Default );

	for( u32 i = 0; i < FieldKusaConstants::s_VisibleRegNum; ++i )
	{
		m_FieldKusaConstants.cVisible[i].Set(1, 1, 1, 1);
	}
}

FieldKusaModel::FieldKusaShaderDriver::~FieldKusaShaderDriver()
{
  if (m_FieldKusaConstantsBuffer)
  {
    delete m_FieldKusaConstantsBuffer;
    m_FieldKusaConstantsBuffer = NULL;
  }

  if (m_pOrigDriver)
  {
  	delete m_pOrigDriver;
    m_pOrigDriver = NULL;
  }
}

b32 FieldKusaModel::FieldKusaShaderDriver::Apply( renderer::RenderState& rRenderState, const renderer::MeshDrawTag *pMeshDrawTag ) const
{
	m_pOrigDriver->Apply( rRenderState, pMeshDrawTag );

#if defined(GF_PLATFORM_CTR)

	{
		nn::gr::CommandBufferJumpHelper		*pCommandBuffer = GFGL::GetJumpHelper();
		bit32															*cmd = pCommandBuffer->GetCurrentPtr();
		static const u32									s_KusaConstantsRegStart = 13;

		for( s32 i = 0; i < FieldKusaConstants::s_VisibleRegNum; ++i )
			cmd = nn::gr::CTR::MakeUniformCommandVS( cmd, i + s_KusaConstantsRegStart, *(nn::math::VEC4*)&m_FieldKusaConstants.cVisible[i] );

		pCommandBuffer->Put(cmd);
	}

#else
	m_FieldKusaConstantsBuffer->Update( &m_FieldKusaConstants, sizeof(FieldKusaConstants) );
	GFGL::SetVertexShaderConstant( 5, m_FieldKusaConstantsBuffer );
#endif

	return true;
}

void FieldKusaModel::FieldKusaShaderDriver::SetShapeDrawFlag( s32 index, b32 flag )
{
	int							row = index % 4;
	int							col = (index / 4) + 1;

	m_FieldKusaConstants.cVisible[col][row] = flag ? 1.0f : 0.0f;
}

FieldKusaModel* FieldKusaModel::CreateKusaModelInstanceNode( gfl2::gfx::IGLAllocator* pAllocator,
                                                            gfl2::renderingengine::scenegraph::resource::ResourceNode* pSrc,
                                                            gfl2::renderingengine::scenegraph::resource::ResourceNode* pTextureShaderNodeRoot )
{
	GFL_ASSERT(pSrc);
  GFL_ASSERT(pTextureShaderNodeRoot);
  gfl2::gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	FieldKusaModel								*pKusaModelInstanceNode = new FieldKusaModel();

	instance::InstanceCreator::SetupModelInstanceNode( pKusaModelInstanceNode, pSrc, pTextureShaderNodeRoot );

	SceneGraphManager::AddChild( pKusaModelInstanceNode );

  gfl2::gfx::GLMemory::ClearInstanceAllocator();

	return pKusaModelInstanceNode;
}

FieldKusaModel::~FieldKusaModel()
{
}

FieldKusaModel::FieldKusaModel()
{
	for( u32 x = 0; x < BlockXNum; ++x )
	{
		for( u32 y = 0; y < BlockYNum; ++y )
		{
			m_FieldKusaShaderDriverMtx[x][y] = NULL;
		}
	}
}

void FieldKusaModel::InitKusaShader( gfl2::gfx::IGLAllocator* pAllocator )
{
	gfl2::gfx::GLMemory::SetInstanceAllocator( pAllocator ); 

	u32		materialNum = this->GetMaterialNum();

	for( u32 materialNo = 0; materialNo < materialNum; ++materialNo )
	{
		scenegraph::instance::MaterialInstanceNode				*pMaterialInstanceNode;

		pMaterialInstanceNode = this->GetMaterialInstanceNode( materialNo );
		const c8*						pMaterialName = pMaterialInstanceNode->GetName();//blc??という名前はエクスポーターで自動生成される
		u8									block_x = pMaterialName[3] - '0';
		u8									block_y = pMaterialName[4] - '0';

		{
			FieldKusaShaderDriver			*pFieldKusaShaderDriver = new FieldKusaShaderDriver();

			pFieldKusaShaderDriver->m_pOrigDriver = pMaterialInstanceNode->GetShaderDriver();						
			pMaterialInstanceNode->SetShaderDriver( pFieldKusaShaderDriver );

			m_FieldKusaShaderDriverMtx[block_x][block_y] = pFieldKusaShaderDriver;
		}
	}

	this->SetUpDrawTag();

	gfl2::gfx::GLMemory::ClearInstanceAllocator();
}

void FieldKusaModel::SetShapeDrawFlag( s32 grid_x, s32 grid_y, b32 flag )
{
	GFL_ASSERT( grid_x < GrassXMax );
	GFL_ASSERT( grid_y < GrassYMax );

	u8							block_x = grid_x / BlockGrassXNum;
	u8							block_y = grid_y / BlockGrassYNum;

	s32							id;
	s32             blockXPos = grid_x % BlockGrassXNum;//ブロック内Ｘの位置
  s32             blockYPos = grid_y % BlockGrassYNum;//ブロック内Ｙの位置

  id = (blockYPos * BlockGrassXNum) + blockXPos;//ブロック内インデックス

	if ( m_FieldKusaShaderDriverMtx[block_x][block_y] )
		m_FieldKusaShaderDriverMtx[block_x][block_y]->SetShapeDrawFlag( id, flag );
}

FieldKusaModel::KusaGrid FieldKusaModel::GetKusaGrid( gfl2::math::Vector3 worldPos ) const
{
	FieldKusaModel::KusaGrid				grid;
	const gfl2::math::Matrix34			&rWorldMat = this->GetWorldMatrix();
	gfl2::math::Matrix34						worldInvMat = rWorldMat.Inverse();

	const gfl2::math::Matrix44			&rTransformMtx = this->m_pModelResourceNode-> GetModelTransform();
	gfl2::math::Matrix44						transformInvMtx = rTransformMtx.Inverse();
	
	worldPos = worldInvMat * worldPos;
	worldPos = transformInvMtx * worldPos;

	grid.grid_y = worldPos.z / GrassSizeY;
  if ( grid.grid_y % 2 )
    worldPos.x -= GrassXOffSet;//奇数行はオフセットされている。

  grid.grid_x = worldPos.x / GrassSizeX;

	return grid;
}

gfl2::math::Vector3 FieldKusaModel::GetKusaWorldPos( s32 grid_x, s32 grid_y ) const
{
	u32											index = (grid_y * GrassXMax) + grid_x;
	gfl2::math::Vector3			pos( 0.0f, KusaErrY, 0.0f );

	u32											indexMax = m_pModelResourceNode->GetUserDataSize() / sizeof(f32);
	const f32								*pHeight = static_cast<const f32*>( this->m_pModelResourceNode->GetUserData() );

	if ( index > indexMax || pHeight == NULL )
		return pos;

	pos.x = GrassSizeX * grid_x;
	if ( grid_y % 2 )
		pos.x += GrassXOffSet;
	pos.y = pHeight[index];

	pos.z = GrassSizeY * grid_y;

	const gfl2::math::Matrix34&			rWorldMat = this->GetWorldMatrix();
	const gfl2::math::Matrix44			&rTransformMtx = this->m_pModelResourceNode-> GetModelTransform();

	pos = rTransformMtx * pos;
	pos = rWorldMat * pos;
	
	return pos;
}


GFL_NAMESPACE_END( KusaModel )
GFL_NAMESPACE_END( Field )

