/**
* @brief  �t�B�[���h�̉��i1���N���X
* @file   TerrainDistantView.h
* @author ikuta_junya
* @data   2015.04.19
*/

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainDistantView.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );


/**
* @brief �R���X�g���N�^
*
* @return ����
*/ 
TerrainDistantView::TerrainDistantView( void ) :
  // ����
  m_DistantViewNo( 0 )
  // �I�u�W�F�N�g
  ,m_pModel( NULL )
{
}

/**
* @brief �f�X�g���N�^
*
* @return ����
*/
TerrainDistantView::~TerrainDistantView( void )
{
  Terminate();
}

/**
* @brief ����������
*
* @param  rInitDesc   �������ݒ�
*
* @return ����
*/
void TerrainDistantView::Initialize( const InitializeDescription& rInitDesc )
{
  m_DistantViewNo = rInitDesc.m_DistantViewNo;
  m_pModel = rInitDesc.m_pTerrainFactory->CreateDistantViewModel( m_DistantViewNo );
}

/**
* @brief �I������
*
* @return ����
*/
void TerrainDistantView::Terminate( void )
{
  GFL_SAFE_DELETE( m_pModel );
  m_DistantViewNo = 0;
}

/**
* @brief �X�V����
*
* @return ����
*/
void TerrainDistantView::Update( void )
{
}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );