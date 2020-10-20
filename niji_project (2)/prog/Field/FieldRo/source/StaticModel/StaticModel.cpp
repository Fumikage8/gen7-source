//===================================================================
/**
 * @file    StaticModel.cpp
 * @brief   配置モデルクラス
 * @author  saita_kazuki
 * @date    2015.04.08
 */
//===================================================================

// StaticModel
#include "Field/FieldRo/include/StaticModel/StaticModel.h"
#include "Field/FieldRo/include/StaticModel/StaticModelManager.h"
#include "Field/FieldRo/include/StaticModel/StaticModelResourceNodeManager.h"
#include "./StaticModelSEPlayer.h"

// gflib2
#include <debug/include/gfl2_DebugPrint.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <system/include/Date/gfl2_Date.h>  // 時間を使用するアニメーション制御に使用
#include <util/include/gfl2_std_string.h>
#include <math/include/gfl2_Float.h>

// Field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/Collision/StaticActor.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h" // バウンディングボックスアニメーションで使用

// System
#include "System/include/nijiAllocator.h"

// GameSys
#include "GameSys/include/TimeZone.h"
#include "GameSys/include/NijiTime.h"
#include <niji_conv_header/field/static_model/static_model_id.h>


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )


// 時間アニメーション更新関数テーブル
const StaticModel::FrameControlAniamtionUpdateFunc StaticModel::m_TimeAnimationUpdateFunc[ TIME_ANIMATION_UPDATE_SEQ_MAX ] =
{
  &StaticModel::TimeAnimationUpdateFunc_None,
  &StaticModel::TimeAnimationUpdateFunc_WaitStart,
  &StaticModel::TimeAnimationUpdateFunc_WaitLoop,
  &StaticModel::TimeAnimationUpdateFunc_WaitEnd,
};

// バウンディングボックスアニメーション更新関数テーブル
const StaticModel::FrameControlAniamtionUpdateFunc StaticModel::m_BBAnimationUpdateFunc[ BB_ANIMATION_UPDATE_SEQ_MAX ] =
{
  &StaticModel::BBAnimationUpdateFunc_None,
  &StaticModel::BBAnimationUpdateFunc_WaitStart,
  &StaticModel::BBAnimationUpdateFunc_WaitLoop,
  &StaticModel::BBAnimationUpdateFunc_WaitEnd,
};


/**
 * @brief   コンストラクタ
 */
StaticModel::StaticModel() :
  m_pHeap( NULL),
  m_pAllocator( NULL),
  m_pStaticModelPack( NULL),
  m_pStaticModelInfo( NULL),
  m_modelID( 0),
  m_modelNum( 0),
  m_eventID(0),
  m_nowModelType( MODEL_TYPE_NONE),
  m_srt(),
  m_modelIndex( -1),
  m_isExecuteTerminate( false),
  m_dirayTerminateList(),
  m_pStaticActor( NULL),
  m_pStaticActorBBAnimation( NULL),
  m_pCollisionSceneForBBAnimation( NULL),
  m_pFieldRootNode( NULL),
  m_state( STATE_NONE),
  m_stopUpdateFrame( -1 ),
  m_pSEPlayer( NULL),
  m_allTimeAnimationWork(),
  m_timeAnimationWork(),
  m_animationSlotControlWork(),
  m_pManager( NULL),
  m_pResourceNodeManager( NULL),
  m_resourceWorkIndex( StaticModelResourceNodeManager::WORK_INDEX_ERROR),
  m_isDynamic( false),
  m_isUpdateCullingEnable( true)

#if PM_DEBUG
  , m_instanceAllocSize( 0)
  , m_animationAllocSize( 0)
#endif
{
  gfl2::std::MemClear( m_pModelResourceRootNode, sizeof( m_pModelResourceRootNode));
  gfl2::std::MemClear( m_pMotionResourceRootNode, sizeof( m_pMotionResourceRootNode));
  gfl2::std::MemClear( m_pModel, sizeof( m_pModel));
  m_isModelVisible = false;
}

/**
 * @brief   デストラクタ
 */
StaticModel::~StaticModel()
{
}

/**
 * @brief 現在のLODレベルのモデルの表示ON・OFF
 */
void StaticModel::SetVisible( bool isVisible )
{
  if( m_nowModelType != MODEL_TYPE_NONE && m_pModel[m_nowModelType]->IsCreated())
  {
    m_isModelVisible = isVisible;
    return m_pModel[m_nowModelType]->SetVisible( isVisible);
  }
}
bool StaticModel::GetVisible( void ) const
{
  if( m_nowModelType != MODEL_TYPE_NONE && m_pModel[m_nowModelType]->IsCreated())
  {
    return GFL_BOOL_CAST( m_pModel[m_nowModelType]->IsVisible());
  }
  return false;
}

/**
 * @brief 全モデル表示ON・OFF
 */
void StaticModel::SetVisibleAll( bool isVisible)
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    if( m_pModel[i]->IsCreated())
    {
      m_isModelVisible = isVisible;
      return m_pModel[i]->SetVisible( isVisible);
    }
  }
}

/**
 * @brief SRTコントロール
 */
void StaticModel::SetLocalSRT( const gfl2::math::SRT & srt )
{
  // どのモデルも位置は同じはずなので、全てのモデルインスタンスにセットする
  for( s32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    if( m_pModel[i]->IsCreated())
    {
      m_pModel[i]->SetPosition( srt.translate);
      m_pModel[i]->SetRotationQuat( srt.rotation);
      m_pModel[i]->SetScale( srt.scale);
    }
  }
  m_srt = srt;
}
const gfl2::math::SRT & StaticModel::GetLocalSRT() const
{
  return m_srt;
}

/**
 * @brief モデルインスタンスを取得
 */
u32 StaticModel::GetModelInstanceNum() const
{
  return m_modelNum;
}
const gfl2::renderingengine::scenegraph::instance::ModelInstanceNode * StaticModel::GetModelInstance( int index ) const
{
  if( index >= MODEL_TYPE_MAX)
  {
    return NULL;
  }

  if( m_pModel[index]->IsCreated())
  {
    return m_pModel[index]->GetModelInstanceNode();
  }
  return NULL;
}

/**
 *  @brief  ベースモデルの取得
 *  @note 取得したBaseModelのポインタは必ずNULLチェックをしてください
 *  @note デバッグ期間にGetBaseModelを使用している箇所でNULLチェックが行われているかを確認する
 */
u32 StaticModel::GetBaseModelNum() const
{
  return m_modelNum;
}
poke_3d::model::BaseModel *StaticModel::GetBaseModel( u32 idx ) const
{
  if( idx >= MODEL_TYPE_MAX)
  {
    return NULL;
  }

  if( m_pModel[idx]->IsCreated())
  {
    return m_pModel[idx];
  }
  return NULL;
}

//! モデルインスタンスの遅延解放は自由に実装
/**! 削除リクエストを受けたモデルインスタンスを解放
 * -実行タイミング　フィールドプロセスの先頭で実行します。
 * -制限　　　　　　PipeLineの描画後にモデルインスタンスの遅延解放リクエストを出さないこと。
 */
void StaticModel::ExecuteTerminateRequest(void)
{
  if( m_isExecuteTerminate && this->IsCanTerminate())
  {
    gfl2::util::List<ModelType>::Iterator iter = m_dirayTerminateList.Begin();
    for( ; iter != m_dirayTerminateList.End(); ++iter)
    {
      // モデルの終了処理
      if( m_pModel[*iter])
      {
        if( m_pModel[*iter]->IsCreated())
        {
          m_pModel[*iter]->Destroy();
        }
      }
      m_modelNum--;
    }

    m_dirayTerminateList.Erase();
    m_isExecuteTerminate = false;
  }
}

//! 安全に破棄できる常態かチェックする。
bool StaticModel::IsCanTerminate(void)
{
  gfl2::util::List<ModelType>::Iterator iter = m_dirayTerminateList.Begin();
  for( ; iter != m_dirayTerminateList.End(); ++iter)
  {
    if( m_pModel[*iter]->IsCreated())
    {
      if( m_pModel[*iter]->GetModelInstanceNode()->GetReferenceCnt() != 0)
      {
        // どれかひとつでも参照されていたら安全に破棄できない
        return false;
      }
    }
  }
  // すべて参照されていなかったら安全に破棄できる
  return true;
}

/**
 * @brief 遅延解放リクエスト
 * @param modelType どのモデルか。ハイ、ミドル、ロー、全てから選択
 */
void StaticModel::RequestDelayTerminate( ModelType modelType)
{
  if ( m_isExecuteTerminate ) return;

  if( modelType == MODEL_TYPE_ALL)
  {
    for( s32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      if( m_pModel[i] && m_pModel[i]->IsCreated() != NULL)
      {
        m_pModel[i]->SetVisible( false);
        m_dirayTerminateList.PushBack( static_cast<ModelType>( i));
        m_isExecuteTerminate = true;
      }
    }
  }
  else if( modelType < MODEL_TYPE_MAX)
  {
    if( m_pModel[modelType] &&  m_pModel[modelType] != NULL)
    {
      m_pModel[modelType]->SetVisible( false);
      m_dirayTerminateList.PushBack( modelType);
      m_isExecuteTerminate = true;
    }
  }
}

/**
 * @brief 初期化
 * @param initData 初期化情報
 */
void StaticModel::Initialize( InitializeData& initData)
{
  m_pHeap               = initData.pHeap;
  m_modelID             = initData.modelID;
  m_eventID             = initData.eventID;
  m_pStaticModelPack    = initData.pStaticModelPack;
  m_srt                 = initData.srt;
  m_pFieldRootNode      = initData.pFieldRootNode;
  m_pAllocator          = initData.pAllocator;
  m_pSEPlayer           = initData.pSEPlayer;
  m_modelIndex          = initData.modelIndex;
  m_pManager            = initData.pManager;
  m_pResourceNodeManager= initData.pResourceNodeManager;
  m_isDynamic           = initData.isDynamic;

  m_stopUpdateFrame = -1;

#if PM_DEBUG
  u32 oldAllocatableSize = m_pHeap->GetTotalAllocatableSize();
#endif

  // 遅延解放リクエスト用バッファ初期化
  m_dirayTerminateList.CreateBuffer( m_pHeap, MODEL_TYPE_MAX);


  // 配置モデル情報を取得
  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPack);
  m_pStaticModelInfo = reinterpret_cast<StaticModelPackAccessor::StaticModelInfo*>( accessor.GetStaticModelInfo());


  // モデルを作成
  if( m_isDynamic)
  {
    this->CreateModelWorkDynamic();
  }
  else
  {
    this->CreateModelWork();
  }


#if PM_DEBUG
  u32 oldAnimationAllocatableSize = m_pHeap->GetTotalAllocatableSize();
#endif

  // アニメーションメモリを初期化時にFIXする
  for( u32 cntModelType = 0; cntModelType < MODEL_TYPE_MAX; ++cntModelType)
  {
    if( m_pModel[cntModelType] && m_pModel[cntModelType]->IsCreated())
    {
      for( u32 cntMotionIndex = 0; cntMotionIndex < ANIMATION_INDEX_MAX; ++cntMotionIndex)
      {
        if( this->IsAnimationExist( static_cast<ModelType>( cntModelType), static_cast<AnimationIndex>( cntMotionIndex)))
        {
          // アニメーションメモリ確保
          m_animationSlotControlWork.SetSlotIndex( cntModelType, cntMotionIndex);
          u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( cntModelType, cntMotionIndex);
          if( this->CanUseChangeAnimeSmooth() )
          {
            // ポケセンカウンター/ジガルデ装置のループアニメーションのみ補間分も確保する
            m_pModel[cntModelType]->AllocateAnimationByResourceNode( m_pHeap, true, m_pMotionResourceRootNode[cntModelType][cntMotionIndex], slotIndex);
          }
          else
          {
            m_pModel[cntModelType]->AllocateAnimationByResourceNode( m_pHeap, false, m_pMotionResourceRootNode[cntModelType][cntMotionIndex], slotIndex);
          }

          // 戻しておく
          m_pModel[cntModelType]->ChangeAnimationByResourceNode( NULL, slotIndex);
          m_animationSlotControlWork.ClearSlotIndex( cntModelType, cntMotionIndex);
        }
      }
    }
  }

#if PM_DEBUG
  u32 afterAnimationAllocatableSize = m_pHeap->GetTotalAllocatableSize();
  m_animationAllocSize = oldAnimationAllocatableSize - afterAnimationAllocatableSize;
  SAITA_PRINT( "StaticModel : WorkIndex = %d, EventID = %d, ModelID = %d, AnimationAllocSize(byte) = %x ( %d )\n", m_modelIndex, m_eventID, m_modelID, m_animationAllocSize, m_animationAllocSize);
#endif



  // アニメーションスロット制御用ワーク初期化
  m_animationSlotControlWork.Initialize();


  // ループアニメーション再生
  if( this->IsAnimationExist( ANIMATION_INDEX_LOOP_0))
  {
    this->ChangeAnimation( ANIMATION_INDEX_LOOP_0);
  }

  // 1日アニメーションワーク初期化
  m_allTimeAnimationWork.Initialize();
  // 1日アニメーション再生
  if( this->IsAnimationExist( ANIMATION_INDEX_ALL_TIME_0))
  {
    this->ChangeAnimation( ANIMATION_INDEX_ALL_TIME_0, true, 0.0f);
  }


  // 時間アニメーションの開始時間、分を決定。時間指定ならそのまま、タイムゾーン指定ならそのタイムゾーンの時間を保持しておく
  s32 startHour,startMinute;
  if( m_pStaticModelInfo->startTimeZone != TIME_ZONE_NONE)
  {
    startHour = GameSys::TimeZone::ConvertTimeZoneToStartHour( static_cast<GameSys::TimeZone::Zone>( m_pStaticModelInfo->startTimeZone));
    startMinute = 0;
  }
  else
  {
    startHour = m_pStaticModelInfo->timeAnimeStartHour;
    startMinute = m_pStaticModelInfo->timeAnimeStartMinute;
  }

  // 時間アニメーションの終了時間、分を決定。時間指定ならそのまま、タイムゾーン指定ならそのタイムゾーンの時間を保持しておく
  s32 endHour,endMinute;
  if( m_pStaticModelInfo->endTimeZone != TIME_ZONE_NONE)
  {
    endHour = GameSys::TimeZone::ConvertTimeZoneToStartHour( static_cast<GameSys::TimeZone::Zone>( m_pStaticModelInfo->endTimeZone));
    endMinute = 0;
  }
  else
  {
    endHour = m_pStaticModelInfo->timeAnimeEndHour;
    endMinute = m_pStaticModelInfo->timeAnimeEndMinute;
  }

  // 時間アニメーションワークの初期化
  m_timeAnimationWork.Initialize( startHour, startMinute, endHour,endMinute);

  // バウンディングボックスアニメーションワーク初期化
  m_BBAnimationWork.Initialize();

  m_state = STATE_INITIALIZE_FINISHED;

  if( m_nowModelType != MODEL_TYPE_NONE)
  {
    m_isModelVisible = GFL_BOOL_CAST( m_pModel[m_nowModelType]->IsVisible());
  }

#if PM_DEBUG
  u32 afterAllocatableSize = m_pHeap->GetTotalAllocatableSize();
  m_instanceAllocSize = oldAllocatableSize - afterAllocatableSize;
  SAITA_PRINT( "StaticModel : WorkIndex = %d, EventID = %d, ModelID = %d, InstanceAllocSize(byte)  = %x ( %d )\n", m_modelIndex, m_eventID, m_modelID, m_instanceAllocSize, m_instanceAllocSize);
#endif

  m_isUpdateCullingEnable = true;
}

/**
* @brief Readyで1度だけ呼び出す
*/
void StaticModel::Ready()
{
  // 更新状態でなかったら何もしない
  if( m_state != STATE_UPDATE)
  {
    return;
  }

  // 時間帯アニメーション更新
  // このタイミングで始まる場合は、開始アニメをスキップさせループアニメから始める
  if( this->IsAnimationExist( ANIMATION_INDEX_TIME_0))
  {
    bool isStart = this->TimeAnimationUpdateFunc_None();
    if( isStart)
    {
      if( m_nowModelType != MODEL_TYPE_NONE)
      {
        u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);
        m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeLoop_StartFrame, slotIndex);
        m_timeAnimationWork.SetUpdateSeq( TIME_ANIMATION_UPDATE_SEQ_WAIT_LOOP);
      }
    }
  }

  // バウンディングボックスアニメーション更新
  // このタイミングで始まる場合は、開始アニメをスキップさせループアニメから始める
  if( this->IsAnimationExist( ANIMATION_INDEX_BB_0))
  {
    bool isStart = this->BBAnimationUpdateFunc_None();
    if( isStart)
    {
      if( m_nowModelType != MODEL_TYPE_NONE)
      {
        u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);
        m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.loop_StartFrame, slotIndex);
        m_BBAnimationWork.SetUpdateSeq( BB_ANIMATION_UPDATE_SEQ_WAIT_LOOP);
      }
    }
  }
}


/**
 * @brief 終了
 */
void StaticModel::Terminate()
{
  GFL_ASSERT( m_isExecuteTerminate == false);

  if( m_pStaticActorBBAnimation)
  {
    m_pCollisionSceneForBBAnimation->UnRegistStaticActor( m_pStaticActorBBAnimation);
    GFL_DELETE( m_pStaticActorBBAnimation);
    m_pStaticActorBBAnimation = NULL;
    m_pCollisionSceneForBBAnimation = NULL;
  }

  // リソースノードを解放する、モデル解放 動的用
  if( m_isDynamic)
  {
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      if( m_pModelResourceRootNode[i])
      {
        m_pFieldRootNode->RemoveResourceChild( m_pModelResourceRootNode[i]);
        GFL_DELETE( m_pModelResourceRootNode[i]);
        m_pModelResourceRootNode[i] = NULL;
      }

      for( s32 motionCount = 0; motionCount < ANIMATION_INDEX_MAX; ++motionCount)
      {
        if( m_pMotionResourceRootNode[i][motionCount])
        {
          m_pFieldRootNode->RemoveResourceChild( m_pMotionResourceRootNode[i][motionCount]);
          GFL_DELETE( m_pMotionResourceRootNode[i][motionCount]);
          m_pMotionResourceRootNode[i][motionCount] = NULL;
        }
      }

      GFL_SAFE_DELETE( m_pModel[i]);
    }
  }
  // リソースノードをNULLにする、モデル解放
  else
  {
    for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
    {
      m_pModelResourceRootNode[i] = NULL;
      for( s32 motionCount = 0; motionCount < ANIMATION_INDEX_MAX; ++motionCount)
      {
        m_pMotionResourceRootNode[i][motionCount] = NULL;
      }

      GFL_SAFE_DELETE( m_pModel[i]);
    }

    // リソースノード破棄
    if( m_pResourceNodeManager)
    {
      m_pResourceNodeManager->Delete( m_modelID);
    }
  }

  m_allTimeAnimationWork.Clear();
  m_timeAnimationWork.Clear();
  m_BBAnimationWork.Clear();
  m_animationSlotControlWork.Clear();

  m_dirayTerminateList.Clear();

  m_pSEPlayer = NULL;
  m_pAllocator = NULL;
  m_pFieldRootNode = NULL;
  m_pStaticModelPack = NULL;
  m_eventID = 0;
  m_modelID = 0;
  m_pHeap = NULL;

  m_stopUpdateFrame = -1;
  m_modelNum = 0;
  m_nowModelType = MODEL_TYPE_NONE;
  m_isExecuteTerminate = false;
  m_pStaticModelInfo = NULL;
  m_pStaticActor = NULL;
  m_modelIndex = -1;

  if( m_pManager)
  {
    m_pManager->TerminateNotification( this);
    m_pManager = NULL;
  }

  m_pResourceNodeManager = NULL;
  m_resourceWorkIndex = -1;
  m_isDynamic = false;

  m_state = STATE_NONE;

  m_isUpdateCullingEnable = true;
}

/**
 * @brief 更新
 */
void StaticModel::Update()
{
  // 更新停止フレームが設定されていたらそのフレーム分経過するまで更新を止める
  // ケンタロス岩のヒットバックのために追加
  if( m_stopUpdateFrame > 0 )
  {
    m_stopUpdateFrame--;
    return;
  }

  // 更新状態でなかったら更新しない
  if( m_state != STATE_UPDATE)
  {
    return;
  }

  // ループアニメーション更新
  this->UpdateLoopAnimation();

  // 1日アニメーション更新
  this->UpdateAllTimeAnimation();

  // イベントアニメーション更新
  this->UpdateEventAnimation();

  // 時間アニメーション更新
  this->UpdateTimeAnimation();

  // バウンディングボックスアニメーション更新
  this->UpdateBBAnimation();

  if( (m_nowModelType != MODEL_TYPE_NONE) && (m_pModel[m_nowModelType]->IsCreated()) )
  {
    // アニメーション更新
    m_pModel[m_nowModelType]->UpdateAnimation();

    // 外部から描画オンオフを書き換えられることがあるので上書き
    m_pModel[m_nowModelType]->SetVisible( m_isModelVisible );
  }
}

/**
 * @brief 配置モデルIDの取得
 * @return 配置モデルID
 */
u32 StaticModel::GetStaticModelID() const
{
  return m_modelID;
}

/**
 * @brief イベントIDの取得
 * @return イベントID
 */
u32 StaticModel::GetEventID() const
{
  return m_eventID;
}

/**
 * @brief モデル管理番号の取得
 * @return モデル管理番号
 */
s32 StaticModel::GetModelIndex() const
{
  return m_modelIndex;
}

/**
 * @brief 未初期化か
 * @retval true 未初期化
 * @retval false 初期化済み
 */
bool StaticModel::IsEmpty() const
{
  return ( m_state == STATE_NONE );
}

/**
 * @brief 現在のモデルタイプを取得 (LOD段階が取れる)
 * @return 現在のモデルタイプ
 */
ModelType StaticModel::GetNowModelType() const
{
  return m_nowModelType;
}

/**
 * @brief 更新フラグのセット
 * @param isEnable trueで更新する
 */
void StaticModel::SetUpdateEnable( bool isEnable)
{
  // 初期化が完了していなければ操作不可
  if( m_state < STATE_INITIALIZE_FINISHED)
  {
    return;
  }

  if( isEnable)
  {
    m_state = STATE_UPDATE;
  }
  else
  {
    m_state = STATE_INITIALIZE_FINISHED;
  }
}

/**
 * @brief LODの変更
 * @return 変更する段階
 */
bool StaticModel::ChangeLOD( ModelType changeType)
{
  if( changeType == MODEL_TYPE_NONE)
  {
    // 変更タイプがNONEは不正
    GFL_ASSERT( 0);
    return false;
  }

  if( m_nowModelType == changeType)
  {
    // 変更の必要がない
    return false;
  }

  if( m_pModel[changeType]->IsCreated() == false)
  {
    // 指定のモデルが生成されていない
    return false;
  }

  // 表示ON/OFF切り替え
  m_pModel[changeType]->SetVisible( m_isModelVisible);
  if( m_nowModelType != MODEL_TYPE_NONE)
  {
    m_pModel[m_nowModelType]->SetVisible( false);
  }

  // モデルタイプ切替
  ModelType oldModelType = m_nowModelType;
  m_nowModelType = changeType;

  // 変更前がモデルなしなら、アニメーションの同期は必要ない
  if( oldModelType == MODEL_TYPE_NONE)
  {
    // 表示ON/OFFのみ成功
    return true;
  }

  // 再生しているアニメーションは同期を取る
  this->SyncAnimation( changeType, oldModelType);

  return true;
}

/**
 * @brief LOD変更 Highより下は表示OFF挙動用
 * @param changeType 変更したいLODレベル
 */
bool StaticModel::ChangeLOD_VisibleOff( ModelType changeType)
{
  if( m_nowModelType == changeType)
  {
    // 変更の必要がない
    return false;
  }

  // ハイモデル以外はなしに変更
  if( changeType == MODEL_TYPE_HIGH )
  {
    m_nowModelType = changeType;

    // 時間アニメーションが動作していた場合、時間をチェックする
    if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, ANIMATION_INDEX_TIME_0) )
    {
      // 終了できる時間かチェック
      s32 hour,minute;
      this->GetNowHourAndMinute( &hour, &minute);
      bool isTimeOverRange = m_timeAnimationWork.IsTimeOverRange( hour, minute);

      if( isTimeOverRange)
      {
        m_timeAnimationWork.SetUpdateSeq( TIME_ANIMATION_UPDATE_SEQ_WAIT_END);

        // 終了アニメーション初期化
        u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, ANIMATION_INDEX_TIME_0);
        m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeEnd_EndFrame, slotIndex);
      }
    }

    // BBアニメーションが動作していた場合、衝突をチェックする
    if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, ANIMATION_INDEX_BB_0) )
    {
      if( m_BBAnimationWork.IsHitCollision() == false)
      {
        m_BBAnimationWork.SetUpdateSeq( BB_ANIMATION_UPDATE_SEQ_WAIT_END);

        // 終了アニメーション初期化
        u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, ANIMATION_INDEX_BB_0);
        m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.end_EndFrame, slotIndex);
      }
    }

  }
  else
  {
    m_nowModelType = MODEL_TYPE_NONE;

    // 時間アニメーションが動作していた場合、シーケンスはループにしておく
    if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, ANIMATION_INDEX_TIME_0) )
    {
      m_timeAnimationWork.SetUpdateSeq( TIME_ANIMATION_UPDATE_SEQ_WAIT_LOOP);
    }

    // BBアニメーションが動作していた場合、シーケンスはループにしておく
    if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, ANIMATION_INDEX_BB_0) )
    {
      m_BBAnimationWork.SetUpdateSeq( BB_ANIMATION_UPDATE_SEQ_WAIT_LOOP);
    }

    // 再生中のイベントアニメーションは終了フレームにする
    s32 eventAnimationMax = ANIMATION_INDEX_EVENT_0 + EVENT_ANIMATION_INDEX_MAX;
    for( u32 i = ANIMATION_INDEX_EVENT_0; i < eventAnimationMax; ++i)
    {
      AnimationIndex index = static_cast<AnimationIndex>( i );
      if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, index) )
      {
        this->SetAnimationLastFrame_High( index);
      }
    }
  }

  return true;
}

/**
 * @brief バウンディングボックスアニメーションStaticActor作成
 * @param pCollisionScene コリジョンシーン
 */
void StaticModel::CreateBBAnimationStaticActor( CollisionSceneDefault* pCollisionScene)
{
  m_pCollisionSceneForBBAnimation = pCollisionScene;
  m_pStaticActorBBAnimation = m_pCollisionSceneForBBAnimation->CreateStaticActor( m_pAllocator, m_pHeap);

  gfl2::math::Matrix44 transformMtx;
  this->GetBBAnimationCollisionModelTransformMatrix( &transformMtx);

  m_pStaticActorBBAnimation->SetPosition( m_srt.translate);

  switch( m_pStaticModelInfo->BBAnimationType)
  {
  case BB_ANIMATION_COLLISION_TYPE_CYLINDER:
    {
      gfl2::math::Vector3 pos = transformMtx.GetElemPosition();
      gfl2::math::Vector3 scl = transformMtx.GetElemScaling();

      m_pStaticActorBBAnimation->CreateCylinder( pos, scl.GetX() * 0.5f, scl.GetY());
    }
    break;

  case BB_ANIMATION_COLLISION_TYPE_BOX:
    {
      gfl2::math::Vector3 pos = transformMtx.GetElemPosition();
      gfl2::math::Vector3 scl = transformMtx.GetElemScaling();
      gfl2::math::Vector3 rot = transformMtx.GetElemRotationZYX();

      gfl2::math::Vector3 x,z;
      x.Set( 1.0f, 0.0f, 0.0f);
      z.Set( 0.0f, 0.0f, 1.0f);

      gfl2::math::Quaternion rotate;
      // @fix z要素にrot.GetX()を渡していたので修正
      rotate.RadianToQuaternion( rot.GetX(), rot.GetY(), rot.GetZ());

      gfl2::math::Matrix34 rotMat;
      rotate.QuaternionToMatrix( rotMat);
      x = rotMat.TransformNormal( x);
      z = rotMat.TransformNormal( z);

      m_pStaticActorBBAnimation->CreateBox( pos, x, z, scl.x * 0.5f, scl.z * 0.5f, scl.y);
    }
    break;

  default:
    GFL_ASSERT( 0);
    break;
  }

  m_pStaticActorBBAnimation->SetUserData( this);

  m_pCollisionSceneForBBAnimation->RegistStaticActor( m_pStaticActorBBAnimation);
}

/**
 * @brief バウンディングボックスアニメーションが存在するか
 * @retval true 存在する
 * @retval false 存在しない
 */
bool StaticModel::IsBBAnimationExist() const
{
  // コリジョンタイプ指定がなしになっている
  if( m_pStaticModelInfo->BBAnimationType == BB_ANIMATION_COLLISION_TYPE_NONE)
  {
    return false;
  }

  // アニメーションデータがない
  if( this->IsAnimationExist( ANIMATION_INDEX_BB_0) == false)
  {
    return false;
  }

  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPack);

  // コリジョンモデルデータがない
  void* pCollisionModelData = accessor.GetBBAnimationCollisionModel();
  if( pCollisionModelData == NULL)
  {
    return false;
  }

  // 上記が全部揃っていたら存在する
  return true;
}

/**
 * @brief  スロット間補間アニメチェンジが再生可能なモデルか？
 * ポケセンカウンター/ジガルデ装置
 */
bool StaticModel::CanUseChangeAnimeSmooth() const
{
  if( m_modelID == STATIC_MODEL_ID_COM_BM_PCOUNTER01 ||
      m_modelID == STATIC_MODEL_ID_F0301I1201_BM_OBJ01 ){
    return true;
  }
  return false;
}

//*******************************************************************
//*******************************************************************
// 以下、private
//*******************************************************************
//*******************************************************************

/**
 * @brief モデルワークの生成
 * @note リソースがある場合は生成される
 */
void StaticModel::CreateModelWork()
{
  // リソースノード作成
  m_resourceWorkIndex = m_pResourceNodeManager->Create( m_modelID, m_pStaticModelPack);

  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_pModel[i] = GFL_NEW( m_pHeap) poke_3d::model::BaseModel();

    bool isCreateModel = this->CreateModelAndMotions( static_cast<ModelType>( i));

    // モデルを生成していたら現在のモデルタイプを変更する
    if( isCreateModel)
    {
      m_nowModelType = static_cast<ModelType>( i);
    }
  }

  // ハイモデルがない配置モデルリソースパックは存在しない
  // 配置情報からエリアごとに配置モデルリソースパックを固めている
  GFL_ASSERT( m_nowModelType != MODEL_TYPE_NONE);

  // LOD挙動がハイ以外は表示OFFのモデルにLODモデルが登録されている場合はアサート
  if( m_pStaticModelInfo->lodType == LOD_TYPE_VISIBLE_OFF )
  {
    GFL_ASSERT( m_modelNum == 1 );
  }
}

/**
 * @brief モデルとモーションの生成
 * @param modelType どのモデルか。ハイ、ミドル、ローから選択
 * @retval true モデルを生成した
 * @retval false モデルを生成していない
 * @note モデルを生成した場合、アニメーションが存在していれば生成する
 */
bool StaticModel::CreateModelAndMotions( ModelType modelType)
{
  m_pModelResourceRootNode[modelType] = m_pResourceNodeManager->GetModelResourceNode( m_resourceWorkIndex, modelType);
  if( m_pModelResourceRootNode[modelType])
  {
    this->CreateModel( modelType);

    m_pModel[modelType]->SetPosition( m_srt.translate);
    m_pModel[modelType]->SetRotationQuat( m_srt.rotation);
    m_pModel[modelType]->SetScale( m_srt.scale);

    m_modelNum++;

    for( s32 cntAnimation = 0; cntAnimation < ANIMATION_INDEX_MAX; ++cntAnimation)
    {
      m_pMotionResourceRootNode[modelType][cntAnimation] = m_pResourceNodeManager->GetMotionResourceNode( m_resourceWorkIndex, modelType, static_cast<AnimationIndex>( cntAnimation));
    }
    return true;
  }
  return false;
}

/**
 * @brief モデルの生成
 * @param modelType どのモデルか。ハイ、ミドル、ローから選択
 */
