//=============================================================================
/**
 * @file    gfl2_MaterialInstanceNode.h
 * @brief   MaterialInstanceNodeクラス（マテリアルリソースを参照するインスタンスノード。個別メンバを持つ）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.8    ishiguro  CopyMaterialDepthStencilStateToStateObject関数追加
 *
 */
//=============================================================================
#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MaterialInstanceNODE_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_INSTANCE_MaterialInstanceNODE_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>

#include <RenderingEngine/include/Renderer/gfl2_IShaderDriver.h>

#define MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY 1

//@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
//                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
//                使う側に混乱が起きないよう、nijiでは封印します。
//                本来は実装されるべき機能です。
#define MATERIAL_SAMPLER_STATE_OBJECT_ENABLE 0

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {

class InstanceCreator;
class ModelInstanceNode;

class MaterialInstanceNode : public NodeTrait<MaterialInstanceNode, InstanceNode>
{
public:

	friend class InstanceCreator;
	friend class ModelInstanceNode;

	virtual ~MaterialInstanceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash		hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::InstanceNode::MaterialInstanceNode") ) );
		return hash;
	}

  void LinkData( const resource::MaterialResourceNode* pMaterial,
                 const resource::TextureResourceNode** pTextures, u32 textureNum,
                 const resource::ShaderResourceNode** pShaders, u32 shaderNum );

	const c8* GetName() const { return m_pMaterialResourceNode->GetName(); }
  u32 GetNameHash() const { return m_pMaterialResourceNode->GetNameHash(); }

  const resource::MaterialResourceNode* GetMaterialResourceNode() const { return m_pMaterialResourceNode; }

  u32 GetTextureNum() const { return m_TextureNum; }

  const resource::TextureResourceNode* GetTextureResourceNode(u32 textureNo) const { return m_pTextureResourceNodeList[textureNo]; }
  const renderer::IShaderDriver* GetShaderDriver() const { return m_pShaderDriver; }
  renderer::IShaderDriver* GetShaderDriver() { return m_pShaderDriver; }

#if MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
  //@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
  //                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
  //                使う側に混乱が起きないよう、nijiでは封印します。
  //                本来は実装されるべき機能です。
	const gfx::SamplerStateObject* GetSamplerStateObject( u32 no ) const
  {
#if MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
    return m_pMaterialResourceNode->GetSamplerStateObject(no);
#else
    return m_pSamplerStateObjectList[no].get();
#endif
  }
#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
  
	gfx::BlendStateObject* GetBlendStateObject() const
  {
#if MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
    return m_pMaterialResourceNode->GetBlendStateObject();
#else
    return m_pBlendStateObject.get();
#endif
  }

	gfx::DepthStencilStateObject* GetDepthStencilStateObject() const
  {
#if MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
    return m_pMaterialResourceNode->GetDepthStencilStateObject();
#else
    return m_pDepthStencilStateObject.get();
#endif
  }

  gfx::RasterizerStateObject* GetRasterizerStateObject() const
  {
#if MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
    return m_pMaterialResourceNode->GetRasterizerStateObject();
#else
    return m_pRasterizerStateObject.get();
#endif
  }

	resource::MaterialResourceNode::TextureInfo* GetTextureInfo( u32 no )
  {
    GFL_ASSERT( no < MATERIAL_TEXTURE_MAX );
    return &m_TextureInfoList[no];
  }
	const resource::MaterialResourceNode::TextureInfo* GetTextureInfo( u32 no ) const
  {
    GFL_ASSERT( no < MATERIAL_TEXTURE_MAX );
    return &m_TextureInfoList[no];
  }

	resource::MaterialResourceNode::AttributeParam* GetAttributeParam()
  {
    return &m_AtrributeParam;
  }
	const resource::MaterialResourceNode::AttributeParam* GetAttributeParam() const
  {
    return &m_AtrributeParam;
  }

  void SetShaderDriver(renderer::IShaderDriver* pShaderDriver)
  {
    m_pShaderDriver = pShaderDriver;
  }

  u32 GetTextureNumOriginal() const
  {
    return m_TextureResourceNodeListNumOriginal;
  }

  const resource::TextureResourceNode* GetTextureResourceNodeOriginal(u32 no) const
  {
    return m_pTextureResourceNodeListOriginal[no];
  }

  void SetTextureResourceNode(u32 no, const resource::TextureResourceNode* pTexture)
  {
    GFL_ASSERT( no < MATERIAL_TEXTURE_MAX );
    m_pTextureResourceNodeList[no] = pTexture;
  }

	const resource::MaterialResourceNode::UserData& GetUserData() const
	{
		return m_pMaterialResourceNode->GetUserData();
	}

#if defined(GF_PLATFORM_CTR)
  static const u32				MATERIAL_TEXTURE_MAX = 3;

	void SetLutCommand( u32 no, const u32 *pCommand, u32 size ){ m_pLutCommand[no] = pCommand; m_LutCommandSize[no] = size; }
	const u32* GetLutCommand( u32 no ) const { return m_pLutCommand[no]; }
	u32 GetLutCommandSize( u32 no ) const { return m_LutCommandSize[no]; }
	u32 GetLutCommandNameHash( u32 no ) const { return m_pMaterialResourceNode->GetLutCommandNameHash(no); }

#elif defined(GF_PLATFORM_WIN32)
  static const u32				MATERIAL_TEXTURE_MAX = 16;

#endif

  const scenegraph::resource::ShaderResourceNode* SetVertexShaderResourceNode() const { return m_pVertexShaderResourceNode; }
	const scenegraph::resource::ShaderResourceNode* SetGeometryShaderResourceNode() const { return m_pGeometryShaderResourceNode; }
	const scenegraph::resource::ShaderResourceNode* SetPixelShaderResourceNode() const { return m_pPixelShaderResourceNode; }

#if defined(GF_PLATFORM_CTR)
  /**
   * @brief マテリアルの初期値をステートオブジェクトにコピーする
   * @@attention ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
   */
  void CopyMaterialDepthStencilStateToStateObject()
  {
#if MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
    resource::MaterialResourceNode        *pMaterialResourceNode = const_cast<resource::MaterialResourceNode*>(m_pMaterialResourceNode);
    pMaterialResourceNode->CopyMaterialDepthStencilStateToStateObject();
#endif
  }
#endif

protected:
	MaterialInstanceNode();

private:

  const resource::TextureResourceNode**				                m_pTextureResourceNodeListOriginal;
  u32                                                         m_TextureResourceNodeListNumOriginal;

  const resource::MaterialResourceNode*				                m_pMaterialResourceNode;
  const resource::ShaderResourceNode*					                m_pVertexShaderResourceNode;
	const resource::ShaderResourceNode*					                m_pGeometryShaderResourceNode;
	const resource::ShaderResourceNode*					                m_pPixelShaderResourceNode;
  u32																					                m_TextureNum;
  const resource::TextureResourceNode*				                m_pTextureResourceNodeList[ MATERIAL_TEXTURE_MAX ];
  renderer::IShaderDriver*                                    m_pShaderDriver;

  // ここから書き換え用にリソースノードからコピーするもの
#if !MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY
  gfx::SamplerStateObject                      *m_pSamplerStateObjectList[MATERIAL_TEXTURE_MAX];
  gfx::BlendStateObject                        *m_pBlendStateObject;
  gfx::DepthStencilStateObject                 *m_pDepthStencilStateObject;
#endif
	resource::MaterialResourceNode::TextureInfo                 m_TextureInfoList[MATERIAL_TEXTURE_MAX];

  resource::MaterialResourceNode::AttributeParam              m_AtrributeParam;

#if defined(GF_PLATFORM_CTR)
	const u32																	*m_pLutCommand[resource::MaterialResourceNode::LUT_MAX];
	u32																				m_LutCommandSize[resource::MaterialResourceNode::LUT_MAX];
#endif

  //-----------------------------------------------------------------
  // Fog : フォグ設定はマテリアル単位ではなくScene単位であり、GfEnv等から上書き設定されることを想定しているので、
  //	   マテリアルリソースノードには書かずインスタンスノードのみに書く
  //-----------------------------------------------------------------
public:
  // フォグカラーの取得
  gfl2::math::Vector4	GetFogColor(void) const 				{ return m_FogColor; }
  // フォグパラメータの取得
  gfl2::math::Vector4	GetFogParam(void) const 				{ return m_FogParam; }
  // フォグタイプの取得
  int					GetFogType (void) const 				{ return m_PsFogType; }
  // フォグカラーの設定
  void					SetFogColor(gfl2::math::Vector4 color)	{ m_FogColor = color; }
  // フォグパラメータの設定
  void					SetFogParam(gfl2::math::Vector4 param)	{ m_FogParam = param; }
  // フォグタイプの設定
  void					SetFogType (int psFogType)				{ m_PsFogType = psFogType; }
private:
  gfl2::math::Vector4	m_FogColor;		// (r, g, b, 1.0f)
  gfl2::math::Vector4	m_FogParam;		// (FogIntencity, FogNear, FogFar, 1.0f)
  int					m_PsFogType;	// None=0;Linear;Log;Log2;
};

}}}}

#endif

