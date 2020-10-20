#if !defined(POKE_3D_LIB_MODEL_POKEMONMODEL_H_INCLUDED)
#define POKE_3D_LIB_MODEL_POKEMONMODEL_H_INCLUDED
#pragma once

#include <model/include/gfl2_BaseModel.h>
#include <gfx/include/gfl2_ColorU8.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <math/include/gfl2_MathAABB.h>
#include <Animation/include/gfl2_AnimationEnums.h>
#include <Animation/include/gfl2_SpringController.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CombinerAttribute.h>

#if defined(GF_PLATFORM_WIN32)
// PC環境でしか使用しないのでDirectSound関連の記述は問題ない.
// ただし, このファイルの中だけに収めること.
#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment(lib, "winmm.lib")
#include <dsound.h>
#include <mmsystem.h>
#endif

#define POKEMONMODEL_PATCHEEL_EXCEPTION

namespace poke_3d { namespace model {

class PokemonModel : public BaseModel
{
public:

  class KeySoundAttribute
  {
  public:
    enum 
    {
      AK_EffectStart01,
      AK_ButuriStart01,
      AK_SEStart01,
      AK_SEStart02,
      AK_SEStart03,
      AK_SEStart04,
      AK_PartsSkel01,
      AK_PartsMaterial01,
      AK_PartsVisible01,
      AK_PartsShader01,
      NumberOf
    };
  };

  struct KeyCallBackInfo
  {
    poke_3d::model::PokemonModel* m_pPokemonModel;
    b32 m_KeySoundMatchFlags[KeySoundAttribute::NumberOf];
    b32 m_KeySoundAttributes[KeySoundAttribute::NumberOf];
    b32 m_LoopAnimationFlags[KeySoundAttribute::AK_PartsShader01 - KeySoundAttribute::AK_PartsSkel01 + 1];
  };

  /**
   * @brief キー音再生用クラス
   */
  class KeyAnimationSlotFunc
  {
  public:
    static const c8* s_pKeyAttributeNames[];
#if defined(GF_PLATFORM_WIN32)
    static bool InitializeKeySound(LPWSTR soundFilePath);
    static void TerminateKeySound();
#endif
    static void KeyCallback(const c8* pKeyName, f32 value, void* pUserData);
  
  private:
#if defined(GF_PLATFORM_WIN32)
    static bool CreateSoundBuffer(LPDIRECTSOUNDBUFFER* dsb, LPWSTR file);
    static LPDIRECTSOUND8 lpDS;
    static LPDIRECTSOUNDBUFFER lpPrimary;
    static LPDIRECTSOUNDBUFFER lpSecondary;
#endif
  };

  //<1 @brief cmataを調査したところ最大数が42. 余裕を見ておくのと, アラインメントを揃える意味で64に設定
  static const s32 MOTION_MAX = 64;
  //<! @brief ポケモン一体あたりの最大マテリアル数16
  static const s32 MATERIAL_MAX = 16;
  //<! @brief カラーテクスチャの枚数は1マテリアルにつき3つまで
  static const s32 COLORTEXTURE_MAX = MATERIAL_MAX * gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureMax;
  //!< @brief モーションの補間可能総数
  static const s32 AVAILABLE_MOTION_NUM = 10;

  //!< @brief アニメーションの種類の総数(Joint, Material, Visibilityの3種類)
  static const s32 LOOP_ANIMATION_BIT_PATTERN_NUM = 3;
  //!< @brief ループモーションの種類
  static const BaseModel::AnimationBit s_LoopAnimationBit[LOOP_ANIMATION_BIT_PATTERN_NUM];

  /**
   * @brief オフセット用の共用体
   */
  union RareColorConstantOriginalUnion
  {
    struct
    {
      u32 value;     //!< @brief レアカラーと符号ビットを有した値
    };
    struct
    {
      u32 red : 8;            //!< @brief 赤成分
      u32 green : 8;          //!< @brief 緑成分
      u32 blue : 8;           //!< @brief 青成分
      u32 materialIndex : 8;  //!< @brief コンスタントカラーのインデックス
    };
  };

  class RareColorConstantOriginal
  {
  public:
    RareColorConstantOriginal()
    {
      m_RareColorConstantOriginalUnion.value = 0;
      // マテリアル数は16が最大なので, 絶対に届かないインデックス値に設定しておく
      m_RareColorConstantOriginalUnion.materialIndex = 0xFF;
    }

    RareColorConstantOriginal(const gfl2::gfx::ColorU8& color, u32 materialIndex)
    {
      m_RareColorConstantOriginalUnion.red = color.r;
      m_RareColorConstantOriginalUnion.green = color.g;
      m_RareColorConstantOriginalUnion.blue = color.b;
      m_RareColorConstantOriginalUnion.materialIndex = static_cast<u8>(materialIndex);
    }

    gfl2::gfx::ColorU8 GetColor()
    {
      gfl2::gfx::ColorU8 color;
      color.r = m_RareColorConstantOriginalUnion.red;
      color.g = m_RareColorConstantOriginalUnion.green;
      color.b = m_RareColorConstantOriginalUnion.blue;
      color.a = 255;
      return color;
    }

    b32 IsAvailableMaterialIndex() const
    {
      return m_RareColorConstantOriginalUnion.materialIndex < static_cast<u8>(MATERIAL_MAX);
    }

    u32 GetMaterialIndex() const
    {
      return m_RareColorConstantOriginalUnion.materialIndex;
    }

  private:
    RareColorConstantOriginalUnion m_RareColorConstantOriginalUnion;
  };

  /**
   * @brief オフセット用の共用体
   */
  union RareColorConstantOffsetUnion
  {
    struct
    {
      u32 value;     //!< @brief レアカラーと符号ビットを有した値
    };
    struct
    {
      u32 red : 8;                  //!< @brief 赤成分
      u32 green : 8;                //!< @brief 緑成分
      u32 blue : 8;                 //!< @brief 青成分
      u32 redSign : 1;              //!< @brief 赤成分の正負の符号
      u32 greenSign : 1;            //!< @brief 緑成分の正負の符号
      u32 blueSign : 1;             //!< @brief 青成分の正負の符号
      u32 constantColorIndex : 5;   //!< @brief コンスタントカラーのインデックス
    };
  };

  class RareColorConstantOffset
  {
  public:
    RareColorConstantOffset()
    {
      m_RareColorConstantOffsetUnion.value = 0;
      // コンスタントカラーは6が最大なので, 絶対に届かないインデックス値に設定しておく
      m_RareColorConstantOffsetUnion.constantColorIndex = 0x1F;
    }

    RareColorConstantOffset(const gfl2::math::Vector4& color, u32 constantColorIndex)
    {
      m_RareColorConstantOffsetUnion.redSign = color.x < 0;   // +なら0, -なら1
      f32 red = gfl2::math::Abs(color.x);
      m_RareColorConstantOffsetUnion.red = static_cast<u8>(red * 255.0f);
      m_RareColorConstantOffsetUnion.greenSign = color.y < 0;   // +なら0, -なら1
      f32 green = gfl2::math::Abs(color.y);
      m_RareColorConstantOffsetUnion.green = static_cast<u8>(green * 255.0f);
      m_RareColorConstantOffsetUnion.blueSign = color.z < 0;   // +なら0, -なら1
      f32 blue = gfl2::math::Abs(color.z);
      m_RareColorConstantOffsetUnion.blue = static_cast<u8>(blue * 255.0f);
      m_RareColorConstantOffsetUnion.constantColorIndex = static_cast<u8>(constantColorIndex);
    }

    gfl2::gfx::ColorU8 AddColorOffset(const gfl2::gfx::ColorU8& sourceColor)
    {
      s32 destinationR = sourceColor.r;
      s32 destinationG = sourceColor.g;
      s32 destinationB = sourceColor.b;
      s32 destinationA = sourceColor.a;

      if (m_RareColorConstantOffsetUnion.redSign)
      {
        destinationR -= static_cast<s32>(m_RareColorConstantOffsetUnion.red);
      }
      else
      {
        destinationR += static_cast<s32>(m_RareColorConstantOffsetUnion.red);
      }

      if (m_RareColorConstantOffsetUnion.greenSign)
      {
        destinationG -= static_cast<s32>(m_RareColorConstantOffsetUnion.green);
      }
      else
      {
        destinationG += static_cast<s32>(m_RareColorConstantOffsetUnion.green);
      }

      if (m_RareColorConstantOffsetUnion.blueSign)
      {
        destinationB -= static_cast<s32>(m_RareColorConstantOffsetUnion.blue);
      }
      else
      {
        destinationB += static_cast<s32>(m_RareColorConstantOffsetUnion.blue);
      }

      gfl2::gfx::ColorU8 destinationColor;

      destinationColor.r = static_cast<u8>(gfl2::math::Min(gfl2::math::Max(0, destinationR), 255));
      destinationColor.g = static_cast<u8>(gfl2::math::Min(gfl2::math::Max(0, destinationG), 255));
      destinationColor.b = static_cast<u8>(gfl2::math::Min(gfl2::math::Max(0, destinationB), 255));
      destinationColor.a = static_cast<u8>(gfl2::math::Min(gfl2::math::Max(0, destinationA), 255));

      return destinationColor;
    }

    gfl2::math::Vector4 GetColorOffset()
    {
      gfl2::math::Vector4 colorOffset;
      colorOffset.x = static_cast<f32>(m_RareColorConstantOffsetUnion.red) / 255.0f;
      if (m_RareColorConstantOffsetUnion.redSign)
      {
        colorOffset.x = -colorOffset.x;
      }

      colorOffset.y = static_cast<f32>(m_RareColorConstantOffsetUnion.green) / 255.0f;
      if (m_RareColorConstantOffsetUnion.greenSign)
      {
        colorOffset.y = -colorOffset.y;
      }

      colorOffset.z = static_cast<f32>(m_RareColorConstantOffsetUnion.blue) / 255.0f;
      if (m_RareColorConstantOffsetUnion.blueSign)
      {
        colorOffset.z = -colorOffset.z;
      }

      colorOffset.w = 0.0f;

      return colorOffset;
    }

    b32 IsAvailableConstantColorIndex() const
    {
      return m_RareColorConstantOffsetUnion.constantColorIndex < static_cast<u8>(gfl2::renderingengine::scenegraph::resource::CombinerAttribute::CombinerMax);
    }

    u32 GetConstantColorIndex() const
    {
      return m_RareColorConstantOffsetUnion.constantColorIndex;
    }

  private:
    RareColorConstantOffsetUnion m_RareColorConstantOffsetUnion;
  };

  /**
   * @brief レアカラー設定用のコンスタントカラー情報
   */
  struct RareColorConstantSettingInfo
  {
    RareColorConstantOriginal m_OriginalColor;
    RareColorConstantOffset  m_ColorOffset;
  };

  /**
   * @brief ポケモンモデルの設定情報(バイナリデータ入出力用)
   */
  struct PokemonModelInfo
  {
    c8 m_GfmdlFilePath[256];
    c8 m_GfmotFilePaths[MOTION_MAX][256];
    c8 m_GfvshFilePaths[MATERIAL_MAX][256];
    c8 m_GfgshFilePaths[MATERIAL_MAX][256];
    c8 m_GffshFilePaths[MATERIAL_MAX][256];
    c8 m_TextureFilePaths[COLORTEXTURE_MAX][256];
    RareColorConstantSettingInfo m_RareColorConstantSettingInfos[MATERIAL_MAX * gfl2::renderingengine::scenegraph::resource::CombinerAttribute::ConstantMax];
    s32 m_GfmotFileNum;
    s32 m_GfvshFileNum;
    s32 m_GfgshFileNum;
    s32 m_GffshFileNum;
    s32 m_TextureFileNum;
    s32 m_RareColorConstantSettingInfoNum;
    s32 m_MotionIndices[AVAILABLE_MOTION_NUM];
    f32 m_MotionStartFrames[AVAILABLE_MOTION_NUM];
    f32 m_MotionEndFrames[AVAILABLE_MOTION_NUM];
    u32 m_MotionInterpFrames[AVAILABLE_MOTION_NUM];
    s32 m_OverrideMotionIndices[gfl2::animation::OverridePartsType::NumberOf];
    b32 m_IsColorShaderEnabled;
    gfl2::math::Vector4 m_ColorShaderColor;
    b32 m_IsAmbientColorEnabled;
    gfl2::math::Vector4 m_AmbientColor;
    gfl2::math::Vector4 m_LightVector;
    gfl2::math::Vector4 m_Scale;
    gfl2::math::Vector4 m_Rotation;
    gfl2::math::Vector4 m_Translate;
    gfl2::math::Matrix44 m_CameraMatrix;
    f32 m_FieldOfView;
    f32 m_NearClip;
    f32 m_FarClip;
    b32 m_IsPlayingAnimation;       // アニメーション再生中かどうか
    s32 m_PlayingFrame;           // 再生中フレーム
    s32 m_LoopPlayingFrame[LOOP_ANIMATION_BIT_PATTERN_NUM];
    s32 m_StopMotionIndex;
    f32 m_StopMotionFrame;
    s32 m_StopPrevMotionIndex;
    f32 m_StopPrevMotionFrame;
    f32 m_StopInterpWeight;
    b32 m_IsBackgroundVisible;
    // 模様固定用のフレーム(2016.01.26現在パッチールのみ)
    u8 m_MarkFrame1;
    u8 m_MarkFrame2;
    u8 m_MarkFrame3;
    u8 m_MarkFrame4;
  };

  /**
   * @brief バネ制御位置
   */
  enum SpringSide
  {
    SPRING_SIDE_L = gfl2::animation::SpringController::SIDE_L,     // 左
    SPRING_SIDE_R = gfl2::animation::SpringController::SIDE_R,     // 右

    SPRING_SIDE_COUNT = gfl2::animation::SpringController::SIDE_COUNT  // 総数
  };

  PokemonModel();
  virtual ~PokemonModel();

  void SetKeyCallBackInfo();
  /**
   * @brief サウンドフラグ設定
   * @param index インデックス
   * @param enabled 有効/無効
   */
  void SetEnabledPlaySoundFlag(u32 index, b32 enabled);

  /**
   * @brief サウンドキー一致フラグ取得
   * @param index インデックス
   * @return b32 true/false
   */
  b32 GetKeySoundMatchFlag(u32 index);

  /**
   * @brief サウンドキー一致フラグクリア
   */
  void ClearKeySoundMatchFlags();

  /**
   * @brief サウンドフラグ取得
   * @param index インデックス
   * @return b32 true/false
   */
  b32 GetPlaySoundFlag(u32 index);

  /**
   * @brief 指定したインデックスのコールバックのフラグを取得
   * @param akPartsIndex 0 : AK_PartsSkel01, 1 : AK_PartsMaterial01, 2 : AK_PartsVisible01
   * @return true / false
   */
  b32 GetLoopAnimationFlag(u32 akPartsIndex);

  /**
   * @brief ループアニメーションフラグをリセット
   * @param akPartsIndex 0 : AK_PartsSkel01, 1 : AK_PartsMaterial01, 2 : AK_PartsVisible01
   */
  void ResetLoopAnimationFlag(u32 akPartsIndex);

  /**
   * @brief サウンドフラグ取得
   * @return b32* サウンドバッファ
   */
  b32* GetPlaySoundFlagBuffer();

  /**
   * @brief 作成
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
   * @brief 作成
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
  void Destroy();

  inline gfl2::math::AABB GetModelAABB() const { return m_AABB; }
  inline void SetModelAABB(const gfl2::math::AABB& aabb) { m_AABB = aabb; }
  inline f32 GetCmHeight() const { return m_CmHeight; }
  inline f32 GetAdjustHeight() const { return m_AdjustHeight; }
  void SetHeight(const f32 cmHeight, const f32 adjustHeight);
  f32 CalculateAdjustScale();
  void CreateColorShaderTextures(gfl2::gfx::IGLAllocator* pGfxAllocator);

  const s32 GetMotionNum() const { return m_MotionNum; }
  
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionResourceNode(s32 index) const;
  void SetMotionResourceNode(s32 index, gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode);

  /**
   * @brief カラーシェーダテクスチャの有効/無効の切り替え
   * @param enable 有効/無効
   */
  void SetEnabledColorShaderTexture(b32 enable);

  /**
   * @brief カラーシェーダテクスチャの有効/無効の取得
   * @return b32 有効/無効
   */
  b32 IsEnabledColorShaderTexture();

  /**
   * @brief 環境光の設定の有効/無効の切り替え
   * @param enable 有効/無効
   */
  void SetEnabledAmbientColor(b32 enable);

  /**
   * @brief 環境光の設定の有効/無効の取得
   * @return b32 有効/無効
   */
  b32 IsEnabledAmbientColor();

  /**
   * @brief レアカラーの定数カラー追加
   * @param info レアカラー変更用のデータ構造体
   */
  void AddRareConstantColor(
    s32 materialNo,
    s32 constantColorIndex,
    gfl2::math::Vector4 colorOffset);

  /**
   * @brief レアカラー用の定数カラーに変更
   */
  void ChangeToRareConstantColor();
  void ResetToRareConstantColor();

  /**
   * @brief マテリアルがカラーシェーダのものを含んでいるか判定
   * @param gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* マテリアルインスタンスノード
   * @return true/false
   */
  bool ContainsColorShaderTextureInMaterial(const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode);

  /**
   * @brief マテリアル内のテクスチャがカラーシェーダの対象かを判定
   * @param gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* マテリアルインスタンスノード
   * @param gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode テクスチャの種類
   * @return true/false
   */
  bool IsColorShaderTextureInMaterial(
    const gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialInstanceNode,
    gfl2::renderingengine::scenegraph::resource::CombinerAttribute::TextureNode textureNodeType);

  void SetInterpColor(const gfl2::math::Vector4& color);
  gfl2::math::Vector4 GetInterpColor();
  void SetAmbientColor(const gfl2::math::Vector4& color);
  gfl2::math::Vector4 GetAmbientColor();

  void SetOriginalColorTextureResourceNode(u32 no, gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pNode)
  {
    m_OriginalColorTextureResourceNodes[no] = pNode;
  }

  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetOriginalColorTextureResourceNode(u32 no)
  {
    return m_OriginalColorTextureResourceNodes[no];
  }

  void SetColorShaderTextureResourceNode(u32 no, gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pNode)
  {
    m_ColorShaderTextureResourceNodes[no] = pNode;
  }

  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetColorShaderTextureResourceNode(u32 no)
  {
    return m_ColorShaderTextureResourceNodes[no];
  }

  u32 GetColorTextureNum()
  {
    return m_ColorTextureNum;
  }

  void SetIdModel(const PokemonModel* pIdModel)
  {
    m_pIdModel = pIdModel;
  }

  void SetShadowModel(const PokemonModel* pShadowModel)
  {
    m_pShadowModel = pShadowModel;
  }

  /**
   * @brief 目, 口, ループモーションのモーションインデックスの設定
   * @param partsType パーツの種類
   * @param slotIndex 設定するスロットのインデックス
   * @param motionResourceIndex 表情用のモーションのリソースノードのインデックス(SetMotionResourceNode()で設定したインデックス)
   */
  void SetOverridePartsMotionIndex(gfl2::animation::OverridePartsType partsType, u32 slotIndex, s32 motionResourceIndex)
  {
    m_OverridePartsMotionIndices[partsType] = motionResourceIndex;
    m_OverridePartsMotionSlots[partsType] = slotIndex;
  }

  /**
   * @brief ループアニメーションの設定
   * @param index ループのインデックス(0〜3)
   * @param enabled 有効/無効
   * @param animationBit 設定するアニメーションのビット
   */
  void SetUpLoopMotion(s32 index, b32 enabled, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループアニメーションのフレーム設定
   * @param index ループのインデックス(0〜3)
   * @param animationBit 設定するアニメーションのビット
   */
  void SetLoopMotionFrame(s32 index, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief キーアニメーションの更新
   */
  void UpdateKeyAnimation();

  /**
   * @brief アニメーションの更新
   *
   * @param interpWeight 補間率を指定したい場合は0.0〜1.0の範囲で設定。デフォルトは自動で補間率を更新
   *
   * @return f32 補間率
   */
  f32 UpdateAnimation(f32 interpWeight = -1.0f);

  /**
   * @brief 目, 口, ループモーションのモーションインデックスの取得
   * @param partsType パーツの種類
   * @return s32 設定されているインデックス
   */
  s32 GetOverridePartsMotionIndex(gfl2::animation::OverridePartsType type)
  {
    return m_OverridePartsMotionIndices[type];
  }

  /**
   * @brief 歩き用速度を取得：ビューアー用：ポケモン用の拡張データ
   *
   * @param stepFrame 再生速度
   * @param prevFrame 補間用：アニメーション０のフレーム
   * @param nextFrame 補間用：アニメーション１のフレーム
   * @param interpWeight 補間率
   *
   * @return 移動速度ベクトル
   */
  gfl2::math::Vector3 GetWalkSpeedManual(f32 stepFrame, f32 prevFrame, f32 nextFrame, f32 interpWeight = -1.0f) const;

  /**
   * @brief RareColorConstantSettingInfo取得
   * @param index インデックス
   * @return RareColorConstantSettingInfo レアカラー用定数情報
   */
  RareColorConstantSettingInfo GetRareColorConstantSettingInfo(s32 index)
  {
    return m_RareColorConstantSettingInfos[index];
  }

  /**
   * @brief レアカラー定数の総数取得
   * @return s32 総数
   */
  s32 GetRareColorConstantSettingInfoNum()
  {
    return m_RareColorConstantSettingInfoNum;
  }

  /**
   * @brief ライトセット番号の設定
   * @param lightSetNo ライトセット番号
   */
  void SetLightSetNo(u32 lightSetNo);

  /**
   * @brief ライトの方向設定
   * @brief direction 方向ベクトル
   */
  void SetLightDirection(const gfl2::math::Vector3& direction);

  /**
   * @brief ライトの方向取得
   * @brief direction 方向ベクトル
   */
  gfl2::math::Vector3 GetLightDirection();

  /** 
   * @brief DrawEnvNode取得
   */
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode() const
  {
    return m_pDrawEnvNode;
  }

  /**
   * @brief エフェクトロケータインスタンスノード取得
   * @return gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode*
   */
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* GetEffectLocatorInstanceNodeTop()
  {
    return m_pEffectLocatorInstanceNodeTop;
  }

  /**
   * @brief ポケデコロケータインスタンスノード取得
   * @return gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode*
   */
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* GetPokedecoLocatorInstanceNodeTop()
  {
    return m_pPokedecoLocatorInstanceNodeTop;
  }

  /**
   * @brief エフェクト用ロケータインスタンスノードをリソースノードから設定
   * @param gfl2::gfx::IGLAllocator* pGfxAllocator LocatorInstanceNodeを生成するためのアロケータ
   * @param pLocatorResourceNode ロケータインスタンスノード
   */
  void AttachEffectLocatorInstanceNodeTopWithLocatorResourceNode(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pLocatorResourceNode);

  /**
   * @brief ポケデコ用ロケータインスタンスノードをリソースノードから設定
   * @param gfl2::gfx::IGLAllocator* pGfxAllocator LocatorInstanceNodeを生成するためのアロケータ
   * @param pLocatorResourceNode ロケータインスタンスノード
   */
  void AttachPokedecoLocatorInstanceNodeTopWithLocatorResourceNode(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pLocatorResourceNode);

  /**
   * @brief 移動速度取得
   * @param interpWeight 補間率
   * @return math::Vector3 移動値
   */
  gfl2::math::Vector3 GetWalkSpeed(f32 interpWeight);

  /**
   * @brief 同名のテクスチャと差し替え
   * @param pIdTextureResourceNode 差し替えるテクスチャリソースノード
   */
  void ReplaceWithIdTextureResourceNode(gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureResourceNode);

  /**
   * @brief バネ初期化
   *
   * @param pAnimationHeap アニメーション用ヒープ
   */
  void InitializeSpring(gfl2::heap::HeapBase* pAnimationHeap);

  /**
   * @brief バネ更新
   */
  void UpdateSpring();

  /**
   * @brief バネ制御有効無効を設定
   *
   * @param enable true:バネ制御有効 false:バネ制御無効
   */
  void SetSpringEnable(bool enable);

  /**
   * @brief バネ制御有効無効を取得
   *
   * @return true:バネ制御有効 false:バネ制御無効
   */
  bool IsSpringEnable() const;

  /**
   * @brief バネ減衰設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param damping 減衰
   */
  void SetSpringDamping(SpringSide lr, f32 damping);

  /**
   * @brief バネ減衰取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return 減衰を返す
   */
  f32 GetSpringDamping(SpringSide lr) const;

  /**
   * @brief バネ定数設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param stiffness バネ定数
   */
  void SetSpringStiffness(SpringSide lr, f32 stiffness);

  /**
   * @brief バネ定数取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return バネ定数を返す
   */
  f32 GetSpringStiffness(SpringSide lr) const;

  /**
   * @brief バネ質量設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param mass 質量
   */
  void SetSpringMass(SpringSide lr, f32 mass);

  /**
   * @brief バネ質量取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return 質量を返す
   */
  f32 GetSpringMass(SpringSide lr) const;

  /**
   * @brief バネ重力設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param gravity 重力
   */
  void SetSpringGravity(SpringSide lr, f32 gravity);

  /**
   * @brief バネ重力取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return 重力を返す
   */
  f32 GetSpringGravity(SpringSide lr) const;

  /**
   * @brief バネＹ軸回転だけ行う設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param flag Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  void SetSpringRotOnlyY(SpringSide lr, b32 flag);

  /**
   * @brief バネＹ軸回転だけ行うか？
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return Ｙ軸回転だけならtrue, 全て回転させるならfalse
   */
  b32 IsSpringRotOnlyY(SpringSide lr) const;

  /**
   * @brief バネ外力設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param externalForce 外力
   */
  void SetSpringExternalForce(SpringSide lr, const gfl2::math::Vector3& externalForce);

  /**
   * @brief バネ外力取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @return 外力を返す
   */
  const gfl2::math::Vector3 GetSpringExternalForce(SpringSide lr) const;

  /**
   * @brief バネダウンフォース設定
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   * @param downForce ダウンフォース
   */
  void SetSpringDownForce(SpringSide lr, f32 downForce);

  /**
   * @brief バネダウンフォース取得
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   *
   * @return ダウンフォースを返す
   */
  f32 GetSpringDownForce(SpringSide lr) const;

  /**
   * @brief バネパラメータのリセットリクエスト
   *
   * @param lr SPRING_SIDE_L or SPRING_SIDE_R
   */
  void RequestSpringParameterReset(SpringSide lr);

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
  b32 HasJointAnimationInLoopMotion(u32 loopIndex);

private:
  void SetCallback(void* pUserData, gfl2::animation::KeyAnimationCallback* pCallback);
  poke_3d::model::PokemonModel::KeyCallBackInfo m_pKeyCallBackInfo;

  /**
   * @brief モデル生成後の初期化
   */
  void Initialize( gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap );

  /**
   * @brief カラーシェーダ用定数の更新
   */
  void UpdateColorShaderConstantColor();

  /**
   * @brief 環境光用定数の更新
   */
  void UpdateAmbientConstantColor();

  /**
   * @brief マテリアルアトリビュートのリセット
   */
  void ResetMaterialAttribute();

  /**
   * @brief 模様を固定する
   */
  void SetUpMarkings();

  s32 m_MotionNum;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionResourceNodes[MOTION_MAX];

  // 目, 口, ループモーションのモーションインデックス
  s32 m_OverridePartsMotionIndices[gfl2::animation::OverridePartsType::NumberOf];

  //!< @brief 目, 口, ループモーションに設定するスロット番号
  //!< @note niji, GFModelViewerで設定しているスロットが異なるため, テーブルを用意する
  s32 m_OverridePartsMotionSlots[gfl2::animation::OverridePartsType::NumberOf];

  gfl2::math::AABB  m_AABB;
  f32               m_CmHeight;
  f32               m_AdjustHeight;

  b32                      m_IsEnabledColorShader;  //!< @brief カラーシェーダ有効/無効
  gfl2::math::Vector4      m_InterpColor;           //!< @brief カラーシェーダ用補間カラー
  b32                      m_IsEnabledAmbientColor; //!< @brief 環境光シェーダ有効/無効
  gfl2::math::Vector4      m_AmbientColor;          //!< @brief 環境光シェーダ用カラー

  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_OriginalColorTextureResourceNodes[COLORTEXTURE_MAX];
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* m_ColorShaderTextureResourceNodes[COLORTEXTURE_MAX];
  u32 m_ColorTextureNum;

  //!< @brief レアカラーの定数カラーの情報
  RareColorConstantSettingInfo m_RareColorConstantSettingInfos[MATERIAL_MAX * gfl2::renderingengine::scenegraph::resource::CombinerAttribute::ConstantMax];
  u32 m_RareColorConstantSettingInfoNum;

  //!< @brief 元のマテリアルアトリビュートの設定値. リセットする際に用いる.
  gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam m_OriginalAttributeParams[MATERIAL_MAX];

  // ライトとライトセットは1個ずつ持つ.
  gfl2::renderingengine::scenegraph::instance::LightNode* m_pLightNode;       //!< @brief ライトノード
  gfl2::renderingengine::scenegraph::instance::LightSetNode* m_pLightSetNode; //!< @brief ライトセットノード

  // ライトセットひとつに対してDrawEnvもひとつ持つ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;

  //!< @brief 火炎放射が出る位置などの, エフェクトの発生源のResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEffectLocatorResourceNode;
  //!< @brief エフェクト発生源のInstanceNode. この子ノードに発生源のノードが設定されている.
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* m_pEffectLocatorInstanceNodeTop;

  //!< @brief ポケデコのロケータのResourceNode
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pPokedecoLocatorResourceNode;
  //!< @brief ポケデコのロケータのInstanceNode. この子ノードに発生源のノードが設定されている.
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode* m_pPokedecoLocatorInstanceNodeTop;

  const PokemonModel* m_pIdModel;     //!< @brief IDテクスチャ表示用のモデル
  const PokemonModel* m_pShadowModel; //!< @brief 影表示用のモデル

  //!< @brief バネ制御
  gfl2::animation::SpringController* m_pSpringController;

  //!< @brief 模様の固定フレーム(ループの総数ぶん, 0～255)
  u8 m_MarkingFrames[gfl2::animation::OverridePartsType::Loop04 - gfl2::animation::OverridePartsType::Loop01 + 1];

#if defined POKEMONMODEL_PATCHEEL_EXCEPTION
  //!< @brief パッチールかどうかの判定をするフラグ(pm0327_00がModelInstanceNodeの名前に含まれているかを判定)
  b8 m_IsPatcheel;
#endif
};

}}

#endif