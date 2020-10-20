#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_AABBTree_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_AABBTree_H_INCLUDED

#include <gfx/include/gfl2_GLMemory.h>
#include <math/include/gfl2_Matrix44.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph {

namespace instance {
	class CullingObjectNode;
}

class AABBTree : public gfx::GLMemory
{
public:
	friend class SceneGraphManager;

	static AABBTree* Create(gfx::IGLAllocator* pAllocator);

	static const u32								ChildBoxNum = 8;

	virtual ~AABBTree();

	inline void SetAreaAABB( const math::AABB &rAABB ){ m_AreaAABB = rAABB; }
	inline const math::AABB& GetAreaAABB() const { return m_AreaAABB; }

	inline void SetDrawableVolumeAABB( const math::AABB &rAABB ){ m_DrawableVolumeAABB = rAABB; }
	inline const math::AABB& GetDrawableVolumeAABB() const { return m_DrawableVolumeAABB; }

	void CreateChildBox( gfx::IGLAllocator* pAllocator, u32 depth );

	//! @name オブジェクト登録
	//@{
		void AddObject( instance::CullingObjectNode* node );//!< オブジェクトを追加
		void RemoveObject( instance::CullingObjectNode* node );	//!< オブジェクトを削除
	//@}

	//! @name 更新
	//@{
		void UpdateVolumeAABB( u32 depth );								//!< VolumeAABBを更新
	//@}

	//! @name オブジェクト検索
	//@{
		//! 視推台カリングをしてから特定の条件のものを取得
		void GetObjectsWithViewProjMatrix( gfx::GLFixedSizeContainer<instance::CullingObjectNode*> *pOutContainer, const math::Matrix44 &rViewProjMatrix, u32 mask ) const ;
	//@}

	void SetDirty( b32 f );

	const AABBTree* GetChildTree() const { return m_pChildBoxArray; }
	const instance::CullingObjectNode* GetCullingObjectNodeTop() const { return m_pMemberTop; }

	void SetProperties( u32 depth, u32 index ){ m_Depth = depth; m_Index = index; }

protected:
	AABBTree();

private:

	void GetObjectsWithFrustum( gfx::GLFixedSizeContainer<instance::CullingObjectNode*> *pOutContainer, const math::Matrix44 *pCullingMatrices, u32 mask ) const ;

	void SetParent( AABBTree *pParentAABB ){ m_pParentAABB = pParentAABB; }

	math::AABB											m_AreaAABB;
	math::AABB											m_DrawableVolumeAABB;
	AABBTree												*m_pParentAABB;
	AABBTree												*m_pChildBoxArray;

	instance::CullingObjectNode			*m_pMemberTop;
	b32															m_fDirty;

	u32															m_Depth;
	u32															m_Index;

};

}}}

#endif

