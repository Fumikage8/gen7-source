#ifndef GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODEFAACTRY_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODEFAACTRY_H_INCLUDED

//#include <iostream>
#include <memory>
#include <types/include/gfl2_Typedef.h>
#include <gfx/include/gfl2_GLMemory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

/**
	* @brief マテリアルリソースノードのファクトリインターフェース
	*/
class MaterialResourceNodeFactory : public IResourceNodeFactory
{
public:

  /**
	* @brief BinaryFileAccessor1.0 (GfModelVersion::MeshOptCTR15103116までのローダー)
	*/
  class BinaryFileAccessor10 : public MaterialResourceNode::IBinaryFileAccessor
  {
  public:

    struct MaterialColor
	  {
		  math::Vector4   m_Emission;
		  math::Vector4   m_Ambient;
		  math::Vector4   m_Diffuse;
		  math::Vector4   m_Specular0;
		  math::Vector4   m_Specular1;
	  };

    struct EtcColor
	  {
		  math::Vector4   m_BlendColor;
		  math::Vector4   m_BufferColor;
		  math::Vector4   m_TextureBorderColor[CombinerAttribute::TextureMax];
	  };

    struct AttributeParam{
      u32                                               m_Priority;         // 描画の優先度(描画ソートで使用, 0〜3の値しか取らない)
      u32                                               m_MaterialHash;     // マテリアルのハッシュ値
		  b32													                      m_PsLightingEnable;
		  b32													                      m_VsLightingEnable;
		  s16													                      m_LightSetNo;
		  b32													                      m_FogEnable;
		  s16													                      m_FogNo;
		  gfl2::gfx::CullMode					                      m_CullMode;
		
		  MaterialColor								                      m_MaterialColor;													//カラー/マテリアルカラー
      math::Vector4  							                      m_ConstantColor[CombinerAttribute::ConstantMax];						//カラー/コンスタントカラー
		  EtcColor                  										m_EtcColor;																//カラー/その他のカラー

		  s16													                      m_BumpMapNo;															//テクスチャ/バンプマッピング/バンプ使用テクスチャ
		  s16													                      m_BumpMapType;														//テクスチャ/バンプマッピング/使用方法
		  b32													                      m_ZRecastEnable;													//テクスチャ/バンプマッピング/Ｚ成分の再計算

		  MaterialResourceNode::TextureAttribute						m_TextureAttribute[CombinerAttribute::TextureMax];						//テクスチャ一覧/テクスチャ
	    b32													                      m_TextureAttributeEnable[CombinerAttribute::TextureMax];

		  b32													                      m_LookUpTableEnable[LookUpTableType::NumberOf];
		  MaterialResourceNode::LookUpTable									m_LookUpTable[LookUpTableType::NumberOf];	//フラグメント/フラグメントライティング
		  b32													                      m_PrimaryFresnelEnable;										//フラグメント/フラグメントライティング/フレネルをプライマリに適応
		  b32													                      m_SecondaryFresnelEnable;									//フラグメント/フラグメントライティング/フレネルをセカンダリに適応
		  b32													                      m_SpecularColorClamp;											//フラグメント/フラグメントライティング/スペキュラをクランプ
		  b32													                      m_CookTorrance0Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０
		  b32													                      m_CookTorrance1Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０

		  MaterialResourceNode::TextureCombinerAttribute		m_TextureCombinerAttribute[CombinerColorType::NumberOf][CombinerAttribute::CombinerMax];	//フラグメント/テクスチャコンバイナ
		  s16													                      m_TextureCombinerConstant[CombinerAttribute::CombinerMax];																//フラグメント/テクスチャコンバイナ/各コンスタントカラー

		  // 特殊設定
		  b32													                      m_IsPokemonShader;//CR様の対応が完了したら削除
		  b32													                      m_UseShadowTexture;
		  b32													                      m_UseObjectSpaceNormalMap;
		  b32													                      m_HighLightTextureEnable;
		  b32													                      m_OutLineColorTextureEnable;
		  b32													                      m_BlurScaleTextureEnable;
		  PokeNormalType							                      m_PokeNormalType;
		  BlendType										                      m_BlendType;
		  b32													                      m_UpdateShaderParam;
	  };

