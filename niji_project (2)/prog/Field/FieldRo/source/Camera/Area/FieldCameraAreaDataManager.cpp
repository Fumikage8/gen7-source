/**
* @brief  フィールドカメラ、カメラエリアデータ管理
* @file   FieldCameraAreaDataManager.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// field
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

// work参照
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/EventWork.h"

// momiji例外処理、特定のゾーンのデフォルトカメラをネクロズマ専用カメラに置き換え
#include "niji_conv_header/field/zone/zone_id.h"
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeNecrozma.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

struct REPLACE_DATA
{
  u32 oldDataNo;
  u32 nowDataNo;
  u32 newDataNo;
};

// カメラエリアデータへのアクセッサー
class AreaDataAccessor
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( AreaDataAccessor );

public:

  // ランタイムコードとデータフォーマットを合わせる為のバージョン
  enum 
  {
    VERSION_151022 = 0,   // 機能追加時
    VERSION_151113,       // ワーク参照機能追加

    // 現在のデータバージョン
    LATEST_VERSION = VERSION_151113
  };

  struct BinFollow
  {
    CommonData commonData;
    Follow     data;
  };

  struct BinFollowTrans
  {
    CommonData  commonData;
    FollowTrans data;
  };

  struct BinHold
  {
    CommonData commonData;
    Hold       data;
  };


  GFL_WARNING_PUSH
    GFL_WARNING_WIN32_IGNORE(4200) //C4200警告を抑止
    // カメラエリア基本形状コリジョン配置情報ヘッダー (エリア単位）
  struct DataHeader
  {
    u32 version;          // データフォーマットのバージョン
    u32 totalNum;         // データ総数
    u32 num[ DataType::TYPE_MAX ];  // 型式別データ数
    u32 repDataNum;       // 置き換えデータ
    void* pData;
  };
  GFL_WARNING_POP

public:

  /**
  *  @biref  コンストラクタ
  */
  AreaDataAccessor() : m_pData( NULL ), m_PlaceDataStartAddr( 0 )
  {
    for( u32 i = 0; i < DataType::TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = 0;
    }
  }

  /**
  *  @brief  デストラクタ
  */
  ~AreaDataAccessor()
  {

  }

  /**
  *  @brief  初期化
  */
  void Initialize( const void * pData )
  {
    m_pData = reinterpret_cast<const DataHeader*>( pData );
    GFL_ASSERT( m_pData ); // 開発時に気がつくためのアサート

    if( m_pData->version != LATEST_VERSION )
    {
      m_pData = NULL;
      return;
    }

    GFL_ASSERT_STOP_MSG(  m_pData->version == LATEST_VERSION, "data version missing %d != %d\n", m_pData->version, LATEST_VERSION );

    // 各形状別のデータ開始アドレスを記憶したテーブルを生成
    SetupDataStartAddrTable();
  }

  b32 IsValid( void ) const
  {
    return ( m_pData != NULL );
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
  *  @brief 指定番号のエリアデータを取得
  */
  b32 GetData( u32 dataNo, AreaData* p_out ) const
  {
    if( m_pData == NULL || dataNo >= m_pData->totalNum )
    {
      return false;
    }

    u32 offset = 0;
    for( u32 i = 0; i < DataType::TYPE_MAX; i++ )
    {
      if( dataNo < offset + m_pData->num[i] )
      {
        u32 loaclIndex = dataNo - offset;
        uptr addr = GetDataStartAddr( static_cast<DataType::Value>( i ) );

        switch( i )
        {
        case DataType::FOLLOW:
          {
            BinFollow* p_data = reinterpret_cast<BinFollow*>(addr);
            p_data += loaclIndex;

            p_out->commonData = p_data->commonData;
            p_out->follow = p_data->data;
            break;
          }
        case DataType::FOLLOW_TRANS:
          {
            BinFollowTrans* p_data = reinterpret_cast<BinFollowTrans*>(addr);
            p_data += loaclIndex;
            p_out->commonData = p_data->commonData;
            p_out->followTrans = p_data->data;
            break;
          }
        case DataType::HOLD:
          {
            BinHold* p_data = reinterpret_cast<BinHold*>(addr);
            p_data += loaclIndex;
            p_out->commonData = p_data->commonData;
            p_out->hold = p_data->data;
            break;
          }
        default:
          {
            return false;
          }
        } // switch

        p_out->isDefault = false;
        p_out->isReplace = false;

        return true;
      }

      offset += m_pData->num[i];
    }

    return false;
  }


  /**
  *  @brief 指定番号のエリアデータを取得
  */
  b32 RePlaceDataNo( u32 oldNo, u32 nowNo, u32* pNewNo ) const
  {
    if( m_pData )
    {
      const REPLACE_DATA* pData = reinterpret_cast< const REPLACE_DATA* >( m_PlaceDataStartAddr );
      for( u32 i = 0; i < m_pData->repDataNum; i++, pData++ )
      {
        // 置き換え実行
        if( pData->oldDataNo == oldNo && pData->nowDataNo == nowNo )
        {
          *pNewNo = pData->newDataNo;
          return true;
        }
      }
    }

    // 置き換えなし
    *pNewNo = nowNo;
    return false;
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
      sizeof( BinFollow ),
      sizeof( BinFollowTrans ),
      sizeof( BinHold ),
      sizeof( BinHold )
    };

    uptr addr = reinterpret_cast<uptr>(&m_pData->pData);
    for( u32 i = 0; i < DataType::TYPE_MAX; i++ )
    {
      m_DataStartAddrTable[ i ] = addr;
      addr += sizeTable[ i ] * m_pData->num[ i ];
    }

    m_PlaceDataStartAddr = addr;
  }


