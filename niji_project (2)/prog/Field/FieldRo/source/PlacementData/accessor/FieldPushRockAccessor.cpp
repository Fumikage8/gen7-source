//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldPushRockAccessor.cpp
 *	@brief  動作モデル配置情報のアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldPushRockAccessor.h"
#include "Savedata/include/EventWork.h"
#include "Field/FieldRo/include/PlacementData/EventIDOffset.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "FieldScript/include/FieldScriptSystem.h"

#include <niji_conv_header/field/script/inc/field_event.inc>

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
const FieldPushRockMoveModelAccessor::DummyCollisionBox FieldPushRockMoveModelAccessor::m_DefCollisionHeader = 
{
  1,
  FieldCollisionAccessor::BOX,
  {
    {0,0,0},
    {1,0,0,0},
    {
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_R),
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_H),
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_R)
    },
  }
};

const FieldPushRockMoveModelAccessor::DummyCollisionBox FieldPushRockMoveModelAccessor::m_DefTalkCollisionHeader = 
{
  1,
  FieldCollisionAccessor::BOX,
  {
    {0,0,0},
    {1,0,0,0},
    {
      
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_R + FieldPushRockAccessor::sc_ROCK_COLLISION_TALK_ADD_R),
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_H),
      static_cast<f32>(FieldPushRockAccessor::sc_ROCK_COLLISION_R + FieldPushRockAccessor::sc_ROCK_COLLISION_TALK_ADD_R)
    },
  }
};

/**
 *  @biref  コンストラクタ
 */
FieldPushRockMoveModelAccessor::FieldPushRockMoveModelAccessor() : 
  FieldMoveModelAccessor()
  , m_pRockData(NULL)
  , m_pPushRockHeader(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldPushRockMoveModelAccessor::~FieldPushRockMoveModelAccessor()
{
}

/**
 *  @brief  PushRockの大本情報
 */
void FieldPushRockMoveModelAccessor::Setup( const FieldPushRockAccessor::PushRockData & pushRockData )
{
  m_pPushRockHeader = &pushRockData;
}

/**
 *  @brief  初期化
 */
void FieldPushRockMoveModelAccessor::Initialize( const void * data )
{
  GFL_ASSERT( m_pPushRockHeader );

  m_pRockData = static_cast<const FieldPushRockAccessor::RockData*>(data);

  gfl2::math::Matrix34 matrix;
  gfl2::math::Vector3 local_pos;
  gfl2::math::Quaternion rotate;

  // マトリクス取：得
  FieldPushRockAccessor::GetPushRockMatrix( *m_pPushRockHeader, &matrix );
  FieldPushRockAccessor::GetPushRockLocalPosition( *m_pPushRockHeader, m_pRockData->x, m_pRockData->z, &local_pos );
  FieldPushRockAccessor::GetPushRockQuaternion( *m_pPushRockHeader, &rotate );

  local_pos = matrix * local_pos;

  // MoveModelData
  m_MoveModelData.type = 0;
  m_MoveModelData.position[0] = local_pos[0];
  m_MoveModelData.position[1] = local_pos[1];
  m_MoveModelData.position[2] = local_pos[2];
  m_MoveModelData.quaternion[0] = rotate[0];
  m_MoveModelData.quaternion[1] = rotate[1];
  m_MoveModelData.quaternion[2] = rotate[2];
  m_MoveModelData.quaternion[3] = rotate[3];
  m_MoveModelData.romVersion    = GET_VERSION();
  m_MoveModelData.flagwork      = 0;
  m_MoveModelData.flagwork_num  = 0;
  m_MoveModelData.eventID       = m_pRockData->uniq_id + PR_ROCK_ACTOR_START;
  m_MoveModelData.moveCode   = MoveModel::FIELD_MOVE_CODE_NONE;
  m_MoveModelData.characterId = m_pPushRockHeader->charaID;
  m_MoveModelData.scriptID  = SCRID_FLD_EV_OSERUROCK_01;
  m_MoveModelData.defaultAnimeData.defaultMotionIDNum =0;
  m_MoveModelData.alias = false;
  m_MoveModelData.reversetime_type = false;
  m_MoveModelData.talkCollisionOffset = 0;
  m_MoveModelData.collisionOffset = 0;


  // コリジョンの情報
  m_CollisionHeader = m_DefCollisionHeader;
  m_TalkCollisionHeader = m_DefTalkCollisionHeader;

  m_CollisionHeader.box.center[0] = local_pos[0];
  m_CollisionHeader.box.center[1] = local_pos[1];
  m_CollisionHeader.box.center[2] = local_pos[2];
  m_TalkCollisionHeader.box.center[0] = local_pos[0];
  m_TalkCollisionHeader.box.center[1] = local_pos[1];
  m_TalkCollisionHeader.box.center[2] = local_pos[2];
}

/**
 *  @brief  最大数
 */
u32 FieldPushRockMoveModelAccessor::GetDataMax() const
{
  return 1;
}


/**
 *  @brief エイリアス　データ数
 */
u32 FieldPushRockMoveModelAccessor::GetAliasDataMax() const
{
  u32 count = 0;
  return count;
}

/**
 *  @brief エイリアス以外のデータ数
 */
u32 FieldPushRockMoveModelAccessor::GetNotAliasDataMax() const
{
  u32 alias = GetAliasDataMax();

  return GetDataMax() - alias;
}

/**
 *  @brief 情報取得
 */
const FieldMoveModelAccessor::MoveModelData * FieldPushRockMoveModelAccessor::GetData( u32 idx ) const
{
  if( GetDataMax() > idx ){
    return &m_MoveModelData;
  }

  return NULL;
}

/**
 *  @brief 話しかけコリジョン情報の取得
 */
const void * FieldPushRockMoveModelAccessor::GetTalkCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const
{
  return reinterpret_cast<const void*>( &m_TalkCollisionHeader );
}

/**
 *  @brief 衝突用コリジョン情報の取得
 */
const void * FieldPushRockMoveModelAccessor::GetCollisionData( const FieldMoveModelAccessor::MoveModelData & data ) const
{
  return reinterpret_cast<const void*>( &m_CollisionHeader );
}


/**
 *  @brief デフォルトのアイドルモーション取得
 */
u32 FieldPushRockMoveModelAccessor::GetDefaultWaitMotionId( const EventWork& eventWork, const DefaultWaitAnimeData& animeData ) const
{
  return 0; 
}


//----------------------------------------------------------------------------------------------------

/**
 *  @biref  コンストラクタ
 */
FieldPushRockAccessor::FieldPushRockAccessor() : m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldPushRockAccessor::~FieldPushRockAccessor()
{
  m_pData = NULL;
}

/**
 *  @brief  初期化
 */
void FieldPushRockAccessor::Initialize( const void * data )
{
  m_pData = reinterpret_cast<const PushRockDataHeader*>(data);
}


/**
 *  @brief  最大数
 */
u32 FieldPushRockAccessor::GetDataMax() const
{
  return m_pData->num;
}

/**
 *  @brief 情報取得
 */
const FieldPushRockAccessor::PushRockData * FieldPushRockAccessor::GetData( u32 idx ) const
{
  if( idx < m_pData->num )
  {
    return &m_pData->data[idx];
  }
  
  GFL_ASSERT( idx < m_pData->num );
  return &m_pData->data[0];
}


/**
 *  @brief 岩データの取得
 */
const FieldPushRockAccessor::RockData & FieldPushRockAccessor::GetRockData( const PushRockData & data, u32 idx ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.rock_data_offset;

  const RockData * cpRock = reinterpret_cast<const RockData*>( addr );

  if( idx < data.rock_num )
  {
    return cpRock[idx];
  }
  GFL_ASSERT( idx < data.rock_num );
  return cpRock[0];
}

/**
 *  @brief 穴データの取得
 */
const FieldPushRockAccessor::HoleData & FieldPushRockAccessor::GetHoleData( const PushRockData & data, u32 idx ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.hole_data_offset;
  const HoleData * cpHole = reinterpret_cast<const HoleData*>( addr );

  if( idx < data.hole_num )
  {
    return cpHole[idx];
  }
  GFL_ASSERT( idx < data.hole_num );
  return cpHole[0];
}

/**
 *  @brief 穴のコリジョンデータ生成
 */
void FieldPushRockAccessor::CreateHoleCollisionData( const PushRockData & data, u32 idx, BaseActor* p_actor ) const
{
  const FieldPushRockAccessor::HoleData & holeData =  GetHoleData( data, idx );
  gfl2::math::Matrix34 matrix;
  gfl2::math::Vector3 position;

  GetPushRockMatrix( data, &matrix );
  GetPushRockLocalPosition( data, holeData.x, holeData.z, &position );
  position  = matrix * position;

  p_actor->SetPosition( position );

  gfl2::math::Vector3 pos(0, 0, 0);
  // 自身を押し返す処理用
  // @fix GFNMCat[1721] : 【押せる岩】落ちる穴の形状が丸い
  p_actor->CreateBox( pos, matrix.GetElemAxisX(), matrix.GetElemAxisZ(), sc_ROCK_COLLISION_R, sc_ROCK_COLLISION_R, sc_ROCK_COLLISION_H );
}


/**
 *  @brief グリッド別タイプ
 */
FieldPushRockAccessor::PushRockDef FieldPushRockAccessor::GetGridType( const PushRockData & data, u32 x, u32 z ) const
{
  uptr addr = reinterpret_cast<uptr>( m_pData );
  addr = addr + data.point_data_offset;
  const u32 * cpEnum = reinterpret_cast<const u32*>( addr );
  u32 index = 0;

  if( (x < data.xnum) && (z < data.znum) )
  {
    index = (z * data.xnum) + x;
    return static_cast<PushRockDef>(cpEnum[index]);
  }

  GFL_ASSERT( (x < data.xnum) && (z < data.znum) );
  return static_cast<PushRockDef>(cpEnum[0]);
}


/**
 *  @brief 押し岩の格子のローカル座標を求める。
 */
bool FieldPushRockAccessor::GetPushRockLocalGrid(const PushRockData& data, const gfl2::math::Vector3& world_position, u32 * out_x, u32 * out_z, ZoneID zoneID)
{
  gfl2::math::Matrix34 matrix;
  GetPushRockMatrix( data, &matrix );
  matrix = matrix.Inverse();

  gfl2::math::Vector3 local_position = matrix * world_position;

  // @fix GFMMCat[156]:一部のフィールドでカイリキ岩が動かなくなっている 対処→momijiで追加された領域外チェック処理をフェローチェマップのゾーンだけで動くように修正
  if (zoneID == Z_F0701PH0501)
  {
    // フェローチェマップのゾーンの場合は領域が複数存在するため領域チェックを行う

    // 領域外の場合は失敗とする momijiにて領域グループが複数存在する状況が発生したため修正を加えています。（複数領域の対応漏れ対処）
    if ((local_position.x < 0.0f) ||
      (local_position.x > data.position[0] + static_cast<f32>(data.width * data.xnum)) ||
      (local_position.z < 0.0f) ||
      (local_position.z > data.position[2] + static_cast<f32>(data.width * data.znum)))
    {
      return false;
    }
  }

  // 基準座標がグリッド中心になっている。　x z 座標を求める際には、半グリッドずらす。
  f32 half_grid = (static_cast<f32>(data.width)*0.50f);
  *out_x = static_cast<u32>((local_position.GetX() + half_grid) / static_cast<f32>(data.width));
  *out_z = static_cast<u32>((local_position.GetZ() + half_grid) / static_cast<f32>(data.width));

  if( (data.xnum > *out_x) && (data.znum > *out_z) )
  {
    return true;
  }

  return false;
}

/**
 *  @brief 押し岩のマトリクスを取得
 */
void FieldPushRockAccessor::GetPushRockMatrix( const PushRockData & data, gfl2::math::Matrix34 * p_out )
{
  *p_out =  gfl2::math::Matrix34::GetRotationY(GFL_MATH_DEG_TO_RAD(data.rot_y));
  (*p_out)[0][3] = data.position[0];
  (*p_out)[1][3] = data.position[1];
  (*p_out)[2][3] = data.position[2];
}

/**
 *  @brief 押し岩のローカル座標を取得
 */
b32 FieldPushRockAccessor::GetPushRockLocalPosition( const PushRockData & data, u32 x, u32 z, gfl2::math::Vector3 * p_out )
{
  p_out->SetX( data.width * x );
  p_out->SetY( 0 );
  p_out->SetZ( data.width * z );

  return ((data.xnum > x) && (data.znum > z));
}


/**
 *  @brief 押し岩の姿勢情報取得
 */
void FieldPushRockAccessor::GetPushRockQuaternion( const PushRockData & data, gfl2::math::Quaternion * p_out )
{
  *p_out = gfl2::math::Quaternion( gfl2::math::Vector3(0,1,0), GFL_MATH_DEG_TO_RAD(data.rot_y) );
}




GFL_NAMESPACE_END(Field);
