//======================================================================
/**
 * @file FieldPlayerShade.cpp
 * @date 2016/11/29 12:59:15
 * @author kawazoe_shinichi
 * @brief フィールドプレイヤー用陰影関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>

// キャラクターシステム
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"


#include "Field/FieldRo/include/Shade/FieldPlayerShade.h"
#include "Field/FieldRo/include/Shade/FieldShadeAreaManager.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Shade);

static const u32 CONSTANT_COLOR_IDX = 5;    //!< 使用するコンスタントカラー番号

//-------------------------------------------------------------------------
/// @brief  コンストラクタ
//-------------------------------------------------------------------------
FieldPlayerShade::FieldPlayerShade() :
m_pHeap             (NULL),
m_pShadeAreaManager (NULL),
m_isFirstFrameForFieldmap(true),
m_isColorChanging   (false),
m_TargetAreaIndex   (-1),
m_CurrentFrame      (0),
m_DoFrame           (20),
m_isRidingForBeforeFrame(false),
// @fix MMCat[262] 影領域外で画面遷移による復帰を行った後ライドするとモデルが真っ黒になる。対処、補間用の色変数が(1.0f,1.0f,1.0f)で初期化されていないため、色変数への設定が行われない処理ルートで色変更が実行されると発生していたので初期化処理を追加
m_vStartColor       (1.0f,1.0f,1.0f),
m_vEndColor         (1.0f,1.0f,1.0f)
{

}

//-------------------------------------------------------------------------
/// @brief  デストラクタ
//-------------------------------------------------------------------------
FieldPlayerShade::~FieldPlayerShade()
{
  Terminate();
}

//-------------------------------------------------------------------------
/// @brief  初期化処理
/// @param desc   初期化設定データ
//-------------------------------------------------------------------------
void FieldPlayerShade::Initialize(const InitDesc& desc)
{
  m_pHeap = desc.pHeap;

  FieldShadeAreaManager::InitDesc managerDesc;
  managerDesc.pShadeAreaData = desc.pShadeAreaData;

  // 影領域管理クラス生成
  m_pShadeAreaManager = GFL_NEW(m_pHeap) FieldShadeAreaManager();
  m_pShadeAreaManager->Initialize(managerDesc);
  
}

//-------------------------------------------------------------------------
/// @brief  終了処理
//-------------------------------------------------------------------------
void FieldPlayerShade::Terminate(void)
{
  GFL_SAFE_DELETE(m_pShadeAreaManager);
}



//-------------------------------------------------------------------------
/// @brief  更新処理
/// @param pPlayer  プレイヤー動作モデル
//-------------------------------------------------------------------------
void FieldPlayerShade::Update(Field::MoveModel::FieldMoveModelPlayer* pPlayer)
{
  if (!pPlayer || !pPlayer->GetCharaDrawInstance())
  {
    return;
  }
  
  // プレイヤーが領域内かどうか判定
  s32 beforeTarget = m_TargetAreaIndex;
  m_TargetAreaIndex = m_pShadeAreaManager->CheckOverlap(pPlayer->GetCharaDrawInstance()->GetPosition());
  // 領域内外または領域IDが切り替わったかどうか判定
  if (beforeTarget != m_TargetAreaIndex)
  {
    m_vStartColor = GetCurrentConstantColor(pPlayer);             // 現在のコンスタントカラー値から変化を開始させる
    if (m_TargetAreaIndex == -1)
    {
      // 領域外に出た瞬間の場合
      m_vEndColor   = gfl2::math::Vector3(1.0f, 1.0f, 1.0f);      // 初期カラーへ戻す
    }
    else
    {
      // 特定の領域内に入った瞬間の場合
      const FieldShadeAreaManager::ShadeAreaData* const pShadeArea = m_pShadeAreaManager->GetData(m_TargetAreaIndex);
      m_vEndColor = gfl2::math::Vector3(pShadeArea->color[0], pShadeArea->color[1], pShadeArea->color[2]);
    }

    m_isColorChanging = true;   // 色変更処理開始
    m_CurrentFrame    = 0;
  }

  // マップ開始直後の初回フレームかどうか
  if (m_isFirstFrameForFieldmap)
  {
    if (m_isColorChanging)
    {
      // マップ開始直後且つ色変更実行時は色変更を瞬時に行う（復帰時用）
      m_CurrentFrame = m_DoFrame;
    }
    m_isFirstFrameForFieldmap = false;

    m_isRidingForBeforeFrame = pPlayer->IsRideMoveModel();    // フィールド開始時にライド状態かどうかを記憶
  }
  else
  {
    // 初回フレームではない場合

    // ライドチェンジ監視
    if (m_isRidingForBeforeFrame != pPlayer->IsRideMoveModel())
    {
      // 切り替わった場合は現在のカラー状態へ瞬時に変更
      m_isRidingForBeforeFrame  = !m_isRidingForBeforeFrame;
      m_isColorChanging         = true;
      m_CurrentFrame            = m_DoFrame;
    }
  }

  // ------------------------------------------------------
  // 色変更中の場合は色変更処理実行
  // ------------------------------------------------------
  if (m_isColorChanging)
  {
    // 色変更実行
    ChangeColor(pPlayer);
  }
}


//-------------------------------------------------------------------------
/// @brief  色変更
/// @param pPlayer  プレイヤー動作モデル
//-------------------------------------------------------------------------
void FieldPlayerShade::ChangeColor(Field::MoveModel::FieldMoveModelPlayer* pPlayer)
{
  // 色補間（急激な変化と緩やかな落ち着きを表現するために減速補間）
  f32 fT = gfl2::math::Easing::GetValue(gfl2::math::Easing::OUT_EXPO, static_cast<f32>(m_CurrentFrame) / static_cast<f32>(m_DoFrame));
  gfl2::math::Vector3 vColor = m_vStartColor;
  vColor.Lerp(m_vEndColor, fT);

  poke_3d::model::DressUpModel* const pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(pPlayer->GetCharaDrawInstance());
  if (pDressUpModel)
  {
    // pDressUpModel自体のコンスタントカラーは書き換え不要

    // 着せ替えモデルのコンスタントカラー書き換え
    for (u32 i = 0; i < poke_3d::model::DressUpModel::PARTS_MODEL_COUNT; i++)
    {
      const poke_3d::model::DressUpModel::PartsModel type = static_cast<poke_3d::model::DressUpModel::PartsModel>(i);

      poke_3d::model::BaseModel* const pPartsModel = pDressUpModel->GetPartsModel(type);
      if (pPartsModel)
      {
        u32 numDressMat = pPartsModel->GetMaterialCount();
        for (u32 idxMat = 0; idxMat < numDressMat; idxMat++)
        {
          pPartsModel->SetMaterialColorConstant(idxMat, CONSTANT_COLOR_IDX, vColor);
        }
      }
    }

    // ライドしているかどうか調べる
    if (pPlayer->IsRideMoveModel())
    {
      // ライドモデルのコンスタントカラー書き換え
      MoveModel::FieldMoveModel* pRideModel = pPlayer->GetOnMoveModel();
      ApplyColorForMoveModel(pRideModel, vColor);
    }
  }

  // フレームカウント
  m_CurrentFrame++;
  if (m_CurrentFrame > m_DoFrame)
  {
    m_isColorChanging   = false;
    m_CurrentFrame      = 0;
  }

#if PM_DEBUG
#if 0
  // 変化具合をスケール変化で確認
  pPlayer->GetCharaDrawInstance()->SetScale(gfl2::math::Vector3(1.0f, 1.0f + ((1.0f - vColor.x)*0.5f), 1.0f));
#endif
#endif

}


//-------------------------------------------------------------------------
/// @brief  指定した動作モデルに色を反映
/// @param pMoveModel   動作モデル
/// @param vColor       反映させる色
//-------------------------------------------------------------------------
void FieldPlayerShade::ApplyColorForMoveModel(Field::MoveModel::FieldMoveModel* pMoveModel, gfl2::math::Vector3 vColor)
{
  if (!pMoveModel) return;

  // 動作モデルのコンスタントカラー書き換え
  poke_3d::model::CharaModel* const pDrawInstance = pMoveModel->GetCharaDrawInstance();
  u32 numMat = pDrawInstance->GetMaterialCount();
  for (u32 i = 0; i < numMat; i++)
  {
    pDrawInstance->SetMaterialColorConstant(i, CONSTANT_COLOR_IDX, vColor);
  }
}


//-------------------------------------------------------------------------
/// @brief  指定した動作モデルに影領域を反映（領域内に進入している場合はその領域の影色を反映）
/// @param pMoveModel   動作モデル
//-------------------------------------------------------------------------
void FieldPlayerShade::ApplyShadeAreaForMoveModel(Field::MoveModel::FieldMoveModel* pMoveModel)
{
  if (!pMoveModel) return;

  s32 targetAreaIndex = m_pShadeAreaManager->CheckOverlap(pMoveModel->GetCharaDrawInstance()->GetPosition());
  if (targetAreaIndex > -1)
  {
    // 領域内に入っている場合は領域データを取得して設定されている色を動作モデルに反映
    const FieldShadeAreaManager::ShadeAreaData* const pShadeArea = m_pShadeAreaManager->GetData(targetAreaIndex);
    ApplyColorForMoveModel(pMoveModel, gfl2::math::Vector3(pShadeArea->color[0], pShadeArea->color[1], pShadeArea->color[2]));
  }
}



//-------------------------------------------------------------------------
/// @brief  現在のコンスタントカラーを取得する
/// @param pPlayer  プレイヤー動作モデル
/// @return コンスタントカラー
//-------------------------------------------------------------------------
gfl2::math::Vector3 FieldPlayerShade::GetCurrentConstantColor(Field::MoveModel::FieldMoveModelPlayer* pPlayer) const
{
  // 現在のコンスタントカラー値を取得する（適当なパーツ１つのカラーデータを使用する）
  gfl2::math::Vector4 vCurrentColor;
  poke_3d::model::DressUpModel* const pDressUpModel = static_cast<poke_3d::model::DressUpModel*>(pPlayer->GetCharaDrawInstance());
  if (pDressUpModel)
  {
    poke_3d::model::DressUpModel::PartsModel parts = poke_3d::model::DressUpModel::PARTS_MODEL_FACE;
    poke_3d::model::BaseModel* const pPartModel = pDressUpModel->GetPartsModel(parts);
    if (pPartModel != NULL)
    {
      if (pPartModel->GetMaterialCount() > 0)
      {
        vCurrentColor = pPartModel->GetMaterialColorConstant(0, CONSTANT_COLOR_IDX);
      }
    }
  }

  return gfl2::math::Vector3(vCurrentColor.x, vCurrentColor.y, vCurrentColor.z);
}




GFL_NAMESPACE_END(Shade)
GFL_NAMESPACE_END(Field)
