#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMDLRESOURCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_GFMDLRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class GfMdlResourceNodeFactory;

class GfMdlResourceNode : public NodeTrait<GfMdlResourceNode, ResourceNode>, public NodeName
{
#ifdef GF_PLATFORM_CTR
  typedef math::Matrix34 MatrixType;
#else
  typedef math::Matrix44 MatrixType;
#endif

public:
	friend class GfMdlResourceNodeFactory;

	struct ElementName
	{
		util::FnvHash				m_NameHash;
    c8									m_Name[ Constant::MODEL_ELEMENT_NAME_MAX ];
	};

	struct JointData
	{
		JointData()
		{
			::std::memset( m_Name, 0, sizeof(m_Name) );
			::std::memset( m_ParentName, 0, sizeof(m_ParentName) );
			
			m_Status32 = 0; // Clear Status Completely
			m_BitStatus.m_NeededRendering = false;
			m_BitStatus.m_ScaleSegmentCompensate = false;

			m_LocalMatrix = MatrixType::GetIdentity();
			m_GlobalMatrix = MatrixType::GetIdentity();
		}

		c8									m_Name[Constant::JOINT_NAME_MAX];
		c8									m_ParentName[Constant::JOINT_NAME_MAX];
		MatrixType		  		m_LocalMatrix;
		MatrixType  				m_GlobalMatrix;
		
		// 4 Byte Status:
		union
		{
			struct
			{
				unsigned char m_NeededRendering : 1;
        unsigned char m_ScaleSegmentCompensate : 1;
        unsigned char m_BillboardType : 2;
				unsigned char m_padding : 4;
			} m_BitStatus;
			u8 m_Status[4];
			u32 m_Status32;
		};			

    f32                 m_LocalScale[3];
    f32                 m_LocalRotation[3];
    f32                 m_LocalTranslate[3];

#ifdef GF_PLATFORM_CTR
    s16                 m_ParentIndex;
    s8                  m_padding[2];
#endif
	};

	struct UserDataHeader{
		u32								m_UserDataSize;
		u32								m_DefaultUserDataSize;
		u32								m_Padding[2];
	};

	struct DefaultModelUserData{
		BillboardType			m_BillboardType;
		u32								m_Padding[3];
	};

	virtual ~GfMdlResourceNode();

	void SetData( const void* data, int size, bool createJointFlag );

	u32 GetShaderNum() const { return m_ShaderNum; };
	u32 GetTextureNum() const { return m_TextureNum; }
	u32 GetMaterialNum() const { return m_MaterialNum; }
	u32 GetMeshNum() const { return m_MeshNum; };
	u32 GetJointNum() const { return m_JointNum; };

	const math::AABB& GetAABB() const { return m_AABB; }

	const c8* GetShaderName( u32 no ) const ;
	const c8* GetTextureName( u32 no ) const ;
	const c8* GetMaterialName( u32 no ) const ;
	const c8* GetMeshName( u32 no ) const ;
	const JointData* GetJointData( u32 no ) const ;

	util::FnvHash GetShaderNameHash( u32 no ) const ;
	util::FnvHash GetTextureNameHash( u32 no ) const ;
	util::FnvHash GetMaterialNameHash( u32 no ) const ;
	util::FnvHash GetMeshNameHash( u32 no ) const ;

	u32 GetLutTableNum() const { return m_LutTableNum; }
	const u32* GetLutTableCommand( u32 nameHash ) const;
	u32 GetLutTableCommandSize() const { return m_LutTableDataSize; }

	const math::Matrix44& GetModelTransform() const { return m_TransMat; }

	u32 GetUserDataSize() const { return m_UserDataSize; }
	const void* GetUserData() const { return m_pUserData; }

	u32 GetDefaultUserDataSize() const { return m_DefaultUserDataSize; }
	const DefaultModelUserData* GetDefaultUserData() const { return m_pDefaultUserData; }

  b8 IsStableJoint() const { return m_IsStableJoint; }

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::GfMdlResourceNode") ) );
		return hash;
	}

protected:
	GfMdlResourceNode();

private:

	u32								m_TextureNum;
	u32								m_MaterialNum;
	u32								m_MeshNum;
	u32								m_ShaderNum;
	u32								m_JointNum;
	math::AABB				m_AABB;
	math::Matrix44		m_TransMat;
	u32								m_UserDataSize;
	const void				*m_pUserData;
	u32								m_DefaultUserDataSize;
	const DefaultModelUserData		*m_pDefaultUserData;
	ElementName				*m_pElementNameList;
	JointData					*m_JointDataList;
	u32								m_LutTableNum;
	u32								m_LutTableDataSize;
	u32								*m_pLutTableDataTop;
  b8                m_IsStableJoint;
};

}}}}

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif