//======================================================================
/**
 * @file FieldMotionSignal.cpp
 * @date 2015/07/27 17:49:40
 * @author miyachi_soichi
 * @brief アニメーション連動イベント発行
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/MotionSignal/FieldMotionSignal.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 *  @class  MotionSignal
 */
/**
 *  @brief  コンストラクタ
 */
MotionSignal::MotionSignal( void )
: m_FreezeCnt( 0 )
, m_bPause( false )
{
}

/**
 *  @brief  デストラクタ
 */
MotionSignal::~MotionSignal( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 *  @param  pResource データ
 */
void MotionSignal::Initialize( const void *pResource )
{
  if( !pResource ){ return; }

  const SignalEventDataHeader *header = reinterpret_cast<const SignalEventDataHeader*>( pResource );
  for( u32 i = 0; i < SIGNAL_EVENT_DATA; ++ i )
  {
    SignalEventData &dat = m_aData[ i ];
    if( i < header->num )
    {
      dat.motion_id     = header->data[ i ].motion_id;
      dat.signal_type   = header->data[ i ].signal_type;
      dat.value         = header->data[ i ].value;
      dat.frame         = header->data[ i ].frame;
    }
    else
    {
      dat.motion_id     = 0;
      dat.signal_type   = SIGNAL_TYPE_NONE;
      dat.value         = 0;
      dat.frame         = 0.0f;
    }
  }
}

/**
 *  @brief  解放
 */
void MotionSignal::Terminate( void )
{
}

/**
 *  @brief  更新
 *  @param  pParent   親
 */
void MotionSignal::Update( Field::MoveModel::FieldMoveModel *pParent )
{
  if( IsPause() ){ return; }

  if( IsFreeze() )
  {
    -- m_FreezeCnt;
    if( !IsFreeze() )
    {
      // 再起動
      resetFreeze( pParent );
    }
    return;
  }

  poke_3d::model::CharaModel *pChara = pParent->GetCharaDrawInstance();
  u32 animeID = pChara->GetAnimationId();
  f32 step = pChara->GetAnimationStepFrame();

  // Old To Now区間にて起動しているイベントを列挙
  f32 old_frame = pChara->GetAnimationFrame();
  f32 now_frame = old_frame + step;

  for( u32 i = 0; i < SIGNAL_EVENT_DATA; ++ i )
  {
    const SignalEventData &dat = m_aData[ i ];
    // ダメなデータをとばす
    if( dat.signal_type == SIGNAL_TYPE_NONE ){ continue;}
    if( dat.motion_id != animeID ){ continue; }
    if( dat.frame < old_frame ){ continue; }
    if( now_frame <= dat.frame ){ continue; }

    // 現在起動すべきイベントであると保証された
    raiseEvent( pParent, dat.signal_type, dat.value );
  }
}

/**
 *  @brief  登録
 *  @param  idx     配列番号
 *  @param  animeID モーション番号
 *  @param  type    起動するシグナルの種別
 *  @param  value   シグナルに必要なパラメータ
 *  @param  frame   起動するフレーム
 */
void MotionSignal::RegistEvent( u32 idx, u32 animeID, u32 type, s32 value, f32 frame )
{
  if( SIGNAL_EVENT_DATA <= idx ){ return; }

  m_aData[ idx ].motion_id    = animeID;
  m_aData[ idx ].signal_type  = type;
  m_aData[ idx ].value        = value;
  m_aData[ idx ].frame        = frame;
}

/**
 *  @brief  削除
 *  @param  idx     配列番号
 */
void MotionSignal::UnRegistEvent( u32 idx )
{
  if( SIGNAL_EVENT_DATA <= idx ){ return; }

  m_aData[ idx ].motion_id    = 0;
  m_aData[ idx ].signal_type  = SIGNAL_TYPE_NONE;
  m_aData[ idx ].value        = 0;
  m_aData[ idx ].frame        = 0.0f;
}

/**
 *  @brief  データ取得
 *  @param  idx     配列番号
 */
const SignalEventData *MotionSignal::GetData( u32 idx )
{
  if( SIGNAL_EVENT_DATA <= idx ){ return NULL; }

  return &( m_aData[ idx ] );
}

#if PM_DEBUG
/**
 *  @brief  デバッグ用：クリア
 */
void MotionSignal::Debug_Clear( void )
{
  for( u32 i = 0; i < SIGNAL_EVENT_DATA; ++ i )
  {
    SignalEventData &dat = m_aData[ i ];
    dat.motion_id     = 0;
    dat.signal_type   = SIGNAL_TYPE_NONE;
    dat.value         = 0;
    dat.frame         = 0.0f;
  }
}

/**
 *  @brief  デバッグ用：再初期化
 */
void MotionSignal::Debug_ReInitialize( void *pResource )
{
  if( !pResource ){ return; }

  SignalEventDataHeader *header = static_cast<SignalEventDataHeader*>( pResource );
  for( u32 i = 0; i < SIGNAL_EVENT_DATA; ++ i )
  {
    SignalEventData &dat = m_aData[ i ];
    if( i < header->num )
    {
      dat.motion_id     = header->data[ i ].motion_id;
      dat.signal_type   = header->data[ i ].signal_type;
      dat.value         = header->data[ i ].value;
      dat.frame         = header->data[ i ].frame;
    }
    else
    {
      dat.motion_id     = 0;
      dat.signal_type   = SIGNAL_TYPE_NONE;
      dat.value         = 0;
      dat.frame         = 0.0f;
    }
  }
}
#endif

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Private Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  イベントの処理
 *  @param  pParent   対象者
 *  @param  type      行為
 *  @param  value     どのくらい
 */
void MotionSignal::raiseEvent( Field::MoveModel::FieldMoveModel *pParent, u32 type, s32 value )
{
  switch( type )
  {
  case SIGNAL_TYPE_SPD:
    // モーション速度の調整
    {
      f32 fValue = static_cast<f32>( value ) * 0.01f;
      poke_3d::model::BaseModel *pBaseModel = pParent->GetCharaDrawInstance();
      if( pBaseModel )
      {
        pBaseModel->SetAnimationStepFrame( fValue );
      }
    }
    break;
  case SIGNAL_TYPE_EYE_RATIO:
    // 視線距離の調整
    if( pParent->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_EYE )
    {
      f32 fValue = static_cast<f32>( value ) * 0.01f;
      Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pParent->GetMoveCodeWork();
      u32 size = sizeof(Field::MoveModel::FieldMoveCodeTrainerEye::Work);
      Field::MoveModel::FieldMoveCodeTrainerEye::Work *pWork = 
        (Field::MoveModel::FieldMoveCodeTrainerEye::Work*)Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, size );
      pWork->eyeLengthRate = fValue;
    }
    else if( pParent->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_PATH )
    {
      f32 fValue = static_cast<f32>( value ) * 0.01f;
      Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pParent->GetMoveCodeWork();
      u32 size = sizeof(Field::MoveModel::FieldMoveCodeTrainerPath::Work);
      Field::MoveModel::FieldMoveCodeTrainerPath::Work *pWork = 
        (Field::MoveModel::FieldMoveCodeTrainerPath::Work*)Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, size );
      pWork->eyeLengthRate = fValue;
    }
    break;
  case SIGNAL_TYPE_FREEZE:
    {
      poke_3d::model::BaseModel *pBaseModel = pParent->GetCharaDrawInstance();
      if( pBaseModel )
      {
        m_FreezeCnt = value;
        // 現在の更新を退避してモーション更新を停止する
        m_KeepStep = pBaseModel->GetAnimationStepFrame();
        pBaseModel->SetAnimationStepFrame( 0.0f );
      }
    }
    break;
  }
}

/**
 *  @brief  停止解除
 *  @param  pParent   対象者
 */
void MotionSignal::resetFreeze( Field::MoveModel::FieldMoveModel *pParent )
{
  poke_3d::model::BaseModel *pBaseModel = pParent->GetCharaDrawInstance();
  if( pBaseModel )
  {
    pBaseModel->SetAnimationStepFrame( m_KeepStep );
    m_FreezeCnt = 0;
    m_KeepStep = 0.0f;
  }
}

GFL_NAMESPACE_END(Field)
