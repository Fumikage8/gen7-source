//==============================================================================
/**
 * @file   LoadMeasurementData.cpp
 * @date   2016/04/04
 * @author uematsu_naoya
 * @brief  処理負荷計測値格納クラス
 * (C)1995-2016 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#if PM_DEBUG

#include "../include/LoadMeasurementData.h"
#include <debug/include/gfl2_DebugWinItemTypes.h>

using namespace gfl2::renderingengine::scenegraph::resource;
using namespace gfl2::renderingengine::scenegraph::instance;

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @brief コンストラクタ
 */
LoadMeasurementData::LoadMeasurementData() :
  m_pModelInstanceNode(NULL),
  m_NumberOfDrawCommands(0)
{
  // 負荷計測値を全て0で初期化
  for (int type = 0; type < MeasurementType::NumberOf; ++type)
  {
    for (int i = 0; i < NUMBER_OF_REGISTERED_VALUES; ++i)
    {
      m_MeasurementValues[type][i] = 0;
    }
  }
}

/**
 * @brief デストラクタ
 */
LoadMeasurementData::~LoadMeasurementData()
{
}

/**
 * @brief 負荷計測対象のモデルデータの設定
 * @param pModel 負荷計測対象のモデルデータ
 * @return 描画コマンドの総数(NULLを渡した場合は必ず0となる.)
 */
unsigned int  LoadMeasurementData::CalculateNumberOfDrawCommands(ModelInstanceNode* pModelInstanceNode)
{
  m_pModelInstanceNode = pModelInstanceNode;
  m_NumberOfDrawCommands = 0;

#if defined(GF_PLATFORM_CTR)
  if (m_pModelInstanceNode == NULL)
  {// モデルが設定されていなければ0のまま終了
    return m_NumberOfDrawCommands;
  }

  // モデルインスタンスノードから描画コマンドの総数を取得
  const GfMdlResourceNode* pModelResourceNode = pModelInstanceNode->GetModelResourceNode();

  // 各メッシュのFaceDataから頂点の描画コマンドのサイズを取得
  u32 meshNum = pModelResourceNode->GetMeshNum();
  for (u32 meshNo = 0; meshNo < meshNum; ++meshNo)
  {
    // メッシュデータを取得し, FaceDataの総数を取得
    const ModelInstanceNode::MeshLinkData* pMeshLinkData = pModelInstanceNode->GetMeshLinkData(meshNo);
    if (pMeshLinkData == NULL)
    {// MeshLinkDataはない可能性もあるので弾く
      continue;
    }

    // 各FaceDataから描画コマンドを取得
    u32 faceDataNum = pMeshLinkData->GetFaceNum();
    for (u32 faceDataNo = 0; faceDataNo < faceDataNum; ++faceDataNo)
    {
      const MeshResourceNode::FaceData* pFaceData = pMeshLinkData->GetFaceData(faceDataNo);
      for (u32 commandTypeNum = 0; commandTypeNum < MeshResourceNode::FaceData::CommandType::Max; ++commandTypeNum)
      {
        m_NumberOfDrawCommands += pFaceData->GetCommandSize(commandTypeNum);
      }
    }
  }

  // 各種マテリアルの描画コマンドのサイズを取得
  u32 materialNum = pModelInstanceNode->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    const MaterialInstanceNode* pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(materialNo);
    const MaterialResourceNode* pMaterialResourceNode = pMaterialInstanceNode->GetMaterialResourceNode();
    m_NumberOfDrawCommands += pMaterialResourceNode->GetMaterialCommandSize();
  }

  // 参照テーブルに関連する描画コマンドのサイズを取得
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    const MaterialInstanceNode* pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(materialNo);
    for (u32 lutNo = 0; lutNo < MaterialResourceNode::LUT_MAX; ++lutNo)
    {
      if (!pMaterialInstanceNode->GetLutCommand(lutNo))
      {// コマンドがなければ無視
        continue;
      }
      m_NumberOfDrawCommands += pMaterialInstanceNode->GetLutCommandSize(lutNo);
    }
  }
#endif // GF_PLATFORM_CTR
  return m_NumberOfDrawCommands;
}

/**
 * @brief 負荷計測値の追加
 * @param measurementType 負荷計測値の種類(CPU/GPU)
 * @param measurementValue 負荷計測値
 * @param bool 成功/失敗
 */
bool LoadMeasurementData::AddMeasurementValue(MeasurementType::Enum type, unsigned int measurementValue)
{
  // まず先頭のデータを1つずつ後ろにずらす
  for (int i = 0; i < NUMBER_OF_REGISTERED_VALUES - 1; ++i)
  {
    m_MeasurementValues[type][i + 1] = m_MeasurementValues[type][i];
  }

  // 先頭に今回追加されたデータを追加
  m_MeasurementValues[type][0] = measurementValue;

  return true;
}

/**
 * @brief 負荷計測値の平均値計算
 * @param measurementType 負荷計測値の種類(CPU/GPU)
 * @return unsgined int 計測値の平均値
 */
unsigned int LoadMeasurementData::CalculateAverageMeasurement(MeasurementType::Enum type)
{
  unsigned int averageValue = 0;
  for (int i = 0; i < NUMBER_OF_REGISTERED_VALUES; ++i)
  {
    averageValue += m_MeasurementValues[type][i];
  }
  return averageValue / NUMBER_OF_REGISTERED_VALUES;
}

/**
 * @brief 負荷計測値をコンソールに出力
 * @return bool 成功/失敗
 */
bool LoadMeasurementData::OutputMeasurementToConsole()
{
  // モデルデータのトップジョイントには, "pmNNNN_FF"のような名前が割り振られている.
  // その名前をコンソール画面に出力する.
  const c8* topJointName = "RefValue";
  if (m_pModelInstanceNode)
  {
    u32 jointNum = m_pModelInstanceNode->GetJointNum();
    if (jointNum > 0)
    {// ジョイントを使用していないモデルを考慮してここで弾いておく
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pTopJointInstanceNode = m_pModelInstanceNode->GetJointInstanceNode(0);
      topJointName = pTopJointInstanceNode->GetName();
    }
  }

  // Topのジョイントは0番なので, 0番のジョイントの名前を取得. pm0327_00かを調べる.

  GFL_RELEASE_PRINT("TopNodeName = %s, Update = %d, Draw = %d, GPU = %d, Command = %d\n",
    topJointName,
    CalculateAverageMeasurement(MeasurementType::UPDATE_MEASUREMENT),
    CalculateAverageMeasurement(MeasurementType::DRAW_MEASUREMENT),
    CalculateAverageMeasurement(MeasurementType::GPU_MEASUREMENT),
    m_NumberOfDrawCommands);

  return true;
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG