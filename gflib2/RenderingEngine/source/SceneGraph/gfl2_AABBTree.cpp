#include <RenderingEngine/include/SceneGraph/gfl2_AABBTree.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_CullingObjectNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

AABBTree* AABBTree::Create( gfx::IGLAllocator* pAllocator )
{
  gfx::GLMemory::SetInstanceAllocator(pAllocator);
	AABBTree* instance = new AABBTree();
  gfx::GLMemory::ClearInstanceAllocator();
  return instance;
}
	
AABBTree::AABBTree() : gfx::GLMemory(),
	m_pParentAABB( NULL ),
	m_pChildBoxArray( NULL ),
	m_pMemberTop( NULL ),
	m_fDirty( false ),
	m_Depth( 0 ),
	m_Index( 0 )
{

}

AABBTree::~AABBTree()
{
	if ( m_pChildBoxArray )
	{
		delete[] m_pChildBoxArray;
	}
}

void AABBTree::CreateChildBox( gfx::IGLAllocator* pAllocator, u32 depth )
{
	--depth;

	if ( m_pChildBoxArray == NULL )
	{
		gfx::GLMemory::SetInstanceAllocator(pAllocator);
		m_pChildBoxArray = new AABBTree[ChildBoxNum];
    gfx::GLMemory::ClearInstanceAllocator();

		math::Vector4			boxPos(1.0f, 1.0f, 1.0f, 0.0f);
		f32								pos[2] = { -1.0f, 1.0f };

		for( u32 x = 0; x < 2; ++x )
		{
			boxPos.x = pos[x];

			for( u32 y = 0; y < 2; ++y )
			{
				boxPos.y = pos[y];

				for( u32 z = 0; z < 2; ++z )
				{
					boxPos.z = pos[z];

					u32			boxIndex = boxPos.GetSignMask();

					math::Vector4			minPos;
					math::Vector4			maxPos;

					for( u32 elm = 0; elm < 3; ++elm )
					{
						if (boxPos[elm] == -1.0f)
						{
							minPos[elm] = m_AreaAABB.GetMin()[elm];
							maxPos[elm] = m_AreaAABB.GetCenter()[elm];
						}
						else
						{
							minPos[elm] = m_AreaAABB.GetCenter()[elm];
							maxPos[elm] = m_AreaAABB.GetMax()[elm];
						}
					}

					math::AABB			aabb( minPos, maxPos );
					m_pChildBoxArray[boxIndex].SetParent( this );
					m_pChildBoxArray[boxIndex].SetProperties( depth, boxIndex );
					m_pChildBoxArray[boxIndex].SetAreaAABB(aabb);

					if ( depth )
					{
						m_pChildBoxArray[boxIndex].CreateChildBox( pAllocator, depth );
					}
				}
			}
		}
	}
}

void AABBTree::AddObject( instance::CullingObjectNode* pNode )
{
	math::AABB			targetAABB = pNode->GetWorldAABB();
	math::Vector4		center = targetAABB.GetCenter();

	if ( m_pChildBoxArray )
	{
		math::Vector4		vec = center - m_AreaAABB.GetCenter();
		vec.w = 0.0f;

		u32							boxIndex = vec.GetSignMask();

		m_pChildBoxArray[boxIndex].AddObject( pNode );
	}
	else
	{
		if ( m_pMemberTop )
		{
			pNode->SetNext( m_pMemberTop );
		}

		m_pMemberTop = pNode;
		pNode->SetAABBTree( this );

		SetDirty( true );//あとでまとめて更新
	}
}

void AABBTree::RemoveObject( instance::CullingObjectNode* pNode )
{
	if ( m_pMemberTop == NULL )		return;

	instance::CullingObjectNode		*pPrev = NULL;
	instance::CullingObjectNode		*pSeek = m_pMemberTop;

	if ( m_pMemberTop == pNode )
	{
		m_pMemberTop = pNode->GetNext();
	}
	else
	{
		pPrev = m_pMemberTop;
		pSeek = m_pMemberTop->GetNext();

		while( pSeek )
		{
			if ( pSeek == pNode )
			{
				pPrev->SetNext( pNode->GetNext() );
				break;
			}

			pPrev = pSeek;
			pSeek = pSeek->GetNext();
		}
	}

	pNode->SetAABBTree( NULL );
	pNode->SetNext( NULL );
	SetDirty( true );//あとでまとめて更新
}

void AABBTree::SetDirty( b32 f )
{
	m_fDirty = f;

	if ( m_fDirty )
	{
		AABBTree*			pParent = this->m_pParentAABB;
		while( pParent )
		{
			pParent->m_fDirty = true;
			pParent = pParent->m_pParentAABB;
		}
	}
}

void AABBTree::UpdateVolumeAABB( u32 depth )
{
	if ( depth != m_Depth )
	{
		if ( m_pChildBoxArray )
		{
			for( u32 i = 0; i < ChildBoxNum; ++i )
			{
				m_pChildBoxArray[i].UpdateVolumeAABB( depth );
			}
		}
		return;
	}

	if ( m_fDirty == false )
		return;

	if ( m_pMemberTop )
	{//末端Box
		instance::CullingObjectNode		*pSeek = m_pMemberTop;

		m_DrawableVolumeAABB = m_AreaAABB;

		while( pSeek )
		{
			if ( pSeek->GetState() & instance::CullingObjectNode::State::DrawableNode )
				m_DrawableVolumeAABB = m_DrawableVolumeAABB.Merge( pSeek->GetWorldAABB() );

			pSeek = pSeek->GetNext();
		}
		SetDirty( false );//更新したのでフラグオフ
	}
	else
	{//親Box
		m_DrawableVolumeAABB = m_AreaAABB;

		if ( m_pChildBoxArray )
		{
			for( u32 i = 0; i < ChildBoxNum; ++i )
			{
				m_DrawableVolumeAABB = m_DrawableVolumeAABB.Merge( m_pChildBoxArray[i].m_DrawableVolumeAABB );
			}
		}
	}
}

void AABBTree::GetObjectsWithViewProjMatrix( gfx::GLFixedSizeContainer<instance::CullingObjectNode*> *pOutContainer, const math::Matrix44 &rViewProjMatrix, u32 mask ) const
{
	math::Matrix44				cullingMatrix[4];
	math::AABB::Util::CalcFrustumPlanes( cullingMatrix, rViewProjMatrix );

	this->GetObjectsWithFrustum( pOutContainer, cullingMatrix, mask );
}

void AABBTree::GetObjectsWithFrustum( gfx::GLFixedSizeContainer<instance::CullingObjectNode*> *pOutContainer, const math::Matrix44 *pCullingMatrices, u32 mask ) const
{
	if ( math::AABB::Util::IsIntersectFrustum( pCullingMatrices, m_DrawableVolumeAABB ) == false )
		return;

	const scenegraph::AABBTree	*pChild = this->GetChildTree();

	if ( pChild )
	{
		for( u32 i = 0; i < scenegraph::AABBTree::ChildBoxNum; ++i )
		{
			pChild[i].GetObjectsWithFrustum( pOutContainer, pCullingMatrices, mask );
		}
	}
	else
	{
		instance::CullingObjectNode			*pSeek = m_pMemberTop;
		u32															hitCnt = 0;
		
		while( pSeek )
		{
			if ( pSeek->GetState() & mask )
			{
				pOutContainer->push_back( pSeek );
				++hitCnt;
			}
			pSeek = pSeek->GetNext();
		}
	}
}

}}}

