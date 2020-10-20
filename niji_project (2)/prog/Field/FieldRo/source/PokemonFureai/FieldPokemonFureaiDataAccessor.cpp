/**
* @brief  �|�P�����ӂꂠ���p�f�[�^�A�N�Z�b�T�[
* @file   FieldDataAccessor.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// gflib2
#include <debug/include/gfl2_DebugPrint.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include <niji_conv_header/field/field_pokemon_fureai/FieldPokemonFureaiVoiceTable.hpp>

// field
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiDataAccessor.h"





GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(PokemonFureai);


/**
* @brief �R���X�g���N�^
*
* @return ����
*/
DataAccessor::DataAccessor(void* pData)
{
  gfl2::fs::BinLinkerAccessor dataAccessor( pData );
  m_pData = dataAccessor.GetData( 0 );
  m_pHeaderData = static_cast<Header*>( m_pData );
  m_pBodyData = reinterpret_cast<POKEMON_SETTING_DATA*>( reinterpret_cast<uptr>( m_pData ) + sizeof( Header ) );
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
DataAccessor::~DataAccessor(void)
{


}

#if 0
/**
* @brief ����������
*
* @param  rInitDesc   �������ݒ�
*
* @return ����
*/
void Initialize( const InitializeDescription& rInitDesc );
/**
* @brief �I������
*
* @return ����
*/
void Terminate( void );
#endif

/**
* @brief �ݒ�f�[�^���擾
*
* @param modelId �擾����|�P�����̃��f��ID
*
* @return �ݒ�f�[�^�ւ̃|�C���^
*/
const POKEMON_SETTING_DATA* DataAccessor::GetSettingData(s32 modelId)
{
  for (u32 i = 0; i < m_pHeaderData->data_num; i++)
  {
    if (m_pBodyData[i].model_id == modelId)
    {
      return &m_pBodyData[i];
    }
  }

  
#if PM_DEBUG
  Dump();
#endif

  GFL_ASSERT( 0 );
  return NULL;
}

  /**
  * @brief �������x�����擾
  */
u32 DataAccessor::GetVoiceID(s32 modelId, s32 type)
{
  for (u32 i = 0; i < m_pHeaderData->data_num; i++)
  {
    if (VOICE_ID_TABLE[i][0] == modelId)
    {
      return VOICE_ID_TABLE[i][1+type];
    }
  }
  
  return 0;
}

#if PM_DEBUG
void DataAccessor::Dump(void)
{
  GFL_PRINT("FureaiSetting Dump data_num=%d\n", m_pHeaderData->data_num );
  for (u32 i = 0; i < m_pHeaderData->data_num; i++)
  {
    GFL_PRINT("==model_id=%d==\n", m_pBodyData[i].model_id );
    GFL_PRINT("base_stance=%d\n", m_pBodyData[i].base_stance );
    GFL_PRINT("nadenade_distance=%f\n", m_pBodyData[i].nadenade_distance );
  }

}
#endif

GFL_NAMESPACE_END(PokemonFureai);
GFL_NAMESPACE_END(Field);