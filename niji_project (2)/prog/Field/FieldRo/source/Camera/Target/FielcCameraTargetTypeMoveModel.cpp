/**
* @brief  フィールドカメラのターゲット、動作モデル
* @file   FieldCameraTargetTypeMoveModelPlayer.cpp
* @author ikuta_junya
* @data   2015.05.21
*/


// field
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetTypeMoveModel.h"

// camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"


// test 
#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

// debug
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "Debug/include/gfl2_DebugWinItemTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

static const f32 MOVE_DIRECTION_CHANGE_SPEED( 0.08f );
static const gfl2::math::Vector3 OFFSET_MOVEMODEL_POS_TO_TARGET_POS( 0.0f, 133.0f, 0.0f );    // 動作モデルの足元->実際に基準とする座標

// @fix NMcat[4827]:ムーランドサーチをすると背景の裏側が見える、対処。カメラ座標の稼動域を徒歩状態と同じにするため、XZ平面の変更量をムーランドと徒歩状態のコリジョンの差に変更。
static const f32 OFFSET_MUURAND_SEARCH_DIR( 63.0f );                                          // サーチ中のカメラ変更最大値(XZ平面) 
static const f32 OFFSET_MUURAND_SEARCH_HEIGHT( -33.0f );                                      // サーチ中のカメラ変更最大値(Y) 

struct FutureMoveSetting
{
  f32 moveDistance;
  gfl2::math::Easing::EaseFunc valueEaseFunc;
  s32 valueMoveFrame;
  s32 valueMoveWaitFrame;
  f32 dirChangeSpeed;
  f32 stepFrameForRun;
  

  s32 runChangeFrame;
  s32 runChangeWaitFrame;
  gfl2::math::Easing::EaseFunc runChangeEaseFunc;
  
  s32 walkChangeFrame;
  s32 walkChangeWaitFrame;
  gfl2::math::Easing::EaseFunc walkChangeEaseFunc;
};


struct TargetSetting
{
  f32 movedDistance;
  f32 baseDistance;
  f32 delayDistance;

  s32 returnFrame;
  s32 returnWaitFrame;
  gfl2::math::Easing::EaseFunc returnEaseFunc;

  f32 distanceValueStartValue;
  s32 distanceValueMoveFrame;
  gfl2::math::Easing::EaseFunc distanceValueEaseFunc;
  
  f32 returnToMoveValueStartValue;
  s32 returnToMoveValueMoveFrame;
  gfl2::math::Easing::EaseFunc returnToMoveValueEaseFunc;
  
  
  s32 dirMoveStartWaitFrame;
  s32 dirMoveStartFrame;
  s32 dirMoveWaitFrame;
  s32 dirMoveFrame;
  f32 dirMoveValue;
  f32 dirMoveOneTimeDiff;
  gfl2::math::Easing::EaseFunc dirMoveStartEaseFunc;
  gfl2::math::Easing::EaseFunc dirMoveEaseFunc;

  FutureMoveSetting futureMove;

};

TargetSetting targetSetting =
{
  1.00f,    // movedDistance
  1000.0f, // baseDistance
  40.0f,   // delayDistance

  30,      // returnFrame
  0,        // returnWaitFrame
  gfl2::math::Easing::OUT_CUBIC,// returnEaseFunc

  0.0f,                         // f32 distanceValueStartValue;
  8,                            // s32 distanceValueMoveFrame;
  gfl2::math::Easing::OUT_SINE, // gfl2::math::Easing::EaseFunc distanceValueEaseFunc;

  0.0f,                         // f32 returnToMoveValueStartValue;
  30,                           // s32 returnToMoveValueMoveFrame;
  gfl2::math::Easing::LINEAR,   // gfl2::math::Easing::EaseFunc returnToMoveValueEaseFunc;

  5,      // dirMoveWaitFrame
  50,     // dirMoveFrame
  0,      // dirMoveWaitFrame
  50,     // dirMoveFrame
  0.3f,  // dirMoveValue
  0.03f,  // dirMoveOneTimeDiff
  gfl2::math::Easing::INOUT_CUBIC, // dirMoveStartEaseFunc
  gfl2::math::Easing::INOUT_CUBIC, // dirMoveEaseFunc

  // FutureMoveSetting
  {
    30.0f,                            // moveDistance
    gfl2::math::Easing::INOUT_QUAD,   // valueEaseFunc;
    60,                               // valueMoveFrame;
    15,                               // valueMoveWaitFrame;
    0.08f,                            // dirChangeSpeed
    1.5f,                             // stepFrameForRun

    30,      // runChangeFrame
    0,       // runChangeWaitFrame
    gfl2::math::Easing::OUT_CUBIC,// runChangeEaseFunc

    10,      // walkChangeFrame
    0,       // walkChangeWaitFrame
    gfl2::math::Easing::OUT_CUBIC // walkChangeEaseFunc
  }

};


class TargetTypeMoveModelPlayer::FutureMove
{
public:

  FutureMove() :
    m_State( 0 )
    ,m_IsRun( false )
    ,m_SrcMoveDirection( 0.0f, 0.0f, 0.0f )
    ,m_FixMoveDirection( 0.0f, 0.0f, 0.0f )
    ,m_TargetMoveDirection( 0.0f, 0.0f, 0.0f )
#if PM_DEBUG
    ,m_DebugForceFutureMoveMaxFlag( false )
    ,m_DebugForceFutureMoveDirFlag( false )
    ,m_DebugForceFutureMoveDir( 0.0f, 1.0f, 0.0f )
#endif
  {
    m_DirMoveValueController.Clear();
    m_ValueController.Clear();
    m_RunValueController.Clear();
  }

  ~FutureMove()
  {

  }

  void Update( const gfl2::math::Vector3& rNowPos )
  {
    // 自機の前回移動量

    if( m_State == 0 )
    {
      m_PrevPosition = rNowPos;

      m_ValueController.Move(
        0.0f,
        1.0f,
        targetSetting.futureMove.valueMoveFrame,
        targetSetting.futureMove.valueEaseFunc,
        targetSetting.futureMove.valueMoveWaitFrame
        );

      if( m_IsRun )
      {
        m_RunValueController.Set( 1.0f );
      }
      else
      {
        m_RunValueController.Set( 0.0f );
      }


      m_State = 1;
    }

    m_RunValueController.Update();


    gfl2::math::Vector3 move( rNowPos - m_PrevPosition );
    move.NormalizeRet();

    if( move.Length() >= 0.0f )
    {
      // XZ平面のずれを更新
      if( m_State == 1 )
      {
        // 初回は一瞬で現在方向を反映
        m_FixMoveDirection = move;
        m_State = 2;
      }
      else
      {
        // 方向を滑らかに更新
        m_FixMoveDirection += ( move - m_FixMoveDirection ) * targetSetting.futureMove.dirChangeSpeed;

        // 方向が正円に収まるように正規化
        if( m_FixMoveDirection.Length() > 1.0f )
        {
          m_FixMoveDirection = m_FixMoveDirection.Normalize();
        }
      }
    }

    m_ValueController.Update();
    m_PrevPosition = rNowPos;
  }

  void Reset( void )
  {
    m_ValueController.Clear();
    m_State = 0;
    m_DirMoveValueController.Set( 0.0f );
    m_SrcMoveDirection = m_FixMoveDirection = m_TargetMoveDirection = gfl2::math::Vector3::GetZero();
  }

  void Excute( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rPreFixPositon, gfl2::math::Vector3* position, f32 scale )
  {
    // 3D空間でずらす量を算出( XZ平面でずらす )
    gfl2::math::Vector3 move( m_FixMoveDirection );

    // 係数をかける( サポート係数と設定されている距離によってずれる量が比例する )
    f32 value( m_ValueController.GetValue() );
    move *= ( targetSetting.futureMove.moveDistance * scale * m_RunValueController.GetValue() );
    
    gfl2::math::Vector3 targetPositon( rTargetPosition + move );
    
    *position = rPreFixPositon;
    position->Lerp( targetPositon, m_ValueController.GetValue());

#if PM_DEBUG
    // カメラチェック用デバッグ機能：必ず未来予知をMAXにする
    if( m_DebugForceFutureMoveMaxFlag )
    {
      move = m_FixMoveDirection;
      if( m_DebugForceFutureMoveDirFlag )
      {
        move = m_DebugForceFutureMoveDir;
      }

      move *= ( targetSetting.futureMove.moveDistance * scale );
      *position =  rTargetPosition + move;
    }
#endif
  }


  void SetStepFrame( f32 stepFrame )
  {
    m_ValueController.SetStepFrame( stepFrame );
  }


  void SetRunFlag( b32 isRun )
  {
    if( m_IsRun != isRun )
    {
      m_IsRun = isRun;
      if( m_IsRun )
      {
        m_RunValueController.Move(
          m_RunValueController.GetValue(),
          1.0f,
          targetSetting.futureMove.runChangeFrame,
          targetSetting.futureMove.runChangeEaseFunc,
          targetSetting.futureMove.runChangeWaitFrame
          );
      }
      else
      {
        m_RunValueController.Move(
          m_RunValueController.GetValue(),
          0.0f,
          targetSetting.futureMove.walkChangeFrame,
          targetSetting.futureMove.walkChangeEaseFunc,
          targetSetting.futureMove.walkChangeWaitFrame
          );
      }
    }
  }

#if PM_DEBUG

  // カメラチェック用機能
  void SetDebugForceFutureMoveMaxFlag( b32 flag )
  {
    m_DebugForceFutureMoveMaxFlag = flag;
  }
  void SetDebugForceFutureMoveDirFlag( b32 flag )
  {
    m_DebugForceFutureMoveDirFlag = flag;
  }
  void SetDebugForceFutureMoveDir( const gfl2::math::Vector3& rDir )
  {
    m_DebugForceFutureMoveDir = rDir;
  }

#endif


private:

  s32                                  m_State;
  
  b32                                  m_IsRun;
  Field::Camera::Area::ValueController m_RunValueController;
  
  Field::Camera::Area::ValueController m_ValueController;
  Field::Camera::Area::ValueController m_DirMoveValueController;
  gfl2::math::Vector3                  m_SrcMoveDirection;
  gfl2::math::Vector3                  m_FixMoveDirection;
  gfl2::math::Vector3                  m_TargetMoveDirection;

  gfl2::math::Vector3                  m_PrevPosition;

#if PM_DEBUG
  b32                                  m_DebugForceFutureMoveMaxFlag;    // true:強制未来予知MAX
  b32                                  m_DebugForceFutureMoveDirFlag;    // true:強制未来予知固定
  gfl2::math::Vector3                  m_DebugForceFutureMoveDir;        // 強制固定向き
#endif
};



/**
* @brief コンストラクタ
*
* @param  pMoveModel 中身、寿命は管理しない
*
* @return 無し
*/ 
TargetTypeMoveModelPlayer::TargetTypeMoveModelPlayer( gfl2::heap::HeapBase* pHeap, MoveModel::FieldMoveModelPlayer* pMoveModel ) : 
  m_State( State::READY )
  ,m_pMoveModelPlayer( pMoveModel )
  ,m_ReturnStartPosition( gfl2::math::Vector3::GetZero() )
  ,m_FuturePosition( gfl2::math::Vector3::GetZero() )
  ,m_DelayPositon( gfl2::math::Vector3::GetZero() )
#if PM_DEBUG
  ,m_pDebugWinGroup( NULL )
  ,m_DebugForceMuurandFlag( false )
  ,m_DebugForceDirFlag( false )
  ,m_DebugForceDir( 0.0f, 1.0f, 0.0f )
#endif
{

  m_MuurandOffsetValueController.Set( 0.0f );
  m_MuurandOffsetDir = gfl2::math::Vector2::GetZero();
  m_ReturnValueController.Clear();
  m_ReturnToMoveValueController.Clear();
  m_DelayValueController.Clear();
  m_DistanceValueController.Clear();

  m_pFutureMove = GFL_NEW( pHeap ) FutureMove();
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TargetTypeMoveModelPlayer::~TargetTypeMoveModelPlayer( void )
{
  GFL_SAFE_DELETE( m_pFutureMove );

#if PM_DEBUG
  UnResistDebugMenu();
#endif
}

/**
* @brief カメラがみる座標を更新する
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::Update( void )
{
  if( GetModelPosition( &m_TargetPosition ) == false  )
  {
    return;
  }

#if PM_DEBUG
  // カメラチェック機能更新
  DebugUpdateCameraCheck();
#endif


  // 未来予知の更新速度を更新
  UpdateFutureMoveStepFreme();

  // ブレンド係数更新
  m_DistanceValueController.Update();
  m_ReturnToMoveValueController.Update();

  // カメラ追従座標を遅延させる処理。意図は下記の通り
  // 操作が常にダイレクトに画面に反映されると、ユーザーが認識する情報更新が激しくなるので緩和する
  // 画面の中心移動方向に画面が中心移動方向をわかりやすくする。
  // カメラがおっているような表現にする事で、自分が操作している対象をわかりやすくする
  b32 returnToMoveFlag( false );

  // 状態切り替え処理
  switch( m_State )
  {
    // 初回更新
  case State::READY:
    {
      m_PreFixPosition = m_TargetPosition;
      m_FixPosition = m_TargetPosition;
      m_State = State::NORMAL;

      // @fix GFNMcat[2030]:移動開始時に自機の表示が画面上でブレる、遅延範囲を滑らかに拡大する
      m_DistanceValueController.Move(
        targetSetting.distanceValueStartValue,
        1.0f,
        targetSetting.distanceValueMoveFrame,
        targetSetting.distanceValueEaseFunc
        );
      m_DistanceValueController.Update();

      // そのまま更新
      // return;
    }

  case State::NORMAL:
    {
      // 入力＆自機移動状態の場合は指定距離だけ遅延して追従する
      if( IsPlayerFollow( ( m_PrevTargetPosition - m_TargetPosition ).Length() ) )
      {

      }
      else
      {
        //
        // 移動停止による吸着開始
        //

        // より滑らかにするため、一旦更新する @note 同様のコードが2箇所にある
        {
          m_pFutureMove->Update( m_TargetPosition );

          // カメラが近い場合、画面表示への影響が強くなるので、遅れる距離を小さくする
          Field::Camera::CameraUnit* pCameraUnit = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetCameraManager()->GetMainGamePlayCamera();
          gfl2::math::Vector3 cameraPositon;
          gfl2::math::Vector3 targetPositon;
          pCameraUnit->GetCameraAndTargetPosition( &cameraPositon, &targetPositon );
          f32 cameraDistance = ( cameraPositon - targetPositon ).Length();

          f32 keepLength( targetSetting.delayDistance * m_DistanceValueController.GetValue() );
          f32 keepScale =  ( cameraDistance < targetSetting.baseDistance ) ? ( cameraDistance / targetSetting.baseDistance ) : 1.0f;
          keepLength *= keepScale;

          if( m_ReturnToMoveValueController.IsValid() )
          {
            keepLength = m_ReturnToMoveStartDistance + ( keepLength - m_ReturnToMoveStartDistance ) * m_ReturnToMoveValueController.GetValue();
          }

          // 指定距離以上離れていたら引っ張る
          gfl2::math::Vector3 targetToFix( m_PreFixPosition - m_TargetPosition );
          if( targetToFix.Length() > keepLength )
          {
            if( keepLength == 0.0f )
            {
              m_PreFixPosition = m_TargetPosition;
            }
            else
            {
              m_PreFixPosition = m_TargetPosition + targetToFix.Normalize() * keepLength;
            }
          }

          m_FixPosition = m_PreFixPosition;
          m_pFutureMove->Excute( m_TargetPosition, m_PreFixPosition, &m_FixPosition, keepScale );
        }

        // 吸着ブレンド設定
        this->m_ReturnValueController.Move(
          0.0f,
          1.0f,
          targetSetting.returnFrame,
          targetSetting.returnEaseFunc,
          targetSetting.returnWaitFrame
          );

        // 不用なブレンド係数をクリア
        m_ReturnToMoveValueController.Clear();
        
        // 情報更新
        m_ReturnStartPosition = m_FixPosition;
        m_State = State::RETURN;

      }
      break;
    }

  case State::RETURN:
    {
      // 動き始めたら吸着中止
      if( IsPlayerFollow( ( m_PrevTargetPosition - m_TargetPosition ).Length() ) )
      {
        m_pFutureMove->Reset();
        m_PreFixPosition = m_FixPosition;
        m_State = State::NORMAL;

        // @fix GFNMcat[2030]:移動開始時に自機の表示が画面上でブレる、遅延範囲を滑らかに拡大する
        m_DistanceValueController.Move(
          targetSetting.distanceValueStartValue,
          1.0f,
          targetSetting.distanceValueMoveFrame,
          targetSetting.distanceValueEaseFunc
          );
        m_DistanceValueController.Update();

        // @fix GFNMcat[2034]:カメラ遅延中に自機が移動を開始するとカメラがカクつく、対処。戻り中に移動開始した際の距離と遅延範囲をブレンドする事で滑らかに遷移するようにした
        if( m_ReturnValueController.IsValid() )
        {
          returnToMoveFlag = true;
          m_ReturnToMoveStartDistance = ( m_FixPosition - m_PrevTargetPosition ).Length();
          m_ReturnToMoveValueController.Move(
            targetSetting.returnToMoveValueStartValue,
            1.0f,
            targetSetting.returnToMoveValueMoveFrame,
            targetSetting.returnToMoveValueEaseFunc
            );

          m_ReturnToMoveValueController.Update();
        }
        else
        {
          // 不用なブレンド係数をクリア
          m_ReturnToMoveValueController.Clear();
        }

      }
      break;
    }
  }


  // 状態別更新処理
  switch( m_State )
  {
    // 追従処理
  case State::NORMAL:
    {
      m_pFutureMove->Update( m_TargetPosition );

      // カメラが近い場合、画面表示への影響が強くなるので、遅れる距離を小さくする
      Field::Camera::CameraUnit* pCameraUnit = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetCameraManager()->GetMainGamePlayCamera();
      gfl2::math::Vector3 cameraPositon;
      gfl2::math::Vector3 targetPositon;
      pCameraUnit->GetCameraAndTargetPosition( &cameraPositon, &targetPositon );
      f32 cameraDistance = ( cameraPositon - targetPositon ).Length();
      f32 keepLength( targetSetting.delayDistance * m_DistanceValueController.GetValue() );
      f32 keepScale =  ( cameraDistance < targetSetting.baseDistance ) ? ( cameraDistance / targetSetting.baseDistance ) : 1.0f;
      keepLength *= keepScale;

      // 指定距離以上離れていたら引っ張る
      gfl2::math::Vector3 targetToFix( m_PreFixPosition - m_TargetPosition );

      // @fix GFNMcat[2034]:カメラ遅延中に自機が移動を開始するとカメラがカクつく、対処。戻り中に移動開始した際の距離が遅延範囲を超えていた場合は補間をかける
      if( returnToMoveFlag )
      {
        // 吸着→移動開始した瞬間の距離が追従距離内なら、吸着→移動開始地点からの補間を行わない
        if( m_ReturnToMoveStartDistance <= keepLength )
        {
          m_ReturnToMoveValueController.Clear();
        }
      }
      if( m_ReturnToMoveValueController.IsValid() )
      {
        keepLength = m_ReturnToMoveStartDistance + ( keepLength - m_ReturnToMoveStartDistance ) * m_ReturnToMoveValueController.GetValue();
      }

      // 一定範囲以上離れていたら吸着
      if( targetToFix.Length() > keepLength )
      {
        if( keepLength == 0.0f )
        {
          m_PreFixPosition = m_TargetPosition;
        }
        else
        {
          m_PreFixPosition = m_TargetPosition + targetToFix.Normalize() * keepLength;
        }
      }

      // 未来予測とブレンド
      m_FixPosition = m_PreFixPosition;
      m_pFutureMove->Excute( m_TargetPosition, m_PreFixPosition, &m_FixPosition, keepScale );

      break;
    } // case State::NORMAL:


    // 停止戻り中の処理
  case State::RETURN:
    {
      // 停止時の座標から最新の座標に補間
      if( m_ReturnValueController.IsValid() )
      {
        m_ReturnValueController.Update();
        m_FixPosition = m_ReturnStartPosition;
        m_FixPosition.Lerp( m_TargetPosition, m_ReturnValueController.GetValue() );

        if( m_ReturnValueController.IsEnd() )
        {
          m_ReturnValueController.Clear();
        }
      }
      else
      {
        m_FixPosition = m_TargetPosition;
      }
      break;
    }

    // 完全追従
  case State::DIRECT:
    {
      m_FixPosition = m_TargetPosition;
      break;
    }
  };

  // ムーランドサーチ中にカメラをずらす処理、値の更新
  UpdateMuurando();
  TargetBase::Update();

  // 外部指定遅延更新処理
  m_DelayValueController.Update();
  if( m_DelayValueController.IsEnd() )
  {
    // 完了
    m_DelayValueController.Clear();
  }

  m_PrevTargetPosition = m_TargetPosition;
}

/**
* @brief フィールドの準備が完了した際に実行される関数
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::Ready( void )
{
#if PM_DEBUG
  ResistDebugMenu();
#endif

  if( GetModelPosition( &m_TargetPosition ) )
  {
    m_PrevTargetPosition = m_FixPosition = m_TargetPosition;
    m_State = State::READY;
    m_pFutureMove->Reset();
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

/**
* @brief 補間を打ち切り、最新の状態に更新する
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::Fix( void )
{
  if( GetModelPosition( &m_TargetPosition ) )
  {
    m_PrevTargetPosition = m_FixPosition = m_TargetPosition;
    m_State = State::READY;
    m_pFutureMove->Reset();
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

/**
* @brief 遅延追従を停止する
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::DelayFollowPause( u32 frame )
{
  // 完全追従へ移行
  m_PrevTargetPosition = m_FixPosition = m_TargetPosition;
  m_State = State::DIRECT;
  m_pFutureMove->Reset();
}

/**
* @brief 遅延追従を再開する
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::DelayFollowResume( void )
{
  if( m_State != State::DIRECT )
  {
    return;
  }

  // 再開設定
  if( GetModelPosition( &m_TargetPosition ) )
  {
    m_PrevTargetPosition = m_FixPosition = m_TargetPosition;
    m_State = State::READY;
    m_pFutureMove->Reset();
  }
  else
  {
    GFL_ASSERT( 0 );
  }
}

/**
* @brief 指定フレーム遅らせる
*
* @param  waitFrame     待ちフレーム
* @param  moveFrame     移動フレーム
* @param  easeFunc      移動中のイージング
*
* @return 無し
*/
void TargetTypeMoveModelPlayer::Delay( u32 waitFrame, u32 moveFrame, gfl2::math::Easing::EaseFunc easeFunc )
{
  this->GetCameraTargetPosition( &m_DelayPositon );
  m_DelayValueController.Move( 0.0f, 1.0f, moveFrame, easeFunc, waitFrame );
}

#if PM_DEBUG

void TargetTypeMoveModelPlayer::ResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ取得」→「FieldDebugMenuから直接グループを取得」に変更。
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::debug::DebugWinGroup* pGroup = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup();
  if( pGroup )
  {
    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::debug::DebugWinGroup* pGroupWork = gfl2::debug::DebugWin_AddGroup( pHeap, L"CameraTarget", pGroup );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"movedDistance", &targetSetting.movedDistance, 0.0f, 1000.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"baseDistance", &targetSetting.baseDistance, 0.0f, 1000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"delayDistance", &targetSetting.delayDistance, 0.0f, 1000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"returnFrame", &targetSetting.returnFrame, 0, 30 * 30, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"returnWaitFrame", &targetSetting.returnWaitFrame, 0, 30 * 30, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"returnEaseFunc", &targetSetting.returnEaseFunc );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"distanceValueStartValue", &targetSetting.distanceValueStartValue, 0, 1.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"distanceValueMoveFrame", &targetSetting.distanceValueMoveFrame, 0, 30 * 30, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"distanceValueEaseFunc", &targetSetting.distanceValueEaseFunc );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"returnToMoveValueStartValue", &targetSetting.returnToMoveValueStartValue, 0, 1.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"returnToMoveValueMoveFrame", &targetSetting.returnToMoveValueMoveFrame, 0, 30 * 30, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"returnToMoveValueEaseFunc", &targetSetting.returnToMoveValueEaseFunc );


    // FutureMove
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroupWork, pHeap, L"FutureMove", NULL, NULL );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"moveDistance", &targetSetting.futureMove.moveDistance, 0.0f, 1000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"moveFrame", &targetSetting.dirMoveFrame, 0, 1000, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"moveEaseFunc", &targetSetting.dirMoveEaseFunc);
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"moveDistance", &targetSetting.dirMoveWaitFrame, 0, 1000, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"moveStartFrame", &targetSetting.dirMoveStartFrame, 0, 1000, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"moveStartEaseFunc", &targetSetting.dirMoveStartEaseFunc);
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"moveStartDistance", &targetSetting.dirMoveStartWaitFrame, 0, 1000, 1 );

    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"valueEaseFunc", &targetSetting.futureMove.valueEaseFunc );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"valueMoveFrame", &targetSetting.futureMove.valueMoveFrame, 0, 1000, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"valueMoveWaitFrame", &targetSetting.futureMove.valueMoveWaitFrame, 0, 1000, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"dirChangeSpeed", &targetSetting.futureMove.dirChangeSpeed, 0.0f, 1000.0f, 0.001f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, L"走り状態での未来予測の速度", &targetSetting.futureMove.stepFrameForRun, 0.01f, 1000.0f, 0.01f );
    
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"走りへの遷移フレーム", &targetSetting.futureMove.runChangeFrame, 0, 30 * 30, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"走りへの遷移待ちフレーム", &targetSetting.futureMove.runChangeWaitFrame, 0, 30 * 30, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"走りへのイージング", &targetSetting.futureMove.runChangeEaseFunc );
    
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"歩きへの遷移フレーム", &targetSetting.futureMove.walkChangeFrame, 0, 30 * 30, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, L"歩きへの遷移待ちフレーム", &targetSetting.futureMove.walkChangeWaitFrame, 0, 30 * 30, 1 );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"歩きへのイージング", &targetSetting.futureMove.walkChangeEaseFunc );

    // 記憶
    m_pDebugWinGroup = pGroupWork;
  }
}

void TargetTypeMoveModelPlayer::UnResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ削除」→「記憶したグループクラスを指定して削除」に変更。
  if( m_pDebugWinGroup )
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->GetFieldmap()->GetFieldDebugMenu() && pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup() )
    {
      pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup()->RemoveItem( m_pDebugWinGroup );
      m_pDebugWinGroup = NULL;
    }
    else
    {
      // @note 無いなら親ごと削除されている
      m_pDebugWinGroup = NULL;
    }
  }
}

// カメラチェック機能更新
void TargetTypeMoveModelPlayer::DebugUpdateCameraCheck( void )
{
  // 未来予知を必ずMAX固定にする
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CAMERA_CHECK_FUTURE_MAX ) )
  {
    m_pFutureMove->SetDebugForceFutureMoveMaxFlag( true );
    m_pFutureMove->SetDebugForceFutureMoveDirFlag( true );    // 向きも合わせて固定

    gfl2::math::Vector3 nowDir( 0.0f, 0.0f, 1.0f );
    nowDir = m_pMoveModelPlayer->GetLocalSRT().rotation.Transform( nowDir );
    m_pFutureMove->SetDebugForceFutureMoveDir( nowDir );
  }
  else
  {
    m_pFutureMove->SetDebugForceFutureMoveMaxFlag( false );
    m_pFutureMove->SetDebugForceFutureMoveDirFlag( false );
  }
  
  // 強制方向指定
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CAMERA_CHECK_DIR_FIX ) )
  {
    m_DebugForceDirFlag = true;
  }
  else
  {
    m_DebugForceDirFlag = false;
  }

  // ムーランドサーチのオフセットを必ずMAX固定にする
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CAMERA_CHECK_FORCE_MUURAND ) )
  {
    m_DebugForceMuurandFlag = true;
  }
  else
  {
    m_DebugForceMuurandFlag = false;
  }

  // 強制方向指定、方向更新処理
  if( m_DebugForceDirFlag )
  {
    // タッチパネルの中心点かたタッチした点の向きを設定する
    gfl2::ui::TouchPanel * pTouchPanel;            //!< タッチパネル
    pTouchPanel = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetUiDeviceManager()->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

    if( pTouchPanel->IsTouch() )
    {
      gfl2::math::Vector3 work( pTouchPanel->GetX() - 160, 0.0f, pTouchPanel->GetY() - 120.0f );
      if( work.Length() > 0.001f )
      {
        work.NormalizeRet();
        m_DebugForceDir = work;
      }
    }

    m_pFutureMove->SetDebugForceFutureMoveDir( m_DebugForceDir );
  }

}

#endif

/**
* @brief 座標を取得する
*
* @param  pOut  格納先
*
* @return 成否
*/
b32 TargetTypeMoveModelPlayer::GetModelPosition( gfl2::math::Vector3* pOut ) const
{
  // 強制上書き
  if( this->IsOverWrite() )
  {
    this->GetOverWrite( pOut );
    return true;
  }

  if( m_pMoveModelPlayer == NULL )
  {
    return false;
  }

  *pOut = m_pMoveModelPlayer->GetLocalSRT().translate;
  return true;
}

/**
* @brief カメラがみる座標を取得する
*
* @param  pOut  格納先
*
* @return 成否
*/
b32 TargetTypeMoveModelPlayer::GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const
{
  // 強制上書き
  if( this->IsOverWrite() )
  {
    this->GetOverWrite( pOut );
    *pOut += OFFSET_MOVEMODEL_POS_TO_TARGET_POS;
    return true;
  }

  if( m_pMoveModelPlayer == NULL )
  {
    return false;
  }
  
  *pOut = m_FixPosition + OFFSET_MOVEMODEL_POS_TO_TARGET_POS;

  // カメラチェック用デバッグ機能：強制的にムーランドサーチのオフセットを反映
#if PM_DEBUG
  if( m_DebugForceMuurandFlag )
  {
    gfl2::math::Vector3 nowDir( 0.0f, 0.0f, 1.0f );
    nowDir = m_pMoveModelPlayer->GetLocalSRT().rotation.Transform( nowDir );

    if( m_DebugForceDirFlag )
    {
      pOut->x += m_DebugForceDir.x * OFFSET_MUURAND_SEARCH_DIR;
      pOut->z += m_DebugForceDir.z * OFFSET_MUURAND_SEARCH_DIR;
      pOut->y += OFFSET_MUURAND_SEARCH_HEIGHT;
    }
    else
    {
      pOut->x += nowDir.x * OFFSET_MUURAND_SEARCH_DIR;
      pOut->z += nowDir.z * OFFSET_MUURAND_SEARCH_DIR;
      pOut->y += OFFSET_MUURAND_SEARCH_HEIGHT;
    }
    
  }
  else
#endif
  {
    // ムーランドサーチ中にカメラをずらす処理、値の反映
    if( m_MuurandOffsetValueController.IsValid() )
    {
      pOut->x += m_MuurandOffsetDir.x * OFFSET_MUURAND_SEARCH_DIR * m_MuurandOffsetValueController.GetValue();
      pOut->z += m_MuurandOffsetDir.y * OFFSET_MUURAND_SEARCH_DIR * m_MuurandOffsetValueController.GetValue();
      pOut->y += OFFSET_MUURAND_SEARCH_HEIGHT * m_MuurandOffsetValueController.GetValue();
    }
  }

  // 外部指定遅延を反映
  if( m_DelayValueController.IsValid() )
  {
    *pOut = m_DelayPositon + ( *pOut - m_DelayPositon ) * m_DelayValueController.GetValue();
  }

  return true;
}

/**
* @brief カメラ制御に使用する座標を取得する
* @note  カメラエリアの判定など
*
* @param  pOut  格納先
*
* @return 成否
*/
b32 TargetTypeMoveModelPlayer::GetCameraControlPosition( gfl2::math::Vector3* pOut ) const
{
  // @fix GFNMcat[2553]:遅延追従で挙動がおかしくなる、対処。カメラエリア選定に使用する座標を自機座標からカメラ追従座標に変更
  return this->GetCameraTargetPosition( pOut );
}

/**
* @brief カメラ制御の元になる座標を取得
*
* @param  pOut  格納先
*
* @return 成否
*/
b32 TargetTypeMoveModelPlayer::GetTargetRowPosition( gfl2::math::Vector3* pOut ) const
{
  return this->GetModelPosition( pOut );
}

/**
* @brief 中身を設定
*
* @param  pMoveModel 中身、寿命は管理しない
*
* @return 無し
*/ 
void TargetTypeMoveModelPlayer::SetContent( MoveModel::FieldMoveModelPlayer* pMoveModel )
{
  m_pMoveModelPlayer = pMoveModel;
}

/**
* @brief プレイヤーに遅延追従するかチェック
*
* @param targetPositionDiff 追従座標の差分
*
* @return true : 追従する false : 吸着する
*/ 
b32 TargetTypeMoveModelPlayer::IsPlayerFollow( f32 targetPositionDiff ) const
{
  if( m_pMoveModelPlayer->IsMoveState() && m_pMoveModelPlayer->IsInputStick() && targetPositionDiff >= targetSetting.movedDistance )
  {
    return true;
  }

#if PM_DEBUG
  // カメラチェック機能：未来予知を強制発動させるため、常に動いている扱いとする
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::CAMERA_CHECK_FUTURE_MAX ) )
  {
    return true;
  }
#endif
  return false;
}


/**
* @brief 未来予測の更新速度を更新
*
* @return 無し
*/ 
void TargetTypeMoveModelPlayer::UpdateFutureMoveStepFreme( void )
{
  b32 isRun = false;

  // @note 走りでも遅い場合は走り扱いにしない

  // ライドしていない状態
  if( m_pMoveModelPlayer->GetOnMoveModel() == NULL )
  {
    if( m_pMoveModelPlayer->IsRunLoopState() )
    {
      isRun = true;
    }
  }
  else
  {
    switch( m_pMoveModelPlayer->GetOnMoveModel()->GetCharacterId() ) 
    {
      // ケンタロス
    case CHARA_MODEL_ID_PM0128_00:
      // サメハダー
    case CHARA_MODEL_ID_PM0319_00:
      {
        // 動く=走り扱い
        if( m_pMoveModelPlayer->IsMoveState() )
        {
          isRun = true;
        }

        break;
      }

      // ムーランド
    case CHARA_MODEL_ID_PM0508_00:
      {
        // サーチ以外の動き=走り扱い
        if( m_pMoveModelPlayer->IsMoveState() && m_pMoveModelPlayer->IsMuurandoSearchState() )
        {
          isRun = true;
        }

        break;
      }

      // カイリキー
    case CHARA_MODEL_ID_PM0068_00:
      {
        // 走り扱いなし
        break;
      }

      // 地面馬
    case CHARA_MODEL_ID_PM0840_00:
      // ラプラス
    case CHARA_MODEL_ID_PM0131_00:
      {
        // 走り=走り扱い
        if( m_pMoveModelPlayer->IsRunLoopState() )
        {
          isRun = true;
        }
        break;
      }
    }
  }
 
  // 未来予知に通知
  if( isRun )
  {
    m_pFutureMove->SetStepFrame( targetSetting.futureMove.stepFrameForRun );
  }
  else
  {
    m_pFutureMove->SetStepFrame( 1.0f );
  }

  // 実際のモード切り替えは自機の歩きだけを走り扱いにしない　　自機の歩き→停止時に未来予測により、進行方向とは逆方向に遅延補間が入っているような見た目になってしまうため。その他の移動では歩き終わりで前にでるため違和感がない。
  if( m_pMoveModelPlayer->GetOnMoveModel() == NULL && !m_pMoveModelPlayer->IsRunLoopState() )
  {
    m_pFutureMove->SetRunFlag( false );
  }
  else
  {
    m_pFutureMove->SetRunFlag( true );
  }
}

/**
* @brief ムーランド専用カメラ挙動更新
*
* @return 無し
*/ 
void TargetTypeMoveModelPlayer::UpdateMuurando( void )
{
  // @note サーチカメラ中にムーランドから乗り換えれる事に注意。ムーランド以外=サーチ以外として更新する


  // サーチ中はムーランドの鼻先に注目点をずらす
  if( m_pMoveModelPlayer->IsMuurandoSearchState() )
  {
    gfl2::math::Vector3 checkOffset( 0.0f, 0.0f, 1.0f );
    checkOffset = m_pMoveModelPlayer->GetLocalSRT().rotation.Transform( checkOffset );
    
    // XZ平面のずれを更新
    if( m_MuurandOffsetValueController.GetValue() == 0.0f )
    {
      // 初回は一瞬で現在方向を反映
      m_MuurandOffsetDir.x = checkOffset.x;
      m_MuurandOffsetDir.y = checkOffset.z;
    }
    else
    {
      // 方向を滑らかに更新
      m_MuurandOffsetDir.x += ( checkOffset.x - m_MuurandOffsetDir.x ) * MOVE_DIRECTION_CHANGE_SPEED;
      m_MuurandOffsetDir.y += ( checkOffset.z - m_MuurandOffsetDir.y) * MOVE_DIRECTION_CHANGE_SPEED;

      // 方向が正円に収まるように正規化
      if( m_MuurandOffsetDir.Length() > 1.0f )
      {
        m_MuurandOffsetDir = m_MuurandOffsetDir.Normalize();
      }
    }
    
    // 係数の増加を設定
    if( m_MuurandOffsetValueController.GetEndValue() != 1.0f )
    {
      m_MuurandOffsetValueController.Move( m_MuurandOffsetValueController.GetValue(), 1.0f, 20, gfl2::math::Easing::OUT_QUINT );
    }
  }
  // サーチ以外は戻す
  else
  {
    // 係数の減少を設定
    if( m_MuurandOffsetValueController.GetEndValue() != 0.0f )
    {
      m_MuurandOffsetValueController.Move( m_MuurandOffsetValueController.GetValue(), 0.0f, 20, gfl2::math::Easing::OUT_CUBIC );
    }
  }

  // 係数を更新
  m_MuurandOffsetValueController.Update();

}

/**
* @brief ムーランドか取得
*
* @return true ムーランド false 違う
*/ 
b32 TargetTypeMoveModelPlayer::IsMuurando( void ) const
{
  if( m_pMoveModelPlayer->GetMoveCodeId() == Field::MoveModel::FIELD_MOVE_CODE_RIDE_MUURAND )
  {
    return true;
  }
  else
  {
    return false;
  }
}



GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );