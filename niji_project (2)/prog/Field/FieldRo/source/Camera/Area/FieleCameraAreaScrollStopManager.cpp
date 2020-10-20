/**
* @brief  フィールドカメラ、スクロールストップ管理
* @file   FieldCameraScrollStopManager.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// gflib
#include <debug/include/gfl2_Assert.h>

// field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaScrollStopManager.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

// バイナリデータへのアクセッサー
class ScrollStopDataAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ScrollStopDataAccessor );

public:

  struct DataType
  {
    enum Value
    {
      IN,
      OUT,

      TYPE_MAX
    };
  };

  struct BinVector
  {
    f32 x;
    f32 y;
    f32 z;
  };

  struct BinInData
  {
    f32 axisDegree;     // 回転(度)

    BinVector hitRangeData[ 4 ];    // 発動範囲
    BinVector inRangeData[ 4 ];     // clamp範囲
  };


  struct BinOutData
  {
    f32 axisDegree;     // 回転(度)

    BinVector hitRangeData[ 4 ];    // 発動範囲
  };

  // ランタイムコードとデータフォーマットを合わせる為のバージョン
  enum 
  {
    VERSION_151106 = 0,   // 機能追加時

    // 現在のデータバージョン
    LATEST_VERSION = VERSION_151106
  };


  GFL_WARNING_PUSH
    GFL_WARNING_WIN32_IGNORE(4200) //C4200警告を抑止
    // カメラエリア基本形状コリジョン配置情報ヘッダー (エリア単位）
  struct DataHeader
  {
    u32 version;          // データフォーマットのバージョン
    u32 totalNum;         // データ総数
    u32 num[ DataType::TYPE_MAX ];  // 型式別データ数
    void* pData;
  };
  GFL_WARNING_POP

public:

  /**
  *  @biref  コンストラクタ
  */
  ScrollStopDataAccessor() : m_pData( NULL )
  {
    for( u32 i = 0; i < DataType::TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = 0;
    }
  }

  /**
  *  @brief  デストラクタ
  */
  ~ScrollStopDataAccessor()
  {

  }

  /**
  *  @brief  初期化
  */
  void Initialize( const void * pData )
  {
    m_pData = reinterpret_cast<const DataHeader*>( pData );
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    GFL_ASSERT_STOP_MSG(  m_pData->version == LATEST_VERSION, "data version missing %d != %d\n", m_pData->version, LATEST_VERSION );

    // 各形状別のデータ開始アドレスを記憶したテーブルを生成
    SetupDataStartAddrTable();
  }

  /**
  *  @brief  データ総数
  */
  u32 GetTotalDataMax() const
  {
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    return m_pData->totalNum;
  }

  /**
  *  @brief  タイプデータ数を取得
  */
  u32 GetDataMax( DataType::Value type ) const
  {
    GFL_ASSERT( type < DataType::TYPE_MAX ); // 開発時に気がつくためのアサート
    return m_pData->num[ type ];
  }

  /**
  *  @brief 各形状別のデータ開始アドレスを取得
  */
  uptr GetDataStartAddr( DataType::Value type ) const
  {
    GFL_ASSERT( type < DataType::TYPE_MAX ); // 開発時に気がつくためのアサート
    return m_DataStartAddrTable[ type ];
  }

private:

  /**
  *  @brief 各形状別のデータ開始アドレスを記憶したテーブルを生成
  */
  void SetupDataStartAddrTable( void )
  {
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート
    size_t sizeTable[] =
    {
      sizeof( BinInData ),
      sizeof( BinOutData ),
    };

    uptr addr = reinterpret_cast<uptr>(&m_pData->pData);
    for( u32 i = 0; i < DataType::TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = addr;
      addr += sizeTable[ i ] * m_pData->num[ i ];
    }
  }


private:

  const DataHeader * m_pData;
  uptr  m_DataStartAddrTable[ DataType::TYPE_MAX ];
}; // class AreaDataAccessor

/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
ScrollStopManager::ScrollStopManager( const InitializeDescription& rInitDesc ) : 
m_pScrollStopDataAccessor(NULL), m_pInDataInvalidFlagTable(NULL)
{
  if (rInitDesc.m_pData)
  {
    m_pScrollStopDataAccessor = GFL_NEW(rInitDesc.m_pHeap) ScrollStopDataAccessor();
    m_pScrollStopDataAccessor->Initialize(rInitDesc.m_pData);

    u32 dataNum = m_pScrollStopDataAccessor->GetDataMax(ScrollStopDataAccessor::DataType::IN);
    if (dataNum > 0)
    {
      m_pInDataInvalidFlagTable = GFL_NEW(rInitDesc.m_pHeap) b32[ dataNum ];
      for (u32 i = 0; i < dataNum; i++)
      {
        m_pInDataInvalidFlagTable[i] = false;
      }
    }
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ScrollStopManager::~ScrollStopManager( void )
{
  GFL_SAFE_DELETE_ARRAY(m_pInDataInvalidFlagTable);
  GFL_SAFE_DELETE( m_pScrollStopDataAccessor );
}

/**
* @brief スクロールストップ処理を実行する
*
* @param  rSrc  加工前の座標
* @param  pDist 加工結果の格納先
*
* @return 実際に加工されていたらtrue
*/
b32 ScrollStopManager::Execute( const gfl2::math::Vector3& rSrc, gfl2::math::Vector3* pDist ) const
{
  s32 typeTable[] =
  {
    // 実行順
    ScrollStopDataAccessor::DataType::IN,
    ScrollStopDataAccessor::DataType::OUT,
  };

  b32 resultFlag(false);
  gfl2::math::Vector3 resultVector( rSrc );

  if( m_pScrollStopDataAccessor == NULL )
  {
    *pDist = resultVector;
    return resultFlag;
  }

  for( u32 i = 0; i < GFL_NELEMS( typeTable ); i++ )
  {
    if( ExecuteImpl( typeTable[i], resultVector, &resultVector ) )
    {
      resultFlag = true;
    }
  }

  *pDist = resultVector;
  return resultFlag;
}

/**
* @brief スクロールストップ処理を実行する
*
* @param  type  データタイプ
* @param  rSrc  加工前の座標
* @param  pDist 加工結果の格納先
*
* @return 実際に加工されていたらtrue
*/
b32 ScrollStopManager::ExecuteImpl( s32 type, const gfl2::math::Vector3& rSrc, gfl2::math::Vector3* pDist ) const
{
  b32 resultFlag(false);
  gfl2::math::Vector3 resultVector( rSrc );

  // 頂点データは下記のように格納されている
  // 23
  // 01
  
  switch( type )
  {
  case ScrollStopDataAccessor::DataType::IN:
    {


      u32 dataNum = m_pScrollStopDataAccessor->GetDataMax( ScrollStopDataAccessor::DataType::IN );
      const ScrollStopDataAccessor::BinInData* pData = reinterpret_cast< const ScrollStopDataAccessor::BinInData* >( m_pScrollStopDataAccessor->GetDataStartAddr( ScrollStopDataAccessor::DataType::IN ));
      for( u32 i = 0; i < dataNum; i++, pData++ )
      {
        // 無効化中
        if (m_pInDataInvalidFlagTable[i])
        {
          continue;
        }

        // 発動範囲チェック
        if( resultVector.x < pData->hitRangeData[0].x || resultVector.x > pData->hitRangeData[1].x 
           || resultVector.z < pData->hitRangeData[2].z || resultVector.z > pData->hitRangeData[0].z
          )
        {
          continue;
        }

        // 実行
        if( resultVector.x < pData->inRangeData[0].x )
        {
          resultVector.x = pData->inRangeData[0].x;
          resultFlag = true;
        }

        if( resultVector.x > pData->inRangeData[1].x )
        {
          resultVector.x = pData->inRangeData[1].x;
          resultFlag = true;
        }
        
        if( resultVector.z < pData->inRangeData[2].z )
        {
          resultVector.z = pData->inRangeData[2].z;
          resultFlag = true;
        }

        if( resultVector.z > pData->inRangeData[0].z )
        {
          resultVector.z = pData->inRangeData[0].z;
          resultFlag = true;
        }
      }
      break;
    }

  case ScrollStopDataAccessor::DataType::OUT:
    {
      u32 dataNum = m_pScrollStopDataAccessor->GetDataMax( ScrollStopDataAccessor::DataType::OUT );
      const ScrollStopDataAccessor::BinOutData* pData = reinterpret_cast< const ScrollStopDataAccessor::BinOutData* >( m_pScrollStopDataAccessor->GetDataStartAddr( ScrollStopDataAccessor::DataType::OUT ));
      for( u32 i = 0; i < dataNum; i++, pData++ )
      {
        // 辺情報
        u32 indexTable [] =
        {
          0, 2,
          2, 3,
          3, 1,
          1, 0
        };

        // 全ての辺の右側ならHIT
        b32 isHit( true );
        for( u32 k = 0; k < GFL_NELEMS( indexTable ); k+=2 )
        {
          u32 index1 = indexTable[ k ];
          u32 index2 = indexTable[ k + 1 ];
   
          gfl2::math::Vector2 vec1( pData->hitRangeData[ index2 ].x - pData->hitRangeData[ index1 ].x, pData->hitRangeData[ index2 ].z - pData->hitRangeData[ index1 ].z );
          gfl2::math::Vector2 vec2( resultVector.x - pData->hitRangeData[ index1 ].x,  resultVector.z - pData->hitRangeData[ index1 ].z );

          f32 work = vec1.Cross( vec2 );
          if( work < 0.0f )
          {
            isHit = false;
            break;
          }
        }

        if( isHit )
        {
          // ベースとなる辺に押し出す(射影する)
          gfl2::math::Vector2 vec1( pData->hitRangeData[1].x - pData->hitRangeData[0].x, pData->hitRangeData[1].z - pData->hitRangeData[0].z );
          gfl2::math::Vector2 vec2( resultVector.x - pData->hitRangeData[0].x,  resultVector.z - pData->hitRangeData[0].z );

          f32 value = ( vec1.Dot( vec2 ) ) / ( vec1.Length() * vec1.Length() );
          gfl2::math::Vector2 vecWork( 0.0f, 0.0f );
          vecWork.x = vec1.x * value;
          vecWork.y = vec1.y * value;

          resultVector.x = pData->hitRangeData[0].x + vecWork.x;
          resultVector.z = pData->hitRangeData[0].z + vecWork.y;
          resultFlag = true;
        }

      }
      break;
    }
  }

   *pDist = resultVector;
  return resultFlag;
}

/**
* @brief INデータの無効化フラグを設定します
*
* @param  data_index データのindex値
* @param  flag       設定するフラグ
*
* @return なし
*/
void ScrollStopManager::SetInDataInvalidFlag(u32 data_index, b32 flag)
{
  if (m_pScrollStopDataAccessor == NULL)
  {
    return;
  }

  u32 dataNum = m_pScrollStopDataAccessor->GetDataMax(ScrollStopDataAccessor::DataType::IN);
  if ( data_index < dataNum )
  {
    m_pInDataInvalidFlagTable[data_index] = flag;
  }
}

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );