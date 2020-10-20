//=============================================================================
/**
 * @file    gfl2_MaterialResourceNode.h
 * @brief   MaterialResourceNodeクラス（マテリアルのリソースを持つノード）
 * @author  ishiguro_masateru
 * @date    2014
 *
 *  2016.4.8    ishiguro  CopyMaterialDepthStencilStateToStateObject関数追加とm_SamplerStateObjectの削除
 *  2016.4.7    ishiguro  SetUserDataShaderParam関数追加
 *
 */
//=============================================================================
#if !defined(GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODE_H_INCLUDED)
#define GFLIB2_RENDERINGENGINE_SCENEGRAPH_RESOURCE_MATERIALRESOURCENODE_H_INCLUDED

#include <RenderingEngine/include/Renderer/gfl2_MaterialTypes.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector4.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_ColorU8.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class MaterialResourceNodeFactory;


class MaterialResourceNode : public NodeTrait<MaterialResourceNode, ResourceNode>, public NodeName
{
public:
	friend class MaterialResourceNodeFactory;

	static const u32									LUT_MAX = 3;

	struct MaterialColor
	{
		gfx::ColorU8   m_Emission;
		gfx::ColorU8   m_Ambient;
		gfx::ColorU8   m_Diffuse;
		gfx::ColorU8   m_Specular0;
		gfx::ColorU8   m_Specular1;
	};

	struct EtcColor
	{
		gfx::ColorU8   m_BlendColor;
		gfx::ColorU8   m_BufferColor;
		gfx::ColorU8   m_TextureBorderColor[CombinerAttribute::TextureMax];
	};

	struct LookUpTable
	{
		LutInputAngleType			m_LutInputAngle;
		f32										m_LutScale;
		u32										m_TextureSlotNo;

		LookUpTable() : m_LutInputAngle(0),m_LutScale(0){}
	};

	struct TextureCombinerAttribute
	{
		s16			m_MathFormula;
		f32			m_Scale;
		s16			m_InputSource[InputSourceType::NumberOf];
		s16			m_Operand[InputSourceType::NumberOf];
		b32			m_OutPutBuffer;
	};

	struct TextureAttribute
	{
		s16		  m_MappingType;
		s16		  m_UvSetNo;
		f32		  m_ScaleU;
		f32		  m_ScaleV;
		f32		  m_Rotate;
		f32		  m_TranslateU;
		f32		  m_TranslateV;
		s16		  m_RepeatU;
		s16		  m_RepeatV;
		s16		  m_MagFilter;
		s16		  m_MinFilter;
	};

	struct Header{
		u32				nameHash;
		c8				nameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];
		u32				typeNameHash;
		c8				typeNameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];
		u32				shaderNameHash;
		c8				shaderNameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];

    /**
     * @brief コンストラクタ
     */
    Header() :
      nameHash(0),
      typeNameHash(0),
      shaderNameHash(0)
    {
    }
	};

	struct TextureInfo
	{
		struct Attribute
		{
			u32						m_UvSetNo;
			u32						m_MappingType;
			f32						m_ScaleU;
			f32						m_ScaleV;
			f32						m_Rotate;
			f32						m_TranslateU;
			f32						m_TranslateV;
			u32						m_RepeatU;
			u32						m_RepeatV;
			u32						m_MagFilter;
			u32						m_MinFilter;
		  f32						m_MipBias;
		};

		u32								m_TextureNameHash;
		c8								m_TextureName[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];
		u32								m_SlotNo;
		u32								m_Usage;
		u32								m_UvUsage;
		Attribute					m_Attribute;
	};

#if defined(GF_PLATFORM_WIN32)
	struct AttributeParam{
    u32                         m_Priority;         // 描画の優先度(描画ソートで使用, 0〜3の値しか取らない)
    u32                         m_MaterialHash;     // マテリアルのハッシュ値
		b32													m_PsLightingEnable;
		b32													m_VsLightingEnable;
		s16													m_LightSetNo;
		b32													m_FogEnable;
		s16													m_FogNo;
		gfl2::gfx::CullMode					m_CullMode;
		
		MaterialColor								m_MaterialColor;													//カラー/マテリアルカラー
    gfx::ColorU8  							m_ConstantColor[CombinerAttribute::ConstantMax];						//カラー/コンスタントカラー
		EtcColor										m_EtcColor;																//カラー/その他のカラー

		s16													m_BumpMapNo;															//テクスチャ/バンプマッピング/バンプ使用テクスチャ
		s16													m_BumpMapType;														//テクスチャ/バンプマッピング/使用方法
		b32													m_ZRecastEnable;													//テクスチャ/バンプマッピング/Ｚ成分の再計算

		TextureAttribute						m_TextureAttribute[CombinerAttribute::TextureMax];						//テクスチャ一覧/テクスチャ
	  b32													m_TextureAttributeEnable[CombinerAttribute::TextureMax];

		b32													m_LookUpTableEnable[LookUpTableType::NumberOf];
		LookUpTable									m_LookUpTable[LookUpTableType::NumberOf];	//フラグメント/フラグメントライティング
		b32													m_PrimaryFresnelEnable;										//フラグメント/フラグメントライティング/フレネルをプライマリに適応
		b32													m_SecondaryFresnelEnable;									//フラグメント/フラグメントライティング/フレネルをセカンダリに適応
		b32													m_SpecularColorClamp;											//フラグメント/フラグメントライティング/スペキュラをクランプ
		b32													m_CookTorrance0Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０
		b32													m_CookTorrance1Enable;										//フラグメント/フラグメントライティング/ジオメトリファクタ０

		TextureCombinerAttribute		m_TextureCombinerAttribute[CombinerColorType::NumberOf][CombinerAttribute::CombinerMax];	//フラグメント/テクスチャコンバイナ
		s16													m_TextureCombinerConstant[CombinerAttribute::CombinerMax];																//フラグメント/テクスチャコンバイナ/各コンスタントカラー

		// 特殊設定
		b32													m_IsPokemonShader;//CR様の対応が完了したら削除
		b32													m_UseShadowTexture;
		b32													m_UseObjectSpaceNormalMap;
		b32													m_HighLightTextureEnable;
		b32													m_OutLineColorTextureEnable;
		b32													m_BlurScaleTextureEnable;
		PokeNormalType							m_PokeNormalType;
		BlendType										m_BlendType;
		b32													m_UpdateShaderParam;
	};

#elif defined(GF_PLATFORM_CTR)

  struct AttributeParam{
    
    u32                         m_MaterialHash;     // マテリアルのハッシュ値
		gfl2::gfx::CullMode					m_CullMode;
		
		MaterialColor								m_MaterialColor;													//カラー/マテリアルカラー
    gfx::ColorU8								m_ConstantColor[CombinerAttribute::ConstantMax];						//カラー/コンスタントカラー
		TextureAttribute						m_TextureAttribute[CombinerAttribute::TextureMax];						//テクスチャ一覧/テクスチャ

    //PokeNormalType							m_PokeNormalType;
		//BlendType										m_BlendType;

    s8													m_LightSetNo;
		s8													m_FogNo;
		s8													m_BumpMapNo;															//テクスチャ/バンプマッピング/バンプ使用テクスチャ
		s8													m_TextureCombinerConstant[CombinerAttribute::CombinerMax];																//フラグメント/テクスチャコンバイナ/各コンスタントカラー

    u8                          m_Priority;         // 描画の優先度(描画ソートで使用, 0〜3の値しか取らない)
    b8													m_PsLightingEnable;
		b8													m_VsLightingEnable;
    b8													m_FogEnable;

		// 特殊設定
		b8													m_IsPokemonShader;//CR様の対応が完了したら削除
		b8													m_UseShadowTexture;
		b8													m_UseObjectSpaceNormalMap;
		b8													m_HighLightTextureEnable;
		b8													m_OutLineColorTextureEnable;
		b8													m_BlurScaleTextureEnable;
		b8													m_UpdateShaderParam;
	};
#endif

	struct BlendTest
	{
		struct AlphaTest
		{
			gfx::CompareFunc						m_AlphaTestType;
			f32													m_AlphaTestRefValue;

			inline bool  operator ==( const AlphaTest& rhs ) const
			{
				if ( m_AlphaTestType != rhs.m_AlphaTestType )					return false;
				if ( m_AlphaTestRefValue != rhs.m_AlphaTestRefValue )	return false;
				return true;
			}
		};
		struct DepthTest
		{
			gfx::CompareFunc						m_DepthTestType;
			b32													m_UpDateDepth;
			s32													m_PolygonOffset;

			inline bool  operator ==( const DepthTest& rhs ) const
			{
				if ( m_DepthTestType != rhs.m_DepthTestType )		return false;
				if ( m_UpDateDepth != rhs.m_UpDateDepth )				return false;
				if ( m_PolygonOffset != rhs.m_PolygonOffset )		return false;
				return true;
			}
		};
		struct StencilTest
		{
			b32													m_StencilTestEnable;
			gfx::CompareFunc						m_StencilTestType;
			u8													m_StencilValue;
			u8													m_StencilMask;
			gfx::StencilOp							m_StencilFailOp;
			gfx::StencilOp							m_StencilZFailOp;
			gfx::StencilOp							m_StencilZpassOp;

			inline bool  operator ==( const StencilTest& rhs ) const
			{
				if ( m_StencilTestEnable != rhs.m_StencilTestEnable )		return false;
				if ( m_StencilTestType != rhs.m_StencilTestType )				return false;
				if ( m_StencilValue != rhs.m_StencilValue )							return false;
				if ( m_StencilMask != rhs.m_StencilMask )								return false;
				if ( m_StencilFailOp != rhs.m_StencilFailOp )						return false;
				if ( m_StencilZFailOp != rhs.m_StencilZFailOp )					return false;
				if ( m_StencilZpassOp != rhs.m_StencilZpassOp )					return false;
				return true;
			}
		};
		struct Blender
		{
			renderer::BlendMode					m_BlendMode;
			renderer::LogicalOperation	m_LogicalOperation;
			struct Color
			{
				gfx::BlendOp								m_BlendFunc;
				renderer::BlendElement			m_SrcElement;
				renderer::BlendElement			m_DstElement;
				inline bool  operator ==( const Color& rhs ) const
				{
					if ( m_BlendFunc != rhs.m_BlendFunc )					return false;
					if ( m_SrcElement != rhs.m_SrcElement )				return false;
					if ( m_DstElement != rhs.m_DstElement )				return false;
					return true;
				}
			};
			struct Alpha
			{
				gfx::BlendOp								m_BlendFunc;
				renderer::BlendElement			m_SrcElement;
				renderer::BlendElement			m_DstElement;
				inline bool  operator ==( const Alpha& rhs ) const
				{
					if ( m_BlendFunc != rhs.m_BlendFunc )					return false;
					if ( m_SrcElement != rhs.m_SrcElement )				return false;
					if ( m_DstElement != rhs.m_DstElement )				return false;
					return true;
				}
			};
			Color													m_Color;
			Alpha													m_Alpha;

			inline bool  operator ==( const Blender& rhs ) const
			{
				if ( m_BlendMode != rhs.m_BlendMode )									return false;
				if ( m_LogicalOperation != rhs.m_LogicalOperation )		return false;
				if ( !(m_Color == rhs.m_Color) )											return false;
				if ( !(m_Alpha == rhs.m_Alpha) )											return false;
				return true;
			}
		};

    scenegraph::resource::DrawLayer	m_DrawLayer;
		AlphaTest			m_AlphaTest;
		DepthTest			m_DepthTest;
		StencilTest		m_StencilTest;
		Blender				m_Blender;

		inline bool  operator ==( const BlendTest& rhs ) const
		{
			if ( m_DrawLayer != rhs.m_DrawLayer )					return false;
			if ( !(m_AlphaTest == rhs.m_AlphaTest) )			return false;
			if ( !(m_DepthTest == rhs.m_DepthTest) )			return false;
			if ( !(m_StencilTest == rhs.m_StencilTest) )	return false;
			if ( !(m_Blender == rhs.m_Blender) )					return false;
			return true;
		}
	};

	struct UserData
	{
		s32												m_EdgeType;
		b32															m_IDEdgeEnable;
		u32															m_EdgeID;
		s32									m_ProjectionType;
		f32															m_RimPow;
		f32															m_RimScale;
		f32															m_PhongPow;
		f32															m_PhongScale;
		b32															m_IDEdgeOffsetEnable;
		s32		m_EdgeMapAlphaMask;
		s32									m_BakeTextures[CombinerAttribute::UvTextures::NumberOf];
    s32									m_BakeConstants[CombinerAttribute::ConstantMax];
		VertexShaderType								m_VertexShaderType;
		f32															m_ShaderParam0;   // シェーダー汎用パラメーター    ファイヤーシェーダーではビルボードの大きさ(スケール)で使用
		f32															m_ShaderParam1;   // シェーダー汎用パラメーター    メガゲンガーシェーダーではエフェクトの高さで使用
		f32															m_ShaderParam2;   // シェーダー汎用パラメーター    ファイヤーシェーダーではテクスチャパターン数で使用
		f32															m_ShaderParam3;   // シェーダー汎用パラメーター    メガゲンガーシェーダーではエフェクト開始位置(地面の位置)で使用
	};

  struct MaterialResourceData
  {
    MaterialResourceData():
#if defined(GF_PLATFORM_CTR)
      m_pMaterialCommandTop(NULL),
      m_CommandLength(0),
#endif
      m_FShaderNameHash(0),
      m_TextureNum(0),
      m_pTextureInfoList(NULL)
    {
#if defined(GF_PLATFORM_CTR)
      for (u32 i = 0; i < LUT_MAX; ++i)
      {
        m_LutCommandNameHash[i] = 0;
      }
      for (u32 i = 0; i < 4; ++i)
      {
        m_VertexColorEnable[i] = false;
      }
#endif
    }

	  Header																		m_Header;

	  u32																				m_FShaderNameHash;
	  c8																				m_FShaderNameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];
	  u32																				m_TextureNum;
	  TextureInfo																*m_pTextureInfoList;
	  AttributeParam														m_AttributeParam;
	  BlendTest																	m_BlendTest;
	  UserData																	m_UserData;

#if defined(GF_PLATFORM_CTR)
	//最適化拡張
	const u8																	*m_pMaterialCommandTop;
	u32																				m_LutCommandNameHash[LUT_MAX];
	u32																				m_CommandLength;
	b8																				m_VertexColorEnable[4];
#endif
  };

  class IBinaryFileAccessor
  {
  public:
    virtual ~IBinaryFileAccessor(){}

    virtual void SetAttributeParam( gfx::IGLAllocator *pAllocator, MaterialResourceData *pMaterialResourceData, const void *pSrc ) = 0;

  protected:
    IBinaryFileAccessor(){}
  };

	virtual ~MaterialResourceNode();

	static util::FnvHash GetTypeHash()
	{
		static util::FnvHash			hash( reinterpret_cast<u8*>( const_cast<char*>("Node::DagNode::ResourceNode::MaterialResourceNode") ) );
		return hash;
	}

  void SetData( IBinaryFileAccessor *pBinaryFileAccessor, const void* data );
  void CreateStateObject();
#if defined(GF_PLATFORM_CTR)
  /**
   * @brief マテリアルの初期値をステートオブジェクトにコピーする
   * @@attention ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
   */
  void CopyMaterialDepthStencilStateToStateObject();
#endif

	const c8* GetName() const { return m_MaterialResourceData.m_Header.nameBuffer; }
	u32 GetNameHash() const { return m_MaterialResourceData.m_Header.nameHash; }
	const c8* GetShaderTypeName() const { return m_MaterialResourceData.m_Header.typeNameBuffer; }
	u32 GetShaderTypeNameHash() const { return m_MaterialResourceData.m_Header.typeNameHash; }
	const c8* GetShaderName( gfx::ShaderType type = gfx::ShaderType::Vertex ) const;
	u32 GetShaderNameHash( gfx::ShaderType type = gfx::ShaderType::Vertex ) const;

	const Header& GetHeader() const { return m_MaterialResourceData.m_Header; }
	u32 GetTextureNum() const { return m_MaterialResourceData.m_TextureNum; }
	const TextureInfo& GetTextureInfo( u32 no ) const { return m_MaterialResourceData.m_pTextureInfoList[no]; }
	TextureInfo& GetTextureInfo( u32 no ) { return m_MaterialResourceData.m_pTextureInfoList[no]; }

#if defined(GF_PLATFORM_WIN32)
  const gfx::SamplerStateObject* GetSamplerStateObject( u32 no ) const { return m_SamplerStateObject[no]; }
#elif defined(GF_PLATFORM_CTR)
  //@attention      ＣＴＲでは不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
  //                DepthStencilStateObjectのサイズ増加があったため、m_SamplerStateObject削除することでメモリサイズを調整いたします。
  //                本来は実装されるべき機能です。
  const gfx::SamplerStateObject* GetSamplerStateObject( u32 no ) const { return NULL; }
#endif
	
	const gfx::SamplerStateObject* GetDummySamplerStateObject() const { return m_DummySamplerStateObject; }
	gfx::BlendStateObject* GetBlendStateObject() const { return m_BlendStateObject; }
	gfx::DepthStencilStateObject* GetDepthStencilStateObject() const { return m_DepthStencilStateObject; }
	gfx::RasterizerStateObject* GetRasterizerStateObject() const { return m_RasterizerStateObject; }

	AttributeParam* GetAttributeParam(){ return &m_MaterialResourceData.m_AttributeParam; }
	const AttributeParam& GetAttributeParam() const { return m_MaterialResourceData.m_AttributeParam; }

	BlendTest* GetBlendTest(){ return &m_MaterialResourceData.m_BlendTest; }
	const BlendTest& GetBlendTest() const { return m_MaterialResourceData.m_BlendTest; }
	const UserData& GetUserData() const { return m_MaterialResourceData.m_UserData; }

  // --------------------------------------------------------------------------
  /**
   * @brief  UserDataの汎用シェーダーパラメーター設定
   * @param  no       汎用シェーダーパラメーター番号設定（０～３）
   * @param  value    セットしたい値
   */
  // --------------------------------------------------------------------------
  void SetUserDataShaderParam( u32 no, f32 value );

  static const u32    MEGAGANGAR_EFFECT_STATE_START_Y_PARAM_NO = 1;//SetUserDataShaderParam関数でエフェクト開始位置(地面の位置)を設定する番号

#if defined(GF_PLATFORM_CTR)
	const u8* GetMaterialCommand() const { return m_MaterialResourceData.m_pMaterialCommandTop; }
	u32 GetMaterialCommandSize() const { return m_MaterialResourceData.m_CommandLength; }

	u32 GetLutCommandNameHash( u32 no ) const { return m_MaterialResourceData.m_LutCommandNameHash[no]; }
	b8 GetVertexColorEnable( u32 no ) const { return m_MaterialResourceData.m_VertexColorEnable[no]; }
#endif

protected:
	MaterialResourceNode();
	
private:
  
  MaterialResourceData                      m_MaterialResourceData;

#if defined(GF_PLATFORM_WIN32)
  //@attention      ＣＴＲでは不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
  //                DepthStencilStateObjectのサイズ増加があったため、m_SamplerStateObject削除することでメモリサイズを調整いたします。
  //                本来は実装されるべき機能です。
	gfx::SamplerStateObject										*m_SamplerStateObject[Constant::TEXTURE_MAX];//(4 x 16) byte
#endif

	gfx::BlendStateObject											*m_BlendStateObject;
	gfx::DepthStencilStateObject							*m_DepthStencilStateObject;
	gfx::RasterizerStateObject							  *m_RasterizerStateObject;
	gfx::SamplerStateObject										*m_DummySamplerStateObject;

};

}}}}

#endif

