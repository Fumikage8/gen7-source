#ifndef GFLIB2_RENDERINGENGINE_RENDERER_RENDERSTATE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_RENDERSTATE_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <math/include/gfl2_Matrix.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightSetNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

class IShaderDriver;

class RenderState
{
public:
	friend class DrawManager;

	static const u32						LightMax = scenegraph::instance::LightSetNode::LightMax;

	union ShaderStateMap
  {
    struct
    {
      u16		value;
    };
    struct
    {
			u16		m_IsSkining				: 1;
			u16		m_LightingEnable	: 1;
			u16		m_Uv0Enable				: 1;
			u16		m_Uv1Enable				: 1;
			u16		m_Uv2Enable				: 1;
			u16		m_Texture1Enable	: 1;
			u16		m_Texture2Enable	: 1;
			u16		m_PhongLightingEnable	: 1;
			u16		m_RimLightingEnable	: 1;
			u16		m_InverseLightingEnable	: 1;
			u16		m_NormalMapgEnable	: 1;
			u16		m_WeightWEnable	: 1;

			u16		padding						: 4;
    };
		ShaderStateMap():value(0){}
  };

	union SetUpEnable
  {
    struct
    {
      u32		value;
    };
    struct
    {
			u32		VertexShader			: 1;
			u32		GeometryShader		: 1;
			u32		FragmentShader		: 1;
			u32		Lighting					: 1;
			u32		Material					: 1;
			u32		UvSetting					: 1;
      u32		VsIsWorldSpace		: 1;

			u32		padding						: 25;
    };
		SetUpEnable():value(0xFFFFFFFF){}
  };

	RenderState::RenderState():
		m_Projection(NULL),
		m_View(NULL),
		m_World(NULL),
		m_pJointNodeList(NULL),
		m_JointNum(0),
		m_pOverrideShader(NULL)
    //kawa20150507
#if defined(GF_PLATFORM_CTR)
    , m_pCurrentFogNode(NULL),
    m_IsCurrentFogCommandPutFlag(false)
#endif  // GF_PLATFORM_CTR
	{
		for( u32 i = 0; i < Constant::VERTEX_BUFFER_MAX; ++i )
			m_BlendWeight[i] = 0.0f;

    this->RemoveAllLightNode();

		m_SetUpEnable.value = 0xFFFFFFFF;

		for( u32 i = 0; i < scenegraph::resource::CombinerAttribute::TextureMax; ++i )
			m_TextureInfoOverride[i] = NULL;
	}

	~RenderState(){}

	inline void SetProjectionMatrix( const math::Matrix44* mat ){ GFL_ASSERT(mat);m_Projection = mat; }
	inline const math::Matrix44* GetProjectionMatrix() const { return m_Projection; }

	inline void SetViewMatrix( const math::Matrix34* mat ){ GFL_ASSERT(mat);m_View = mat; }
	inline const math::Matrix34* GetViewMatrix() const { return m_View; }

	inline void SetWorldMatrix( const math::Matrix34* mat ){ GFL_ASSERT(mat);m_World = mat; }
	inline const math::Matrix34* GetWorldMatrix() const { return m_World; }

	inline void SetJointInstanceNode( const scenegraph::instance::JointInstanceNode *pNode ){ m_pJointNodeList = pNode; }
	inline const scenegraph::instance::JointInstanceNode* GetJointInstanceNode() const { return m_pJointNodeList; }

	inline void SetJointNum( u32 num ){ m_JointNum = num; }
	inline u32 GetJointNum() const { return m_JointNum; }

	inline void SetBlendMeshWeight( u32 index, f32 weight ){ m_BlendWeight[index] = weight; }
	inline f32 GetBlendMeshWeight( u32 index ) const { return m_BlendWeight[index]; }

	inline const IShaderDriver* GetOverrideShader() const { return m_pOverrideShader; }

	inline void SetLightNode( u32 no, const scenegraph::instance::LightNode* pNode );
	inline void RemoveAllLightNode();

	inline u32 GetLightNum() const;
	inline const scenegraph::instance::LightNode* GetLightNode( u32 no ) const;

	inline void SetShaderStateMap( ShaderStateMap state ){ m_ShaderStateMap = state; }
	inline ShaderStateMap GetShaderStateMap() const { return m_ShaderStateMap; }

	inline void SetSetUpEnable( SetUpEnable state ){ m_SetUpEnable = state; }
	inline const SetUpEnable& GetSetUpEnable() const { return m_SetUpEnable; }

	inline void SetTextureInfoOverride( u32 no, const scenegraph::resource::MaterialResourceNode::TextureInfo* pInfo ){ m_TextureInfoOverride[no] = pInfo; }
	inline const scenegraph::resource::MaterialResourceNode::TextureInfo* GetTextureInfoOverride( u32 no ) const { return m_TextureInfoOverride[no]; }

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  inline void ResetCurrentFog(void) { m_pCurrentFogNode = NULL; m_IsCurrentFogCommandPutFlag = false; }
  inline scenegraph::instance::DrawEnvNode* GetCurrentFogNode(void) const { return m_pCurrentFogNode; }
  inline void SetCurrentFogNode(scenegraph::instance::DrawEnvNode* pCurrentFogNode) { m_pCurrentFogNode = pCurrentFogNode; }
  inline bool GetCurrentFogCommandPutFlag(void) const { return m_IsCurrentFogCommandPutFlag; }
  inline void SetCurrentFogCommandPutFlag(bool isCurrentFogCommandPutFlag) { m_IsCurrentFogCommandPutFlag = isCurrentFogCommandPutFlag; }
#endif  // GF_PLATFORM_CTR

private:

	const math::Matrix44																													*m_Projection;   // 透視変換行列
	const math::Matrix34																													*m_View;					// ビュー変換行列
	const math::Matrix34																													*m_World;				// ワールド変換行列

	const scenegraph::instance::JointInstanceNode																	*m_pJointNodeList;
	u32																																						m_JointNum;

	f32																																						m_BlendWeight[Constant::VERTEX_BUFFER_MAX];
	const IShaderDriver																														*m_pOverrideShader;
	//util::FixedSizeContainer< const scenegraph::instance::LightNode*, LightMax >	m_LightNodeContianer;
  const scenegraph::instance::LightNode*                                        m_LightNodeList[LightMax];
	ShaderStateMap																																m_ShaderStateMap;
	SetUpEnable																																		m_SetUpEnable;

	const scenegraph::resource::MaterialResourceNode::TextureInfo									*m_TextureInfoOverride[scenegraph::resource::CombinerAttribute::TextureMax];

  //kawa20150507
#if defined(GF_PLATFORM_CTR)
  // フォグ
  scenegraph::instance::DrawEnvNode                                             *m_pCurrentFogNode;            // 現在のフォグ
  bool                                                                          m_IsCurrentFogCommandPutFlag;  // m_pCurrentFogNodeのコマンドを発行済みならtrue
#endif  // GF_PLATFORM_CTR

};

inline void RenderState::SetLightNode( u32 no, const scenegraph::instance::LightNode* pNode )
{
  m_LightNodeList[no] = pNode;
}

inline const scenegraph::instance::LightNode* RenderState::GetLightNode( u32 no ) const
{
	if ( no < LightMax )
		return m_LightNodeList[no];

	GFL_ASSERT( 0 );
	return NULL;
}

inline void RenderState::RemoveAllLightNode()
{
  for (u32 i = 0; i < LightMax; ++i)
  {
    m_LightNodeList[i] = NULL;
  }
}

inline u32 RenderState::GetLightNum() const
{
  for (u32 i = 0; i < LightMax; ++i)
  {
    if (m_LightNodeList[i] == NULL)
    {
      return i;
    }
  }

  return LightMax;
}

}}}

#endif

