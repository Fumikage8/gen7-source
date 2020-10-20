//==============================================================================
/**
 * @file   LoadMeasurementData.h
 * @date   2016/04/04
 * @author uematsu_naoya
 * @brief  処理負荷計測値格納クラス
 * (C)1995-2016 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================
#if PM_DEBUG

#if !defined(LOADMEASUREMENTDATA_H_INCLUDED)
#define LOADMEASUREMENTDATA_H_INCLUDED
#pragma once

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

class LoadMeasurementData
{
public:
  /**
   * @brief 負荷計測の種類
   */
  struct MeasurementType
  {
    enum Enum
    {
      UPDATE_MEASUREMENT,   //!< @brief Update負荷
      DRAW_MEASUREMENT,     //!< @brief Draw負荷
      GPU_MEASUREMENT,      //!< @brief GPU負荷
      NumberOf              //!< @brief 総数
    };
  };

  /**
   * @brief コンストラクタ
   */
  LoadMeasurementData();

  /**
   * @brief デストラクタ
   */
  ~LoadMeasurementData();

  /**
   * @brief 負荷計測対象のモデルデータの設定
   * @param pModel 負荷計測対象のモデルデータ
   * @return 描画コマンドの総数
   */
  unsigned int CalculateNumberOfDrawCommands(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);

  /**
   * @brief 負荷計測値の追加
   * @param measurementType 負荷計測値の種類(CPU/GPU)
   * @param measurementValue 負荷計測値
   * @param bool 成功/失敗
   */
  bool AddMeasurementValue(MeasurementType::Enum type, unsigned int measurementValue);

  /**
   * @brief 負荷計測値の平均値計算
   * @param measurementType 負荷計測値の種類(CPU/GPU)
   * @return unsgined int 計測値の平均値
   */
  unsigned int CalculateAverageMeasurement(MeasurementType::Enum type);

  /**
   * @brief 負荷計測値をコンソールに出力
   * @return bool 成功/失敗
   */
  bool OutputMeasurementToConsole();

private:
  //!< @brief 登録できるデータの総数(300フレーム. これはsangoの際の負荷計測値と同じ.)
  static const unsigned int NUMBER_OF_REGISTERED_VALUES = 300;

  //!< @brief 登録された計測データ
  unsigned int m_MeasurementValues[MeasurementType::NumberOf][NUMBER_OF_REGISTERED_VALUES];

  //!< @brief 負荷計測対象のモデルインスタンスノード
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  //!< @brief 負荷計測対象のモデルのコマンド総数
  unsigned int m_NumberOfDrawCommands;
};

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif

#endif // PM_DEBUG