#include "gfl2_CharaModelResourceUnit.h"

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

namespace poke_3d { namespace model {

/**
 * @brief コンストラクタ
 */
CharaModelResourceUnit::CharaModelResourceUnit() :
  m_pMotionConfigResourceNode(NULL),
  m_pEyeMotionResourceNode(NULL),
  m_pMouthMotionResourceNode(NULL)
{
}

/**
 * @brief デストラクタ
 */
CharaModelResourceUnit::~CharaModelResourceUnit()
{
  this->Finalize();
}


/**
 * @brief 読み込んだ静的データの開放：派生クラス用
 */
void CharaModelResourceUnit::UnloadStaticDataDerived()
{
  // リソースノードとバッファを開放
  GFL_SAFE_DELETE(m_pMotionConfigResourceNode);
  GFL_SAFE_DELETE(m_pEyeMotionResourceNode);
  GFL_SAFE_DELETE(m_pMouthMotionResourceNode);
}

/**
 * @brief モーションコンフィグリソースノードセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelResourceUnit::SetupMotionConfigResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // すでにリソースノードが生成されているなら何もしない
  if (m_pMotionConfigResourceNode != NULL)
  {
    return true;
  }

  // BinLinkerからモーションコンフィグデータを取得する
  const void* pMcnfData = this->GetBinLinkerData(pFileManager, CHARA_BIN_LINKER_INDEX_MCNF);
  if (pMcnfData == NULL)
  {
    return false;
  }

  // リソースノードを生成する
  gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData mcnfData;
  mcnfData.SetMcnfData((c8*)pMcnfData);

  m_pMotionConfigResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &mcnfData, 0);
  return (m_pMotionConfigResourceNode != NULL);
}

/**
 * @brief モーションコンフィグリソースノード取得
 *
 * @return モーションコンフィグリソースノードを返す。まだデータが読み込まれていないならNULL
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* CharaModelResourceUnit::GetMotionConfigResourceNode()
{
  return m_pMotionConfigResourceNode;
}

/**
 * @brief モーションコンフィグリソースノード取得
 *
 * @return モーションコンフィグリソースノードを返す。まだデータが読み込まれていないならNULL
 */
const gfl2::renderingengine::scenegraph::resource::ResourceNode* CharaModelResourceUnit::GetMotionConfigResourceNode() const
{
  return m_pMotionConfigResourceNode;
}

/**
 * @brief アプリケーションデータを取得
 *
 * @param pFileManager ファイルマネージャ
 *
 * @return アプリケーションデータを返す。まだデータが読み込まれていないかセットアップ前ならNULL
 */
const void* CharaModelResourceUnit::GetApplicationData(gfl2::fs::AsyncFileManager* pFileManager) const
{
  return this->GetBinLinkerData(pFileManager, CHARA_BIN_LINKER_INDEX_APP_DATA);
}

/**
 * @brief 目パチモーションリソースノードセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelResourceUnit::SetupEyeMotionResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // すでにリソースノードが生成されているなら何もしない
  if (m_pEyeMotionResourceNode != NULL)
  {
    return true;
  }

  // BinLinkerから目パチモーションデータを取得する
  u32 dataSize = this->GetBinLinkerDataSize(pFileManager, CHARA_BIN_LINKER_INDEX_EYE_ANIM);
  if (dataSize == 0)
  {
    // ダミーデータなので何もしない
    return true;
  }
  const void* pEyeMotionData = this->GetBinLinkerData(pFileManager, CHARA_BIN_LINKER_INDEX_EYE_ANIM);
  if (pEyeMotionData == NULL)
  {
    return false;
  }

  // リソースノードを生成する
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motData;
  motData.SetMotionData((c8*)pEyeMotionData);

  m_pEyeMotionResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &motData, 0);
  return (m_pEyeMotionResourceNode != NULL);
}

/**
 * @brief 目パチモーションリソースノード取得
 *
 * @return 目パチモーションリソースノードを返す。まだデータが読み込まれていないかセットアップ前、もしくはダミーデータならNULL
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* CharaModelResourceUnit::GetEyeMotionResourceNode()
{
  return m_pEyeMotionResourceNode;
}

/**
 * @brief 口パクモーションリソースノードセットアップ
 *
 * @param pFileManager ファイルマネージャ
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelResourceUnit::SetupMouthMotionResourceNode(gfl2::fs::AsyncFileManager* pFileManager, gfl2::gfx::IGLAllocator* pGfxAllocator)
{
  // すでにリソースノードが生成されているなら何もしない
  if (m_pMouthMotionResourceNode != NULL)
  {
    return true;
  }

  // BinLinkerから目パチモーションデータを取得する
  u32 dataSize = this->GetBinLinkerDataSize(pFileManager, CHARA_BIN_LINKER_INDEX_MOUTH_ANIM);
  if (dataSize == 0)
  {
    // ダミーデータなので何もしない
    return true;
  }
  const void* pMouthMotionData = this->GetBinLinkerData(pFileManager, CHARA_BIN_LINKER_INDEX_MOUTH_ANIM);
  if (pMouthMotionData == NULL)
  {
    return false;
  }

  // リソースノードを生成する
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motData;
  motData.SetMotionData((c8*)pMouthMotionData);

  m_pMouthMotionResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNodeIndex(pGfxAllocator, &motData, 0);
  return (m_pMouthMotionResourceNode != NULL);
}

/**
 * @brief 口パクモーションリソースノード取得
 *
 * @return 口パクモーションリソースノードを返す。まだデータが読み込まれていないかセットアップ前、もしくはダミーデータならNULL
 */
gfl2::renderingengine::scenegraph::resource::ResourceNode* CharaModelResourceUnit::GetMouthMotionResourceNode()
{
  return m_pMouthMotionResourceNode;
}

}}
