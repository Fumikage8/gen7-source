//======================================================================
/**
 * @file DebugFinderInfo.cpp
 * @date 2016/02/23 15:52:51
 * @author miyachi_soichi
 * @brief デバッグ用：ファインダー情報表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// gfl2
#include <gfx/include/gfl2_DrawUtil.h>
#include <util/include/gfl2_DrawUtilText.h>
// self
#include "App/Finder/include/Field/DebugFinderInfo.h"
// field
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
// gamesys
#include "GameSys/include/TimeZone.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Finder)

/**
 *  @class  DebugInfo
 *  @brief  デバッグ用：ファインダー情報表示
 */
/**
 *  @brief  コンストラクタ
 */
DebugInfo::DebugInfo( gfl2::heap::HeapBase *pHeap )
: m_pWalkerDataA( NULL )
, m_pWalkerDataB( NULL )
, m_pStatueData( NULL )
, m_pOnewayData( NULL )
, m_vPokeRect_color()
, m_vPokeRect_min()
, m_vPokeRect_max()
, m_EnablePokeRect( false )
, m_EnableWalkerA( false )
, m_EnableWalkerB( false )
, m_EnableStatue( false )
, m_EnableOneway( false )
, m_EnableScore( false )
{
  m_fScore_Total = 0.0f;
  m_fScore_Rand = 0.0f;
  m_fScore_Spot = 0.0f;

  m_VertexNum = 3;
  m_pVertex = GFL_NEW(pHeap) gfl2::gfx::DrawUtil::VertexFormat[m_VertexNum];
}

/**
 *  @brief  デストラクタ
 */
DebugInfo::~DebugInfo( void )
{
  GFL_DELETE[] m_pVertex;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  描画
 */
void DebugInfo::Draw( void )
{
  // ポケモン矩形
  drawPokeRect();
  // 巡回パスの描画
  if( IsEnableWalkerA() && m_pWalkerDataA )
  {
    drawWalker( m_pWalkerDataA );
  }
  if( IsEnableWalkerB() && m_pWalkerDataB )
  {
    drawWalker( m_pWalkerDataB );
  }
  // 待機位置の描画
  drawStatue();
  // 一方通行の描画
  drawOneway();
  // スコアの描画
  drawScore();
  // コメント
  drawComment();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  ポケモン矩形
 */
void DebugInfo::drawPokeRect( void )
{
  // 有効化されていない
  if( !IsEnablePokeRect() ){ return; }
  if( !IsEnableScore() ){ return; }

  f32 x = m_vPokeRect_min.x;
  f32 y = m_vPokeRect_max.y;
  f32 w = m_vPokeRect_max.x - m_vPokeRect_min.x;
  f32 h = m_vPokeRect_max.y - m_vPokeRect_min.y;
  // 原点の変更（中央->左上）
  x += 200.0f;
  y = -y + 120.0f;

  // 矩形の描画
  gfl2::gfx::DrawUtil::DrawRect( x, y, w, h, m_vPokeRect_color );
}

/**
 *  @brief  巡回の描画
 */
void DebugInfo::drawWalker( Field::TrialModel::FinderPathData *pData )
{
  gfl2::math::Vector4 color( 1.0f, 0.0f, 0.0f, 0.4f );
  u32 pos_num = pData->pos_num;
  for( u32 pos_idx = 0; pos_idx < pos_num; ++ pos_idx )
  {
    gfl2::math::Vector3 start = pData->route[pos_idx].pos;
    for( u32 next_idx = 0; next_idx < Field::TrialModel::FINDER_CONECTION_MAX; ++ next_idx )
    {
      s32 next_pos_idx = pData->route[pos_idx].next_idx[ next_idx ];
      if( next_pos_idx == -1 ){ continue; }

      gfl2::math::Vector3 end = pData->route[next_pos_idx].pos;
      drawLine( start, end, color );
    }
  }
}

/**
 *  @brief  待機位置の描画
 */
void DebugInfo::drawStatue( void )
{
  // 有効化されていない
  if( !IsEnableStatue() ){ return; }
  // データがない
  if( !m_pStatueData ){ return; }

  f32 radius = 10.0f;
  f32 height = 100.0f;
  gfl2::math::Vector4 color( 1.0f, 0.0f, 0.0f, 1.0f );
  gfl2::math::Vector4 colorG( 0.0f, 1.0f, 0.0f, 1.0f );
  gfl2::math::Vector4 colorB( 0.0f, 0.0f, 1.0f, 1.0f );

  for( u32 i = 0; i < m_pStatueData->num; ++ i )
  {
    gfl2::math::Vector4 pos( m_pStatueData->data[i].position );

    // 矩形の描画
    gfl2::gfx::DrawUtil::DrawTube( pos, radius, height, colorG );

    // 方向の描画
    gfl2::math::Vector3 arrow( 0.0f, 0.0f, 50.0f );
    gfl2::math::Matrix34 mtx = gfl2::math::Matrix34::GetRotationY( m_pStatueData->data[i].rotation );
    arrow = mtx * arrow;
    arrow += m_pStatueData->data[i].position;
    drawLine( m_pStatueData->data[i].position, arrow, colorB );
  }
}

/**
 *  @brief  一方通行の描画
 */
void DebugInfo::drawOneway( void )
{
  // 有効化されていない
  if( !IsEnableOneway() ){ return; }
  // データがない
  if( !m_pOnewayData ){ return; }
  if( m_pOnewayData->num <= 0 ){ return; }

  gfl2::math::Vector4 color( 1.0f, 0.0f, 0.0f, 1.0f );
  u32 num = m_pOnewayData->num - 1;
  for( u32 i = 0; i < num; ++ i )
  {
    // 線分の描画
    gfl2::math::Vector3 start = m_pOnewayData->position[i];
    gfl2::math::Vector3 end   = m_pOnewayData->position[i+1];
    drawLine( start, end, color );
  }
}

/**
 *  @brief  スコアの描画
 */
void DebugInfo::drawScore( void )
{
  // 有効化されていない
  if( !IsEnableScore() ){ return; }
  // データがない
  gfl2::util::DrawUtilText *pUtilText = Field::Debug::DebugTypes::s_pDrawUtilTextUpper;
  if( !pUtilText ){ return; }

  gfl2::math::Vector4 color( 1.0f, 0.0f, 0.75f, 1.0f );
  pUtilText->SetTextScale( 0.8f, 0.8f );
  pUtilText->SetTextColor( color );
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
  pUtilText->DrawText( 20, 28, L"Total:%f", m_fScore_Total );
  pUtilText->DrawText( 20, 42, L"Rand :%f", m_fScore_Rand );
  pUtilText->DrawText( 20, 56, L"Spot :%f", m_fScore_Spot );

  pUtilText->DrawText( 300,  30+ 0, L"Bas:%f", m_rScore_Poke[0].basepoint );
  pUtilText->DrawText( 300,  30+14, L"App:%f", m_rScore_Poke[0].motion );
  pUtilText->DrawText( 300,  30+28, L"Dir:%f", m_rScore_Poke[0].dir );
  pUtilText->DrawText( 300,  30+42, L"Scr:%f", m_rScore_Poke[0].screen );
  pUtilText->DrawText( 300,  30+56, L"Out:%f", m_rScore_Poke[0].out );

  pUtilText->DrawText( 300, 130+ 0, L"Bas:%f", m_rScore_Poke[1].basepoint );
  pUtilText->DrawText( 300, 130+14, L"App:%f", m_rScore_Poke[1].motion );
  pUtilText->DrawText( 300, 130+28, L"Dir:%f", m_rScore_Poke[1].dir );
  pUtilText->DrawText( 300, 130+42, L"Scr:%f", m_rScore_Poke[1].screen );
  pUtilText->DrawText( 300, 130+56, L"Out:%f", m_rScore_Poke[1].out );
#endif
}

/**
 *  @brief  コメント用パラメータの描画
 */
void DebugInfo::drawComment( void )
{
  // 有効化されていない
  if( !IsEnableScore() ){ return; }
  if( !IsEnableComment() ){ return; }
  // データがない
  gfl2::util::DrawUtilText *pUtilText = Field::Debug::DebugTypes::s_pDrawUtilTextUpper;
  if( !pUtilText ){ return; }

  gfl2::math::Vector4 color( 1.0f, 0.0f, 0.75f, 1.0f );
  pUtilText->SetTextScale( 0.8f, 0.8f );
  pUtilText->SetTextColor( color );
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
  pUtilText->DrawText( 20, 80+ 0, L"SpotType:%d", m_rComment_Param.spot_type );
  switch( m_rComment_Param.time_zone )
  {
  case GameSys::TimeZone::MORNING:
    pUtilText->DrawText( 20, 80+14, L"TimeZone:MORNING" );
    break;
  case GameSys::TimeZone::NOON:
    pUtilText->DrawText( 20, 80+14, L"TimeZone:NOON" );
    break;
  case GameSys::TimeZone::EVENING:
    pUtilText->DrawText( 20, 80+14, L"TimeZone:EVENING" );
    break;
  case GameSys::TimeZone::NIGHT:
    pUtilText->DrawText( 20, 80+14, L"TimeZone:NIGHT" );
    break;
  case GameSys::TimeZone::MIDNIGHT:
    pUtilText->DrawText( 20, 80+14, L"TimeZone:MIDNIGHT" );
    break;
  }
  switch( m_rComment_Param.weather )
  {
  case Field::weather::SUNNY:
    pUtilText->DrawText( 20, 80+28, L"Weather :SUNNY" );
    break;
  case Field::weather::CLOUDINESS:
    pUtilText->DrawText( 20, 80+28, L"Weather :CLOUDINESS" );
    break;
  case Field::weather::RAIN:
    pUtilText->DrawText( 20, 80+28, L"Weather :RAIN" );
    break;
  case Field::weather::THUNDERSTORM:
    pUtilText->DrawText( 20, 80+28, L"Weather :THUNDERSTORM" );
    break;
  case Field::weather::SNOW:
    pUtilText->DrawText( 20, 80+28, L"Weather :SNOW" );
    break;
  case Field::weather::SNOWSTORM:
    pUtilText->DrawText( 20, 80+28, L"Weather :SNOWSTORM" );
    break;
  case Field::weather::DRY:
    pUtilText->DrawText( 20, 80+28, L"Weather :DRY" );
    break;
  case Field::weather::SANDSTORM:
    pUtilText->DrawText( 20, 80+28, L"Weather :SANDSTORM" );
    break;
  case Field::weather::MIST:
    pUtilText->DrawText( 20, 80+28, L"Weather :MIST" );
    break;
  case Field::weather::SUNSHOWER:
    pUtilText->DrawText( 20, 80+28, L"Weather :SUNSHOWER" );
    break;
  case Field::weather::DIAMONDDUST:
    pUtilText->DrawText( 20, 80+28, L"Weather :DIAMONDDUST" );
    break;
  }
#endif
}

/**
 *  @brief  ラインの描画
 */
void DebugInfo::drawLine( const gfl2::math::Vector3 &start, const gfl2::math::Vector3 &end, const gfl2::math::Vector4 &color )
{
  gfl2::math::Vector3 top = start + gfl2::math::Vector3( 0.0f, 5.0f, 0.0f );

  m_pVertex[0].pos   = start;
  m_pVertex[0].color = color;
  m_pVertex[1].pos   = end;
  m_pVertex[1].color = color;
  m_pVertex[2].pos   = top;
  m_pVertex[2].color = color;

  gfl2::gfx::DrawUtil::DrawTriangles( m_pVertex, 3, false );
}

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(Field)
#endif // PM_DEBUG
