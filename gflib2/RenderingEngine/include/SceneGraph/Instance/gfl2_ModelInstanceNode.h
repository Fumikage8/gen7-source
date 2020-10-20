//=============================================================================
/**
 * @file    gfl2_ModelInstanceNode.h
 * @brief   ModelInstanceNodeクラス（各種リソース、インスタンスノードより構成されるモデルインスタンスノード。ジョイントインスタンスを持つ。）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.11   ishiguro  SetOptionalRenderKey関数追加
 *  2016.4.8    ishiguro  CopyMaterialDepthStencilStateToStateObject関数追加
 *
 */
//=============================================================================
#pragma once
#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MODELINSTANCENODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MODELINSTANCENODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawableNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/renderer/gfl2_MeshDrawTag.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;

class ModelInstanceNode : public NodeTrait<ModelInstanceNode, DrawableNode>, public renderer::DrawTag::IDrawListener
{
public:

	friend class InstanceCreator;
	friend class SceneGraphManager;
  friend class ModelInstanceNodeReferenceCountVisitor;

	class MeshLinkData : public gfx::GLMemory
	{
	public:
		friend class ModelInstanceNode;

		struct AnimationData
		{
			struct BlendMeshData
			{
				BlendMeshData():m_pBlendMesh(NULL),m_BlendWeight(0){}
				const MeshLinkData														*m_pBlendMesh;
				f32																						m_BlendWeight;
			};

      AnimationData() :
        m_IsVisible(true)
      {
      }

      bool                      m_IsVisible;
			BlendMeshData							m_BlendMeshData[Constant::BLEND_MESH_MAX];
		};

		MeshLinkData();
		virtual ~MeshLinkData();

		inline const c8* GetName() const { return m_pMeshResourceNode->GetName(); }
		inline u32 GetNameHash() const { return m_pMeshResourceNode->GetNameHash(); }

		inline u32 GetFaceNum() const { return m_pMeshResourceNode->GetFaceNum(); }
		inline const resource::MeshResourceNode::FaceData* GetFaceData( u32 no ) const { return m_pMeshResourceNode->GetFaceData( no ); }

    inline void SetVisible(bool isVisible)
    {
      m_AnimationData.m_IsVisible = isVisible;
    }
    inline bool IsVisible() const
    {
      return m_AnimationData.m_IsVisible;
    }

		inline void SetBlendMeshData( u32 blendMeshNo, const MeshLinkData *pBlendMesh, f32 blendWeight )
		{
			m_AnimationData.m_BlendMeshData[blendMeshNo].m_pBlendMesh = pBlendMesh;
			m_AnimationData.m_BlendMeshData[blendMeshNo].m_BlendWeight = blendWeight;
		}

		inline const math::AABB& GetAABB() const { return m_pMeshResourceNode->GetAABB(); }

    inline const resource::MeshResourceNode* GetMeshResourceNode() const
    {
      return m_pMeshResourceNode;
    }

	private:
		void LinkData( const resource::MeshResourceNode* pMesh, const MaterialInstanceNode* pMaterials, u32 materialNum );

		const resource::MeshResourceNode*							m_pMeshResourceNode;
		const MaterialInstanceNode										**m_pMaterialInstanceNodeList;

		AnimationData																	m_AnimationData;
	};

	virtual ~ModelInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::TransformNode::CullingObjectNode::DrawableNode::ModelInstanceNode") ) );
		return hash;
	}

	void SetShaderResourceNode( const resource::ShaderResourceNode **pShaderResourceNodeList, u32 size );
	void SetTextureResourceNode( const resource::TextureResourceNode **pTextureResourceNodeList, u32 size );
	void SetMaterialResourceNode( const resource::MaterialResourceNode **pMaterialResourceNodeList, u32 size );
	void SetGfMdlResourceNode( const resource::GfMdlResourceNode *pModelResourceNode );
	void SetMeshResourceNode( const resource::MeshResourceNode **pMeshResourceNodeList, u32 size );
	void LinkNode();

	virtual u32 GetDrawTagNum();
	virtual void UpdateDrawTag(const DrawableNode::UpdateDrawTagInfo& updateDrawTagInfo);
	virtual renderer::DrawTag* GetDrawTag( u32 no );
	inline renderer::MeshDrawTag* GetMeshDrawTag( u32 no );
	virtual void PreDraw(renderer::RenderState&);
	virtual void PostDraw(renderer::RenderState&);

	virtual void StateChangeCallBack( u32 beforeState );

	u32 GetMaterialNum() const { return m_MaterialResourceNum; }
	MaterialInstanceNode* GetMaterialInstanceNode( u32 no ){ return &m_pMaterialInstanceNodeList[no]; }
	MaterialInstanceNode* GetMaterialInstanceNodeByNameHash( u32 nameHash );

  const resource::GfMdlResourceNode* GetModelResourceNode() const { return m_pModelResourceNode; }

  u32 GetTextureNum() const { return m_TextureResourceNum; }

	u32 GetJointNum() const
  {
    return (m_pJointReferenceModel)
      ? m_pJointReferenceModel->GetJointNum()
      : m_JointNum;
  }
	JointInstanceNode* GetJointInstanceNode( u32 no )
  {
    return (m_pJointReferenceModel)
      ? m_pJointReferenceModel->GetJointInstanceNode(no)
      : &m_JointInstanceNodeList[no];
  }
  const ModelInstanceNode* GetJointReferenceModel() const
  {
    return m_pJointReferenceModel;
  }

	u32 GetMeshNum() const { return m_MeshResourceNum; }
	u32 GetMehsNum() const { return m_MeshResourceNum; }
	MeshLinkData* GetMeshLinkData( u32 no ){ return &m_pMeshLinkDataList[no]; }
	const MeshLinkData* GetMeshLinkData( u32 no ) const { return &m_pMeshLinkDataList[no]; }
  u32 GetTotalVertexNum() const;
  u32 GetTotalIndexNum() const;
  u32 GetTotalPolygonNum() const;

	//! @brief DrawTagの初期化。DrawTagの設定に関わる変更を行った際は、実行してください。
	void InitializeDrawTag();
	void SetUpDrawTag();
	void SetDrawCheckListener( b32 enable );

  void SetMotionConfigInstanceNode(InstanceNode* pMotionConfigResourceNode)
  {
    m_pMotionConfigInstanceNode = pMotionConfigResourceNode;
  }
  const InstanceNode* GetMotionConfigInstanceNode() const
  {
    return m_pMotionConfigInstanceNode;
  }
  InstanceNode* GetMotionConfigInstanceNode()
  {
    return m_pMotionConfigInstanceNode;
  }

	static const u8									s_DefaultPriority = 128;
	void SetSortPriority( u8 value ){ m_SortPriority = value; }
	u8 GetSortPriority() const { return m_SortPriority; }

	u32 GetReferenceCnt() const;

  const c8* GetNodeName( void ) const { return m_pModelResourceNode->GetNodeName(); }


#if defined(GF_PLATFORM_CTR)
  /**
   * @brief マテリアルの初期値をステートオブジェクトにコピーする
   * @@attention ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
   */
  void CopyMaterialDepthStencilStateToStateObject();
#endif

  void SetOptionalRenderKey( void ) { m_RenderKeyType = true; }


protected:
	ModelInstanceNode(ModelInstanceNode* pJointReferenceModel = NULL);

	void ReferenceCntInc();
	void ReferenceCntDec();

	class ModelLinkData : public GLMemory
	{
	public:
		ModelLinkData();
		~ModelLinkData();

		void LinkData( const resource::GfMdlResourceNode* pModel, const MeshLinkData* pMeshs, u32 meshNum );

		const resource::GfMdlResourceNode					*m_pModelResourceNode;
		const MeshLinkData												**m_pMeshLinkData;
	};

	u32															m_MaterialResourceNum;
	u32															m_TextureResourceNum;
	u32															m_ShaderResourceNum;
	u32															m_MeshResourceNum;
	u32															m_JointNum;

	const resource::GfMdlResourceNode			*m_pModelResourceNode;
	const resource::MaterialResourceNode	**m_pMaterialResourceNodeList;
	const resource::TextureResourceNode		**m_pTextureResourceNodeList;
	const resource::ShaderResourceNode		**m_pShaderResourceNodeList;
	const resource::MeshResourceNode			**m_pMeshResourceNodeList;

	MaterialInstanceNode						*m_pMaterialInstanceNodeList;
	MeshLinkData										*m_pMeshLinkDataList;
	ModelLinkData										m_ModelLinkData;
	JointInstanceNode								*m_JointInstanceNodeList;
  InstanceNode                    *m_pMotionConfigInstanceNode;

	u32															m_MeshDrawTagListNum;
	renderer::MeshDrawTag						*m_pMeshDrawTagList;
	math::AABB											m_WorldAABB;
	b32															m_fPreDraw;
	u32															m_GpuReferenceCnt;

	u8															m_SortPriority;
  b8                              m_RenderKeyType;  //優先度ソートのタイプ指定

  ModelInstanceNode               *m_pJointReferenceModel;
};

inline renderer::MeshDrawTag* ModelInstanceNode::GetMeshDrawTag( u32 no )
{
	GFL_ASSERT( no < m_MeshDrawTagListNum );
	return &m_pMeshDrawTagList[no];
}

}}}}

#endif

