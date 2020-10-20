//======================================================================
/**
* @file FieldDowsingTool.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドダウジング関連の処理
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include "niji_conv_header/sound/SoundMiddleID.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// field
#include "Field/FieldRo/include/Dowsing/FieldDowsingTool.h"
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/FieldWorld.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Dowsing );


static const f32 OUT_RANGE_HEIGHT_DIFF( 850 );  // 足元の高さがこの値以上離れていたらサーチ圏外。傾き40度(マップ設計ルール)で10m(サーチ距離)移動した際の差が839


static const RANK_SETTING sc_RankSettingTable[] = 
{
  {
    0.0f, 1.0f, 0, SMID_NULL, SEQ_SE_FLD_RIDE_MOOLAND_NOTICE_LV3
  },
  {
    1000.0f, 0.75f, 0, SEQ_SE_FLD_RIDE_MOOLAND_NOSE_LV2, SEQ_SE_FLD_RIDE_MOOLAND_NOTICE_LV1
    },
    {
      9999.0f, 1.0f, 0, SEQ_SE_FLD_RIDE_MOOLAND_NOSE_LV1, SMID_NULL
    },
    {
      9999.0f, 1.0f, 0, SMID_NULL, SMID_NULL
    },
};



Tool::Tool( void ) :
  m_pFieldEventCheck( NULL )
{

}

Tool::~Tool( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void Tool::Initialize( const InitializeDescription& rInitDesc )
{
  m_pFieldEventCheck = rInitDesc.m_pFieldEventCheck;
}

/**
* @brief 終了処理
*
* @return 無し
*/
void Tool::Terminate( void )
{
  m_pFieldEventCheck = NULL;
}

// 最新の反応結果を取得
SearchRank::Value Tool::Search( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range ) const
{
  // 取得できる状態
  if( m_pFieldEventCheck && m_pFieldEventCheck->IsHitHiddenItem() )
  {
    return SearchRank::RANK_A;
  }

  // チェック
  f32 distance;
  if( GetNearestItemDistance( rPos, rDir, range, &distance ) )
  {
    return GetSerchRankByDistance( distance );
  }
  else
  {
    return SearchRank::RANK_LOST;
  }
}

// 指定ランクの設定を取得
b32 Tool::GetSetting( SearchRank::Value value, RANK_SETTING* pOut )
{
  *pOut = Dowsing::sc_RankSettingTable[ value ];
  return true;
}

//
// private
//

// 一番近いアイテムまでの距離を取得
b32 Tool::GetNearestItemDistance( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range, f32* pOut )
{
  GameSys::GameManager*     game_manager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  b32 isHit( false );
  f32 result( 0.0f );

  // 現在のエリアに含まれる全ゾーンをチェック
  {
    u32 areaId = game_manager->GetFieldmap()->GetAreaID();

    Field::World* pWorld = game_manager->GetGameData()->GetFieldWorld();
    for( u32 i = 0; i < pWorld->GetZoneNum(); i++ )
    {
      const Field::World::ZoneInfo& rZoneInfo = pWorld->GetZoneInfo( i );
      if( rZoneInfo.m_AreaID != areaId )
      {
        continue;
      }

      f32 distanceWork( 0.0f );
      if( GetNearestItemDistance( rPos, rDir, range, rZoneInfo.m_ZoneID, &distanceWork ) )
      {
        if( isHit )
        {
          if( distanceWork >= result )
          {
            continue;
          }
        }

        isHit = true;
        result = distanceWork;
      }

    } // pWorld->GetZoneNum()
  }

  *pOut = result;
  return isHit;
}

// 距離から近さのランクを取得
SearchRank::Value Tool::GetSerchRankByDistance( f32 distance )
{
  for( u32 i = SearchRank::DISTANCE_RANK_START; i <= SearchRank::DISTANCE_RANK_END; i++ )
  {
    if( distance <= sc_RankSettingTable[i].border )
    {
      return static_cast< SearchRank::Value > (i);
    }
  }

  return SearchRank::RANK_LOST;
}

// 一番近いアイテムまでの距離を取得 (指定ゾーンチェック)
b32 Tool::GetNearestItemDistance( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range, u32 zoneId, f32* pOut )
{
  gfl2::math::Vector3 sourcePos( rPos );


  f32 rangeRad = gfl2::math::ConvDegToRad( range );

  b32 isHit( false );
  f32 result( 0.0f );

  GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  void * p_data = game_manager->GetFieldResident()->GetPlacementDataManager()->GetZonePlacementData( zoneId, PlacementDataManager::PACK_IDX_PLACEMENT_ITEM );

  // 配置データが無い場合はNULL
  if( p_data == NULL )
  {
    *pOut = result;
    return isHit;
  }

  // アクセサー生成
  FieldItemAccessor Accessor;
  Accessor.Initialize( p_data );

  // posイベントの生成
  u32 maxi = Accessor.GetDataMax();
  for( u32 i = 0; i < maxi; ++ i )
  {
    // 隠し以外のアイテムは無視
    const FieldItemAccessor::FieldItemData *pData = Accessor.GetData( i );
    if( !FieldItemAccessor::IsHiddenShapeType( pData->shapeType ) )
    {
      continue;
    }

    // 生成条件チェック
    if( !PlacementDataManager::IsEventCreate( game_manager->GetGameData()->GetEventWork(), pData->romVersion, pData->flagwork, pData->flagwork_num ) )
    {
      continue;
    }

    // あたり判定
    gfl2::math::Vector3 pos( pData->position[ 0 ], pData->position[ 1 ], pData->position[ 2 ] );
    gfl2::math::Vector3 dir( pos - sourcePos );
    dir.y = 0.0f;

    // @fix NMcat[1550]:別マップの復活サーチ道具にムーランドが反応してしまう、対処。Y座標が非常に離れている場合はサーチ圏外。
    // 高さチェック @note 元データの設定座標をそのまま見ている。アイテムは配置モデルなので、Y座標が接地処理で変わる事はない。
    if( gfl2::math::Abs( pos.y - sourcePos.y ) >= OUT_RANGE_HEIGHT_DIFF )
    {
      continue;
    }

    f32 distanceWork( 0.0f );
    distanceWork = dir.Length();

    // 距離チェック
    if( distanceWork > sc_RankSettingTable[ SearchRank::RANK_WORST ].border )
    {
      continue;
    }

    // 角度チェック
    dir.NormalizeRet();

    if( gfl2::math::AcosRad( dir.Dot( rDir ) ) > ( rangeRad * 0.5f ) )
    {
      continue;
    }

    // ここまできたらHit
    if( isHit )
    {
      if( distanceWork >= result )
      {
        continue;
      }
    }

    isHit = true;
    result = distanceWork;

  }

  *pOut = result;
  return isHit;
}


GFL_NAMESPACE_END( Dowsing );
GFL_NAMESPACE_END( Field );