//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EventAddPokemon.cpp
 *  @brief  ポケモン追加イベント
 *  @author miyuki iwasawa 
 *  @date   2015.09.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// pml
#include <pml/include/pmlib.h>
#include "PokeTool/include/PokeToolPokePlace.h"

#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ZukanSave.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"


#include "Field/FieldStatic/include/Event/EventAddPokemon.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"

#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

#include "FieldScript/include/EventScriptCall.h" // for CallScript
#include "FieldScript/include/FieldScriptSystem.h"

#include "App/GameOver/include/GameOverProc.h"
#include "App/AppEvent/include/GameOver/GameOverEvent.h"

#include "PokeTool/include/PokeToolPokeGet.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"

#include "niji_conv_header/field/script/PokeAddTableDef.h"
#include "niji_reference_files/poketool/PokeToolRibbonDefine.h"
#include "niji_conv_header/message/msg_place_name_out.h"
#include "niji_conv_header/message/msg_namelist.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/script_event_data.gaix>
#include <arc_index/message.gaix>

#include "Savedata/include/MiscSave.h"

namespace Field{
  
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

//----------------------------------------------------------------------------
/**
 *  @brief  ポケモン追加シーケンス　コール
 *
 *  @param  p_gman        ゲームマネージャ
 *  @param  add_poke_id   AddPokeID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
EventAddPokemon* EventAddPokemon::CallAddPokemon( GameSys::GameManager* p_gman, u32 addPokeID, AddPokemonMode addMode )
{
  EventAddPokemon* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventAddPokemon>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達
  p_event->SetParam( addPokeID, addMode );

  return p_event;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン追加シーケンスコール PokemonParam指定版
 *
 *	@param	p_gman    ゲームマネージャ
 *	@param  addPokeID  AddPokeID
 *
 *	@return 生成したイベント
 *	
 *	PokemonParamのコピーを作って保持するので、この関数を呼び出した後はPokemonParamは破棄しても構わない
 */
//-----------------------------------------------------------------------------
EventAddPokemon* EventAddPokemon::CallAddPokemonFromPP( GameSys::GameManager* p_gman, const pml::pokepara::PokemonParam& p_pp, AddPokemonMode addMode )
{
  EventAddPokemon* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventAddPokemon>( p_gman->GetGameEventManager() );

  // イベントにパラメータを通達
  p_event->SetParamFromPP( p_pp, addMode );

  return p_event;
}


//-----------------------------------------------------------------------------
/**
 *    イベントの実装
 */
//-----------------------------------------------------------------------------
// コンストラクタデストラクタ
EventAddPokemon::EventAddPokemon( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent( heap )
  , m_Seq(0)
  , m_InitSeq(0)
  , m_AddPokeID(0)
  , m_AddPokeMode(ADD_POKEMON_MODE_DEFAULT)
  , m_FadeDisp(gfl2::Fade::DISP_DOUBLE)
  , m_FadeType(gfl2::Fade::FADE_TYPE_ALPHA)
  , m_FadeStartCol(0.0f,0.0f,0.0f,0.0f)
  , m_FadeEndCol(0.0f,0.0f,0.0f,255.0f)
  , m_FadeSync(gfl2::Fade::FADE_DEFAULT_SYNC)
  , m_pHeap(NULL)
  , m_pBuffer(NULL)
  , m_BufferSize(0)
  , m_pPokeParam(NULL)

{
}

//--------------------------------------------------------------
/**
 * @brief   フェードパラメータセット
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventAddPokemon::SetParam( u32 addPokeID, AddPokemonMode addMode )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_paramType = PARAM_TYPE_ARC;
  m_AddPokeID = addPokeID;
  m_AddPokeMode = addMode;

  m_Seq = SEQ_ARC_INIT;
} 


//--------------------------------------------------------------
/**
 * @brief   PokemonParamセット
 */
//--------------------------------------------------------------
void EventAddPokemon::SetParamFromPP( const pml::pokepara::PokemonParam& p_pp, AddPokemonMode addMode )
{
  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  m_paramType = PARAM_TYPE_PP;
  m_AddPokeID = 0;
  m_AddPokeMode = addMode;
  m_Seq = SEQ_APP_CALL_CHECK;

  //コピーを生成
  m_pPokeParam = GFL_NEW(heap) pml::pokepara::PokemonParam(heap);
  m_pPokeParam->CopyFrom(p_pp);
} 


//--------------------------------------------------------------
/**
 * @brief   フェードパラメータセット
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventAddPokemon::SetFadeParam( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4& start_col, const gfl2::math::Vector4& end_col, u32 sync /* = gfl2::Fade::FADE_DEFAULT_SYNC*/, bool fade_in /*=true*/)
{
  m_FadeDisp = disp;
  m_FadeType = type;
  m_FadeStartCol.Set( start_col );
  m_FadeEndCol.Set( end_col );
  m_FadeSync = sync;
  m_FadeInEnable = fade_in;
} 


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventAddPokemon::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
} 

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventAddPokemon::InitFunc(GameSys::GameManager* p_gman)
{
  m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
} 

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventAddPokemon::MainFunc(GameSys::GameManager* p_gman)
{

  switch( m_Seq ){
  case SEQ_ARC_INIT:
    this->Initialize( p_gman );
    m_Seq = SEQ_ARC_INIT_WAIT;
    //break throw

  case SEQ_ARC_INIT_WAIT:
    if( this->InitializeWait(p_gman) ){
      m_Seq = SEQ_APP_CALL_CHECK;
    }
    break;

  case SEQ_APP_CALL_CHECK:
    //図鑑登録チェック
    if( !EventAddPokemon::CheckZukanRegist(p_gman) ){
      m_Seq = SEQ_END;
      break;
    }
    {
      // クローズイベント
      EventFieldmapClose* p_event;
      EventFieldmapClose::Desc desc;
      
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
                  p_gman->GetGameEventManager( ) );
      desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
      p_event->SetDesc( desc );
      p_event->SetFadeCallBackCustom( m_FadeDisp, m_FadeType, m_FadeStartCol, m_FadeEndCol, m_FadeSync );
    }

    m_Seq++;
    break;
  case SEQ_APP_CALL:
    CallZukanRegist(p_gman);
    m_Seq++;
    break;
  case SEQ_FIELD_OPEN:
    {
      EventFieldmapOpen* p_event;
      p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                    p_gman->GetGameEventManager( ) );
      p_event->SetDesc( FM_OPEN_MODE_APP_RECOVER, *p_gman->GetGameData()->GetStartLocation() );

      if( m_FadeInEnable ){
        p_event->SetFadeCallBackCustom( m_FadeDisp, m_FadeType, m_FadeSync );
      }else{
        p_event->SetFadeCallBack( NULL,NULL,NULL );
      }
    }
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
void EventAddPokemon::EndFunc(GameSys::GameManager* p_gman)
{
  GFL_SAFE_DELETE(m_pPokeParam);
}

//--------------------------------------------------------------
/**
 * @brief データ初期化 
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventAddPokemon::Initialize(GameSys::GameManager* p_gman)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();

  m_InitSeq = 0;

  //---------------------------------------------------
  // ARCファイルオープン
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    openReq.heapForFile = m_pHeap->GetLowerHandle();
    openReq.heapForArcSrc = m_pHeap->GetLowerHandle();
    openReq.heapForReq = m_pHeap->GetLowerHandle();
    pAsyncFileManager->AddArcFileOpenReq( openReq );
  }
  //---------------------------------------------------
  // ファイル読み込みリクエスト
  //---------------------------------------------------
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;

    loadReq.arcId = ARCID_SCRIPT_EVENT_DATA;
    loadReq.datId = GARC_script_event_data_add_poke_BIN;
    loadReq.ppBuf = &m_pBuffer;
    loadReq.heapForBuf = m_pHeap;
    loadReq.heapForReq = m_pHeap->GetLowerHandle();
    loadReq.heapForCompressed = NULL;
    loadReq.pBufSize = &m_BufferSize;
    loadReq.align = 4;
    pAsyncFileManager->AddArcFileLoadDataReq( loadReq );  
  }
  
}

//--------------------------------------------------------------
/**
 * @brief データ初期化 
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
bool EventAddPokemon::InitializeWait(GameSys::GameManager* p_gman)
{
  gfl2::fs::AsyncFileManager* pAsyncFileManager = p_gman->GetAsyncFileManager();
  
  switch( m_InitSeq ){
  case 0:
    // 読み込み完了待ち
    if( !pAsyncFileManager->IsArcFileOpenFinished( ARCID_SCRIPT_EVENT_DATA ) ){
      return false;
    }
    if( !pAsyncFileManager->IsArcFileLoadDataFinished( &m_pBuffer ) ){
      return false;
    }

    //データ格納
    {
      AddPokemonParam* pParam = reinterpret_cast<AddPokemonParam *>(m_pBuffer);
      u32 param_num = m_BufferSize/sizeof(AddPokemonParam);
      if( m_AddPokeID >= param_num ){
        GFL_ASSERT_MSG(0,"%d>=%d",m_AddPokeID,param_num);
        m_AddPokeID = 0;
      }
      m_pPokeParam = MakePokeParam( p_gman, &pParam[m_AddPokeID] );
    }

    //バッファは開放
    GflHeapSafeFreeMemory(m_pBuffer);  

    //アーカイブ閉じる
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId = ARCID_SCRIPT_EVENT_DATA;
      closeReq.heapForReq = m_pHeap->GetLowerHandle();
      pAsyncFileManager->AddArcFileCloseReq( closeReq );
    }

    m_InitSeq++;
    return false;

  case 1:
    if( !pAsyncFileManager->IsArcFileCloseFinished( ARCID_SCRIPT_EVENT_DATA ) ){
      return false;
    }
    m_InitSeq++;
    break;

  default:
    break;
  }

  return true;
}

//--------------------------------------------------------------
/**
 * @brief 図鑑登録チェッカー
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
bool EventAddPokemon::CheckZukanRegist(GameSys::GameManager* p_gman)
{
  PokeTool::PokeGetUtil::CallerType::Tag callerType;
  PokeTool::PokeGetUtil::ExData exData;
  GameSys::GameData* p_gdata = p_gman->GetGameData();

  //追加モード
  if( m_AddPokeID == POKE_ADD_TBL_MOVIE_MAGIANA ){
    gfl2::system::Date date;
    const Savedata::GameTime* p_gtime = p_gdata->GetGameTime();

    p_gtime->GetDate( &date );

    exData.exDataUnion.exDataForHaifu.placeNo = PokeTool::PokePlace::PLACE_NO_START_OUTER+MAPNAME_SUTEKINABASYO;
    exData.exDataUnion.exDataForHaifu.year = date.GetYear();
    exData.exDataUnion.exDataForHaifu.month = date.GetMonth();
    exData.exDataUnion.exDataForHaifu.day = date.GetDay();

    callerType = PokeTool::PokeGetUtil::CallerType::HAIFU_NANASHI;
  }
  else if( m_AddPokeID == POKE_ADD_TBL_SATOSHI_PIKACHU ){
    /*  M20ピカチュウ  */
    gfl2::system::Date date;
    const Savedata::GameTime* p_gtime = p_gdata->GetGameTime();

    p_gtime->GetDate( &date );

    exData.exDataUnion.exDataForHaifu.placeNo = PokeTool::PokePlace::PLACE_NO_START_OUTER+MAPNAME_EIGA17;
    exData.exDataUnion.exDataForHaifu.year    = date.GetYear();
    exData.exDataUnion.exDataForHaifu.month   = date.GetMonth();
    exData.exDataUnion.exDataForHaifu.day     = date.GetDay();

//    callerType = PokeTool::PokeGetUtil::CallerType::HAIFU_NANASHI;    /*  名無しはzukanで親がプレイヤーになる  */
    callerType = PokeTool::PokeGetUtil::CallerType::HAIFU;
  }
  else if( m_pPokeParam->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ){
    callerType = PokeTool::PokeGetUtil::CallerType::MORAU_TAMAGO;
    exData.exDataUnion.exDataForMorauTamago.placeNo = PokeTool::PokePlace::PLACE_NO_PERSON_SODATEYA;
  }
  else if( m_AddPokeMode == ADD_POKEMON_MODE_DEFAULT ){
    callerType = PokeTool::PokeGetUtil::CallerType::MORAU_NANASHI;
  }
  else{
    callerType = PokeTool::PokeGetUtil::CallerType::MORAU_NANASHI_PARTY;
  }
  //呼び出し用パラメータも設定しておく
  m_EventParam.pp = m_pPokeParam;
  m_EventParam.callerType = callerType;

  PokeTool::PokeGetUtil::ReturnType::Tag ret = 
    PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
        m_pHeap->GetLowerHandle(),
        m_pPokeParam,
        callerType,
        NULL,
        &exData);

  if( ret == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER ){
    return true;  //図鑑アプリを呼ぶ
  }else{
    return false; //呼ぶ必要がない
  }
}

//--------------------------------------------------------------
/**
 * @brief 図鑑登録アプリコール
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventAddPokemon::CallZukanRegist(GameSys::GameManager* p_gman)
{
  app::event::ZukanRegisterEvent::StartEvent( p_gman, &m_EventParam );
}

//--------------------------------------------------------------
/**
 * @brief 指定のポケモンパラメータ生成
 * @param p_gman  ゲームマネージャークラス
 */
//--------------------------------------------------------------
pml::pokepara::PokemonParam* EventAddPokemon::MakePokeParam(GameSys::GameManager* p_gman, const AddPokemonParam* cp_param )
{
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  Savedata::MyStatus * p_status = p_gdata->GetPlayerStatus();

  pml::pokepara::InitialSpec spec;
  spec.monsno = static_cast<MonsNo>( cp_param->monsno );
  spec.formno = cp_param->form;
  spec.level  = cp_param->level;
  spec.id     = p_status->GetID();
  
  // パーソナルロード
  pml::personal::LoadPersonalData( spec.monsno, spec.formno );

  // 強制レア・ノーマル
  switch ( cp_param->rare_mode )
  {
  case POKE_ADD_TBL_RARE_RARE:
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_TRUE;
    break;
  case POKE_ADD_TBL_RARE_NORMAL:
    spec.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE;
    break;
  default:
    break;
  }

  // 強制とくせいセット
  /**
      2017/03/16 Thu. 13:15:21  Yu Muto
      特性指定にレア指定のフィールドを参照してしまっている
      nijiからの不具合で、マギアナとコスモッグにおいて
      R:\home\momiji\resource\field_data\script_event_data\add_poke.xlsx
      で指定した通りの値となっていない
      対応検討中
      
      MMCat[45]   サブイベントでもらえるポケモンで意図通りの特性フラグになっていないものがある
      2017/03/23 Thu. 15:07:24  Yu Muto
      momijiでは本不具合を修正する
      修正前後で、配布されるマギアナ/コスモッグに変化がないようadd_poke.xlsxも修正（「特性2」に設定）した
  */
  switch ( cp_param->tokusei_mode )
  {
  case POKE_ADD_TBL_TOKUSEI_1:
    spec.tokusei_index = 0;
    break;
  case POKE_ADD_TBL_TOKUSEI_2:
    spec.tokusei_index = 1;
    break;
  case POKE_ADD_TBL_TOKUSEI_3:
    spec.tokusei_index = 2;
    break;
  default:
    break;
  }

  //強制性格セット
  if ( cp_param->seikaku_id >= 0 )
  {
    spec.seikaku = static_cast<pml::pokepara::Seikaku>(cp_param->seikaku_id);
  }
  else if ( cp_param->egg_mode == POKE_ADD_TBL_EGG_MODE_NONE )
  {
    //タマゴではない時シンクロ反映
    pml::PokeParty* party = p_gdata->GetPlayerParty();
    pml::pokepara::PokemonParam* p_pp;
    int  idx;

    //タマゴでない先頭ポケモン(瀕死は可)のパラメータチェック
    idx = party->GetMemberTopIndex(pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG);
    if( idx != pml::PokeParty::MEMBER_INDEX_ERROR ){

      p_pp = party->GetMemberPointer(idx);
      if( p_pp->GetTokuseiNo() == TOKUSEI_SINKURO ){
        spec.seikaku = p_pp->GetSeikaku();
      }
    }
  }

  // 強制性別セット
  if ( cp_param->sex_mode != POKE_ADD_TBL_SEX_RANDOM )
  {
    switch ( cp_param->sex_mode )
    {
    case POKE_ADD_TBL_SEX_MALE:
      spec.sex = PM_MALE;
      break;
    case POKE_ADD_TBL_SEX_FEMALE:
      spec.sex = PM_FEMALE;
      break;
    default : GFL_ASSERT(0);
    }
  }


  // 強制個体値セット
  if (cp_param->talent_hp < -1 ) //-1ランダム(spec.talentVNumの初期値),-2以下でNV
  {
    // NV設定
    spec.talentVNum = (cp_param->talent_hp*-1)-1;
  }
  else
  {
    if ( cp_param->talent_hp >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_HP ] = cp_param->talent_hp;
    }
    if ( cp_param->talent_atk >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_ATK ] = cp_param->talent_atk;
    }
    if ( cp_param->talent_def >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_DEF ] = cp_param->talent_def;
    }
    if ( cp_param->talent_spatk >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_SPATK ] = cp_param->talent_spatk;
    }
    if ( cp_param->talent_spdef >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_SPDEF ] = cp_param->talent_spdef;
    }
    if ( cp_param->talent_agi >= 0 )
    {
      spec.talentPower[ pml::pokepara::POWER_AGI ] = cp_param->talent_agi;
    }
  }


  pml::pokepara::PokemonParam* p_pp;
    p_pp = GFL_NEW(m_pHeap) pml::pokepara::PokemonParam(m_pHeap, spec );

  // アイテム
  if ( cp_param->item_no > ITEM_DUMMY_DATA && cp_param->item_no <= ITEM_DATA_MAX )
  {
    p_pp->SetItem( cp_param->item_no );
  }

  bool bFastMode = p_pp->StartFastMode();

  {

    // 親の名前、性別を設定
    u8 player_sex = p_status->GetSex();

    gfl2::str::StrBuf tmpBuf( 32, m_pHeap->GetLowerHandle() );
    p_status->GetNameString( &tmpBuf );

    p_pp->SetParentName( &tmpBuf );
    p_pp->SetParentSex( static_cast<pml::Sex>(player_sex) );

    //バージョン設定
    p_pp->SetCassetteVersion( System::GetVersion() );

    // タマゴ判定
    if ( cp_param->egg_mode != POKE_ADD_TBL_EGG_MODE_NONE )
    {
      // タマゴにする（フラグ立てて、ニックネームを"タマゴ"にして、孵化歩数セットする）
      p_pp->ChangeEgg(m_pHeap);
    }

    // 固有技（押し出し）
    if ( cp_param->wazano != WAZANO_NULL )
    {
      p_pp->PushWaza( static_cast<WazaNo>(cp_param->wazano) );
      p_pp->SetTamagoWazaNo( 0, static_cast<WazaNo>(cp_param->wazano) ); //デフォ技+固有技1個なので、タマゴ技は絶対1個
    }

  }

  //モンスターボールセット
  p_pp->SetGetBall( item::BALLID_MONSUTAABOORU );

  //初期3体の場合、個性乱数を保存 @fix cov_ctr[12613]:MONSNOと比較してしまっていた部分を修正
	if (m_AddPokeID == POKE_ADD_TBL_FIRST_HONOO || m_AddPokeID == POKE_ADD_TBL_FIRST_KUSA || m_AddPokeID == POKE_ADD_TBL_FIRST_MIZU )
  {
    p_gman->GetGameData()->GetMisc()->SetShoki3PersonalRnd( p_pp->GetPersonalRnd() );
  }

  //マギアナ専用のパラメータセット処理
  if( m_AddPokeID == POKE_ADD_TBL_MOVIE_MAGIANA )
  {
    //イベントフラグセット
    p_pp->SetEventPokeFlag(true);

    //ウィッシュリボンセット(GTSアップロードを防ぐ役割)
    p_pp->SetRibbon( RIBBON_NO_WISH );

    //プレシャスボールセット
    p_pp->SetGetBall( item::BALLID_PURESYASUBOORU );

    //技セット
    p_pp->SetWaza( 0, WAZANO_HURUURUKANON ); //フルールカノン
    p_pp->SetTamagoWazaNo( 0, WAZANO_HURUURUKANON );

    p_pp->SetWaza( 1, WAZANO_RASUTAAKANON ); //ラスターカノン
    p_pp->SetTamagoWazaNo( 1, WAZANO_RASUTAAKANON );

    p_pp->SetWaza( 2, WAZANO_OMAZINAI ); //おまじない
    p_pp->SetTamagoWazaNo( 2, WAZANO_OMAZINAI );

    p_pp->SetWaza( 3, WAZANO_TEDASUKE ); //てだすけ
    p_pp->SetTamagoWazaNo( 3, WAZANO_TEDASUKE );

    // @fix NMCat[3890] 【ふしぎなおくりもの】捕獲レベルが反映されない
    //         →マギアナは配布と同じ処理を通すので、捕獲レベルを設定しておく。
    p_pp->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL, p_pp->GetLevel() );
  }

  //M20ピカチュウ専用処理
  if( m_AddPokeID == POKE_ADD_TBL_SATOSHI_PIKACHU )
  {
#if 0
  struct AddPokemonParam
  {
    u16 monsno;  ///< モンスターNO
    u8  form;    ///< フォルムNO
    u8  level;   ///< レベル

    u8  rare_mode;       ///< 強制レア PokeAddTableRareMode
    u8  sex_mode;        ///< せいべつ PokeAddTableSexMode
    u8  tokusei_mode;    ///< とくせいID PokeAddTableTokuseiMode
    s8  seikaku_id;      ///< せいかくID

    u16 item_no;        ///< アイテムNO
    u16 egg_mode;       ///< タマゴ指定モード

    u16 wazano;         ///< 固有技(押しだし)
    s8 talent_hp;       ///< 個体値：HP
    s8 talent_atk;      ///< 個体値：ATK
    s8 talent_def;      ///< 個体値：DEF
    s8 talent_spatk;    ///< 個体値：SPATK
    s8 talent_spdef;    ///< 個体値：SPDEF
    s8 talent_agi;      ///< 個体値：AGI
  };
#endif
    /** xlsx:    mons#： 25         */
    /** xlsx:    form#：  7         */
    /** default: NickN：(empty)     */
    /** xlsx:      Lv.： 21         */
    /** code:     親名：サトシ      */
    /** code:     親性：♂          */
    /** code:     親ID：20391700715 */
    /** xlsx:       卵：false       */
    /** code:      技1：WAZANO_10MANBORUTO    */
    /** code:      技2：WAZANO_DENKOUSEKKA    */
    /** code:      技3：WAZANO_KAMINARI       */
    /** code:      技4：WAZANO_AIANTEERU      */
    /** code:    卵技1：WAZANO_10MANBORUTO    */
    /** code:    卵技2：WAZANO_DENKOUSEKKA    */
    /** code:    卵技3：WAZANO_KAMINARI       */
    /** code:    卵技4：WAZANO_AIANTEERU      */
    /** xlsx:     性格：がんばりや  */
    /** xlsx:     持物：ITEM_KIREINAHANE(きれいなハネ)  */
    /** default:ボール：モンスターボール  */
    /** default:   ROM：            */
    /** default:  言語：            */
    /** default:  個乱：ランダム    */
    /** xlsx:     特性：1:せいでんき  */
    /** xlsx:     性別：♂          */
    /** xlsx:     レア：なし        */
    /** code:     配布：true        */
    /** default:卵入手：(empty)     */
    /** code:     捕獲：ポケモン映画17  */
    /** code:  捕獲Lv.：21          */
    /** default:     prm：            */
    /** default: prm乱数：ランダム    */
    /** default:  努力値：            */
    /** code:   リボン：ウィッシュリボン  */

    const u32 M20PikatyuCapturedLevel = 21U;            /**<  TPC様より：入手レベルは21     */
    const u32 M20PikatyuParentID      = 2039170715UL;   /**<  TPC様より：親IDは2039170715  */

    /*  親関連  */
    {
      gfl2::heap::HeapBase* const pHeapBase = m_pHeap->GetLowerHandle();
      gfl2::str::MsgData*         pMsgData  = GFL_NEW(pHeapBase) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_namelist_DAT, pHeapBase);   /*  LOAD_PART  */
      gfl2::str::StrBuf           strBuf(System::STRLEN_PLAYER_NAME + System::EOM_LEN, pHeapBase);    /*  check[2977] 文字列のバッファ確保長チェック・EOM長が確保されているか？:  EOM_LEN追加  */
      const u32                   nameID    = ash;
      
      pMsgData->GetString( nameID, strBuf );

      p_pp->SetParentName(&strBuf);           /** code:     親名：サトシ      */
      p_pp->SetParentSex(pml::SEX_MALE);      /** code:     親性：♂          */
      p_pp->SetID(M20PikatyuParentID);        /** code:     親ID：2039170715  */

      GFL_SAFE_DELETE(pMsgData);
    }
    
    //イベントフラグセット
    p_pp->SetEventPokeFlag(true);

    //ウィッシュリボンセット(GTSアップロードを防ぐ役割)
    p_pp->SetRibbon( RIBBON_NO_WISH );

    //技セット
    p_pp->SetWaza( 0, WAZANO_10MANBORUTO );    p_pp->SetTamagoWazaNo( 0, WAZANO_10MANBORUTO );    /*  10まんボルト    */
    p_pp->SetWaza( 1, WAZANO_DENKOUSEKKA );    p_pp->SetTamagoWazaNo( 1, WAZANO_DENKOUSEKKA );    /*  でんこうせっか  */
    p_pp->SetWaza( 2, WAZANO_KAMINARI );       p_pp->SetTamagoWazaNo( 2, WAZANO_KAMINARI );       /*  かみなり        */
    p_pp->SetWaza( 3, WAZANO_AIANTEERU );      p_pp->SetTamagoWazaNo( 3, WAZANO_AIANTEERU );      /*  アイアンテール  */

    /*  捕獲  */
    p_pp->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL,  M20PikatyuCapturedLevel);      /*  捕獲レベル  */
  }



  p_pp->EndFastMode( bFastMode );
  return p_pp;
}

} // namespace Field
