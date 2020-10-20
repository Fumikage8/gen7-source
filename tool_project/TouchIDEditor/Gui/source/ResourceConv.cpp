
#include "../include/ResourceConv.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>
#include <gfx/include/WinDX11/gfl2_WinDX11IndexBuffer.h>
#include <gfx/include/WinDX11/gfl2_WinDX11VertexBuffer.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Texture.h>

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph;

u8* CreateGLTexture(u32 glTexName, const resource::TextureResourceNode* pTextureResource, u32& width, u32& height)
{
  width = pTextureResource->GetTexture()->GetTextureDesc().width;
  height = pTextureResource->GetTexture()->GetTextureDesc().height;

  u8* buf = new u8[width * height * 4];

  // ここで読み込み
  void* pTex = const_cast<gfl2::gfx::Texture*>(pTextureResource->GetTexture())->Lock();
  glBindTexture(GL_TEXTURE_2D, static_cast<const gfl2::gfx::winOpenGL::WinOpenGLTexture*>(pTextureResource->GetTexture())->GetHandle());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTex);
  const_cast<gfl2::gfx::Texture*>(pTextureResource->GetTexture())->UnLock();

  memcpy(buf, pTex, width * height * 4);

  return reinterpret_cast<u8*>(buf);
}

void SetLightingTable(gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable::ReflectanceSampler* pSampler, const resource::MaterialResourceNode::LookUpTable* pLookUpTable)
{
  pSampler->m_Input = QString();
  pSampler->m_IsAbs = false;
  pSampler->m_Scale = (pLookUpTable->m_LutScale < 0.26f)
    ? "Quarter"
    : (pLookUpTable->m_LutScale < 0.6f)
    ? "Half"
    : (pLookUpTable->m_LutScale < 1.1f)
    ? "One"
    : (pLookUpTable->m_LutScale < 2.1f)
    ? "Two"
    : (pLookUpTable->m_LutScale < 4.1f)
    ? "Four"
    : "Eight";
  pSampler->m_pReferenceLookupTable = NULL;
}

void SetMaterial(gfl::cmdl::Material* pMaterial, gfl::cmdl::Model* pModel, const resource::MaterialResourceNode* pMaterialResource, const instance::MaterialInstanceNode* pMaterialInstance)
{
  // ここ以外でGLエラーが起きてる。。とりあえずここで回収
  glGetError();

  // 名前
  QString name(pMaterialResource->GetName());
  pMaterial->SetName(name);

  // シェーダー
  gfl::cmdl::Material::Shader* pShader = new gfl::cmdl::Material::Shader();

  gfl::cmdl::Material::Shader::FragmentShader* pFragmentShader = &pShader->m_FragmentShader;
  const resource::MaterialResourceNode::AttributeParam& attributeParam = pMaterialResource->GetAttributeParam();

  // bufferColor
  const gfl2::math::Vector4& bufferColor = gfl2::math::Vector4(attributeParam.m_EtcColor.m_BufferColor.r, attributeParam.m_EtcColor.m_BufferColor.g, attributeParam.m_EtcColor.m_BufferColor.b, attributeParam.m_EtcColor.m_BufferColor.a);
  pFragmentShader->m_BufferColor.Set(bufferColor.x, bufferColor.y, bufferColor.z, bufferColor.w);

  // bump
  pFragmentShader->m_FragmentBump.m_BumpTextureIndex = ""; //attributeParam.m_BumpMapNo;
  pFragmentShader->m_FragmentBump.m_BumpMode = ""; //attributeParam.m_BumpMapType;
  pFragmentShader->m_FragmentBump.m_IsBumpRenormalize = attributeParam.m_ZRecastEnable;

  // lighting
  gfl::cmdl::Material::Shader::FragmentShader::FragmentLighting* pFragmentLighting = &pFragmentShader->m_FragmentLighting;
  pFragmentLighting->m_FresnelConfig = QString();
  pFragmentLighting->m_IsClampHighLight = attributeParam.m_SpecularColorClamp;
  pFragmentLighting->m_IsDistribution0Enabled = attributeParam.m_LookUpTableEnable[resource::LookUpTableType::Distribution0];
  pFragmentLighting->m_IsDistribution1Enabled = attributeParam.m_LookUpTableEnable[resource::LookUpTableType::Distribution1];
  pFragmentLighting->m_IsGeometricFactor0Enabled = attributeParam.m_CookTorrance0Enable;
  pFragmentLighting->m_IsGeometricFactor1Enabled = attributeParam.m_CookTorrance1Enable;
  pFragmentLighting->m_IsReflectionEnabled = attributeParam.m_LookUpTableEnable[resource::LookUpTableType::ReflectionR];

  // lightingTable
  gfl::cmdl::Material::Shader::FragmentShader::FragmentLightingTable* pFragmentLightingTable = &pFragmentShader->m_FragmentLightingTable;
  SetLightingTable(&pFragmentLightingTable->m_ReflectanceRSampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::ReflectionR]);
  SetLightingTable(&pFragmentLightingTable->m_ReflectanceGSampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::ReflectionG]);
  SetLightingTable(&pFragmentLightingTable->m_ReflectanceBSampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::ReflectionB]);
  SetLightingTable(&pFragmentLightingTable->m_Distribution0Sampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::Distribution0]);
  SetLightingTable(&pFragmentLightingTable->m_Distribution1Sampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::Distribution1]);
  SetLightingTable(&pFragmentLightingTable->m_FresnelSampler, &attributeParam.m_LookUpTable[resource::LookUpTableType::Fresnel]);

  // textureCompiner
  for (u32 i = 0; i < resource::CombinerAttribute::CombinerMax; ++i)
  {
    const resource::MaterialResourceNode::TextureCombinerAttribute& combinerRgb = attributeParam.m_TextureCombinerAttribute[resource::CombinerColorType::RGB][i];
    const resource::MaterialResourceNode::TextureCombinerAttribute& combinerAlpha = attributeParam.m_TextureCombinerAttribute[resource::CombinerColorType::A][i];

    gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner* pTextureCombiner = new gfl::cmdl::Material::Shader::FragmentShader::TextureCombiner();

    static const c8* sc_MathFormulaList[] =
    {
      "Replace",
      "Modulate",
      "Add",
      "AddSigned",
      "Interpolate",
      "Subtract",
      "DotProduct3Rgb",
      "DotProduct3Rgba",
      "AddMult",
      "MultAdd",
    };
    pTextureCombiner->m_CombineRgb = sc_MathFormulaList[combinerRgb.m_MathFormula];
    pTextureCombiner->m_CombineAlpha = sc_MathFormulaList[combinerAlpha.m_MathFormula];

    static const c8* sc_ScaleList[] = {
      "",
      "One",//	等倍スケールモードです。
      "Two",//	２倍スケールモードです。
      "",
      "Four",//	４倍スケールモードです。 
    };
    pTextureCombiner->m_ScaleRgb = sc_ScaleList[(int)combinerRgb.m_Scale];
    pTextureCombiner->m_ScaleAlpha = sc_ScaleList[(int)combinerAlpha.m_Scale];

    static const c8* sc_ConstantList[] =
    {
      "Constant0",
      "Constant1",
      "Constant2",
      "Constant3",
      "Constant4",
      "Constant5",
      "Constant6",
      "Constant7",
    };
    pTextureCombiner->m_Constant = sc_ConstantList[attributeParam.m_TextureCombinerConstant[i]];

    pTextureCombiner->m_BufferInputRgb = (combinerRgb.m_OutPutBuffer) ? "PreviousBuffer" : "";
    pTextureCombiner->m_BufferInputAlpha = (combinerAlpha.m_OutPutBuffer) ? "PreviousBuffer" : "";

    static const c8* sc_SourceList[] = {
      "Texture0",//	テクスチャ０です。
      "Texture1",//	テクスチャ１です。
      "Texture2",//	テクスチャ２です。
      "Texture3",//	テクスチャ３です。 プロシージャルテクスチャになります。
      "Constant",//	コンスタントカラーです。
      "PrimaryColor",//	テクスチャ処理に先立ちテクスチャコンバイナ０に入ってくるフラグメントカラーです。
      "FragmentPrimaryColor",//	フラグメントライトのプライマリーカラーです。 ディフューズ、アンビエント、エミッシブを表します。
      "FragmentSecondaryColor",//	フラグメントライトのセカンダリーカラーです。 スペキュラーを表します。 
      "Previous",//	前のテクスチャコンバイナから入ってくるフラグメントカラーです。
      "PreviousBuffer",//	前のテクスチャコンバイナのバッファから入ってくるカラーです。
    };
    pTextureCombiner->m_SourceRgb.m_Source0 = sc_SourceList[combinerRgb.m_InputSource[0]];
    pTextureCombiner->m_SourceRgb.m_Source1 = sc_SourceList[combinerRgb.m_InputSource[1]];
    pTextureCombiner->m_SourceRgb.m_Source2 = sc_SourceList[combinerRgb.m_InputSource[2]];

    pTextureCombiner->m_SourceAlpha.m_Source0 = sc_SourceList[combinerAlpha.m_InputSource[0]];
    pTextureCombiner->m_SourceAlpha.m_Source1 = sc_SourceList[combinerAlpha.m_InputSource[1]];
    pTextureCombiner->m_SourceAlpha.m_Source2 = sc_SourceList[combinerAlpha.m_InputSource[2]];

    static const c8* sc_OperandList[] = {
      "Color",//	ソースカラーです。
      "OneMinusColor",//	1-Colorです。
      "Alpha",//	ソースのアルファ値です。
      "OneMinusAlpha",//	1-Alphaです。
      "Red",//	ソースのRed値です。

      "OneMinusRed",//	1-Redです。
      "Green",//	ソースのGreen値です。
      "OneMinusGreen",//	1-Greenです。
      "Blue",//	ソースのBlue値です。
      "OneMinusBlue"//	1-Blueです。 
    };
    pTextureCombiner->m_OperandRgb.m_Operand0 = sc_OperandList[combinerRgb.m_Operand[0]];
    pTextureCombiner->m_OperandRgb.m_Operand1 = sc_OperandList[combinerRgb.m_Operand[1]];
    pTextureCombiner->m_OperandRgb.m_Operand2 = sc_OperandList[combinerRgb.m_Operand[2]];

    pTextureCombiner->m_OperandAlpha.m_Operand0 = sc_OperandList[combinerAlpha.m_Operand[0]];
    pTextureCombiner->m_OperandAlpha.m_Operand1 = sc_OperandList[combinerAlpha.m_Operand[1]];
    pTextureCombiner->m_OperandAlpha.m_Operand2 = sc_OperandList[combinerAlpha.m_Operand[2]];

    pFragmentShader->m_TextureConbiner.append(pTextureCombiner);
  }

  const resource::MaterialResourceNode::BlendTest& blendTest = pMaterialResource->GetBlendTest();

  static const c8* sc_CompfuncList[] =
  {
    "Less", // Less,					//!< リファレンス値より小さい場合にのみパスさせる
    "Lequal", // LessEqual,		//!< リファレンス値と同じか小さい場合にパスさせる
    "Equal", // Equal,				//!< リファレンス値と同じ場合にはパスさせる
    "Gequal", // GreaterEqual,	//!< リファレンス値と同じか大きい場合にパスさせる
    "Greater", // Greater,			//!< リファレンス値よりも大きい場合にパスさせる
    "Notequal", // NotEqual,			//!< リファレンス値と同じではない場合にパスさせる
    "Always", // Always,				//!< 常にパスさせる
    "Never", // Never,				//!< パスさせない
  };

  // fragmentOperation
  gfl::cmdl::Material::Shader::FragmentOperation* pFragmentOperation = &pShader->m_FragmentOperation;

  // depthOperation
  gfl::cmdl::Material::Shader::FragmentOperation::DepthOperation* pDepthOperation = &pFragmentOperation->m_DepthOperation;
  pDepthOperation->m_IsTestEnabled = true;
  pDepthOperation->m_TestFunction = sc_CompfuncList[blendTest.m_DepthTest.m_DepthTestType];
  pDepthOperation->m_IsMaskEnabled = blendTest.m_DepthTest.m_UpDateDepth;

  // blendOperation
  gfl::cmdl::Material::Shader::FragmentOperation::BlendOperation* pBlendOperation = &pFragmentOperation->m_BlendOperation;

  static const c8* sc_BlendModeList[] =
  {
    "Blend", // Blend,
    "SeparateBlend", // Separate,
    "Logic", // LogicOp,
    "NotUsed", // None,
  };

  static const c8* sc_LogicalOperationList[] =
  {
    "Clear", // CLEAR,//すべてのビット値を 0
    "Set", // SET,//すべてのビット値を 1	
    "Copy", // COPY,//ソースカラーのビット値	
    "NoOp", // NOOP,//デスティネーションカラーのビット値	
    "CopyInverted", // COPY_INVERTED,//ソースカラーの反転ビット値	
    "Invert", // INVERT,//デスティネーションカラーの反転ビット値	
    "And", // AND,//ソースとデスティネーションカラーの論理積	
    "Or", // OR,//ソースとデスティネーションカラーの論理和	
    "Xor", // XOR,//ソースとデスティネーションカラーの排他的論理和	
    "AndReverse", // AND_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理積	
    "OrReverse", // OR_REVERSE,//ソースとデスティネーションカラーの反転ビット値の論理和	
    "AndInverted", // AND_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理積	
    "OrInverted", // OR_INVERTED,//ソースカラーの反転ビット値と、デスティネーションカラーの論理和	
    "Nand", // NAND,//ソースとデスティネーションカラーの論理積の反転ビット値	
    "Nor", // NOR,//ソースとデスティネーションカラーの論理和の反転ビット値						
    "Equiv", // EQUIV,//ソースとデスティネーションカラーの排他的論理和の反転ビット値	
  };

  static const c8* sc_BlendEquationList[] =
  {
    "FuncAdd", // Add = 0,					//!< ( R, G, B, A ) = ( Rs * Rx + Rd * Ry, Gs * Gx + Gd * Gy, Bs * Bx + Bd * By, As * Ax + Ad * Ay )
    "FuncSubtract", // Subtract,					//!< ( R, G, B, A ) = ( Rs * Rx - Rd * Ry, Gs * Gx - Gd * Gy, Bs * Bx - Bd * By, As * Ax - Ad * Ay )
    "FuncReverseSubtract", // ReverseSubtract,	//!< ( R, G, B, A ) = ( Rd * Ry - Rs * Rx, Gd * Gy - Gs * Gx, Bd * By - Bs * Bx, Ad * Ay - As * Ax )
    "Min", // Min,							//!< ( R, G, B, A ) = ( min( Rs * Rd ), min( Gs * Gd ), min( Bs * Bd ), min( As * Ad ) )
    "Max", // Max,							//!< ( R, G, B, A ) = ( MAX( Rs * Rd ), MAX( Gs * Gd ), MAX( Bs * Bd ), MAX( As * Ad ) )
  };

  static const c8* sc_BlendElementList[] =
  {
    "Zero",
    "One",
    "SourceColor",
    "OneMinusSourceColor",
    "DestinationColor",
    "OneMinusDestinationColor",
    "SourceAlpha",
    "OneMinusSourceAlpha",
    "DestinationAlpha",
    "OneMinusDestinationAlpha",
    "ConstantColor",
    "OneMinusConstantColor",
    "ConstantAlpha",
    "OneMinusConstantAlpha",
    "SourceAlphaSaturate",
  };

  static const c8* sc_StencilOpList[] =
  {
    "Keep",		  //!< 更新しない
    "Zero",					//!< ０に設定
    "Replace",			//!< 参照値で置き換える
    "Increment",		//!< 最大値に達するまで増加
    "IncrementWrap",//!< 増加させその値が最大値を超えると０に戻します。
    "Decrement",		//!< ０になるまで減少
    "DecrementWrap",//!< 減少させその値が０より小さくなった場合は最大値に戻します。
    "Invert",				//!< ビットを反転
  };

  pBlendOperation->m_Mode = sc_BlendModeList[blendTest.m_Blender.m_BlendMode];
  pBlendOperation->m_LogicOperation = sc_LogicalOperationList[blendTest.m_Blender.m_LogicalOperation];

  pBlendOperation->m_RgbParameter.m_BlendEquation = sc_BlendEquationList[blendTest.m_Blender.m_Color.m_BlendFunc];
  pBlendOperation->m_RgbParameter.m_BlendFunctionSource = sc_BlendElementList[blendTest.m_Blender.m_Color.m_SrcElement];
  pBlendOperation->m_RgbParameter.m_BlendFunctionDestination = sc_BlendElementList[blendTest.m_Blender.m_Color.m_DstElement];

  pBlendOperation->m_AlphaParameter.m_BlendEquation = sc_BlendEquationList[blendTest.m_Blender.m_Alpha.m_BlendFunc];
  pBlendOperation->m_AlphaParameter.m_BlendFunctionSource = sc_BlendElementList[blendTest.m_Blender.m_Alpha.m_SrcElement];
  pBlendOperation->m_AlphaParameter.m_BlendFunctionDestination = sc_BlendElementList[blendTest.m_Blender.m_Alpha.m_DstElement];

  // stencilOperation
  gfl::cmdl::Material::Shader::FragmentOperation::StencilOperation* pStencilOperation = &pFragmentOperation->m_StencilOperation;
  pStencilOperation->m_IsTestEnabled = blendTest.m_StencilTest.m_StencilTestEnable;
  pStencilOperation->m_TestFunction = sc_CompfuncList[blendTest.m_StencilTest.m_StencilTestType];
  pStencilOperation->m_TestReference = blendTest.m_StencilTest.m_StencilValue;
  pStencilOperation->m_TestMask = blendTest.m_StencilTest.m_StencilMask;
  pStencilOperation->m_FailOperation = sc_StencilOpList[blendTest.m_StencilTest.m_StencilFailOp];
  pStencilOperation->m_ZFailOperation = sc_StencilOpList[blendTest.m_StencilTest.m_StencilZFailOp];
  pStencilOperation->m_PassOperation = sc_StencilOpList[blendTest.m_StencilTest.m_StencilZpassOp];

  pMaterial->SetShader(pShader);

  static const c8* sc_LayerList[] =
  {
    "Layer0",
    "Layer1",
    "Layer2",
    "Layer3",
    "Layer4",
    "Layer5",
  };

  pMaterial->SetName(pMaterialResource->GetName());
  pMaterial->m_IsCompressible = false;
  pMaterial->m_LightSetIndex = attributeParam.m_LightSetNo;
  pMaterial->m_FogIndex = attributeParam.m_FogNo;
  pMaterial->m_IsFragmentLightEnabled = attributeParam.m_PsLightingEnable;
  pMaterial->m_IsVertexLightEnabled = attributeParam.m_VsLightingEnable;
  pMaterial->m_IsHemiSphereLightEnabled = false;
  pMaterial->m_IsFogEnabled = attributeParam.m_FogEnable;
  pMaterial->m_TextureCoordinateConfig = "Config0120";
  pMaterial->m_TranslucencyKind = sc_LayerList[blendTest.m_DrawLayer];
  pMaterial->m_ShaderProgramDescriptionIndex = 0;
  pMaterial->m_ShaderBinaryKind = "Default";

  // rastrization
  static const c8* sc_CullModeList[] =
  {
    "Never", // None = 0,		   //!< 背面カリングしない
    "BackFace", // CCW,				//!< 背面を反時計回りでカリング
    "FrontFace", // CW,				 //!< 背面を時計回りでカリング
  };
  pMaterial->m_Rasterization.m_CullingMode = sc_CullModeList[attributeParam.m_CullMode];
  pMaterial->m_Rasterization.m_IsPolygonOffsetEnabled = true;
  pMaterial->m_Rasterization.m_PolygonOffsetUnit = blendTest.m_DepthTest.m_PolygonOffset;

  // materialColor
  gfl::cmdl::Material::MaterialColor* pMaterialColor = new gfl::cmdl::Material::MaterialColor();

  pMaterialColor->m_VertexColorScale = 1.0f;
  pMaterialColor->SetEmission(gfl::core::Vector4(
    attributeParam.m_MaterialColor.m_Emission.r,
    attributeParam.m_MaterialColor.m_Emission.g,
    attributeParam.m_MaterialColor.m_Emission.b,
    attributeParam.m_MaterialColor.m_Emission.a));
  pMaterialColor->SetAmbient(gfl::core::Vector4(
    attributeParam.m_MaterialColor.m_Ambient.r,
    attributeParam.m_MaterialColor.m_Ambient.g,
    attributeParam.m_MaterialColor.m_Ambient.b,
    attributeParam.m_MaterialColor.m_Ambient.a));
  pMaterialColor->SetDiffuse(gfl::core::Vector4(
    attributeParam.m_MaterialColor.m_Diffuse.r,
    attributeParam.m_MaterialColor.m_Diffuse.g,
    attributeParam.m_MaterialColor.m_Diffuse.b,
    attributeParam.m_MaterialColor.m_Diffuse.a));
  pMaterialColor->SetSpecular(0, gfl::core::Vector4(
    attributeParam.m_MaterialColor.m_Specular0.r,
    attributeParam.m_MaterialColor.m_Specular0.g,
    attributeParam.m_MaterialColor.m_Specular0.b,
    attributeParam.m_MaterialColor.m_Specular0.a));
  pMaterialColor->SetSpecular(1, gfl::core::Vector4(
    attributeParam.m_MaterialColor.m_Specular1.r,
    attributeParam.m_MaterialColor.m_Specular1.g,
    attributeParam.m_MaterialColor.m_Specular1.b,
    attributeParam.m_MaterialColor.m_Specular1.a));
  for (u32 i = 0; i < gfl::cmdl::Material::MaterialColor::CONSTANT_MAX; ++i)
  {
    pMaterialColor->SetConstant(i, gfl::core::Vector4(
      attributeParam.m_ConstantColor[i].r,
      attributeParam.m_ConstantColor[i].g,
      attributeParam.m_ConstantColor[i].b,
      attributeParam.m_ConstantColor[i].a));
  }
  pMaterial->SetMaterialColor(pMaterialColor);

  // textureCoordinator
  u32 textureCount = 0;
  for (u32 i = 0; i < pMaterialResource->GetTextureNum(); ++i)
  {
    if (pMaterialResource->GetTextureInfo(i).m_SlotNo < 8)
    {
      ++textureCount;
    }
  }

  gfl::cmdl::Material::TextureCoordinator* pTextureCoordinatorList = new gfl::cmdl::Material::TextureCoordinator[textureCount];
  for (u32 i = 0; i < textureCount; ++i)
  {
    const resource::MaterialResourceNode::TextureInfo& textureInfo = pMaterialResource->GetTextureInfo(i);
    gfl::cmdl::Material::TextureCoordinator* pTextureCoordinator = &pTextureCoordinatorList[i];

    pTextureCoordinator->SetSourceCoordinate(textureInfo.m_Attribute.m_UvSetNo);
    pTextureCoordinator->SetMappingMethod("UvCoordinateMap");
    pTextureCoordinator->SetReferenceCamera(-1);
    pTextureCoordinator->SetScaleS(textureInfo.m_Attribute.m_ScaleU);
    pTextureCoordinator->SetScaleT(textureInfo.m_Attribute.m_ScaleV);
    pTextureCoordinator->SetRotate(textureInfo.m_Attribute.m_Rotate);
    pTextureCoordinator->SetTranslateS(textureInfo.m_Attribute.m_TranslateU);
    pTextureCoordinator->SetTranslateT(textureInfo.m_Attribute.m_TranslateV);
  }

  pMaterial->SetTextureCoordinator(pTextureCoordinatorList);
  pMaterial->SetTextureCoordinatorCount(textureCount);

  // textureMapper
  gfl::cmdl::Material::TextureMapper* pTextureMapperList = new gfl::cmdl::Material::TextureMapper[textureCount];
  for (u32 i = 0; i < textureCount; ++i)
  {
    const resource::MaterialResourceNode::TextureInfo& textureInfo = pMaterialResource->GetTextureInfo(i);
    gfl::cmdl::Material::TextureMapper* pTextureMapper = &pTextureMapperList[i];

    pTextureMapper->SetName(textureInfo.m_TextureName);
    pTextureMapper->SetFileName(textureInfo.m_TextureName);

    // OpenGLのテクスチャ作成
    *pTextureMapper->GetTextureId() = 1234;
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    glGenTextures( 1, pTextureMapper->GetTextureId() );
    glBindTexture( GL_TEXTURE_2D, *pTextureMapper->GetTextureId() );

    u32 type = (textureInfo.m_Attribute.m_MagFilter == gfl2::gfx::TextureFilter::Point) ? GL_NEAREST : GL_LINEAR;
    pTextureMapper->SetMagFilter(type);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);

    type = (textureInfo.m_Attribute.m_MinFilter == gfl2::gfx::TextureFilter::Point) ? GL_NEAREST : GL_LINEAR ;
    pTextureMapper->SetMinFilter(type);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);

    type = (textureInfo.m_Attribute.m_RepeatU == gfl2::gfx::TextureAddress::Wrap)
      ? GL_REPEAT
      : (textureInfo.m_Attribute.m_RepeatU == gfl2::gfx::TextureAddress::Mirror)
      ? GL_MIRRORED_REPEAT
      : GL_CLAMP;
    pTextureMapper->SetWrapS(type);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, type );

    type = (textureInfo.m_Attribute.m_RepeatV == gfl2::gfx::TextureAddress::Wrap)
      ? GL_REPEAT
      : (textureInfo.m_Attribute.m_RepeatV == gfl2::gfx::TextureAddress::Mirror)
      ? GL_MIRRORED_REPEAT
      : GL_CLAMP;
    pTextureMapper->SetWrapT(type);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, type );

    pTextureMapper->m_MinLod = 0;
    pTextureMapper->m_LodBias = 0;

    const resource::TextureResourceNode* pTextureResource = NULL;
    for (u32 j = 0; j < pMaterialInstance->GetTextureNum(); ++j)
    {
      pTextureResource = pMaterialInstance->GetTextureResourceNode(j);

      if (pTextureResource->GetNameHash() == textureInfo.m_TextureNameHash)
      {
        if (strcmp(pTextureResource->GetName(), textureInfo.m_TextureName) == 0)
        {
          break;
        }
      }
    }

    u32 width = 0;
    u32 height = 0;
    u8* data = CreateGLTexture(*pTextureMapper->GetTextureId(), pTextureResource, width, height);

    QString texfname(pTextureResource->GetName());
    QList<gfl::cmdl::Model::TextureTabInfo>* tt =  pModel->GetTextureTabInfo();
    tt->append(gfl::cmdl::Model::TextureTabInfo(texfname, pMaterial, data, width, height));

    gfl::cmdl::Image* c_img = new gfl::cmdl::Image();

    c_img->SetHeader(width, height);
    c_img->SetImage(reinterpret_cast<c8*>(data));
    c_img->SetRGB_Type(GL_RGBA);
    c_img->SetOrderType(GL_UNSIGNED_BYTE);

    c_img->SetTextureMapper(pTextureMapper);
    pTextureMapper->GetImageList()->append(c_img);
    pModel->GetImageData()->append(c_img);
  }

  pMaterial->SetTextureMapper(pTextureMapperList);
  pMaterial->SetTextureMapperCount(textureCount);
  pMaterial->glCreateShader();
}

void SetPrimitive(gfl::cmdl::Shape::Primitive* pPrimitive, const resource::MeshResourceNode::FaceData* pFaceData, s32 jointCount)
{
  // ここで読み込み
  u32 unitSize = 0;
  switch (pFaceData->GetIndexBuffer()->GetIndexType())
  {
  case gfl2::gfx::IndexType::Index8:
    unitSize = 1;
    break;

  case gfl2::gfx::IndexType::Index16:
    unitSize = 2;
    break;

  case gfl2::gfx::IndexType::Index32:
    unitSize = 4;
    break;

  default:
    assert(0);
    break;
  }

  u32 size = pFaceData->GetIndexBuffer()->GetIndexNum() * unitSize;
  u8* pIndexList = new u8[size];
  void* pIndexBuf = const_cast<gfl2::gfx::IndexBuffer*>(pFaceData->GetIndexBuffer())->Lock();
  memcpy(pIndexList, pIndexBuf, size); 
  const_cast<gfl2::gfx::IndexBuffer*>(pFaceData->GetIndexBuffer())->UnLock();

  // トライアングルズ
  pPrimitive->SetTriangles(reinterpret_cast<s32*>(pIndexList));
  pPrimitive->SetTriangleCount(pFaceData->GetIndexBuffer()->GetIndexNum() / 3);

  // ボーンインデックスリスト
  s32* pBoneIndexList = new s32[jointCount];
  for (s32 i = 0; i < jointCount; ++i)
  {
    pBoneIndexList[i] = i;
  }
  pPrimitive->SetBoneIndexTable(pBoneIndexList);
  pPrimitive->SetBoneIndexTableCount(jointCount);
}

void SetShape(gfl::cmdl::Shape* pShape, const resource::MeshResourceNode::FaceData* pFaceData, const resource::MeshResourceNode* pMeshResourceNode)
{
  struct GfMdlVertex
  {
    gfl2::math::Vector position;
    gfl2::math::Vector normal;
    gfl2::math::Vector tangent;
    gfl2::math::Vector texcoord0;
    gfl2::math::Vector texcoord1;
    gfl2::math::Vector texcoord2;
    gfl2::math::Vector color;
    gfl2::math::Vector blendIndices;
    gfl2::math::Vector blendWeight;
  };

  //// ここで読み込み
  std::vector<GfMdlVertex> gfMdlVertexList(pFaceData->m_VertexNum);
  void* pVertexBuf = const_cast<gfl2::gfx::VertexBuffer*>(pFaceData->GetVertexBuffer())->Lock();
  memcpy(gfMdlVertexList.data(), pVertexBuf, sizeof(GfMdlVertex)*pFaceData->m_VertexNum); 
  const_cast<gfl2::gfx::VertexBuffer*>(pFaceData->GetVertexBuffer())->UnLock();

  // 頂点を展開
  gfl::core::Vector* pCmdlVertexList = new gfl::core::Vector[gfMdlVertexList.size()];
  gfl::core::Vector* pCmdlNormalList = new gfl::core::Vector[gfMdlVertexList.size()];
  gfl::core::Vector* pCmdlTangentList = new gfl::core::Vector[gfMdlVertexList.size()];
  gfl::core::Vector2* pCmdlTexcoord0List = new gfl::core::Vector2[gfMdlVertexList.size()];
  gfl::core::Vector2* pCmdlTexcoord1List = new gfl::core::Vector2[gfMdlVertexList.size()];
  gfl::core::Vector2* pCmdlTexcoord2List = new gfl::core::Vector2[gfMdlVertexList.size()];
  u8* pCmdlColorList = new u8[gfMdlVertexList.size() * 4];
  s32* pCmdlBlendIndicesList = new s32[gfMdlVertexList.size() * 4];
  f32* pCmdlBlendWeightList = new f32[gfMdlVertexList.size() * 4];
  for (u32 i = 0; i < gfMdlVertexList.size(); ++i)
  {
    GfMdlVertex& vertex = gfMdlVertexList[i];

    pCmdlVertexList[i].Set(vertex.position.x, vertex.position.y, vertex.position.z);
    pCmdlNormalList[i].Set(vertex.normal.x, vertex.normal.y, vertex.normal.z);
    pCmdlTangentList[i].Set(vertex.tangent.x, vertex.tangent.y, vertex.tangent.z);
    pCmdlTexcoord0List[i].Set(vertex.texcoord0.x, vertex.texcoord0.y);
    pCmdlTexcoord1List[i].Set(vertex.texcoord1.x, vertex.texcoord1.y);
    pCmdlTexcoord2List[i].Set(vertex.texcoord2.x, vertex.texcoord2.y);

    u32 index2 = i * 4;
    pCmdlColorList[index2 + 0] = vertex.color.x * 255.0f;
    pCmdlColorList[index2 + 1] = vertex.color.y * 255.0f;
    pCmdlColorList[index2 + 2] = vertex.color.z * 255.0f;
    pCmdlColorList[index2 + 3] = vertex.color.w * 255.0f;

    pCmdlBlendIndicesList[index2 + 0] = vertex.blendIndices.x;
    pCmdlBlendIndicesList[index2 + 1] = vertex.blendIndices.y;
    pCmdlBlendIndicesList[index2 + 2] = vertex.blendIndices.z;
    pCmdlBlendIndicesList[index2 + 3] = vertex.blendIndices.w;

    pCmdlBlendWeightList[index2 + 0] = vertex.blendWeight.x;
    pCmdlBlendWeightList[index2 + 1] = vertex.blendWeight.y;
    pCmdlBlendWeightList[index2 + 2] = vertex.blendWeight.z;
    pCmdlBlendWeightList[index2 + 3] = vertex.blendWeight.w;
  }

  pShape->SetVertices(pCmdlVertexList);
  pShape->SetNormals(pCmdlNormalList);
  pShape->SetTangents(pCmdlTangentList);
  pShape->SetTextures(0, pCmdlTexcoord0List);
  pShape->SetTextures(1, pCmdlTexcoord1List);
  pShape->SetTextures(2, pCmdlTexcoord2List);
  pShape->SetColor(pCmdlColorList);
  pShape->SetColorScale(1.0f);
  pShape->SetColorStep(4);
  pShape->SetBoneIdIndex(pCmdlBlendIndicesList);
  pShape->SetBoneIndexCount(4);
  pShape->SetWeights(pCmdlBlendWeightList);
  pShape->SetIndexStep(4);
}

void ResourceConv::ToModel(gfl::cmdl::Model* pModel, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance)
{
  // フェイス数加算
  u32 faceCount = 0;
  for (u32 i = 0; i < pModelInstance->GetMehsNum(); ++i)
  {
    instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = pModelInstance->GetMeshLinkData(i);
    faceCount += pMeshLinkData->GetFaceNum();
  }

  // フェイスの数だけメッシュとシェイプを確保
  gfl::cmdl::Mesh* pMeshList = new gfl::cmdl::Mesh[faceCount];
  gfl::cmdl::Shape* pShapeList = new gfl::cmdl::Shape[faceCount];

  // マテリアル確保
  gfl::cmdl::Material* pMaterialList = new gfl::cmdl::Material[pModelInstance->GetMaterialNum()];
  for (u32 i = 0; i < pModelInstance->GetMaterialNum(); ++i)
  {
    const instance::MaterialInstanceNode* pMaterialInstance = pModelInstance->GetMaterialInstanceNode(i);
    SetMaterial(&pMaterialList[i], pModel, pMaterialInstance->GetMaterialResourceNode(), pMaterialInstance);
  }

  // gdmdlのメッシュは複数のフェイス相当
  u32 faceIndex = 0;
  for (u32 i = 0; i < pModelInstance->GetMehsNum(); ++i)
  {
    instance::ModelInstanceNode::MeshLinkData* pMeshLinkData = pModelInstance->GetMeshLinkData(i);

    // ShapeとPrimitive相当
    for (u32 j = 0; j < pMeshLinkData->GetFaceNum(); ++j)
    {
      const resource::MeshResourceNode::FaceData* pFaceData = pMeshLinkData->GetFaceData(j);

      // マテリアル
      QString materialName(pFaceData->m_MaterialName);
      gfl::cmdl::Material* pMaterial = NULL;
      for (u32 materialIndex = 0; materialIndex < pModelInstance->GetMaterialNum(); ++materialIndex)
      {
        pMaterial = &pMaterialList[materialIndex];
        if (pMaterial->GetName() == materialName)
        {
          break;
        }
      }

      // バーテクスバッファはVertexAttributes相当。インターリーブされている
      const gfl2::gfx::VertexBuffer* pVertexBuffer = pFaceData->GetVertexBuffer();
      gfl::cmdl::Shape::Primitive* pPrimitive = new gfl::cmdl::Shape::Primitive[1];

      // プリミティブ設定
      SetPrimitive(pPrimitive, pFaceData, pModelInstance->GetJointNum());

      // シェイプ設定
      gfl::cmdl::Shape* pShape = &pShapeList[faceIndex];
      pShape->SetPrimitives(pPrimitive);
      pShape->SetPrimitiveCount(1);

      s32* pTriangleCountList = new s32[1];
      *pTriangleCountList = pPrimitive->GetTriangleCount();
      pShape->SetTriangleCount(pTriangleCountList);

      // バウンディングボックス
      const gfl2::math::AABB& aabb = pModelInstance->GetAABB();
      const gfl2::math::Vector4& center = aabb.GetCenter();
      const gfl2::math::Vector4& maxval = aabb.GetMax();
      pShape->m_BoundingBox.m_Matrix.Set(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        center.x, center.y, center.z, 1.0f);

      pShape->m_BoundingBox.m_Size.x = 2 * (maxval.x - center.x);
      pShape->m_BoundingBox.m_Size.y = 2 * (maxval.y - center.y);
      pShape->m_BoundingBox.m_Size.z = 2 * (maxval.z - center.z);

      SetShape(pShape, pFaceData, pMeshLinkData->GetMeshResourceNode());

      // メッシュ設定
      gfl::cmdl::Mesh* pMesh = &pMeshList[faceIndex];
      pMesh->SetShape(pShape);
      pMesh->SetMaterial(pMaterial);

      ++faceIndex;
    }
  }

  // マテリアル
  pModel->SetMaterial(pMaterialList);
  pModel->SetMaterialCount(pModelInstance->GetMaterialNum());

  // メッシュ
  pModel->SetMesh(pMeshList);
  pModel->SetMeshCount(faceCount);

  // シェイプ
  pModel->SetShape(pShapeList);
  pModel->SetShapeCount(faceCount);

  // ボーン
  s32 boneCount = pModelInstance->GetJointNum();
  gfl::cmdl::Bone* pBoneList = new gfl::cmdl::Bone[boneCount];

  for (s32 i = 0; i < boneCount; ++i)
  {
    instance::JointInstanceNode* pJoint = pModelInstance->GetJointInstanceNode(i);
    gfl::cmdl::Bone* pBone = &pBoneList[i];

    pBone->SetName(pJoint->GetNodeName());
    pBone->SetParentName(pJoint->GetParentName());

    const gfl2::math::Matrix34& matrix = pJoint->GetLocalSRT().ToMatrix();
    gfl2::math::Vector3 pos = matrix.GetElemPosition();
    gfl2::math::Vector3 rot = matrix.GetElemRotationZYX();
    gfl2::math::Vector3 scale = matrix.GetElemScaling();

    pBone->SetTranslate(gfl::core::Vector(pos.x, pos.y, pos.z));
    pBone->SetRotate(gfl::core::Vector(rot.x, rot.y, rot.z));
    pBone->SetScale(gfl::core::Vector(scale.x, scale.y, scale.z));
  }

  pModel->SetBone(pBoneList);
  pModel->SetBoneCount(boneCount);
}
