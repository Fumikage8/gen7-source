//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterMethodEncountGrass.cpp
 *	@brief  エンカウント草エフェクトの生成
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/MoveModel/Effect/FieldMoveModelEffectCreaterMethodEncountGrass.h"

#include "Field/FieldStatic/include/FieldWorld.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectEncGrass.h"

#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

#include "Field/FieldStatic/include/FieldAttribute.h"

GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(MoveModel);

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
 *  @brief  コンストラクタ
 */
FieldMoveModelEffectCreaterMethodEncountGrass::FieldMoveModelEffectCreaterMethodEncountGrass() : 
  m_pTerrainManager(NULL)
  ,m_pPlayer(NULL)
  ,m_pEffectManager(NULL)
  ,m_OldPos(0,0,0)
  ,m_OldBlockX(-1)
  ,m_OldBlockZ(-1)
  ,m_OldGridX(-1)
  ,m_OldGridZ(-1)
  ,m_OldFloorIndex(-1)
{
}

/**
 *  @brief  デストラクタ
 */
FieldMoveModelEffectCreaterMethodEncountGrass::~FieldMoveModelEffectCreaterMethodEncountGrass()
{
}



/**
 *  @brief  初期化
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::Initialize( Terrain::TerrainManager* pTerrainManager, Effect::EffectManager* pEffectManager, MoveModel::FieldMoveModel* pPlayer )
{
  m_pEffectManager = pEffectManager;
  m_pPlayer        = pPlayer;
  m_pTerrainManager= pTerrainManager;

  for( s32 i=0; i<EFFECT_MAX; ++i )
  {
    m_pEffectArray[i] = NULL;
  }
}

/**
 *  @brief  破棄処理
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::Terminate()
{
  for( s32 i=0; i<EFFECT_MAX; ++i )
  {
    if( m_pEffectArray[i] )
    {
      m_pEffectManager->DeleteEffect(m_pEffectArray[i]);
      // 無効化して終わる。
      m_pEffectArray[i] = NULL;
    }
  }
}



/** 
 *  @brief  ジャンプしたタイミングのエフェクト生成
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::CreateTimingJump()
{
  // 不要なエフェクトを破棄
 this->DeleteEffect();

  s32 blockX, blockZ, gridX, gridZ, floorIndex;

  // グリッド座標の取得
  if( this->GetBlockAndGridPos( &blockX, &blockZ, &gridX, &gridZ, &floorIndex ) )
  {
    // アトリビュート取得
    if( this->IsAttrGrass() )
    {
      // 生成
      this->CreateEffect( blockX, blockZ, gridX, gridZ, floorIndex );
    }
  }
  // 生成座標の更新
  this->UpdateBlockAndGridPos( blockX, blockZ, gridX, gridZ, floorIndex );
}

/** 
 *  @brief  移動したタイミングのエフェクト生成
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::CreateTimingMove()
{
  // 不要なエフェクトを破棄
 this->DeleteEffect();

  s32 blockX, blockZ, gridX, gridZ, floorIndex;

  // グリッド座標の取得
  if( this->GetBlockAndGridPos( &blockX, &blockZ, &gridX, &gridZ, &floorIndex ) )
  {
    // 座標が変わったかチェック
    if( this->IsBlockAndGridChange( blockX, blockZ, gridX, gridZ, floorIndex ) )
    {
      // アトリビュート取得
      if( this->IsAttrGrass() )
      {
        // 生成
        this->CreateEffect( blockX, blockZ, gridX, gridZ, floorIndex );
      }
    }
  }

  // 生成座標の更新
  this->UpdateBlockAndGridPos( blockX, blockZ, gridX, gridZ, floorIndex );
}

/** 
 *  @brief  ターンしたタイミングのエフェクト生成
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::CreateTimingTurn()
{
}

/** 
 *  @brief  現座標でのブロックとグリッド座標を更新
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::UpdateBlockAndGridPos()
{
  // グリッド座標の取得
  s32 blockX, blockZ, gridX, gridZ, floorIndex;
  GetBlockAndGridPos( &blockX, &blockZ, &gridX, &gridZ, &floorIndex );
  // 生成座標の更新
  UpdateBlockAndGridPos( blockX, blockZ, gridX, gridZ, floorIndex );
}

//=====================================================================
//  private
//=====================================================================
/**
 *  @brief  グリッド座標、ブロック座標の取得
 */
bool FieldMoveModelEffectCreaterMethodEncountGrass::GetBlockAndGridPos( s32* pBlockX, s32* pBlockZ, s32* pGridX, s32* pGridZ, s32* pFloorIndex )
{
  // プレイヤーの見た目があるかチェック
  if( m_pPlayer->GetCharaDrawInstance() == NULL ) return false;

  // 主人公の座標を取得
  gfl2::math::SRT srt = m_pPlayer->GetLocalSRT();

  // ブロック座標の取得
  m_pTerrainManager->GetBlockIndex( srt.translate, pBlockX, pBlockZ ); 

  // 無効値を入れる。
  *pGridX = -1;
  *pGridZ = -1;

  // ブロック座標から所属するマップブロックを取得
  *pFloorIndex = -1;
  f32 min_y = 0.0f;
  for( u32 i=0; i<m_pTerrainManager->GetKusaModelFloorNum(); ++i )
  {
    KusaModel::FieldKusaModel* pKusaModel = m_pTerrainManager->GetKusaModel( *pBlockX, *pBlockZ, i );
    if( pKusaModel )
    {
      KusaModel::FieldKusaModel::KusaGrid grid = pKusaModel->GetKusaGrid( srt.translate );

      gfl2::math::Vector3 worldPos = pKusaModel->GetKusaWorldPos( grid );
      f32 now_diff_y = gfl2::math::Abs( worldPos.y - srt.translate.y );

      if( (*pFloorIndex == -1) || (min_y > now_diff_y) ){
        *pGridX = grid.grid_x;
        *pGridZ = grid.grid_y;
        *pFloorIndex = i;
        min_y = now_diff_y;
      }
    }
  }

  if( *pFloorIndex == -1 ){
    return false;
  }
  return true;
}

/**
 *  @brief  グリッド座標、ブロック座標が変わっているかチェック
 *  @retval true  変わった
 *  @retval false 変わらない
 *
 */
bool FieldMoveModelEffectCreaterMethodEncountGrass::IsBlockAndGridChange( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex ) const
{
  // ブロック座標とグリッド座標が変わっているかチェック
  if( (m_OldBlockX == BlockX) &&
    (m_OldBlockZ == BlockZ) &&
    (m_OldGridX == GridX) &&
    (m_OldGridZ == GridZ) )
  {
    return false;
  }
  return true;
}

/**
 *  @brief  アトリビュートが草かチェック
 */
bool FieldMoveModelEffectCreaterMethodEncountGrass::IsAttrGrass( void ) const
{
  u32 attr = m_pPlayer->GetGroundAttribute();

  return Attribute::IsGrass( attr );
}

// エフェクトのタイプを取得
u32 FieldMoveModelEffectCreaterMethodEncountGrass::GetEffectType(void) const
{
  u32 attr = m_pPlayer->GetGroundAttribute();
  switch(attr)
  {
  case ATTR_KUSAMURA:
    return Effect::EFFECT_TYPE_ENCGRASS01;

  case ATTR_MIJIKAIKUSA:
    return Effect::EFFECT_TYPE_ENCGRASS02;

  case ATTR_NAGAKUSA:
    return Effect::EFFECT_TYPE_ENCGRASS03;

  case ATTR_KAREKUSA:
    return Effect::EFFECT_TYPE_ENCGRASS04;

  case ATTR_FLOWER_Y:
    return Effect::EFFECT_TYPE_ENCGRASS05;

  case ATTR_FLOWER_R:
    return Effect::EFFECT_TYPE_ENCGRASS06;

  case ATTR_KUSAHANA:
    // @fix GFMMCat[341] 主人公が草むらを歩いた際に表示されるエフェクトが異なる箇所がある　＞＞　エフェクトの指定を変更
    return Effect::EFFECT_TYPE_ENCGRASS02;

  default:
    GFL_ASSERT(0);  // 存在しない草
    break;
  }

  return Effect::EFFECT_TYPE_ENCGRASS01;
}

/**
 *  @brief  エフェクト生成
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::CreateEffect( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex )
{
  s32 index = 0;
  // ワークが空いているか？
  for( ; index<EFFECT_MAX; ++index )
  {
    if( m_pEffectArray[index] == NULL )
    {
      break;
    }
  }

  if( index < EFFECT_MAX )
  {
    bool is_hit = false;

    // 座標を求める。
    gfl2::math::Vector3 grassPos;

    // 草モデル取得
    KusaModel::FieldKusaModel* pKusaModel = m_pTerrainManager->GetKusaModel( BlockX, BlockZ, FloorIndex );
    if( pKusaModel ){
      grassPos = pKusaModel->GetKusaWorldPos( GridX, GridZ );
      grassPos.x += KusaModel::FieldKusaModel::GrassSizeX/2.0f;
      grassPos.z += KusaModel::FieldKusaModel::GrassSizeY/2.0f;
    }else{
      is_hit = true;  // 生成しないルートを取る。
    }

    // 同じブロック、グリッドの草がいないか？
    for( u32 i=0; i<EFFECT_MAX; ++i )
    {
      Effect::EffectEncGrass* p_work = reinterpret_cast<Effect::EffectEncGrass*>(m_pEffectArray[i]);

      if(p_work && !p_work->IsEnd()){
        if( reinterpret_cast<Effect::EffectEncGrass*>(m_pEffectArray[i])->IsEqualEncountGrassPos( BlockX, BlockZ, GridX, GridZ, FloorIndex ) )
        {
          // 
          is_hit = true;
          break;
        }
      }
    }

    // 生成
    if( is_hit == false )
    {
      Effect::EffectEncGrass* pEffect = NULL;
      m_pEffectArray[index] = m_pEffectManager->CreateEffect( static_cast<Effect::Type>(this->GetEffectType()), grassPos );
      pEffect = reinterpret_cast<Effect::EffectEncGrass*>(m_pEffectArray[index]);
      if( pEffect )
      {
        pEffect->SetEncountGrassPos( BlockX, BlockZ, GridX, GridZ, FloorIndex );
        pEffect->SetParameter( m_pTerrainManager, m_pPlayer );
      }
    }
  }
}

/**
 *  @brief  エフェクト破棄
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::DeleteEffect()
{
  for( u32 i=0; i<EFFECT_MAX; ++i )
  {
    Effect::EffectEncGrass* p_work = reinterpret_cast<Effect::EffectEncGrass*>(m_pEffectArray[i]);

    if(p_work && p_work->IsEnd()){
      m_pEffectManager->DeleteEffect( m_pEffectArray[i] );
      m_pEffectArray[i] = NULL;
    }
  }
}

/**
 *  @brief  ブロックとグリッド座標を更新
 */
void FieldMoveModelEffectCreaterMethodEncountGrass::UpdateBlockAndGridPos( const s32& BlockX, const s32& BlockZ, const s32& GridX, const s32& GridZ, const s32& FloorIndex )
{
  m_OldBlockX     = BlockX;
  m_OldBlockZ     = BlockZ;
  m_OldGridX      = GridX;
  m_OldGridZ      = GridZ;
  m_OldFloorIndex = FloorIndex;
}


GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);
