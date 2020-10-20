/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectEncGrass.cpp
 *  @brief  フィールドエフェクト：草エフェクト
 *  @author takahashi tomoya
 *  @date   2015.06.09
 */

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include "System/include/GflUse.h"

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/KusaModel/FieldKusaModel.h"

#include "Field/FieldRo/include/Effect/content/FieldEffectEncGrass.h"

#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 *  @brief  コンストラクタ
 */
EffectEncGrass::EffectEncGrass( void )
  :IEffectBase()
  ,m_pAllocator( NULL )
  ,m_Status(ST_NULL)
  ,m_isEffectEnd(false)
  ,m_isEnd(false)
{
  for( u32 i=0; i<IDX_MAX; ++i )
  {
    m_pModelResourceRootNode[i] = NULL;
    m_pMotionResourceRootNode[i] = NULL;
    m_pModelInstanceNode[i] = NULL;
  }
  m_pTerrainManager = NULL;

  m_isAnimeNone = false;
  m_OldLength = 1000.0f;

  m_Frame = 0.0f;
  m_StepFrame = 0.0f;
  m_StartFrame = 0.0f;
  m_EndFrame = 0.0f;

}

/**
 *  @brief  デストラクタ
 */
EffectEncGrass::~EffectEncGrass( void )
{
}

/**
 *  @brief  初期化
 */
void EffectEncGrass::Initialize( SetupData &setupData )
{
  // エンカウント草カラーテーブルの取得
  {
    const ColorTableArray* cpColorTableArray;
    const ColorTable * cpTable = NULL;


    cpColorTableArray = static_cast<const ColorTableArray*>(setupData.cpColorTable);

    m_cpColorTable = NULL;

    for( u32 i=0; i<cpColorTableArray->num; ++i )
    {
      cpTable = &cpColorTableArray->data[i];
      
      if( cpTable->worldID == setupData.worldID )
      {
        m_cpColorTable = cpTable;
        break;
      }
    }
  }

  m_OnLengthType = setupData.onLengthType;
  if(m_OnLengthType >= ON_LENGTH_TYPE_MAX)
  {
    GFL_ASSERT( m_OnLengthType < ON_LENGTH_TYPE_MAX );
    m_OnLengthType = ON_LENGTH_TYPE_NORMAL;
  }

  // アロケータ作成
  m_pAllocator = GFL_NEW( setupData.pHeap ) gfl2::util::GLHeapAllocator( setupData.pHeap );
  m_pPipeLine = setupData.pPipeLine;

  setupBallon( IDX_MODEL, setupData );

#if PM_DEBUG

  gfl2::animation::AnimationController::AnimationSlotList list = m_Model[IDX_MODEL].GetAnimationController()->GetAnimationSlotList(0);
  // アニメーションがあるかチェック
  if( (list.GetSlot( gfl2::animation::AnimationController::ANIMATION_TYPE_JOINT ) == NULL) &&
      (list.GetSlot( gfl2::animation::AnimationController::ANIMATION_TYPE_MATERIAL ) == NULL) &&
      (list.GetSlot( gfl2::animation::AnimationController::ANIMATION_TYPE_VISIBILITY ) == NULL) )
  {
    m_isAnimeNone = true;
  }
#endif // PM_DEBUG

  // ランダムで2割りで出す。
  u32 rand = System::GflUse::GetPublicRand( 1000 );
  if( rand < 300 ){
    setupBallon( IDX_EFFECT, setupData );
    m_Model[IDX_EFFECT].SetVisible(true);
  }

  m_Frame = 0.0f;
  m_StepFrame = 1.0f;
  m_StartFrame = 0.0f;
  m_EndFrame = 3.0f;
}

/**
 *  @brief  破棄
 */
bool EffectEncGrass::Terminate( void )
{
  for( u32 i=0; i<IDX_MAX; ++i )
  {
    if( m_Model[i].IsCreated() )
    {
      m_pPipeLine->RemoveEdgeRenderingTarget( m_Model[i].GetModelInstanceNode() );
      m_Model[i].Destroy();
    }

    if(m_pModelResourceRootNode[i]){
      m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode[i] );
    }
    GFL_SAFE_DELETE( m_pModelResourceRootNode[i] );
    if(m_pMotionResourceRootNode[i]){
      m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode[i] );
    }
    GFL_SAFE_DELETE( m_pMotionResourceRootNode[i] );
  }

  GFL_SAFE_DELETE( m_pAllocator );
  return true;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectEncGrass::Delete( void )
{
  for( u32 i=0; i<IDX_MAX; ++i )
  {
    if(m_Model[i].IsCreated()){
      m_Model[i].SetVisible( false );
    }
  }

  // @fix NMCat[1590] 11番道路の草むらにて、草の上でライドポケモンに乗ると、草のグラフィックが一部消える
  // ブロックの草を再度表示
  if( (m_Status > ST_NULL) && (m_Status < ST_END) )
  {
    // 足元の草を出現
    KusaModel::FieldKusaModel* pKusaModel = m_pTerrainManager->GetKusaModel( m_BlockX, m_BlockZ, m_FloorIndex );
    if(pKusaModel){
      pKusaModel->SetShapeDrawFlag( m_GridX, m_GridZ, true );
    }
  }
}


/**
 *  @brief  エンカウント草のポジションを設定
 */
void EffectEncGrass::SetEncountGrassPos( s32 BlockX, s32 BlockZ, s32 GridX, s32 GridZ, s32 FloorIndex )
{
  m_BlockX = BlockX;
  m_BlockZ = BlockZ;
  m_GridX  = GridX;
  m_GridZ  = GridZ;
  m_FloorIndex = FloorIndex;
}

/**
 *  @brief  エンカウント草のポジションが一致しているかチェック
 */
bool EffectEncGrass::IsEqualEncountGrassPos( const s32 BlockX, const s32 BlockZ, const s32 GridX, const s32 GridZ, const s32 FloorIndex ) const
{
  if( (BlockX == m_BlockX) &&
      (BlockZ == m_BlockZ) &&
      (GridX  == m_GridX) &&
      (GridZ  == m_GridZ) &&
      (FloorIndex == m_FloorIndex) )
  {
    return true;
  }
  return false;
}


/**
 *  @brief パラメーターを設定 
 */
void EffectEncGrass::SetParameter( Terrain::TerrainManager* pTerrain, MoveModel::FieldMoveModel* pPlayer )
{
  m_pTerrainManager = pTerrain;
  m_pPlayer         = pPlayer;
}


/**
 *  @brief  終了のチェック
 */
bool EffectEncGrass::IsEnd() const
{
  return m_isEnd && m_isEffectEnd;
}

/**
 *  @brief  初期化
 */
void EffectEncGrass::setupBallon( u32 index, SetupData &setupData )
{
  m_pFieldRootNode = setupData.pRootNode;

  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );
  // モデルを作る
  void *pModelData = binLinkerAccessor.GetData( 0 + index );
  createModel( index, pModelData );
  m_Model[index].Create( m_pAllocator, setupData.pHeap, m_pModelInstanceNode[index] );
  m_Model[index].SetVisible(false);

  // レンダリング対象に追加
  setupData.pPipeLine->AddEdgeRenderingTarget(m_Model[index].GetModelInstanceNode());

  // モーションを作る
  void *pMotionData = binLinkerAccessor.GetData( 2 + index );
  createMotion( index, pMotionData );
  m_Model[index].ChangeAnimationByResourceNode( m_pMotionResourceRootNode[index] );
  m_Model[index].SetAnimationStepFrame(1.0f);
  //m_Model[index].SetAnimationLoop(true);

  // 座標を指定
  m_Model[index].SetPosition( setupData.vPos );

  // 色調整
  if( m_cpColorTable )
  {
    Type type = this->GetType();
    s32 material_index;
    switch(type)
    {
    case EFFECT_TYPE_ENCGRASS01:  // 通常草
      {
        static const c8* sc_MaterialName[ ] = {
          "enc_color",
        };

        for( u32 i=0; i<GFL_NELEMS(sc_MaterialName); ++i )
        {
          material_index = m_Model[index].GetMaterialIndex( sc_MaterialName[i] );
          if(material_index>=0)
          {
            m_Model[index].SetMaterialColorConstant( material_index, 0, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_R]) / 255.0f, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_G]) / 255.0f, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_B]) / 255.0f, 
              1.0f
              );
          }
        }
      }
      break;

    case EFFECT_TYPE_ENCGRASS03:  // 長草
      {
        static const c8* sc_MaterialName[ ] = {
          "enc_color",
        };

        for( u32 i=0; i<GFL_NELEMS(sc_MaterialName); ++i )
        {
          material_index = m_Model[index].GetMaterialIndex( sc_MaterialName[i] );
          if(material_index>=0)
          {
            m_Model[index].SetMaterialColorConstant( material_index, 0, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_R]) / 255.0f, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_G]) / 255.0f, 
              static_cast<f32>(m_cpColorTable->color[COLOR_IDX_G_B]) / 255.0f, 
              1.0f
              );
          }
        }
      }
      break;
    }
  }
}

