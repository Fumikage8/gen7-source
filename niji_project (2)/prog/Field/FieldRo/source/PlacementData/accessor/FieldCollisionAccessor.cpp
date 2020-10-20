//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldCollisionAccessor.cpp
 *	@brief  コリジョンデータアクセサー
 *	@author	tomoya takahashi
 *	@date		2015.05.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldRo/include/PlacementData/accessor/FieldCollisionAccessor.h"


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
/**
 *  @biref  コンストラクタ
 */
FieldCollisionAccessor::FieldCollisionAccessor() : 
  m_pData(NULL)
{
}

/**
 *  @brief  デストラクタ
 */
FieldCollisionAccessor::~FieldCollisionAccessor()
{
}

/**
 *  @brief  初期化
 */
void FieldCollisionAccessor::Initialize( const void * data )
{
  m_pData = reinterpret_cast<const CollisionDataHeader*>(data);
}

/**
 *  @brief  最大数
 */
u32 FieldCollisionAccessor::GetDataMax() const
{
  if( !m_pData ){ return 0; }
  return m_pData->num;
}

/**
 *  @brief 情報取得
 */
bool FieldCollisionAccessor::GetData( u32 idx, FieldCollisionAccessor::CollisionData* p_out ) const
{
  u32 type  = getDataType(idx);
  uptr addr = getDataAddr( idx );

  p_out->type = type;
  switch( type ){
  case CYLINDER:
    {
      Cylinder * p_data = reinterpret_cast<Cylinder*>(addr);
      p_out->cylinder  = *p_data;
    }
    break;
  case BOX:
    {
      Box * p_data = reinterpret_cast<Box*>(addr);
      p_out->box  = *p_data;
    }
    break;
  case LINE:
    {
      Line * p_data = reinterpret_cast<Line*>(addr);
      p_out->line  = *p_data;
    }
    break;

  default:
    GFL_ASSERT_MSG(0, "type not found %d", type);
    return false;
  }

  return true;
}

/**
 *  @brief  BaseActorのCreate処理を実行
 */
void FieldCollisionAccessor::CreateActor( BaseActor* actor, const CollisionData& data, const gfl2::math::Vector3& parent_pos )
{
  // アクターの座標設定
  actor->SetPosition( parent_pos );

  switch( data.type ){
  case CYLINDER:
    {
      gfl2::math::Vector3 pos(data.cylinder.center[0], data.cylinder.center[1], data.cylinder.center[2]);
      pos = pos - parent_pos; // ローカルポジションに変更
      // 自身を押し返す処理用
      actor->CreateCylinder( pos, data.cylinder.radius, data.cylinder.height );
    }
    break;
  case BOX:
    {
      gfl2::math::Vector3 x, z;
      gfl2::math::Quaternion quaternion( data.box.quaternion[0], data.box.quaternion[1], data.box.quaternion[2], data.box.quaternion[3] );
      x.Set(1.0f, 0.0f, 0.0f);
      z.Set(0.0f, 0.0f, 1.0f);
      
      gfl2::math::Matrix34 rotMat;
      quaternion.QuaternionToMatrix( rotMat );
      x = rotMat.TransformNormal(x);
      z = rotMat.TransformNormal(z);


      gfl2::math::Vector3 pos(data.box.center[0], data.box.center[1], data.box.center[2]);
      pos = pos - parent_pos; // ローカルポジションに変更

      // 自身を押し返す処理用
      actor->CreateBox( pos, x, z, data.box.scale[0], data.box.scale[2], data.box.scale[1] );
    }
    break;

  case LINE:
    // 自身を押し返す処理用
    actor->CreateLine( gfl2::math::Vector3(data.line.start[0], data.line.start[1], data.line.start[2]) - parent_pos,
      gfl2::math::Vector3(data.line.end[0], data.line.end[1], data.line.end[2]) - parent_pos, data.line.height );
    break;

  case TRIANGLE:
    // 自身を押し返す処理用
    actor->CreateTriangle( gfl2::math::Vector3(data.triangle.pos0[0], data.triangle.pos0[1], data.triangle.pos0[2]),
                           gfl2::math::Vector3(data.triangle.pos1[0], data.triangle.pos1[1], data.triangle.pos1[2]),
                           gfl2::math::Vector3(data.triangle.pos2[0], data.triangle.pos2[1], data.triangle.pos2[2]));
    break;

  default:
    GFL_ASSERT(0);  // 未対応タイプ
    break;
  }
}

/**
 *  @brief コリジョン情報からパラメータ取得
 */
void FieldCollisionAccessor::GetParameter( const CollisionData& data, gfl2::math::Vector3* out_center_pos, gfl2::math::Vector3* out_x, f32 * out_width )
{
  switch( data.type ){
  case CYLINDER:
    {
      gfl2::math::Vector3 pos(data.cylinder.center[0], data.cylinder.center[1], data.cylinder.center[2]);
      if(out_center_pos) 
      {
        *out_center_pos = pos;
      }
      if( out_x )
      {
        *out_x = gfl2::math::Vector3(1,0,0);
        GFL_ASSERT( 0 );
      }
      if( out_width )
      {
        *out_width = data.cylinder.radius;
      }
    }
    break;
  case BOX:
    {
      gfl2::math::Vector3 x;
      gfl2::math::Quaternion quaternion( data.box.quaternion[0], data.box.quaternion[1], data.box.quaternion[2], data.box.quaternion[3] );
      x.Set(1.0f, 0.0f, 0.0f);
      
      gfl2::math::Matrix34 rotMat;
      quaternion.QuaternionToMatrix( rotMat );
      x = rotMat.TransformNormal(x);

      if(out_center_pos) 
      {
        out_center_pos->Set( data.box.center[0], data.box.center[1], data.box.center[2] );
      }
      if( out_x )
      {
        *out_x = x;
      }
      if( out_width )
      {
        *out_width = data.box.scale[0];
      }
    }
    break;

  case LINE:
    {
      gfl2::math::Vector3 start_pos(data.line.start[0], data.line.start[1], data.line.start[2]); 
      gfl2::math::Vector3 end_pos(data.line.end[0], data.line.end[1], data.line.end[2]);

      gfl2::math::Vector3 start_end = end_pos - start_pos;
      f32 len = start_end.Length();

      if( len > EPSILON )
      {
        gfl2::math::Vector3 start_end_vec = start_end.Normalize();
        gfl2::math::Vector3 start_end_center = start_pos + (start_end_vec * (len*0.50f));

        if(out_center_pos) 
        {
          *out_center_pos = start_end_center;
        }
        if( out_x )
        {
          *out_x = start_end_vec;
        }
        if( out_width )
        {
          *out_width = len*0.50f;
        }
      }
      else
      {
        // ラインのコリジョンなのに長さがない。＝ありえない。

        if(out_center_pos) 
        {
          *out_center_pos = start_pos;
        }
        if( out_x )
        {
          *out_x = gfl2::math::Vector3(1,0,0);
        }
        if( out_width )
        {
          *out_width = 1.0f;
        }
        GFL_ASSERT(0);
      }
    }
    break;

  default:
    GFL_ASSERT(0);  // 未対応タイプ
    break;
  }
}


//******************************************************************************************************
// private
//******************************************************************************************************

/**
 *  @brief  getDataType
 */
u32 FieldCollisionAccessor::getDataType(u32 idx) const
{
  if( idx < m_pData->num ){
    return m_pData->type[idx];
  }

  return TYPE_MAX;
}

/**
 *  @brief  getDataAddr
 */
uptr FieldCollisionAccessor::getDataAddr( u32 idx ) const
{
  uptr addr = reinterpret_cast<uptr>(m_pData->type);
  addr += sizeof(u32) * m_pData->num;

  GFL_ASSERT_STOP(idx < m_pData->num);  // インデックスオーバーは許さない

  for( u32 i=0; i<idx; ++i ){
    switch( m_pData->type[i] ){
    case CYLINDER:
      {
        addr += sizeof(Cylinder);
      }
      break;
    case BOX:
      {
        addr += sizeof(Box);
      }
      break;
    case LINE:
      {
        addr += sizeof(Line);
      }
      break;

    default:
      // 知らないタイプへの対応も難しい。
      GFL_ASSERT_STOP_MSG(0, "type not found %d", m_pData->type[i]);
      break;
    }
  }

  return addr;
}



GFL_NAMESPACE_END(Field);

