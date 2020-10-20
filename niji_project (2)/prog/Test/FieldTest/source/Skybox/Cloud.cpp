//===================================================================
/**
 * @file    Cloud.cpp
 * @brief   天球の雲の制御
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#include "Test/FieldTest/include/Skybox/Cloud.h"
#include "Test/FieldTest/include/Skybox/SkyboxModel.h"

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// fs
#include <fs/include/gfl2_FsAsyncFileManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/skybox_test.gaix>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @brief   コンストラクタ
 */
Cloud::Cloud() :
  m_pCloudModel()
{
}

///< デストラクタ
Cloud::~Cloud()
{
}

/**
 * @brief   生成
 * @param   pSystemHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pAsyncFileManager garc読み込みクラス
 */
void Cloud::Create( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, gfl2::fs::AsyncFileManager* pAsyncFileManager)
{
  const u32 DAT_ID_TABLE[] =
  {
    GARC_skybox_cloud_side_1_1_GFBMDLP,
    GARC_skybox_cloud_side_1_2_GFBMDLP,
  };

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    const SkyboxModel::Description desc = 
    {
      pAsyncFileManager,
      pSystemHeap,
      pGLAllocator,
      ARCID_SKYBOX,
      DAT_ID_TABLE[i]
    };

    m_pCloudModel[i] = GFL_NEW( pSystemHeap) SkyboxModel();
    m_pCloudModel[i]->Create( desc);
  }
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 Cloud::Destroy()
{
  b8 isFinish = true;
  for( s32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    if( m_pCloudModel[i])
    {
      if( m_pCloudModel[i]->Destroy())
      {
        GFL_SAFE_DELETE( m_pCloudModel[i])
      }
      else
      {
        isFinish = false;
      }
    }
  }
  return isFinish;
}

/**
 * @brief   初期化
 */
void Cloud::Initialzie()
{
}

/**
 * @brief   更新
 */
void Cloud::Update()
{
}

/**
 * @brief   描画
 */
void Cloud::Draw()
{
}

/**
 * @brief   表示フラグ切り替え
 * @param   isEnable trueで有効、falseで無効
 */
void Cloud::SetVisible( b8 isEnable)
{
  for( s32  i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_pCloudModel[i]->SetVisible( isEnable);
  }
}

/**
 * @brief   表示フラグ取得
 * @retval  true 有効
 * @retval  false 無効
 */
b8 Cloud:: IsVisible() const
{
  return m_pCloudModel[0]->IsVisible();
}

/**
 * @brief   モデルインスタンスの取得
 */
SkyboxModel* Cloud::GetModel( u32 index)
{
  GFL_ASSERT( index < MODEL_TYPE_MAX);
  return m_pCloudModel[index];
}

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