  #pragma pack(1)
	  struct AttributeParamSlim{
		  u32                         m_LutTextureNameHash[4];
		  s8													m_BumpMapNo;
		  s8													m_ConstantColorIndex[CombinerAttribute::CombinerMax];
		  s8													m_LightSetNo;
		  math::Vector4								m_ConstantColor[CombinerAttribute::ConstantMax];
		  b8													m_VsLightingEnable;
		  b8													m_PsLightingEnable;
		  b8													m_FogEnable;
		  s8													m_FogNo;
		  b8													m_VertexColorEnable[4];
		  b8													m_UseShadowTexture;
		  b8													dummy[3];

      math::Vector4               m_EmissionColor;
      math::Vector4               m_AmbientColor;
      math::Vector4               m_DiffuseColor;

		  static const u32						s_FirstSize = (sizeof(u32) * 4) + sizeof(s8) + 3;
		  static const u32						s_SupportConstantColorAnimSize = (sizeof(u32) * 4) + (sizeof(s8)*(1+CombinerAttribute::ConstantMax+1)) + (sizeof(math::Vector4)*CombinerAttribute::ConstantMax);
		  static const u32						s_VertexShaderUpdate15031412Size = (sizeof(u32) * 4) + (sizeof(math::Vector4)*CombinerAttribute::ConstantMax) + 12;
		  static const u32						s_VertexShaderUpdate15041820Size = (sizeof(u32) * 4) + (sizeof(math::Vector4)*CombinerAttribute::ConstantMax) + 12 + 4;
		  static const u32						s_SupportShadowTexture15090718Size = s_VertexShaderUpdate15041820Size + 4;
      static const u32						s_SupportMaterialColor15101716Size = s_SupportShadowTexture15090718Size + (sizeof(math::Vector4) * 3);//m_EmissionColor + m_AmbientColor + m_DiffuseColor
	  };
  #pragma pack()

    BinaryFileAccessor10::BinaryFileAccessor10( u32 versionId ) : MaterialResourceNode::IBinaryFileAccessor(), m_VersionId( versionId ){}
    virtual ~BinaryFileAccessor10(){}
    virtual void SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc );

  private:
    u32                       m_VersionId;
  };

  /**
	* @brief BinaryFileAccessor1.1 (GfModelVersion::ColorU8CTR15111314からのローダー)
	*/
  class BinaryFileAccessor11 : public MaterialResourceNode::IBinaryFileAccessor
  {
  public:

  #pragma pack(1)
	  struct AttributeParamSlim{
		  u32                         m_LutTextureNameHash[4];
		  s8													m_BumpMapNo;
		  s8													m_ConstantColorIndex[CombinerAttribute::CombinerMax];
		  s8													m_LightSetNo;
      gfx::ColorU8								m_ConstantColor[CombinerAttribute::ConstantMax];
		  b8													m_VsLightingEnable;
		  b8													m_PsLightingEnable;
		  b8													m_FogEnable;
		  s8													m_FogNo;
		  b8													m_VertexColorEnable[4];
		  b8													m_UseShadowTexture;
		  b8													dummy[3];

      gfx::ColorU8                m_EmissionColor;
      gfx::ColorU8                m_AmbientColor;
      gfx::ColorU8                m_DiffuseColor;
	  };
  #pragma pack()

    BinaryFileAccessor11( u32 versionId ) : MaterialResourceNode::IBinaryFileAccessor(), m_VersionId( versionId ){}
    virtual ~BinaryFileAccessor11(){}
    virtual void SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceNode::MaterialResourceData *pMaterialResourceData, const void *pSrc );

  private:
    u32                       m_VersionId;
  };

	MaterialResourceNodeFactory():IResourceNodeFactory(){}
	virtual ~MaterialResourceNodeFactory() {}

	/**
		* @brief ResourceNodeのファクトリメソッド　所有権付きのリソースノードを返す
		*/
	//virtual ResourceNode* CreateResourceNode(const void* data, int size);
  virtual ResourceNode* CreateResourceNode(gfx::IGLAllocator* pAllocator, const void* data, int size);

	virtual b32 IsSupport( IResourceData::Type type );
};

}}}}

#endif

