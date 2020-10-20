//==============================================================================
/**
 * @file        ScriptFuncSetMessage.cpp
 * @brief       ScriptFuncSetCommon系 メッセージウィンドウ他UI制御系統
    
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:48
 */
//==============================================================================

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/SaveData.h"
#include "Print/include/WordSet.h"

#include "FieldScript/include/ScriptWork.h"
#include "FieldScript/include/ScriptObject.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "System/include/HeapDefine.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/Window/FieldWindowManager.h"

#include "Field/FieldRo/include/Window/FieldYesNoWindow.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"
#include "Field/FieldRo/include/Window/FieldListMenuWindow.h"

#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelPlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldFacialAnime.h"

// trainer
#include "Trainer/Trainer/include/tr_tool.h"
#include "Trainer/Trainer/include/TrainerMessage.h"

// ジョインフェスタギミック
#include "Field/FieldRos/FieldGimmickJoinFesta/include/FieldGimmickJoinFesta.h"
// ジョインフェスタパーソナルデータ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
// ジョインフェスタ動的動作モデルロードシステム
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModel.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFModelManager.h"
#include "NetApp/JoinFesta/include/JFDynamicLoadSystem/JFDynamicLoadSystem.h"

// バトルフェス
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"

#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "niji_reference_files/script/FieldPawnTypes.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/script_message.gaix>

// debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )





/* ------------------------------------------------------------------------- */
/**
 * @brief 数値をワードセット
 */
/* ------------------------------------------------------------------------- */
static void _WordSetNumber( AMX* amx, u32 buf_id, u32 number, u32 keta, u32 disp )
{
  // 桁数指定が0の場合、自動で桁数を求める
  if (keta == 0) {
    keta = 1;
    int tmp = number;
    while (tmp > 9) {
      tmp /= 10;
      ++keta;
    }
  }

//ScriptObject      *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
//Work              *p_script_work = p_obj->GetWork();
  FieldScriptSystem *pSys = FieldScriptSystem::GetInstance();
  print::WordSet    *wset = pSys->GetSystemWork()->GetWordSet();

  wset->RegisterNumber( buf_id, number, keta, (print::NumberDispType)disp, print::NUM_CODE_DEFAULT );
}


/* ------------------------------------------------------------------------- */
/**
 * @brief WaitFunc:非常駐（＝イベント専用）メッセージクローズ
 */
/* ------------------------------------------------------------------------- */
static bool WaitFunc_MsgClose( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  FieldScriptSystem     *pSys           = FieldScriptSystem::GetInstance();
  GameSys::GameManager  *p_gman         = pSys->GetGameManager();
  SystemSingletones     *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  Work                  *p_script_work  = (Work*)p_work;
  u16                   *seq            = p_script_work->GetSeq();
  
  enum
  {
    SEQ_INIT,
    SEQ_END,
  };
  
  switch (*seq)
  {
    case SEQ_INIT:
    {
      if( p_swk->m_pFieldTalkWindow[ label ] ) p_swk->m_pFieldTalkWindow[ label ]->CloseMsgWin();
      *seq = SEQ_END;
      break;
    }
    case SEQ_END:
      GFL_SAFE_DELETE( p_swk->m_pFieldTalkWindow[ label ] );
      *seq = 0;
      return true;
  }
  
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  インスタント拡張メッセージ設定
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ExtraMsgInstant_( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  Work                    *p_script_work  = p_obj->GetWork();

  u32 extra_arc_id = ptr[1];
  u32 extra_msg_id = ptr[2];

  u32 arcid = print::GetMessageArcId();
  if( extra_arc_id ){
    arcid = print::GetMessageArcId_Script();
  }
  p_script_work->SetExtraMsgInstant(arcid,extra_msg_id);

  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  拡張メッセージロード
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ExtraMsgLoad_( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  SystemSingletones       *p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  gfl2::heap::HeapBase    *p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  u32 extra_arc_id = ptr[1];
  u32 extra_msg_id = ptr[2];

  u32 arcid = print::GetMessageArcId();
  if( extra_arc_id ){
    arcid = print::GetMessageArcId_Script();
  }
  gfl2::fs::AsyncFileManager* pAsyncFileMan = p_gman->GetAsyncFileManager();

  MsgWork* wk = GFL_NEW(p_heap) MsgWork;
  wk->arcID = arcid;
  wk->datID = extra_msg_id;
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
    oreq.arcId       = arcid;
    oreq.heapForFile = p_heap->GetLowerHandle();
    oreq.heapForReq  = p_heap->GetLowerHandle();
    pAsyncFileMan->AddArcFileOpenReq(oreq);
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId             = arcid;
    req.datId             = extra_msg_id;
    req.ppBuf             = &wk->pMsgBuf;
    req.heapForBuf        = p_heap;                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
    req.heapForReq        = p_heap->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
    req.heapForCompressed = NULL;
    req.align             = 4;
    pAsyncFileMan->AddArcFileLoadDataReq(req);
  }
  p_swk->m_pMsgLoadWork = wk;
#if PM_DEBUG
  p_swk->m_load_msg_arc_id   = arcid;
  p_swk->m_load_msg_data_id  = extra_msg_id;
#endif
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  拡張メッセージロード待ち
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ExtraMsgIsLoaded_( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  SystemSingletones       *p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  gfl2::fs::AsyncFileManager* pAsyncFileMan = p_gman->GetAsyncFileManager();
  gfl2::heap::HeapBase    *p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );

  if( pAsyncFileMan->IsArcFileOpenFinished( p_swk->m_pMsgLoadWork->arcID ) &&
      pAsyncFileMan->IsArcFileLoadDataFinished(&p_swk->m_pMsgLoadWork->pMsgBuf) )
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
    creq.arcId       = p_swk->m_pMsgLoadWork->arcID;
    creq.heapForReq  = p_heap_dev->GetLowerHandle();
    pAsyncFileMan->SyncArcFileClose(creq);

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  拡張メッセージアンロード
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ExtraMsgUnload( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  SystemSingletones       *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  GFL_ASSERT(p_swk->m_pMsgLoadWork);
  MsgWork* wk = p_swk->m_pMsgLoadWork;

  GflHeapSafeFreeMemory(wk->pMsgBuf);
  GFL_SAFE_DELETE(p_swk->m_pMsgLoadWork);
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  拡張メッセージ Push
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ExtraMsgSwap_( AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  SystemSingletones       *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  MsgWork* wk    = p_swk->m_pMsgLoadWork;
  p_swk->m_pMsgLoadWork = p_swk->m_pMsgSwapWork;
  p_swk->m_pMsgSwapWork = wk;

  return NULL;
}

//------------------------------------------------------------------------------
/**
 * @func        PG_WordSetRegister
 * @brief       sango時代のレジスト関連を纏めてIDで分岐させるようにしたもの.
 * @author      N.Takeda
 * @date        2015/05/12(火) 19:11:27
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::PG_WordSetRegister( AMX* amx, const cell* ptr )
{
  ScriptObject          *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager  *p_gman         = p_obj->GetGameManager();
  print::WordSet        *wset           = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetWordSet();
  Savedata::MyStatus    *status         = p_gman->GetGameData()->GetPlayerStatus();
  SystemSingletones     *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  Work                  *p_script_work  = p_obj->GetWork();

  GameSys::GameData*    p_gdata = p_gman->GetGameData();
  pml::PokeParty*       p_party = p_gdata->GetPlayerParty();

  u32   registType = ptr[1];
  u32   bufID      = ptr[2];
  int   ret = 0;

  GFL_ASSERT( bufID < SystemWork::BUF_ID_NORMAL_MAX );
  
  switch ( registType )
  {
    case SCR_WORDSET_TYPE_PLAYER_NAME:
    {
      wset->RegisterPlayerName( bufID, status );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1011):   wset->RegisterPlayerName(bufID, status);
      break;
    }
    case SCR_WORDSET_TYPE_POKEMONSNAMENO:
    {
      MonsNo  monsno = static_cast<MonsNo> (ptr[3]);
      wset->RegisterPokeMonsNameNo( bufID, monsno );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1036):   wset->RegisterPokeMonsNameNo(buf_id, monsno);
      break;
    }
    case SCR_WORDSET_TYPE_TRTYPENAME:
    {
      trainer::TrainerID    tr_id = static_cast< trainer::TrainerID >( ptr[3] );
      gfl2::heap::HeapBase  *p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      trainer::TRAINER_DATA tr_data;
      trainer::LoadTrainerData( p_heap, tr_id, &tr_data );
      
      wset->RegisterTrTypeName( bufID, tr_data.tr_type );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1719):   wset->RegisterTrTypeName( buf_id, tr_data.tr_type );
      break;
    }
    case SCR_WORDSET_TYPE_TRAINERNAME:
    {
      u32 trID  = ptr[3];
      wset->RegisterTrainerName( bufID, trID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1763):   wset->RegisterTrainerName( bufID, tr_id );
      break;
    }
    case SCR_WORDSET_TYPE_POKETYPENAME:
    {
      u32 typeID = ptr[3];
      wset->RegisterPokeTypeName( bufID, typeID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1311):   wset->RegisterPokeTypeName( buf_id, type_id );
      break;
    }
    case SCR_WORDSET_TYPE_TOKUSEINAME:
    {
      u32 tokuseiID = ptr[3];
      wset->RegisterTokuseiName( bufID, tokuseiID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1512):   System::GetInstance()->GetSystemWork()->GetWordSet()->RegisterTokuseiName( bufID, tokusei_id );
      break;
    }
    case SCR_WORDSET_TYPE_SEIKAKUNAME:
    {
      u32 seikakuID = ptr[3];
      wset->RegisterSeikakuName( bufID, seikakuID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1334):   wset->RegisterSeikakuName( buf_id, seikaku_id );
      break;
    }
    case SCR_WORDSET_TYPE_WAZANAME:
    {
      u32 wazaID = ptr[3];
      wset->RegisterWazaName( bufID, wazaID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1196):   wset->RegisterWazaName( bufID, waza_no );
      break;
    }
    case SCR_WORDSET_TYPE_ITEMWAZANAME:
    {
      u32 itemNo = ptr[3];
      u32 wazaID = item::ITEM_GetWazaNo(itemNo);
      wset->RegisterWazaName( bufID, wazaID );
        // c:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1082):   wset->RegisterWazaName(buf_id, waza_no);
      break;
    }
    case SCR_WORDSET_TYPE_ITEMNAME:
    {
      u32 itemNo = ptr[3];
      u32 itemNum = ptr[4];
      wset->RegisterItemName( bufID, itemNo, itemNum);
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1060):   wset->RegisterItemName(buf_id, item_no, item_num);
      break;
    }
    case SCR_WORDSET_TYPE_PLACENAME:
    {
      u16 zone_id = ptr[3];
      ZoneDataAccessor zoneDataAccessor( p_gdata->GetFieldZoneDataLoader()->GetZoneData( zone_id ) );

      wset->RegisterPlaceName( bufID, zoneDataAccessor.GetPlaceLabelID() );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1384):   wset->RegisterPlaceName( bufID, place_id );
      break;
    }
    case SCR_WORDSET_TYPE_RIBBONNAME:
    {
      u32 ribbonID = ptr[3];
      wset->RegisterRibbonName( bufID, ribbonID );
        // C:/home/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1692):   System::GetInstance()->GetSystemWork()->GetWordSet()->RegisterRibbonName( buf_id, ribbon_id );
      break;
    }
    case SCR_WORDSET_TYPE_ITEMPOCKETNAME:
    {
      u32 pocket_no = ptr[3];
      wset->RegisterFieldPocketName( bufID, pocket_no );
        // c:/work/sangoScript/sango_project/prog/src/field/script/ScriptFuncSetCommon.cpp(1109):   wset->RegisterFieldPocketName(buf_id, pocket_no);
      break;
    }
    case SCR_WORDSET_TYPE_MESSAGE:
    {
      //Param
      u32  msg_label = ptr[3];
      bool msgbuf_f = GFL_BOOL_CAST(ptr[4]);

      gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

      if(msgbuf_f){
        //ExtraMsgLoad_によって確保されたものを使う
        gfl2::str::MsgData* pMsgData = GFL_NEW( p_heap ) gfl2::str::MsgData( p_swk->m_pMsgLoadWork->pMsgBuf, p_heap );//ExtraMsgLoad_で確保されたもの

        wset->RegisterWord( bufID, pMsgData, msg_label);

        GFL_SAFE_DELETE( pMsgData );
      }else{
        //通常のを使う
        u32 msg_arc_id = p_script_work->GetMsgArcId();
        u32 msg_dat_id = p_script_work->GetMsgDatId();

        gfl2::str::MsgData* pMsgData = GFL_NEW( p_heap ) gfl2::str::MsgData( msg_arc_id, msg_dat_id, p_heap );

        wset->RegisterWord( bufID, pMsgData, msg_label);

        GFL_SAFE_DELETE( pMsgData );
      }
      break;
    }
    case SCR_WORDSET_TYPE_POKENICKNAME:
    {
      //Para
      u32 poke_slot = ptr[3];

      pml::pokepara::CoreParam* pPoke = p_party->GetMemberPointer( poke_slot );
      wset->RegisterPokeNickName( bufID, pPoke );
      break;
    }
    case SCR_WORDSET_TYPE_POKEOYANAME:
    {
      //Para
      u32 poke_slot = ptr[3];

      pml::pokepara::CoreParam* pPoke = p_party->GetMemberPointer(poke_slot);
      wset->RegisterPokeOyaName(bufID, pPoke);
      break;
    }
    case SCR_WORDSET_TYPE_POKEPASTOYANAME:
    {
      //Para
      u32 poke_slot = ptr[3];

      pml::pokepara::CoreParam* pPoke = p_party->GetMemberPointer(poke_slot);
      wset->RegisterPokePastOyaName(bufID, pPoke);
      break;
    }
    case SCR_WORDSET_TYPE_DENDOU_PARTY:
    {
      //Para
      DendouiriPartyType type = static_cast<DendouiriPartyType>(ptr[3]);
      ret = 0;
      u16* pMember = p_gdata->GetEventWork()->GetDendouiriPartyArrayPtr(type);

      for( u32 i = 0; i< pml::PokeParty::MAX_MEMBERS; i++ ){
        if( pMember[i] != MONSNO_NULL ){
          wset->RegisterPokeMonsNameNo( bufID+i, static_cast<MonsNo>(pMember[i]) );
          ret++;
        }
      }
      break;
    }
  }
  
  return ret;
}

//-----------------------------------------------------------------------------
/**
 * @brief  数字(整数)をレジスト
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::WordSetNumber(AMX * amx, const cell * ptr)
{
  u32 buf_id = ptr[1];
  u32 number = ptr[2];
  u32 keta = ptr[3];
  u32 disp = ptr[4];

  GFL_ASSERT( buf_id < SystemWork::BUF_ID_NORMAL_MAX );

  _WordSetNumber( amx, buf_id, number, keta, disp );

  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * @brief  おこづかい(整数)をレジスト
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::WordSetMoney(AMX * amx, const cell * ptr)
{
  u32 buf_id = SystemWork::BUF_ID_MONEY;
  u32 number = ptr[1];
  u32 keta = ptr[2];
  u32 disp = ptr[3];

  _WordSetNumber( amx, buf_id, number, keta, disp );

  return NULL;
}


//------------------------------------------------------------------------------
/**
 * @func        YesNoWin_Seq
 * @brief       YESNOウィンドウ終了待ち.
 * @author      N.Takeda
 * @date        2015/04/14(火) 15:06:31
 *
 * @param[in,out] AMX*        amx .
 * @param[in]     const cell* ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::YesNoWin_Seq( AMX *amx, const cell *ptr )
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  SystemSingletones       *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  Work                    *p_script_work  = p_obj->GetWork();
  u16                     *seq            = p_script_work->GetSeq();

  static const s32 YESNOWIN_X = 180;
  static const s32 YESNOWIN_Y = 3; ///< +で上、-で下   ///< GFMNCat#1902:汎用選択メニューと会話ウィンドウが重なってしまいます  +3 対応
  bool is_ctrl_str      = GFL_BOOL_CAST(ptr[1]);                                    ///< true/false でArc先を変更するもののようだ.
  u32  str1_id          = ptr[2];                                           ///< 省略時:0
  u32  str2_id          = ptr[3];                                           ///< 省略時:0
  u8   init_pos         = ptr[4];                                           ///< 省略時:0
  bool b_msgbuf         = GFL_BOOL_CAST(ptr[5]);
  
  GFL_ASSERT( init_pos < 2 );
  
  gfl2::heap::HeapBase    *p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  
  
  u32 msg_arc_id, msg_dat_id;
  
  if( is_ctrl_str ){
    msg_arc_id = p_script_work->GetMsgArcId();
    msg_dat_id = p_script_work->GetMsgDatId();
  }else{
    msg_arc_id = print::GetMessageArcId_Script();
    msg_dat_id = GARC_script_message_common_scr_DAT;
  }
  
  enum
  {
    SEQ_LOAD_INIT,
    SEQ_LOAD,
    SEQ_KEYWAIT,
    SEQ_END,
  };
  
  switch (*seq)
  {
    case SEQ_LOAD_INIT:
    {
      void  *pMsgData = (b_msgbuf) ? p_swk->m_pMsgLoadWork->pMsgBuf : NULL;
      
      p_swk->m_pYesNoWin = GFL_NEW(p_heap_dev) Field::FieldWindow::FieldYesNoWindow( p_gman->GetFieldmap()->GetFieldWindowManager(),
        YESNOWIN_X, YESNOWIN_Y,
        msg_arc_id, msg_dat_id, str1_id, str2_id, init_pos, App::Tool::ListMenu::B_BUTTON_MODE_CANCEL,
        pMsgData
      );
        *seq = SEQ_KEYWAIT;
    }
    break;

    case SEQ_KEYWAIT:
    {
      if( p_swk->m_pYesNoWin->GetDecide() )
      {
        u32 uRes = p_swk->m_pYesNoWin->GetSelectId();
        if( uRes == App::Tool::ListMenu::SELECT_CANCEL )  uRes = 1;
        p_script_work->SetTempWorkValue( SCWK_ANSWER, uRes );
        p_gman->GetFieldmap()->GetFieldWindowManager()->SetLastResultYesNo( uRes );
        p_swk->m_pYesNoWin->SetVisible( false );
        p_swk->m_pYesNoWin->DeleteMenu();
        *seq = SEQ_END;
      }
    }
    break;

    case SEQ_END:
    {
      if( p_swk->m_pYesNoWin->IsInstanceDelete() )
      {
        GFL_SAFE_DELETE( p_swk->m_pYesNoWin );
        *seq = 0;
        return true;
      }
      break;
    }
    default: GFL_ASSERT(0);
  }
  
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        TalkMdlMsg_Seq
 * @brief       TalkMdlMsg_Seq関数.
 */
//------------------------------------------------------------------------------
bool FuncSetCommon::WaitFunc_TalkMsgSeq( u32 label, void* p_work )
{
  TalkMsgSeqData * pData = reinterpret_cast<TalkMsgSeqData*>(p_work);

  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( pData->amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  SystemSingletones       *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  Work                    *p_script_work  = p_obj->GetWork();
  u16                     *seq            = p_script_work->GetSeq();
  FieldScriptSystem       *pSys           = p_gman->GetFieldScriptSystem();
  
  u8    win_id            = pData->win_id;        ///< ウィンドウID
  u32   EventID           = pData->EventID;       ///< キャラのイベントID(0で無効値扱い)
  s32   str_id            = pData->str_id;        ///< メッセージID
  u16   win_type          = pData->win_type;      ///< ウィンドウ位置タイプ r:/home/niji/git_program/niji_project/prog/AppLib/include/Tool/TalkWindowType.h
  u16   win_pos           = pData->win_pos;       ///< ウィンドウタイプ
  s32   win_pos_x         = pData->win_pos_x;     ///< ウィンドウ座標X
  s32   win_pos_y         = pData->win_pos_y;     ///< ウィンドウ座標Y
  s32   win_pos_z         = pData->win_pos_z;     ///< ウィンドウ座標Z
  s32   timer             = pData->timer;         ///< タイマー
  bool  b_msgbuf          = pData->b_msgbuf;      ///< メッセージバッファ
  //s32   option            = pData->option;    ///< オプション WIN_OPTION_XXX : field/script/ScriptMessageDefine.h
  
  
  gfl2::heap::HeapBase    *p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  Field::MoveModel::FieldMoveModel* pCharaModel = NULL;
  
  if( 0 < EventID )
  {
    Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
    if( pFieldMoveModelManager ){
      u32   chrID = pFieldMoveModelManager->GetFieldMoveModelIndexFromEventId( EventID );
      pCharaModel = pFieldMoveModelManager->GetFieldMoveModel( chrID );
    }
    if( pCharaModel==NULL){
      GFL_ASSERT_MSG(0,"EventID Error %d\n", EventID );
    } 
  }
 
  u32 msg_arc_id = p_script_work->GetMsgArcId();
  u32 msg_dat_id = p_script_work->GetMsgDatId();
  p_script_work->ClearExtraMsgInstant();

  enum
  {
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_END,
  };
  
  GFL_ASSERT( win_id < FIELDTALKWINDOW_MAX );
  
  switch (*seq)
  {
  case SEQ_INIT:
    {
      if( !p_swk->m_pFieldTalkWindow[ win_id ] )
      {
        p_swk->m_pFieldTalkWindow[ win_id ] = GFL_NEW(p_heap_dev) Field::FieldWindow::FieldTalkWindow( p_gman->GetFieldmap() );
        {
          App::Tool::TalkWindowType eWinType = static_cast<App::Tool::TalkWindowType>((win_type <= App::Tool::WIN_TYPE_GIZA_DOWN) ? win_type + (win_pos * App::Tool::WIN_TYPE_SYS_DOWN) : win_type);
          p_swk->m_pFieldTalkWindow[ win_id ]->SetWindowType( eWinType );
          switch( win_type ){
            case App::Tool::WIN_TYPE_GAIN_BOARD:
            case App::Tool::WIN_TYPE_NARUHODO_BOARD:
            case App::Tool::WIN_TYPE_NORMAL_BOARD:
              p_swk->m_pFieldTalkWindow[ win_id ]->SetMessageSpeed( Savedata::ConfigSave::MSG_NO_WAIT );
              break;
          }

          if( eWinType == App::Tool::WIN_TYPE_JOIN_FESTA )
          {
            Field::FieldGimmickJoinFesta*       p_jfGimmick   = static_cast<Field::FieldGimmickJoinFesta*>(p_gman->GetFieldmap()->GetFieldGimmick( Field::Fieldmap::GIMMICK_IDX_ZONE ));
            NetApp::JoinFesta::JFModelManager*  p_jfModelManager = p_jfGimmick->GetJFDynamicLoadSystem()->GetJFModelManager();

            u32 eventId = p_script_work->GetTempWorkValue( SCWK_TARGET_OBJID );

            NetAppLib::JoinFesta::JoinFestaPersonalData* jfPersonal = p_jfModelManager->GetJoinFestaPersonalData( eventId );
            p_swk->m_pFieldTalkWindow[ win_id ]->SetJoinFestaPlayerName( jfPersonal->GetName() );
          }
          else if (eWinType == App::Tool::WIN_TYPE_BATTLE_FES)
          {
            GameSys::GameData* pGameData = p_gman->GetGameData();
            Field::EventWork* ev = pGameData->GetEventWork();
            bool isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);
            Savedata::BattleFesSave* pBattleFesSave = pGameData->GetBattleFesSave(isEvent);
            u32 partnerId = 0;
            GFL_ASSERT(EventID >= BATTLE_FES_EVENT_ID_PARTNER1);
            GFL_ASSERT(EventID <= BATTLE_FES_EVENT_ID_PARTNER2);
            if (EventID == Field::BATTLE_FES_EVENT_ID_PARTNER2)
            {
              partnerId = 1;
            }
            gfl2::str::STRCODE name[System::STRLEN_PLAYER_NAME + System::EOM_LEN];
            pBattleFesSave->GetPartnerName(partnerId, name);
            p_swk->m_pFieldTalkWindow[win_id]->SetJoinFestaPlayerName(name);
          }
        }
        
        if( b_msgbuf ){
          p_swk->m_pFieldTalkWindow[ win_id ]->InitMsgData( p_swk->m_pMsgLoadWork->pMsgBuf, p_swk->m_pMsgLoadWork->arcID, p_swk->m_pMsgLoadWork->datID );
        }else{
          p_swk->m_pFieldTalkWindow[ win_id ]->InitMsgData( msg_arc_id, msg_dat_id );
        }
        p_swk->m_pFieldTalkWindow[ win_id ]->SetMessage( str_id );
        
        p_swk->m_pFieldTalkWindow[ win_id ]->SetWindowPos( win_pos_x, win_pos_y, win_pos_z );
        p_swk->m_pFieldTalkWindow[ win_id ]->SetTimer( timer );
        
        // タイマーが設定されていないウィンドウを出した場合はON
        if( timer <= 0 )
        {
          pSys->GetSystemWork()->OnCheckBit( SCR_END_CHECK_BIT_WIN_OPEN );
        }
        
        if( pCharaModel ){
          /// 会話前の口のパターンを記録.
          Field::MoveModel::FieldFacialAnimeMouth* pFacialAnimeMouth = pCharaModel->GetFacialAnimeMouth();
          if( pFacialAnimeMouth ) p_swk->m_pFieldTalkWindow[ win_id ]->SetMouseId( pFacialAnimeMouth->GetMouthIndex() );
        }
      }else{
        p_swk->m_pFieldTalkWindow[ win_id ]->NextMessage( str_id );
      }
      *seq = SEQ_WAIT;
    }
    //break throw;
  
  case SEQ_WAIT:
    {
      bool  bRes = p_swk->m_pFieldTalkWindow[ win_id ]->Update();
      
      Field::MoveModel::FieldFacialAnimeMouth* pFacialAnimeMouth = NULL;
      if( pCharaModel ) pFacialAnimeMouth = pCharaModel->GetFacialAnimeMouth();
//      GFL_ASSERT_MSG(pFacialAnimeMouth,"chr_id %dが不正です]n",EventID);
      
      if( pFacialAnimeMouth )
      {
        if( p_swk->m_pFieldTalkWindow[ win_id ]->GetStatus() == App::Tool::TalkWindow::WIN_STATUS_WIN_UPDATE )
        {
          if( pFacialAnimeMouth->CheckAutoAnime() != true )  pFacialAnimeMouth->SetAutoAnime( true );
        }else{
          if( pFacialAnimeMouth->CheckAutoAnime() != false )  pFacialAnimeMouth->SetAutoAnime( false );
        }
      }
      if( bRes )
      {
        if( pFacialAnimeMouth )
        {
          /// アニメは終了.
          pFacialAnimeMouth->SetAutoAnime( false );
          if( pCharaModel ){
            /// 会話前の口のパターンに戻す.
            Field::MoveModel::FieldFacialAnimeMouth* pAnimeMouth = pCharaModel->GetFacialAnimeMouth();
            poke_3d::model::CharaModel::MouthIndex  eMouseId = static_cast<poke_3d::model::CharaModel::MouthIndex>( p_swk->m_pFieldTalkWindow[ win_id ]->GetMouseId() );
            if( eMouseId == poke_3d::model::CharaModel::MOUTH_INDEX_NONE )  eMouseId = poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE;
            if( pAnimeMouth ) pAnimeMouth->ChangeAnime( eMouseId );
          }
        }
        *seq = 0;
        return true;
      }
    }
    break;
  }
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        TalkMdlMsg_Seq
 * @brief       TalkMdlMsg_Seq関数.
 * @author      N.Takeda
 * @date        2015/05/01(金) 15:05:27
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMsg_Seq(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *pSys           = p_gman->GetFieldScriptSystem();
  
  u8    win_id            = ptr[ 1 ];         ///< ウィンドウID
  u32   EventID           = ptr[ 2 ];         ///< キャラのイベントID(0で無効値扱い)
  s32   str_id            = ptr[ 3 ];         ///< メッセージID
  u16   win_type          = ptr[ 4 ];         ///< ウィンドウ位置タイプ r:/home/niji/git_program/niji_project/prog/AppLib/include/Tool/TalkWindowType.h
  u16   win_pos           = ptr[ 5 ];         ///< ウィンドウタイプ
  s32   win_pos_x         = ptr[ 6 ];         ///< ウィンドウ座標X
  s32   win_pos_y         = ptr[ 7 ];         ///< ウィンドウ座標Y
  s32   win_pos_z         = ptr[ 8 ];         ///< ウィンドウ座標Z
  s32   timer             = ptr[ 9 ];         ///< タイマー
  bool  b_msgbuf          = GFL_BOOL_CAST(ptr[ 10 ]);  ///< メッセージバッファ

  static TalkMsgSeqData s_TalkMsgSeqData; /// ワーク領域を確保 必ず１つしか動作しないので、これでよい。

  if( pSys->IsMsgUpdateFuncEnd() )
  {
    s_TalkMsgSeqData.win_id            = win_id;
    s_TalkMsgSeqData.EventID           = EventID;         ///< キャラのイベントID(0で無効値扱い)
    s_TalkMsgSeqData.str_id            = str_id;         ///< メッセージID
    s_TalkMsgSeqData.win_type          = win_type;         ///< ウィンドウ位置タイプ r:/home/niji/git_program/niji_project/prog/AppLib/include/Tool/TalkWindowType.h
    s_TalkMsgSeqData.win_pos           = win_pos;         ///< ウィンドウタイプ
    s_TalkMsgSeqData.win_pos_x         = win_pos_x;         ///< ウィンドウ座標X
    s_TalkMsgSeqData.win_pos_y         = win_pos_y;         ///< ウィンドウ座標Y
    s_TalkMsgSeqData.win_pos_z         = win_pos_z;         ///< ウィンドウ座標Z
    s_TalkMsgSeqData.timer             = timer;         ///< タイマー
    s_TalkMsgSeqData.b_msgbuf          = b_msgbuf;  ///< メッセージバッファ
    s_TalkMsgSeqData.amx               = amx;

    // メッセージアップデート設定
    pSys->SetMsgUpdateFunc( WaitFunc_TalkMsgSeq, 0, &s_TalkMsgSeqData );
  }
  else
  {
    GFL_ASSERT_STOP_MSG(0, "TalkMsg_Seq Overwrite\n");
  }
  pSys->Suspend();

  return false;
}

//------------------------------------------------------------------------------
/**
 * @func        TalkMdlMsg_SeqWait
 * @brief       TalkMdlMsg_SeqWait関数.
 * @author      takahashi
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      true : 完了  false : 途中
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMsg_SeqWait(AMX * amx, const cell *)
{
  ScriptObject            *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem       *pSys           = p_gman->GetFieldScriptSystem();

  return pSys->IsMsgUpdateFuncEnd();
}


//------------------------------------------------------------------------------
/**
 * @func        TalkMsg_VisubleCursor
 * @brief       会話ウィンドウのカーソル表示切替.
 * @author      N.Takeda
 * @date        2015/05/08(金) 20:56:03
 *
 * @param[in,out] AMX *         amx .
 * @param[in]     const cell *  ptr ポインタ.
 *
 * @return      cell AMX_NATIVE_CALL  
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::TalkMsg_VisubleCursor(AMX * amx, const cell * ptr)
{
  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  SystemSingletones       *p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  bool  bEnable   = GFL_BOOL_CAST(ptr[ 1 ]);             ///< 表示 / 非表示
  
  if( p_swk->m_pFieldTalkWindow[ 0 ] )                    ///< メッセージは 0番固定.
  {
    p_swk->m_pFieldTalkWindow[ 0 ]->SetMsgCursorUserVisible( bEnable );
  }
  
  return true;
}


//-----------------------------------------------------------------------------
/**
 *  @brief  メッセージウィンドウを閉じる(番号指定)
 */
//-----------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::MsgWinCloseNo(AMX * amx, const cell * ptr)
{
  FieldScriptSystem     *pSys           = FieldScriptSystem::GetInstance();
  ScriptObject          *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  Work                  *p_script_work  = p_obj->GetWork();
  
  u8    win_id            = ptr[ 1 ];         ///< ウィンドウID
  
  pSys->SetWaitFunc( amx, WaitFunc_MsgClose, win_id, p_script_work );
  pSys->Suspend();
  
  pSys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_WIN_OPEN );
  
  return false;
}


cell AMX_NATIVE_CALL FuncSetCommon::SetTimerIcon(AMX * amx, const cell * ptr )
{
  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  SystemSingletones       *p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  if( p_swk->m_pFieldTalkWindow[ 0 ] )                    ///< メッセージは 0番固定.
  {
    p_swk->m_pFieldTalkWindow[ 0 ]->SetTimerIconVisible( true );
  }

  return 0;
}

cell AMX_NATIVE_CALL FuncSetCommon::VanishTimerIcon(AMX * amx, const cell * ptr )
{
  FieldScriptSystem     *pSys           = FieldScriptSystem::GetInstance();

  ScriptObject            *p_obj  = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager    *p_gman = p_obj->GetGameManager();
  SystemSingletones       *p_swk  = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  if( p_swk->m_pFieldTalkWindow[ 0 ] )                    ///< メッセージは 0番固定.
  {
    p_swk->m_pFieldTalkWindow[ 0 ]->SetTimerIconVisible( false );

    pSys->SetWaitFunc( amx, WaitFunc_TimerIconClose, 0, p_swk->m_pFieldTalkWindow[ 0 ] );
    pSys->Suspend();
  }

  return 0;
}

//------------------------------------------------------------------------------
/**
 * @func        WaitFunc_TimerIconClose
 * @brief       タイマーアイコンのクローズチェック
 * @author      iwasawa
 * @date        2016/05/05
 */
//------------------------------------------------------------------------------
bool FuncSetCommon::WaitFunc_TimerIconClose( FieldScriptSystem* p_sys, u32 label, void* p_work )
{
  GameSys::GameManager  *p_gman     = p_sys->GetGameManager();

  Field::FieldWindow::FieldTalkWindow* pWindow = reinterpret_cast<Field::FieldWindow::FieldTalkWindow*>(p_work);

  return pWindow->IsTimerIconClose();
}

cell AMX_NATIVE_CALL FuncSetCommon::CheckWinAllSuspend(AMX * amx, const cell * ptr )
{
/*
  System* pSys = System::GetInstance();
  gamesystem::GameManager* p_gman = pSys->GetGameManager();
  Fieldmap* p_fieldmap = p_gman->GetFieldmap();
  MsgWin* p_msgwin = p_fieldmap->GetMsgWin();
  
  // すべてのウィンドウが待機状態かチェック
  if (p_msgwin->IsAllSuspend( false )) {
    return true;
  }
  return false;
*/
  return true;
}


/* ------------------------------------------------------------------------- */
/**
 * @brief メッセージウィンドウのデストラクタ
 *
 * @param p_gman ゲームマネージャへのポインタ
 *
 * @return true:終了, false:継続
 */
/* ------------------------------------------------------------------------- */
bool FuncSetCommon::SCREND_CheckEndWin( GameSys::GameManager* p_gman, s32* seq )
{
  FieldScriptSystem     *pSys           = p_gman->GetFieldScriptSystem();
  
  SystemSingletones     *p_swk          = pSys->GetSystemWork()->GetSingletones();
  
  switch( *seq )
  {
    case 0:
      /// チェックは 0番 メッセージのみ,
      if( p_swk && p_swk->m_pFieldTalkWindow[0] ) p_swk->m_pFieldTalkWindow[0]->CloseMsgWin();
      *seq = 1;
      return false;;
    
    case 1:
      if( p_swk ) GFL_SAFE_DELETE( p_swk->m_pFieldTalkWindow[0] );
      pSys->GetSystemWork()->OffCheckBit( SCR_END_CHECK_BIT_WIN_OPEN );
      break;
    default:
      break;
  }
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ListMenuInit_Seq
 * @brief       リストMenuInitSeq関数.
 * @author      N.Takeda
 * @date        2015/05/11(月) 15:45:46
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ListMenuInit_Seq(AMX * amx, const cell * ptr)
{
  ScriptObject          *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager  *p_gman         = p_obj->GetGameManager();
  FieldScriptSystem     *p_sys          = p_gman->GetFieldScriptSystem();
  SystemSingletones     *p_swk          = p_sys->GetSystemWork()->GetSingletones();
  Work                  *p_script_work  = p_obj->GetWork();
  u16                   *seq            = p_script_work->GetSeq();

  f32 posx = ptr[1];
  f32 posy = ptr[2];
  u32 curpos = ptr[3];
  bool is_cancel = GFL_BOOL_CAST(ptr[4]);
  bool is_right = GFL_BOOL_CAST(ptr[5]);
  bool b_msgbuf = GFL_BOOL_CAST(ptr[6]);
  
  
  gfl2::heap::HeapBase    *p_heap_dev = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE );
  
  u32 msg_arc_id = p_script_work->GetMsgArcId();
  u32 msg_dat_id = p_script_work->GetMsgDatId();
  gfl2::str::MsgData  *pMsgData = (b_msgbuf) ? reinterpret_cast<gfl2::str::MsgData*>(p_swk->m_pMsgLoadWork->pMsgBuf) : NULL;
  
  p_swk->m_pListMenu = GFL_NEW(p_heap_dev) Field::FieldWindow::FieldListMenuWindow( p_gman->GetFieldmap()->GetFieldWindowManager(),
    static_cast<s32>(posx), static_cast<s32>(posy), 
    msg_arc_id, msg_dat_id,
    curpos, is_cancel, is_right, pMsgData
  );
  p_swk->m_pListMenu->SetWordSet( p_sys->GetSystemWork()->GetWordSet() );
  *seq = 0;
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ListMenuAdd
 * @brief       リストMenuAdd関数.
 * @author      N.Takeda
 * @date        2015/05/11(月) 15:45:48
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ListMenuAdd(AMX * amx, const cell * ptr)
{
  ScriptObject          *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager  *p_gman         = p_obj->GetGameManager();
  SystemSingletones     *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();

  u32   str_id   = ptr[1];
  u32   sel_id   = ptr[2];
  //bool  b_msgbuf = ptr[3];
  //void* p_msgbuf = NULL;
  
  GFL_ASSERT( p_swk->m_pListMenu );
  if( !p_swk->m_pListMenu ) return true;
  
  p_swk->m_pListMenu->AddItem( str_id, sel_id );
  
  return true;
}


//------------------------------------------------------------------------------
/**
 * @func        ListMenuStart_Seq
 * @brief       リストMenuStartSeq関数.
 * @author      N.Takeda
 * @date        2015/05/11(月) 15:45:50
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::ListMenuStart_Seq(AMX * amx, const cell * ptr)
{
  ScriptObject          *p_obj          = ScriptObject::GetClass<ScriptObject*>( amx );
  GameSys::GameManager  *p_gman         = p_obj->GetGameManager();
  SystemSingletones     *p_swk          = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();
  Work                  *p_script_work  = p_obj->GetWork();
  u16                   *seq            = p_script_work->GetSeq();
  
  GFL_ASSERT( p_swk->m_pListMenu );
  if( !p_swk->m_pListMenu ) return true;
  
  enum
  {
    SEQ_INIT,
    SEQ_WAIT,
    SEQ_END,
  };
  
  switch (*seq)
  {
    case SEQ_INIT:
    {
      p_swk->m_pListMenu->Start();
      *seq = SEQ_WAIT;
      break;
    }
    
    case SEQ_WAIT:
    {
      if( p_swk->m_pListMenu->GetDecide() )
      {
        if( p_swk->m_pListMenu->GetSelectId() == App::Tool::ListMenu::SELECT_CANCEL )
        {
          p_script_work->SetTempWorkValue( SCWK_ANSWER, SCR_LISTMENU_CANCEL );
          p_gman->GetFieldmap()->GetFieldWindowManager()->SetLastResultYesNo( SCWK_ANSWER );
        }else{
          p_script_work->SetTempWorkValue( SCWK_ANSWER, p_swk->m_pListMenu->GetSelectId() );
          p_gman->GetFieldmap()->GetFieldWindowManager()->SetLastResultYesNo( p_swk->m_pListMenu->GetSelectId() );
        }
        p_swk->m_pListMenu->SetVisible( false );
        p_swk->m_pListMenu->DeleteMenu();
        *seq = SEQ_END;
      }
      break;
    }
    
    case SEQ_END:
    {
      /// Coverity.12229 無用な呼び出し の削除.
      if( p_swk->m_pListMenu->IsInstanceDelete() )
      {
        GFL_SAFE_DELETE( p_swk->m_pListMenu );
        *seq = 0;
        return true;
      }
      break;
    }
  }
  
  return false;
}


//------------------------------------------------------------------------------
/**
 * @func        DebugListMenuAdd_
 * @brief       リストMenuAdd関数.
 */
//------------------------------------------------------------------------------
cell AMX_NATIVE_CALL FuncSetCommon::DebugListMenuAdd_(AMX * amx, const cell * ptr)
{
#if PM_DEBUG
  ScriptObject          *p_obj = ScriptObject::GetClass<ScriptObject*>(amx);
  GameSys::GameManager  *p_gman = p_obj->GetGameManager();
  SystemSingletones     *p_swk = p_gman->GetFieldScriptSystem()->GetSystemWork()->GetSingletones();


  cell*                    text;          // 文字列アドレス取得用
  char                     item_text[512] = { 0 }; // 登録コード

  amx_GetAddr(amx, ptr[1], &text);                  // リスト登録名のアドレスを取得
  amx_GetString(&item_text[0], text, 0, sizeof(item_text)); // リスト登録名を保存
  u32   sel_id = ptr[2];

  GFL_ASSERT(p_swk->m_pListMenu);
  if (!p_swk->m_pListMenu) return true;

  p_swk->m_pListMenu->DebugAddItem( item_text , sel_id);

#endif
  return 0;
}

GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )
