//=================================================================================================
/**
*  @file   FieldMoveModel.cpp
*  @brief  フィールド用 動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include "ViewerProc/include/FieldStatic/FieldNode.h"
#include "ViewerProc/include/FieldStatic/MoveModel/FieldMoveModel.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCode_define.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeManager.h"
#include "ViewerProc/include/FieldStatic/MoveCode/FieldMoveCodeBase.h"
///#include "ViewerProc/include/FieldStatic/ActionCommand/FieldActionCommand_define.h"
///#include "ViewerProc/include/FieldStatic/ActionCommand/FieldActionCommandManager.h"
///#include "ViewerProc/include/FieldStatic/ActionCommand/FieldActionCommandBase.h"
///#include "ViewerProc/include/FieldStatic/ActionCommand/FieldActionCommandStepMove.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseShape.h"
#include "ViewerProc/include/FieldStatic/Collision/BaseCollisionScene.h"
#include "ViewerProc/include/FieldStatic/Collision/RaycastCustomCallback.h"
#include "../../../../../../git_program/poke_3d_lib/model/include/gfl2_CharaModelFactory.h"

// RTTIデバッグシステム
///#include "../../include/RttiDebug/RttiEditor.h"
///#include "../../include/RttiDebug/RttiManager.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>



namespace Field{ namespace MoveModel {

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::FieldMoveModel()
: m_isSetupResource(false)
, m_isSetupWork(false)
, m_stateBit(0)
, m_defaultIdleAnimationId(0)
, m_isForceLanding(true)
, m_groundAttribute(0)
, m_isOneStepFrame(false)
, m_isTurnFrame(false)
, m_oneStepCnt(0.0f)
, m_oneStepMaxCnt(16)
{
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    m_pDynamicActor[i] = NULL;
  }
  for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
  {
    m_pStaticActor[i] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModel::~FieldMoveModel()
{
  ClearWork();
  ClearResource();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(ワーク)
*
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetupWork(const FieldMoveModelHeader* pFieldMoveModelHeader)
{
  // ワークが既に初期化されているかどうか
  if( m_isSetupWork )
  {
    return;
  }

  // リソースが既に作成されていた場合、ワークとのリクエスト内容に差がないかチェック
  if( m_isSetupResource && m_header != *pFieldMoveModelHeader )
  {
    GFL_ASSERT_MSG( false, "ワークとリソースの初期化で利用されたヘッダーの内容に違いがあります" );
  }
  else
  {
    m_header = *pFieldMoveModelHeader;
  }
  m_isSetupWork = true;

  // ムーブコードワークの初期化  
  Field::MoveModel::FieldMoveCodeManager::InitializeMoveCodeWork( &m_moveCodeWork, this );
  m_pMoveCode  = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( m_header.moveCodeId );
  m_pMoveCode->InitProcess( &m_moveCodeWork );
	/**
  // アクションコマンドワークの初期化
  Field::MoveModel::FieldActionCommandManager::InitializeActionCommandWork( &m_actionCommandWork, this );
	*///@saito_del
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(リソース)
*
*  @param  pAllocator             アロケータ
*  @param  pHeap                  ヒープ
*  @param  pCharaModelFactory     キャラリソースファクトリー
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetupResource(gfl2::gfx::IGLAllocator*           pAllocator,
                                   gfl2::heap::HeapBase*              pHeap,
                                   ///poke_3d::model::CharaModelFactory* pCharaModelFactory,
                                   const FieldMoveModelHeader*        pFieldMoveModelHeader,
																	 GLResourceAllocator* pResourceAllocator)
{
  // リソースが既に作成されているかどうか
  if( m_isSetupResource )
  {
    return;
  }

  // ワークが既に作成されていた場合、リソースとのリクエスト内容に差がないかチェック
  if( m_isSetupWork && m_header != *pFieldMoveModelHeader )
  {
    GFL_ASSERT_MSG( false, "ワークとリソースの初期化で利用されたヘッダーの内容に違いがあります" );
  }
  else
  {
    m_header = *pFieldMoveModelHeader;
  }
  m_isSetupResource = true;

  // 着せ替えなし 描画インスタンスを作成
  if( !pFieldMoveModelHeader->pDressUpParam )
  {
    m_pCharaDrawInstance = GFL_NEW(pHeap) poke_3d::model::CharaModel();
    ///pCharaModelFactory->CreateModel( pAllocator, pHeap, m_pCharaDrawInstance , pFieldMoveModelHeader->characterId );
		CreateModel( pAllocator, pHeap, m_pCharaDrawInstance , pResourceAllocator );
  }
  // 着せ替えあり 描画インスタンスを作成
  else
  {
#if 0
    poke_3d::model::DressUpModel* pCharaDrawInstance = GFL_NEW(pHeap) poke_3d::model::DressUpModel();
    m_pCharaDrawInstance = pCharaDrawInstance;
    pCharaModelFactory->CreateDressUpModel( pAllocator, pHeap, pCharaDrawInstance, *(pFieldMoveModelHeader->pDressUpParam), pFieldMoveModelHeader->characterId );
#endif //@saito_del
	}

  // note: アクターのワークは事前に確保せず都度動的に確保するため、断片化のことも考えワークですがリソースと同タイミング(アプリ遷移時)に作成します。

  // コリジョンの作成
  if( m_header.pCollisionSceneForTerrain )
  {
    m_pDynamicActor[COLLISION_TYPE_TERRAIN] = m_header.pCollisionSceneForTerrain->CreateDynamicActor();
  }
  if( m_header.pCollisionSceneForStaticModel )
  {
    m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]        = m_header.pCollisionSceneForStaticModel->CreateDynamicActor();
    m_pStaticActor [COLLISION_STATIC_TYPE_STATIC_MODEL] = m_header.pCollisionSceneForStaticModel->CreateStaticActor();
  }
  if( m_header.pCollisionSceneForEventPos )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_POS] = m_header.pCollisionSceneForEventPos->CreateDynamicActor();
  }
  if( m_header.pCollisionSceneForEventTalk )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_TALK]        = m_header.pCollisionSceneForEventTalk->CreateDynamicActor();
    m_pStaticActor [COLLISION_STATIC_TYPE_EVENT_TALK] = m_header.pCollisionSceneForEventTalk->CreateStaticActor();
  }
  if( m_header.pCollisionSceneForEventPush )
  {
    m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] = m_header.pCollisionSceneForEventPush->CreateDynamicActor();
  }
  if( m_header.pCollisionSceneForEncount )
  {
    m_pDynamicActor[COLLISION_TYPE_ENCOUNT] = m_header.pCollisionSceneForEncount->CreateDynamicActor();
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(ワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearWork(void)
{
  m_isSetupWork = false;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(リソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearResource(void)
{
  // note: アクターのワークは事前に確保せず都度動的に確保するため、断片化のことも考えワークですがリソースと同タイミング(アプリ遷移時)に破棄します。

  // アクターの登録解除
  if( m_header.pCollisionSceneForTerrain && m_pDynamicActor[COLLISION_TYPE_TERRAIN] )
  {
    m_header.pCollisionSceneForTerrain->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_TERRAIN] ); 
  }
  if( m_header.pCollisionSceneForStaticModel && m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] )
  {
    m_header.pCollisionSceneForStaticModel->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] ); 
  }
  if( m_header.pCollisionSceneForEventPos && m_pDynamicActor[COLLISION_TYPE_EVENT_POS] )
  {
    m_header.pCollisionSceneForEventPos->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_POS] ); 
  }
  if( m_header.pCollisionSceneForEventTalk && m_pDynamicActor[COLLISION_TYPE_EVENT_TALK] )
  {
    m_header.pCollisionSceneForEventTalk->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_TALK] ); 
  }
  if( m_header.pCollisionSceneForEventPush && m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] )
  {
    m_header.pCollisionSceneForEventPush->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_EVENT_PUSH] ); 
  }
  if( m_header.pCollisionSceneForEncount && m_pDynamicActor[COLLISION_TYPE_ENCOUNT] )
  {
    m_header.pCollisionSceneForEncount->UnRegistDynamicActor( m_pDynamicActor[COLLISION_TYPE_ENCOUNT] ); 
  }
  if( m_header.pCollisionSceneForStaticModel && m_pStaticActor[COLLISION_STATIC_TYPE_STATIC_MODEL] )
  {
    m_header.pCollisionSceneForStaticModel->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_STATIC_MODEL] ); 
  }
  if( m_header.pCollisionSceneForEventTalk && m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_TALK] )
  {
    m_header.pCollisionSceneForEventTalk->UnRegistStaticActor( m_pStaticActor[COLLISION_STATIC_TYPE_EVENT_TALK] ); 
  }

  // アクターの破棄
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    GFL_SAFE_DELETE( m_pDynamicActor[i] );
  }
  for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
  {
    GFL_SAFE_DELETE(m_pStaticActor[i]);
  }

  // 描画インスタンスの破棄
  GFL_SAFE_DELETE( m_pCharaDrawInstance );

  m_isSetupResource = false;

	if (m_pAnimationPackList != NULL)
	{
		m_pAnimationPackList->UnloadAll();
	  m_pAnimationPackList->Finalize();
	  GFL_DELETE(m_pAnimationPackList);
		m_pAnimationPackList = NULL;
	}

	for( s32 i=0; i< BIN_DATA_NUM;i++)
	{
		if(m_pBinData[i] != NULL){
			m_pResourceAllocator->SystemMemoryFree(m_pBinData[i]);
			m_pBinData[i] = NULL;
		}
	}

	if (m_pModelResourceNode != NULL)
	{
		GFL_SAFE_DELETE(m_pModelResourceNode);
		m_pModelResourceNode = NULL;
	}

	if (m_pMotionConfigResourceNode != NULL)
	{
		GFL_SAFE_DELETE(m_pMotionConfigResourceNode);
	  m_pMotionConfigResourceNode = NULL;
	}
}

//-----------------------------------------------------------------------------
/**
*  @brief  前更新(コリジョン以外の座標/姿勢の変更を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PrevUpdate(void)
{
  //--------------------------------------------------------------------------
  // 動作コード or アクションコマンド
  //--------------------------------------------------------------------------
  // 優先順位は アクションコマンド > イベント > 動作コード
  // アクションコマンドが終わっていたら、動作コードを実行 
  if( !CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_EVENT)
   && !CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD ) )
  {
    m_pMoveCode->MainProcess( &m_moveCodeWork );
  }
#if 0
  // アクションコマンドを実行
  else if( CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD)  )
  {    
    Field::MoveModel::FieldActionCommandManager::UpdateActionCommand( &m_actionCommandWork );
  }

  // アクションコマンド終了時はムーブコードのワークを再初期化
  if( CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD_END) )
  {
    Field::MoveModel::FieldMoveCodeManager::InitializeMoveCodeWork( &m_moveCodeWork, this );
    m_pMoveCode = Field::MoveModel::FieldMoveCodeManager::GetMoveCode( m_header.moveCodeId );
    m_pMoveCode->InitProcess( &m_moveCodeWork );
    OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_ACMD_END );
  }
#endif //@saito_del
  //--------------------------------------------------------------------------
  // アニメーション
  //--------------------------------------------------------------------------

  // note: 動作コード・アクションコマンドにてモーションが変えられるケースがあるので、
  //       アニメーションの更新は動作コード・アクションコマンドの後に行う必要あり

  // アニメーション更新
  f32 prevOriginPosY = m_pCharaDrawInstance->GetWalkSpeed( -1.0f ).y;
  m_pCharaDrawInstance->UpdateAnimation();

  // アニメーションが切り替わっているかどうか
  s32 animeId = m_pCharaDrawInstance->GetAnimationId();
  if( m_animationId != animeId )
  {
    prevOriginPosY = 0.0f;
  }
  m_animationId = animeId;

  // ジョイント"Origin"の移動要素を取得(ジョイント"Origin"はスキニング対象外です)
  gfl2::math::Vector3 originMoveFactor( m_pCharaDrawInstance->GetWalkSpeed( -1.0f ) );
  gfl2::math::Vector4  originMoveVector( 0.0f,0.0f,0.0f,0.0f );
  if( !CheckStateBit(Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE) )
  {
    // ジョイント"Origin"による移動ベクトルを取得
    originMoveVector = gfl2::math::Vector4( 0.0f,                                  // X軸データは入っていない
                                           originMoveFactor.y - prevOriginPosY, // Y軸データは絶対値で入っているので1フレーム前の値を引いて、1フレーム前の姿勢からのオフセット値にする
                                           originMoveFactor.z,                    // Z軸データは1フレーム前の姿勢からのオフセットで入っている
                                           0.0f );
  }

  //--------------------------------------------------------------------------
  // ジョイント"Origin"による動作モデルの移動
  //--------------------------------------------------------------------------

  // note: ジョイント"Origin"による動作モデルの移動はアニメーション更新後でないと1フレーム遅れてしまう

  // ジョイント"Origin"による移動ベクトルを適応する
  if( originMoveVector.Length() > 0.0f )
  {
    // 1フレームの移動ベクトルを自身のローカルにする
    gfl2::math::Matrix mat;
    if( m_isUseCustomValueForOrigin )
    {
      m_customQuaternionForOrigin.QuaternionToMatrix(mat);
      originMoveVector *= m_customScaleForOrigin;
    }
    else
    {
      m_pCharaDrawInstance->GetRotationQuat().QuaternionToMatrix(mat);
    }

    originMoveVector = mat.TransformNormal( originMoveVector );
    m_pCharaDrawInstance->SetPosition( m_pCharaDrawInstance->GetPosition() + gfl2::math::Vector3( originMoveVector.x,originMoveVector.y,originMoveVector.z ) );
  }

  // ジョイント"Origin"による移動情報を保存と初期化
  if( originMoveVector.Length3() > 0.0f )
  {
    m_lastEnableOriginMoveVector = originMoveVector;
  }
  m_isUseCustomValueForOrigin = false;

  //--------------------------------------------------------------------------
  // 地面チェック
  //--------------------------------------------------------------------------
  m_isOnGround      = false;
  m_groundAttribute = 0;
  m_groundHeight    = 0.0f;

  if( m_pCollisionSceneContainer )
  {
    // 地面の高さを取得
		RaycastCustomCallback::HIT_DATA hitData;
    gfl2::math::Vector3   startVec3(m_pCharaDrawInstance->GetPosition() + gfl2::math::Vector3(0.0f,10000.0f,0.0f));
    gfl2::math::Vector3   endVec3  (m_pCharaDrawInstance->GetPosition() - gfl2::math::Vector3(0.0f,10000.0f,0.0f));
    gfl2::math::Vector4    startVec (startVec3.x,startVec3.y,startVec3.z,0.0f);
    gfl2::math::Vector4    endVec   (  endVec3.x,  endVec3.y,  endVec3.z,0.0f);
    
    // レイが地面に当たったかどうか
    if( m_pCollisionSceneContainer->RaycastFromStaticActorsMesh( startVec,endVec,&hitData ) )
    {
      // 取得した一番近い接点の高さを自機の高さとする
      if( m_isForceLanding )
      {
        gfl2::math::Vector3 newPos = m_pCharaDrawInstance->GetPosition();
        newPos.y = hitData.intersection.y;
        m_pCharaDrawInstance->SetPosition(newPos);
      }

      // 地面情報の確保
      m_isOnGround      = true;
      m_groundAttribute = hitData.pTriangle->m_GroundAttribute;
      m_groundHeight    = hitData.intersection.y;
    }
  }

  //--------------------------------------------------------------------------
  // その他
  //--------------------------------------------------------------------------

  // 1歩分進んだか調査
  {
    m_oneStepCnt += originMoveVector.Length3();
    if( (u32)m_oneStepCnt / m_oneStepMaxCnt > 0 )
    {
      m_isOneStepFrame = true;
      m_oneStepCnt     = 0.0f;
    }
    else
    {
      m_isOneStepFrame = false;
    }
  }

  // ターンしたか調査
  {
    gfl2::math::Vector3 temp0(             originMoveVector.x,            originMoveVector.y,            originMoveVector.z ); temp0 = temp0.Normalize();
    gfl2::math::Vector3 temp1( m_lastEnableOriginMoveVector.x,m_lastEnableOriginMoveVector.y,m_lastEnableOriginMoveVector.z ); temp1 = temp1.Normalize();
    if( temp0.Dot( temp1 ) < 0.0f )
    {
      m_isTurnFrame = true;
    }
    else
    {
      m_isTurnFrame = false;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  更新(コリジョンシーンへの座標/姿勢の登録を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::Update(void)
{
  // コリジョンシステムに伝える
  gfl2::math::Vector3    pos  ( m_pCharaDrawInstance->GetPosition()     );
  gfl2::math::Quaternion quat ( m_pCharaDrawInstance->GetRotationQuat() );
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    if( m_pDynamicActor[i] )
    {
      m_pDynamicActor[i]->SetPosition  ( pos  );
      m_pDynamicActor[i]->SetQuaternion( quat );
    }
  }

  for( s32 i=0 ; i<COLLISION_STATIC_TYPE_MAX ; i++ )
  {
    if( m_pStaticActor[i] )
    {
      m_pStaticActor[i]->SetPosition  ( pos  );
      //m_pStaticActor[i]->SetQuaternion( quat ); // @todo StaticActorも回転していいのか？
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(コリジョンシーンからの結果を適用し、最終的な座標/姿勢の決定/イベントの実行を行う)
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::PostUpdate(void)
{
  // コリジョンシステムから結果を受け取る
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    // 当たった相手がいるかどうか
    if( !m_pDynamicActor[i] || !m_pDynamicActor[i]->GetCollisionResult(0)->pTargetActor )
    {
      continue;
    }

    // イベント用は押し返しを受けない
    if( i < COLLISION_TYPE_EVENT_POS )
    {
      if( !CheckStateBit( FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE ) )
      {
        gfl2::math::Vector3    pos ( m_pDynamicActor[i]->GetPosition()   );
        gfl2::math::Quaternion quat( m_pDynamicActor[i]->GetQuaternion() );
        m_pCharaDrawInstance->SetPosition    ( pos );
        m_pCharaDrawInstance->SetRotationQuat( quat );
      }
    }
    // イベント用の場合は押し返しをせずイベントを実行
    else
    {
      EventDataForCollision* pEventData = static_cast<EventDataForCollision*>(m_pDynamicActor[i]->GetCollisionResult(0)->pTargetActor->GetUserData());
      if( pEventData && pEventData->isEnable() )
      {
        GFL_PRINT("イベントヒット \n" );
      }
    }
  }

  // 別アプリから復帰用の値を更新
  m_positionForReturn   = m_pCharaDrawInstance->GetPosition();
  m_quaternionForReturn = m_pCharaDrawInstance->GetRotationQuat();
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビット  ON
*
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::OnStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_stateBit |= fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットOFF
*  
*  @param  fieldMoveModelStateBit  ビット
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::OffStateBit(FieldMoveModelStateBit fieldMoveModelStateBit)
{
  m_stateBit &= ~fieldMoveModelStateBit;
}

//----------------------------------------------------------------------------
/**
*  @brief  動作ビットのチェック
*
*  @param  fieldMoveModelStateBit  チェックするビット
*  @return trueはbitOn、falseはbitOff
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::CheckStateBit(FieldMoveModelStateBit fieldMoveModelStateBit) const
{
  if( (m_stateBit & fieldMoveModelStateBit) == fieldMoveModelStateBit )
  {
    return true;
  }
  return false;
}
#if 0
//-----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの設定
*
*  @param  actionCommand  アクションコマンド
*  @param  dirY           回転Y
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetActionCommand(FieldActionCommandId actionCommand,f32 dirY)
{
  FieldActionCommandManager::SetActionCommand( &m_actionCommandWork, actionCommand, dirY );
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドの強制停止
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ClearActionCommand(void)
{
  FieldActionCommandManager::ClearActionCommandWork( &m_actionCommandWork );
}
#endif //@saito_del
//-----------------------------------------------------------------------------
/**
*  @brief  ワークからリソースの状態を元に戻す
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ReturnResourceStateFromWork(void)
{
  m_pCharaDrawInstance->SetPosition    ( m_positionForReturn );
  m_pCharaDrawInstance->SetRotationQuat( m_quaternionForReturn );
  // Tスタンスにならないように待機モーションを流す
  m_pCharaDrawInstance->ChangeAnimation( m_defaultIdleAnimationId );

  m_oneStepCnt     = 0.0f;
  m_isOneStepFrame = false;
  m_isTurnFrame    = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIマネージャーに自身のプロパティを登録
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::RegistRtti(void)
{
#if USE_RTTI_DEBUG

  RTTI_START_REGIST( 256 );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isSetupWork            , L"初期化済み(ワーク)"            , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isSetupResource        , L"初期化済み(リソース)"          , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isOnGround             , L"地面に立っているか"            , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_groundAttribute        , L"地面属性"                      , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_groundHeight           , L"地面の高さ"                    , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_isForceLanding         , L"地面に吸着させるかどうか"      );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_oneStepCnt             , L"現在のステップカウント"        , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST     ( FieldMoveModel , m_oneStepMaxCnt          , L"一歩とするステップカウント"    );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isTurnFrame            , L"振り返ったタイミングかどうか"  , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
    RTTI_PARAM_REGIST_ATTR( FieldMoveModel , m_isOneStepFrame         , L"一歩進んだタイミングかどうか"  , gfl2::rttidebug::RttiParamManager::ATTR_READONLY );
  RTTI_END_REGIST( FieldMoveModel );

#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタに自身のインスタンスを登録
*
*  @param  エディタに表示したい名前
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::CreateRttiEditorPage(c16* pDispName)
{
#if USE_RTTI_DEBUG

  // RTTIエディタに自身のプロパティを登録
  RegistRtti();
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    if( m_pDynamicActor[i] )
    {
      m_pDynamicActor[i]->GetShape()->RegistRtti();
    }
  }

  // RTTIエディタに自身のインスタンス内容を表示する
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( FieldMoveModel ) , this );
  }
  if( m_pDynamicActor[COLLISION_TYPE_TERRAIN] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:地面用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShape() )      , m_pDynamicActor[COLLISION_TYPE_TERRAIN]->GetShape() );
  }
  if( m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:配置モデル用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]->GetShape() ) , m_pDynamicActor[COLLISION_TYPE_STATIC_MODEL]->GetShape() );
  }
  if( m_pDynamicActor[COLLISION_TYPE_EVENT_POS] )
  {
    c16 dispName[gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE];
    gfl2::rttidebug::wideCharaCopy( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , pDispName );
    gfl2::rttidebug::wideCharaCat ( dispName , gfl2::rttidebug::RttiEditor::EditorRequest::DISP_NAME_SIZE , L"[衝突形状:POSイベント用]" );
    gfl2::rttidebug::RttiEditor::getInstance()->CreateEditorFromInstace( dispName , &typeid( *m_pDynamicActor[COLLISION_TYPE_EVENT_POS]->GetShape() )        , m_pDynamicActor[COLLISION_TYPE_EVENT_POS]->GetShape() );
  }
#endif
}

//-----------------------------------------------------------------------------
/**
*  @brief  RTTIエディタから自身のインスタンスを削除
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::DeleteRttiEditorPage(void)
{
#if USE_RTTI_DEBUG

  gfl2::rttidebug::RttiEditor::getInstance()->DeleteEditorFromInstace( this );
  for( s32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    if( m_pDynamicActor[i] && m_pDynamicActor[i]->GetShape() )
    {
      gfl2::rttidebug::RttiEditor::getInstance()->DeleteEditorFromInstace( m_pDynamicActor[i]->GetShape() );
    }
  }

#endif
}

//================================================================================================================
// 親のインターフェースの実装部
//================================================================================================================

//-----------------------------------------------------------------------------
/**
*  @brief 表示ON・OFFを設定
*
*  @param isVisible  表示ON・OFF
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetVisible(bool isVisible)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetVisible(isVisible);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  表示ON・OFFを取得
*
*  @return 表示ON・OFF
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::GetVisible(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->IsVisible();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief SRTを設定
*
*  @param srt 設定したいSRT
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::SetLocalSRT(const gfl2::math::SRT & srt)
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    m_pCharaDrawInstance->SetScale( srt.scale );
    m_pCharaDrawInstance->SetRotationQuat( srt.rotation );
    m_pCharaDrawInstance->SetPosition( srt.translate );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  SRTを取得
*
*  @return SRT
*/
//-----------------------------------------------------------------------------
const gfl2::math::SRT& FieldMoveModel::GetLocalSRT(void) const
{
  GFL_ASSERT_MSG( m_pCharaDrawInstance                          , "モデルがセットアップされていない状態でLocalSrtを取得することはできません" );
  GFL_ASSERT_MSG( m_pCharaDrawInstance->GetModelInstanceNode() , "モデルがセットアップされていない状態でLocalSrtを取得することはできません" );
  return m_pCharaDrawInstance->GetModelInstanceNode()->GetLocalSRT();
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンス数を取得
*
*  @return モデルインスタンス数
*/
//-----------------------------------------------------------------------------
u32 FieldMoveModel::GetModelInstanceNum(void) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスを取得
*
*  @return モデルインスタンス
*/
//-----------------------------------------------------------------------------
const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * FieldMoveModel::GetModelInstance(int index) const
{
  if( m_pCharaDrawInstance && m_pCharaDrawInstance->GetModelInstanceNode() )
  {
    return m_pCharaDrawInstance->GetModelInstanceNode();
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief モデルインスタンスの遅延解放は自由に実装
*
*  @note:
*   削除リクエストを受けたモデルインスタンスを解放
*   -実行タイミング  フィールドプロセスの先頭で実行します。
*   -制限            PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
*/
//-----------------------------------------------------------------------------
void FieldMoveModel::ExecuteTerminateRequest(void)
{
}

//-----------------------------------------------------------------------------
/**
*  @brief  安全に破棄できる常態かチェック
*
*  @return 安全に解放できるかどうか
*/
//-----------------------------------------------------------------------------
bool FieldMoveModel::IsCanTerminate(void)
{
  return true;
}


bool FieldMoveModel::CreateModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, poke_3d::model::CharaModel* pCharaModel, GLResourceAllocator *pResourceAllocator)
{
	m_pResourceAllocator = pResourceAllocator;
	// パックされたデータからリソースノードを作成
	gfl2::renderingengine::scenegraph::resource::ResourceNode *pModelResourceNode = NULL;
	{
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
	  c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Player/black_doll.wingl.gfbmdlp", pResourceAllocator );
	  resourceData.SetModelData( pBinData );
		m_pBinData[0] = pBinData;
	  pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
		m_pModelResourceNode = pModelResourceNode;
	}
  // モデルリソースノード取得
  ///gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceNode = pResourceUnit->GetModelResourceNode();
  if (pModelResourceNode == NULL)
  {
    // 読み込み終わってないので生成できない
    return false;
  }

	gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResourceNode = NULL;
	{
		c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001old.bmcnf", pResourceAllocator);
    // モーションコンフィグ設定
    gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData motionConfigResourceData;
    motionConfigResourceData.SetMcnfData( pBinData );
		m_pBinData[1] = pBinData;
    pMotionConfigResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);
		m_pMotionConfigResourceNode = pMotionConfigResourceNode;
	}
  // モーションコンフィグ設定
  ///gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResourceNode = pResourceUnit->GetMotionConfigResourceNode();
  if (pMotionConfigResourceNode == NULL)
  {
    // モーションコンフィグが無い？
    return false;
  }

	m_pAnimationPackList = GFL_NEW(pAnimationHeap)(gfl2::animation::AnimationPackList);
	{
		c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Player/black_doll.gfbmotp", pResourceAllocator);
		m_pAnimationPackList->Initialize(pAnimationHeap, 1);
		m_pAnimationPackList->LoadData(0,pGfxAllocator,pAnimationHeap,pBinData);
		m_pBinData[2] = pBinData;
	}
  // アニメーションパックリスト設定
  ///gfl2::animation::AnimationPackList* pAnimationPackList = pResourceUnit->GetAnimationPackList();
  if (m_pAnimationPackList == NULL)
  {
    // アニメーションパックリストが無い？
    return false;
  }
	
  // キャラモデル生成
  if (!pCharaModel->Create(pGfxAllocator, pAnimationHeap, pModelResourceNode, pMotionConfigResourceNode))
  {
    // キャラモデル生成失敗
    return false;
  }

  // アニメーションパック設定
  pCharaModel->SetAnimationPackList(m_pAnimationPackList);

  // 作成成功
  return true;
}

}; //end of namespace MoveModel
}; //end of namespace Field
