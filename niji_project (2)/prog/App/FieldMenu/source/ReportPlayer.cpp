//======================================================================
/**
 * @file ReportPlayer.cpp
 * @date 2015/12/07 18:36:08
 * @author takahashi_tomoya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>

#include  "ReportPlayer.h"

#include "System/include/HeapDefine.h"

#include "GameSys/include/GameManager.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectReport.h"

#include "niji_conv_header/field/chara/chara_model_id.h"
#include "niji_conv_header/field/chara/chara_model_dynamic_motion.h"
#include "niji_conv_header/field/chara/pc0001_00_pgmot_ev000.h"
#include "niji_conv_header/field/chara/pc0002_00_pgmot_ev000.h"
#include "niji_conv_header/field/chara/ob0028_00_pc0001_00_pgmot_ev000.h"
#include "niji_conv_header/field/chara/ob0028_00_pc0002_00_pgmot_ev000.h"


GFL_NAMESPACE_BEGIN(Field)


const u32 FieldReportPlayer::LOAD_DYNAMIC_ANIMATION[2] = 
{
  PC0001_00_PGMOT_EV000,
  PC0002_00_PGMOT_EV000,
};

const u32 FieldReportPlayer::DYNAMIC_START_ANIMATION_ID[2] = 
{
  P1_BASE_FI043_REPORT01_START,
  P2_BASE_FI043_REPORT01_START,
};

const u32 FieldReportPlayer::DYNAMIC_UPDATE_ANIMATION_ID[2] = 
{
  P1_BASE_FI044_REPORT01_LOOP,
  P2_BASE_FI044_REPORT01_LOOP,
};

const u32 FieldReportPlayer::DYNAMIC_END_ANIMATION_ID[2] = 
{
  P1_BASE_FI045_REPORT01_END,
  P2_BASE_FI045_REPORT01_END,
};

const u32 FieldReportPlayer::LOAD_OB0028_CHARA_ID[2] = 
{
  CHARA_MODEL_ID_OB0028_00_PC0001_00,
  CHARA_MODEL_ID_OB0028_00_PC0002_00,
};

const u32 FieldReportPlayer::LOAD_OB0028_DYNAMIC_ANIMATION[2] =
{
  OB0028_00_PC0001_00_PGMOT_EV000,
  OB0028_00_PC0002_00_PGMOT_EV000,
};

const u32 FieldReportPlayer::DYNAMIC_OB0028_START_ANIMATION_ID[2] = 
{
  OB0028_00_PC0001_00_FI043_REPORT01_START,
  OB0028_00_PC0002_00_FI043_REPORT01_START,
};

const u32 FieldReportPlayer::DYNAMIC_OB0028_UPDATE_ANIMATION_ID[2] = 
{
  OB0028_00_PC0001_00_FI044_REPORT01_LOOP,
  OB0028_00_PC0002_00_FI044_REPORT01_LOOP,
};

const u32 FieldReportPlayer::DYNAMIC_OB0028_END_ANIMATION_ID[2] = 
{
  OB0028_00_PC0001_00_FI045_REPORT01_END,
  OB0028_00_PC0002_00_FI045_REPORT01_END,
};


FieldReportPlayer::FieldReportPlayer() : 
   m_State(ST_NONE)
  ,m_Seq(0)
  ,m_pPlayer(NULL)
  ,m_pManager(NULL)
  ,m_pEventWork(NULL)
{
}

FieldReportPlayer::~FieldReportPlayer()
{
  this->Terminate();
}

/**
 *  @brief 初期化
 */
