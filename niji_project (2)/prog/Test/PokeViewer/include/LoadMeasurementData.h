//==============================================================================
/**
 * @file   LoadMeasurementData.h
 * @date   2016/04/04
 * @author uematsu_naoya
 * @brief  �������׌v���l�i�[�N���X
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
   * @brief ���׌v���̎��
   */
  struct MeasurementType
  {
    enum Enum
    {
      UPDATE_MEASUREMENT,   //!< @brief Update����
      DRAW_MEASUREMENT,     //!< @brief Draw����
      GPU_MEASUREMENT,      //!< @brief GPU����
      NumberOf              //!< @brief ����
    };
  };

  /**
   * @brief �R���X�g���N�^
   */
  LoadMeasurementData();

  /**
   * @brief �f�X�g���N�^
   */
  ~LoadMeasurementData();

  /**
   * @brief ���׌v���Ώۂ̃��f���f�[�^�̐ݒ�
   * @param pModel ���׌v���Ώۂ̃��f���f�[�^
   * @return �`��R�}���h�̑���
   */
  unsigned int CalculateNumberOfDrawCommands(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode);

  /**
   * @brief ���׌v���l�̒ǉ�
   * @param measurementType ���׌v���l�̎��(CPU/GPU)
   * @param measurementValue ���׌v���l
   * @param bool ����/���s
   */
  bool AddMeasurementValue(MeasurementType::Enum type, unsigned int measurementValue);

  /**
   * @brief ���׌v���l�̕��ϒl�v�Z
   * @param measurementType ���׌v���l�̎��(CPU/GPU)
   * @return unsgined int �v���l�̕��ϒl
   */
  unsigned int CalculateAverageMeasurement(MeasurementType::Enum type);

  /**
   * @brief ���׌v���l���R���\�[���ɏo��
   * @return bool ����/���s
   */
  bool OutputMeasurementToConsole();

private:
  //!< @brief �o�^�ł���f�[�^�̑���(300�t���[��. �����sango�̍ۂ̕��׌v���l�Ɠ���.)
  static const unsigned int NUMBER_OF_REGISTERED_VALUES = 300;

  //!< @brief �o�^���ꂽ�v���f�[�^
  unsigned int m_MeasurementValues[MeasurementType::NumberOf][NUMBER_OF_REGISTERED_VALUES];

  //!< @brief ���׌v���Ώۂ̃��f���C���X�^���X�m�[�h
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;

  //!< @brief ���׌v���Ώۂ̃��f���̃R�}���h����
  unsigned int m_NumberOfDrawCommands;
};

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif

#endif // PM_DEBUG