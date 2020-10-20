

#include "print/include/PrintSystem.h"
#include "print/include/WordSet.h"
#include <Trainer/Trainer/include/TrainerMessage.h>

#include "btl_str.h"
#include "btl_mainmodule.h"
#include "btl_pokeparam.h"
#include "btl_UltraBeast.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>

#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

GFL_NAMESPACE_BEGIN(btl)
namespace str {



//------------------------------------------------------
/**
 *  WORDSETバッファ用途
 */
//------------------------------------------------------
typedef enum {

  BUFIDX_POKE_1ST = 0,
  BUFIDX_POKE_2ND,
  BUFIDX_TOKUSEI,

}WordBufID;

//------------------------------------------------------
/**
 *  同内容メッセージセットの分類
 */
//------------------------------------------------------
typedef enum {

  SETTYPE_MINE = 0, ///< 自分のポケ
  SETTYPE_WILD,     ///< 野生のポケ
  SETTYPE_ENEMY,    ///< 相手のポケ
  SETTYPE_NUSI,     ///< ぬしのポケ

  SETTYPE_MAX,

}SetStrFormat;

//------------------------------------------------------
/**
 *  文字列リソースID
 */
//------------------------------------------------------
typedef enum {
  MSGSRC_STD = 0,
  MSGSRC_SET,
  MSGSRC_ATK,
  MSGSRC_ATK_ZENRYOKU,
  MSGSRC_ZENRYOKU_WAZANAME,
  MSGSRC_UI,
  MSGDATA_MAX,
}MsgSrcID;

//------------------------------------------------------------------------------
/**
 *
 *  システムワーク
 *
 */
//------------------------------------------------------------------------------
typedef struct {

  const MainModule*  mainModule;               ///< メインモジュール
  const POKECON*     pokeCon;                  ///< ポケモンコンテナ
  print::WordSet*    wset;                     ///< WORDSET
  STRBUF*            SrcStrBuf;                ///< 展開前文字列の格納バッファ
  STRBUF*            tmpBuf;                   ///< 一時利用文字列バッファ
  gfl2::str::MsgData* msg[ MSGDATA_MAX ];       ///< メッセージデータハンドル
  void*              msgData[ MSGDATA_MAX ];   ///< メッセージデータ
  int                args[ BTL_STR_ARG_MAX ];  ///< 引数保存配列

  HeapHandle         heapHandle;           ///< ヒープハンドル
  u32                clientID;             ///< UIクライアントID
  u16                fIgnoreFormat;        ///< 対象による文字列フォーマット変更をオフ（常に自分用=接頭詞なし）
  u16                fForceTrueName;       ///< ポケモンニックネームタグで強制的に正体名を利用（外部gmm対応）

}SYSWORK;

SYSWORK  SysWork;

//-------------------------------------------------
//-------------------------------------------------
void            registerWords( const STRBUF* buf, const int* args );
const STRCODE*  seekNextTag( const STRCODE* sp, print::TagGroup* tagGroup, u16* tagIndex, u8* argIndex );
u8              searchPokeTagCount( const STRBUF* buf );
SetStrFormat    get_strFormat( u8 pokeID );
u16             get_setStrID( u8 pokeID, u16 defaultStrID );
u16             get_setStrID_Poke2( u8 pokeID1, u8 pokeID2, u16 defaultStrID );
u16             get_setPtnStrID( u8 pokeID, u16 originStrID, u8 ptnNum );
bool            isIgnoreStrFormat( u8 pokeID );

void mkstr_std_simple( STRBUF* dst, u32 strID, const int* args );
void mkstr_set_auto( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_default( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_poke2( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_rankup( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_rankup_at_once( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_rankup_item( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_rank_limit( STRBUF* dst, u16 strID, const int* args );
void mkstr_set_useitem( STRBUF* dst, u16 strID, const int* args );

//=============================================================================================
/**
 * 文字列生成システム初期化（バトル起動直後に１回だけ呼ぶ）
 */
//=============================================================================================
static const u16 msgDataID[] = {
  GARC_message_btl_std_DAT,
  GARC_message_btl_set_DAT,
  GARC_message_btl_attack_DAT,
  GARC_message_btl_attack_zenryoku_DAT,
  GARC_message_btl_zenryoku_wazaname_DAT,
  GARC_message_btl_ui_DAT,
};

void InitSystem( const MainModule* mainModule, u8 playerClientID, const POKECON* pokeCon, gfl2::heap::HeapBase *pHeap , bool isAsync)
{
  SysWork.mainModule = mainModule;
  SysWork.pokeCon = pokeCon;
  SysWork.heapHandle = pHeap;
  SysWork.clientID = playerClientID;
  if(mainModule)
  {
    SysWork.fIgnoreFormat = mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_NO_WILD_MESSAGE);
  }
  else
  {
    SysWork.fIgnoreFormat = true; //@fix 全力デモ対応
  }
  SysWork.fForceTrueName = false;

  SysWork.wset = GFL_NEW(pHeap) print::WordSet( pHeap );
  SysWork.wset->SetAreaWidthTagEnable( true );
  SysWork.SrcStrBuf = GFL_NEW(pHeap) STRBUF( BTL_STRBUF_STD_LENGTH, pHeap );
  SysWork.tmpBuf    = GFL_NEW(pHeap) STRBUF( BTL_STRBUF_STD_LENGTH, pHeap );

  if( isAsync )
  {
    for(u32 i=0; i<MSGDATA_MAX; i++)
    {
      SysWork.msg[i] = NULL;
      SysWork.msgData[i] = NULL;
    }
  }
  else
  {
    for(u32 i=0; i<MSGDATA_MAX; i++)
    {
      SysWork.msg[i] = GFL_NEW(pHeap) gfl2::str::MsgData( print::GetMessageArcId(), msgDataID[i], pHeap, gfl2::str::MsgData::LOAD_FULL );
      SysWork.msgData[i] = NULL;
    }
  }
}

//=============================================================================================
/**
 * メッセージの非同期読みセット
 */
//=============================================================================================
void StartLoadMessage( gfl2::fs::AsyncFileManager *readMng, gfl2::heap::HeapBase *dataHeap , gfl2::heap::HeapBase *workHeap)
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
  loadReq.arcId = print::GetMessageArcId();
  loadReq.align = 32;
  loadReq.heapForBuf = dataHeap;
  loadReq.heapForReq = dataHeap->GetLowerHandle();

  for(u32 i=0; i<MSGDATA_MAX; i++)
  {
    loadReq.datId = msgDataID[i];
    loadReq.ppBuf = &SysWork.msgData[i];
    readMng->AddArcFileLoadDataReq( loadReq );
  }
}

bool IsFinishLoadMessage(gfl2::fs::AsyncFileManager *readMng)
{
  for(u32 i=0; i<MSGDATA_MAX; i++)
  {
    if( readMng->IsArcFileLoadDataFinished( &SysWork.msgData[i] ) == false )
    {
      return false;
    }
  }
  return true;
}

void CreateMessage( gfl2::fs::AsyncFileManager *readMng, gfl2::heap::HeapBase *pHeap )
{
  for(u32 i=0; i<MSGDATA_MAX; i++)
  {
    SysWork.msg[i] = GFL_NEW(pHeap) gfl2::str::MsgData( SysWork.msgData[i], pHeap );
  }
}
//=============================================================================================
/**
 * 文字列生成システム終了（バトル終了時に１回だけ呼ぶ）
 */
//=============================================================================================
void QuitSystem( void )
{
  for(u32 i=0; i<MSGDATA_MAX; i++)
  {
    if( SysWork.msg[i] )
    {
      GFL_DELETE SysWork.msg[i];
      SysWork.msg[i] = NULL;
    }
    
    GflHeapSafeFreeMemory(SysWork.msgData[i]);
  }

  if( SysWork.tmpBuf ){
    GFL_DELETE  SysWork.tmpBuf;
    SysWork.tmpBuf = NULL;
  }
  if( SysWork.SrcStrBuf ){
    GFL_DELETE  SysWork.SrcStrBuf;
    SysWork.SrcStrBuf = NULL;
  }
  if( SysWork.wset ){
    GFL_DELETE  SysWork.wset;
    SysWork.wset = NULL;
  }
}


/**
 *  ポケモンのニックネーム名を登録（イリュージョン考慮し、見た目と一致したニックネーム）
 */
void register_PokeNickname( u8 pokeID, WordBufID bufID )
{
  const BTL_POKEPARAM* bpp;
  const PokePara* pp;

  GFL_ASSERT_STOP(SysWork.pokeCon);

  bpp = SysWork.pokeCon->GetPokeParamConst( pokeID );
  pp = bpp->GetViewSrcData();

  SysWork.wset->RegisterPokeNickName( (u32)(bufID), pp );
}
/**
 *  ポケモンのニックネーム名を登録（本当のニックネーム）
 */
void register_PokeNicknameTruth( u8 pokeID, WordBufID bufID )
{
  const BTL_POKEPARAM* bpp;
  const PokePara* pp;

  GFL_ASSERT_STOP(SysWork.pokeCon);

  bpp = SysWork.pokeCon->GetPokeParamConst( pokeID );
  pp = bpp->GetSrcData();

  SysWork.wset->RegisterPokeNickName( (u32)(bufID), pp );
}
/**
 *  ポケモンの種族名を登録
 */
void register_PokeName( u8 pokeID, u8 bufID )
{
  const BTL_POKEPARAM* bpp;
  const PokePara* pp;

  GFL_ASSERT_STOP(SysWork.pokeCon);

  bpp = SysWork.pokeCon->GetPokeParamConst( pokeID );
  pp = bpp->GetViewSrcData();

  // 対象が未発見ウルトラビーストなら、種族名を「？？？？？」に置き換える
  const MonsNo monsno = pp->GetMonsNo();
  if( UltraBeast::IsUnknownNamePokemon( *SysWork.mainModule, monsno ) )
  {
    SysWork.wset->RegisterPokeMonsNameMsgData( bufID, SysWork.msg[ MSGSRC_STD ], BTL_STRID_STD_UB_SecretName );
    return;
  }

  SysWork.wset->RegisterPokeMonsName( (u32)(bufID), pp );
}
/**
 *  トレーナータイプ名を登録
 */
void register_TrainerType( u8 bufIdx, u8 clientID )
{
  if( clientID == BTL_CLIENTID_NULL )
  {
    return;
  }

  if( SysWork.mainModule == NULL )
  {
    GFL_ASSERT(SysWork.mainModule);
    return ;
  }

  if( SysWork.mainModule->IsClientNPC(clientID) )
  {
    //NPCの場合
    const STRBUF* trtype_name;
    print::GrammaticalAttribute grammar;
    trtype_name = SysWork.mainModule->GetClientTrTypeName( clientID, &grammar );

    SysWork.wset->RegisterWord( bufIdx, *trtype_name,
            grammar,
            print::WordSet::FORM_SINGLE
    );
  }
  else {
    //プレイヤーの場合
    u16 trType = SysWork.mainModule->GetClientTrainerType( clientID );
    SysWork.wset->RegisterTrTypeName( bufIdx, trType );
  }
}
/**
 *  トレーナー名を登録
 */
void register_TrainerName( u8 bufIdx, u8 clientID )
{
  if( clientID != BTL_CLIENTID_NULL )
  {
  	GFL_ASSERT_STOP(SysWork.mainModule);

    if( !SysWork.mainModule->IsClientTrainerExist(clientID) ){
      GFL_ASSERT_MSG(0, "not exist trainer... clientID=%d", clientID);
      return;
    }
    if( SysWork.mainModule->IsClientNPC(clientID) )
    {
      const STRBUF* name;
      print::GrammaticalAttribute grammar;
      name = SysWork.mainModule->GetClientTrainerName( clientID, &grammar );
      BTL_PRINT("クライアント %d はＮＰＣ name=%p\n", clientID, name );

      SysWork.wset->RegisterWord( bufIdx, *name,
              grammar,
              print::WordSet::FORM_SINGLE
      );
    }
    else{
      if (SysWork.mainModule->GetSetupStatusFlag(BTL_STATUS_FLAG_BATTLE_FES) && clientID == 0)
      {
        //バトルフェス例外対応
        //自分側のトレーナーは入れ替わる。先頭のポケモンのPokeIDで何番目か分かる
        u8 pokeId = SysWork.pokeCon->GetFrontPokeDataConst(BTL_POS_1ST_0)->GetID();
        const Savedata::MyStatus* status = SysWork.mainModule->GetBattleFesPlayerData(pokeId);

        BTL_PRINT("クライアント %d はプレイヤー(Fes), mystatus=%p\n", clientID, status);
        SysWork.wset->RegisterPlayerName(bufIdx, status);
      }
      else{
        const Savedata::MyStatus* status = SysWork.mainModule->GetClientPlayerData(clientID);
        BTL_PRINT("クライアント %d はプレイヤー, mystatus=%p\n", clientID, status);

        SysWork.wset->RegisterPlayerName(bufIdx, status);
      }
    }
  }
}
/**
 *  トレーナータイプ名とトレーナー名を結合したものをバッファ typeAndNameBufIdx に登録
 *  同時に、バッファ trainerNameBufIdx にトレーナー名が登録される
 */
void register_TrainerTypeAndTrainerName( u8 typeAndNameBufIdx, u8 trainerNameBufIdx, u8 clientID )
{
  if( clientID == BTL_CLIENTID_NULL )
  {
    return;
  }

  // トレーナータイプ名とトレーナー名の一時バッファ、兼トレーナー名設定
  register_TrainerType( typeAndNameBufIdx, clientID );
  register_TrainerName( trainerNameBufIdx, clientID );
  // トレーナータイプ名とトレーナー名を結合したものを登録
  SysWork.wset->RegisterTrTypeNameAndTrainerName( typeAndNameBufIdx, typeAndNameBufIdx, trainerNameBufIdx );
}

//=============================================================================================
/**
 * 標準メッセージの生成
 * ※標準メッセージ：対象者なし、あるいは対象が誰であっても一定のフォーマットで生成される文字列
 *
 * @param   buf       [out] 生成文字列格納バッファ
 * @param   strID     文字列ID
 * @param   numArgs   可変個引数の数
 * @param   ...       引数（可変）
 *
 */
//=============================================================================================
void MakeStringStd( STRBUF* buf, u32 strID, u32 numArgs, ... )
{
  GFL_ASSERT(numArgs<=BTL_STR_ARG_MAX);
  {
    va_list   list;
    u32 i;

    va_start( list, numArgs );
    for(i=0; i<numArgs; ++i)
    {
      SysWork.args[i] = va_arg( list, int );
    }
    va_end( list );

    // ゴミ除去ねんのため
    while( i < GFL_NELEMS(SysWork.args) ){
      SysWork.args[i++] = 0;
    }

    MakeStringStdWithArgArray( buf, strID, SysWork.args );
  }
}
//=============================================================================================
/**
 * 標準メッセージの生成（引数配列渡し版）
 *
 * @param   buf
 * @param   strID
 * @param   args
 *
 */
//=============================================================================================
void MakeStringStdWithArgArray( STRBUF* buf, u32 strID, const int* args )
{
  /**
   *  対象陣営に応じてIDを補正する文字列群
   */
  static const u16 sideConvStrID[] = {
    BTL_STRID_STD_Makibisi,
    BTL_STRID_STD_MakibisiOff,
    BTL_STRID_STD_Dokubisi,
    BTL_STRID_STD_DokubisiOff,
    BTL_STRID_STD_StealthRock,
    BTL_STRID_STD_StealthRockOff,
    BTL_STRID_STD_NebaNebaNet,
    BTL_STRID_STD_NebaNebaNetOff,
    BTL_STRID_STD_WideGuard,
    BTL_STRID_STD_FastGuard,
    BTL_STRID_STD_TrickGuard,
    BTL_STRID_STD_Kinchoukan,
    BTL_STRID_STD_SinpiNoMamori,
    BTL_STRID_STD_SinpiNoMamoriOff,
    BTL_STRID_STD_HikariNoKabe,
    BTL_STRID_STD_HikariNoKabeOff,
    BTL_STRID_STD_Reflector,
    BTL_STRID_STD_ReflectorOff,
    BTL_STRID_STD_AuroraVeil,
    BTL_STRID_STD_AuroraVeilOff,
    BTL_STRID_STD_SiroiKiri,
    BTL_STRID_STD_SiroiKiriOff,
    BTL_STRID_STD_Oikaze,
    BTL_STRID_STD_OikazeOff,
    BTL_STRID_STD_Omajinai,
    BTL_STRID_STD_OmajinaiOff,
    BTL_STRID_STD_Rainbow,
    BTL_STRID_STD_RainbowOff,
    BTL_STRID_STD_Burning,
    BTL_STRID_STD_BurningOff,
    BTL_STRID_STD_Moor,
    BTL_STRID_STD_MoorOff,
  };
  int i;

  BTL_N_Printf( DBGSTR_STR_StdStrID, strID);

  // 対象陣営による補正：対象陣営ID（BtlSide）は args[0] に入れておくこと
  for(i=0; i<GFL_NELEMS(sideConvStrID); ++i)
  {
    if( strID == sideConvStrID[i] )
    {
  	  GFL_ASSERT_STOP(SysWork.mainModule);
      if( !SysWork.mainModule->IsPlayerSide(args[0]) ){
        ++strID;
      }
      break;
    }
  }

  mkstr_std_simple( buf, strID, args );
}
/**
 *  標準メッセージ作成コア
 */
void mkstr_std_simple( STRBUF* dst, u32 strID, const int* args )
{
  SysWork.msg[ MSGSRC_STD ]->GetString( strID, *(SysWork.SrcStrBuf) );

  //ローカライズ専用分岐。タッグトレーナーの対応
  //@attention タッグ用メッセージの場合トレーナータイプは専用のものになり、クライアントから取得できない。
  //           そのため独自展開処理をする
  if( strID == BTL_STRID_STD_Encount_Tag ||
      strID == BTL_STRID_STD_WinTrainer_Tag )
  {
    //引数はトレーナータイプ・クライアントID・クライアントIDで来る
    SysWork.wset->RegisterTrTypeName(0,args[0]);
    register_TrainerName(1,args[1]);
    register_TrainerName(2,args[2]);

    const STRCODE* sp = SysWork.SrcStrBuf->GetPtr();
    const STRCODE eomCode = gfl2::str::EOM_CODE;
    const STRCODE tagCode = gfl2::str::TAG_START_CODE;
    while( *sp != eomCode )
    {
      if( *sp == tagCode )
      {
        if( print::IsWordSetTagGroup(sp) )
        {
          u8  tagGrp = print::GetTagGroup( sp );
          if( tagGrp != print::TAGGROUP_NUMBER )
          {
            u8 tagIdx = print::GetTagIndex( sp );
            if( tagIdx == print::TAGIDX_WORD_TRAINER_TYPE_AND_NAME )
            {
              //トレーナータイプ＆名前タグを使っていた場合はうわがき
              // トレーナータイプ名とトレーナー名を結合したものを登録
              SysWork.wset->RegisterTrTypeNameAndTrainerName( 0, 0, 1 );
            }
          }
        }
        sp = gfl2::str::SkipTagCode( sp );
      }
      else
      {
        ++sp;
      }
    }

  }
  else
  {
    registerWords( SysWork.SrcStrBuf, args );
  }

  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );

}

//----------------------------------------------------------------------------------
/**
 * 文字列中のタブIDから必要な情報を判別してWORDSETに登録する
 *
 * @param   buf
 * @param   args
 * @param   wset
 */
//----------------------------------------------------------------------------------
void registerWords( const STRBUF* buf, const int* args )
{
  const STRCODE* sp;
  u8  argIdxDec[print::WordSet::DEFAULT_WORD_MAX];
  const STRCODE eomCode = gfl2::str::EOM_CODE;
  const STRCODE tagCode = gfl2::str::TAG_START_CODE;

  // タグを走査して、bufIdxから減算すべき値argIdxDecのテーブルを作成。
  // タグが bufIdx の順に出現するとは限らないため（特に、言語によってはトレーナータイプ名がトレーナー名よりも後に来る点が挙げられる）。
  for ( int i = 0; i < GFL_NELEMS(argIdxDec); i ++ )
  {
    argIdxDec[i] = 0;
  }
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u8  tagGrp = print::GetTagGroup( sp );

        if( tagGrp == print::TAGGROUP_WORD )
        {
          u8 tagIdx = print::GetTagIndex( sp );
          switch( tagIdx ){
          case print::TAGIDX_WORD_TRAINER_TYPE:
          case print::TAGIDX_WORD_TRAINER_TYPE_AND_NAME:
            // [Name:TrainerType ] の bufIdx より大きい bufIdx を持つWORDは、取得するargIdxを1減らす
            for ( int i = bufIdx+1; i < GFL_NELEMS(argIdxDec); i ++ )
            {
              argIdxDec[i] ++;
            }
            break;
          default:
            break;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else {
      ++sp;
    }
  }

  // 文字列中に出現するタグに対応するWORDをWordSetに設定
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u32 argIdx = bufIdx;
        u8  tagGrp = print::GetTagGroup( sp );
        if( argIdx >= argIdxDec[bufIdx] ){
          argIdx -= argIdxDec[bufIdx];
        }
        BTL_N_Printf( DBGSTR_PRINT_BufIdx, bufIdx);

        if( tagGrp == print::TAGGROUP_NUMBER )
        {
          u8 keta = print::GetTagIndex( sp ) + 1;
          SysWork.wset->RegisterNumber( bufIdx, args[argIdx], keta, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
        }
        else
        {
          u8 tagIdx = print::GetTagIndex( sp );

          BTL_PRINT("[BTLSTR] TagIdx=%d\n", tagIdx);

          switch( tagIdx ){
          case print::TAGIDX_WORD_TRAINER_TYPE_AND_NAME:
            {
              u8 clientID = args[ argIdx ];
              BTL_PRINT(" ->ClientID:%dのトレーナータイプ＋トレーナー名をBuf[%d]にセット\n",clientID,bufIdx);
              BTL_PRINT(" ->ClientID:%dのトレーナー名をBuf[%d]にセット\n",clientID,bufIdx+1);
              register_TrainerTypeAndTrainerName( bufIdx, bufIdx+1, clientID );
            }
            break;
          case print::TAGIDX_WORD_TRAINER_TYPE:
            {
              u8 clientID = args[ argIdx ];
              BTL_PRINT(" ->ClientID:%dのトレーナータイプをBuf[%d]にセット\n",clientID,bufIdx);
              register_TrainerType( bufIdx, clientID );
            }
            break;
          case print::TAGIDX_WORD_TRAINER_NAME:
            {
              u8 clientID = args[argIdx];
              BTL_PRINT(" ->ClientID:%dのトレーナー名をBuf[%d]にセット\n",clientID,bufIdx);
              register_TrainerName( bufIdx, clientID );
            }
            break;
          case print::TAGIDX_WORD_POKE_NICKNAME:
            BTL_N_Printf( DBGSTR_STR_SetPokeNickname, args[argIdx]);
            if( !(SysWork.fForceTrueName) ){
              register_PokeNickname( args[argIdx], static_cast<WordBufID>(bufIdx) );
            }else{
              register_PokeNicknameTruth( args[argIdx], static_cast<WordBufID>(bufIdx) );
            }
            break;
          case print::TAGIDX_WORD_POKE_NICKNAME_TRUTH:
            BTL_N_Printf( DBGSTR_STR_SetPokeNicknameTruth, args[argIdx]);
            register_PokeNicknameTruth( args[argIdx], static_cast<WordBufID>(bufIdx) );
            break;
          case print::TAGIDX_WORD_POKE_NAME:
            register_PokeName( args[argIdx], static_cast<WordBufID>(bufIdx) );
            break;
          case print::TAGIDX_WORD_POKE_TYPE:
            SysWork.wset->RegisterPokeTypeName( bufIdx, args[argIdx] );
            break;
          case print::TAGIDX_WORD_TOKUSEI_NAME:
            SysWork.wset->RegisterTokuseiName( bufIdx, args[argIdx] );
            break;
          case print::TAGIDX_WORD_WAZA_NAME:
            BTL_N_Printf( DBGSTR_STR_SetTagWazaName, args[argIdx] );
            SysWork.wset->RegisterWazaName( bufIdx, args[argIdx] );
            break;
          case print::TAGIDX_WORD_ZWAZA_NAME:
            SysWork.wset->RegisterWazaName( bufIdx, args[argIdx] );
            break;
          case print::TAGIDX_WORD_ITEM_NAME:
            BTL_N_Printf( DBGSTR_STR_SetItemName, argIdx, args[argIdx]);
            SysWork.wset->RegisterItemName( bufIdx, args[argIdx] , 1 );
            break;
          case print::TAGIDX_WORD_POKETYPE_NAME:
            SysWork.wset->RegisterPokeTypeName( bufIdx, args[argIdx] );
            break;
          default:
            GFL_ASSERT_MSG(0, "unknown tagIdx = %d\n", tagIdx);
            break;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else{
      ++sp;
    }
  }
}

//加工無しのStdを受け取る(WordSet展開もされていません
void MakeStringStdPlane(STRBUF* buf, u32 strID)
{
  SysWork.msg[MSGSRC_STD]->GetString(strID, *buf);
}

//=============================================================================================
/**
 * セットメッセージの生成
 * ※セットメッセージ：自分側，敵側（やせいの），敵側（あいての）, 敵側（ぬしの）が一定の法則でセットになった文字列
 *
 * @param[out]   buf
 * @param[in]    strID
 * @param[in]    args
 *
 */
//=============================================================================================
void MakeStringSet( STRBUF* buf, u32 strID, const int* args )
{
  static const struct {
    u32   strID;
    void  (* func)( STRBUF*, u16, const int* );
  }funcTbl[] = {
    { BTL_STRID_SET_Rankup_ATK,               mkstr_set_rankup         },
    { BTL_STRID_SET_Rankdown_ATK,             mkstr_set_rankup         },
    { BTL_STRID_SET_RankupMax_ATK,            mkstr_set_rank_limit     },
    { BTL_STRID_SET_RankdownMin_ATK,          mkstr_set_rank_limit     },
    { BTL_STRID_SET_UseItem_Rankup_ATK,       mkstr_set_rankup_item    },
    { BTL_STRID_STD_UseItem_Self,             mkstr_set_useitem        },
    { BTL_STRID_SET_AuraEffect_Rankup_ATK,    mkstr_set_rankup         },
    { BTL_STRID_SET_AuraEffect_Rankup_Multi5, mkstr_set_rankup_at_once },
                                           
    { BTL_STRID_SET_Tedasuke,              mkstr_set_default          },  // てだすけはポケモン名２体入り文字列だが標準（３セット）
    { BTL_STRID_SET_JikoAnji,              mkstr_set_default          },  // じこあんじも同様
    { BTL_STRID_SET_MeromeroAct,           mkstr_set_default          },  // メロメロ
    { BTL_STRID_SET_SideChange,            mkstr_set_default          },  // サイドチェンジ
    { BTL_STRID_SET_AffGood_2,             mkstr_set_default          },  // 効果バツグン２体同時
    { BTL_STRID_SET_AffGood_3,             mkstr_set_default          },  // 効果バツグン３体同時
    { BTL_STRID_SET_AffBad_2,              mkstr_set_default          },  // 効果イマイチ２体同時
    { BTL_STRID_SET_AffBad_3,              mkstr_set_default          },  // 効果イマイチ３体同時
    { BTL_STRID_SET_MegaEvo,               mkstr_set_default          },  // メガ進化した！
    { BTL_STRID_SET_Omitoosi,              mkstr_set_default          },  // おみとおし
    { BTL_STRID_SET_Kyousei,               mkstr_set_default          },  // きょうせい
                                           
    //@バトル変更箇所                      
    { BTL_STRID_SET_MegaRayquazaEvo,       mkstr_set_default          },  // レックウザ専用メガシンカ後メッセージ(タグは2個だが通常)
  };

  BTL_N_Printf(DBGSTR_STR_SetStrID, strID);

  for(int i=0; i<GFL_NELEMS(funcTbl); i++)
  {
    if( funcTbl[i].strID == strID )
    {
      funcTbl[i].func( buf, strID, args );
      return;
    }
  }


  mkstr_set_auto( buf, strID, args );
}

//--------------------------------------------------------------
/**
 *  セット文字列生成（標準処理）：
 *  args[0]〜[1] にポケモンIDが入っているのを自動判別して処理を振り分け
 */
//--------------------------------------------------------------
void mkstr_set_auto( STRBUF* dst, u16 strID, const int* args )
{
  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );

  if( searchPokeTagCount(SysWork.SrcStrBuf) == 2 ){
    mkstr_set_poke2( dst, strID, args );
  }else{
    mkstr_set_default( dst, strID, args );
  }
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（標準処理）：（args[0] にポケモンIDが入っている）
 */
//--------------------------------------------------------------
void mkstr_set_default( STRBUF* dst, u16 strID, const int* args )
{
  BTL_N_Printf( DBGSTR_STR_SetPoke1, strID, args[0], args[1]);

  strID = get_setStrID( args[0], strID );

  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  registerWords( SysWork.SrcStrBuf, args );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（標準処理）：（args[0], args[1] にポケモンIDが入っている）
 */
//--------------------------------------------------------------
void mkstr_set_poke2( STRBUF* dst, u16 strID, const int* args )
{
  BTL_N_Printf( DBGSTR_STR_SetPoke2, strID, args[0], args[1]);

  strID = get_setStrID_Poke2( args[0], args[1], strID );

  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  registerWords( SysWork.SrcStrBuf, args );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（ランク増減）：○○の××が（ぐーんと）あがった／さがった！
 *  args... [0]:pokeID,  [1]:statusType, [2]:volume
 */
//--------------------------------------------------------------
void mkstr_set_rankup( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[1] - pml::wazadata::RANKEFF_ORIGIN;
  u16 leap = 0;
  if( args[2] >= 3 ){ leap = 2; }
  if( args[2] == 2 ){ leap = 1; }

  strID += (leap * SETTYPE_MAX * pml::wazadata::RANKEFF_NUMS);

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  strID = get_setPtnStrID( args[0], strID, statusType );
  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（ランク増減）：○○の　のうりょくが（ぐーんと）あがった／さがった！
 *  args... [0]:pokeID,  [1]:volume
 */
//--------------------------------------------------------------
void mkstr_set_rankup_at_once( STRBUF* dst, u16 strID, const int* args )
{
  u16 leap = 0;
  if( args[1] >= 3 ){ leap = 2; }
  if( args[1] == 2 ){ leap = 1; }

  strID += ( leap * SETTYPE_MAX ) + get_strFormat( args[0] );

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（アイテムによるランク増減）： ○○は△△で××が（ぐーんと）あがった！
 *  args... [0]:pokeID,  [1]:itemID,  [2]:statusType,  [3]:volume
 */
//--------------------------------------------------------------
void mkstr_set_rankup_item( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[2] - pml::wazadata::RANKEFF_ORIGIN;

  u16 leap = 0;
  if( args[3] >= 3 ){ leap = 2; }
  if( args[3] == 2 ){ leap = 1; }

  strID += (leap * SETTYPE_MAX * pml::wazadata::RANKEFF_NUMS);
  strID = get_setPtnStrID( args[0], strID, statusType );

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  SysWork.wset->RegisterItemName( 1, args[1] , 1 );

  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（ランク増減失敗）： ○○の××は　もうあがらない／さがらない！
 *  args... [0]:pokeID,  [1]:statusType
 */
//--------------------------------------------------------------
void mkstr_set_rank_limit( STRBUF* dst, u16 strID, const int* args )
{
  u8 statusType = args[1] - pml::wazadata::RANKEFF_ORIGIN;

  register_PokeNickname( args[0], BUFIDX_POKE_1ST );
  strID = get_setPtnStrID( args[0], strID, statusType );
  SysWork.msg[MSGSRC_SET]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}
//--------------------------------------------------------------
/**
 *  セット文字列生成（アイテム使用）：
 */
//--------------------------------------------------------------
void mkstr_set_useitem( STRBUF* dst, u16 strID, const int* args )
{
  u8 clientID = args[0];

  GFL_ASSERT_STOP(SysWork.mainModule);

  if( SysWork.mainModule->GetPlayerClientID() != clientID )
  {
    if( SysWork.mainModule->IsClientNPC(clientID) ){
      strID = BTL_STRID_STD_UseItem_NPC;
    }else{
      strID = BTL_STRID_STD_UseItem_Player;
    }
  }
  else{
    strID = BTL_STRID_STD_UseItem_Self;
  }

  SysWork.msg[MSGSRC_STD]->GetString( strID, *SysWork.SrcStrBuf );
  registerWords( SysWork.SrcStrBuf, args );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}

//----------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * ポケモンIDから、「自分の・野生の・相手の・ぬしの」いずれのパターンか判定
 *
 * @param   pokeID
 *
 * @retval  SetStrFormat
 */
//--------------------------------------------------------------------------
SetStrFormat get_strFormat( u8 pokeID )
{
  if( isIgnoreStrFormat( pokeID ) == false )
  {
    GFL_ASSERT_STOP(SysWork.pokeCon);
    GFL_ASSERT_STOP(SysWork.mainModule);

    const u8 targetClientID = MainModule::PokeIDtoClientID( pokeID );
    const BTL_POKEPARAM* targetPoke = SysWork.pokeCon->GetPokeParamConst( pokeID );

    if( SysWork.mainModule->IsOpponentClientID(SysWork.clientID, targetClientID) )
    {
      if( SysWork.mainModule->GetCompetitor() == BTL_COMPETITOR_WILD )
      {
        // ぬし戦なら、最初から場に居るポケモンを「ぬし」とする
        if( ( SysWork.mainModule->GetSetupStatusFlag( BTL_STATUS_FLAG_VS_NUSI ) ) &&
           !( targetPoke->PERMFLAG_Get( BTL_POKEPARAM::PERMFLAG_INTRUDE_IN ) ) )
        {
          return SETTYPE_NUSI;
        }

        return SETTYPE_WILD;
      }
      else
      {
        return SETTYPE_ENEMY;
      }
    }
  }
  return SETTYPE_MINE;
}

/**
 * @brief メッセージの「野生の・相手の」などの所属ワード表示を行うか？
 * @param pokeID  チェック対象ポケモンのID
 * @retval true   所属ワードを表示しない
 * @retval false  所属ワードを表示する
 */
bool isIgnoreStrFormat( u8 pokeID )
{
  if( SysWork.fIgnoreFormat )
  {
    return true;
  }

  // 名前を「？？？？？」と表示するポケモンは、所属ワードを表示しない
  {
    const BTL_POKEPARAM* poke = SysWork.pokeCon->GetPokeParamConst( pokeID );
    const MonsNo monsno = static_cast<MonsNo>( poke->GetMonsNo() );
    if( UltraBeast::IsUnknownNamePokemon( *(SysWork.mainModule), monsno ) )
    {
      return true;
    }
  }

  return false;
}

/**
 *  セット文字列ID変換（自分の・野生の・相手の・ぬしの　４パターン）
 */
u16 get_setStrID( u8 pokeID, u16 defaultStrID )
{
  return defaultStrID + get_strFormat( pokeID );
}
/**
 *  セット文字列ID変換（自分→自分、自分→野生、野生→自分など、ポケ名２つを含む９パターン）
 *
 * 文字列の定義は、次の順番に並んでいる必要があります。
 * 自分 → 自分
 * 自分 → 野生 ( 文字列ラベル _2Y  )
 * 自分 → 相手 ( 文字列ラベル _2E  )
 * 自分 → ぬし ( 文字列ラベル _2N  )
 * 野生 → 自分 ( 文字列ラベル _Y   )
 * 野生 → 野生 ( 文字列ラベル _Y2Y )
 * 相手 → 自分 ( 文字列ラベル _E   )
 * 相手 → 相手 ( 文字列ラベル _E2E )
 * ぬし → 自分 ( 文字列ラベル _N   )
 * ぬし → ぬし ( 文字列ラベル _N2N )
 */
u16 get_setStrID_Poke2( u8 pokeID1, u8 pokeID2, u16 defaultStrID )
{
  SetStrFormat  fmt = get_strFormat( pokeID1 );

  switch( fmt ){
  case SETTYPE_MINE:
    return defaultStrID + static_cast<u16>(get_strFormat( pokeID2 ));
  case SETTYPE_WILD:
    defaultStrID += static_cast<u16>(SETTYPE_MAX);
    break;
  case SETTYPE_ENEMY:
    defaultStrID += static_cast<u16>(SETTYPE_MAX + 2);
    break;
  case SETTYPE_NUSI:
    defaultStrID += static_cast<u16>(SETTYPE_MAX + 2 + 2);
    break;
  }
  if( get_strFormat(pokeID2) == fmt ){
    defaultStrID++;
  }
  return defaultStrID;
}
/**
 *  セット文字列ID変換（パターン数指定）
 */
u16 get_setPtnStrID( u8 pokeID, u16 originStrID, u8 ptnNum )
{
  return originStrID + (ptnNum * SETTYPE_MAX) + get_strFormat( pokeID );
}

/**
 * 文字列中に含まれるポケモン名タグの数を返す
 *
 * @param[in]   buf 文字列バッファ
 *
 * @retval      u8  ポケモン名タグの数
 */
u8 searchPokeTagCount( const STRBUF* buf )
{
  const STRCODE* sp = buf->GetPtr();

  print::TagGroup tagGroup;
  u16 tagIdx;
  u8  argIdx;
  u8 count = 0;

  while( sp )
  {
    sp = seekNextTag( sp, &tagGroup, &tagIdx, &argIdx );
    if( sp )
    {
      if( tagGroup == print::TAGGROUP_WORD )
      {
        if( (tagIdx == print::TAGIDX_WORD_POKE_NAME)
        ||  (tagIdx == print::TAGIDX_WORD_POKE_NICKNAME)
        ||  (tagIdx == print::TAGIDX_WORD_POKE_NICKNAME_TRUTH)
        ){
          ++count;
        }
      }
    }
  }
  return count;
}
/**
 * 次のタグ位置まで文字列ポインタを移動
 *
 * @param[in]    sp
 * @param[out]   tagGroup
 * @param[out]   tagIndex
 * @param[out]   argIndex
 *
 * @retval  const STRCODE*    移動後のポインタ
 */
const STRCODE* seekNextTag( const STRCODE* sp, print::TagGroup* tagGroup, u16* tagIndex, u8* argIndex )
{
  if( sp != NULL )
  {
    const gfl2::str::STRCODE EOM_Code = gfl2::str::EOM_CODE;
    const gfl2::str::STRCODE TAG_Code = gfl2::str::TAG_START_CODE;

    while( (*sp) != EOM_Code )
    {
        if( *sp == TAG_Code )
        {
          if( print::IsWordSetTagGroup(sp) )
          {
            *argIndex = static_cast<u8>(print::GetTagParam( sp, 0 ));
            *tagGroup = print::GetTagGroup( sp );
            *tagIndex = print::GetTagIndex( sp );
            return gfl2::str::SkipTagCode( sp );
          }
          else{
            sp = gfl2::str::SkipTagCode( sp );
          }
        }
        else{
          ++sp;
        }
    }
    return NULL;
  }
  return NULL;
}


//=============================================================================================
/**
 * ワザメッセージの生成
 * ※ワザメッセージ：○○の××こうげき！とか。セットメッセージと同様、必ず３つセット。
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 * @param[in]    args
 */
//=============================================================================================
void MakeStringWaza( STRBUF* dst, u8 pokeID, u16 waza )
{
  u16 strID = get_setStrID( pokeID, waza * SETTYPE_MAX );

  register_PokeNickname( pokeID, BUFIDX_POKE_1ST );
  SysWork.msg[MSGSRC_ATK]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}

//=============================================================================================
/**
 * 全力技のワザメッセージの生成
 * ※全力技を使用した時の「○○の××ブースト！」など
 * ※自分の、野生の、相手の、ぬしの　の４つセット
 * ※ローカライズ要望により、ワードセットで解決せずに、技ごとにテキストを用意
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 * @param[in     args
 *
 */
//=============================================================================================
void MakeStringZenryokuWaza( STRBUF* dst, u8 pokeID, u16 waza )
{
  u16 strID = get_setStrID( pokeID, waza * SETTYPE_MAX );

  register_PokeNickname( pokeID, BUFIDX_POKE_1ST );
  SysWork.msg[MSGSRC_ATK_ZENRYOKU]->GetString( strID, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
}

//=============================================================================================
/**
 * @brief 全力技の名前を取得する
 * @param[out] dst   全力技名の格納先
 * @param[in]  waza  全力技の技No.
 */
//=============================================================================================
void GetZenryokuWazaName( STRBUF* dst, u16 waza )
{
  u16 strID = waza;
  SysWork.msg[MSGSRC_ZENRYOKU_WAZANAME]->GetString( strID, *dst );
}

//=============================================================================================
/**
 * UI用文字列取得
 *
 * @param[out]   buf    生成テキスト格納バッファ
 * @param[in]    strID
 *
 */
//=============================================================================================
void GetUIString( STRBUF* dst, u16 strID )
{
  SysWork.msg[MSGSRC_UI]->GetString( strID, *dst );
}

//=============================================================================================
/**
 * ワザ選択時UI用文字列生成
 *
 * @param[out]   dst
 * @param[in]    wazaID
 * @param[in]    wazaPP
 * @param[in]    wazaPPMax
 */
//=============================================================================================
void MakeWazaUIString( STRBUF* dst, u16 wazaID, u8 wazaPP, u8 wazaPPMax )
{
  GFL_ASSERT_MSG(0,"未使用！\n");
  /*
  SysWork.wset->RegisterWazaName( 0, wazaID );
  SysWork.wset->RegisterNumber( 1, wazaPP, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );
  SysWork.wset->RegisterNumber( 2, wazaPPMax, 2, print::NUM_DISP_ZERO, print::NUM_CODE_DEFAULT );

  SysWork.msg[MSGSRC_UI]->GetString( BTLMSG_UI_SEL_WAZA, *SysWork.SrcStrBuf );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
  */
 }

//=============================================================================================
/**
 * ２択メッセージの生成
 *
 * @param[out]   dst      生成先バッファ
 * @param[in]    strID
 * @param[in]    args
 *
 */
//=============================================================================================
void MakeStringYesNoWithArgArray( STRBUF* dst, u32 strID, const int* args )
{
  GFL_ASSERT_MSG(0,"メッセージ未初期化です。\n");
  /*
  SysWork.msg[MSGSRC_YESNO]->GetString( strID, *SysWork.SrcStrBuf );
  registerWords( SysWork.SrcStrBuf, args );
  SysWork.wset->Expand( dst, SysWork.SrcStrBuf );
  */
}

//=============================================================================================
/**
 * レベルアップウィンドウテキスト（前レベルとの差分）生成
 *
 * @param[out]   dst    生成先バッファ
 * @param[in]    hp
 * @param[in]    atk
 * @param[in]    def
 * @param[in]    sp_atk
 * @param[in]    sp_def
 * @param[in]    agi
 */
//=============================================================================================
void MakeString_LvupInfo_Diff( print::WordSet* wset, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi )
{
  wset->RegisterNumber( 0, hp,     2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 1, atk,    2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 2, def,    2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 3, sp_atk, 2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 4, sp_def, 2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 5, agi,    2,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
}

//=============================================================================================
/**
 * レベルアップウィンドウテキスト（新レベルパラメータ）生成
 *
 * @param[out]   dst    生成先バッファ
 * @param[in]    hp
 * @param[in]    atk
 * @param[in]    def
 * @param[in]    sp_atk
 * @param[in]    sp_def
 * @param[in]    agi
 */
//=============================================================================================
void MakeString_LvupInfo_Param( print::WordSet* wset, u32 hp, u32 atk, u32 def, u32 sp_atk, u32 sp_def, u32 agi )
{
  wset->RegisterNumber( 0, hp,     3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 1, atk,    3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 2, def,    3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 3, sp_atk, 3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 4, sp_def, 3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->RegisterNumber( 5, agi,    3,  print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
}

//=============================================================================================
/**
 * トレーナーメッセージの生成
 *
 * @param[out]   buf        生成文字列格納バッファ
 * @param[in]    trainerID  トレーナーID
 * @param[in]    msgID      メッセージ種類
 *
 */
//=============================================================================================
void MakeStringTrainer( STRBUF* buf, trainer::TrainerID trainerID, trainer::MessageKind msgID)
{
  trainer::TrainerMessage::GetString( SysWork.SrcStrBuf, trainerID, msgID, SysWork.heapHandle );
  register_TrainerName( 0 , BTL_CLIENT_PLAYER );

  // EFIGS文法タグの展開のためExpand
  SysWork.wset->Expand( buf, SysWork.SrcStrBuf );
}

//=============================================================================================
/**
 * タグの展開
 *
 * @param[out]   dst        生成文字列格納バッファ
 * @param[in]    src        展開前の文字列
 *
 */
//=============================================================================================
extern void SetStringExpanded( STRBUF* dst, const STRBUF* src)
{
  // EFIGS文法タグの展開のためExpand
  SysWork.wset->Expand( dst, src );
}

//=============================================================================================
/**
 * @brief ニックネームを、未発見ウルトラビースト用の文字列に変更する
 *
 * @param[out] poke  ニックネーム変更対象のポケモン
 */
//=============================================================================================
extern void SetUnknownUBNickName( HeapHandle heap, pml::pokepara::PokemonParam* poke )
{
  gfl2::str::StrBuf str_buf( BTL_STRBUF_STD_LENGTH, heap->GetLowerHandle() );
  gfl2::str::MsgData msg_data( print::GetMessageArcId(), GARC_message_btl_std_DAT, heap->GetLowerHandle() );
  msg_data.GetString( BTL_STRID_STD_UB_SecretName, str_buf );
  poke->SetNickName( &str_buf );
}


} // end of namespace 'str'
GFL_NAMESPACE_END(btl)

