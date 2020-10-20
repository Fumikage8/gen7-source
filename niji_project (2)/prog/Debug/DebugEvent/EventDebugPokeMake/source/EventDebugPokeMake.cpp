//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventDebugPokeMake.cpp
 *  @brief  デバッグポケモン生成イベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if PM_DEBUG

#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>
#include "System/include/HeapDefine.h"

// pml
#include <pml/include/pmlib.h>

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ZukanSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"


#include "Field/FieldStatic/include/Event/EventProcessCall.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "Savedata/include/BoxPokemonSave.h"

#include "Debug/DebugKeyboard/include/DebugKeyboard.h"
#include "Debug/DebugPokeMake/include/DebugPokeMake.h"
#include "Debug/DebugBattle/include/DebugBattleProc.h"
#include "Debug/DebugEvent/EventDebugPokeMake/include/EventDebugPokeMake.h"

#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Debug )
  
//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------
u32 EventDebugPokeMake::DebugPokeEditIndex = 0;

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグポケモン生成 コール
 *
 *  @param  p_gman        ゲームマネージャ
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
EventDebugPokeMake* EventDebugPokeMake::CallDebugPokeAdd( GameSys::GameManager* p_gman, bool field_control )
{
  EventDebugPokeMake* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugPokeMake>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達
  p_event->SetParamAdd();
  p_event->SetFieldControlFlag(field_control);

  return p_event;
}
//----------------------------------------------------------------------------
/**
 *  @brief  デバッグポケモンエディット コール
 *
 *  @param  p_gman        ゲームマネージャ
 *  @param  add_poke_id   AddPokeID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
EventDebugPokeMake* EventDebugPokeMake::CallDebugPokeEdit( GameSys::GameManager* p_gman, u32 party_index, bool field_control  )
{
  EventDebugPokeMake* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugPokeMake>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達
  p_event->SetParamEdit(party_index);
  p_event->SetFieldControlFlag(field_control);
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  デバッグポケモンエディット(育て屋) コール
 *
 *  @param  p_gman        ゲームマネージャ
 *  @param  add_poke_id   AddPokeID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
EventDebugPokeMake* EventDebugPokeMake::CallDebugSodateyaPokeEdit( GameSys::GameManager* p_gman, u32 party_index, bool field_control ,debug::PokeMake::APP_PARAM* p_app_param, ReflectCallback p_reflect_callback )
{
  EventDebugPokeMake* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventDebugPokeMake>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達
  p_event->SetParamSodateya();
  p_event->SetFieldControlFlag(field_control);

  p_event->m_pReflectCallback = p_reflect_callback;
  p_event->m_pParam = p_app_param;

  return p_event;
}

//-----------------------------------------------------------------------------
/**
 *    イベントの実装
 */
//-----------------------------------------------------------------------------
EventDebugPokeMake::EventDebugPokeMake( gfl2::heap::HeapBase* heap ) :
  GameSys::GameEvent( heap )
  , m_Seq(0)
  , m_PartyIndex(0)
  , m_Mode(MODE_ADD)
  , m_pHeap(NULL)
  , m_pParam(NULL)
  , m_pMyParty(NULL)
  , m_pPokeParam(NULL)
  , m_CreateParam(false)
{
}



//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventDebugPokeMake::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
} 

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::InitFunc(GameSys::GameManager* p_gman)
{
  this->Initialize( p_gman );
} 

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventDebugPokeMake::MainFunc(GameSys::GameManager* p_gman)
{
  enum{
    SEQ_FIELD_CLOSE,
    SEQ_APP_CALL,
    SEQ_APP_WAIT,
    SEQ_FIELD_OPEN,
    SEQ_PARAM_SET,
    SEQ_END,
  };
  GameSys::GameProcManager* proc_manager = p_gman->GetProcManager();


  switch( m_Seq ){
  case SEQ_FIELD_CLOSE:
    if( m_FieldControlFlag ){
      // クローズイベント
      EventFieldmapClose* p_event;
      EventFieldmapClose::Desc desc;
      
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
                  p_gman->GetGameEventManager( ) );
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackDefault();  //フェードはデフォルト
    }
    m_Seq++;
    break;

  case SEQ_APP_CALL:
    CallDebugPokeMake(p_gman);
    m_Seq++;
    break;
  case SEQ_APP_WAIT:
    if( proc_manager->IsRegisterProcess(m_pProc) == false ){
      m_Seq++;
    }
    break;

  case SEQ_FIELD_OPEN:
    if( m_FieldControlFlag )
    {
      EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                    p_gman->GetGameEventManager( ) );
      p_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, *p_gman->GetGameData()->GetStartLocation() );
      p_event->SetFadeCallBackDefault();
    }
    m_Seq++;
    break;

  case SEQ_PARAM_SET:
    ReflectParam(p_gman);
    m_Seq++;
    break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::EndFunc(GameSys::GameManager* p_gman)
{
  if( m_CreateParam )
  {
    GFL_SAFE_DELETE(m_pPokeParam);
    GFL_SAFE_DELETE(m_pParam);
    m_CreateParam = false;
  }
}

//--------------------------------------------------------------
/**
 * @brief データ初期化 
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::Initialize(GameSys::GameManager* p_gman)
{
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  
  m_pGameManager = p_gman;
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  if( m_pParam == NULL )
  {
    m_pParam = GFL_NEW(m_pHeap) debug::PokeMake::APP_PARAM;
    m_CreateParam = true;
  }

  DEFAULT_POWERUP_DESC_Clear( &m_DefaultPowerUpDesc );

  m_pMyParty = p_gdata->GetPlayerParty();

  switch( m_Mode ){
  case MODE_ADD:
    m_pPokeParam = GFL_NEW(m_pHeap) pml::pokepara::PokemonParam( m_pHeap, MONSNO_HUSIGIDANE, 5, p_gdata->GetPlayerStatus()->GetID() );
    break;
  case MODE_EDIT:
    m_pPokeParam = GFL_NEW(m_pHeap) pml::pokepara::PokemonParam( m_pHeap, MONSNO_HUSIGIDANE, 5, p_gdata->GetPlayerStatus()->GetID() );
    m_pPokeParam->CopyFrom( *m_pMyParty->GetMemberPointer(m_PartyIndex) );
    break;
  case MODE_SODATEYA:
    m_pPokeParam = m_pParam->poke;
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief 呼び出しパラメータセットアップ  
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::SetParamAdd()
{
  m_Mode = EventDebugPokeMake::MODE_ADD;
  m_PartyIndex = 0;
}

//--------------------------------------------------------------
/**
 * @brief 呼び出しパラメータセットアップ  
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::SetParamEdit( u32 index )
{
  m_Mode = EventDebugPokeMake::MODE_EDIT;
  m_PartyIndex = index;
}

//--------------------------------------------------------------
/**
 * @brief 呼び出しパラメータセットアップ  
 */
//--------------------------------------------------------------
void EventDebugPokeMake::SetParamSodateya()
{
  m_Mode = EventDebugPokeMake::MODE_SODATEYA;
}

//--------------------------------------------------------------
/**
 * @brief 編集パラメータの反映   
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::ReflectParam(GameSys::GameManager* p_gman)
{

  switch( m_Mode ){
  case MODE_ADD:
    {
      if( !m_pParam->result ) return;
      Savedata::BoxPokemon* pBoxPokemon = m_pGameManager->GetGameData()->GetBoxPokemon();
      bool ret = m_pMyParty->AddMember( *m_pPokeParam );
      if( !ret ){ //手持ちがいっぱい
        if( pBoxPokemon->GetSpaceCountAll( m_pHeap ) > 0 ){
          pBoxPokemon->PutPokemonEmptyTrayAll( m_pPokeParam, 0, m_pHeap );
        }
      }
    }
    break;
  case MODE_EDIT:
    if( !m_pParam->result ) return;
    m_pMyParty->GetMemberPointer(m_PartyIndex)->CopyFrom( *m_pPokeParam );
    break;
  case MODE_SODATEYA:
    if( m_pReflectCallback != NULL )
    {
      m_pReflectCallback(m_pParam,p_gman);
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief  DebugPokeMakeアプリコール
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventDebugPokeMake::CallDebugPokeMake(GameSys::GameManager* p_gman)
{
  m_pParam->poke = m_pPokeParam;
  m_pParam->defaultPowerUpDesc = &m_DefaultPowerUpDesc;
  m_pParam->result = false;

  m_pProc = GameSys::CallDebugPokeMakeProc( m_pParam );
}


GFL_NAMESPACE_END( Debug )
GFL_NAMESPACE_END( Field )

#endif //PM_DEBUG
