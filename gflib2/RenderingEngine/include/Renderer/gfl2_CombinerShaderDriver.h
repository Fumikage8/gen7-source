#ifndef GFLIB2_RENDERINGENGINE_RENDERER_COMBINERSHADERDRIVER_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_RENDERER_COMBINERSHADERDRIVER_H_INCLUDED

#if defined(GF_PLATFORM_WIN32)

#include <gfx/include/gfl2_GFGL.h>
#include <RenderingEngine/include/renderer/gfl2_IShaderDriver.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ShaderResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_JointInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MeshResourceNode.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 { namespace renderingengine { namespace renderer { 

class CombinerShaderDriver : public gfx::GLMemory, public IShaderDriver
{
public:

	//static CombinerShaderDriver* Create();
	static CombinerShaderDriver* Create( gfx::IGLAllocator* pAllocator );

	virtual ~CombinerShaderDriver();

	void SetMaterialInstanceNode( const scenegraph::instance::MaterialInstanceNode* pMaterial ){ GFL_ASSERT( pMaterial );m_pMaterial = pMaterial; }
	void SetVertexShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ GFL_ASSERT(pShader);m_pVertexShader = pShader; }
	void SetGeometryShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ GFL_ASSERT(pShader);m_pGeometryShader = pShader; }
	void SetPixelShaderResourceNode( const scenegraph::resource::ShaderResourceNode* pShader ){ GFL_ASSERT(pShader);m_pPixelShader = pShader; }

	virtual b32 Apply( RenderState& rRenderState, const MeshDrawTag* pMeshDrawTag ) const;

