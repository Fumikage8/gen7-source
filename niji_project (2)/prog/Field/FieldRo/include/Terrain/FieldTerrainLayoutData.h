/**
* @brief  フィールド地形の配置設定データ
* @file   FieldTerrainLayoutData.h
* @author ikuta_junya
* @data   2015.04.19
*/

#if !defined( __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__ )
#define __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__
#pragma once

// @note 器だけ用意した状態

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

static const u16 DATA_ID_INVALID_VALUE = 0xFFFF;
static const f32 PLAN_BLOCK_SIZE = 4200.0f * 20.0f;

/**
* @brief フィールド地形の配置設定データ
*/
class TerrainLayoutData
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TerrainLayoutData );

public:

  // 地形の状態識別子
  struct TerrainMode
  {
    enum Value
    {
      FIX = 0,
      PLAN,
      PLAN_ONLY_WIN32,
      MAX
    };
  };

  struct BLOCK_DATA
  {
    u16 highModelDataId;
    u16 middleModelDataId;
    u16 lowModelDataId;
    u16 pad;
    s32 replaceWork;
  };

  struct BLOCK_REPLACE_DATA
  {
    u16 x;
    u16 z;
    u16 replaceNo;
    u16 highModelDataId;
    u16 middleModelDataId;
    u16 lowModelDataId;
  };

  // データヘッダー
  struct BIN_DATA_HEADER
  {
    u32 blockNumOfXAxis;
    u32 blockNumOfZAxis;
    u32 blockSize;          // ブロックサイズ
    u32 terrainMode; 
    u32 replaceDataNum;
    BLOCK_DATA blockData;
    char data;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TerrainLayoutData( void ) :
    m_BlockStartPosition( 0.0f ,0.0f, 0.0f )
    ,m_BlockNumOfXAxis( 0 )       // X軸ブロック数
    ,m_BlockNumOfZAxis( 0 )      // Z軸ブロック数
    ,m_BlockSize( 0.0f )         // 1ブロック大きさ
    ,m_ppBlockDataTable( NULL )
    ,m_pData( NULL )
    ,m_pReplaceDataStart( NULL )
  {
  }

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TerrainLayoutData( void )
  {
    Delete();
  }

  /**
  * @brief 初期化処理
  *
  * @return 無し
  */
  void Initialize( void )
  {
  }

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void )
  {
  }

  /**
  * @brief 指定ブロック番号のHighモデルのデータ番号を取得する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 無し
  */
  u32 GetHighModelDataIndex( u32 x, u32 z, u32 replaceNo = 0 ) const;

  /**
  * @brief 指定ブロック番号のMiddleモデルのデータ番号を取得する
  *
  * @param  x      x軸番号
  * @param  z      z軸番号
  *
  * @return 無し
  */
  u32 GetMiddleModelDataIndex( u32 x, u32 z, u32 replaceNo = 0 ) const;


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
    if( IsPlanMode() )
    {
      return PLAN_BLOCK_SIZE;
    }

    return m_BlockSize;
  }

  // 配置データが正常かチェック
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
    if( m_pData->terrainMode == TerrainMode::PLAN || m_pData->terrainMode == TerrainMode::PLAN_ONLY_WIN32 )
    {
      return true;
    }

    return false;
  }

  void Test( void )
  {
    m_BlockStartPosition.x = -3200.0f;
    m_BlockStartPosition.z = -3200.0f;
    m_BlockNumOfXAxis = 30;
    m_BlockNumOfZAxis = 30;
    m_BlockSize = 3200.0f;
  };

  void Create( const void* pData, u32 dataSize, gfl2::heap::HeapBase* pHeap );

  void Delete( void )
  {
    // 配置データの中身は上層の物、テーブルだけ削除
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
      x < 0 || x >= static_cast<s32>(GetBlockNumOfXAxis()) || 
      z < 0 || z >= static_cast<s32>(GetBlockNumOfZAxis())
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

    // 登録されていない
    return false;
  }

  u16 GetReplaceNo( s32 x, s32 z ) const;


private:
  
  b32 SearchReplaceData( s32 x, s32 z, u16 replaceNo, BLOCK_REPLACE_DATA* pOut ) const;
  
  gfl2::math::Vector3 m_BlockStartPosition;
  u32 m_BlockNumOfXAxis;        // X軸ブロック数
  u32 m_BlockNumOfZAxis;        // Z軸ブロック数
  f32 m_BlockSize;              // 1ブロック大きさ
  const BLOCK_DATA**      m_ppBlockDataTable; // [m_BlockNumOfZAxis][m_BlockNumOfXAxis]
  const BIN_DATA_HEADER*  m_pData;       
  const BLOCK_REPLACE_DATA* m_pReplaceDataStart;
};  // class TerrainLayoutData


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );

#endif // __FIELDTERRAINLAYOUTDATAER_H_INCLUDED__