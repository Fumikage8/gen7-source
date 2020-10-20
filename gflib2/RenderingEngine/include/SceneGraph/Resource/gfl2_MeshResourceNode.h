#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MESHRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MESHRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_MathAABB.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class MeshResourceNodeFactory;

class MeshResourceNode : public NodeTrait<MeshResourceNode, ResourceNode>, public NodeName
{
public:
	friend class MeshResourceNodeFactory;

	class FaceData : public gfx::GLMemory
	{
	public:
		friend class MeshResourceNode;

		FaceData();
		~FaceData();

		u32					m_MaterialNameHash;
    c8					m_MaterialName[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];
		u8					m_JointNum;
		const u8		*m_pJointList;
		u32					m_VertexNum;
		u32					m_IndexNum;
		u32					m_JointWeightMax;

#if defined(GF_PLATFORM_CTR)

		class CommandType{
		public:
			enum {
				VertexAttrEnable = 0,
				VertexAttrDisable,
				Draw,
				Max
			};
		};

		const u32* GetCommand( u32 type ) const { return m_pCommandTop[type]; }
		u32 GetCommandSize( u32 type ) const { return m_CommandSize[type]; }

		const u8* GetVertexBuffer() const { return m_pVertexBuffer; }
    u32 GetVertexBufferSize() const { return m_VertexBufferSize; } 
		const u8* GetIndexBuffer() const { return m_pIndexBuffer; }

#elif defined(GF_PLATFORM_WIN32)

		const gfx::VertexBuffer* GetVertexBuffer() const { return m_pVertexBuffer; }
		const gfx::IndexBuffer* GetIndexBuffer() const { return m_pIndexBuffer; }

#endif

	private:
		
#if defined(GF_PLATFORM_CTR)
		

		u32					m_CommandSize[CommandType::Max];
		u32					*m_pCommandTop[CommandType::Max];

		const u8		*m_pVertexBuffer;
		const u8		*m_pIndexBuffer;
		u32					m_VertexBufferSize;
		u32					m_IndexBufferSize;

#elif defined(GF_PLATFORM_WIN32)

		gfx::VertexBuffer													*m_pVertexBuffer;
		gfx::IndexBuffer													*m_pIndexBuffer;

#endif

	};

	virtual ~MeshResourceNode();

	inline const c8* GetName() const { return GetNodeName(); }
	inline u32 GetNameHash() const { return m_NameHash; }

	void SetData( const void* data, int size );

	inline u32 GetFaceNum() const { return m_FaceDataNum; }
	inline const FaceData* GetFaceData( u32 no ) const { return &m_pFaceDataList[no]; }

  u32 GetTotalVertexNum() const;
  u32 GetTotalIndexNum() const;
  u32 GetTotalPolygonNum() const { return GetTotalIndexNum() / 3; } 


	inline b32 IsBlendShape() const { return m_IsBlendShape; }
	inline const math::AABB& GetAABB() const { return m_AABB; }
	inline u32 GetJointWeightMax() const { return m_JointWeightMax; }

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::MeshResourceNode") ) );
		return hash;
	}

  inline u32 GetVertexElementSize() const
  {
    return m_VertexElements.size();
  }

  inline const gfx::VertexElement* GetVertexElement(u32 no) const
  {
    return &m_VertexElements[no];
  }

protected:
	MeshResourceNode();

private:
	util::FixedSizeContainer<gfx::VertexElement, 16>				m_VertexElements;
	u32																											m_StreamSize;
	u32																											m_NameHash;
	u32																											m_IsBlendShape;
	math::AABB																							m_AABB;
	u32						m_FaceDataNum;
	u32						m_JointWeightMax;
	FaceData			*m_pFaceDataList;


};

}}}}

#endif

