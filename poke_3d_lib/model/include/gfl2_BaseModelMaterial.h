//=============================================================================
/**
 * @file    gfl2_BaseModelMaterial.h
 * @brief   
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.4    ishiguro  「returning reference to local temporary」警告対処
 *  2016.4.8    ishiguro  (MATERIAL_SAMPLER_STATE_OBJECT_ENABLEにて)未使用機能の封印
 *
 */
//=============================================================================
#if !defined(POKE_3D_LIB_MODEL_BASE_MODEL_MATERIAL_H_INCLUDED)
#define POKE_3D_LIB_MODEL_BASE_MODEL_MATERIAL_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_MaterialInstanceNode.h>

namespace poke_3d { namespace model {

/**
 * @brief モデル基底クラス：マテリアルノードのラッパー
 */
class BaseModelMaterial
{
public:

  /**
   * @brief マテリアルのテクスチャの繰り返し設定
   */
  enum TextureWrap
  {
    TEXTURE_WRAP_CLAMP_TO_EDGE    = gfl2::gfx::TextureAddress::Clamp,    // テクスチャの端のピクセルを引き伸ばします。 
    TEXTURE_WRAP_CLAMP_TO_BORDER  = gfl2::gfx::TextureAddress::Border,  // テクスチャの端でボーダーカラーを引き伸ばします。
    TEXTURE_WRAP_REPEAT           = gfl2::gfx::TextureAddress::Wrap,           // テクスチャの端で繰り返します。 
    TEXTURE_WRAP_MIRRORED_REPEAT  = gfl2::gfx::TextureAddress::Mirror   // テクスチャの端で反転しながら繰り返します。 
  };

public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  BaseModelMaterial(gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode); 

  /**
   * @brief デストラクタ
   */
  ~BaseModelMaterial();


  //---------------------------------------------------------------------------------
  // ■ マテリアルカラー

  /**
   * @brief エミッションカラー設定
   *
   * @param color カラー
   */
  void SetEmissionColor(const gfl2::math::Vector4& color);

  /**
   * @brief エミッションカラー設定：要素毎
   *
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetEmissionColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief エミッションカラー取得
   *
   * @return エミッションカラーを返す
   */
  gfl2::math::Vector4 GetEmissionColor() const;

  /**
   * @brief アンビエントカラー設定
   *
   * @param color カラー
   */
  void SetAmbientColor(const gfl2::math::Vector4& color);

  /**
   * @brief アンビエントカラー設定：要素毎
   *
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetAmbientColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief アンビエントカラー取得
   *
   * @return アンビエントカラーを返す
   */
  gfl2::math::Vector4 GetAmbientColor() const;

  /**
   * @brief ディフューズカラー設定
   *
   * @param color カラー
   */
  void SetDiffuseColor(const gfl2::math::Vector4& color);

  /**
   * @brief ディフューズカラー設定：要素毎
   *
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetDiffuseColor(f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief ディフューズカラー取得
   *
   * @return ディフューズカラーを返す
   */
  gfl2::math::Vector4 GetDiffuseColor() const;

  /**
   * @brief スペキュラカラー０設定
   *
   * @param color カラー
   */
  void SetSpecularColor0(const gfl2::math::Vector4& color);

  /**
   * @brief スペキュラカラー０設定：要素毎
   *
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetSpecularColor0(f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief スペキュラカラー０取得
   *
   * @return スペキュラカラー０を返す
   */
  gfl2::math::Vector4 GetSpecularColor0() const;

  /**
   * @brief スペキュラカラー１設定
   *
   * @param color カラー
   */
  void SetSpecularColor1(const gfl2::math::Vector4& color);

  /**
   * @brief スペキュラカラー１設定：要素毎
   *
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetSpecularColor1(f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief スペキュラカラー１取得
   *
   * @return スペキュラカラー１を返す
   */
  gfl2::math::Vector4 GetSpecularColor1() const;

  //---------------------------------------------------------------------------------
  // ■ コンスタントカラー

  /**
   * @brief コンスタントカラー設定
   *
   * @param constantColorIndex コンスタントカラー番号
   * @param color カラー
   */
  void SetConstantColor(s32 constantColorIndex, const gfl2::math::Vector4& color);

  /**
   * @brief コンスタントカラー設定：要素毎
   *
   * @param constantColorIndex コンスタントカラー番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetConstantColor(s32 constantColorIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief コンスタントカラー取得
   *
   * @param constantColorIndex コンスタントカラー番号
   *
   * @return コンスタントカラーを返す
   */
  gfl2::math::Vector4 GetConstantColor(s32 constantColorIndex) const;

