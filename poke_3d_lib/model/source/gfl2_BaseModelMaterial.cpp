//=============================================================================
/**
 * @file    gfl2_BaseModelMaterial.cpp
 * @brief   
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.4    ishiguro  「returning reference to local temporary」警告対処
 *  2016.4.8    ishiguro  (MATERIAL_SAMPLER_STATE_OBJECT_ENABLEにて)未使用機能の封印
 *
 */
//=============================================================================
#include <model/include/gfl2_BaseModelMaterial.h>

using namespace gfl2::renderingengine::scenegraph;

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
BaseModelMaterial::BaseModelMaterial(gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode) :
  m_pMaterialInstanceNode(pMaterialInstanceNode)
{
}

/**
 * @brief デストラクタ
 */
BaseModelMaterial::~BaseModelMaterial()
{
}

//---------------------------------------------------------------------------------
// ■ マテリアルカラー

/**
 * @brief エミッションカラー設定
 *
 * @param color カラー
 */
void BaseModelMaterial::SetEmissionColor(const gfl2::math::Vector4& color)
{
  this->GetMaterialColor_()->m_Emission = color;
}

/**
 * @brief エミッションカラー設定：要素毎
 *
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetEmissionColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetEmissionColor(gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief エミッションカラー取得
 *
 * @return エミッションカラーを返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetEmissionColor() const
{
  return this->GetMaterialColor_()->m_Emission.GetColor();
}

/**
 * @brief アンビエントカラー設定
 *
 * @param color カラー
 */
void BaseModelMaterial::SetAmbientColor(const gfl2::math::Vector4& color)
{
  this->GetMaterialColor_()->m_Ambient = color;
}

/**
 * @brief アンビエントカラー設定：要素毎
 *
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetAmbientColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetAmbientColor(gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief アンビエントカラー取得
 *
 * @return アンビエントカラーを返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetAmbientColor() const
{
  return this->GetMaterialColor_()->m_Ambient.GetColor();
}

/**
 * @brief ディフューズカラー設定
 *
 * @param color カラー
 */
void BaseModelMaterial::SetDiffuseColor(const gfl2::math::Vector4& color)
{
  this->GetMaterialColor_()->m_Diffuse = color;
}

/**
 * @brief ディフューズカラー設定：要素毎
 *
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetDiffuseColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetDiffuseColor(gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief ディフューズカラー取得
 *
 * @return ディフューズカラーを返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetDiffuseColor() const
{
  return this->GetMaterialColor_()->m_Diffuse.GetColor();
}

/**
 * @brief スペキュラカラー０設定
 *
 * @param color カラー
 */
void BaseModelMaterial::SetSpecularColor0(const gfl2::math::Vector4& color)
{
  this->GetMaterialColor_()->m_Specular0 = color;
}

/**
 * @brief スペキュラカラー０設定：要素毎
 *
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetSpecularColor0(f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetSpecularColor0(gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief スペキュラカラー０取得
 *
 * @return スペキュラカラー０を返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetSpecularColor0() const
{
  return this->GetMaterialColor_()->m_Specular0.GetColor();
}

/**
 * @brief スペキュラカラー１設定
 *
 * @param color カラー
 */
void BaseModelMaterial::SetSpecularColor1(const gfl2::math::Vector4& color)
{
  this->GetMaterialColor_()->m_Specular1 = color;
}

/**
 * @brief スペキュラカラー１設定：要素毎
 *
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetSpecularColor1(f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetSpecularColor1(gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief スペキュラカラー１取得
 *
 * @return スペキュラカラー１を返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetSpecularColor1() const
{
  return this->GetMaterialColor_()->m_Specular1.GetColor();
}

//---------------------------------------------------------------------------------
// ■ コンスタントカラー

/**
 * @brief コンスタントカラー設定
 *
 * @param constantColorIndex コンスタントカラー番号
 * @param color カラー
 */
void BaseModelMaterial::SetConstantColor(s32 constantColorIndex, const gfl2::math::Vector4& color)
{
  this->GetMaterialAttributeParam_()->m_ConstantColor[constantColorIndex] = color;
}

/**
 * @brief コンスタントカラー設定：要素毎
 *
 * @param constantColorIndex コンスタントカラー番号
 * @param colorR カラーＲ
 * @param colorG カラーＧ
 * @param colorB カラーＢ
 * @param colorA カラーＡ
 */
void BaseModelMaterial::SetConstantColor(s32 constantColorIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA)
{
  this->SetConstantColor(constantColorIndex, gfl2::math::Vector4(colorR, colorG, colorB, colorA));
}

/**
 * @brief コンスタントカラー取得
 *
 * @param constantColorIndex コンスタントカラー番号
 *
 * @return コンスタントカラーを返す
 */
gfl2::math::Vector4 BaseModelMaterial::GetConstantColor(s32 constantColorIndex) const
{
  return this->GetMaterialAttributeParam_()->m_ConstantColor[constantColorIndex].GetColor();
}

//---------------------------------------------------------------------------------
// ■ フォグ

/**
 * @brief フォグの有効設定
 *
 * @param enable trueで有効、falseで無効
 */
void BaseModelMaterial::SetFogEnable(b32 enable)
{
  this->GetMaterialAttributeParam_()->m_FogEnable = enable;
}

/**
 * @brief フォグが有効か？
 *
 * @return enable trueで有効、falseで無効
 */
b32 BaseModelMaterial::IsFogEnable() const
{
  return this->GetMaterialAttributeParam_()->m_FogEnable;
}

/**
 * @brief フォグの有効設定
 *
 * @param enable trueで有効、falseで無効
 */
void BaseModelMaterial::SetFogIndex(s16 fogIndex)
{
  this->GetMaterialAttributeParam_()->m_FogNo = fogIndex;
}

/**
 * @brief フォグの有効設定
 *
 * @param enable trueで有効、falseで無効
 */
s16 BaseModelMaterial::GetFogIndex() const
{
  return this->GetMaterialAttributeParam_()->m_FogNo;
}

//---------------------------------------------------------------------------------
// ■ ライトセット

/**
 * @brief フォグの有効設定
 *
 * @param lightSetIndex trueで有効、falseで無効
 */
void BaseModelMaterial::SetLightSetIndex(s16 lightSetIndex)
{
  this->GetMaterialAttributeParam_()->m_LightSetNo = lightSetIndex;
}

/**
 * @brief フォグが有効か？
 *
 * @return trueで有効、falseで無効
 */
s16 BaseModelMaterial::GetLightSetIndex() const
{
  return this->GetMaterialAttributeParam_()->m_LightSetNo;
}

//---------------------------------------------------------------------------------
// ■ カリングモード

/**
 * @brief カリングモード設定
 *
 * @param cullingMode カリングモード
 */
void BaseModelMaterial::SetCullingMode(gfl2::gfx::CullMode cullingMode)
{
  this->GetMaterialAttributeParam_()->m_CullMode.Set(cullingMode);
}

/**
 * @brief カリングモード取得
 *
 * @return カリングモードを返す
 */
gfl2::gfx::CullMode BaseModelMaterial::GetCullingMode() const
{
  return this->GetMaterialAttributeParam_()->m_CullMode.Get();
}

//---------------------------------------------------------------------------------
// ■ アルファテスト

/**
 * @brief アルファテスト設定
 *
 * @param enable tureなら有効, falseなら無効
 */
void BaseModelMaterial::SetAlphaTestEnable(b32 enable)
{
  this->GetBlendStateObject_()->SetAlphaTestEnable(enable);
}

/**
 * @brief アルファテスト有効か？
 *
 * @return tureなら有効, falseなら無効
 */
b32 BaseModelMaterial::IsAlphaTestEnable() const
{
  return this->GetBlendStateObject_()->GetAlphaTestEnable();
}

/**
 * @brief アルファテストの比較関数設定
 *
 * @param testFunction 比較関数
 */
void BaseModelMaterial::SetAlphaTestFunction(gfl2::gfx::CompareFunc testFunction)
{
  (void)testFunction;

  // @attention gfxに機能が無い
}

/**
 * @brief アルファテストの比較関数取得
 *
 * @return 比較関数を返す
 */
gfl2::gfx::CompareFunc BaseModelMaterial::GetAlphaTestFunction() const
{
  // @attention gfxに機能が無い
  return gfl2::gfx::CompareFunc::Always;
}

/**
 * @brief アルファテストの参照値設定
 *
 * @param reference 参照値
 */
void BaseModelMaterial::SetAlphaTestReference(f32 reference)
{
  this->GetBlendStateObject_()->SetAlphaRef(reference);
}

/**
 * @brief アルファテストの参照値取得
 *
 * @return 参照値を返す
 */
f32 BaseModelMaterial::GetAlphaTestReference() const
{
  return this->GetBlendStateObject_()->GetAlphaRef();
}

//---------------------------------------------------------------------------------
// ■ デプス書き込み

/**
 * @brief デプス書き込み設定
 *
 * @param enable trueならデプスを更新する、 falseなら更新しない
 */
void BaseModelMaterial::SetDepthWriteEnable(b32 enable)
{
  // trueのときデプスバッファを更新する
  this->GetDepthStencilStateObject_()->SetDepthWriteEnable(enable);
}

/**
 * @brief デプス書き込み有効か？
 *
 * @return tureなら有効、 falseなら無効
 */
b32 BaseModelMaterial::IsDepthWriteEnable() const
{
  return this->GetDepthStencilStateObject_()->GetDepthWriteEnable();
}

//---------------------------------------------------------------------------------
// ■ デプステスト

/**
 * @brief デプステスト設定
 *
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModelMaterial::SetDepthTestEnable(b32 enable)
{
  this->GetDepthStencilStateObject_()->SetDepthTestEnable(enable);
}

/**
 * @brief デプステスト有効か？
 *
 * @return tureなら有効、 falseなら無効
 */
b32 BaseModelMaterial::IsDepthTestEnable() const
{
  return this->GetDepthStencilStateObject_()->GetDepthTestEnable();
}

/**
 * @brief デプステスト比較関数設定
 *
 * @param testFunction 比較関数
 */
void BaseModelMaterial::SetDepthTestFunction(gfl2::gfx::CompareFunc testFunction)
{
  this->GetDepthStencilStateObject_()->SetDepthFunc(testFunction);
}

/**
 * @brief デプステスト比較関数の取得
 *
 * @return 比較関数を返す
 */
gfl2::gfx::CompareFunc BaseModelMaterial::GetDepthTestFunction() const
{
  return this->GetDepthStencilStateObject_()->GetDepthFunc().Get();
}

//---------------------------------------------------------------------------------
// ■ ステンシルテスト

/**
 * @brief ステンシルテスト設定
 *
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModelMaterial::SetStencilTestEnable(b32 enable)
{
  this->GetDepthStencilStateObject_()->SetStencilTestEnable(enable);
}

/**
 * @brief ステンシルテスト有効か？
 *
 * @return trueなら有効、 falseなら無効
 */
b32 BaseModelMaterial::IsStencilTestEnable() const
{
  return this->GetDepthStencilStateObject_()->GetStencilTestEnable();
}

/**
 * @brief ステンシルテスト比較関数設定：両面設定
 *
 * @param testFunction 比較関数
 * @param reference 参照値
 * @param mask マスク値
 */
void BaseModelMaterial::SetStencilTestFunction(gfl2::gfx::CompareFunc testFunction, u8 reference, u8 mask)
{
  // 3DS準拠で両面に同じ設定を行う
  this->GetDepthStencilStateObject_()->SetStencilFunc(gfl2::gfx::PolygonFace::CW, testFunction, reference, mask);
  this->GetDepthStencilStateObject_()->SetStencilFunc(gfl2::gfx::PolygonFace::CCW, testFunction, reference, mask);
}

/**
 * @brief ステンシルテスト比較関数取得
 *
 * @return 比較関数を返す
 */
gfl2::gfx::CompareFunc BaseModelMaterial::GetStencilTestFunction() const
{
  return this->GetDepthStencilStateObject_()->GetStencilFunc(gfl2::gfx::PolygonFace::CW).Get();
}

/**
 * @brief ステンシルテスト参照値取得
 *
 * @return 参照値を返す
 */
u8 BaseModelMaterial::GetStencilTestReference() const
{
  return this->GetDepthStencilStateObject_()->GetStencilReference();
}

/**
 * @brief ステンシルテストマスク値取得
 *
 * @return マスク値を返す
 */
u8 BaseModelMaterial::GetStencilTestMask() const
{
  return this->GetDepthStencilStateObject_()->GetStencilMask();
}

/**
 * @brief ステンシルテスト処理設定：両面設定
 *
 * @param failOp ステンシルテスト通過できない場合の処理
 * @param depthFailOp デプステスト通過できない場合の処理
 * @param depthPassOp デプステスト通過時の処理
 */
void BaseModelMaterial::SetStencilTestOperation(gfl2::gfx::StencilOp failOp, gfl2::gfx::StencilOp depthFailOp, gfl2::gfx::StencilOp depthPassOp)
{
  // 3DS準拠で両面に同じ設定を行う
  this->GetDepthStencilStateObject_()->SetStencilOp(gfl2::gfx::PolygonFace::CW, failOp, depthFailOp, depthPassOp);
  this->GetDepthStencilStateObject_()->SetStencilOp(gfl2::gfx::PolygonFace::CCW, failOp, depthFailOp, depthPassOp);
}

