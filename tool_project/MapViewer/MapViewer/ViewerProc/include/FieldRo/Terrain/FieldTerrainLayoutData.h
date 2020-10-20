/**
* @brief  �t�B�[���h�n�`�̔z�u�ݒ�f�[�^
* @file   FieldTerrainLayoutData.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__ )
#define __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__
#pragma once

// @note �킾���p�ӂ������

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>

// �O���錾
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

static const u16 DATA_ID_INVALID_VALUE = 0xFFFF;

/**
* @brief �t�B�[���h�n�`�̔z�u�ݒ�f�[�^
*/
class TerrainLayoutData
{
  /** @brief  �R�s�[�֎~�N���X�ɂ��� */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainLayoutData );

public:


  struct BLOCK_DATA
  {
    u16 highModelDataId;
    u16 middleModelDataId;
    u16 lowModelDataId;
  };

  // �f�[�^�w�b�_�[
  struct BIN_DATA_HEADER
  {
    u32 blockNumOfXAxis;
    u32 blockNumOfZAxis;
    u32 blockSize;          // �u���b�N�T�C�Y
    b32 planFlag;           // �v�����i�[�݌v���}�b�v���H
    BLOCK_DATA blockData;
    char data;
  };

  /**
  * @brief �R���X�g���N�^
  *
  * @return ����
  */ 
  TerrainLayoutData( void ) :
    m_BlockStartPosition( 0.0f ,0.0f, 0.0f )
    ,m_BlockNumOfXAxis( 0 )       // X���u���b�N��
    ,m_BlockNumOfZAxis( 0 )      // Z���u���b�N��
    ,m_BlockSize( 0.0f )         // 1�u���b�N�傫��
    ,m_ppBlockDataTable( NULL )
    ,m_pData( NULL )
  {
  }

  /**
  * @brief �f�X�g���N�^
  *
  * @return ����
  */
  virtual ~TerrainLayoutData( void )
  {
    Delete();
  }

  /**
  * @brief ����������
  *
  * @return ����
  */
  void Initialize( void )
  {
  }

  /**
  * @brief �I������
  *
  * @return ����
  */
  void Terminate( void )
  {
  }

  /**
  * @brief �w��u���b�N�ԍ���High���f���̃f�[�^�ԍ����擾����
  *
  * @param  x      x���ԍ�
  * @param  z      z���ԍ�
  *
  * @return ����
  */
  u32 GetHighModelDataIndex( u32 x, u32 z ) const 
  {
    if( x >= m_BlockNumOfXAxis || z >= m_BlockNumOfZAxis )
    {
      // @todo
      return 0;
    }
    
    return m_ppBlockDataTable[z][x].highModelDataId;
  }

  /**
  * @brief �w��u���b�N�ԍ���Middle���f���̃f�[�^�ԍ����擾����
  *
  * @param  x      x���ԍ�
  * @param  z      z���ԍ�
  *
  * @return ����
  */
  u32 GetMiddleModelDataIndex( u32 x, u32 z ) const 
  {
    if( x >= m_BlockNumOfXAxis || z >= m_BlockNumOfZAxis )
    {
      // @todo
      return 0;
    }

    return m_ppBlockDataTable[z][x].middleModelDataId;
  }

  void GetBlockStartPosition( gfl2::math::Vector3* pOut ) const
  {
    *pOut = m_BlockStartPosition;
  }

  u32 GetBlockNumOfXAxis( void ) const
  {
    return m_BlockNumOfXAxis;
  }

  u32 GetBlockNumOfZAxis( void ) const
  {
    return m_BlockNumOfZAxis;
  }

  f32 GetBlockSize( void ) const
  {
    return m_BlockSize;
  }

  // �z�u�f�[�^�����킩�`�F�b�N
  b32 IsValid( void ) const
  {
    if( m_pData == NULL || m_BlockNumOfXAxis <= 0 || m_BlockNumOfZAxis <= 0 || m_BlockSize <= 0.0f )
    {
      return false;
    }

    return true;
  }

  b32 IsPlanMode( void ) const
  {
    return m_pData->planFlag;
  }

  void Test( void )
  {
    m_BlockStartPosition.x = -3200.0f;
    m_BlockStartPosition.z = -3200.0f;
    m_BlockNumOfXAxis = 30;
    m_BlockNumOfZAxis = 30;
    m_BlockSize = 3200.0f;
  };

  void Create( const void* pData, gfl2::heap::HeapBase* pHeap )
  {
    m_pData = static_cast<const BIN_DATA_HEADER*>( pData );
    m_BlockNumOfXAxis = m_pData->blockNumOfXAxis;
    m_BlockNumOfZAxis = m_pData->blockNumOfZAxis;
    m_BlockSize = m_pData->blockSize;

    // �A�N�Z�X���₷���悤�Ƀ|�C���^���e�[�u���ŕێ����� [z][x]
    const BLOCK_DATA* pBlockData = static_cast<const BLOCK_DATA*>( &( m_pData->blockData ) );
    m_ppBlockDataTable = GFL_NEW_ARRAY( pHeap ) const BLOCK_DATA* [ m_BlockNumOfZAxis ];
    for( u32 z = 0; z < m_BlockNumOfZAxis; z++ )
    {
      m_ppBlockDataTable[z] = &pBlockData[ m_BlockNumOfXAxis * z ];
    }

    Dump();
  }

  void Delete( void )
  {
    // �z�u�f�[�^�̒��g�͏�w�̕��A�e�[�u�������폜
    if( m_ppBlockDataTable )
    {
      GFL_SAFE_DELETE_ARRAY( m_ppBlockDataTable );
    }

    m_BlockNumOfZAxis = 0;
    m_BlockNumOfXAxis = 0;
    m_BlockSize = 0.0f;
    m_pData = NULL;
  }
  
  void Dump( void ) const
  {
    GFL_RELEASE_PRINT("\n");
    GFL_RELEASE_PRINT( "-- Terrain Layout Data Dump START -- \n");
    GFL_RELEASE_PRINT( "num of z=%d x=%d\n", m_BlockNumOfZAxis, m_BlockNumOfXAxis );
    GFL_RELEASE_PRINT( "block size =%f\n", m_BlockSize );
    GFL_RELEASE_PRINT( "block start pos x=%f y=%f z=%f\n", m_BlockStartPosition.x, m_BlockStartPosition.y, m_BlockStartPosition.z );
    

    GFL_RELEASE_PRINT( "highModelDataId \n");
    for( u32 z = 0; z < m_BlockNumOfZAxis; z++ )
    {
      const BLOCK_DATA* pBlockData = m_ppBlockDataTable[z];
      for( u32 x = 0; x < m_BlockNumOfXAxis; x++, pBlockData++ )
      {
        GFL_RELEASE_PRINT("%4d ", pBlockData->highModelDataId );
      }
      GFL_RELEASE_PRINT("\n");
    }

    GFL_RELEASE_PRINT( "middleModelDataId \n");
    for( u32 z = 0; z < m_BlockNumOfZAxis; z++ )
    {
      const BLOCK_DATA* pBlockData = m_ppBlockDataTable[z];
      for( u32 x = 0; x < m_BlockNumOfXAxis; x++, pBlockData++ )
      {
        GFL_RELEASE_PRINT("%4d ", pBlockData->middleModelDataId );
      }
      GFL_RELEASE_PRINT("\n");
    }

    GFL_RELEASE_PRINT( "lowModelDataId \n");
    for( u32 z = 0; z < m_BlockNumOfZAxis; z++ )
    {
      const BLOCK_DATA* pBlockData = m_ppBlockDataTable[z];
      for( u32 x = 0; x < m_BlockNumOfXAxis; x++, pBlockData++ )
      {
        GFL_RELEASE_PRINT("%4d ", pBlockData->lowModelDataId );
      }
      GFL_RELEASE_PRINT("\n");
    }

    GFL_RELEASE_PRINT( "-- Terrain Layout Data Dump END -- \n\n");
  }

  b32 IsExist( s32 x, s32 z ) const
  {
    if( 
      x < 0 || x >= GetBlockNumOfXAxis() || 
      z < 0 || z >= GetBlockNumOfZAxis()
      )
    {
      return false;
    }

    const BLOCK_DATA* pData = &m_ppBlockDataTable[z][x];
    if( pData->middleModelDataId != DATA_ID_INVALID_VALUE )
    {
      return true;
    }
    
    if( pData->highModelDataId != DATA_ID_INVALID_VALUE )
    {
      return true;
    }

    if( pData->lowModelDataId != DATA_ID_INVALID_VALUE )
    {
      return true;
    }

    // �o�^����Ă��Ȃ�
    return false;
  }

private:
  
  gfl2::math::Vector3 m_BlockStartPosition;
  u32 m_BlockNumOfXAxis;        // X���u���b�N��
  u32 m_BlockNumOfZAxis;        // Z���u���b�N��
  f32 m_BlockSize;              // 1�u���b�N�傫��
  const BLOCK_DATA**      m_ppBlockDataTable; // [m_BlockNumOfZAxis][m_BlockNumOfXAxis]
  const BIN_DATA_HEADER*  m_pData;       

};  // class TerrainLayoutData


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__