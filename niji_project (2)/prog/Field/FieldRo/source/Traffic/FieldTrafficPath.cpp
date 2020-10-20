/**
 *  GAME Freak inc.
 *
 *  @file   FieldTrafficPath.cpp
 *  @brief  往来パス
 *  @author miyachi_soichi
 *  @date   2015/06/25
 */

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficActor.h"
#include "Field/FieldRo/include/Traffic/FieldTrafficPath.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

GFL_NAMESPACE_BEGIN(Field)

static const u32 TRAFFIC_PATH_HEAP_SIZE = 0x500;

/**
 *  @brief  コンストラクタ
 */
TrafficPath::TrafficPath( void )
: m_PathControl()
, m_pHeap( NULL )
, m_nValueNum( 0 )
, m_fLength( 0.0f )
, m_fProgress( 0.0f )
, m_Flags( FLAG_CLEAR )
{
  for( u32 i = 0; i < TRAFFIC_VALUE_MAX; ++ i )
  {
    m_vValueList[ i ] = gfl2::math::Vector3::GetZero();
  }
}

/**
 *  @brief  デストラクタ
 */
TrafficPath::~TrafficPath( void )
{
  Terminate();
}

/**
 *  @brief  初期化
 */
void TrafficPath::Initialize( gfl2::heap::HeapBase *pHeap )
{
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( pHeap, TRAFFIC_PATH_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false, "FieldTrafficPath" );
}

/**
 *  @brief  破棄
 */
void TrafficPath::Terminate( void )
{
  if( m_pHeap )
  {
    m_PathControl.Finalize();
    GFL_DELETE_HEAP( m_pHeap );
    m_pHeap = NULL;
  }
}

/**
 *  @brief  パスの生成
 */
void TrafficPath::CreatePath( const Traffic::PathCoreData *pCoreData )
{
  if( CheckFlags( FLAG_INITIALIZED ) ){ return; }

  MIYACHI_PRINT( "---- TrafficPath Debug OutPut ----\n" );
  MIYACHI_PRINT( "\n" );
  MIYACHI_PRINT( "Point Num = %d\n", pCoreData->num );
  if( pCoreData->curve != 0 )  { MIYACHI_PRINT( "Path is Curve\n" ); }
  else                         { MIYACHI_PRINT( "Path is Liner\n" ); }
  if( pCoreData->loop != 0 )   { MIYACHI_PRINT( "Path is Loop\n" ); }
  else                         { MIYACHI_PRINT( "Path is Piston\n" ); }
  if( pCoreData->landing != 0 ){ MIYACHI_PRINT( "Path is Landing\n" ); }
  else                         { MIYACHI_PRINT( "Path is In air\n" ); }
  for( u32 i = 0; i < pCoreData->num; ++ i )
  {
    MIYACHI_PRINT( "Position[ %d ] = ( %0.6f, %0.6f, %0.6f )\n", i, pCoreData->position[ i*3 + 0 ], pCoreData->position[ i*3 + 1 ], pCoreData->position[ i*3 + 2 ] );
  }
  MIYACHI_PRINT( "\n" );
  MIYACHI_PRINT( "----------------------------------\n" );

  // データ保持
  m_Flags = FLAG_CLEAR;
  m_nValueNum = pCoreData->num;
  for( u32 i = 0; i < m_nValueNum; ++ i )
  {
    m_vValueList[ i ].x = pCoreData->position[ i*3 + 0 ];
    m_vValueList[ i ].y = pCoreData->position[ i*3 + 1 ];
    m_vValueList[ i ].z = pCoreData->position[ i*3 + 2 ];
  }
  SetFlags( FLAG_CURVE, pCoreData->curve != 0 );
  SetFlags( FLAG_LOOP,  pCoreData->loop != 0 );
  SetFlags( FLAG_LANDING,  pCoreData->landing != 0 );

  // パス関係のデータを初期化
  m_fProgress = 0.0f;
  SetFlags( FLAG_INITIALIZED, true );
  if( !CheckFlags( FLAG_CURVE ) )
  {
    // ループ対策
    if( CheckFlags( FLAG_LOOP ) )
    {
      // 最初に戻す
      m_vValueList[ m_nValueNum ] = m_vValueList[ 0 ];
      m_nValueNum += 1;
    }
    m_fLength = getLengthForStraight();
  }
  else
  {
    m_PathControl.Initialize( m_pHeap, m_vValueList, m_nValueNum, CheckFlags( FLAG_LOOP ) );
    m_fLength = getLengthForCurve();
  }
}

/**
 *  @brief  パスの破棄
 */
void TrafficPath::DeletePath( void )
{
  if( !CheckFlags( FLAG_INITIALIZED ) ){ return; }

  EndUse();
  m_PathControl.Finalize();
  m_Flags = FLAG_CLEAR;
}

/**
 *  @brief  使用開始
 */
void TrafficPath::StartUse( void )
{
  // 既に使用しているので使えない
  if( CheckFlags( FLAG_USED ) )
    return;

  SetFlags( FLAG_USED, true );
  SetFlags( FLAG_REVERSE, false );
  m_fProgress = 0.0f;
}

/**
 *  @brief  使用終了
 */
void TrafficPath::EndUse( void )
{
  // 既に解放している
  if( !CheckFlags( FLAG_USED ) )
    return;

  SetFlags( FLAG_USED | FLAG_REVERSE, false );
  m_fProgress = 0.0f;
}

/**
 *  @brief  進捗させる
 *  @param  spd   移動速度
 */
f32 TrafficPath::Progress( f32 spd )
{
  f32 fResult = spd;
  if( !CheckFlags( FLAG_CURVE ) ){ fResult = progressForStraight( spd ); }
  else                           { fResult = progressForCurve( spd );    }

  return fResult;
}

/**
 *  @brief  フラグ設定
 */
void TrafficPath::SetFlags( u32 flag, bool state )
{
  if( state ) { m_Flags |=    flag;   }
  else{         m_Flags &= ~( flag ); }
}

/**
 *  @brief  初期位置の取得
 */
gfl2::math::Vector3 TrafficPath::GetStartPosition( void )
{
  if( !CheckFlags( FLAG_CURVE ) )
  {
    return getPositionForStraight( 0.0f );
  }

  return getPositionForCurve( 0.0f );
}

/**
 *  @brief  初期方向の取得
 */
gfl2::math::Vector3 TrafficPath::GetStartRotation( void )
{
  if( !CheckFlags( FLAG_CURVE ) )
  {
    return getRotationForStraight( 0.0f );
  }
  return getRotationForCurve( 0.0f );
}

/**
 *  @brief  現在位置の取得
 */
gfl2::math::Vector3 TrafficPath::GetCurrentPosition( void )
{
  if( !CheckFlags( FLAG_CURVE ) )
  {
    return getPositionForStraight( m_fProgress );
  }
  return getPositionForCurve( m_fProgress );
}

/**
 *  @brief  現在方向の取得
 */
gfl2::math::Vector3 TrafficPath::GetCurrentRotation( void )
{
  if( !CheckFlags( FLAG_CURVE ) )
  {
    return getRotationForStraight( m_fProgress );
  }
  return getRotationForCurve( m_fProgress );
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 直線用関数
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  進捗
 */
f32 TrafficPath::progressForStraight( f32 spd )
{
  f32 ratio = spd / m_fLength;
  f32 rev = 1.0f;
  if( CheckFlags( FLAG_REVERSE ) ){ rev = -1.0f;}
  f32 progress = m_fProgress + ratio * rev;
  // 安全チェック
  if( progress < 0.0f )
    progress = 0.0f;
  if( 1.0f < progress )
    progress = 1.0f;

  // 全長から現在地を割り出す
  f32 old_len = m_fProgress *m_fLength;
  f32 cur_len = progress * m_fLength;
  f32 dst_len;
  gfl2::math::Vector3 dst_dir;

  for( u32 idx = 1; idx < TRAFFIC_VALUE_MAX; ++ idx )
  {
    u32 prev = idx - 1;
    dst_dir = m_vValueList[ idx ] - m_vValueList[ prev ];
    dst_len = dst_dir.Length();

    if( cur_len <= dst_len )
    {
      // 見つけた
      break;
    }

    cur_len -= dst_len;
    old_len -= dst_len;
  }

  bool bCorner = false;
  if( old_len < 0.0f )
  {
    spd = spd - cur_len;
    spd += 0.001f;
    ratio = spd / m_fLength;
    bCorner = true;
  }
  else if( dst_len < old_len )
  {
    spd = old_len - dst_len;
    spd += 0.001f;
    ratio = spd / m_fLength;
    bCorner = true;
  }

  m_fProgress += ( ratio * rev );
  // 安全チェック
  if( m_fProgress <= 0.0f )
  {
    if( CheckFlags( FLAG_LOOP ) ){ m_fProgress = 1.0f; }
    else                         { m_fProgress = 0.0f; SetFlags( FLAG_REVERSE, false ); }
    bCorner = true;
  }
  else if( 1.0f <= m_fProgress )
  {
    if( CheckFlags( FLAG_LOOP ) ){ m_fProgress = 0.0f; }
    else                         { m_fProgress = 1.0f; SetFlags( FLAG_REVERSE, true ); }
    bCorner = true;
  }
  SetFlags( FLAG_CORNER, bCorner );

  return spd;
}
/**
 *  @brief  長さの取得
 */
f32 TrafficPath::getLengthForStraight( void )
{
  f32 fLength = 0.0f;
  for( u32 idx = 1; idx < TRAFFIC_VALUE_MAX; ++ idx )
  {
    if( m_nValueNum <= idx ){ continue; }
    u32 prev = idx - 1;
    gfl2::math::Vector3 vDir = m_vValueList[ idx ] - m_vValueList[ prev ];
    fLength += vDir.Length();
  }

  return fLength;
}
/**
 *  @brief  指定位置の取得
 */
gfl2::math::Vector3 TrafficPath::getPositionForStraight( f32 ratio )
{
  // 安全チェック
  if( ratio < 0.0f )
    ratio = 0.0f;
  if( 1.0f < ratio )
    ratio = 1.0f;

  // 全長から現在地を割り出す
  f32 cur_len = ratio * m_fLength;
  f32 dst_len;
  u32 dst_idx = 0;
  gfl2::math::Vector3 dst_dir;

  for( u32 idx = 1; idx < TRAFFIC_VALUE_MAX; ++ idx )
  {
    u32 prev = idx - 1;
    dst_dir = m_vValueList[ idx ] - m_vValueList[ prev ];
    dst_len = dst_dir.Length();

    if( cur_len <= dst_len )
    {
      // 見つけた
      dst_idx = prev;
      break;
    }

    cur_len -= dst_len;
  }

  f32 dst_ratio = cur_len / dst_len;

  gfl2::math::Vector3 vPos = m_vValueList[ dst_idx ] + ( dst_dir * dst_ratio );
  return vPos;
}
/**
 *  @brief  指定方向の取得
 */
gfl2::math::Vector3 TrafficPath::getRotationForStraight( f32 ratio )
{
  // 安全チェック
  if( ratio < 0.0f )
    ratio = 0.0f;
  if( 1.0f < ratio )
    ratio = 1.0f;

  // 全長から現在地を割り出す
  f32 cur_len = ratio * m_fLength;
  f32 dst_len;
  gfl2::math::Vector3 dst_dir;

  for( u32 idx = 1; idx < TRAFFIC_VALUE_MAX; ++ idx )
  {
    u32 prev = idx - 1;
    dst_dir = m_vValueList[ idx ] - m_vValueList[ prev ];
    dst_len = dst_dir.Length();

    if( cur_len <= dst_len )
    {
      // 見つけた
      break;
    }

    cur_len -= dst_len;
  }
  
  // 逆流
  if( CheckFlags( FLAG_REVERSE ) )
    dst_dir *= -1.0f;

  // 方向ベクトルから回転を算出
  f32 fRotY = atan2( dst_dir.x, dst_dir.z );

  return gfl2::math::Vector3( 0.0f, fRotY, 0.0f );
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 曲線用関数
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  進捗
 */
f32 TrafficPath::progressForCurve( f32 spd )
{
  f32 fLength = m_fLength;
  f32 ratio = spd / fLength;
  f32 rev = 1.0f;
  if( CheckFlags( FLAG_REVERSE ) ){ rev = -1.0f; }

  bool bCorner = false;
  m_fProgress += ratio * rev;
  if( m_fProgress <= 0.0f )
  {
    if( CheckFlags( FLAG_LOOP ) ){ m_fProgress = 1.0f; }
    else                         { m_fProgress = 0.0f; SetFlags( FLAG_REVERSE, false ); bCorner = true; }
  }
  else if( 1.0f <= m_fProgress )
  {
    if( CheckFlags( FLAG_LOOP ) ){ m_fProgress = 0.0f; }
    else                         { m_fProgress = 1.0f; SetFlags( FLAG_REVERSE, true ); bCorner = true; }
  }
  SetFlags( FLAG_CORNER, bCorner );

  return spd;
}
/**
 *  @brief  長さの取得
 */
f32 TrafficPath::getLengthForCurve( void )
{
  return m_PathControl.GetTotalLength();
}
/**
 *  @brief  指定位置の取得
 */
gfl2::math::Vector3 TrafficPath::getPositionForCurve( f32 ratio )
{
  return m_PathControl.GetValue( ratio );
}
/**
 *  @brief  指定方向の取得
 */
gfl2::math::Vector3 TrafficPath::getRotationForCurve( f32 ratio )
{
  // 極小距離での方向ベクトルを算出
  gfl2::math::Vector3 vDir;
  {
    f32 delta = 0.001f;
    if( ratio + delta >= 1.0f )
    {
      ratio = 1.0f - delta;
    }
    gfl2::math::Vector3 pos_a = getPositionForCurve( ratio );
    gfl2::math::Vector3 pos_b = getPositionForCurve( ratio + delta );
    if( !CheckFlags( FLAG_REVERSE) ){ vDir = pos_b - pos_a; }
    else                            { vDir = pos_a - pos_b; }
  }

  // 方向ベクトルから回転を算出
  f32 fRotY = atan2( vDir.x, vDir.z );

  return gfl2::math::Vector3( 0.0f, fRotY, 0.0f );
}

GFL_NAMESPACE_END(Field)