private:

  const DataHeader * m_pData;
  uptr  m_DataStartAddrTable[ DataType::TYPE_MAX ];
  uptr  m_PlaceDataStartAddr;
}; // class AreaDataAccessor


// サポートデータ内のデフォルト値設定を解決する
void ResolveDefaultData( BIN_SUPPORT_DATA* pOut, const BIN_SUPPORT_DATA& rDefaultData )
{
  // サポート設定
  // ゾーン設定をそのまま使用
  if( pOut->zoneDefaultFlag )
  {
    *pOut = rDefaultData;
    pOut->zoneDefaultFlag = true;
  }
  else
  {
    // 共通のパラメータだけゾーン設定を使用
    if( pOut->defaultParamFlag )
    {
      pOut->maxValue = rDefaultData.maxValue;
      pOut->inData = rDefaultData.inData;
      pOut->outData= rDefaultData.outData;
    }
  }
}


/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
AreaDataManager::AreaDataManager( const InitializeDescription& rInitDesc ) 
  : 
  m_pZoneData( NULL ),
  m_pZoneDefaultDataAccessor( NULL ),
  m_pAreaDataAccessor( NULL ),
  m_pZoneDefaultSupportDataAccessor( NULL )
{
  m_pAreaDataAccessor = GFL_NEW( rInitDesc.m_pHeap ) AreaDataAccessor();
  m_pAreaDataAccessor->Initialize( rInitDesc.m_pAreaData );

  m_pZoneDefaultDataAccessor = GFL_NEW( rInitDesc.m_pHeap ) AreaDataAccessor();
  m_pZoneDefaultDataAccessor->Initialize( rInitDesc.m_pZoneDefaultData );

  m_pZoneDefaultSupportDataAccessor = GFL_NEW( rInitDesc.m_pHeap ) AreaDataAccessor();
  m_pZoneDefaultSupportDataAccessor->Initialize( rInitDesc.m_pZoneDefaultSupportData );
  
  m_pZoneData = rInitDesc.m_pZoneData;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
AreaDataManager::~AreaDataManager( void )
{
  GFL_SAFE_DELETE( m_pAreaDataAccessor );
  GFL_SAFE_DELETE( m_pZoneDefaultDataAccessor );
  GFL_SAFE_DELETE( m_pZoneDefaultSupportDataAccessor );
}

/**
* @brief エリアのデフォルト設定を取得(ゾーンのデフォルト設定が不正だった際のケア用)
*
* @param  pOut  格納先
*
* @return なし
*/
void AreaDataManager::GetDefaultData( AreaData* pOut ) const
{
  pOut->isDefault = true;
  pOut->isReplace = false;

  pOut->commonData.type = Type::FOLLOW;
  pOut->commonData.interpolationFrame = 5;
  pOut->commonData.priority = 0;
  pOut->follow.pointData.distance = 776.0f;
  pOut->follow.pointData.defaultFlag = false;
  pOut->follow.pointData.fovY = 35.0f;
  pOut->follow.pointData.offset[ 0 ] = 0.0f;
  pOut->follow.pointData.offset[ 1 ] = -60.0f;
  pOut->follow.pointData.offset[ 2 ] = 0.0f;

  pOut->follow.pointData.rotation[ 0 ] = -28.0f;
  pOut->follow.pointData.rotation[ 1 ] = 0.0f;
  pOut->follow.pointData.rotation[ 2 ] = 0.0f;
}

/**
* @brief デフォルト設定をゾーンIDから取得
*
* @param  zoneId    ゾーンID
* @param  pOut      格納先
*
* @return 成否( ゾーンID範囲外の場合、失敗 )
*/
b32 AreaDataManager::GetDefaultDataByZoneId( u32 zoneId, AreaData* pOut ) const
{
  if( zoneId >= m_pZoneData->GetZoneNum() )
  {
    return false;
  }

  // momiji例外処理、特定のゾーンのデフォルトカメラをネクロズマ専用カメラに置き換え
  if (zoneId == Z_F0701PH0203)
  {
    pOut->isDefault = true;
    pOut->isReplace = false;
    pOut->commonData.type = Type::NECROZMA;
    pOut->commonData.interpolationFrame = Area::SubController::SubControllerTypeNecrozma::GetInterpolationFrame();
    pOut->commonData.priority = 0;
    return true;
  }

  // カメラエリア設定
  ZoneDataAccessor zoneDataAccessor( m_pZoneData->GetZoneData( zoneId ) );
  u32 dataIndex = zoneDataAccessor.GetCameraDefaultIndex();
  if( GetDefaultDataByIndex( dataIndex, pOut ) == false )
  {
    return false;
  }

  // サポート設定
  dataIndex = zoneDataAccessor.GetCameraSupportDefaultIndex();
  return GetDefaultSupportDataByIndex( dataIndex, &pOut->follow.supportData );
}

/**
* @brief 指定エリアデータ番号のパラメータを取得
* @note  前回のエリアデータ番号によってパラメータが変わる事がある(カメラエリア継承機能)
*
* @param  pOut            格納先
* @param  zoneId          ゾーンID
* @param  areaDataNo      エリアデータ番号
* @param  prevAreaDataNo  前回のエリアデータ番号( 初めてなら無効値を指定 )
*
* @return 成否( データ番号が無効なら失敗 )
*/
b32 AreaDataManager::GetData( AreaData* pOut, u32 zoneId, u32 areaDataNo, u32 prevAreaDataNo ) const
{
  if( m_pAreaDataAccessor->IsValid() == false )
  {
    return false;
  }

  // 置き換えチェック
  u32 implDataNo( 0 );
  b32 replaceFlag = m_pAreaDataAccessor->RePlaceDataNo( prevAreaDataNo, areaDataNo, &implDataNo );

  // ゾーンのデフォルト値を取得
  AreaData defaultData;
  if( !GetDefaultDataByZoneId( zoneId, &defaultData ) )
  {
    return false;
  }

  // データ取得
  if( m_pAreaDataAccessor->GetData( implDataNo, pOut ) )
  {
    // ゾーンデフォルト値を設定する
    switch( pOut->commonData.type )
    {
    case DataType::FOLLOW:
      {
        if( pOut->follow.pointData.defaultFlag )
        {
          pOut->follow.pointData = defaultData.follow.pointData;
          pOut->follow.pointData.defaultFlag = true;
        }

        ResolveDefaultData( &pOut->follow.supportData, defaultData.follow.supportData );
        break;
      }
    case DataType::FOLLOW_TRANS:
      {
        if( pOut->followTrans.pointDataA.defaultFlag )
        {
          pOut->followTrans.pointDataA = defaultData.follow.pointData;
          pOut->followTrans.pointDataA.defaultFlag = true;
        }
        if( pOut->followTrans.pointDataB.defaultFlag )
        {
          pOut->followTrans.pointDataB = defaultData.follow.pointData;
          pOut->followTrans.pointDataB.defaultFlag = true;
        }
        
        ResolveDefaultData( &pOut->followTrans.supportData, defaultData.follow.supportData );
        break;
      }
    }

    pOut->isReplace = replaceFlag;
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @brief 指定エリアデータ番号のプライオリティを取得
* @note  エリアデータ番号が無効な場合は0が返る
* @note  カメラエリア継承機能でプライオリティが変わる事はありません
*
* @param  areaDataNo      エリアデータ番号
*
* @return プライオリティ( 0 〜 大きいほど優先 )
*/
s32 AreaDataManager::GetPriority( u32 areaDataNo ) const
{
  if( m_pAreaDataAccessor->IsValid() == false || IsAreaDataNoValid( areaDataNo ) == false )
  {
    return 0;
  }

  AreaData areaData;
  m_pAreaDataAccessor->GetData( areaDataNo, &areaData );
  return areaData.commonData.priority;
}

/**
* @brief 指定エリアデータ番号が有効か取得
* @note  AreaDataNo::DEFAULTも無効扱いになる
*
* @param  areaDataNo      エリアデータ番号
*
* @return true 有効 false 無効
*/
b32 AreaDataManager::IsAreaDataNoValid( u32 areaDataNo ) const
{ 
  if( m_pAreaDataAccessor->IsValid() == false )
  {
    return false;
  }

  if( areaDataNo >= m_pAreaDataAccessor->GetTotalDataMax()  || areaDataNo == AreaDataNo::INVALID || areaDataNo == AreaDataNo::DEFAULT )
  {
    return false;
  }

  // ワーク参照チェック
  AreaData areaData;
  m_pAreaDataAccessor->GetData( areaDataNo, &areaData );

  if( areaData.commonData.effectiveWork != WORK_NULL )
  {
    const EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
    const u16 * pWk = pEventWork->GetEventWorkAdrs( areaData.commonData.effectiveWork );
    if( *pWk != areaData.commonData.effectiveValue )
    {
      return false;
    }
  }

  return true;
}

#if PM_DEBUG
//
// 開発中のデータ作成サポート関連機能
//

/**
* @brief 指定エリアデータ番号のパラメータAがユーザー編集されたものか取得
* @note  エリアデータ番号が無効な場合はfalseが返る
*
* @param  areaDataNo      エリアデータ番号
*
* @return true ユーザー編集値 false エリアデフォルト値
*/
b32 AreaDataManager::IsParamAUserSet( u32 areaDataNo ) const
{
  if( IsAreaDataNoValid( areaDataNo ) == false )
  {
    return false;
  }

  return false;
  // BIN_AREA_DATA* pBinAreaData = static_cast<BIN_AREA_DATA*>( ((&m_pAreaData->areaData) + areaDataNo ) );
  // return pBinAreaData->defaultFlagA;
}

/**
* @brief 指定エリアデータ番号のパラメータBがユーザー編集されたものか取得
* @note  エリアデータ番号が無効な場合はfalseが返る
*
* @param  areaDataNo      エリアデータ番号
*
* @return true ユーザー編集値 false エリアデフォルト値
*/
b32 AreaDataManager::IsParamBUserSet( u32 areaDataNo ) const
{
  if( IsAreaDataNoValid( areaDataNo ) == false )
  {
    return false;
  }

  return true;
  // BIN_AREA_DATA* pBinAreaData = static_cast<BIN_AREA_DATA*>( ((&m_pAreaData->areaData) + areaDataNo ) );
  // return pBinAreaData->defaultFlagB;
}

#endif // #if PM_DEBUG


/**
* @brief デフォルト設定をデータ番号から取得
*
* @param  dataIndex データ番号
* @param  pOut      格納先
*
* @return 成否( データ番号が範囲外の場合、失敗 )
*/
b32 AreaDataManager::GetDefaultDataByIndex( u32 dataIndex, AreaData* pOut ) const
{
  if( m_pZoneDefaultDataAccessor->IsValid() == false )
  {
    return false;
  }

  if( m_pZoneDefaultDataAccessor->GetData( dataIndex, pOut ) )
  {
    // 値チェック
    GFL_ASSERT( pOut->commonData.type == DataType::FOLLOW );
    GFL_ASSERT( pOut->follow.pointData.defaultFlag == false );

    pOut->isDefault = true;
    return true;
  }
  else
  {
    return false;
  }
}

/**
* @brief デフォルト設定をデータ番号から取得(カメラサポート)
*
* @param  dataIndex データ番号
* @param  pOut      格納先
*
* @return 成否( データ番号が範囲外の場合、失敗 )
*/
b32 AreaDataManager::GetDefaultSupportDataByIndex( u32 dataIndex, BIN_SUPPORT_DATA* pOut ) const
{
  if( m_pZoneDefaultSupportDataAccessor->IsValid() == false )
  {
    return false;
  }

  AreaData work;
  if( m_pZoneDefaultSupportDataAccessor->GetData( dataIndex, &work ) )
  {
    // 値チェック
    // GFL_ASSERT( pBinData->defaultParamFlag == false );
    GFL_ASSERT( work.commonData.type == DataType::FOLLOW );
    GFL_ASSERT( work.follow.supportData.zoneDefaultFlag == false );

    *pOut = work.follow.supportData;
    return true;
  }
  else
  {
    return false;
  }
}


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );