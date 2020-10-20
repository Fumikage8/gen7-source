//===================================================================
/**
 * @file    Sky.cpp
 * @brief   天球の空の制御
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#include "Test/FieldTest/include/Skybox/Sky.h"
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
Sky::Sky() :
  m_pSkyModel()
{
}

///< デストラクタ
Sky::~Sky()
{
}

/**
 * @brief   生成
 * @param   pSystemHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pAsyncFileManager garc読み込みクラス
 */
void Sky::Create( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, gfl2::fs::AsyncFileManager* pAsyncFileManager)
{
  const SkyboxModel::Description box_desc = 
  {
    pAsyncFileManager,
    pSystemHeap,
    pGLAllocator,
    ARCID_SKYBOX,
    GARC_skybox_box_GFBMDLP
  };

  m_pSkyModel[ MODEL_TYPE_BOX ] = GFL_NEW( pSystemHeap) SkyboxModel();
  m_pSkyModel[ MODEL_TYPE_BOX ]->Create( box_desc);
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 Sky::Destroy()
{
  b8 isFinish = true;
  for( s32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    if( m_pSkyModel[i])
    {
      if( m_pSkyModel[i]->Destroy())
      {
        GFL_SAFE_DELETE( m_pSkyModel[i])
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
void Sky::Initialzie()
{
}

/**
 * @brief   更新
 */
void Sky::Update()
{
}

/**
 * @brief   描画
 */
void Sky::Draw()
{
}

/**
 * @brief   表示フラグ切り替え
 * @param   isEnable trueで有効、falseで無効
 */
void Sky::SetVisible( b8 isEnable)
{
  m_pSkyModel[ MODEL_TYPE_BOX ]->SetVisible( isEnable);
}

/**
 * @brief   表示フラグ取得
 * @retval  true 有効
 * @retval  false 無効
 */
b8 Sky::IsVisible() const
{
  return m_pSkyModel[ MODEL_TYPE_BOX ]->IsVisible();
}

/**
 * @brief   モデルインスタンスの取得
 */
SkyboxModel* Sky::GetModel( u32 index)
{
  GFL_ASSERT( index < MODEL_TYPE_MAX);
  return m_pSkyModel[index];
}


GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )
