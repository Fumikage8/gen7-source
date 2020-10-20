/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEncountActor.cpp
 *  @brief  エンカウント用アクター
 *  @author saita_kazuki
 *  @date   2015.05.31
 */

// Field
#include "Field/FieldRo/include/PlacementData/actor/FieldEncountActor.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "GameSys/include/GameManager.h"

// conv_header
#include <niji_conv_header/field/attribute/attribute_id.h>


GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief  コンストラクタ
 */
FieldEncountActor::FieldEncountActor( void )
  : IFieldActorBase()
  , m_pData( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
FieldEncountActor::~FieldEncountActor( void )
{
}

/**
 *  @brief  初期化
 */
void FieldEncountActor::Initialize( EncountSetupData &setupData )
{
  m_pData = setupData.pEncountData;

  // 衝突用コリジョンの設定
  {
    const void *p_col = setupData.pAccessor->GetCollisionData( *( m_pData ) );

    FieldCollisionAccessor collisionAccessor;
    collisionAccessor.Initialize( p_col );

    IFieldActorBase::SetupData baseSetupData;
    baseSetupData.pAllocator = setupData.pAllocator;
    baseSetupData.pHeap = setupData.pHeap;
    baseSetupData.pScene = setupData.pCollisionScene;
    baseSetupData.pColAccessor = &collisionAccessor;
    baseSetupData.vPos = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f);

    IFieldActorBase::Initialize( baseSetupData);
  }
}

/**
 *  @brief  破棄
 */
void FieldEncountActor::Terminate( void )
{
  IFieldActorBase::Terminate();
}

/**
 *  @brief  追加
 */
void FieldEncountActor::RegistActor( void )
{
  IFieldActorBase::RegistActor();

  m_bIsRegist = true;
}

/**
 *  @brief  削除
 */
void FieldEncountActor::UnregistActor( void )
{
  IFieldActorBase::UnregistActor();

  m_bIsRegist = false;
}

/**
 *  @brief  起動
 */
bool FieldEncountActor::Kick( const KickRequest &req, bool * p_outIsEventCreate )
{
  // イベント起動したかフラグをクリア
  if( p_outIsEventCreate ) *p_outIsEventCreate = false;
  
  if( IsInvalid() ){ return false; }
  return false;
}

/**
 *  @brief  判定
 */
bool FieldEncountActor::IsInvalid( void )
{
  if( !m_pData ){ return true; }

  return false;
}

/**
 *  @brief エンカウントできるか
 *  @param attr チェックするアトリビュート
 *  @retval true エンカウント可
 *  @retval false エンカウント不可
 */
bool FieldEncountActor::IsEncount( u32 attr)
{
  u32 targetAttrBits = this->GetTargetAttributeBits( attr);
  u32 checkAttrBits  = this->MakeTargetAttributeBits( attr);
  if( targetAttrBits & checkAttrBits)
  {
    return true;
  }
  return false;
}

/**
 *  @brief  対象アトリビュートビットフィールド取得
 */
u32 FieldEncountActor::GetTargetAttributeBits( u32 attr)
{
  if( attr == ATTRIBUTE_ERROR || attr == 0xFFFFFFFF)
  {
    return 0;
  }

  for( u32 i = 0; i < FieldEncountAccessor::TARGET_ATTRIBUTE_ARRAY_SIZE; ++i)
  {
    if( attr < (FieldEncountAccessor::TARGET_ATTRIBUTE_BITS_DIVISION_SIZE * (i+1)))
    {
      return m_pData->targetAttrBits[i];
    }
  }
  GFL_ASSERT( 0); // ここにはこない
  return 0;
}

/**
 *  @brief  対象アトリビュートビットフィールド作成
 */
u32 FieldEncountActor::MakeTargetAttributeBits( u32 attr)
{
  if( attr == ATTRIBUTE_ERROR || attr == 0xFFFFFFFF)
  {
    return 0;
  }

  for( u32 i = 0; i < FieldEncountAccessor::TARGET_ATTRIBUTE_ARRAY_SIZE; ++i)
  {
    u32 division = FieldEncountAccessor::TARGET_ATTRIBUTE_BITS_DIVISION_SIZE * (i + 1);
    if( attr < division)
    {
      u32 offset = FieldEncountAccessor::TARGET_ATTRIBUTE_BITS_DIVISION_SIZE * i;
      return ( 0x01 << (attr - offset) );
    }
  }
  GFL_ASSERT( 0); // ここにはこない
  return 0;
}

GFL_NAMESPACE_END( Field );