void FieldReportPlayer::Initialize( gfl2::heap::HeapBase* pHeap, MoveModel::FieldMoveModelManager* pManager, const GameSys::GameData& gameData, Effect::EffectManager* pEffectManager )
{
  if( m_State == ST_NONE )
  {
    u32 sex = gameData.GetPlayerStatusConst()->GetSex();

    m_Sex = sex;

    m_pEffectManager = pEffectManager;
    m_pHeap          = pHeap;

    m_pManager = pManager;
    m_pPlayer = reinterpret_cast<MoveModel::FieldMoveModelPlayer*>(pManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER ));

    // プレイヤーの非常駐モーション
    m_pManager->LoadDressUpDynamicAnimationAsync( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),*m_pPlayer->GetHeaderResource().pDressUpParam, LOAD_DYNAMIC_ANIMATION[m_Sex] );

    // ペンのモデルリソース
    m_pManager->LoadDynamicAsync( MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_EVENT, LOAD_OB0028_CHARA_ID[m_Sex] );
    m_pManager->LoadDynamicAnimationAsync( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),LOAD_OB0028_DYNAMIC_ANIMATION[m_Sex] );

    // エフェクトの読み込み
    m_pEffectManager->LoadDynamicEffectResource( Field::Effect::EFFECT_TYPE_REPORT, m_pHeap, false );

    m_pEventWork = gameData.GetEventWork();

    m_State = ST_LOAD;
  }
  else
  {
    GFL_ASSERT( m_State == ST_NONE );
  }
}

/**
 *  @brief 破棄
 */
void FieldReportPlayer::Terminate()
{
  GFL_ASSERT( m_State != ST_LOAD ); // 読み込み中は禁止

  if(m_State != ST_NONE)
  {
    //エフェクトの非常駐リソース破棄
    m_pEffectManager->UnLoadDynamicEffectResource( Field::Effect::EFFECT_TYPE_REPORT, m_pHeap );

    // 主人公のモーション
    if( isPlayerAction() ) // ライド中は再生しない。 @fix NMCat[1376] レポートに記録するとライドポケモンと主人公の動きがずれる
    {
      m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( m_pPlayer->GetDefaultIdleAnimationId() );
    }
    m_pManager->UnloadDressUpDynamicAnimation( *m_pPlayer->GetHeaderResource().pDressUpParam, LOAD_DYNAMIC_ANIMATION[m_Sex] );

    // ペンのリソース
    m_pManager->UnloadDynamicAnimation( LOAD_OB0028_DYNAMIC_ANIMATION[m_Sex] );
    m_pManager->Unload( LOAD_OB0028_CHARA_ID[m_Sex] );

    m_pManager = NULL;
    m_pPlayer = NULL;

    m_State = ST_NONE;
  }
}

b32 FieldReportPlayer::Update(b32 is_end)
{
  enum
  {
    SEQ_LOAD,
    SEQ_START,
    SEQ_START_WAIT,
    SEQ_LOOP_ANIME,
    SEQ_END_ANIME_WAIT,
    SEQ_SKIP,
    SEQ_END,
  };

  switch( m_Seq )
  {
  case SEQ_LOAD:
    {
      if( !load() )
      {
        break;
      }
      m_Seq = SEQ_START;
    }
    //break through

  case SEQ_START:

    start( DYNAMIC_START_ANIMATION_ID[m_Sex], DYNAMIC_OB0028_START_ANIMATION_ID[m_Sex], Effect::EffectReport::ANIME_START );
    m_Seq = SEQ_START_WAIT;
    break;

  case SEQ_START_WAIT:
    if( !update() )
    {
      break;
    }
    start( DYNAMIC_UPDATE_ANIMATION_ID[m_Sex], DYNAMIC_OB0028_UPDATE_ANIMATION_ID[m_Sex], Effect::EffectReport::ANIME_LOOP ); // ループアニメを常に再生
    m_Seq = SEQ_LOOP_ANIME;
    // break through

  case SEQ_LOOP_ANIME:
    if( !is_end )
    {
      break;
    }
    start( DYNAMIC_END_ANIMATION_ID[m_Sex], DYNAMIC_OB0028_END_ANIMATION_ID[m_Sex], Effect::EffectReport::ANIME_END );
    m_Seq = SEQ_END_ANIME_WAIT;
    break;

  case SEQ_END_ANIME_WAIT:
    if( !update() )
    {
      break;
    }

    // 終了
    if(m_pEffect)
    {
      reinterpret_cast<Effect::EffectReport*>(m_pEffect)->End();
    }

    // 非表示にする。
    if( isPlayerAction() ) // ライド中はなし
    {
      m_pPlayer->SetSubobject( MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_STATIC, NULL, NULL );
    }

    // エフェクトを消す
    if(m_pEffect)
    {
      m_pEffectManager->DeleteEffect(m_pEffect);
      m_pEffect = NULL;
    }

    m_Seq = SEQ_SKIP;
    break;

  case SEQ_SKIP:
    m_Seq = SEQ_END;
    break;

  case SEQ_END:
    return true;
  }

  return false;
}



b32 FieldReportPlayer::load()
{
  b32 ret = false;

  if( m_State == ST_LOAD )
  {
    b32 result = true;

    // プレイヤーの非常駐モーション
    if( !m_pManager->IsDressUpDynamicAnimationLoaded( *m_pPlayer->GetHeaderResource().pDressUpParam, LOAD_DYNAMIC_ANIMATION[m_Sex] ) )
    {
      result = false;
    }

    // OB0028
    if( !m_pManager->IsLoaded( LOAD_OB0028_CHARA_ID[m_Sex] ) )
    {
      result = false;
    }

    // OB0028の非常駐モーション
    if( !m_pManager->IsDynamicAnimationLoaded( LOAD_OB0028_DYNAMIC_ANIMATION[m_Sex] ) )
    {
      result = false;
    }

    // 非常駐エフェクト
    if( !m_pEffectManager->IsLoadedEffectResource( Field::Effect::EFFECT_TYPE_REPORT ) )
    {
      result = false;
    }

    if( result )
    {
      // 主人公の非常駐モーション
      m_pManager->SetupDressUpDynamicAnimation( System::GflUse::GetEventDeviceAllocator(),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),*m_pPlayer->GetHeaderResource().pDressUpParam, LOAD_DYNAMIC_ANIMATION[m_Sex] );


      // OB0028生成
      m_pManager->SetupModel( LOAD_OB0028_CHARA_ID[m_Sex] );
      m_pManager->SetupDynamicAnimation( System::GflUse::GetEventDeviceAllocator(),gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ),LOAD_OB0028_DYNAMIC_ANIMATION[m_Sex] );

      // ライド中は生成しない
      if( isPlayerAction() )
      {
        Field::MoveModel::FieldSubobjectHeader header;
        header.characterId = LOAD_OB0028_CHARA_ID[m_Sex];
        m_pManager->SetSubobjectToMoveModel( m_pPlayer , MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_STATIC, &header, true );
      }

      // エフェクトの生成
      m_pEffect  = m_pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_REPORT, m_pPlayer, false );

      m_State = ST_INITIALIZED;
      ret = true; // 読み込み完了
    }
  }
  else if( m_State == ST_NONE )
  {
    GFL_ASSERT( m_State != ST_NONE );
    ret = false;
  }
  else
  {
    ret = true;
  }

  return ret;
}


/** 
 *  @brief アニメーション開始
 */
void FieldReportPlayer::start(u32 animationID, u32 obAnimationID, u32 layoutAnimationID )
{
  if( (m_State == ST_INITIALIZED) || (m_State == ST_ANIMATION) )
  {
    // アニメーション開始
    if( isPlayerAction() ) // ライド中は再生しない
    {
      m_pPlayer->GetCharaDrawInstance()->ChangeAnimation( animationID );
      // @fix GFNMCat[1207]Tスタンスを打ち消すためにStep0で更新を行う
      Field::MoveModel::FieldSubobject *pSubobject = m_pPlayer->GetSubobject( MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_STATIC );
      pSubobject->GetCharaDrawInstance()->ChangeAnimation( obAnimationID );
      f32 stepFrame = pSubobject->GetCharaDrawInstance()->GetAnimationStepFrame();
      pSubobject->GetCharaDrawInstance()->SetAnimationStepFrame(0.0f);
      pSubobject->GetCharaDrawInstance()->UpdateAnimation();
      pSubobject->GetCharaDrawInstance()->SetAnimationStepFrame(stepFrame);
    }

    if(m_pEffect)
    {
      reinterpret_cast<Effect::EffectReport*>(m_pEffect)->Start( static_cast<Effect::EffectReport::AnimeIndex>(layoutAnimationID) );
    }

    m_State = ST_ANIMATION;
  }
  else
  {
    GFL_ASSERT( (m_State == ST_INITIALIZED) || (m_State == ST_ANIMATION) );
  }
}

/**
 *  @brief 更新
 *  @retval true 終了
 *  @retval false 途中
 */
b32 FieldReportPlayer::update()
{
  if(m_State == ST_ANIMATION)
  {
    b32 result = true;
    
    if( isPlayerAction() ) // ライド中は再生しない。
    {
      result = m_pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame();

      if( !m_pPlayer->GetSubobject( MoveModel::FIELD_MOVE_MODEL_SUB_OBJECT_STATIC )->GetCharaDrawInstance()->IsAnimationLastFrame() )
      {
        result = false;
      }
    }

    if(m_pEffect)
    {
      if( !reinterpret_cast<Effect::EffectReport*>(m_pEffect)->IsAnimationLastFrame() )
      {
        result = false;
      }
    }

    if( result )
    {
      m_State = ST_INITIALIZED;
    }
    return result;
  }
  else
  {
    return true;
  }
}


/**
 *  @brief プレイヤーは動作させるか？
 *  @fix 大森さん要望 : 冒頭バッグが無いときはレポートの演出をカット
 */
b32 FieldReportPlayer::isPlayerAction() const
{
  // ライド中は動作させない。
  if( m_pPlayer->GetOnMoveModel() != NULL )
  {
    return false;
  }

  // ゲーム開始時でバッグを持っていなければ出さない
  if( m_pEventWork->CheckEventFlag( SYS_FLAG_DRESSUP_BEFORE ) )
  {
    return false;
  }


  return true;
}


#if PM_DEBUG

void EventFieldReportPlayerTest::StartEvent( GameSys::GameManager* p_gameman )
{
  GameSys::GameEventManager * p_geventman = p_gameman->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldReportPlayerTest>( p_geventman );
}


//--------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   heap         workをとるためのメモリクラス
 */
//--------------------------------------------------------------
EventFieldReportPlayerTest::EventFieldReportPlayerTest( gfl2::heap::HeapBase* heap ) : 
   GameSys::GameEvent(heap)
  ,m_pPlayerControl(NULL)
{
}

EventFieldReportPlayerTest::~EventFieldReportPlayerTest()
{
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventFieldReportPlayerTest::InitFunc(GameSys::GameManager* gmgr)
{
  gfl2::heap::HeapBase * pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_pPlayerControl = GFL_NEW(pHeap) FieldReportPlayer();
  m_pPlayerControl->Initialize( pHeap, gmgr->GetGameData()->GetFieldCharaModelManager(), *gmgr->GetGameData(), gmgr->GetFieldmap()->GetEffectManager() );

  m_Count = 0;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFieldReportPlayerTest::MainFunc(GameSys::GameManager* gmgr)
{

  b32 is_end = m_Count > 90;
  m_Count ++;

  if( m_pPlayerControl->Update(is_end) )
  {
    m_pPlayerControl->Terminate();
    GFL_SAFE_DELETE(m_pPlayerControl);
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventFieldReportPlayerTest::EndFunc(GameSys::GameManager* gmgr)
{

}



#endif // PM_DEBUG



GFL_NAMESPACE_END(Field)
