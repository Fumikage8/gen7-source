#if !defined(POKE_3D_LIB_MODEL_CHARA_MODEL_RESOURCE_UNIT_H_INCLUDED)
#define POKE_3D_LIB_MODEL_CHARA_MODEL_RESOURCE_UNIT_H_INCLUDED

#include "gfl2_BaseModelResourceUnit.h"

namespace poke_3d { namespace model {

/**
 * @brief リソースの管理単位
 */
class CharaModelResourceUnit : public BaseModelResourceUnit
{
public:

  /**
   * @brief BinLinkerでこの順番にパックしている
   */
  enum CharaBinLinkerIndex
  {
    CHARA_BIN_LINKER_INDEX_MCNF = BASE_BIN_LINKER_INDEX_COUNT,  ///< モーションコンフィグ
    CHARA_BIN_LINKER_INDEX_APP_DATA,                            ///< アプリケーション用の任意データ
    CHARA_BIN_LINKER_INDEX_EYE_ANIM,                            ///< 目パチのアニメーションデータ
    CHARA_BIN_LINKER_INDEX_MOUTH_ANIM,                          ///< 口ぱくのアニメーションデータ

    CHARA_BIN_LINKER_INDEX_COUNT                                ///< パック総数
  };

public:

  /**
   * @brief コンストラクタ
   */
  CharaModelResourceUnit();

  /**
   * @brief デストラクタ
   */
  virtual ~CharaModelResourceUnit();

  //---------------------------------------------------------------------------------
  // ■ 静的データ

  /**
   * @brief モーションコンフィグリソースノードセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupMotionConfigResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator);

  /**
   * @brief モーションコンフィグリソースノード取得
   *
   * @return モーションコンフィグリソースノードを返す。まだデータが読み込まれていないかセットアップ前ならNULL
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionConfigResourceNode();

  /**
   * @brief モーションコンフィグリソースノード取得
   *
   * @return モーションコンフィグリソースノードを返す。まだデータが読み込まれていないかセットアップ前ならNULL
   */
  const gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMotionConfigResourceNode() const;

  /**
   * @brief アプリケーションデータを取得
   *
   * @param pFileManager ファイルマネージャ
   *
   * @return アプリケーションデータを返す。まだデータが読み込まれていないかセットアップ前ならNULL
   */
  const void* GetApplicationData(gfl2::fs::AsyncFileManager* pFileManager) const;

  /**
   * @brief 目パチモーションリソースノードセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupEyeMotionResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator);

  /**
   * @brief 目パチモーションリソースノード取得
   *
   * @return 目パチモーションリソースノードを返す。まだデータが読み込まれていないかセットアップ前、もしくはダミーデータならNULL
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetEyeMotionResourceNode();

  /**
   * @brief 口パクモーションリソースノードセットアップ
   *
   * @param pFileManager ファイルマネージャ
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupMouthMotionResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator);

  /**
   * @brief 口パクモーションリソースノード取得
   *
   * @return 口パクモーションリソースノードを返す。まだデータが読み込まれていないかセットアップ前、もしくはダミーデータならNULL
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetMouthMotionResourceNode();

private:

  /**
   * @brief 読み込んだ静的データの開放：派生クラス用
   */
  virtual void UnloadStaticDataDerived();

private:

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMotionConfigResourceNode;     ///< 読み込み用バッファから生成したモーションコンフィグリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEyeMotionResourceNode;        ///< 目パチモーションのリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pMouthMotionResourceNode;      ///< 口パクモーションのリソースノード
};

}}

#endif // POKE_3D_LIB_MODEL_CHARA_MODEL_RESOURCE_UNIT_H_INCLUDED
