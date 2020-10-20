#if !defined(POKE_3D_LIB_MODEL_DRESSUP_MODEL_RESOURCE_UNIT_H_INCLUDED)
#define POKE_3D_LIB_MODEL_DRESSUP_MODEL_RESOURCE_UNIT_H_INCLUDED

#include <model/include/gfl2_DressUpModel.h>

#include "gfl2_BaseModelResourceUnit.h"

namespace poke_3d { namespace model {

/**
 * @brief 着せ替え用モデルリソース管理単位
 */
class DressUpModelResourceUnit : public BaseModelResourceUnit
{
public:

  /**
   * @brief マスクテクスチャID
   */
  enum MaskTextureId
  {
    MASK_TEXTURE_ID_SKIN,                                           ///< 肌 
    MASK_TEXTURE_ID_CLOTH,                                          ///< 服 
    MASK_TEXTURE_ID_HAIR,                                           ///< 髪 
    MASK_TEXTURE_ID_EYE,                                            ///< 目 
    MASK_TEXTURE_ID_EYEBLOW,                                        ///< 眉 
    MASK_TEXTURE_ID_LIP,                                            ///< リップ 

    MASK_TEXTURE_ID_COUNT                                           ///< 総数 
  };

  /**
   * @brief サブマスクテクスチャID
   */
  enum SubMaskTextureId
  {
    SUBMASK_TEXTURE_ID_NORMAL,                                      ///< 通常
    SUBMASK_TEXTURE_ID_FACEMOUTH,                                   ///< バトルモデルのfacemouth用

    SUBMASK_TEXTURE_ID_COUNT                                        ///< 総数
  };

  /**
   * @brief オプションのbit
   */
  enum OptionBit
  {
    OPTION_BIT_LEGS,                                                ///< 足の長さ long[0], short[1]
    OPTION_BIT_SHADOW,                                              ///< 影を落とすか on[0], off[1]
    OPTION_BIT_SANDAL,                                              ///< サンダル 
    OPTION_BIT_HEADBAND,                                            ///< カチューシャ 
    OPTION_BIT_NO_HAT,                                              ///< 帽子をかぶれない 

    OPTION_BIT_COUNT                                                ///< 総数 
  };

  /**
   * @brief ロケータ
   */
  struct DressUpLocator
  {
    f32 scaleX;
    f32 scaleY;
    f32 scaleZ;

    f32 rotationX;
    f32 rotationY;
    f32 rotationZ;

    f32 translateX;
    f32 translateY;
    f32 translateZ;
  };

public:

  /**
   * @brief BinLinkerでこの順番にパックしている
   */
  enum DressUpBinLinkerIndex
  {
    DRESSUP_BIN_LINKER_INDEX_LOCATOR = BASE_BIN_LINKER_INDEX_COUNT, ///< ロケータ
    DRESSUP_BIN_LINKER_INDEX_MASK,                                  ///< マスクテクスチャ

    // ここに可変長でマスクテクスチャが含まれる

    DRESSUP_BIN_LINKER_INDEX_COUNT                                  ///< パック総数
  };

public:

  /**
   * @brief コンストラクタ
   */
  DressUpModelResourceUnit();

  /**
   * @brief デストラクタ
   */
  virtual ~DressUpModelResourceUnit();

  /**
   * @brief 着せ替えパーツのセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupDressUpParts(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator);

  /**
   * @brief テクスチャカラーの変更
   *
   * @param maskTextureId マスクテクスチャID
   * @param submaskTextureId サブマスクテクスチャID
   * @param color カラー(RGB 0.0f〜1.0f で指定)
   * @param inout_pTexturePixel テクスチャの処理位置
   * @param out_pComplete true:色替え完了 false:処理中
   *
   * @return ture:色替え対象のテクスチャが存在する false:色替え対象のテクスチャが存在しない
   */
  bool ChangeTextureColor(MaskTextureId maskTextureId, SubMaskTextureId submaskTextureId, const gfl2::math::Vector3& color, u32* inout_pTexturePixel, bool* out_pComplete);

  /**
   * @brief テクスチャカラーの変更：face専用処理
   *
   * @param colorList カラーリスト(RGB 0.0f〜1.0f で指定)
   * @param hasLipColor true:リップカラーあり, false:リップカラー無し
   * @param submaskTextureId サブマスクテクスチャID
   * @param inout_pTexturePixel テクスチャの処理位置
   * @param out_pComplete true:色替え完了 false:処理中
   *
   * @return ture:色替え対象のテクスチャが存在する false:色替え対象のテクスチャが存在しない
   */
  bool ChangeTextureColorFace(const gfl2::math::Vector3 colorList[], bool hasLipColor, SubMaskTextureId submaskTextureId, u32* inout_pTexturePixel, bool* out_pComplete);

  /**
   * @brief ロケータの取得
   */
  const DressUpLocator* GetDressUpLocator() const;

  /**
   * @brief アイテムごとのオプションビット設定
   *
   * @param optionBit 例外設定などのオプションビット
   */
  void SetItemOptionBit(u8 optionBit);

  /**
   * @brief アイテムごとのオプションビットが立っているか確認
   *
   * @param checkOptionBit 確認するオプション
   * @return true:立っている false:立っていない
   */
  bool IsItemOption(OptionBit checkOptionBit) const;

  /**
   * @brief 対応する着せ替えのアイテム番号を設定
   *
   * @param itemIndex 着せ替えアイテム番号
   */
  void SetItemIndex(s16 itemIndex);

  /**
   * @brief 対応する着せ替えのアイテム番号を取得
   *
   * @return 着せ替えアイテム番号
   */
  s16 GetItemIndex() const;

private:

  // マスクテクスチャリソース
  struct MaskTextureResource
  {
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode;       // リソースノードリスト
    const char* pOriginalTextureName;                                               // オリジナルテクスチャ名
    u32 originalTextureNameLength;                                                  // オリジナルテクスチャ名サイズ

    MaskTextureResource() :
      pResourceNode(NULL),
      pOriginalTextureName(NULL),
      originalTextureNameLength(0)
    {
    }

    void Unload()
    {
      GFL_SAFE_DELETE(this->pResourceNode);
      this->pOriginalTextureName = NULL;
      this->originalTextureNameLength = 0;
    }
  };

private:

  /**
   * @brief 読み込んだ静的データの開放：派生クラス用
   */
  virtual void UnloadStaticDataDerived();

  /**
   * @brief マスクテクスチャに対応するテクスチャリソースを探す
   */
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode* FindTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* pModelResourceNode, MaskTextureResource* pMaskTextureResource);

  /**
   * @brief マスクテクスチャとそのマスクに対応するテクスチャリソースを探す
   */
  bool FindMaskTextureResourceAndOriginalTextureResourceNode(MaskTextureId maskTextureId, u32 submask, MaskTextureResource** out_ppMaskTextureResource, gfl2::renderingengine::scenegraph::resource::TextureResourceNode** out_ppOriginalTextureResourceNode);

private:

  MaskTextureResource m_MaskTextureResourceList[MASK_TEXTURE_ID_COUNT][SUBMASK_TEXTURE_ID_COUNT];             ///< マスクテクスチャリソースノードリスト
  s16 m_ItemIndex;                                                                                            ///< 対応するアイテムインデックス
  u8 m_ItemOptionBit;                                                                                         ///< アイテムごとのオプションビット
  u8 :8; // パディング
};

}}

#endif