/**
 *  @brief  更新
 */
void EffectEncGrass::updateNormal( void )
{
  static const f32 OnLength[ ON_LENGTH_TYPE_MAX ][ 2 ] = 
  {
    { 50.0f, 100.0f },  // ON_LENGTH_TYPE_NORMAL 通常草
    { 50.0f,  80.0f },  // ON_LENGTH_TYPE_SHORT 短い草
    { 60.0f, 100.0f },  // ON_LENGTH_TYPE_YELLOW  黄色いくさ
  };

  f32 ON_LENGTH = 0.0f;
  u32 SPEED_INDEX = 0;

  // プレイヤーのスピードを求める。
  if( m_pPlayer->GetCharaDrawInstance() )
  {
    static f32 RUN_SPEED = 427.0f;
    gfl2::math::Vector3 move = m_pPlayer->GetCharaDrawInstance()->GetWalkSpeed(-1);
    if( move.LengthSq() >= RUN_SPEED )
    {
      SPEED_INDEX = 1;
    }
  }

  ON_LENGTH = OnLength[ m_OnLengthType ][ SPEED_INDEX ];

#if PM_DEBUG
  // 初期値と違う場合は、設定
  if(FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENCGRASS_ON_LENGTH ) != Field::Debug::DebugTypes::GetSwitchInitialValue( Debug::DebugTypes::Switch::ENCGRASS_ON_LENGTH ) )
  {
    ON_LENGTH = static_cast<f32>(FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENCGRASS_ON_LENGTH ) );
  }
#endif


  f32 length;
  
  if( m_pPlayer->GetCharaDrawInstance() )
  {
    length =  gfl2::math::Abs( (m_Model[IDX_MODEL].GetPosition() - m_pPlayer->GetLocalSRT().translate).Length() );
  }
  else
  {
    length =  m_OldLength;
  }

  switch( m_Status )
  {
  // 初期状態
  case ST_NULL:
    if( length < ON_LENGTH )
    {
      // 足元の草を消す。
      KusaModel::FieldKusaModel* pKusaModel = m_pTerrainManager->GetKusaModel( m_BlockX, m_BlockZ, m_FloorIndex );
      if(pKusaModel){
        pKusaModel->SetShapeDrawFlag( m_GridX, m_GridZ, false );
      }
      m_Model[IDX_MODEL].SetVisible(true);

      // アニメーション開始
      startOnAnimation();
      m_Status = ST_ON;
    }
    else
    {
      m_Status = ST_END;
    }
    break;

  case ST_ON:
    if( length >= ON_LENGTH )
    {
      // OFF
      startOffAnimation();
      m_Status = ST_OFF;
    }
    else
    {
      // アニメーション完了待ち
      if( m_isAnimeNone || isAnimationEndFrame() )
      {
        m_Status = ST_KEEP;
      }
    }
    break;

  case ST_KEEP:
    // 踏んだまま
    if( length >= ON_LENGTH  )
    {
      // OFFアニメーション開始
      startOffAnimation();
      m_Status = ST_OFF;
    }
    break;

  case ST_OFF:
    if( length < ON_LENGTH )
    {
      // 再度ON
      startOnAnimation();
      m_Status = ST_ON;
    }
    else
    {
      // アニメーション完了待ち
      if( m_isAnimeNone || isAnimationEndFrame() )
      {
        m_Status = ST_END_OFF;
      }
    }
    break;

  case ST_END_OFF:
    {
      m_Model[IDX_MODEL].SetVisible(false);

      // 足元の草を出現
      KusaModel::FieldKusaModel* pKusaModel = m_pTerrainManager->GetKusaModel( m_BlockX, m_BlockZ, m_FloorIndex );
      if(pKusaModel){
        pKusaModel->SetShapeDrawFlag( m_GridX, m_GridZ, true );
      }

      m_Status = ST_END;
    }
    break;

  case ST_END:
    {
      // 終了
      m_isEnd = true;
    }
    break;
  }

  // エフェクトは再生しきったら終わり
  if( m_isEffectEnd == false ){
    if(m_Model[IDX_EFFECT].IsCreated()){
      if( m_Model[IDX_EFFECT].IsAnimationLastFrame() )
      {
        m_Model[IDX_EFFECT].SetVisible(false);
        m_isEffectEnd = true;
      }
    }
    else
    {
      m_isEffectEnd = true;
    }
  }


  // 座標更新
  updatePosition();

  for( u32 i=0; i<IDX_MAX; ++i )
  {
    if(m_Model[i].IsCreated()){
      m_Model[i].UpdateAnimation();
    }
  }

  // フレーム更新
  updateAnimationFrame();

  // 距離を保存
  m_OldLength =  length;
}

/**
 *  @brief  モデルの作成
 */
void EffectEncGrass::createModel( u32 index, void *pModelBinary )
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pModelBinary ) );
  m_pModelResourceRootNode[index] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode[index] );

  // モデルインスタンス作成
  m_pModelInstanceNode[index] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pAllocator, m_pModelResourceRootNode[index] );
  m_pFieldRootNode->AddAfterTransChild( m_pModelInstanceNode[index] );
}

/**
 *  @brief  モーションの作成
 */
void EffectEncGrass::createMotion( u32 index, void *pMotionBinary )
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( pMotionBinary ));
  m_pMotionResourceRootNode[index] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData );
  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode[index] );
}

const f32 CENTER_FRAME = 3.0f;
const f32 END_FRAME = 6.0f;

/**
 *  @brief  ONアニメーション開始
 */
void EffectEncGrass::startOnAnimation( void )
{
  f32 frame = m_Model[IDX_MODEL].GetAnimationFrame();
  m_StartFrame = 0.0f;
  m_EndFrame = 3.0f;
  m_StepFrame = 1.0f;
  m_Model[IDX_MODEL].SetAnimationStepFrame( 0.0f );

  TOMOYA_PRINT( "before %f\n", frame );
  if(frame > CENTER_FRAME) frame = END_FRAME - frame;
  TOMOYA_PRINT( "after %f\n", frame );

  m_Frame= frame;
  m_Model[IDX_MODEL].SetAnimationFrame( frame );
}

/**
 *  @brief  OFFアニメーション開始
 */
void EffectEncGrass::startOffAnimation( void )
{
  f32 frame = m_Model[IDX_MODEL].GetAnimationFrame();
  m_StartFrame = CENTER_FRAME;
  m_EndFrame = END_FRAME;
  m_StepFrame = 1.0f;
  m_Model[IDX_MODEL].SetAnimationStepFrame( 0.0f );

  TOMOYA_PRINT( "before %f\n", frame );
  if(frame < CENTER_FRAME) frame = END_FRAME - frame;
  TOMOYA_PRINT( "after %f\n", frame );

  m_Frame = frame;
  m_Model[IDX_MODEL].SetAnimationFrame( frame );
}


/**
 *  @brief アニメーションフレームを進める
 */
void EffectEncGrass::updateAnimationFrame( void )
{
  if( m_Frame + m_StepFrame <= m_EndFrame )
  {
    m_Frame += m_StepFrame;
  }
  else
  {
    m_Frame = m_EndFrame;
  }

  m_Model[IDX_MODEL].SetAnimationFrame( m_Frame );
}

/**
 *  @brief アニメーションフレームが最終フレームかチェック
 */
bool EffectEncGrass::isAnimationEndFrame( void ) const
{
  if( m_Frame >= m_EndFrame )
  {
    return true;
  }
  return false;
}

/**
 *  @brief  座標更新
 */
void EffectEncGrass::updatePosition( void )
{
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