  //---------------------------------------------------------------------------------
  // ■ フォグ

  /**
   * @brief フォグの有効設定
   *
   * @param enable trueで有効、falseで無効
   */
  void SetFogEnable(b32 enable);

  /**
   * @brief フォグが有効か？
   *
   * @return enable trueで有効、falseで無効
   */
  b32 IsFogEnable() const;

  /**
   * @brief フォグの有効設定
   *
   * @param enable trueで有効、falseで無効
   */
  void SetFogIndex(s16 fogIndex);

  /**
   * @brief フォグの有効設定
   *
   * @param enable trueで有効、falseで無効
   */
  s16 GetFogIndex() const;

  //---------------------------------------------------------------------------------
  // ■ ライトセット

  /**
   * @brief フォグの有効設定
   *
   * @param lightSetIndex trueで有効、falseで無効
   */
  void SetLightSetIndex(s16 lightSetIndex);

  /**
   * @brief フォグが有効か？
   *
   * @return trueで有効、falseで無効
   */
  s16 GetLightSetIndex() const;

  //---------------------------------------------------------------------------------
  // ■ カリングモード

  /**
   * @brief カリングモード設定
   *
   * @param cullingMode カリングモード
   */
  void SetCullingMode(gfl2::gfx::CullMode cullingMode);

  /**
   * @brief カリングモード取得
   *
   * @return カリングモードを返す
   */
  gfl2::gfx::CullMode GetCullingMode() const;

  //---------------------------------------------------------------------------------
  // ■ アルファテスト

  /**
   * @brief アルファテスト設定
   *
   * @param enable tureなら有効, falseなら無効
   */
  void SetAlphaTestEnable(b32 enable);

  /**
   * @brief アルファテスト有効か？
   *
   * @return tureなら有効, falseなら無効
   */
  b32 IsAlphaTestEnable() const;

  /**
   * @brief アルファテストの比較関数設定
   *
   * @param testFunction 比較関数
   */
  void SetAlphaTestFunction(gfl2::gfx::CompareFunc testFunction);

  /**
   * @brief アルファテストの比較関数取得
   *
   * @return 比較関数を返す
   */
  gfl2::gfx::CompareFunc GetAlphaTestFunction() const;

  /**
   * @brief アルファテストの参照値設定
   *
   * @param reference 参照値
   */
  void SetAlphaTestReference(f32 reference);

  /**
   * @brief アルファテストの参照値取得
   *
   * @return 参照値を返す
   */
  f32 GetAlphaTestReference() const;

  //---------------------------------------------------------------------------------
  // ■ デプス書き込み

  /**
   * @brief デプス書き込み設定
   *
   * @param enable trueならデプスを更新する、 falseなら更新しない
   */
  void SetDepthWriteEnable(b32 enable);

  /**
   * @brief デプス書き込み有効か？
   *
   * @return tureなら有効、 falseなら無効
   */
  b32 IsDepthWriteEnable() const;

  //---------------------------------------------------------------------------------
  // ■ デプステスト

  /**
   * @brief デプステスト設定
   *
   * @param enable trueなら有効、 falseなら無効
   */
  void SetDepthTestEnable(b32 enable);

  /**
   * @brief デプステスト有効か？
   *
   * @return tureなら有効、 falseなら無効
   */
  b32 IsDepthTestEnable() const;

  /**
   * @brief デプステスト比較関数設定
   *
   * @param testFunction 比較関数
   */
  void SetDepthTestFunction(gfl2::gfx::CompareFunc testFunction);

  /**
   * @brief デプステスト比較関数の取得
   *
   * @return 比較関数を返す
   */
  gfl2::gfx::CompareFunc GetDepthTestFunction() const;

  //---------------------------------------------------------------------------------
  // ■ ステンシルテスト

  /**
   * @brief ステンシルテスト設定
   *
   * @param enable trueなら有効、 falseなら無効
   */
  void SetStencilTestEnable(b32 enable);

  /**
   * @brief ステンシルテスト有効か？
   *
   * @return trueなら有効、 falseなら無効
   */
  b32 IsStencilTestEnable() const;

  /**
   * @brief ステンシルテスト比較関数設定：両面設定
   *
   * @param testFunction 比較関数
   * @param reference 参照値
   * @param mask マスク値
   */
  void SetStencilTestFunction(gfl2::gfx::CompareFunc testFunction, u8 reference, u8 mask);

  /**
   * @brief ステンシルテスト比較関数取得
   *
   * @return 比較関数を返す
   */
  gfl2::gfx::CompareFunc GetStencilTestFunction() const;

  /**
   * @brief ステンシルテスト参照値取得
   *
   * @return 参照値を返す
   */
  u8 GetStencilTestReference() const;

  /**
   * @brief ステンシルテストマスク値取得
   *
   * @return マスク値を返す
   */
  u8 GetStencilTestMask() const;

  /**
   * @brief ステンシルテスト処理設定：両面設定
   *
   * @param failOp ステンシルテスト通過できない場合の処理
   * @param depthFailOp デプステスト通過できない場合の処理
   * @param depthPassOp デプステスト通過時の処理
   */
  void SetStencilTestOperation(gfl2::gfx::StencilOp failOp, gfl2::gfx::StencilOp depthFailOp, gfl2::gfx::StencilOp depthPassOp);
  
  /**
   * @brief ステンシルテスト：ステンシルテスト通過できない場合の処理の取得
   *
   * @return 処理を返す
   */
  gfl2::gfx::StencilOp GetStencilTestFailOperation() const;

  /**
   * @brief ステンシルテスト：デプステスト通過できない場合の処理の取得
   *
   * @return 処理を返す
   */
  gfl2::gfx::StencilOp GetStencilTestZFailOperation() const;

  /**
   * @brief ステンシルテスト：デプステスト通過時の処理の取得
   *
   * @return 処理を返す
   */
  gfl2::gfx::StencilOp GetStencilTestPassOperation() const;

  //---------------------------------------------------------------------------------
  // ■ ブレンド

  /**
   * @brief ブレンド設定
   *
   * @param enable trueなら有効、 falseなら無効
   */
  void SetBlendEnable(b32 enable);

  /**
   * @brief ブレンド有効か？
   *
   * @return trueなら有効、 falseなら無効
   */
  b32 IsBlendEnable() const;

  /**
   * @brief ブレンド関数設定
   *
   * @param srcRgb ソースＲＧＢ
   * @param dstRgb ディスティネーションＲＧＢ
   * @param srcAlpha ソースα
   * @param dstAlpha ディスティネーションα
   */
  void SetBlendFunction(gfl2::gfx::BlendFunc srcRgb, gfl2::gfx::BlendFunc dstRgb, gfl2::gfx::BlendFunc srcAlpha, gfl2::gfx::BlendFunc dstAlpha);

  /**
   * @brief ブレンド関数：ソースＲＧＢ取得
   *
   * @return ソースＲＧＢを返す
   */
  gfl2::gfx::BlendFunc GetBlendFunctionSourceRgb() const;

  /**
   * @brief ブレンド関数：ソースα取得
   *
   * @return ソースαを返す
   */
  gfl2::gfx::BlendFunc GetBlendFunctionSourceAlpha() const;

  /**
   * @brief ブレンド関数：ディスティネーションＲＧＢ取得
   *
   * @return ディスティネーションＲＧＢを返す
   */
  gfl2::gfx::BlendFunc GetBlendFunctionDestinationRgb() const;

  /**
   * @brief ブレンド関数：ディスティネーションα取得
   *
   * @return ディスティネーションαを返す
   */
  gfl2::gfx::BlendFunc GetBlendFunctionDestinationAlpha() const;

  /**
   * @brief ブレンド式設定
   *
   * @param colorEq カラー式
   * @param alphaEq α式
   */
  void SetBlendEquation(gfl2::gfx::BlendOp colorEq, gfl2::gfx::BlendOp alphaEq);

  /**
   * @brief ブレンド式取得：カラー
   *
   * @return カラーの式を返す
   */
  gfl2::gfx::BlendOp GetBlendEquationRgb() const;

  /**
   * @brief ブレンド式取得：α
   *
   * @return αの式を返す
   */
  gfl2::gfx::BlendOp GetBlendEquationAlpha() const;

  //---------------------------------------------------------------------------------
  // ■ テクスチャマッパー

  /**
   * @brief テクスチャの差し替え
   *
   * @param no マッパー番号
   * @param pTextureResourceNode 差し替え先のテクスチャリソース
   */
  void SetTexture(s32 no, const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResourceNode);

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
  void SetTextureMapperWrap(s32 no, gfl2::gfx::TextureAddress wrapU, gfl2::gfx::TextureAddress wrapV);

  /**
   * @brief ＵＶのラップ方法設定取得：Ｕ方向
   *
   * @param no マッパー番号
   *
   * @return Ｕ方向のラップ設定を返す
   */
  gfl2::gfx::TextureAddress GetTextureMapperWrapU(s32 no) const;