void StaticModel::CreateModel( ModelType modelType)
{
  m_pModel[modelType]->Create( m_pAllocator, m_pHeap, m_pModelResourceRootNode[modelType]);
}

/**
 * @brief モデルワークの生成 動的用
 * @note リソースがある場合は生成される
 */
void StaticModel::CreateModelWorkDynamic()
{
  for( u32 i = 0; i < MODEL_TYPE_MAX; ++i)
  {
    m_pModel[i] = GFL_NEW( m_pHeap) poke_3d::model::BaseModel();

    bool isCreateModel = this->CreateModelAndMotionsDynamic( static_cast<ModelType>( i));

    // モデルを生成していたら現在のモデルタイプを変更する
    if( isCreateModel)
    {
      m_nowModelType = static_cast<ModelType>( i);
    }
  }

  // ハイモデルがない配置モデルリソースパックは存在しない
  // 配置情報からエリアごとに配置モデルリソースパックを固めている
  GFL_ASSERT( m_nowModelType != MODEL_TYPE_NONE);

  // LOD挙動がハイ以外は表示OFFのモデルにLODモデルが登録されている場合はアサート
  if( m_pStaticModelInfo->lodType == LOD_TYPE_VISIBLE_OFF )
  {
    GFL_ASSERT( m_modelNum == 1 );
  }
}

/**
 * @brief モデルとモーションの生成 動的用
 * @param modelType どのモデルか。ハイ、ミドル、ローから選択
 * @retval true モデルを生成した
 * @retval false モデルを生成していない
 * @note モデルを生成した場合、アニメーションが存在していれば生成する
 */
bool StaticModel::CreateModelAndMotionsDynamic( ModelType modelType)
{
  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPack);
  
  void* pModelData;
  pModelData = accessor.GetModelBinary( modelType);
  if( pModelData)
  {
    ModelResourceData modelResourceData;
    modelResourceData.pModelBinary = pModelData;
    this->CreateModelResourceNode( modelType, modelResourceData);
    this->CreateModel( modelType);

    m_pModel[modelType]->SetPosition( m_srt.translate);
    m_pModel[modelType]->SetRotationQuat( m_srt.rotation);
    m_pModel[modelType]->SetScale( m_srt.scale);

    m_modelNum++;

    for( s32 cntAnimation = 0; cntAnimation < ANIMATION_INDEX_MAX; ++cntAnimation)
    {
      MotionResourceData motionResourceData;
      void* pMotionData = accessor.GetMotionBinary( modelType, cntAnimation);
      if( pMotionData)
      {
        motionResourceData.pMotionBinary = pMotionData;
        this->CreateMotionResourceNode( modelType, cntAnimation, motionResourceData);
      }
    }

    return true;
  }
  return false;
}

/**
 * @brief モデルリソースノードの生成
 * @param modelType どのモデルか。ハイ、ミドル、ローから選択
 * @param modelResourceData モデルインスタンスノード生成に必要なデータ
 */
void StaticModel::CreateModelResourceNode( ModelType modelType, const ModelResourceData& modelResourceData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( modelResourceData.pModelBinary));
  m_pModelResourceRootNode[modelType] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData);

  m_pFieldRootNode->AddResourceChild( m_pModelResourceRootNode[modelType]);
}

/**
 * @brief モーションリソースノードの生成
 * @param modelType どのモデルのモーションか。ハイ、ミドル、ローから選択
 * @param index アニメーション番号
 * @param modelResourceData モーションリソースノード生成に必要なデータ
 */
void StaticModel::CreateMotionResourceNode( ModelType modelType, u32 index, const MotionResourceData& motionResourceData)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData( static_cast<c8*>( motionResourceData.pMotionBinary));
  m_pMotionResourceRootNode[modelType][index] =  gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData);

  m_pFieldRootNode->AddResourceChild( m_pMotionResourceRootNode[modelType][index]);
}

/**
 * @brief 現在のモデルタイプに指定したアニメーションが存在するか
 * @retval true　存在する
 * @retval false 存在しない
 */
bool StaticModel::IsAnimationExist( AnimationIndex index) const
{
  return this->IsAnimationExist( m_nowModelType, index);
}

/**
 * @brief 指定したアニメーションが存在するか
 * @retval true　存在する
 * @retval false 存在しない
 */
bool StaticModel::IsAnimationExist( ModelType type, AnimationIndex index) const
{
  if( type == MODEL_TYPE_NONE)
  {
    return false;
  }

  if( m_pMotionResourceRootNode[type][index])
  {
    return true;
  }
  return false;
}

/**
 * @brief 指定したアニメーションが再生中か
 * @retval true　再生中
 * @retval false それ以外
 */
bool StaticModel::IsAnimationPlaying( ModelType type, AnimationIndex index) const
{
  if( this->IsAnimationExist( type, index) == false)
  {
    // アニメーションリソースが存在しない
    return false;
  }

  if( m_pModel[type]->IsCreated() == false)
  {
    // モデルが生成されていない
    return false;
  }

  if( m_animationSlotControlWork.IsEnableSlotIndex( type, index) == false)
  {
    // 有効なスロットではない(再生されていない)
    return false;
  }

  s32 slotIndex = m_animationSlotControlWork.GetSlotIndex( type, index);
  if( m_pModel[type]->GetAnimationResourceNode( slotIndex) == NULL)
  {
    // スロットにアニメーションリソースがセットされていない
    GFL_ASSERT( 0);
    return false;;
  }

  return true;
}

/**
 * @brief 指定したアニメーションが再生中か
 * @retval true　再生中
 * @retval false それ以外
 * @fix スロット指定版を用意
 */
bool StaticModel::IsAnimationPlayingEx( ModelType type, AnimationIndex index, s32 slotIndex) const
{
  if( this->IsAnimationExist( type, index) == false)
  {
    // アニメーションリソースが存在しない
    return false;
  }

  if( m_pModel[type]->IsCreated() == false)
  {
    // モデルが生成されていない
    return false;
  }

  if( m_animationSlotControlWork.IsEnableSlotIndex( type, index) == false)
  {
    // 有効なスロットではない(再生されていない)
    return false;
  }

  if( m_pModel[type]->GetAnimationResourceNode( slotIndex) == NULL)
  {
    // スロットにアニメーションリソースがセットされていない
    GFL_ASSERT( 0);
    return false;;
  }

  return true;
}

/**
 * @brief アニメーション変更
 * @param index アニメーション番号
 * @param isLoop trueでループ。デフォルトはtrue
 * @param stepFrame 再生速度。デフォルトは1.0f
 * @retval true 成功
 * @retval false 失敗
 */
bool StaticModel::ChangeAnimation( AnimationIndex index, bool isLoop, f32 stepFrame)
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  if( m_pMotionResourceRootNode[m_nowModelType][index] && m_pModel[m_nowModelType]->IsCreated())
  {
    m_animationSlotControlWork.SetSlotIndex( m_nowModelType, index);
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, index);

    m_pModel[m_nowModelType]->ChangeAnimationByResourceNode( m_pMotionResourceRootNode[m_nowModelType][index], slotIndex);
    m_pModel[m_nowModelType]->SetAnimationLoop( isLoop, slotIndex);
    m_pModel[m_nowModelType]->SetAnimationStepFrame( stepFrame, slotIndex);

    return true;
  }
  return false;
}

/**
 * @brief アニメーションを同期させて再生
 * @param changeType 変更するモデルタイプ
 * @param oldType 変更前のモデルタイプ
 */
void StaticModel::SyncAnimation( ModelType changeType, ModelType oldType)
{
  for( s32 i = 0; i < ANIMATION_INDEX_MAX; ++i)
  {
    if( m_animationSlotControlWork.IsEnableSlotIndex( oldType, i) == false)
    {
      continue;
    }

    u32 oldSlotIndex = m_animationSlotControlWork.GetSlotIndex( oldType, i);

    if( m_pModel[oldType]->GetAnimationResourceNode( oldSlotIndex) == NULL)
    {
      continue;
    }

    // アニメーションの情報を保持しておく
    f32 frame = m_pModel[oldType]->GetAnimationFrame( oldSlotIndex);
    f32 stepFrame = m_pModel[oldType]->GetAnimationStepFrame( oldSlotIndex);
    bool isLoop = GFL_BOOL_CAST( m_pModel[oldType]->IsAnimationLoop( oldSlotIndex));

    // アニメーション終了
    m_pModel[oldType]->ChangeAnimationByResourceNode( NULL, oldSlotIndex);
    m_animationSlotControlWork.ClearSlotIndex( oldType, i);

    // アニメーション初期化、再生
    m_animationSlotControlWork.SetSlotIndex( changeType, i);
    u32 nextSlotIndex = m_animationSlotControlWork.GetSlotIndex( changeType, i);
    m_pModel[changeType]->ChangeAnimationByResourceNode( m_pMotionResourceRootNode[changeType][i], nextSlotIndex);

    // 保持していた情報をセット
    m_pModel[changeType]->SetAnimationFrame( frame, nextSlotIndex);
    m_pModel[changeType]->SetAnimationStepFrame( stepFrame, nextSlotIndex);
    m_pModel[changeType]->SetAnimationLoop( isLoop, nextSlotIndex);
  }
}

/**
 * @brief アニメーションのフレームを設定
 * @param index アニメーション番号
 * @param frame フレーム
 */
void StaticModel::SetAnimationFrame( AnimationIndex index, f32 frame)
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return;
  }

  if( m_animationSlotControlWork.IsEnableSlotIndex( m_nowModelType, index) &&
      m_pModel[m_nowModelType]->IsCreated())
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, index);

    m_pModel[m_nowModelType]->SetAnimationFrame( frame, slotIndex);
  }
}

/**
 * @brief アニメーションのステップフレームを設定
 * @param index アニメーション番号
 * @param stepFrame ステップフレーム
 */
void StaticModel::SetAnimationStepFrame( AnimationIndex index, f32 stepFrame)
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return;
  }

  if( m_animationSlotControlWork.IsEnableSlotIndex( m_nowModelType, index) &&
      m_pModel[m_nowModelType]->IsCreated())
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, index);

    m_pModel[m_nowModelType]->SetAnimationStepFrame( stepFrame, slotIndex);
  }
}

/**
 * @brief アニメーション変更 ハイモデル限定
 * @param index アニメーション番号
 * @param isLoop trueでループ。デフォルトはtrue
 * @param stepFrame 再生速度。デフォルトは1.0f
 * @retval true 成功
 * @retval false 失敗
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
bool StaticModel::ChangeAnimation_High( AnimationIndex index, bool isLoop, f32 stepFrame)
{
  if( this->IsAnimationExist( MODEL_TYPE_HIGH, index) && m_pModel[MODEL_TYPE_HIGH]->IsCreated())
  {
    m_animationSlotControlWork.SetSlotIndex( MODEL_TYPE_HIGH, index);
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, index);

    m_pModel[MODEL_TYPE_HIGH]->ChangeAnimationByResourceNode( m_pMotionResourceRootNode[MODEL_TYPE_HIGH][index], slotIndex);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationLoop( isLoop, slotIndex);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationStepFrame( stepFrame, slotIndex);
    return true;
  }
  return false;
}

/**
 * @brief アニメーション変更 スロット指定版 ハイモデル限定
 * @param slotIndex スロット番号
 * @param index アニメーション番号
 * @param isLoop trueでループ。デフォルトはtrue
 * @param stepFrame 再生速度。デフォルトは1.0f
 * @param lerpTime 補間フレーム
 * @retval true 成功
 * @retval false 失敗
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
bool StaticModel::ChangeAnimationEx_High( s32 slotIndex, AnimationIndex index, bool isLoop, f32 stepFrame, f32 startFrame, f32 lerpTime)
{
  if( this->IsAnimationExist( MODEL_TYPE_HIGH, index) && m_pModel[MODEL_TYPE_HIGH]->IsCreated())
  {
    m_animationSlotControlWork.SetSlotIndex( MODEL_TYPE_HIGH, index);

    if( lerpTime < 0.0f)
    {
      m_pModel[MODEL_TYPE_HIGH]->ChangeAnimationByResourceNode( m_pMotionResourceRootNode[MODEL_TYPE_HIGH][index], slotIndex);
    }
    else
    {
      m_pModel[MODEL_TYPE_HIGH]->ChangeAnimationSmoothByResourceNode( m_pMotionResourceRootNode[MODEL_TYPE_HIGH][index], lerpTime, slotIndex);
    }

    m_pModel[MODEL_TYPE_HIGH]->SetAnimationLoop( isLoop, slotIndex);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationStepFrame( stepFrame, slotIndex);

    return true;
  }

  return false;
}

/**
 * @brief アニメーション停止 ハイモデル限定
 * @param index アニメーション番号
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
void StaticModel::StopAnimation_High( AnimationIndex index)
{
  if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, index) )
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, index);
    m_pModel[MODEL_TYPE_HIGH]->ChangeAnimationByResourceNode( NULL, slotIndex);
    m_animationSlotControlWork.ClearSlotIndex( MODEL_TYPE_HIGH, index);
  }
}

/**
 * @brief アニメーション停止 スロット指定版 ハイモデル限定
 * @param slotIndex スロット番号
 * @param index アニメーション番号
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
void StaticModel::StopAnimationEx_High( s32 slotIndex, AnimationIndex index)
{
  // @fix スロット指定版に変更
  if( this->IsAnimationPlayingEx( MODEL_TYPE_HIGH, index, slotIndex) )
  {
    m_pModel[MODEL_TYPE_HIGH]->ChangeAnimationByResourceNode( NULL, slotIndex);
    m_animationSlotControlWork.ClearSlotIndex( MODEL_TYPE_HIGH, index);
  }
}

/**
 * @brief アニメーションフレーム変更 ハイモデル限定
 * @param index アニメーション番号
 * @param frame 変更フレーム数
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
void StaticModel::SetAnimationFrame_High( AnimationIndex index, f32 frame)
{
  if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, index) )
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, index);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( frame, slotIndex);
  }
}

/**
 * @brief アニメーションフレームを最終フレームに変更 ハイモデル限定
 * @param index アニメーション番号
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
void StaticModel::SetAnimationLastFrame_High( AnimationIndex index)
{
  if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, index) )
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, index);
    f32 frame = m_pModel[MODEL_TYPE_HIGH]->GetAnimationEndFrame( slotIndex);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( frame, slotIndex);
  }
}

/**
 * @brief アニメーションフレームを最終フレームに変更 スロット指定版 ハイモデル限定
 * @param index アニメーション番号
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 * @fix スロット指定版を用意
 */
void StaticModel::SetAnimationLastFrameEx_High( s32 slotIndex, AnimationIndex index)
{
  if( this->IsAnimationPlayingEx( MODEL_TYPE_HIGH, index, slotIndex) )
  {
    f32 frame = m_pModel[MODEL_TYPE_HIGH]->GetAnimationEndFrame( slotIndex);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( frame, slotIndex);
  }
}

/**
 * @brief アニメーションステップフレーム変更 ハイモデル限定
 * @param index アニメーション番号
 * @param stepFrame ステップフレーム
 * @note LOD挙動がVISIBLE_OFFのものが使用する想定
 */
void StaticModel::SetAnimationStepFrame_High( AnimationIndex index, f32 stepFrame)
{
  if( this->IsAnimationPlaying( MODEL_TYPE_HIGH, index) )
  {
    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, index);
    m_pModel[MODEL_TYPE_HIGH]->SetAnimationStepFrame( stepFrame, slotIndex);
  }
}

/**
 * @brief イベントアニメーションに対応するSEの再生
 * @param index イベントアニメーション番号。0-3の範囲
 * @param isInverse 逆再生ならtrue
 * @note 対応するSEがない場合は再生しない
 */
void StaticModel::PlayEvnetAnimationSE( u32 index, bool isInverse)
{
  if( m_pSEPlayer->IsPlaySE( m_pStaticModelInfo->programID))
  {
    m_pSEPlayer->PlaySE( m_pStaticModelInfo->programID, index, isInverse);
  }
}

/**
 * @brief 現在の時間と分を取得
 * @param pOutHour 時間格納先
 * @param pOutMinute 分格納先
 */
void StaticModel::GetNowHourAndMinute( s32* pOutHour, s32* pOutMinute) const
{
  GameSys::AlolaTime time;
  time.Initialize();
  *pOutHour   = time.GetHour();
  *pOutMinute = time.GetMinute();
}

/**
 * @brief ループアニメーションの更新
 */
void StaticModel::UpdateLoopAnimation()
{
  if( this->IsAnimationExist( ANIMATION_INDEX_LOOP_0) == false)
  {
    return;
  }

}

/**
 * @brief 1日アニメーションの更新
 */
void StaticModel::UpdateAllTimeAnimation()
{
  if( this->IsAnimationExist( ANIMATION_INDEX_ALL_TIME_0) == false)
  {
    return;
  }

  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return;
  }

  s32 hour,minute;
  this->GetNowHourAndMinute( &hour, &minute);

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_ALL_TIME_0);
  if( m_allTimeAnimationWork.IsUpdate( hour, minute))
  {
    f32 maxFrame = m_pModel[m_nowModelType]->GetAnimationEndFrame( slotIndex);
    f32 frame = m_allTimeAnimationWork.CalculateAnimationFrame( maxFrame, hour, minute);

    m_pModel[m_nowModelType]->SetAnimationFrame( frame, slotIndex);
  }
}

/**
 * @brief イベントアニメーション更新
 */
void StaticModel::UpdateEventAnimation()
{
  // イベントアニメーションは再生した方に停止してもらう
  // ポケセンカウンターのように、同じマテリアルをアニメーションさせる複数のイベントアニメを連続で流す構成だと、
  // 停止させないと最後に再生したスロットのものが後勝ちして、2度目以向に見た目の不具合となる
  // 
  // ケンタロス岩は停止させることができないため流しっぱなしだが、
  // 最終フレームで止まるので問題ない。
}

/**
 * @brief 時間アニメーション更新
 */
void StaticModel::UpdateTimeAnimation()
{
  if( this->IsAnimationExist( ANIMATION_INDEX_TIME_0) == false)
  {
    return;
  }

  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return;
  }

  bool isFinish;
  isFinish = (this->*m_TimeAnimationUpdateFunc[m_timeAnimationWork.GetUpdateSeq()])();

  if( isFinish)
  {
    bool isResetSeq;
    isResetSeq = m_timeAnimationWork.AddUpdateSeq();

    if( isResetSeq)
    {
      // アニメーション終了
      u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);
      m_pModel[m_nowModelType]->ChangeAnimationByResourceNode( NULL, slotIndex);
      m_animationSlotControlWork.ClearSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);
    }
  }
}

/**
 * @brief 時間アニメーション更新 時間外
 */
bool StaticModel::TimeAnimationUpdateFunc_None()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  s32 hour,minute;
  this->GetNowHourAndMinute( &hour, &minute);

  // 開始するかチェック
  if( m_timeAnimationWork.IsTimeInRange( hour, minute))
  {
    // アニメーション開始
    this->ChangeAnimation( ANIMATION_INDEX_TIME_0, false);

    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);
    m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeStart_StartFrame, slotIndex);
    return true;
  }
  return false;
}

/**
 * @brief 時間アニメーション更新 開始アニメーション終了待ち
 */
bool StaticModel::TimeAnimationUpdateFunc_WaitStart()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);

  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->timeAnimeStart_EndFrame)
  {
    // ループアニメーション初期化
    m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeLoop_StartFrame, slotIndex);
    return true;
  }
  return false;
}

/**
 * @brief 時間アニメーション更新 ループアニメーション終了待ち
 */
bool StaticModel::TimeAnimationUpdateFunc_WaitLoop()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  // 終了できる時間かチェック
  s32 hour,minute;
  this->GetNowHourAndMinute( &hour, &minute);
  bool isTimeOverRange = m_timeAnimationWork.IsTimeOverRange( hour, minute);

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);

  // フレームチェック
  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->timeAnimeLoop_EndFrame)
  {
    // 時間外になっていたら終了アニメーションのフレームにして、次のシーケンスに遷移
    if( isTimeOverRange)
    {
      // 終了アニメーション初期化
      m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeEnd_StartFrame, slotIndex);
      return true;
    }
    // 時間内のまままならループアニメーションのフレームにして、このシーケンスを続行
    else
    {
      // ループアニメーション初期化
      m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->timeAnimeLoop_StartFrame, slotIndex);
    }
  }
  return false;
}

/**
 * @brief 時間アニメーション更新 終了アニメーション終了待ち
 */
bool StaticModel::TimeAnimationUpdateFunc_WaitEnd()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_TIME_0);

  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->timeAnimeEnd_EndFrame)
  {
    return true;
  }
  return false;
}

/**
 * @brief バウンディングボックスアニメーション更新
 */
void StaticModel::UpdateBBAnimation()
{
  if( this->IsAnimationExist( ANIMATION_INDEX_BB_0) == false)
  {
    return;
  }

  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return;
  }

  bool isFinish;
  isFinish = (this->*m_BBAnimationUpdateFunc[m_BBAnimationWork.GetUpdateSeq()])();

  if( isFinish)
  {
    bool isResetSeq;
    isResetSeq = m_BBAnimationWork.AddUpdateSeq();

    if( isResetSeq)
    {
      // アニメーション終了
      u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);
      m_pModel[m_nowModelType]->ChangeAnimationByResourceNode( NULL, slotIndex);
      m_animationSlotControlWork.ClearSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);
    }
  }
}

/**
 * @brief バウンディングボックスアニメーション更新 なし
 */
bool StaticModel::BBAnimationUpdateFunc_None()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  // 開始するかチェック
  if( m_BBAnimationWork.IsStartAnimation())
  {
    // アニメーション開始
    this->ChangeAnimation( ANIMATION_INDEX_BB_0, false);

    u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);
    m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.start_StartFrame, slotIndex);
    return true;
  }
  return false;
}

/**
 * @brief バウンディングボックスアニメーション更新 開始アニメーション終了待ち
 */
bool StaticModel::BBAnimationUpdateFunc_WaitStart()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);

  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->BBAnimationFrameWork.start_EndFrame)
  {
    // ループアニメーション初期化
    m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.loop_StartFrame, slotIndex);
    return true;
  }
  return false;
}

/**
 * @brief バウンディングボックスアニメーション更新 ループアニメーション終了待ち
 */
bool StaticModel::BBAnimationUpdateFunc_WaitLoop()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);

  // フレームチェック
  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->BBAnimationFrameWork.loop_EndFrame)
  {
    // 時間外になっていたら終了アニメーションのフレームにして、次のシーケンスに遷移
    if( m_BBAnimationWork.IsHitCollision() == false)
    {
      // 終了アニメーション初期化
      m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.end_StartFrame, slotIndex);
      return true;
    }
    // 時間内のまままならループアニメーションのフレームにして、このシーケンスを続行
    else
    {
      // ループアニメーション初期化
      m_pModel[m_nowModelType]->SetAnimationFrame( m_pStaticModelInfo->BBAnimationFrameWork.loop_StartFrame, slotIndex);
    }
  }
  return false;
}

/**
 * @brief バウンディングボックスアニメーション更新 終了アニメーション終了待ち
 */
bool StaticModel::BBAnimationUpdateFunc_WaitEnd()
{
  if( m_nowModelType == MODEL_TYPE_NONE)
  {
    return false;
  }

  u32 slotIndex = m_animationSlotControlWork.GetSlotIndex( m_nowModelType, ANIMATION_INDEX_BB_0);

  f32 currentFrame = m_pModel[m_nowModelType]->GetAnimationFrame( slotIndex);
  if( currentFrame >= m_pStaticModelInfo->BBAnimationFrameWork.end_EndFrame)
  {
    return true;
  }
  return false;
}

/**
 * @brief バウンディングボックスアニメーションコリジョン判定の状態を通知してもらう
 * @param state バウンディングボックスアニメーションのコリジョン判定の状態
 * @note マネージャーにのみ公開する
 */
void StaticModel::NotificationBBAnimationCollisionState( const BBAnimationCollisionState& state)
{
  m_BBAnimationWork.NotificationBBAnimationCollisionState( state);
}

/**
 * @brief バウンディングボックスアニメーションコリジョンモデルのTransformMatrix取得
 * @param pOut 出力先マトリクス
 */
void StaticModel::GetBBAnimationCollisionModelTransformMatrix( gfl2::math::Matrix44* pOut)
{
  StaticModelPackAccessor accessor;
  accessor.SetData( m_pStaticModelPack);

  void* pCollisionModelData = accessor.GetBBAnimationCollisionModel();

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( static_cast<c8*>( pCollisionModelData));

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pResourceNode;
  pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pAllocator, &resourceData);

  gfl2::renderingengine::scenegraph::resource::GfMdlResourceNode* pModelResourceNode = NULL;
  gfl2::renderingengine::scenegraph::DagNode* pNode = pResourceNode->GetChild()->GetChild(); // root -> mdlp -> mdl,tex,shd...

  while( 1 )
  {
    if( pNode == NULL )
    {
      break;
    }

    pModelResourceNode = pNode->SafeCast<gfl2::renderingengine::scenegraph::resource::GfMdlResourceNode>();
    if( pModelResourceNode )
    {
      break;
    }

    pNode = pNode->GetSibling();
  }

  if( pModelResourceNode )
  {
    *pOut = pModelResourceNode->GetModelTransform();
  }
  else
  {
    GFL_ASSERT( 0 );
    *pOut = gfl2::math::Matrix44::GetIdentity();
  }

  GFL_DELETE( pResourceNode);
  pResourceNode = NULL;
  pModelResourceNode = NULL;
}

//===================================================================
// 1日アニメーションワーク
//===================================================================

/**
 * @brief コンストラクタ
 */
StaticModel::AllTimeAnimationWork::AllTimeAnimationWork()
  : m_isUpdateRequest( false)
  , m_beforeHour( -1)
  , m_beforeMinute( -1)
{
}

/**
 * @brief デストラクタ
 */
StaticModel::AllTimeAnimationWork::~AllTimeAnimationWork()
{
}

/**
 * @brief 初期化
 */
void StaticModel::AllTimeAnimationWork::Initialize()
{
  this->Clear();
}

/**
 * @brief クリア
 */
void StaticModel::AllTimeAnimationWork::Clear()
{
  m_isUpdateRequest   = false;
  m_beforeHour        = -1;
  m_beforeMinute      = -1;
}

/**
 * @brief アニメーション更新リクエストがあるか
 * @retval true 更新
 * @retval false 必要なし
 */
bool StaticModel::AllTimeAnimationWork::IsUpdateAnimationRequest() const
{
  return m_isUpdateRequest;
}

/**
 * @brief 更新タイミングか
 * @retval true 更新する
 * @retval false 更新しない
 */
bool StaticModel::AllTimeAnimationWork::IsUpdate( s32 hour, s32 minute)
{
  // @fix 時間が同じで分がbeforeより小さいときに対応できていなかった。+-1分差なら変更するように修正
  // @fix GFNMCat[4280] 天球の見た目の変化と合わせるため、更新間隔を1分に修正
  if( gfl2::math::Abs( minute - m_beforeMinute ) >= 1 ||
      hour != m_beforeHour )
  {
    m_beforeHour = hour;
    m_beforeMinute = minute;
    m_isUpdateRequest = true;
    return true;
  }
  m_isUpdateRequest = false;
  return false;
}

/**
 * @brief アニメーションフレームを計算
 * @param maxFrame 最大フレーム数
 * @param hour 現在の時間
 * @param minute 現在の分
 * @return 現在の時間のアニメーションのフレーム数
 */
f32 StaticModel::AllTimeAnimationWork::CalculateAnimationFrame( f32 maxFrame, s32 hour, s32 minute) const
{
  f32 oneDayMinute = 24.0f * 60.0f;
  f32 nowMinute = (hour * 60.0f) + minute;

  f32 t = 0.0f;
  if( gfl2::math::IsAlmostZero( nowMinute) == false)
  {
    t = nowMinute / oneDayMinute;
    t = gfl2::math::Clamp( t, 0.0f, 1.0f);
  }
  return gfl2::math::Float::Floor( ( maxFrame * t ), 2);
}

//===================================================================
// 時間アニメーションワーク
//===================================================================

/**
 * @brief コンストラクタ
 */
StaticModel::TimeAnimationWork::TimeAnimationWork()
  : m_isUpdateRequest( false)
  , m_updateSeq( 0)
  , m_startHour( -1)
  , m_startMinute( -1)
  , m_endHour( -1)
  , m_endMinute( -1)
{}

/**
 * @brief デストラクタ
 */
StaticModel::TimeAnimationWork::~TimeAnimationWork()
{
}

/**
 * @brief 初期化
 * @param startHour 開始時間
 * @param startMinute 開始分
 * @param endHour 終了時間
 * @param endMinute 終了分
 */
void StaticModel::TimeAnimationWork::Initialize( s32 startHour, s32 startMinute, s32 endHour, s32 endMinute)
{
  this->Clear();

  m_startHour   = startHour;
  m_startMinute = startMinute;
  m_endHour     = endHour;
  m_endMinute   = endMinute;
}

/**
 * @brief クリア
 */
void StaticModel::TimeAnimationWork::Clear()
{
  m_isUpdateRequest     = false;
  m_updateSeq           = 0;
  m_startHour           = -1;
  m_startMinute         = -1;
  m_endHour             = -1;
  m_endMinute           = -1;
}

/**
 * @brief アニメーション更新リクエストがあるか
 * @retval true 更新
 * @retval false 必要なし
 */
bool StaticModel::TimeAnimationWork::IsUpdateAnimationRequest() const
{
  return m_isUpdateRequest;
}

/**
* @brief 時間アニメ開始時間と終了時間の範囲内か
* @param hour 比較したい時間
* @param minute 比較したい分
* @retval true 範囲内
* @retval false 範囲外
*/
bool StaticModel::TimeAnimationWork::IsTimeInRange( s32 hour, s32 minute) const
{
  s32 startTime,endTime,nowTime;
  this->GetCombineTime( &startTime, &endTime, &nowTime, hour, minute);

  if( nowTime >= startTime && nowTime <= endTime)
  {
    return true;
  }
  return false;
}

/**
 * @brief 時間アニメ開始時間と終了時間の範囲外か
 * @param hour 比較したい時間
 * @param minute 比較したい分
 * @retval true 範囲内
 * @retval false 範囲外
 */
bool StaticModel::TimeAnimationWork::IsTimeOverRange( s32 hour, s32 minute) const
{
  s32 startTime,endTime,nowTime;
  this->GetCombineTime( &startTime, &endTime, &nowTime, hour, minute);

  if( nowTime < startTime || nowTime > endTime)
  {
    return true;
  }
  return false;
}

/**
 * @brief 更新シーケンスを進める
 */
bool StaticModel::TimeAnimationWork::AddUpdateSeq()
{
  m_updateSeq++;
  m_isUpdateRequest = true;
  if( m_updateSeq >= TIME_ANIMATION_UPDATE_SEQ_MAX)
  {
    m_updateSeq = TIME_ANIMATION_UPDATE_SEQ_NONE;
    m_isUpdateRequest = false;
    return true;
  }
  return false;
}

/**
* @brief 更新シーケンス設定
*/
void StaticModel::TimeAnimationWork::SetUpdateSeq( u8 seq)
{
  m_isUpdateRequest = true;
  m_updateSeq = seq;
}

/**
 * @brief 更新シーケンスを取得
 * @return シーケンス
 */
u8 StaticModel::TimeAnimationWork::GetUpdateSeq() const
{
  return m_updateSeq;
}

/**
 * @brief 時間と分を結合した時間値の取得
 * @param pOutStart 開始時間の計算結果を格納する変数のポインタ
 * @param pOutEnd 終了時間の計算結果を格納する変数のポインタ
 * @param pOutNow 現在の時間の計算結果を格納する変数のポインタ
 * @param nowHour 現在の時間
 * @param nowMinute 現在の分
 */
void StaticModel::TimeAnimationWork::GetCombineTime( s32* pOutStart, s32* pOutEnd, s32* pOutNow, s32 nowHour, s32 nowMinute) const
{
  this->ConvertTimeToCombineTime( pOutStart, m_startHour, m_startMinute);

  if( m_endHour > m_startHour)
  {
    this->ConvertTimeToCombineTime( pOutEnd, m_endHour, m_endMinute);
    this->ConvertTimeToCombineTime( pOutNow, nowHour, nowMinute);
  }
  else
  {
    this->ConvertTimeToCombineTime( pOutEnd, m_endHour, m_endMinute, true);
    this->ConvertTimeToCombineTime( pOutNow, nowHour, nowMinute, true);
  }
}

/**
 * @brief 時間と分を結合した時間値に変換する
 * @param pOut 計算結果を格納する変数のポインタ
 * @param hour 時間
 * @param minute 分
 * @param isIncrementDigit 桁を増やすかどうか(デフォルトfalse)
 */
void StaticModel::TimeAnimationWork::ConvertTimeToCombineTime( s32* pOut, s32 hour, s32 minute, bool isIncrementDigit) const
{
  if( isIncrementDigit == false)
  {
    *pOut = hour * 100 + minute;
  }
  else
  {
    *pOut = hour * 1000 + minute;
  }
}

//===================================================================
// バウンディングボックスアニメーションワーク
//===================================================================

/**
 * @brief コンストラクタ
 */
StaticModel::BBAnimationWork::BBAnimationWork()
  : m_isUpdateRequest( false)
  , m_updateSeq( BB_ANIMATION_UPDATE_SEQ_NONE)
  , m_state()
{
}

/**
 * @brief デストラクタ
 */
StaticModel::BBAnimationWork::~BBAnimationWork()
{
}

/**
 * @brief 初期化
 */
void StaticModel::BBAnimationWork::Initialize()
{
  this->Clear();
}

/**
 * @brief クリア
 */
void StaticModel::BBAnimationWork::Clear()
{
  m_isUpdateRequest = false;
  m_updateSeq = BB_ANIMATION_UPDATE_SEQ_NONE;
  m_state.isHit = false;
}

/**
 * @brief アニメーション更新リクエストがあるか
 * @retval true 更新
 * @retval false 必要なし
 */
bool StaticModel::BBAnimationWork::IsUpdateAnimationRequest() const
{
  return m_isUpdateRequest;
}

/**
 * @brief 更新シーケンスを進める
 */
bool StaticModel::BBAnimationWork::AddUpdateSeq()
{
  m_updateSeq++;
  m_isUpdateRequest = true;
  if( m_updateSeq >= BB_ANIMATION_UPDATE_SEQ_MAX)
  {
    m_updateSeq = BB_ANIMATION_UPDATE_SEQ_NONE;
    m_isUpdateRequest = false;
    return true;
  }
  return false;
}

/**
* @brief 更新シーケンス設定
*/
void StaticModel::BBAnimationWork::SetUpdateSeq( u8 seq)
{
  m_isUpdateRequest = true;
  m_updateSeq = seq;
}

/**
 * @brief 更新シーケンスを取得
 * @return シーケンス
 */
u8 StaticModel::BBAnimationWork::GetUpdateSeq() const
{
  return m_updateSeq;
}

/**
* @brief バウンディングボックスアニメーションコリジョン判定の状態を通知してもらう
* @param state バウンディングボックスアニメーションのコリジョン判定の状態
*/
void StaticModel::BBAnimationWork::NotificationBBAnimationCollisionState( const BBAnimationCollisionState& state)
{
  m_state = state;
}

/**
* @brief コリジョンと当たっているか
* @retval true 当たっている
* @retval false 当たっていない
*/
bool StaticModel::BBAnimationWork::IsHitCollision() const
{
  if( m_state.isHit)
  {
    return true;
  }
  return false;
}

/**
* @brief アニメーションを開始するか
* @retval true 開始する
* @retval false 開始しない
*/
bool StaticModel::BBAnimationWork::IsStartAnimation() const
{
  if( this->IsHitCollision() && m_updateSeq == BB_ANIMATION_UPDATE_SEQ_NONE)
  {
    return true;
  }
  return false;
}

//===================================================================
// アニメーションスロット制御用ワーク
//===================================================================

/**
 * @brief コンストラクタ
 */
StaticModel::AnimationSlotControlWork::AnimationSlotControlWork()
{
  this->Clear();
}

/**
 * @brief デストラクタ
 */
StaticModel::AnimationSlotControlWork::~AnimationSlotControlWork()
{
}

/**
 * @brief 初期化
 */
void StaticModel::AnimationSlotControlWork::Initialize()
{
  this->Clear();
}

/**
 * @brief クリア
 */
void StaticModel::AnimationSlotControlWork::Clear()
{
  for( u32 cntModelType = 0; cntModelType < MODEL_TYPE_MAX; ++cntModelType)
  {
    for( u32 cntMotionIndex = 0; cntMotionIndex < ANIMATION_INDEX_MAX; ++cntMotionIndex)
    {
      m_isUseSlotIndex[cntModelType][cntMotionIndex] = false;
    }
  }
}

/**
 * @brief スロット番号取得
 * @param animationndex アニメーション番号
 * @return スロット番号
 */
u32 StaticModel::AnimationSlotControlWork::GetSlotIndex( u32 modelType, u32 animationIndex) const
{
  GFL_ASSERT( modelType < MODEL_TYPE_MAX);
  GFL_ASSERT( animationIndex < ANIMATION_INDEX_MAX);
  if( m_isUseSlotIndex[modelType][animationIndex])
  {
    return animationIndex;
  }
  return INVALID_SLOT_INDEX;
}

/**
 * @brief スロット番号設定
 * @param animationndex アニメーション番号
 */
void StaticModel::AnimationSlotControlWork::SetSlotIndex( u32 modelType, u32 animationIndex)
{
  GFL_ASSERT( modelType < MODEL_TYPE_MAX);
  GFL_ASSERT( animationIndex < ANIMATION_INDEX_MAX);
  m_isUseSlotIndex[modelType][animationIndex] = true;
}

/**
 * @brief スロット番号クリア
 * @param animationndex アニメーション番号
 */
void StaticModel::AnimationSlotControlWork::ClearSlotIndex( u32 modelType, u32 animationIndex)
{
  GFL_ASSERT( modelType < MODEL_TYPE_MAX);
  GFL_ASSERT( animationIndex < ANIMATION_INDEX_MAX);
  m_isUseSlotIndex[modelType][animationIndex] = false;
}

/**
 * @brief 有効なスロット番号か
 * @param animationndex アニメーション番号
 * @retval true 有効
 * @retval true 無効
 */
bool StaticModel::AnimationSlotControlWork::IsEnableSlotIndex( u32 modelType, u32 animationIndex) const
{
  GFL_ASSERT( modelType < MODEL_TYPE_MAX);
  GFL_ASSERT( animationIndex < ANIMATION_INDEX_MAX);
  return m_isUseSlotIndex[modelType][animationIndex];
}

//*******************************************************************
//*******************************************************************
// ここから下はハンドルクラス
//*******************************************************************
//*******************************************************************

//===================================================================
// StartLoopAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StartLoopAnimation::Handle_StartLoopAnimation( f32 startFrame) :
  startFrame( startFrame)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StartLoopAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StartLoopAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief ループアニメーションの開始
 */
void StaticModel::Handle_StartLoopAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->ChangeAnimation_High( ANIMATION_INDEX_LOOP_0);
    return;
  }

  pNode->ChangeAnimation( ANIMATION_INDEX_LOOP_0);
  pNode->SetAnimationFrame( ANIMATION_INDEX_LOOP_0, startFrame);
}

//===================================================================
// StopLoopAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StopLoopAnimation::Handle_StopLoopAnimation()
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StopLoopAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StopLoopAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief ループアニメーションの停止
 */
void StaticModel::Handle_StopLoopAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->StopAnimation_High( ANIMATION_INDEX_LOOP_0);
    return;
  }

  u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, ANIMATION_INDEX_LOOP_0);
  if( slotIndex == StaticModel::AnimationSlotControlWork::INVALID_SLOT_INDEX)
  {
    // 再生されていない
    return;
  }

  pNode->m_pModel[pNode->m_nowModelType]->ChangeAnimationByResourceNode( NULL, slotIndex);
  pNode->m_animationSlotControlWork.ClearSlotIndex( pNode->m_nowModelType, ANIMATION_INDEX_LOOP_0);
}

//===================================================================
// StartEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StartEventAnimation::Handle_StartEventAnimation( u32 index, f32 stepFrame, bool isLoop) :
  index( index),
  stepFrame( stepFrame),
  isLoop( isLoop)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StartEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StartEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションの開始
 */
void StaticModel::Handle_StartEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->ChangeAnimation_High( animIndex, isLoop, stepFrame);

    // 逆再生なら最終フレームにセットしておく
    if( stepFrame < 0.0f)
    {
      pNode->SetAnimationLastFrame_High( animIndex);
    }
    return;
  }

  pNode->PlayEvnetAnimationSE( index, false);
  pNode->ChangeAnimation( animIndex, isLoop, stepFrame);

  // 逆再生なら最終フレームにセットしておく
  if( stepFrame < 0.0f)
  {
    u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, animIndex);
    f32 frame = pNode->m_pModel[pNode->m_nowModelType]->GetAnimationEndFrame( slotIndex);
    pNode->SetAnimationFrame( animIndex, frame);
  }
}

//===================================================================
// StartEventAnimationEx
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StartEventAnimationEx::Handle_StartEventAnimationEx( u32 slotIndex, u32 index, f32 stepFrame, bool isLoop, u32 lerpTime, s32 startFrame ) :
  m_slotIndex( slotIndex),
  m_index( index),
  m_stepFrame( stepFrame),
  m_isLoop( isLoop),
  m_lerpTime( lerpTime),
  m_startFrame( startFrame )
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StartEventAnimationEx::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StartEventAnimationEx::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションの開始 (スロット指定版)
 */
void StaticModel::Handle_StartEventAnimationEx::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  pNode->PlayEvnetAnimationSE( m_index, false);
  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->ChangeAnimationEx_High( m_slotIndex, animIndex, m_isLoop, m_stepFrame, m_lerpTime);

    if( m_startFrame >= 0 ){
      f32 frame = m_startFrame;

      // 0が指定された時、逆再生なら最終フレームにセットしておく
      if( m_startFrame == 0 && m_stepFrame < 0.0f)
      {
        frame = pNode->m_pModel[MODEL_TYPE_HIGH]->GetAnimationEndFrame( m_slotIndex);
      }
      pNode->m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( frame, m_slotIndex);
    }
    return;
  }

  if( pNode->IsAnimationExist( animIndex) && pNode->m_pModel[pNode->m_nowModelType]->IsCreated())
  {
    pNode->m_animationSlotControlWork.SetSlotIndex( pNode->m_nowModelType, animIndex);

    if(m_lerpTime <= 0){
      pNode->m_pModel[pNode->m_nowModelType]->ChangeAnimationByResourceNode( pNode->m_pMotionResourceRootNode[pNode->m_nowModelType][animIndex], m_slotIndex);
    }else{
      pNode->m_pModel[pNode->m_nowModelType]->ChangeAnimationSmoothByResourceNode( pNode->m_pMotionResourceRootNode[pNode->m_nowModelType][animIndex], m_lerpTime, m_slotIndex);
    }
    pNode->m_pModel[pNode->m_nowModelType]->SetAnimationLoop( m_isLoop, m_slotIndex);
    pNode->m_pModel[pNode->m_nowModelType]->SetAnimationStepFrame( m_stepFrame, m_slotIndex);

    if( m_startFrame >= 0 ){
      f32 frame = m_startFrame;

      // 0が指定された時、逆再生なら最終フレームにセットしておく
      if( m_startFrame == 0 && m_stepFrame < 0.0f)
      {
        frame = pNode->m_pModel[pNode->m_nowModelType]->GetAnimationEndFrame( m_slotIndex);
      }
      pNode->m_pModel[pNode->m_nowModelType]->SetAnimationFrame( frame, m_slotIndex);
    }
  }

}

//===================================================================
// StopEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StopEventAnimation::Handle_StopEventAnimation( u32 index) :
  index( index)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StopEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StopEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションの停止
 */
void StaticModel::Handle_StopEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->StopAnimation_High( animIndex);
    return;
  }

  u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, animIndex);

  if( slotIndex == StaticModel::AnimationSlotControlWork::INVALID_SLOT_INDEX)
  {
    // 再生されていない
    return;
  }

  pNode->m_pModel[pNode->m_nowModelType]->ChangeAnimationByResourceNode( NULL, slotIndex);
  pNode->m_animationSlotControlWork.ClearSlotIndex( pNode->m_nowModelType, animIndex);
}

//===================================================================
// StopEventAnimationEx
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_StopEventAnimationEx::Handle_StopEventAnimationEx( u32 slotIndex, u32 index) :
  m_index( index),
  m_slotIndex( slotIndex)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StopEventAnimationEx::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StopEventAnimationEx::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションの停止 (スロット指定版)
 */
void StaticModel::Handle_StopEventAnimationEx::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->StopAnimationEx_High( m_slotIndex, animIndex);
    return;
  }

  if( pNode->m_animationSlotControlWork.IsEnableSlotIndex( pNode->m_nowModelType, animIndex))
  {
    // 再生されていない
    return;
  }

  pNode->m_pModel[pNode->m_nowModelType]->ChangeAnimationByResourceNode( NULL, m_slotIndex);
  pNode->m_animationSlotControlWork.ClearSlotIndex( pNode->m_nowModelType, animIndex);
}

//===================================================================
// IsEndEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_IsEndEventAnimation::Handle_IsEndEventAnimation( u32 index) :
  index( index)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_IsEndEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_IsEndEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  const StaticModel* pNode = node->SafeCast<StaticModel>();

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + index);

  if( pNode == NULL )
  {
    return true;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    return true;
  }

  if( pNode->IsAnimationExist( animIndex ) && pNode->m_animationSlotControlWork.IsEnableSlotIndex( pNode->m_nowModelType, animIndex) )
  {
    u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, animIndex);
    if( pNode->m_pModel[pNode->m_nowModelType]->IsAnimationLastFrame( slotIndex))
    {
      return true;
    }
    return false;
  }

  // アニメーションが無い場合、再生されていることもない
  return true;
}

//===================================================================
// IsEndEventAnimationEx
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_IsEndEventAnimationEx::Handle_IsEndEventAnimationEx( u32 slotIndex, u32 index) :
  m_index( index),
  m_slotIndex( slotIndex)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_IsEndEventAnimationEx::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_IsEndEventAnimationEx::IsEnd( const IField3DObjectNode * node ) const
{
  const StaticModel* pNode = node->SafeCast<StaticModel>();

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index );

  if( pNode == NULL )
  {
    return true;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    return true;
  }

  if( pNode->IsAnimationExist( animIndex ) && pNode->m_animationSlotControlWork.IsEnableSlotIndex( pNode->m_nowModelType, animIndex) )
  {
    if( pNode->m_pModel[pNode->m_nowModelType]->IsAnimationLastFrame( m_slotIndex ) )
    {
      return true;
    }
    return false;
  }

  // アニメーションが無い場合、再生されていることもない
  return true;
}

//===================================================================
// SetFrameEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_SetFrameEventAnimation::Handle_SetFrameEventAnimation( u32 index, f32 frame) :
  index( index),
  frame( frame)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_SetFrameEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_SetFrameEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションにフレームセット
 */
void StaticModel::Handle_SetFrameEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->SetAnimationFrame_High( animIndex, frame);
    return;
  }

  pNode->SetAnimationFrame( animIndex, frame);
}

//===================================================================
// SetLastFrameEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_SetLastFrameEventAnimation::Handle_SetLastFrameEventAnimation( u32 index) :
  m_index( index)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_SetLastFrameEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_SetLastFrameEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションに終了フレームセット
 */
void StaticModel::Handle_SetLastFrameEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->SetAnimationLastFrame_High( animIndex);
    return;
  }

  u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, animIndex);
  f32 frame = pNode->m_pModel[pNode->m_nowModelType]->GetAnimationEndFrame( slotIndex);
  pNode->SetAnimationFrame( animIndex, frame);
}

//===================================================================
// PauseEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_PauseEventAnimation::Handle_PauseEventAnimation( u32 index) :
  m_index( index)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_PauseEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_PauseEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーション一時停止
 */
void StaticModel::Handle_PauseEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE )
  {
    pNode->SetAnimationStepFrame_High( animIndex, 0.0f);
    return;
  }

  pNode->SetAnimationStepFrame( animIndex, 0.0f);
}

//===================================================================
// RestartEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_RestartEventAnimation::Handle_RestartEventAnimation( u32 index, f32 stepFrame) :
  m_index( index),
  m_stepFrame( stepFrame)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_RestartEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_RestartEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーション再開
 */
void StaticModel::Handle_RestartEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE )
  {
    pNode->SetAnimationStepFrame_High( animIndex, m_stepFrame);
    return;
  }

  pNode->SetAnimationStepFrame( animIndex, m_stepFrame);
}

//===================================================================
// EndEventAnimation
//===================================================================
/**
 *  @brief コンストラクタ
 */
StaticModel::Handle_EndEventAnimation::Handle_EndEventAnimation( u32 index) :
  m_index( index)
{}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_EndEventAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_EndEventAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief イベントアニメーションを終了状態にする
 */
void StaticModel::Handle_EndEventAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  AnimationIndex animIndex = static_cast<AnimationIndex>( ANIMATION_INDEX_EVENT_0 + m_index);

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->ChangeAnimation_High( animIndex, false, 0.0f);
    pNode->SetAnimationLastFrame_High( animIndex);
    return;
  }

  pNode->ChangeAnimation( animIndex, false, 0.0f);

  // 最終フレームセット
  u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, animIndex);
  f32 frame = pNode->m_pModel[pNode->m_nowModelType]->GetAnimationEndFrame( slotIndex);
  pNode->SetAnimationFrame( animIndex, frame);
}

//===================================================================
// StartDoorOpenAnimation
//===================================================================
/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StartDoorOpenAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StartDoorOpenAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief ドアが開くアニメーション開始
 */
void StaticModel::Handle_StartDoorOpenAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    pNode->ChangeAnimation_High( ANIMATION_INDEX_EVENT_0, false);
    return;
  }

  pNode->PlayEvnetAnimationSE( ANIMATION_INDEX_EVENT_0, false);
  pNode->ChangeAnimation( ANIMATION_INDEX_EVENT_0, false);
}

//===================================================================
// StartDoorCloseAnimation
//===================================================================

/**
 *  @brief  コンストラクタ
 */
StaticModel::Handle_StartDoorCloseAnimation::Handle_StartDoorCloseAnimation(f32 startFrame) : 
  m_StartFrame(startFrame)
{
}

/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_StartDoorCloseAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_StartDoorCloseAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  return true;
}

/**
 * @brief ドアが閉まるアニメーション開始
 */
void StaticModel::Handle_StartDoorCloseAnimation::Handle( IField3DObjectNode * node )
{
  StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return;
  }

  bool isSuccess;

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    isSuccess = pNode->ChangeAnimation_High( ANIMATION_INDEX_EVENT_0, false, -1.0f);
    if( isSuccess)
    {
      u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( MODEL_TYPE_HIGH, ANIMATION_INDEX_EVENT_0);
      f32 setFrame = pNode->m_pModel[MODEL_TYPE_HIGH]->GetAnimationEndFrame( slotIndex) - m_StartFrame;
      if( setFrame >= 0.0f ) // frame数が足りていない
      {
        pNode->m_pModel[MODEL_TYPE_HIGH]->SetAnimationFrame( setFrame, slotIndex);
      }
      else
      {
        GFL_ASSERT_MSG( 0, "DoorCloseアニメーションの総フレーム数が不足しています。%d\n", -setFrame );
      }
    }
    return;
  }

  pNode->PlayEvnetAnimationSE( ANIMATION_INDEX_EVENT_0, true);

  isSuccess = pNode->ChangeAnimation( ANIMATION_INDEX_EVENT_0, false, -1.0f); // 閉めるときは逆再生
  if( isSuccess && m_StartFrame >= 0.0f )
  {
    u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, ANIMATION_INDEX_EVENT_0);
    f32 setFrame = pNode->m_pModel[pNode->m_nowModelType]->GetAnimationEndFrame( slotIndex) - m_StartFrame;
    if( setFrame >= 0.0f ) // frame数が足りていない
    {
      pNode->m_pModel[pNode->m_nowModelType]->SetAnimationFrame( setFrame, slotIndex);
    }
    else
    {
      GFL_ASSERT_MSG( 0, "DoorCloseアニメーションの総フレーム数が不足しています。%d\n", -setFrame );
    }
  }
}

//===================================================================
// IsEndDoorAnimation
//===================================================================
/**
 *  @brief 操作可能なオブジェクトかチェック
 */
bool StaticModel::Handle_IsEndDoorAnimation::IsSupport( const IField3DObjectNode & node ) const
{
  return ( node.SafeCast<StaticModel>() != NULL );
}

/**
 * @brief 終了待ち
 */
bool StaticModel::Handle_IsEndDoorAnimation::IsEnd( const IField3DObjectNode * node ) const
{
  const StaticModel* pNode = node->SafeCast<StaticModel>();

  if( pNode == NULL )
  {
    return true;
  }

  if( pNode->m_nowModelType == MODEL_TYPE_NONE)
  {
    return true;
  }

  if( pNode->IsAnimationExist( ANIMATION_INDEX_EVENT_0 ) && pNode->m_animationSlotControlWork.IsEnableSlotIndex( pNode->m_nowModelType, ANIMATION_INDEX_EVENT_0) )
  {
    u32 slotIndex = pNode->m_animationSlotControlWork.GetSlotIndex( pNode->m_nowModelType, ANIMATION_INDEX_EVENT_0);
    if( pNode->m_pModel[pNode->m_nowModelType]->IsAnimationLastFrame( slotIndex))
    {
      return true;
    }
    return false;
  }

  // アニメーションが無い場合、再生されていることもない
  return true;
}

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )
