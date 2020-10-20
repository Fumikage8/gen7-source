//=============================================================================
/**
 * @file    gfl2_BaseModel.h
 * @brief   
 * @author  takebe_makoto
 * @date    2014
 *
 *  2016.5.4    ishiguro  「returning reference to local temporary」警告対処
 *  2016.4.19   ishiguro  DetachLocatorNode関数追加
 *  2016.4.8    ishiguro  (MATERIAL_SAMPLER_STATE_OBJECT_ENABLEにて)未使用機能の封印
 *  2016.4.8    ishiguro  ＣＴＲにおきまして、マテリアルのDepthStencilStateObjectがマテリアルの初期値を引き継いでいなかったのでコピー関数追加。
 *
 */
//=============================================================================
#if !defined(POKE_3D_LIB_MODEL_BASE_MODEL_H_INCLUDED)
#define POKE_3D_LIB_MODEL_BASE_MODEL_H_INCLUDED

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LocatorInstanceNode.h>

#include <math/include/gfl2_EulerRotation.h>
#include <util/include/gfl2_FnvHash.h>

#include "gfl2_BaseModelMaterial.h"
#include "gfl2_BaseModelAnimation.h"

#include <Animation/include/gfl2_AnimationPackList.h>
#include <Animation/include/gfl2_AnimationId.h>
#include <Animation/include/gfl2_InterestController.h>
#include <Animation/include/gfl2_SpringController.h>

namespace poke_3d { namespace model {


/**
 * @brief モデル基底クラス
 */
class BaseModel
{
public:

  /**
   * @brief マテリアルのカリングモード
   */
  enum MaterialCullingMode
  {
		MATERIAL_CULLING_MODE_FRONT = gfl2::gfx::CullMode::CW,		   ///< 表面カリング
		MATERIAL_CULLING_MODE_BACK = gfl2::gfx::CullMode::CCW,		   ///< 裏面カリング
		MATERIAL_CULLING_MODE_NONE = gfl2::gfx::CullMode::None		   ///< カリング無し（両面表示）
  };

  /**
   * @brief マテリアルの比較関数：アルファテスト、デプステスト、ステンシルテスト用
   */
  enum MaterialTestFunction
  {
    MATERIAL_TEST_FUNCTION_NEVER = gfl2::gfx::CompareFunc::Never,         ///< すべて不通過です。
    MATERIAL_TEST_FUNCTION_ALWAYS = gfl2::gfx::CompareFunc::Always,       ///< すべて通過です。
    MATERIAL_TEST_FUNCTION_EQUAL = gfl2::gfx::CompareFunc::Equal,         ///< 値と等しければ通過します。
    MATERIAL_TEST_FUNCTION_NOTEQUAL = gfl2::gfx::CompareFunc::NotEqual,   ///< 値と等しくなければ通過します。
    MATERIAL_TEST_FUNCTION_LESS = gfl2::gfx::CompareFunc::Less,           ///< 値より小さければ通過します。
    MATERIAL_TEST_FUNCTION_LEQUAL = gfl2::gfx::CompareFunc::LessEqual,    ///< 値以下であれば通過します。
    MATERIAL_TEST_FUNCTION_GREATER = gfl2::gfx::CompareFunc::Greater,     ///< 値より大きければ通過します。
    MATERIAL_TEST_FUNCTION_GEQUAL = gfl2::gfx::CompareFunc::GreaterEqual  ///< 値以上であれば通過します。
  };

  /**
   * @brief マテリアルのステンシル処理
   */
  enum MaterialStencilOperation
  {
    MATERIAL_STENCIL_OPERATION_KEEP           = gfl2::gfx::StencilOp::Keep,           ///< 現在の値を保持します。
    MATERIAL_STENCIL_OPERATION_ZERO           = gfl2::gfx::StencilOp::Zero,           ///< 0に置換します。 
    MATERIAL_STENCIL_OPERATION_REPLACE        = gfl2::gfx::StencilOp::Replace,        ///< 参照値に置換します。 
    MATERIAL_STENCIL_OPERATION_INCREASE       = gfl2::gfx::StencilOp::Increment,      ///< 増加させます。極値処理はクランプです。
    MATERIAL_STENCIL_OPERATION_DECREASE       = gfl2::gfx::StencilOp::Decrement,      ///< 減少させます。極値処理はクランプです。
    MATERIAL_STENCIL_OPERATION_INVERT         = gfl2::gfx::StencilOp::Invert,         ///< ビット単位で反転させます。 
    MATERIAL_STENCIL_OPERATION_INCREASE_WRAP  = gfl2::gfx::StencilOp::IncrementWrap,  ///< 増加させます。極値処理は回り込みです。
    MATERIAL_STENCIL_OPERATION_DECREASE_WRAP  = gfl2::gfx::StencilOp::DecrementWrap   ///< 減少させます。極値処理は回り込みです。 
  };
  
  /**
   * @brief マテリアルのブレンダーの入出力の要素
   */
  enum MaterialBlendFactor
  {
    MATERIAL_BLEND_FACTOR_ZERO                     = gfl2::gfx::BlendFunc::Zero,                  // 指定の成分に「 0 」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE                      = gfl2::gfx::BlendFunc::One,                   // 指定の成分に「 1 」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_COLOR                = gfl2::gfx::BlendFunc::SrcColor,              // 指定の成分に「ソースカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR      = gfl2::gfx::BlendFunc::InverseSrcColor,       // 指定の成分に「1 - ソースカラー」 を利用します。 
    MATERIAL_BLEND_FACTOR_DST_COLOR                = gfl2::gfx::BlendFunc::DestColor,             // 指定の成分に「デスティネーションカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_COLOR      = gfl2::gfx::BlendFunc::InverseDestColor,      // 指定の成分に「 1 - デスティネーションカラー」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA                = gfl2::gfx::BlendFunc::SrcAlpha,              // 指定の成分に「ソースアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      = gfl2::gfx::BlendFunc::InverseSrcAlpha,       // 指定の成分に「 1 - ソースアルファ」 を利用します。 
    MATERIAL_BLEND_FACTOR_DST_ALPHA                = gfl2::gfx::BlendFunc::DestAlpha,             // 指定の成分に「デスティネーションアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA      = gfl2::gfx::BlendFunc::InverseDestAlpha,      // 指定の成分に「 1 - デスティネーションアルファ」を利用します。 
    MATERIAL_BLEND_FACTOR_SRC_ALPHA_SATURATE       = gfl2::gfx::BlendFunc::SrcAlphaSaturate       // 指定の成分に「 MIN( ソースアルファ , 1 - デスティネーションアルファ ) 」を利用します。 
  };

  /**
   * @brief マテリアルのブレンダーのブレンド式
   */
  enum MaterialBlendEquation
  {
    MATERIAL_BLEND_EQUATION_FUNC_ADD              = gfl2::gfx::BlendOp::Add,               // ソースとデスティネーションを加算します。 
    MATERIAL_BLEND_EQUATION_FUNC_SUBTRACT         = gfl2::gfx::BlendOp::Subtract,           // ソースからデスティネーションを減算します。
    MATERIAL_BLEND_EQUATION_FUNC_REVERSE_SUBTRACT = gfl2::gfx::BlendOp::ReverseSubtract,   // デスティネーションからソースを減算します。
    MATERIAL_BLEND_EQUATION_MIN                   = gfl2::gfx::BlendOp::Min,                     // ソースとデスティネーションのより小さい方です。
    MATERIAL_BLEND_EQUATION_MAX                   = gfl2::gfx::BlendOp::Max                      // ソースとデスティネーションのより大きい方です。
  };

  /**
   * @brief マテリアルのテクスチャの繰り返し設定
   */
  enum MaterialTextureWrap
  {
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_EDGE    = gfl2::gfx::TextureAddress::Clamp,    // テクスチャの端のピクセルを引き伸ばします。 
    MATERIAL_TEXTURE_WRAP_CLAMP_TO_BORDER  = gfl2::gfx::TextureAddress::Border,  // テクスチャの端でボーダーカラーを引き伸ばします。
    MATERIAL_TEXTURE_WRAP_REPEAT           = gfl2::gfx::TextureAddress::Wrap,           // テクスチャの端で繰り返します。 
    MATERIAL_TEXTURE_WRAP_MIRRORED_REPEAT  = gfl2::gfx::TextureAddress::Mirror   // テクスチャの端で反転しながら繰り返します。 
  };

  /**
   * @brief アニメーションビット
   */
  enum AnimationBit
  {
    ANIMATION_BIT_NONE = gfl2::animation::AnimationController::ANIMATION_BIT_NONE,              ///< 無効

    ANIMATION_BIT_JOINT = gfl2::animation::AnimationController::ANIMATION_BIT_JOINT,            ///< ジョイントアニメーション
    ANIMATION_BIT_MATERIAL = gfl2::animation::AnimationController::ANIMATION_BIT_MATERIAL,      ///< マテリアルアニメーション
    ANIMATION_BIT_VISIBILITY = gfl2::animation::AnimationController::ANIMATION_BIT_VISIBILITY,  ///< ビジビリティアニメーション
    ANIMATION_BIT_BLENDSHAPE = gfl2::animation::AnimationController::ANIMATION_BIT_BLENDSHAPE,  ///< ブレンドシェイプアニメーション
    ANIMATION_BIT_KEY = gfl2::animation::AnimationController::ANIMATION_BIT_KEY,                ///< キーアニメーション

    ANIMATION_BIT_ALL = gfl2::animation::AnimationController::ANIMATION_BIT_ALL                 ///< 全アニメーション
  };

  /**
   * @brief 初期化設定
   */
  struct InitDescription
  {
    u8 animationSlotCount;     ///< アニメーションスロット数
    u8 animationBit;           ///< 有効にするアニメーションにビット

    InitDescription() :
      animationSlotCount(0),
      animationBit(0)
    {
    }
  };

  /**
   * @brief アニメーションIDの高速取得用
   */
  static const u32 SLOT_CACHE_CNT_FOR_ANIMATION_ID = 8;
  static const u32 BIT_CACHE_CNT_FOR_ANIMATION_ID  = 5;

public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  BaseModel(); 
  /**
   * @brief デストラクタ
   */
  virtual ~BaseModel();

  /**
   * @brief リソースノードから作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pModelResource ベースモデルリソース
   * @param pMotionConfigResource モーションコンフィグリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource = NULL);
  
  /**
   * @brief リソースノードから作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pModelResource ベースモデルリソース
   * @param pTextureShaderNodeRoot テクスチャリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
    gfl2::renderingengine::scenegraph::DagNode* pTextureShaderNodeRoot);

  /**
   * @brief ジョイント共有モデルを作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pModelResource ベースモデルリソース
   * @param pJointReferenceModel ジョイント共有モデル
   * @param pMotionConfigResource モーションコンフィグリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
    BaseModel* pJointReferenceModel,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource = NULL);

  /**
   * @brief モデルインスタンスノードを指定して作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pModelInstance モデルインスタンス
   * @param pMotionConfigResource モーションコンフィグリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource = NULL);

  /**
   * @brief 破棄
   */
  virtual void Destroy();

  /**
   * @brief 破棄が可能か確認
   */
  virtual bool CanDestroy() const;

  /**
   * @brief 解放チェック有効無効設定
   */
  virtual void SetDestroyCheckEnable(bool enable);

  /**
   * @brief 作成済みか確認
   *
   * @return true:作成済み false:作成されていない
   */
  bool IsCreated() const;

  //---------------------------------------------------------------------------------
  // ■ 階層構造、親子関係

  /**
   * @brief 親モデルの設定
   *
   * @param pParent 親になるモデル
   * @param jointIndex 接続するジョイント番号：-1でモデル自体に接続
   */
  void SetParent(BaseModel* pParent, s32 jointIndex = -1);

  /**
   * @brief 親モデルの取得
   *
   * @return 親モデルを返す
   */
  BaseModel* GetParent() const;

  /**
   * @brief 親モデルの接続先のジョイント番号取得
   *
   * @return 親モデルの接続先のジョイント番号を返す、-1でモデル自体に接続
   */
  s32 GetParentJointIndex() const;

  //---------------------------------------------------------------------------------
  // ■ 位置、回転、拡縮の設定

  /**
   * @brief 位置設定
   *
   * @param position 位置
   */
  void SetPosition(const gfl2::math::Vector3& position);

  /**
   * @brief 位置設定：要素毎
   *
   * @param positionX 位置Ｘ
   * @param positionY 位置Ｙ
   * @param positionZ 位置Ｚ
   */
  void SetPosition(f32 positionX, f32 positionY, f32 positionZ);

  /**
   * @brief 位置取得
   *
   * @return 位置を返す
   */
  const gfl2::math::Vector3& GetPosition() const;

  /**
   * @brief 回転設定
   *
   * @param rotationRad 回転（ラジアン）
   * @param rotationOrder 回転順：デフォルトはZYX
   */
  void SetRotation(const gfl2::math::Vector3& rotationRad, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);

  /**
   * @brief 回転設定：要素毎
   *
   * @param rotationRadX 回転Ｘ（ラジアン）
   * @param rotationRadY 回転Ｙ（ラジアン）
   * @param rotationRadZ 回転Ｚ（ラジアン）
   * @param rotationOrder 回転順：デフォルトはZYX
   */
  void SetRotation(f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);

  /**
   * @brief 回転取得
   *        ※ SetRotation以外で回転を設定した場合は見た目に合わない値が返ります
   *
   * @return 回転を返す
   */
  gfl2::math::Vector3 GetRotation() const;

  /**
   * @brief 拡縮設定
   *
   * @param scale 拡縮
   */
  void SetScale(const gfl2::math::Vector3& scale);

  /**
   * @brief 拡縮設定：要素毎
   *
   * @param scaleX 拡縮Ｘ
   * @param scaleY 拡縮Ｙ
   * @param scaleZ 拡縮Ｚ
   */
  void SetScale(f32 scaleX, f32 scaleY, f32 scaleZ);

  /**
   * @brief 拡縮取得
   *
   * @return 拡縮を返す
   */
  const gfl2::math::Vector3& GetScale() const;


  //---------------------------------------------------------------------------------
  // ■ 回転の設定：クォータニオン

  /**
   * @brief 回転設定：クォータニオン
   *
   * @param rotation 回転
   */
  void SetRotationQuat(const gfl2::math::Quaternion& rotation);

  /**
   * @brief 回転取得：クォータニオン
   *
   * @return 回転を返す
   */
  const gfl2::math::Quaternion& GetRotationQuat() const;

  //---------------------------------------------------------------------------------
  // ■ メッシュ

  /**
   * @brief メッシュ数取得
   *
   * @return メッシュ数を返す
   */
  s32 GetMeshCount() const;

  /**
   * @brief メッシュ番号取得
   *
   * @param pName 検索するメッシュ名
   *
   * @return メッシュ番号を返す：見つからなければ-1
   */
  s32 GetMeshIndex(const char* pName) const;

  /**
   * @brief メッシュの表示設定
   *
   * @param meshIndex メッシュ番号
   * @param visible tureなら表示、 falseなら非表示
   */
  void SetMeshVisible(const s32 meshIndex, b32 visible);

  /**
   * @brief メッシュが表示されるか？
   *
   * @param meshIndex メッシュ番号
   *
   * @return tureなら表示、 falseなら非表示
   */
  b32 IsMeshVisible(const s32 meshIndex) const;

  //---------------------------------------------------------------------------------
  // ■ 表示のON/OFF

  /**
   * @brief 表示設定
   *
   * @param flag trueなら表示、falseなら非表示
   */
  virtual void SetVisible(b32 flag);

  /**
   * @brief 表示されているか？
   *
   * @return trueなら表示、falseなら非表示
   */
  virtual b32 IsVisible() const;

  //---------------------------------------------------------------------------------
  // ■ モデル行列

  /**
   * @brief ワールド行列取得 ※シーンのトラバース後に有効
   *
   * @return ワールド行列を返す
   */
  const gfl2::math::Matrix34& GetWorldMatrix();

  //---------------------------------------------------------------------------------
  // ■ ジョイント

  /**
   * @brief ジョイント数取得
   *
   * @return ジョイント数を返す
   */
  s32 GetJointCount() const;

  /**
   * @brief ジョイント番号取得
   *
   * @param pName 検索するジョイント名
   *
   * @return ジョイント番号を返す：見つからなければ-1
   */
  s32 GetJointIndex(const char* pName) const;

  /**
   * @brief ジョイントのローカルSRT取得
   *
   * @param jointIndex ジョイント番号
   *
   * @return ジョイントのローカルSRTを返す
   */
  const gfl2::math::SRT& GetJointLocalSRT(s32 jointIndex) const;

  /**
   * @brief ジョイントのワールド位置取得 ※シーンのトラバース後に有効
   *
   * @param jointIndex ジョイント番号
   *
   * @return ジョイントのワールド位置を返す
   */
  gfl2::math::Vector3 GetJointWorldPosition(s32 jointIndex) const;

  /**
   * @brief ジョイントのワールド行列取得 ※シーンのトラバース後に有効
   *
   * @param jointIndex ジョイント番号
   *
   * @return ジョイントのワールド行列を返す
   */
  const gfl2::math::Matrix34& GetJointWorldMatrix(s32 jointIndex) const;

  /**
   * @brief ロケーターノードをアタッチ
   *
   * @param pLocatorNodeTop ロケーターノード
   */
  void AttachLocatorNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pLocatorNodeTop);

  /**
   * @brief ロケーターノードをデタッチ
   *
   * @param pLocatorNodeTop ロケーターノード
   */
  void DetachLocatorNode(gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* pLocatorNodeTop);


  //---------------------------------------------------------------------------------
  // ■ マテリアル

  /**
   * @brief マテリアル数取得
   *
   * @return マテリアル数を返す
   */
  u32 GetMaterialCount() const;

  /**
   * @brief マテリアル番号取得
   *
   * @param pName 検索するマテリアル名
   *
   * @return マテリアル番号を返す：見つからなければ-1
   */
  s32 GetMaterialIndex(const c8* pName) const;

  //---------------------------------------------------------------------------------
  // ■ マテリアルカラー

  /**
   * @brief エミッションカラー設定
   *
   * @param materialIndex マテリアル番号
   * @param color カラー
   */
  void SetMaterialColorEmission(s32 materialIndex, const gfl2::math::Vector4& color);

  /**
   * @brief エミッションカラー設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorEmission(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief エミッションカラー取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return エミッションカラーを返す
   */
  gfl2::math::Vector4 GetMaterialColorEmission(s32 materialIndex) const;

  /**
   * @brief アンビエントカラー設定
   *
   * @param materialIndex マテリアル番号
   * @param color カラー
   */
  void SetMaterialColorAmbient(s32 materialIndex, const gfl2::math::Vector4& color);

  /**
   * @brief アンビエントカラー設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorAmbient(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief アンビエントカラー取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return アンビエントカラーを返す
   */
  gfl2::math::Vector4 GetMaterialColorAmbient(s32 materialIndex) const;

  /**
   * @brief ディフューズカラー設定
   *
   * @param materialIndex マテリアル番号
   * @param color カラー
   */
  void SetMaterialColorDiffuse(s32 materialIndex, const gfl2::math::Vector4& color);

  /**
   * @brief ディフューズカラー設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorDiffuse(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief ディフューズカラー取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return ディフューズカラーを返す
   */
  gfl2::math::Vector4 GetMaterialColorDiffuse(s32 materialIndex) const;

  /**
   * @brief スペキュラカラー０設定
   *
   * @param materialIndex マテリアル番号
   * @param color カラー
   */
  void SetMaterialColorSpecular0(s32 materialIndex, const gfl2::math::Vector4& color);

  /**
   * @brief スペキュラカラー０設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorSpecular0(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief スペキュラカラー０取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return スペキュラカラー０を返す
   */
  gfl2::math::Vector4 GetMaterialColorSpecular0(s32 materialIndex) const;

  /**
   * @brief スペキュラカラー１設定
   *
   * @param materialIndex マテリアル番号
   * @param color カラー
   */
  void SetMaterialColorSpecular1(s32 materialIndex, const gfl2::math::Vector4& color);

  /**
   * @brief スペキュラカラー１設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorSpecular1(s32 materialIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief スペキュラカラー１取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return スペキュラカラー１を返す
   */
  gfl2::math::Vector4 GetMaterialColorSpecular1(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ コンスタントカラー

  /**
   * @brief コンスタントカラー設定
   *
   * @param materialIndex マテリアル番号
   * @param constantColorIndex コンスタントカラー番号
   * @param color カラー
   */
  void SetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex, const gfl2::math::Vector4& color);

  /**
   * @brief コンスタントカラー設定：要素毎
   *
   * @param materialIndex マテリアル番号
   * @param constantColorIndex コンスタントカラー番号
   * @param colorR カラーＲ
   * @param colorG カラーＧ
   * @param colorB カラーＢ
   * @param colorA カラーＡ
   */
  void SetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex, f32 colorR, f32 colorG, f32 colorB, f32 colorA);

  /**
   * @brief コンスタントカラー取得
   *
   * @param materialIndex マテリアル番号
   * @param constantColorIndex コンスタントカラー番号
   *
   * @return コンスタントカラーを返す
   */
  gfl2::math::Vector4 GetMaterialColorConstant(s32 materialIndex, s32 constantColorIndex) const;

  //---------------------------------------------------------------------------------
  // ■ フォグ

  /**
   * @brief フォグの有効設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueで有効、falseで無効
   */
  void SetMaterialFogEnable(s32 materialIndex, b32 enable);

  /**
   * @brief フォグが有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return enable trueで有効、falseで無効
   */
  b32 IsMaterialFogEnable(s32 materialIndex) const;

  /**
   * @brief フォグの有効設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueで有効、falseで無効
   */
  void SetMaterialFogIndex(s32 materialIndex, s16 fogIndex);

  /**
   * @brief フォグの有効設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueで有効、falseで無効
   */
  s16 GetMaterialFogIndex(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ ライトセット

  /**
   * @brief フォグの有効設定
   *
   * @param materialIndex マテリアル番号
   * @param lightSetIndex trueで有効、falseで無効
   */
  void SetMaterialLightSetIndex(s32 materialIndex, s16 lightSetIndex);

  /**
   * @brief フォグが有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return trueで有効、falseで無効
   */
  s16 GetMaterialLightSetIndex(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ カリングモード

  /**
   * @brief カリングモード設定
   *
   * @param materialIndex マテリアル番号
   * @param cullingMode カリングモード
   */
  void SetMaterialCullingMode(s32 materialIndex, MaterialCullingMode cullingMode);

  /**
   * @brief カリングモード取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return カリングモードを返す
   */
  MaterialCullingMode GetMaterialCullingMode(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ アルファテスト

  /**
   * @brief アルファテスト設定
   *
   * @param materialIndex マテリアル番号
   * @param enable tureなら有効, falseなら無効
   */
  void SetMaterialAlphaTestEnable(s32 materialIndex, b32 enable);

  /**
   * @brief アルファテスト有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return tureなら有効, falseなら無効
   */
  b32 IsMaterialAlphaTestEnable(s32 materialIndex) const;

  /**
   * @brief アルファテストの比較関数設定
   *
   * @param materialIndex マテリアル番号
   * @param testFunction 比較関数
   */
  void SetMaterialAlphaTestFunction(s32 materialIndex, MaterialTestFunction testFunction);

  /**
   * @brief アルファテストの比較関数取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 比較関数を返す
   */
  MaterialTestFunction GetMaterialAlphaTestFunction(s32 materialIndex) const;

  /**
   * @brief アルファテストの参照値設定
   *
   * @param materialIndex マテリアル番号
   * @param reference 参照値
   */
  void SetMaterialAlphaTestReference(s32 materialIndex, f32 reference);

  /**
   * @brief アルファテストの参照値取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 参照値を返す
   */
  f32 GetMaterialAlphaTestReference(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ デプス書き込み

  /**
   * @brief デプス書き込み設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueならデプスを更新する、 falseなら更新しない
   */
  void SetMaterialDepthWriteEnable(s32 materialIndex, b32 enable);

  /**
   * @brief デプス書き込み有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return tureなら有効、 falseなら無効
   */
  b32 IsMaterialDepthWriteEnable(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ デプステスト

  /**
   * @brief デプステスト設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueなら有効、 falseなら無効
   */
  void SetMaterialDepthTestEnable(s32 materialIndex, b32 enable);

  /**
   * @brief デプステスト有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return tureなら有効、 falseなら無効
   */
  b32 IsMaterialDepthTestEnable(s32 materialIndex) const;

  /**
   * @brief デプステスト比較関数設定
   *
   * @param materialIndex マテリアル番号
   * @param testFunction 比較関数
   */
  void SetMaterialDepthTestFunction(s32 materialIndex, MaterialTestFunction testFunction);

  /**
   * @brief デプステスト比較関数の取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 比較関数を返す
   */
  MaterialTestFunction GetMaterialDepthTestFunction(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ ステンシルテスト

  /**
   * @brief ステンシルテスト設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueなら有効、 falseなら無効
   */
  void SetMaterialStencilTestEnable(s32 materialIndex, b32 enable);

  /**
   * @brief ステンシルテスト有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return trueなら有効、 falseなら無効
   */
  b32 IsMaterialStencilTestEnable(s32 materialIndex) const;

  /**
   * @brief ステンシルテスト比較関数設定：両面設定
   *
   * @param materialIndex マテリアル番号
   * @param testFunction 比較関数
   * @param reference 参照値
   * @param mask マスク値
   */
  void SetMaterialStencilTestFunction(s32 materialIndex, MaterialTestFunction testFunction, u8 reference, u8 mask);

  /**
   * @brief ステンシルテスト比較関数取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 比較関数を返す
   */
  MaterialTestFunction GetMaterialStencilTestFunction(s32 materialIndex) const;

  /**
   * @brief ステンシルテスト参照値取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 参照値を返す
   */
  u8 GetMaterialStencilTestReference(s32 materialIndex) const;

  /**
   * @brief ステンシルテストマスク値取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return マスク値を返す
   */
  u8 GetMaterialStencilTestMask(s32 materialIndex) const;

  /**
   * @brief ステンシルテスト処理設定：両面設定
   *
   * @param materialIndex マテリアル番号
   * @param failOp ステンシルテスト通過できない場合の処理
   * @param depthFailOp デプステスト通過できない場合の処理
   * @param depthPassOp デプステスト通過時の処理
   */
  void SetMaterialStencilTestOperation(s32 materialIndex, MaterialStencilOperation failOp, MaterialStencilOperation depthFailOp, MaterialStencilOperation depthPassOp);
  
  /**
   * @brief ステンシルテスト：ステンシルテスト通過できない場合の処理の取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 処理を返す
   */
  MaterialStencilOperation GetMaterialStencilTestFailOperation(s32 materialIndex) const;

  /**
   * @brief ステンシルテスト：デプステスト通過できない場合の処理の取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 処理を返す
   */
  MaterialStencilOperation GetMaterialStencilTestZFailOperation(s32 materialIndex) const;

  /**
   * @brief ステンシルテスト：デプステスト通過時の処理の取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return 処理を返す
   */
  MaterialStencilOperation GetMaterialStencilTestPassOperation(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ ブレンド

  /**
   * @brief ブレンド設定
   *
   * @param materialIndex マテリアル番号
   * @param enable trueなら有効、 falseなら無効
   */
  void SetMaterialBlendEnable(s32 materialIndex, b32 enable);

  /**
   * @brief ブレンド有効か？
   *
   * @param materialIndex マテリアル番号
   *
   * @return trueなら有効、 falseなら無効
   */
  b32 IsMaterialBlendEnable(s32 materialIndex) const;

  /**
   * @brief ブレンド関数設定
   *
   * @param materialIndex マテリアル番号
   * @param srcRgb ソースＲＧＢ
   * @param dstRgb ディスティネーションＲＧＢ
   * @param srcAlpha ソースα
   * @param dstAlpha ディスティネーションα
   */
  void SetMaterialBlendFunction(s32 materialIndex, MaterialBlendFactor srcRgb, MaterialBlendFactor dstRgb, MaterialBlendFactor srcAlpha, MaterialBlendFactor dstAlpha);

  /**
   * @brief ブレンド関数：ソースＲＧＢ取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return ソースＲＧＢを返す
   */
  MaterialBlendFactor GetMaterialBlendFunctionSourceRgb(s32 materialIndex) const;

  /**
   * @brief ブレンド関数：ソースα取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return ソースαを返す
   */
  MaterialBlendFactor GetMaterialBlendFunctionSourceAlpha(s32 materialIndex) const;

  /**
   * @brief ブレンド関数：ディスティネーションＲＧＢ取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return ディスティネーションＲＧＢを返す
   */
  MaterialBlendFactor GetMaterialBlendFunctionDestinationRgb(s32 materialIndex) const;

  /**
   * @brief ブレンド関数：ディスティネーションα取得
   *
   * @param materialIndex マテリアル番号
   *
   * @return ディスティネーションαを返す
   */
  MaterialBlendFactor GetMaterialBlendFunctionDestinationAlpha(s32 materialIndex) const;

  /**
   * @brief ブレンド式設定
   *
   * @param materialIndex マテリアル番号
   * @param colorEq カラー式
   * @param alphaEq α式
   */
  void SetMaterialBlendEquation(s32 materialIndex, MaterialBlendEquation colorEq, MaterialBlendEquation alphaEq);

  /**
   * @brief ブレンド式取得：カラー
   *
   * @param materialIndex マテリアル番号
   *
   * @return カラーの式を返す
   */
  MaterialBlendEquation GetMaterialBlendEquationRgb(s32 materialIndex) const;

  /**
   * @brief ブレンド式取得：α
   *
   * @param materialIndex マテリアル番号
   *
   * @return αの式を返す
   */
  MaterialBlendEquation GetMaterialBlendEquationAlpha(s32 materialIndex) const;

  //---------------------------------------------------------------------------------
  // ■ テクスチャマッパー

  /**
   * @brief テクスチャの差し替え
   *
   * @param materialIndex マテリアル番号
   * @param no マッパー番号
   * @param pTextureResourceNode 差し替え先のテクスチャリソース
   */
  void SetMaterialTexture(s32 materialIndex, s32 no, const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResourceNode);

#if MATERIAL_SAMPLER_STATE_OBJECT_ENABLE
  //@attention      不要なわけではありません。2016/04/08時点で使用箇所が無いのと、
  //                ＣＴＲ版ではマテリアルがSamplerStateObjectを持たずＮＵＬＬを返す状態なので、
  //                使う側に混乱が起きないよう、nijiでは封印します。
  //                本来は実装されるべき機能です。

  /**
   * @brief ＵＶのラップ方法設定
   *
   * @param materialIndex マテリアル番号
   * @param no マッパー番号
   * @param wrapU Ｕ方向の設定
   * @param wrapV Ｖ方向の設定
   */
  void SetMaterialTextureMapperWrap(s32 materialIndex, s32 no, MaterialTextureWrap wrapU, MaterialTextureWrap wrapV);

  /**
   * @brief ＵＶのラップ方法設定取得：Ｕ方向
   *
   * @param materialIndex マテリアル番号
   * @param no マッパー番号
   *
   * @return Ｕ方向のラップ設定を返す
   */
  MaterialTextureWrap GetMaterialTextureMapperWrapU(s32 materialIndex, s32 no) const;

  /**
   * @brief ＵＶのラップ方法設定取得：Ｖ方向
   *
   * @param materialIndex マテリアル番号
   * @param no マッパー番号
   *
   * @return Ｖ方向のラップ設定を返す
   */
  MaterialTextureWrap GetMaterialTextureMapperWrapV(s32 materialIndex, s32 no) const;
#endif//  MATERIAL_SAMPLER_STATE_OBJECT_ENABLE

  //---------------------------------------------------------------------------------
  // ■ テクスチャコーディネーター

  /**
   * @brief ＵＶのスケール設定
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   * @param scaleUv スケール値
   */
  void SetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 no, const gfl2::math::Vector2& scaleUv);

  /**
   * @brief ＵＶのスケール取得
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   *
   * @return ＵＶのスケールを返す
   */
  gfl2::math::Vector2 GetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 no) const;
  
  /**
   * @brief ＵＶの回転設定
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   * @param rotateRad 回転値
   */
  void SetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 no, const f32 rotateRad);

  /**
   * @brief ＵＶの回転取得
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   *
   * @return 回転値を返す
   */
  f32 GetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 no) const;
  
  /**
   * @brief ＵＶの平行移動成分設定
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   * @param transUv 平行移動成分
   */
  void SetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 no, const gfl2::math::Vector2& transUv);

  /**
   * @brief ＵＶの平行移動成分取得
   *
   * @param materialIndex マテリアル番号
   * @param no コーディネーター番号
   *
   * @return 平行移動値を返す
   */
  gfl2::math::Vector2 GetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 no) const;

  //---------------------------------------------------------------------------------
  // ■ アニメーション

  /**
   * @brief アニメーションコントローラーを持っているか？
   *
   * @return true:持っている false:持っていない
   */
  bool HasAnimationController() const;

  /**
   * @brief アニメーションの更新
   *
   * @param animationBit アニメーションの種類
   */
  virtual f32 UpdateAnimation(AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションの更新
   *
   * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
   * @param animationBit アニメーションの種類
   *
   * @return f32 補間率
   */
  f32 UpdateAnimationManual(f32 interpWeight, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションパックリストの設定
   *
   * @param pAnimationPackList セットするアニメーションパックリスト
   */
  void SetAnimationPackList(gfl2::animation::AnimationPackList* pAnimationPackList);

  /**
   * @brief アニメーションパックリストの取得
   *
   * @return セットされているアニメーションパックリスト
   */
  const gfl2::animation::AnimationPackList* GetAnimationPackList() const;

  /**
   * @brief アニメーション再生用のメモリを明示的に確保する　※この関数を呼ぶとChangeAnimation()内の動的確保は行わなくなります
   *
   * @param pAnimationHeap アニメーションヒープ
   * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void AllocateAnimation(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, u32 animationId, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーション再生用のメモリを明示的に確保する：リソースノード指定版　※この関数を呼ぶとChangeAnimation()内の動的確保は行わなくなります
   *
   * @param pAnimationHeap アニメーションヒープ
   * @param smoothFlag アニメーション補間を使うかどうか、使う場合は補間用のメモリも合わせて確保
   * @param pResourceNode リソースノード
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void AllocateAnimationByResourceNode(gfl2::heap::HeapBase* pAnimationHeap, b32 smoothFlag, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief 現在のアニメーションIDを取得する
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return アニメーションIDを返す、設定されていなければ-1を返す
   */
  s32 GetAnimationId(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief デフォルトのブレンドフレーム数取得
   *
   * @param animationId アニメーションパック内のID
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
   */
  u32 GetAnimationDefaultBlendFrame(u32 animationId, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief ブレンド機能有効無効設定
   *
   * @param enable true:ブレンド機能有効 false:ブレンド機能無効、以降のSmooth系関数は全て補間無しに設定
   */
  void SetAnimationBlendEnable(bool enable);

  /**
   * @brief ブレンド機能有効か？
   *
   * @return true:ブレンド機能有効 false:ブレンド機能無効、以降のSmooth系関数は全て補間無しに設定
   */
  bool IsAnimationBlendEnable() const;

  /**
   * @brief デフォルトのブレンドフレーム数取得：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 現在再生中のアニメーションとのデフォルトのブレンドフレーム数を返す
   */
  u32 GetAnimationDefaultBlendFrameByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief アニメーションの変更：バインドポーズに戻す
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimationBindpose(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションの変更：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimation(u32 animationId, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);
  
  /**
   * @brief 指定アニメーションデータが存在するか取得
   *
   * @param animationId アニメーションパック内のID
   *
   * @return true : ある  false : ない
   */
  virtual b32 IsAnimationExist(u32 animationId) const;

  /**
   * @brief アニメーションの変更：補間あり：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   * @param changeFrame 補間フレーム数
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimationSmooth(u32 animationId, u32 changeFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションの変更：補間率の手動設定版、ビューアー用に用意：パック内のID指定版
   *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
   *
   * @param animationId0 アニメーションパック内のID0
   * @param animationId1 アニメーションパック内のID1
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  void ChangeAnimationSmoothManual(u32 animationId0, u32 animationId1, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief 現在のアニメーションリソースノードを取得
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return アニメーションリソースノードを返す、設定されていなければNULLを返す
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetAnimationResourceNode(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief アニメーションの変更：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimationByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションの変更：補間あり：リソースノード指定版
   *
   * @param pResourceNode リソースノード
   * @param changeFrame 補間フレーム数
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimationSmoothByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, u32 changeFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションの変更：補間率の手動設定版、ビューアー用に用意：リソースノード指定版
   *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
   *
   * @param pResourceNode0 リソースノード0
   * @param pResourceNode1 リソースノード1
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  void ChangeAnimationSmoothManualByResourceNode(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode0, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode1, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーション補間中か？：ChangeAnimationSmoothの終了判定用
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 補間中ならtrue, 補間中でなければfalse
   */
  bool IsAnimationInterp(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
   *
   * @param enable trueの場合はループする、 falseの場合はループしない
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationLoop(b32 enable, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループ再生されるか？
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return trueの場合はループする、 falseの場合はループしない
   */
  b32 IsAnimationLoop(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief フレーム設定
   *
   * @param frame フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationFrame(f32 frame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief フレーム取得
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 現在のフレームを返す
   */
  f32 GetAnimationFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief 再生速度設定
   *
   * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationStepFrame(f32 stepFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief 再生速度取得
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 再生速度を返す
   */
  f32 GetAnimationStepFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief ループの開始フレーム設定
   *
   * @param startFrame 開始フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationStartFrame(f32 startFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループの開始フレーム取得
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return 開始フレームを返す
   */
  f32 GetAnimationStartFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief ループの終了フレーム設定
   *
   * @param endFrame 終了フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationEndFrame(f32 endFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループの終了フレーム取得
   *
   * @param endFrame 終了フレーム
   * @param animationBit アニメーションの種類
   *
   * @return 終了フレームを返す
   */
  f32 GetAnimationEndFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief 最終フレームか？ 順再生の場合はEndFrame, 逆再生の場合はStartFrameに到達するかチェック
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   *
   * @return このフレームで最終フレームに到達するならtrue, 到達しないならfalse
   */
  b32 IsAnimationLastFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL) const;

  /**
   * @brief キーアニメーションのコールバックを設定
   *
   * @param pUserData ユーザーデータ
   * @param pCallback コールバック
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  void SetKeyAnimationCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーションパックに含まれるリソース数を返す
   *
   * @param packIndex パック番号
   *
   * @return リソース数。読み込み完了前なら0を返す
   */
  u32 GetAnimationPackResouceCount( u32 packIndex);

  /**
   * @brief 操作対象を補間スロットに切り替える：ビューアー用
   *
   * @param isTypeInterp 操作対象が補間スロットか？
   */
  void SetAnimationControllTargetInterp(bool isTargetInterp);

  /**
   * @brief 補間用スロットのフレームと再生速度の同期を取る：ビューアー用
   *
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  void SyncAnimationFrameAndStepFrame(u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーション計算有効フラグの設定
   *
   * @param enable true:計算有効, false:計算無効
   */
  virtual void SetAnimationCalculateEnable(bool enable);

  /**
   * @brief アニメーション計算有効フラグの取得
   *
   * @return true:計算有効, false:計算無効
   */
  virtual bool IsAnimationCalculateEnable() const;

  /**
   * @brief フレーム更新有効フラグの設定
   *
   * @param enable true:更新有効, false:更新無効
   */
  virtual void SetAnimationUpdateFrameEnable(bool enable);

  /**
   * @brief フレーム更新有効フラグの取得
   *
   * @return true:更新有効, false:更新無効
   */
  virtual bool IsAnimationUpdateFrameEnable() const;

  //---------------------------------------------------------------------------------
  // ■ 注視処理

  /**
   * @brief 注視処理初期化
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pMotionConfigResource モーションコンフィグリソース
   */
  void InitializeInterest(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource);

  /**
   * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
   */
  virtual void UpdateInterest();

  /**
   * @brief 注視処理終了確認
   *
   * @return true:終了 false:更新中
   */
  bool IsFinishInterest() const;

  /**
   * @brief 注視位置設定
   *
   * @param pPosition 注視位置：NULLなら終了
   */
  void SetInterestPosition(const gfl2::math::Vector3* pPosition);

  /**
   * @brief 注視用のジョイント数取得
   *
   * @return 注目用ジョイント数
   */
  u32 GetInterestJointCount() const;

  /**
   * @brief 注目タイプ設定
   *
   * @param index インデックス
   * @param type 注目タイプ
   */
  void SetInterestType(u32 index, gfl2::renderingengine::InterestType type);

  /**
   * @brief 注目タイプ取得
   *
   * @param index インデックス
   *
   * @return 注目タイプを返す
   */
  gfl2::renderingengine::InterestType GetInterestType(u32 index) const;

  /**
   * @brief 注目速度設定
   *
   * @param index インデックス
   * @param speed 注目速度
   */
  void SetInterestSpeed(u32 index, f32 speed);

  /**
   * @brief 注目速度取得
   *
   * @param index インデックス
   *
   * @return 注目速度を返す
   */
  f32 GetInterestSpeed(u32 index) const;

  /**
   * @brief 角度制限：最小値設定（ラジアン）
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetInterestLimitAngleLow(u32 index, f32 low);

  /**
   * @brief 角度制限：最小値取得（ラジアン）
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値を返す
   */
  f32 GetInterestLimitAngleLow(u32 index) const;

  /**
   * @brief 角度制限：最大値設定（ラジアン）
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetInterestLimitAngleHigh(u32 index, f32 high);

  /**
   * @brief 角度制限：最大値取得（ラジアン）
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値を返す
   */
  f32 GetInterestLimitAngleHigh(u32 index) const;

  /**
   * @brief 角度制限：最小値２設定（ラジアン）
   *
   * @param index インデックス
   * @param low 角度制限：最小値
   */
  void SetInterestLimitAngleLow2(u32 index, f32 low);

  /**
   * @brief 角度制限：最小値２取得（ラジアン）
   *
   * @param index インデックス
   *
   * @return 角度制限：最小値２を返す
   */
  f32 GetInterestLimitAngleLow2(u32 index) const;

  /**
   * @brief 角度制限：最大値２設定（ラジアン）
   *
   * @param index インデックス
   * @param high 角度制限：最大値
   */
  void SetInterestLimitAngleHigh2(u32 index, f32 high);

  /**
   * @brief 角度制限：最大値２取得（ラジアン）
   *
   * @param index インデックス
   *
   * @return 角度制限：最大値２を返す
   */
  f32 GetInterestLimitAngleHigh2(u32 index) const;

  /**
   * @brief 注目解除にかかるフレーム数を設定
   *
   * @param index インデックス
   * @param resetFrameCount 注目解除にかかるフレーム数
   */
  void SetInterestResetFrameCount(u32 index, s32 resetFrameCount);

  /**
   * @brief 注目解除にかかるフレーム数を取得
   *
   * @param index インデックス
   *
   * @return 注目解除にかかるフレーム数を返す
   */
  s32 GetInterestResetFrameCount(u32 index) const;

	/**
	 * 顔向け補正の有効無効設定
	 * @param isEnabled true:補正有効, false:補正無効
	 */
	void SetInterestAdjustAngleEnable(bool isEnabled);

	/**
	 * 顔向け補正の有効無効取得
	 */
	bool IsInterestAdjustAngleEnabled() const;

	/**
	 * 顔向け補正角度を設定
	 * @param tagAdjustAngle　顔向け補正角度（ラジアン）
	 */
	void SetInterestTagAdjustAngle(f32 tagAdjustAngle);

	/**
	 * 顔向け補正角度をリセット
	 */
	void ResetInterestTagAdjustAngle();

	/**
	 * 顔向け補正角度を取得
	 */
	f32 GetInterestTagAdjustAngle() const;

	/**
	 * 顔向け傾きを設定
	 * @param tagDecline　顔向け傾き（ラジアン）
	 */
	void SetInterestTagDecline(f32 tagDecline);

	/**
	 * 顔向け補間用バネ係数を指定
	 * @param adjustSpringVal バネ係数(初期値0.050f)
	 * @param DeclineSpringVal バネ係数(初期値0.150f)
	 */
  void SetInterestSpringVal( f32 adjustSpringVal, f32 declineSpringVal );

	/**
	 * 顔向け傾きを取得
	 */
	f32 GetInterestTagDecline( ) const;

  //---------------------------------------------------------------------------------
  // ■ バネ制御

  /**
   * @brief バネ制御更新
   */
  void UpdateSpring();

public: // システム用の関数

  // モデルインスタンスノード取得
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode()
  {
    return m_pModelInstanceNode;
  }

  // モデルインスタンスノード取得
  const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() const
  {
    return m_pModelInstanceNode;
  }

  // アニメーションコントローラー作成
  void CreateAnimationController();

  // アニメーションコントローラー取得
  gfl2::animation::AnimationController* GetAnimationController()
  {
    return m_pAnimationController;
  }
  // アニメーションコントローラー取得
  const gfl2::animation::AnimationController* GetAnimationController() const
  {
    return m_pAnimationController;
  }

  // 操作用のマテリアル取得
  BaseModelMaterial GetBaseModelMaterial(u32 materialIndex)
  {
    return BaseModelMaterial(m_pModelInstanceNode->GetMaterialInstanceNode(materialIndex));
  }

  // 操作用のマテリアル取得
  const BaseModelMaterial GetBaseModelMaterial(u32 materialIndex) const
  {
    return BaseModelMaterial(m_pModelInstanceNode->GetMaterialInstanceNode(materialIndex));
  }

  /**
   * @brief 操作用のアニメーション
   *
   * @param slotIndex 操作するスロット
   */
  BaseModelAnimation GetBaseModelAnimation(u32 slotIndex, AnimationBit animationBit)
  {
    return BaseModelAnimation(m_pAnimationController, slotIndex, (gfl2::animation::AnimationController::AnimationBit)animationBit);
  }

  /**
   * @brief 操作用のアニメーション
   *
   * @param slotIndex 操作するスロット
   */
  const BaseModelAnimation GetBaseModelAnimation(u32 slotIndex, AnimationBit animationBit) const
  {
    return BaseModelAnimation(const_cast<gfl2::animation::AnimationController*>(m_pAnimationController), slotIndex, (gfl2::animation::AnimationController::AnimationBit)animationBit);
  }

  /**
   * @brief 移動速度取得
   * @param interpWeight 補間率
   * @return math::Vector3 移動値
   */
  gfl2::math::Vector3 GetWalkSpeed(f32 interpWeight) const;

  /**
   * @brief 総ポリゴン数取得
   * @return u32 総ポリゴン数
   */
  u32 GetPolygonCount() const;

  /**
   * @brief 総頂点数取得
   * @return u32 総頂点数
   */
  u32 GetVertexCount() const;

  /**
   * @brief テクスチャ数取得
   * @return u32 テクスチャ数
   */
  u32 GetTextureCount() const;

#if defined(GF_PLATFORM_CTR)
  /**
   * @brief マテリアルの初期値をステートオブジェクトにコピーする
   * @caution ＣＴＲのステートオブジェクトはマテリアルの初期値を引き継いでいません。別途コピーの実行が必要です。
   */
  void CopyMaterialDepthStencilStateToStateObject()
  {
    m_pModelInstanceNode->CopyMaterialDepthStencilStateToStateObject();
  }
#endif

private:

  /**
   * @brief 共通作成処理
   */
  bool CreateCommon(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource);

  /**
   * @brief 作成時に呼ばれるコールバック
   */
  virtual void OnCreate();

  /**
   * @brief BaseModelのフィールド初期化
   */
  void InitializeField_BaseModel();

private:

  // @fix GFNMCat[879]
  // 補間キャンセル時に視線制御などのアニメーション後のポスト処理の結果が、更にポスト処理されてしまう不具合の対処
  // ポスト処理前のSRTを退避させておくことでこの問題を回避する
  /**
   * @brief 視線制御前のSRT保存用構造体
   */
  struct JointSRTPair
  {
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode;   ///< SRTを保存するジョイントノード
    gfl2::math::SRT                                                 localSRT;     ///< 視線制御前のSRT
  };

  /**
   * @brief 保存用のSRT構造体の確保と初期化
   */
  void InitializeJointSRTPair();

  /**
   * @brief SRTの保存
   */
  void StoreJointSRT();

  /**
   * @brief 保存していたSRTの復元
   */
  static void RestoreJointSRT(void* pWork);

private:

  // モデル
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode;

  // 階層構造用
  BaseModel*                                                        m_pParent;
  s32                                                               m_ParentJointIndex;

  // SRT計算用
  gfl2::math::EulerRotation                                         m_EulerRotation;

  // アニメーション用
  gfl2::animation::AnimationController*                             m_pAnimationController;
  gfl2::animation::AnimationPackList*                               m_pAnimationPackList;
  gfl2::heap::HeapBase*                                             m_pAnimationHeap;
  s32                                                               m_animationIdCache[SLOT_CACHE_CNT_FOR_ANIMATION_ID][BIT_CACHE_CNT_FOR_ANIMATION_ID];

  // 視線制御用
  gfl2::animation::InterestController*                              m_pInterestController;

  // バネ制御用
  gfl2::animation::SpringController*                                m_pSpringController;

  // @fix GFNMCat[879]
  JointSRTPair*                                                     m_pStoreJointSRTPairList; ///< 保存用のSRT構造体リスト
};

}}

#endif
