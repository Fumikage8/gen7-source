#if !defined(POKE_3D_LIB_MODEL_DRESSUP_MODEL_H_INCLUDED)
#define POKE_3D_LIB_MODEL_DRESSUP_MODEL_H_INCLUDED

#include "gfl2_CharaModel.h"
#include "gfl2_DressUpParam.h"

namespace poke_3d { namespace model {

// 着せ替えモデルリソースの管理単位
class DressUpModelResourceUnit;

/**
 * @brief 着せ替え用モデルクラス
 */
class DressUpModel : public CharaModel
{
public:

  friend class CharaModelFactory;

  /**
   * @brief 着せ替え性別
   */
  enum Sex
  {
    SEX_MALE = DressUpParam::SEX_MALE,
    SEX_FEMALE = DressUpParam::SEX_FEMALE,

    SEX_COUNT = DressUpParam::SEX_COUNT
  };

  /**
   * @brief 着せ替えパーツモデル
   *    追加修正が合った場合は、以下の定義も修正してください  Modify. N.Takeda 2015/10/15
   *    r:/home/niji/exec/reference_files/niji_reference_files/script/FieldPawnTypes.h(315):   PLAYER_PARTS_FACE,      ///< 顔
   */
  enum PartsModel
  {
    PARTS_MODEL_FACE = DressUpParam::ITEM_CATEGORY_FACE,                // 顔
    PARTS_MODEL_HAIR = DressUpParam::ITEM_CATEGORY_HAIR_STYLE,          // 髪
    PARTS_MODEL_ACCBADGE = DressUpParam::ITEM_CATEGORY_ACCBADGE,        // バッジアクセサリ
    PARTS_MODEL_ACCEYE = DressUpParam::ITEM_CATEGORY_ACCEYE,            // アイアクセサリ
    PARTS_MODEL_ACCHAIR = DressUpParam::ITEM_CATEGORY_ACCHAIR,          // ヘアアクセサリ
    PARTS_MODEL_BAG = DressUpParam::ITEM_CATEGORY_BAG,                  // バッグ
    PARTS_MODEL_BNGL = DressUpParam::ITEM_CATEGORY_BNGL,                // バングル
    PARTS_MODEL_BODY = DressUpParam::ITEM_CATEGORY_BODY,                // ボディ
    PARTS_MODEL_BOTTOMS = DressUpParam::ITEM_CATEGORY_BOTTOMS,          // ボトムス
    PARTS_MODEL_HAT = DressUpParam::ITEM_CATEGORY_HAT,                  // 帽子
    PARTS_MODEL_LEGS = DressUpParam::ITEM_CATEGORY_LEGS,                // レッグ
    PARTS_MODEL_SHOES = DressUpParam::ITEM_CATEGORY_SHOES,              // シューズ
    PARTS_MODEL_TOPS = DressUpParam::ITEM_CATEGORY_TOPS,                // トップス
    PARTS_MODEL_SITU = DressUpParam::ITEM_CATEGORY_SITU,                // シチュエーション着替え

    PARTS_MODEL_COUNT = DressUpParam::ITEM_CATEGORY_COUNT               // 着せ替えパーツモデル総数
  };

public:

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  DressUpModel(); 
  /**
   * @brief デストラクタ
   */
  virtual ~DressUpModel();

  /**
   * @brief リソースノードから作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pDressUpResourceUnitList ドレスアップリソースユニットリスト
   * @param pModelResource ベースモデルリソース
   * @param pMotionConfigResource モーションコンフィグリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  bool Create(gfl2::gfx::IGLAllocator* pGfxAllocator,
              gfl2::heap::HeapBase* pAnimationHeap,
              DressUpModelResourceUnit* pDressUpModelResourceUnitList,
              gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
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
  // ■ アニメーション

  /**
   * @brief アニメーションの更新
   *
   * @param animationBit アニメーションの種類
   */
  virtual f32 UpdateAnimation(AnimationBit animationBit = ANIMATION_BIT_ALL);

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
   * @brief アニメーションの変更：補間あり：パック内のID指定版
   *
   * @param animationId アニメーションパック内のID
   * @param changeFrame 補間フレーム数
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void ChangeAnimationSmooth(u32 animationId, u32 changeFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループ設定：デフォルトはアニメーションデータに設定されている値
   *
   * @param enable trueの場合はループする、 falseの場合はループしない
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationLoop(b32 enable, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief フレーム設定
   *
   * @param frame フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationFrame(f32 frame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief 再生速度設定
   *
   * @param stepFrame 再生速度：1.0fで等速、マイナスの値を指定すると逆再生
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationStepFrame(f32 stepFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループの開始フレーム設定
   *
   * @param startFrame 開始フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationStartFrame(f32 startFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief ループの終了フレーム設定
   *
   * @param endFrame 終了フレーム
   * @param slotIndex スロット番号
   * @param animationBit アニメーションの種類
   */
  virtual void SetAnimationEndFrame(f32 endFrame, u32 slotIndex = 0, AnimationBit animationBit = ANIMATION_BIT_ALL);

  /**
   * @brief アニメーション計算有効フラグの設定、falseを設定した場合はフレーム更新のみ有効
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
   * @brief 注視処理更新： ※シーンのトラバース後に呼んでください
   */
  virtual void UpdateInterest();


  // パーツのベースモデル取得
  BaseModel* GetPartsModel(PartsModel partsModel)
  {
    return (m_PartsModelList[partsModel].IsCreated())
      ? &m_PartsModelList[partsModel]
      : NULL;
  }

  // パーツのベースモデル取得
  const BaseModel* GetPartsModel(PartsModel partsModel) const
  {
    return (m_PartsModelList[partsModel].IsCreated())
      ? &m_PartsModelList[partsModel]
      : NULL;
  }

  // パーツのモデルインスタンスノード取得
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetPartsModelInstanceNode(PartsModel partsModel)
  {
    return (m_PartsModelList[partsModel].IsCreated())
      ? m_PartsModelList[partsModel].GetModelInstanceNode()
      : NULL;
  }

  // パーツのモデルインスタンスノード取得
  const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetPartsModelInstanceNode(PartsModel partsModel) const
  {
    return (m_PartsModelList[partsModel].IsCreated())
      ? m_PartsModelList[partsModel].GetModelInstanceNode()
      : NULL;
  }

  // 全力ビーズのON/OFF設定
  void SetZenBeadsVisible(b32 flag);

  // 全力ビーズのON/OFF取得
  b32 IsZenBeadsVisible() const;

  // バッグ証のON/OFF設定
  void SetBagCharmVisible(b32 flag);

  // バッグ証のON/OFF取得
  b32 IsBagCharmVisible() const;

private:

  template <typename F, typename A0>
  void SetAnimationCore(F func, A0 arg0, u32 slotIndex, AnimationBit animationBit)
  {
    for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
    {
      BaseModel* pBaseModel = &m_PartsModelList[i];

      if (!pBaseModel->IsCreated())
      {
        continue;
      }

      if (!pBaseModel->HasAnimationController())
      {
        continue;
      }

      (pBaseModel->*func)(arg0, slotIndex, animationBit);
    }
  }
  template <typename F, typename A0, typename A1>
  void SetAnimationCore(F func, A0 arg0, A1 arg1, u32 slotIndex, AnimationBit animationBit)
  {
    for (u32 i = 0; i < PARTS_MODEL_COUNT; ++i)
    {
      BaseModel* pBaseModel = &m_PartsModelList[i];

      if (!pBaseModel->IsCreated())
      {
        continue;
      }

      if (!pBaseModel->HasAnimationController())
      {
        continue;
      }

      (pBaseModel->*func)(arg0, arg1, slotIndex, animationBit);
    }
  }

  // 着せ替えは顔のパーツにセットする必要がある
  virtual void ChangeAnimationByResourceNode_Face(gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode, s32 slot);
  virtual void SetAnimationStepFrame_Face(s32 stepFrame, s32 slot);
  virtual void SetAnimationFrame_Face(s32 frame, s32 slot);

  // パーツモデルのメッシュON/OFF設定
  void SetPartsModelMeshVisible(PartsModel partsModel, const char* pMeshName, b32 flag);

  // パーツモデルのメッシュON/OFF取得
  b32 IsPartsModelMeshVisible(PartsModel partsModel, const char* pMeshName) const;

  // パーツモデルからメッシュインデックス取得
  s32 GetPartsModelMeshIndex(PartsModel partsModel, const char* pMeshName) const;

  // パーツの視線更新
  void UpdateInterestParts(PartsModel partsModel);

  // 首更新後処理
  virtual void OnUpdateNeck();

private:

  poke_3d::model::BaseModel m_PartsModelList[PARTS_MODEL_COUNT];
  DressUpModelResourceUnit* m_pDressUpModelResourceUnitList;
};

}}

#endif
