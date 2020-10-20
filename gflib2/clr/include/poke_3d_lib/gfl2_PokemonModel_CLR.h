#ifndef GFLIB2_POKE_3D_LIB_CLR_POKEMONMODEL_H_INCLUDED
#define GFLIB2_POKE_3D_LIB_CLR_POKEMONMODEL_H_INCLUDED

#include <model/include/gfl2_PokemonModel.h>
#include <clr/include/math/gfl2_Vector_CLR.h>
#include <clr/include/math/gfl2_MathAABB_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_JointInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ModelInstanceNode_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Resource/gfl2_ResourceNode_CLR.h>
#include <clr/include/Animation/gfl2_AnimationEnums_CLR.h>
#include <clr/include/RenderingEngine/SceneGraph/Instance/gfl2_ViewerLocatorInstanceNode_CLR.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>

#using <System.dll>
#using <System.Windows.Forms.dll>

using namespace System::Diagnostics;
using namespace System::Collections::Generic;

namespace gfl2 { namespace clr { namespace poke_3d_lib {

/**
 * @brief GFモデルビューアで表示するマテリアル情報
 */
public ref class PokemonModelMaterialAttribute
{
public:
  u32 m_IdEdgeNo;                   //!< @brief IDエッジ番号
  u32 m_LightSetNo;                 //!< @brief ライトセット番号
  System::String^ m_CullModeName;   //!< @brief カリングの設定(None, CW, CCWのいずれか)

  PokemonModelMaterialAttribute()
  {
    m_IdEdgeNo = 0;
    m_LightSetNo = 0;
  }
};

/**
 * @brief ポケモンモデルクラス
 */
public ref class PokemonModel
{
public:

  // キー音再生用の関数群. 初期化, 終了, コールバック用データ設定
  static bool InitializeKeySound(System::String^ soundFilePath);
  static void TerminateKeySound();

  /**
   * @brief アニメーションビット
   */
  // @fixme : gfl2_BaseModel.hから定義をコピペしてるだけ.
  enum class AnimationBit
  {
    ANIMATION_BIT_JOINT = 1 << 0,         ///< ジョイントアニメーション
    ANIMATION_BIT_MATERIAL = 1 << 1,      ///< マテリアルアニメーション
    ANIMATION_BIT_VISIBILITY = 1 << 2,    ///< ビジビリティアニメーション
    ANIMATION_BIT_BLENDSHAPE = 1 << 3,    ///< ブレンドシェイプアニメーション
    ANIMATION_BIT_KEY = 1 << 4,           ///< キーアニメーション

    ANIMATION_BIT_ALL =                   ///< 全アニメーション
      ANIMATION_BIT_JOINT |
      ANIMATION_BIT_MATERIAL |
      ANIMATION_BIT_VISIBILITY |
      ANIMATION_BIT_BLENDSHAPE |
      ANIMATION_BIT_KEY
  };



  /**
   * @brief レアカラー設定用のコンスタントカラー情報
   */
  ref struct RareColorConstantSettingInfo
  {
    System::String^ m_MaterialName;  //!< @brief マテリアル名
    u32 m_ConstantColorIndex;
    f32 m_Red;
    f32 m_Green;
    f32 m_Blue;
    f32 m_Alpha;

    RareColorConstantSettingInfo(System::String^ materialName, u32 index, float r, float g, float b, float a)
    {
      m_MaterialName = materialName;
      m_ConstantColorIndex = index;
      m_Red = r;
      m_Green = g;
      m_Blue = b;
      m_Alpha = a;
    }
  };

  /**
   * @brief ポケモンモデルの設定情報(バイナリデータ入出力用)
   */
  ref struct PokemonModelInfo
  {
    System::String^ m_GfmdlFilePath;
    array<System::String^>^ m_GfmotFilePaths;
    array<System::String^>^ m_GfvshFilePaths;
    array<System::String^>^ m_GfgshFilePaths;
    array<System::String^>^ m_GffshFilePaths;
    array<System::String^>^ m_TextureFilePaths;
    array<INT32>^ m_MotionIndices;
    array<float>^ m_MotionStartFrames;
    array<float>^ m_MotionEndFrames;
    array<UINT32>^ m_MotionInterpFrames;
    array<INT32>^ m_OverrideMotionIndices;
    b32 m_IsColorShaderEnabled;
    gfl2::clr::math::Vector^ m_ColorShaderColor;
    b32 m_IsAmbientColorEnabled;
    gfl2::clr::math::Vector^ m_AmbientColor;
    gfl2::clr::math::Vector^ m_LightVector;
    gfl2::clr::math::Vector^ m_Scale;
    gfl2::clr::math::Vector^ m_Rotation;
    gfl2::clr::math::Vector^ m_Translate;
    gfl2::clr::math::Matrix^ m_CameraMatrix;
    f32 m_FieldOfView;
    f32 m_NearClip;
    f32 m_FarClip;
    b32 m_IsPlayingAnimation;       // アニメーション再生中かどうか
    s32 m_PlayingFrame;           // 再生中フレーム
    array<INT32>^ m_LoopPlayingFrame; // ループモーションの再生フレーム
    s32 m_StopMotionIndex;
    f32 m_StopMotionFrame;
    s32 m_StopPrevMotionIndex;
    f32 m_StopPrevMotionFrame;
    f32 m_StopInterpWeight;
    b32 m_IsBackgroundVisible;
    u8 m_MarkFrame1;
    u8 m_MarkFrame2;
    u8 m_MarkFrame3;
    u8 m_MarkFrame4;

    PokemonModelInfo()
    {
      m_GfmotFilePaths = gcnew array<System::String^>(poke_3d::model::PokemonModel::MOTION_MAX);
      m_GfvshFilePaths = gcnew array<System::String^>(poke_3d::model::PokemonModel::MATERIAL_MAX);
      m_GfgshFilePaths = gcnew array<System::String^>(poke_3d::model::PokemonModel::MATERIAL_MAX);
      m_GffshFilePaths = gcnew array<System::String^>(poke_3d::model::PokemonModel::MATERIAL_MAX);
      m_TextureFilePaths = gcnew array<System::String^>(poke_3d::model::PokemonModel::COLORTEXTURE_MAX);
      m_MotionIndices = gcnew array<INT32>(poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM);
      m_MotionStartFrames = gcnew array<float>(poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM);
      m_MotionEndFrames = gcnew array<float>(poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM);
      m_MotionInterpFrames = gcnew array<UINT32>(poke_3d::model::PokemonModel::AVAILABLE_MOTION_NUM);
      m_OverrideMotionIndices = gcnew array<INT32>((s32)gfl2::animation::OverridePartsType::NumberOf);
      m_LoopPlayingFrame = gcnew array<INT32>(poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM);
    }
  };

  /**
   * @brief リソースファイルからのハッシュ値の生成
   * @param array<string^>^ ファイルパス
   * @return u32 ハッシュ値
   */
  static u32 CreateHashFromResources(array<System::String^>^ filePaths);

  static System::Collections::Generic::Dictionary<System::String^, System::Drawing::Color>^ GetTreeNodeBackColorDictionary()
  {
    return s_TreeNodeBackColorDictionary;
  }
  
  /**
   * @brief TreeNodeの色分け用Dictionaryの作成
   */
  static void CreateTreeNodeColorDictionary();

  /**
   * @brief コンストラクタ
   */
  PokemonModel();

  /**
   * @brief デストラクタ
   */
  ~PokemonModel()
  {
    this->!PokemonModel();
  }

  /**
   * @brief ファイナライザ
   */
  !PokemonModel();

  /**
   * @brief ネイティブポインタ取得
   */
  poke_3d::model::PokemonModel* GetNative()
  {
    return m_pNative;
  }

  /**
   * @brief 作成
   *
   * @param pBaseModelResource ベースモデルリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(
    gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pModelResource,
    gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pMotionConfigNode,
    PokemonModel^ pJointReferenceModel);

  /**
   * @brief 破棄
   */
  void Destroy();

  void SetKeyCallBackInfo();

  //! @brief スケール設定
  void SetScale(gfl2::clr::math::Vector^ scale);

  //! @brief スケール取得
  gfl2::clr::math::Vector^ GetScale();
  
  //! @brief 回転設定
  void SetRotateZYX(gfl2::clr::math::Vector^ rotate);

  //! @brief 回転取得
  gfl2::clr::math::Vector^ GetRotateZYX();

  //! @brief 位置設定
  void SetTranslate(gfl2::clr::math::Vector^ translate);

  //! @brief 位置取得
  gfl2::clr::math::Vector^ GetTranslate();

  /**
   * @brief AABB取得
   */
  clr::math::AABB^ GetModelAABB();

  /**
   * @brief AABB設定
   */
  void SetModelAABB(clr::math::AABB^ aabb);

  /**
   * @brief 高さ取得
   *
   * @return モデルの高さ
   */
  f32 GetCmHeight();

  /**
   * @brief 高さ取得
   *
   * @return モデルの高さ
   */
  f32 GetAdjustHeight();

  /**
   * @brief 高さ設定
   *
   * @param cmHeight モデルの元の高さ
   * @param adjustHeight モデルの補正後の高さ
   */
  void SetHeight(f32 cmHeight, f32 adjustHeight);

  /**
   * @brief スケール値計算
   *
   * @return CmHeightとAdjustHeightから求めたスケール値
   */
  f32 CalculateAdjustScale();

  /**
   * @brief MotionResourceNodeの取得
   *
   * @param index インデックス
   * @return ResourceNode
   */
  clr::renderingengine::scenegraph::resource::ResourceNode^ GetMotionResourceNode(s32 index);

  /**
   * @brief アニメーションの変更
   *
   * @param pResourceNode リソースノード
   * @param slotIndex スロット番号
   */
  void ChangeAnimation(gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pResourceNode, s32 slotIndex, AnimationBit animationBit)
  {
    if (pResourceNode != nullptr)
    {
      m_pNative->ChangeAnimationByResourceNode(pResourceNode->GetNative(), slotIndex, (poke_3d::model::BaseModel::AnimationBit)animationBit);
    }
    else
    {
      m_pNative->ChangeAnimationByResourceNode(NULL, slotIndex, (poke_3d::model::BaseModel::AnimationBit)animationBit);
    }
  }

  /**
   * @brief MotionResourceNodeの設定
   *
   * @param index インデックス
   * @param pResourceNode リソースノード
   */
  void SetMotionResourceNode(s32 index, clr::renderingengine::scenegraph::resource::ResourceNode^ pResourceNode);

  /**
   * @brief アニメーションの変更：補間率の手動設定版
   *        この関数でアニメーションをセットした後UpdateAnimation()の引数に0.0〜1.0の範囲を指定してください
   *
   * @param pResourceNode0 リソースノード0
   * @param pResourceNode1 リソースノード1
   * @param slotIndex スロット番号
   */
  void ChangeAnimationSmoothManual(
    gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pResourceNode0,
    gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pResourceNode1,
    s32 slotIndex,
    AnimationBit animationBit)
  {
    m_pNative->ChangeAnimationSmoothManualByResourceNode(
      (pResourceNode0 == nullptr) ? nullptr : pResourceNode0->GetNative(),
      (pResourceNode1 == nullptr) ? nullptr : pResourceNode1->GetNative(),
      slotIndex,
      (poke_3d::model::BaseModel::AnimationBit)animationBit);
  }

  /**
   * @brief キーアニメーションの更新
   *
   */
  void UpdateKeyAnimation()
  {
    m_pNative->UpdateKeyAnimation();
  }

  /**
   * @brief アニメーションの更新
   *
   * @param interpWeight 補間率を0.0〜1.0の範囲で設定
   */
  void UpdateAnimation(f32 interpWeight)
  {
    m_pNative->UpdateAnimation(interpWeight);
  }

  /**
   * @brief フレーム設定
   *
   * @param frame フレーム
   * @param slotIndex スロット番号
   */
  void SetAnimationFrame(f32 frame, s32 slotIndex, AnimationBit animationBit)
  {
    m_pNative->SetAnimationFrame(frame, slotIndex, (poke_3d::model::BaseModel::AnimationBit)animationBit);
  }

  /**
   * @brief 再生速度設定
   *
   * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
   * @param slotIndex スロット番号
   */
  void SetAnimationStepFrame(f32 stepFrame, s32 slotIndex, AnimationBit animationBit)
  {
    m_pNative->SetAnimationStepFrame(stepFrame, slotIndex, (poke_3d::model::BaseModel::AnimationBit)animationBit);
  }

  /**
   * @brief ループの終了フレーム取得
   *
   * @param endFrame 終了フレーム
   *
   * @return 終了フレームを返す
   */
  f32 GetAnimationEndFrame(s32 slotIndex, AnimationBit animationBit)
  {
    return m_pNative->GetAnimationEndFrame(slotIndex, (poke_3d::model::BaseModel::AnimationBit)animationBit);
  }

  /**
   * @brief モーション保存可能総数の取得
   *
   * @return モーションの保存可能総数
   */
  s32 GetMotionNum();

  /**
   * @brief JointInstanceNodeリストの取得
   *
   * @return JointInstanceNodeリストを返す
   */
  array<clr::renderingengine::scenegraph::instance::JointInstanceNode^>^ GetJointInstanceNodeList();

  /**
   * @brief ロケーターノードをアタッチ
   *
   * @param pLocatorNodeTop ロケーターノード
   */
  void AttachLocatorNode(gfl2::clr::renderingengine::scenegraph::instance::LocatorInstanceNode^ pLocatorNodeTop)
  {
    m_pNative->AttachLocatorNode(pLocatorNodeTop->GetNative());
  }

  /**
   * @brief 現在のテクスチャをカラーシェーダ用テクスチャに差し替え
   * @param enable 有効/無効
   */
  void SetEnabledColorShaderTexture(bool enable);

  /**
   * @brief カラーシェーダテクスチャの有効/無効の取得
   * @return b32 有効/無効
   */
  bool IsEnabledColorShaderTexture();

  /**
   * @brief 環境光の設定の有効/無効の切り替え
   * @param enable 有効/無効
   */
  void SetEnabledAmbientColor(bool enable);

  /**
   * @brief 環境光の設定の有効/無効の取得
   * @return b32 有効/無効
   */
  bool IsEnabledAmbientColor();

  /**
   * @brief レアカラーの定数カラー追加
   * @param info レアカラー変更用のデータ構造体
   */
  void AddRareConstantColor(RareColorConstantSettingInfo^ info);

  /**
   * @brief レアカラー用の定数カラーに変更
   */
  void ChangeToRareConstantColor();

  /**
   * @brief 現在のテクスチャをレアカラーテクスチャに差し替え
   * @param enable 有効/無効
   */
  void ReplaceWithRareColorTexture();

  /**
   * @brief IDテクスチャへの差し替え
   */
  void ReplaceWithIdTexture();

  /**
   * @brief カラーシェーダの色設定
   * @param color 設定する色
   */
  void SetInterpColor(clr::math::Vector^ color);

  /**
   * @brief カラーシェーダの色取得
   * @return clr::math::Vector^ 設定されている色
   */
  clr::math::Vector^ GetInterpColor();

  /**
   * @brief 環境光シェーダの色設定
   * @param color 設定する色
   */
  void SetAmbientColor(clr::math::Vector^ color);

  /**
   * @brief 環境光シェーダの色取得
   * @return clr::math::Vector^ 設定されている色
   */
  clr::math::Vector^ GetAmbientColor();

  /**
   * @brief カラーシェーダ描画パスのインデックス設定
   * @param index 開始インデックス
   */
  void SetColorShaderPassStartIndex(s32 index);

  /**
   * @brief カラーシェーダ描画パスのインデックス取得
   * @return 開始インデックス
   */
  s32 GetColorShaderPassStartIndex();

  /**
   * @brief カラーシェーダテクスチャの総数取得
   * @return カラーシェーダテクスチャの総数
   */
  s32 GetColorTextureNum();

  /**
   * @brief カラーシェーダテクスチャのインデックスを詰める
   * @param indexNum 詰めるインデックス数
   */
  void PackColorShaderTextureIndex(s32 indexNum);

  u32 GetMaterialCount() { return m_pNative->GetMaterialCount(); }
  u32 GetJointCount() { return m_pNative->GetJointCount(); }
  u32 GetPolygonCount() { return m_pNative->GetPolygonCount(); }
  void SetMaterialLightSetIndex(s32 materialIndex, s16 lightSetIndex) { m_pNative->SetMaterialLightSetIndex(materialIndex, lightSetIndex); }
  void SetVisible(bool flag) { m_pNative->SetVisible(flag); }

  /**
   * @brief IDテクスチャの生成
   * @param texturePath IDテクスチャのパス
   */
  void CreateIdTexture(System::String^ texturePath);

  /**
   * @brief レアカラーテクスチャの生成
   * @param texturePath レアカラーテクスチャのパス
   */
  void CreateRareColorTexture(System::String^ texturePath);

  /**
   * @brief サウンドフラグ設定
   * @param index インデックス
   * @param enabled 有効/無効
   */
  void SetEnabledPlaySoundFlag(u32 index, bool enabled);

  /**
   * @brief サウンドキー一致フラグ取得
   * @param index インデックス
   * @return b32 true/false
   */
  bool GetKeySoundMatchFlag(u32 index);

  /**
   * @brief サウンドキー一致フラグクリア
   */
  void ClearKeySoundMatchFlags();

  /**
   * @brief サウンドフラグ取得
   * @param index インデックス
   * @return b32 true/false
   */
  bool GetPlaySoundFlag(u32 index);

  /**
   * @brief ループアニメーションフラグ取得
   * @param index インデックス
   * @return b32 true/false
   */
  bool GetLoopAnimationFlag(u32 index);

  /**
   * @brief ループアニメーションフラグのリセット
   * @param index インデックス
   */
  void ResetLoopAnimationFlag(u32 index);

  /**
   * @brief サウンドフラグ取得
   * @return b32* サウンドバッファ
   */
  b32* GetPlaySoundFlagBuffer();

  /**
   * @brief 目, 口, ループモーションのモーションインデックスの設定
   * @param partsType パーツの種類
   * @param slotIndex 設定するスロットのインデックス
   * @param motionResourceIndex 表情用のモーションのリソースノードのインデックス(SetMotionResourceNode()で設定したインデックス)
   */
  void SetOverridePartsMotionIndex(s32 partsType, u32 slotIndex, s32 motionResourceIndex);

  /**
   * @brief 目, 口, ループモーションのモーションインデックスの取得
   * @param partsType パーツの種類
   * @return s32 設定されているインデックス
   */
  s32 GetOverridePartsMotionIndex(s32 type);

  /**
   * @brief info.dat出力
   * @param 出力先ディレクトリ
   */
  void OutputInfoDat(
    System::String^ outputDirectory,
    gfl2::clr::poke_3d_lib::PokemonModel::PokemonModelInfo^ info);

  /**
   * @brief 操作対象を補間スロットに切り替える：ビューアー用
   *
   * @param isTypeInterp 操作対象が補間スロットか？
   */
  void SetAnimationControllTargetInterp(bool isTargetInterp);

  /**
   * @brief 歩き用速度を取得：ポケモン用の拡張データ
   *
   * @param prevFrame このフレームから
   * @param nextFrame このフレームまでの変化量を取得
   * @param interpWeight 補間率
   *
   * @return 移動速度ベクトル
   */
  gfl2::clr::math::Vector^ GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight);

  /**
   * @brief ライトセット番号の設定
   * @param lightSetNo ライトセット番号
   */
  void SetLightSetNo(u32 lightSetNo);

  /**
   * @brief ライトの方向設定
   * @brief direction 方向ベクトル
   */
  void SetLightDirection(gfl2::clr::math::Vector^ direction);

  /**
   * @brief ライトの方向取得
   * @brief direction 方向ベクトル
   */
  gfl2::clr::math::Vector^ GetLightDirection();

  /**
   * @brief gfblocファイルからEfffectNodeを生成
   * @param gfblocFilePath ファイルパス
   * @return <gfl2::clr::renderingengine::scenegraph::instance::LocatorInstanceNode^>^ エフェクトロケータのノードの配列
   */
  array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ LoadEffectLocatorInstanceNodes(System::String^ gfblocFilePath);

  /**
   * @brief gfblocファイルからEfffectNodeを生成
   * @param gfblocFilePath ファイルパス
   * @return <gfl2::clr::renderingengine::scenegraph::instance::LocatorInstanceNode^>^ エフェクトロケータのノードの配列
   */
  array<gfl2::clr::renderingengine::scenegraph::instance::ViewerLocatorInstanceNode^>^ LoadPokedecoLocatorInstanceNodes(System::String^ gfblocFilePath);

  /**
   * @brief レアカラーテクスチャの作成
   */
  void CreateColorShaderTextures();

  /**
   * @brief ノード名を集めたTreeNodeの作成
   * @return System::Windows::Forms::TreeNode^ 作成したツリーノード
   */
  System::Windows::Forms::TreeNode^ CreateModelNameTreeNode();

  /**
   * @brief ツリーノードの取得
   * return System::Windows::Forms::TreeNode^ 作成したツリーノード
   */
  System::Windows::Forms::TreeNode^ GetModelNameTreeNode();

  /**
   * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
   */
  void UpdateInterest();

  /**
   * @brief 注視位置設定
   *
   * @param pPosition 注視位置：NULLなら終了
   */
  void SetInterestPosition(gfl2::clr::math::Vector^ pPosition);

  /**
   * @brief 注目タイプ設定
   *
   * @param index インデックス
   * @param type 注目タイプ
   */
  void SetInterestType(u32 index, gfl2::clr::renderingengine::InterestType::Enum type);

  /**
   * @brief 注目タイプ取得
   *
   * @param index インデックス
   *
   * @return 注目タイプを返す
   */
  gfl2::renderingengine::InterestType GetInterestType(u32 index);

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
  f32 GetInterestSpeed(u32 index);

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
  f32 GetInterestLimitAngleLow(u32 index);

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
  f32 GetInterestLimitAngleHigh(u32 index);

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
  f32 GetInterestLimitAngleLow2(u32 index);

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
  f32 GetInterestLimitAngleHigh2(u32 index);

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
  s32 GetInterestResetFrameCount(u32 index);

  void UpdateSpring();
  void ResetMotionConfig(gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^ pMotionConfigNode);

  /**
   * @brief PokemonModelMaterialAttributeを生成
   *
   * @return モデルが持っている全てのマテリアルの情報をマテリアル名をキーとした連想配列で返す
   */
  Dictionary<System::String^, PokemonModelMaterialAttribute^>^ CreatePokemonModelMaterialAttributes();

  /**
   * @brief 模様を固定するフレーム数の指定
   * @param u8 markingFrame1 Loop01の固定フレーム
   * @param u8 markingFrame2 Loop02の固定フレーム
   * @param u8 markingFrame3 Loop03の固定フレーム
   * @param u8 markingFrame4 Loop04の固定フレーム
   */
  void SetMarkingFrames(u8 markingFrame1, u8 markingFrame2, u8 markingFrame3, u8 markingFrame4);

  /**
   * @brief animationBitで指定したアニメーションがループモーションに含まれているかどうかの確認
   * @param loopIndex ループモーションのインデックス(0～3)
   * @return true/false ジョイントアニメーションを持っている/持っていない
   */
  bool HasJointAnimationInLoopMotion(u32 loopIndex);

  System::String^ GetMaterialName(s32 materialIndex);
  s32 GetMaterialIndex(System::String^ materialName);
  gfl2::clr::math::Vector^ GetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 slotNo);
  void SetMaterialTextureCoordinatorScaleUV(s32 materialIndex, s32 slotNo, gfl2::clr::math::Vector^ scale);
  f32 GetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 slotNo);
  void SetMaterialTextureCoordinatorRotateUV(s32 materialIndex, s32 slotNo, f32 rotate);
  gfl2::clr::math::Vector^ GetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 slotNo);
  void SetMaterialTextureCoordinatorTranslateUV(s32 materialIndex, s32 slotNo, gfl2::clr::math::Vector^ translate);

private:
  static System::Collections::Generic::Dictionary<System::String^, System::Drawing::Color>^ s_TreeNodeBackColorDictionary;
  System::Windows::Forms::TreeNode^ PokemonModel::FindTreeNodeByName(
    System::Windows::Forms::TreeNode^ treeNode,
    System::String^ name);

  //!< @brief ツリービューで表示するTreeNode
  System::Windows::Forms::TreeNode^ m_RootTreeNode;

  poke_3d::model::PokemonModel* m_pNative;

  // IDテクスチャ
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode** m_IdTextureResourceNodes;
  u32 m_IdTextureNum;

  // レアカラーテクスチャ
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode** m_RareColorTextureResourceNodes;
  u32 m_RareColorTextureNum;

  //!< @brief エフェクト用ロケータリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEffectLocatorResourceNode;
  //!< @brief gfblocファイルの読み取ったバイナリデータ
  c8* m_EffectGfblocRowBinaryData;

  //!< @brief ポケデコ用ロケータリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pPokedecoLocatorResourceNode;
  //!< @brief gfblocファイルの読み取ったバイナリデータ
  c8* m_PokedecoGfblocRowBinaryData;

  // ビューア上でのレンダリングパスの開始インデックス
  s32 m_ColorShaderPassStartIndex;

  // C#側でガベージコレクタで破棄されないように保持
  array<gfl2::clr::renderingengine::scenegraph::resource::ResourceNode^>^ m_pManagedMotionResourceRootNodes;

  //! 注視用の位置
  gfl2::math::Vector3* m_pInterestPosition;
};

}}}

#endif