protected:

	CombinerShaderDriver();

	struct cFloatArray{
		float				data[4];

		cFloatArray&  operator = ( const float& rhs )
		{
			this->data[0] = rhs;
			return *this;
		}
	};

	struct cIntArray{
		int				data[4];

		cIntArray&  operator = ( const int& rhs )
		{
			this->data[0] = rhs;
			return *this;
		}
	};

	struct cBoolArray{
		int			data[4];

		cBoolArray&  operator = ( const int& rhs )
		{
			this->data[0] = rhs;
			return *this;
		}
	};

	typedef		unsigned int			cBool;
	typedef		math::Vector4			vec4;
	typedef		math::Matrix44			mat4x4;
	#define		cbuffer						struct

	static const int UvMax = 3;
	static const int TextureMax = 4;
	static const int CombinerStageMax = 6;
	static const int LutMax = 6;
	static const int LightMax = 8;
	static const int SourceTypeMax = 3;
	static const int OperandTypeMax = 10;
	static const int ColorTypeMax = 2;

  cbuffer ColorShaderConstants
	{
    math::Vector4 cInterpColor;      // PCビューア用. カラーシェーダ対応

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Vector, 1 },   // PCビューア用. カラーシェーダ対応
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const renderer::RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial )
    {
      (void)rRenderState;
      (void)pMaterial;

      cInterpColor.x = 0.0f;
      cInterpColor.y = 0.0f;
      cInterpColor.z = 0.0f;
      cInterpColor.w = 0.0f;
    }
  };

	cbuffer VertexShaderVectorConstants
	{
		math::Matrix44	cVsModelMatrix;
		math::Matrix44	cVsViewMatrix;
		math::Matrix44	cVsModelViewMatrix;		// Maya上ではModelView行列しか取れないが, ランタイムではModelとViewを分けている.
		math::Matrix44	cVsProjectionMatrix;
		math::Matrix44	cVsNoramlRotate;
		math::Vector4	cVsWorldLightVec;
		math::Vector4	cVsWorldLightColor;
		math::Vector4	cVsAmbientLightColor;
		math::Vector4	cVsCameraPosition;
		math::Vector4	cVsObjSpaceCameraPosition;
		math::Vector4	cVsRimParam;
		math::Vector4	cVsShaderParam;
		math::Vector4	cVsBlendMeshWeight;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial, const MeshDrawTag* pMeshDrawTag );
	};

	cbuffer VertexShaderJointMatrixConstants
	{
		math::Matrix44	cVsJointMatrix[255];
		cBool					cVsSkiningEnable;
		cBool					cVsPadding[3];

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Matrix, 255 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 3 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const MeshDrawTag* pMeshDrawTag, const scenegraph::resource::MeshResourceNode::FaceData *pFace, const scenegraph::instance::JointInstanceNode* pNode, u32 size );
    void DumpModelData( const MeshDrawTag* pMeshDrawTag, const scenegraph::resource::MeshResourceNode::FaceData *pFace, const scenegraph::instance::JointInstanceNode* pNode, u32 size );
	};

	cbuffer VertexShaderBoolConstants
	{
		cBool		cVsHalfVecLightEnable;
		cBool		cVsVertexColorEnable;
		cBool		cVsSceneContrastEnable;
		cBool		cVsLightingEnable;

		cBool		cIsPokemonShader;
		cBool		cIsMegaGangarShader;
		cBool		cUseObjectSpaceNormalMap;
		cBool		cRotateSphereMapEnable;

		cBool		cVsFog;
		cBool		cVsNormalToUv;
		cBool		cIsMegaYamiramiShader;
		cBool		cIsReflectShader;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer VertexShaderIntArrayConstants
	{
		cIntArray			cMappingType[TextureMax];
		cIntArray     cUvSetNo[TextureMax];

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::IntArray, TextureMax },
				{ gfx::ConstantType::IntArray, TextureMax },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer GeometryShaderVectorConstants
	{
		math::Matrix44	cGsViewMatrix;
		math::Matrix44	cGsProjectionMatrix;
		math::Vector4	cGsShaderParam;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Matrix, 1 },
				{ gfx::ConstantType::Vector, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial, const MeshDrawTag* pMeshDrawTag );
	};

	cbuffer PixelShaderVectorConstants
	{
		math::Vector4 cPsObjSpaceCameraPosition;

		//-----カラー-------
		math::Vector4 cEmission;
		math::Vector4 cAmbient;
		math::Vector4 cDiffuse;
		math::Vector4 cSpecular0;
		math::Vector4 cSpecular1;
		math::Vector4 cConsntantColor[CombinerStageMax];
		math::Vector4 cBufferColor;

		//-------- ライト情報 --------------
		math::Vector4 cPsObjSpaceLightPos[LightMax];
		math::Vector4 cPsObjSpaceLightColor[LightMax];
		math::Vector4 cPsObjSpaceLightVec[LightMax];
		math::Vector4 cPsAmbientLightColor;

		//-------- 参照テーブル情報 --------------
		math::Vector4 cPsCookTorranceParam;
		
		math::Vector4 cGaussianOffset[16];
		math::Vector4 cFsParam[2];
		math::Vector4 cFsScreanSize;

		//-----------Fog------------
		math::Vector4 cPsFogColor;	//(r, g, b, 1.0f)
		math::Vector4 cPsFogParam;	//(FogIntencity, FogNear, FogFar, 1.0f)
		s32			  cPsFogType;	//None=0;Linear;Log;Log2;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, CombinerStageMax },
				{ gfx::ConstantType::Vector, 1 },

				{ gfx::ConstantType::Vector, LightMax },
				{ gfx::ConstantType::Vector, LightMax },
				{ gfx::ConstantType::Vector, LightMax },
				{ gfx::ConstantType::Vector, 1 },

				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 16 },
				{ gfx::ConstantType::Vector, 2 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Vector, 1 },
				{ gfx::ConstantType::Int,	 1 },

				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer PixelShaderFloatArrayConstants
	{
		cFloatArray cPsObjSpaceLightConeAngle[LightMax];

		//-------- 参照テーブル情報 --------------
		cFloatArray cLutScale[LutMax];

		//-----テクスチャ-------
		cFloatArray cScaleU[TextureMax];
		cFloatArray cScaleV[TextureMax];
		cFloatArray cRotate[TextureMax];
		cFloatArray cTranslateU[TextureMax];
		cFloatArray cTranslateV[TextureMax];

		//-----フラグメント-------
		cFloatArray cScale[ColorTypeMax][CombinerStageMax];

    //-----アルファテスト用参照値-----
    cFloatArray cAlphaTestRefValue[1];

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::FloatArray, LightMax },
				{ gfx::ConstantType::FloatArray, LutMax },

				{ gfx::ConstantType::FloatArray, TextureMax },
				{ gfx::ConstantType::FloatArray, TextureMax },
				{ gfx::ConstantType::FloatArray, TextureMax },
				{ gfx::ConstantType::FloatArray, TextureMax },
				{ gfx::ConstantType::FloatArray, TextureMax },

				{ gfx::ConstantType::FloatArray, ColorTypeMax * CombinerStageMax },
				
        { gfx::ConstantType::FloatArray, 1 },

				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer PixelShaderBoolConstants
	{
		//-------- 参照テーブル情報 --------------
		cBool cCookTorrance0Enable;
		cBool cCookTorrance1Enable;
		cBool cSpecularColorClamp;
		cBool cPrimaryFresnelEnable;
		cBool cSecondaryFresnelEnable;

		//-----テクスチャ-------
		cBool cZRecastEnable;

		//-----特殊フラグ-------
		cBool cUseShadowTexture;
		cBool cHighLightTextureEnable;
		cBool cFsIsPokemonShader;
		cBool cBattleChara20Enable;

		cBool cFsRenderNormalMap;
		cBool cFsDisableNormalColor;
		cBool cFsBorderLineMapSampling;

		cBool cBlurMask;
		cBool cBlurScale;
		cBool cDummy;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },

				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				{ gfx::ConstantType::Bool, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer PixelShaderBoolArrayConstants
	{
		//-------- ライト情報 --------------
		cBoolArray cPsIsSpotLight[LightMax];

		//-------- 参照テーブル情報 --------------
		cBoolArray cLookUpTableEnable[LutMax];

		//-----テクスチャ-------
		cBoolArray cTextureEnable[TextureMax];
		cBoolArray cTexture2D[TextureMax];

		//-----フラグメント-------
		cBoolArray cOutPutBuffer[ColorTypeMax][CombinerStageMax];

		math::Vector4 cTestColor3;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::BoolArray, LightMax },
				{ gfx::ConstantType::BoolArray, LutMax },
				{ gfx::ConstantType::BoolArray, TextureMax },
				{ gfx::ConstantType::BoolArray, TextureMax },
				{ gfx::ConstantType::BoolArray, ColorTypeMax * CombinerStageMax },
				{ gfx::ConstantType::Vector, 1 },
				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer PixelShaderIntConstants
	{
		//-------- ライト情報 --------------
		s32 cPsObjSpaceLightCount;

		//-----テクスチャ-------
		s32 cBumpMapNo;
		s32 cTangentNo;
		s32 cBumpMapType;

		math::Vector4 cTestColor4;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::Int, 1 },

				{ gfx::ConstantType::Int, 1 },
				{ gfx::ConstantType::Int, 1 },
				{ gfx::ConstantType::Int, 1 },

				{ gfx::ConstantType::Vector, 1 },

				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	cbuffer PixelShaderIntArrayConstants
	{
		//-------- 参照テーブル情報 --------------
		cIntArray cLutInputAngle[LutMax];

		//-----フラグメント-------
		cIntArray cMathFormula[ColorTypeMax][CombinerStageMax];
		cIntArray cInputSource[ColorTypeMax][CombinerStageMax][SourceTypeMax];
		cIntArray cSourceOperand[ColorTypeMax][CombinerStageMax][SourceTypeMax];
		cIntArray cTextureCombinerConstant[CombinerStageMax];

		math::Vector4 cTestColor5;

		static const gfx::ConstantBufferElement* GetConstantBufferElement()
		{
			static const gfx::ConstantBufferElement element[] = {
				{ gfx::ConstantType::IntArray, LutMax },

				{ gfx::ConstantType::IntArray, ColorTypeMax * CombinerStageMax },
				{ gfx::ConstantType::IntArray, ColorTypeMax * CombinerStageMax * SourceTypeMax },
				{ gfx::ConstantType::IntArray, ColorTypeMax * CombinerStageMax * SourceTypeMax },
				{ gfx::ConstantType::IntArray, CombinerStageMax },

				{ gfx::ConstantType::Vector, 1 },

				gfx::ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
			};
			return element;
		}

		void Set( const RenderState& rRenderState, const scenegraph::instance::MaterialInstanceNode *pMaterial );
	};

	const scenegraph::instance::MaterialInstanceNode		*m_pMaterial;
	const scenegraph::resource::ShaderResourceNode			*m_pVertexShader;
	const scenegraph::resource::ShaderResourceNode			*m_pGeometryShader;
	const scenegraph::resource::ShaderResourceNode			*m_pPixelShader;

	gfx::ConstantBuffer									*m_VertexShaderVectorConstantsBuffer;
	gfx::ConstantBuffer									*m_VertexShaderJointMatrixConstantsBuffer;
	gfx::ConstantBuffer									*m_VertexShaderBoolConstantsBuffer;
	gfx::ConstantBuffer									*m_VertexShaderIntArrayConstantsBuffer;

	gfx::ConstantBuffer									*m_GeometryShaderVectorConstantsBuffer;

	gfx::ConstantBuffer									*m_PixelShaderVectorConstantsBuffer;
	gfx::ConstantBuffer									*m_PixelShaderFloatArrayConstantsBuffer;
	gfx::ConstantBuffer									*m_PixelShaderBoolConstantsBuffer;
	gfx::ConstantBuffer									*m_PixelShaderBoolArrayConstantsBuffer;
	gfx::ConstantBuffer									*m_PixelShaderIntConstantsBuffer;
	gfx::ConstantBuffer									*m_PixelShaderIntArrayConstantsBuffer;

  gfx::ConstantBuffer* m_pColorShaderConstantsBuffer;
};

}}}

#endif

#endif