//==============================================================================
/**
 * @file   LoadMeasurementData.cpp
 * @date   2016/04/04
 * @author uematsu_naoya
 * @brief  �������׌v���l�i�[�N���X
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
 * @brief �R���X�g���N�^
 */
LoadMeasurementData::LoadMeasurementData() :
  m_pModelInstanceNode(NULL),
  m_NumberOfDrawCommands(0)
{
  // ���׌v���l��S��0�ŏ�����
  for (int type = 0; type < MeasurementType::NumberOf; ++type)
  {
    for (int i = 0; i < NUMBER_OF_REGISTERED_VALUES; ++i)
    {
      m_MeasurementValues[type][i] = 0;
    }
  }
}

/**
 * @brief �f�X�g���N�^
 */
LoadMeasurementData::~LoadMeasurementData()
{
}

/**
 * @brief ���׌v���Ώۂ̃��f���f�[�^�̐ݒ�
 * @param pModel ���׌v���Ώۂ̃��f���f�[�^
 * @return �`��R�}���h�̑���(NULL��n�����ꍇ�͕K��0�ƂȂ�.)
 */
unsigned int  LoadMeasurementData::CalculateNumberOfDrawCommands(ModelInstanceNode* pModelInstanceNode)
{
  m_pModelInstanceNode = pModelInstanceNode;
  m_NumberOfDrawCommands = 0;

#if defined(GF_PLATFORM_CTR)
  if (m_pModelInstanceNode == NULL)
  {// ���f�����ݒ肳��Ă��Ȃ����0�̂܂܏I��
    return m_NumberOfDrawCommands;
  }

  // ���f���C���X�^���X�m�[�h����`��R�}���h�̑������擾
  const GfMdlResourceNode* pModelResourceNode = pModelInstanceNode->GetModelResourceNode();

  // �e���b�V����FaceData���璸�_�̕`��R�}���h�̃T�C�Y���擾
  u32 meshNum = pModelResourceNode->GetMeshNum();
  for (u32 meshNo = 0; meshNo < meshNum; ++meshNo)
  {
    // ���b�V���f�[�^���擾��, FaceData�̑������擾
    const ModelInstanceNode::MeshLinkData* pMeshLinkData = pModelInstanceNode->GetMeshLinkData(meshNo);
    if (pMeshLinkData == NULL)
    {// MeshLinkData�͂Ȃ��\��������̂Œe��
      continue;
    }

    // �eFaceData����`��R�}���h���擾
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

  // �e��}�e���A���̕`��R�}���h�̃T�C�Y���擾
  u32 materialNum = pModelInstanceNode->GetMaterialNum();
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    const MaterialInstanceNode* pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(materialNo);
    const MaterialResourceNode* pMaterialResourceNode = pMaterialInstanceNode->GetMaterialResourceNode();
    m_NumberOfDrawCommands += pMaterialResourceNode->GetMaterialCommandSize();
  }

  // �Q�ƃe�[�u���Ɋ֘A����`��R�}���h�̃T�C�Y���擾
  for (u32 materialNo = 0; materialNo < materialNum; ++materialNo)
  {
    const MaterialInstanceNode* pMaterialInstanceNode = pModelInstanceNode->GetMaterialInstanceNode(materialNo);
    for (u32 lutNo = 0; lutNo < MaterialResourceNode::LUT_MAX; ++lutNo)
    {
      if (!pMaterialInstanceNode->GetLutCommand(lutNo))
      {// �R�}���h���Ȃ���Ζ���
        continue;
      }
      m_NumberOfDrawCommands += pMaterialInstanceNode->GetLutCommandSize(lutNo);
    }
  }
#endif // GF_PLATFORM_CTR
  return m_NumberOfDrawCommands;
}

/**
 * @brief ���׌v���l�̒ǉ�
 * @param measurementType ���׌v���l�̎��(CPU/GPU)
 * @param measurementValue ���׌v���l
 * @param bool ����/���s
 */
bool LoadMeasurementData::AddMeasurementValue(MeasurementType::Enum type, unsigned int measurementValue)
{
  // �܂��擪�̃f�[�^��1�����ɂ��炷
  for (int i = 0; i < NUMBER_OF_REGISTERED_VALUES - 1; ++i)
  {
    m_MeasurementValues[type][i + 1] = m_MeasurementValues[type][i];
  }

  // �擪�ɍ���ǉ����ꂽ�f�[�^��ǉ�
  m_MeasurementValues[type][0] = measurementValue;

  return true;
}

/**
 * @brief ���׌v���l�̕��ϒl�v�Z
 * @param measurementType ���׌v���l�̎��(CPU/GPU)
 * @return unsgined int �v���l�̕��ϒl
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
 * @brief ���׌v���l���R���\�[���ɏo��
 * @return bool ����/���s
 */
bool LoadMeasurementData::OutputMeasurementToConsole()
{
  // ���f���f�[�^�̃g�b�v�W���C���g�ɂ�, "pmNNNN_FF"�̂悤�Ȗ��O������U���Ă���.
  // ���̖��O���R���\�[����ʂɏo�͂���.
  const c8* topJointName = "RefValue";
  if (m_pModelInstanceNode)
  {
    u32 jointNum = m_pModelInstanceNode->GetJointNum();
    if (jointNum > 0)
    {// �W���C���g���g�p���Ă��Ȃ����f�����l�����Ă����Œe���Ă���
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pTopJointInstanceNode = m_pModelInstanceNode->GetJointInstanceNode(0);
      topJointName = pTopJointInstanceNode->GetName();
    }
  }

  // Top�̃W���C���g��0�ԂȂ̂�, 0�Ԃ̃W���C���g�̖��O���擾. pm0327_00���𒲂ׂ�.

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