/**
 * @brief ステンシルテスト：ステンシルテスト通過できない場合の処理の取得
 *
 * @return 処理を返す
 */
gfl2::gfx::StencilOp BaseModelMaterial::GetStencilTestFailOperation() const
{
  return this->GetDepthStencilStateObject_()->GetStencilFailOp(gfl2::gfx::PolygonFace::CW).Get();
}

/**
 * @brief ステンシルテスト：デプステスト通過できない場合の処理の取得
 *
 * @return 処理を返す
 */
gfl2::gfx::StencilOp BaseModelMaterial::GetStencilTestZFailOperation() const
{
  return this->GetDepthStencilStateObject_()->GetStencilDepthFailOp(gfl2::gfx::PolygonFace::CW).Get();
}

/**
 * @brief ステンシルテスト：デプステスト通過時の処理の取得
 *
 * @return 処理を返す
 */
gfl2::gfx::StencilOp BaseModelMaterial::GetStencilTestPassOperation() const
{
  return this->GetDepthStencilStateObject_()->GetStencilPassOp(gfl2::gfx::PolygonFace::CW).Get();
}

//---------------------------------------------------------------------------------
// ■ ブレンド

/**
 * @brief ブレンド設定
 *
 * @param enable trueなら有効、 falseなら無効
 */
void BaseModelMaterial::SetBlendEnable(b32 enable)
{
  this->GetBlendStateObject_()->SetBlendEnable(enable);
}

/**
 * @brief ブレンド有効か？
 *
 * @return trueなら有効、 falseなら無効
 */
b32 BaseModelMaterial::IsBlendEnable() const
{
  return this->GetBlendStateObject_()->GetBlendEnable();
}

/**
 * @brief ブレンド関数設定
 *
 * @param srcRgb ソースＲＧＢ
 * @param dstRgb ディスティネーションＲＧＢ
 * @param srcAlpha ソースα
 * @param dstAlpha ディスティネーションα
 */
void BaseModelMaterial::SetBlendFunction(gfl2::gfx::BlendFunc srcRgb, gfl2::gfx::BlendFunc dstRgb, gfl2::gfx::BlendFunc srcAlpha, gfl2::gfx::BlendFunc dstAlpha)
{
  this->GetBlendStateObject_()->SetBlendFunc(srcRgb, dstRgb, srcAlpha, dstAlpha);
}

/**
 * @brief ブレンド関数：ソースＲＧＢ取得
 *
 * @return ソースＲＧＢを返す
 */
gfl2::gfx::BlendFunc BaseModelMaterial::GetBlendFunctionSourceRgb() const
{
  return this->GetBlendStateObject_()->GetBlendFuncSrcColor().Get();
}

/**
 * @brief ブレンド関数：ソースα取得
 *
 * @return ソースαを返す
 */
gfl2::gfx::BlendFunc BaseModelMaterial::GetBlendFunctionSourceAlpha() const
{
  return this->GetBlendStateObject_()->GetBlendFuncSrcAlpha().Get();
}

/**
 * @brief ブレンド関数：ディスティネーションＲＧＢ取得
 *
 * @return ディスティネーションＲＧＢを返す
 */
gfl2::gfx::BlendFunc BaseModelMaterial::GetBlendFunctionDestinationRgb() const
{
  return this->GetBlendStateObject_()->GetBlendFuncDestColor().Get();
}

/**
 * @brief ブレンド関数：ディスティネーションα取得
 *
 * @return ディスティネーションαを返す
 */
gfl2::gfx::BlendFunc BaseModelMaterial::GetBlendFunctionDestinationAlpha() const
{
  return this->GetBlendStateObject_()->GetBlendFuncDestAlpha().Get();
}

/**
 * @brief ブレンド式設定
 *
 * @param colorEq カラー式
 * @param alphaEq α式
 */
void BaseModelMaterial::SetBlendEquation(gfl2::gfx::BlendOp colorEq, gfl2::gfx::BlendOp alphaEq)
{
  this->GetBlendStateObject_()->SetBlendOp(colorEq, alphaEq);
}

/**
 * @brief ブレンド式取得：カラー
 *
 * @return カラーの式を返す
 */
gfl2::gfx::BlendOp BaseModelMaterial::GetBlendEquationRgb() const
{
  return this->GetBlendStateObject_()->GetBlendOpColor().Get();
}

/**
 * @brief ブレンド式取得：α
 *
 * @return αの式を返す
 */
gfl2::gfx::BlendOp BaseModelMaterial::GetBlendEquationAlpha() const
{
  return this->GetBlendStateObject_()->GetBlendOpAlpha().Get();
}

//---------------------------------------------------------------------------------
// ■ テクスチャマッパー

/**
 * @brief テクスチャの差し替え
 *
 * @param no マッパー番号
 * @param pTextureResourceNode 差し替え先のテクスチャリソース
 */
void BaseModelMaterial::SetTexture(s32 no, const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResourceNode)
{
  m_pMaterialInstanceNode->SetTextureResourceNode(no, pTextureResourceNode);
}

#if MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
//@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
//                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
//                使う側に混乱が起きないよう、nijiでは封印します。
//                本来は実装されるべき機能です。

/**
 * @brief ＵＶのラップ方法設定
 *
 * @param no マッパー番号
 * @param wrapU Ｕ方向の設定
 * @param wrapV Ｖ方向の設定
 */
void BaseModelMaterial::SetTextureMapperWrap(s32 no, gfl2::gfx::TextureAddress wrapU, gfl2::gfx::TextureAddress wrapV)
{
  this->GetSamplerStateObject_(no)->SetSamplerAddress(wrapU, wrapV);
}

/**
 * @brief ＵＶのラップ方法設定取得：Ｕ方向
 *
 * @param no マッパー番号
 *
 * @return Ｕ方向のラップ設定を返す
 */
gfl2::gfx::TextureAddress BaseModelMaterial::GetTextureMapperWrapU(s32 no) const
{
  return this->GetSamplerStateObject_(no)->GetSamplerAddressS().Get();
}

/**
 * @brief ＵＶのラップ方法設定取得：Ｖ方向
 *
 * @param no マッパー番号
 *
 * @return Ｖ方向のラップ設定を返す
 */
gfl2::gfx::TextureAddress BaseModelMaterial::GetTextureMapperWrapV(s32 no) const
{
  return this->GetSamplerStateObject_(no)->GetSamplerAddressT().Get();
}

#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE

//---------------------------------------------------------------------------------
// ■ テクスチャコーディネーター

/**
 * @brief ＵＶのスケール設定
 *
 * @param no コーディネーター番号
 * @param scaleUv スケール値
 */
void BaseModelMaterial::SetTextureCoordinatorScaleUV(s32 no, const gfl2::math::Vector2& scaleUv)
{
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  pTextureAttribute->m_ScaleU = scaleUv.x;
  pTextureAttribute->m_ScaleV = scaleUv.y;
}

/**
 * @brief ＵＶのスケール取得
 *
 * @param no コーディネーター番号
 *
 * @return ＵＶのスケールを返す
 */
gfl2::math::Vector2 BaseModelMaterial::GetTextureCoordinatorScaleUV(s32 no) const
{
  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  gfl2::math::Vector2 scaleUv;
  scaleUv.x = pTextureAttribute->m_ScaleU;
  scaleUv.y = pTextureAttribute->m_ScaleV;

  return scaleUv;
}

/**
 * @brief ＵＶの回転設定
 *
 * @param no コーディネーター番号
 * @param rotateRad 回転値
 */
void BaseModelMaterial::SetTextureCoordinatorRotateUV(s32 no, const f32 rotateRad)
{
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  pTextureAttribute->m_Rotate = rotateRad;
}

/**
 * @brief ＵＶの回転取得
 *
 * @param no コーディネーター番号
 *
 * @return 回転値を返す
 */
f32 BaseModelMaterial::GetTextureCoordinatorRotateUV(s32 no) const
{
  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  return pTextureAttribute->m_Rotate;
}

/**
 * @brief ＵＶの平行移動成分設定
 *
 * @param no コーディネーター番号
 * @param transUv 平行移動成分
 */
void BaseModelMaterial::SetTextureCoordinatorTranslateUV(s32 no, const gfl2::math::Vector2& transUv)
{
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  pTextureAttribute->m_TranslateU = transUv.x;
  pTextureAttribute->m_TranslateV = transUv.y;
}

/**
 * @brief ＵＶの平行移動成分取得
 *
 * @param no コーディネーター番号
 *
 * @return 平行移動値を返す
 */
gfl2::math::Vector2 BaseModelMaterial::GetTextureCoordinatorTranslateUV(s32 no) const
{
  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* pTextureAttribute = this->GetTextureAttribute_(no);

  gfl2::math::Vector2 transUv;
  transUv.x = pTextureAttribute->m_TranslateU;
  transUv.y = pTextureAttribute->m_TranslateV;

  return transUv;
}

}}