  /**
   * @brief ＵＶのラップ方法設定取得：Ｖ方向
   *
   * @param no マッパー番号
   *
   * @return Ｖ方向のラップ設定を返す
   */
  gfl2::gfx::TextureAddress GetTextureMapperWrapV(s32 no) const;
#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE

  //---------------------------------------------------------------------------------
  // ■ テクスチャコーディネーター

  /**
   * @brief ＵＶのスケール設定
   *
   * @param no コーディネーター番号
   * @param scaleUv スケール値
   */
  void SetTextureCoordinatorScaleUV(s32 no, const gfl2::math::Vector2& scaleUv);

  /**
   * @brief ＵＶのスケール取得
   *
   * @param no コーディネーター番号
   *
   * @return ＵＶのスケールを返す
   */
  gfl2::math::Vector2 GetTextureCoordinatorScaleUV(s32 no) const;
  
  /**
   * @brief ＵＶの回転設定
   *
   * @param no コーディネーター番号
   * @param rotateRad 回転値
   */
  void SetTextureCoordinatorRotateUV(s32 no, const f32 rotateRad);

  /**
   * @brief ＵＶの回転取得
   *
   * @param no コーディネーター番号
   *
   * @return 回転値を返す
   */
  f32 GetTextureCoordinatorRotateUV(s32 no) const;
  
  /**
   * @brief ＵＶの平行移動成分設定
   *
   * @param no コーディネーター番号
   * @param transUv 平行移動成分
   */
  void SetTextureCoordinatorTranslateUV(s32 no, const gfl2::math::Vector2& transUv);

  /**
   * @brief ＵＶの平行移動成分取得
   *
   * @param no コーディネーター番号
   *
   * @return 平行移動値を返す
   */
  gfl2::math::Vector2 GetTextureCoordinatorTranslateUV(s32 no) const;

private:

  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* GetMaterialAttributeParam_() const
  {
    return m_pMaterialInstanceNode->GetAttributeParam();
  }
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* GetMaterialAttributeParam_()
  {
    return m_pMaterialInstanceNode->GetAttributeParam();
  }

  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::MaterialColor* GetMaterialColor_() const
  {
    return &this->GetMaterialAttributeParam_()->m_MaterialColor;
  }
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::MaterialColor* GetMaterialColor_()
  {
    return &this->GetMaterialAttributeParam_()->m_MaterialColor;
  }

  const gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* GetTextureAttribute_(s32 no) const
  {
    return &m_pMaterialInstanceNode->GetTextureInfo(no)->m_Attribute;
  }
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::TextureInfo::Attribute* GetTextureAttribute_(s32 no)
  {
    return &m_pMaterialInstanceNode->GetTextureInfo(no)->m_Attribute;
  }

  const gfl2::gfx::BlendStateObject* GetBlendStateObject_() const
  {
    return m_pMaterialInstanceNode->GetBlendStateObject();
  }
  gfl2::gfx::BlendStateObject* GetBlendStateObject_()
  {
    return m_pMaterialInstanceNode->GetBlendStateObject();
  }

  const gfl2::gfx::DepthStencilStateObject* GetDepthStencilStateObject_() const
  {
    return m_pMaterialInstanceNode->GetDepthStencilStateObject();
  }
  gfl2::gfx::DepthStencilStateObject* GetDepthStencilStateObject_()
  {
    return m_pMaterialInstanceNode->GetDepthStencilStateObject();
  }

#if MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
  //@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
  //                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
  //                使う側に混乱が起きないよう、nijiでは封印します。
  //                本来は実装されるべき機能です。
  const gfl2::gfx::SamplerStateObject* GetSamplerStateObject_(s32 no) const
  {
    return m_pMaterialInstanceNode->GetSamplerStateObject(no);
  }
  gfl2::gfx::SamplerStateObject* GetSamplerStateObject_(s32 no)
  {
    // @attention マテリアルインスタンスノードにサンプラーステートのコピーが無いのでリソースを直で書き換えてしまう
    // gfl2_MaterialInstanceNode.h の MATERIAL_INSTANCE_COPY_TEXTURE_INFO_ONLY を 0 にすることでメモリサイズは増えるがコピーは可能
    return (gfl2::gfx::SamplerStateObject*)m_pMaterialInstanceNode->GetSamplerStateObject(no);
  }
#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE

private:

  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode*   m_pMaterialInstanceNode;
};

}}

#endif
