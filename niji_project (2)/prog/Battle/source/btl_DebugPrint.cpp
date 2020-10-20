
#include <str/include/gfl2_StrSys.h>

#include "btl_DebugPrint.h"
#include "btl_ui.h"

GFL_NAMESPACE_BEGIN(btl)

static u32  gPrintBit = 0;
static int  gCurrent = 0;
static u32  gPrintType = BTL_PRINTTYPE_UNKNOWN;
static bool PrintSysEnableFlag = false;

/**
 *  ファイル名 - 識別シンボル名変換テーブル
 */
static const struct {
  char* longName;
  char* shortName;
}FileNames[] = {
  { "btl_mainmodule.cpp",      "MAI" },
  { "btl_server.cpp",          "SVR" },
  { "btl_client.cpp",          "CLI" },
  { "btl_adapter.cpp",         "ADP" },
  { "btl_str.cpp",             "STR" },
  { "btl_net.cpp",             "NET" },
  { "btl_calc.cpp",            "CAL" },
  { "btl_sick.cpp",            "SIC" },
  { "btl_event.cpp",           "EVE" },
  { "btl_pokeparam.cpp",       "BPP" },
  { "btl_pokeset.cpp",         "SET" },
  { "btl_pospoke.cpp",         "POS" },
  { "btl_server_cmd.cpp",      "CMD" },
  { "btl_serverflow.cpp",      "SVF" },
  { "btl_serverflowsub.cpp",   "SUB" },
  { "btl_hem.cpp",             "HEM" },
  { "btl_field.cpp",           "FLD" },
  { "btl_rec.cpp",             "REC" },
  { "btlv_core.cpp",           "VIW" },
  { "btlv_scu.cpp",            "scU" },
  { "btlv_scd.cpp",            "scD" },
  { NULL,                      "OTR" },
};

static u32 filename_to_index( const char* filename )
{
  // armcc は filename がフルパスで来るみたいなのでファイル名の先頭まで移動
  {
    enum {
      BUF_SIZE = 40,
    };
    static char buf[ BUF_SIZE ];

    const char* p = filename;
    const char* last_sep = NULL;
    while(*p != '\0'){
      if( *p == '\\' || *p == '/' ){
        last_sep = p;
      }
      ++p;
    }
    if( last_sep ){
      filename = ++last_sep;
    }

    // さらに小文字で統一
    u32 c;
    for(c=0, p=filename; *p!='\0' && c<(BUF_SIZE-1); ++p, ++c)
    {
      buf[ c ] = *p;
      if( buf[c] >= 'A' && buf[c] <= 'Z' ){
        buf[c] = 'a' + (buf[c] - 'A');
      }
    }
    buf[ c ] = '\0';
    filename = buf;
  }

  u32 i;
  for(i=0; FileNames[i].longName!=NULL; ++i)
  {
    if( gfl2::str::StrComp_J(FileNames[i].longName, filename) )
    {
      return i;
    }
  }
  return i;
}

static void print_file_info( const char* filename, int line )
{
  u32 idx = filename_to_index( filename );
  NN_LOG( "[%s-%4d]", FileNames[idx].shortName, line );
}

static bool  printCtrl_isEnable( u32 idx )
{
  return (gPrintBit & (1<<idx)) != 0;
}

static bool printCtrl_isEnableFile( const char* filename )
{
  u32 idx = filename_to_index( filename );;
  return printCtrl_isEnable( idx );
}



static void print_type( void )
{
  switch( gPrintType ){
  case BTL_PRINTTYPE_SERVER:      NN_LOG("SV/"); break;
  case BTL_PRINTTYPE_CLIENT:      NN_LOG("CL/"); break;
  case BTL_PRINTTYPE_STANDALONE:  NN_LOG("SA/"); break;
  case BTL_PRINTTYPE_UNKNOWN:
  default:
    NN_LOG("UN/");
    break;
  }
}

#ifdef BTL_PRINT_SYSTEM_ENABLE
static void printCtrl_AllOn( void )
{
  gPrintBit = 0;
  for(u32 i=0; i<GFL_NELEMS(FileNames); ++i)
  {
    gPrintBit |= (1 << i);
  }
}
static void printCtrl_putCur( void )
{
  NN_LOG("[PrintCTRL]  %2d [%s] = ", gCurrent, FileNames[gCurrent].shortName );
  if( printCtrl_isEnable(gCurrent) ){
    NN_LOG("ON\n");
  }else{
    NN_LOG("OFF\n");
  }

}
void BTL_DEBUGPRINT_Ctrl( void )
{
  if( ui::CheckKeyCont(BUTTON_L) )
  {
    if( ui::CheckKeyTrg(DIR_UP) ){
      if( --gCurrent < 0 ){
        gCurrent = GFL_NELEMS(FileNames) - 1;
      }
      printCtrl_putCur();
    }
    else if( ui::CheckKeyTrg(DIR_DOWN) ){
      if( ++gCurrent >= (GFL_NELEMS(FileNames)) ){
        gCurrent = 0;
      }
      printCtrl_putCur();
    }
    else if( ui::CheckKeyTrg(DIR_LEFT) ){
      gPrintBit &= ~( 1 << gCurrent);
      printCtrl_putCur();
    }
    else if( ui::CheckKeyTrg(DIR_RIGHT) ){
      gPrintBit |= ( 1 << gCurrent);
      printCtrl_putCur();
    }
    else if( ui::CheckKeyTrg(BUTTON_R) )
    {
      if( gPrintBit == 0 ){
        printCtrl_AllOn();
        NN_LOG("[PrintCTRL] All ON\n");
      }else{
        gPrintBit = 0;
        NN_LOG("[PrintCTRL] All OFF\n");
      }
    }
  }
}
#endif

//=============================================================================================
/**
 * プリント時のヘッダタイプ通知を受け取り
 *
 * @param   type
 */
//=============================================================================================
void BTL_DEBUGPRINT_SetType( BtlPrintType type )
{
  gPrintType = type;
  gPrintBit = 0;
}
const char* BTL_DEBUGPRINT_GetFormatStr( BtlDebugStrID strID )
{
  switch( strID ){
  case DBGSTR_SETUP_DONE:                    return "セットアップ完了\n";
  case DBGSTR_DEBUGFLAG_BIT:                 return "デバッグフラグbit=%04x\n";
  case DBGSTR_ADAPTER_SEND_START:            return " Adapter-%d, コマンド %d を送信開始します\n";
  case DBGSTR_ADAPTER_RECV_DONE:             return " [Adapter] Received from All Clients\n";
  case DBGSTR_ADAPTER_RECVED_A_CLIENT:       return " [Adapter] Received from AdapterID=%d\n";
  case DBGSTR_ADAPTER_Create:                return "アダプタ(%d）作成：通信対応フラグ=%d\n";

  case DBGSTR_MAIN_PokeConGetByPos:          return "存在しない位置(pos=%d, clientID=%d, idx=%d)のポケモンデータを参照した\n";
  case DBGSTR_MAIN_PartyDataNotifyComplete:  return "パーティデータ相互受信できました。\n";
  case DBGSTR_MAIN_AIPartyDataSendComplete:  return "AIパーティデータ受信完了\n";
  case DBGSTR_MAIN_SwapPartyMember:          return "パーティメンバー入れ替え %d <-> %d\n";
  case DBGSTR_MAIN_RecvedMultiAITrainer:     return "ClientID=%d, trID=%d set\n";
  case DBGSTR_MAIN_SendAIPartyStart:         return "AIクライアント数=%d, パーティデータ送受信開始\n";
  case DBGSTR_MAIN_SendAIPartyParam:         return "AI Client=%d パーティ送信完了（ポケ数=%d\n";
  case DBGSTR_MAIN_GetFrontPokeData:         return "戦闘位置[%d] のポケモン... ";
  case DBGSTR_MAIN_GetFrontPokeDataResult:   return "    クライアント[%d]の %d 番目のポケを返す\n";
  case DBGSTR_MAIN_CommError:                return "通信エラー発生 line=%d\n";
  case DBGSTR_MAIN_CheckHPByLvup:            return "PPLVUP Main line:%d, HP=%d\n";
  case DBGSTR_MAIN_PerappVoiceAdded:         return "ClientID=%d のペラップデータ構築\n";
  case DBGSTR_MAIN_PerappVoiceComplete:      return "ペラップボイス相互受信完了\n";
  case DBGSTR_MAIN_Illusion1st:              return "%d番目にいるイリュージョン持ちポケモン[%d]の参照ポケを\n";
  case DBGSTR_MAIN_Illusion2nd:              return " %d番目のポケ[%d]に変更\n";
  case DBGSTR_MAIN_MultiAITrainer_SeqStart:  return "AIマルチ用トレーナーデータ送受信(idx:%d) 開始\n";
  case DBGSTR_MAIN_MultiAITrainer_SendDone:  return "AIマルチ用トレーナーデータ (ClientID=%d) 送信完了\n";
  case DBGSTR_MAIN_CheckResultStart:         return "*** 勝敗チェック ***\n";
  case DBGSTR_MAIN_Result_CommError:         return "  通信エラー\n";
  case DBGSTR_MAIN_Result_Capture:           return "  捕獲終了\n";
  case DBGSTR_MAIN_Result_Escape:            return "  逃げて終了\n";
  case DBGSTR_MAIN_Result_ServerCalc:        return "  サーバ計算結果コード=%d\n";

  case DBGSTR_CLIENT_RETURN_CMD_START:       return "ID[%d], 返信開始へ\n";
  case DBGSTR_CLIENT_RETURN_CMD_DONE:        return "ID[%d], %d byte 返信しました\n";
  case DBGSTR_CLIENT_SelectActionSkip:       return "Act選択(%d体目）スキップ\n";
  case DBGSTR_CLIENT_SelectActionRoot:       return "Act選択（%d体目）開始 .. pokeID=%d, actAdrs=%p\n";
  case DBGSTR_CLIENT_SelActPokeDead:         return "死んでてスキップなのでCheckedCntそのまま\n";
  case DBGSTR_CLIENT_SelectActionStart:      return "Act選択(%d体目=ID:%d）開始します  checked %d poke\n";
  case DBGSTR_CLIENT_SelectActionDone:       return "カバー位置数(%d)終了、アクション送信へ\n";
  case DBGSTR_CLIENT_SelectActionBacktoRoot: return "カバー位置未達、次のポケ(Idx=%d）アクション選択ルートへ\n";
  case DBGSTR_CLIENT_SelectAction_Pokemon:   return "「ポケモン」を選んだ->選択画面へ\n";
  case DBGSTR_CLIENT_SelectAction_Fight:     return "「たたかう」を選んだ->選択画面へ\n";
  case DBGSTR_CLIENT_SelectChangePoke:       return " ポケモン選んだ ... idx=%d\n";
  case DBGSTR_CLIENT_SelectChangePokeCancel: return " ポケモン選ばなかった\n";
  case DBGSTR_CLIENT_ReturnSeqDone:       return "返信シーケンス終了\n";
  case DBGSTR_CLIENT_WazaLockInfo:        return "ワザロック：Client[%d] 前回使ったワザは %d, targetPos=%d\n";
  case DBGSTR_CLIENT_NoMorePuttablePoke:  return "myID=%d もう戦えるポケモンいない\n";
  case DBGSTR_CLIENT_NotDeadMember:       return "myID=%d 誰も死んでないから選ぶ必要なし\n";
  case DBGSTR_CLIENT_ChangePokeCmdInfo:   return "myID=%d %d体選択する必要あり（先頭位置=%d） mode=%d\n";
  case DBGSTR_CLIENT_NumChangePokeBegin:  return " 全Client, 選択すべきポケモン数=%d\n　位置=";
  case DBGSTR_CLIENT_NumChangePokeResult: return " 自分[%d]が選択すべきポケモン数=%d\n";
  case DBGSTR_CLIENT_PokeSelCnt:          return "入れ替え%d体選んだ\n";
  case DBGSTR_CLIENT_PokeChangeIdx:       return "ポケモン入れ替え %d体目 <-> %d体目\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk:     return "逃げ交換禁止チェック「かげふみ」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable:  return "  「かげふみ」有効！\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk:    return "逃げ交換禁止チェック「ありじごく」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable: return "  「ありじごく」有効！\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk:      return "逃げ交換禁止チェック「じりょく」by pokeID(%d)\n";
  case DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable:   return "  「じりょく」有効！\n";
  case DBGSTR_CLIENT_UpdateWazaProcResult:          return "ワザプロセス情報更新 : PokeID=%d, OrgWaza=%d, ActWaza=%d, ActWazaType=%d, ActTargetPos=%d, ActEnable=%d\n";
  case DBGSTR_CLIENT_CmdLimitTimeEnable:   return "コマンド選択制限時間が有効 -> %d sec\n";
  case DBGSTR_CLIENT_CmdLimitTimeOver:     return "コマンド選択タイムアップ ... 強制終了フラグON\n";
  case DBGSTR_CLIENT_ForcePokeChange:      return "強制入れ替え %d/%d PosIdx=%d, MemberIdx=%d\n";
  case DBGSTR_CLIENT_UpdateEnemyBaseHP:    return "相手ポケ基準HP値を更新 ->%d  (Poke=%d)\n";
  case DBGSTR_CLIENT_ForceQuitByTimeLimit: return "時間制限による強制終了(ClientID=%d)\n";
  case DBGSTR_CLIENT_StartCmd:             return "Client(%d), AdaptCmd(%d) 実行開始\n";
  case DBGSTR_CLIENT_RecvedQuitCmd:        return "Client(%d), バトル終了コマンド受信\n";
  case DBGSTR_CLIENT_ReplyToQuitCmd:       return "Client(%d), バトル終了コマンドに対し返信完了\n";
  case DBGSTR_CLIENT_StartRotAct:          return "ローテーション動作開始 clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_EndRotAct:            return "ローテーション動作終了 clientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_ReadRecAct:           return "録画データ読み込み clientID=%d, Action count=%d\n";
  case DBGSTR_CLIENT_ReadRecTimerOver:     return "録画再生タイムオーバー検出 myClientID=%d\n";
  case DBGSTR_CLIENT_ReadRecError:         return "録画再生エラー検出 myClientID=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Fight:     return "   action=Fight, waza=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Change:    return "   action=Change, nextPokeIdx=%d\n";
  case DBGSTR_CLIENT_ReadRecAct_Move:      return "   action=Move!\n";
  case DBGSTR_CLIENT_UnknownServerCmd:     return "用意されていないコマンドNo[%d]！\n";
  case DBGSTR_CLIENT_RecPlayerBlackOut:    return "録画再生コントローラ：ブラックアウト\n";
  case DBGSTR_CLIENT_RecPlay_ChapterSkipped: return "client(%d), 指定チャプタ[%d]に到達した..\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokStart:    return "受けとくせい所持による交換チェック ... wazaType=%d\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokFind:     return " %d 番目がtype=%dを受けられるポケモンだ\n";
  case DBGSTR_CLIENT_CHGAI_UkeTokDecide:   return "受けるための交替確定\n";
  case DBGSTR_CLIENT_CHGAI_PreSortList:    return " 入れ替え対象ソート前 ";
  case DBGSTR_CLIENT_CHGAI_AfterSortList:  return " 入れ替え対象ソート後 ";
  case DBGSTR_CLIENT_CHGAI_SortListVal:    return "Idx(%d)=Pow(%d), ";
  case DBGSTR_CLIENT_CHGAI_HOROBI:         return "入れ替えたい：ほろびのうた\n";
  case DBGSTR_CLIENT_CHGAI_FusigiNaMamori: return "入れ替えたい：ふしぎなまもり\n";
  case DBGSTR_CLIENT_CHGAI_NoEffWaza:      return "入れ替えたい：相性無効\n";
  case DBGSTR_CLIENT_CHGAI_Kodawari:       return "入れ替えたい：こだわり系\n";
  case DBGSTR_CLIENT_CHGAI_UkeTok:         return "入れ替えたい：受けとくせい\n";
  case DBGSTR_CLIENT_CHGAI_SizenKaifuku:   return "入れ替えたい：しぜんかいふく\n";
  case DBGSTR_CLIENT_CHGAI_WazaEff:        return "入れ替えたい：ワザ相性による\n";
  case DBGSTR_CLIENT_ROT_R:                return "ローテ右回転  対象ポケ=%d\n";
  case DBGSTR_CLIENT_ROT_L:                return "ローテ左回転  対象ポケ=%d\n";
  case DBGSTR_CLIENT_ROT_Determine:        return "ローテ対象ポケ、ワザ有効 dir=%d, Idx=%d, pokeID=%d\n";
  case DBGSTR_CLIENT_AI_PutPokeStart:      return "myID=%d 戦闘ポケが死んで %d体選択\n";
  case DBGSTR_CLIENT_AI_PutPokeDecide:     return "  -> %d番目を新たに出す\n";
  case DBGSTR_CLIENT_CoverRotateDecide:    return "空きを埋めるローテーション確定 ClientID=%d, dir=%d\n";
  case DBGSTR_CLIENT_HPCheckByLvup:        return "PPLVUP Client line:%d, pokeID=%d, HP=%d\n";
  case DBGSTR_CLIENT_OP_HPPlus:            return "ポケモンHP回復 : bpp=%p, pokeID=%d, AddHP=%d, ResultHP=%d\n";
  case DBGSTR_CLIENT_OP_MemberIn:          return "メンバー入場 : pos %d<->%d  inPokeID=%d, inPokeHP=%d\n";
  case DBGSTR_CLIENT_StartWazaTargetSel:   return "対象選択開始へ\n";
  case DBGSTR_CLIENT_MoveAction:           return "ムーブ位置 pos(%d<->%d) = vpos(%d<->%d)\n";
  case DBGSTR_CLIENT_StartWazaEffect:      return "ワザエフェクト再生：WazaID=%d, TurnNum=%d, pos(%d -> %d)\n";

  case DBGSTR_PSET_Copy:          return "ポケセットコピー  adrs=%p, count=%d\n";
  case DBGSTR_PSET_Add:           return "ポケセット追加    adrs=%p, pokeID=%d, count=%d/%d\n";
  case DBGSTR_PSET_Remove:        return "ポケセット削除    adrs=%p, pokeID=%d, count=%d/%d\n";
  case DBGSTR_FIDLD_FuinCheck:    return "ふういんポケチェック %d 体目 : ふういんポケ=%d, 攻撃ポケ=%d, チェックワザ=%d\n";
  case DBGSTR_FIELD_FuinHit:      return "ふういんポケ(ID=%d)が、ワザ:%dを持っていた！\n";

  case DBGSTR_BPP_NemuriWakeCheck:   return "ポケ[%d]のねむりターン最大値=%d, 経過ターン=%d\n";
  case DBGSTR_BPP_WazaLinkOff:       return "ワザ書き換え、リンク切れます（新ワザナンバー=%d）\n";
  case DBGSTR_BPP_WazaRecord:        return "ワザ記録(PokeID=%d)：選んだワザ=%d, 派生したワザ=%d, 狙った位置=%d,\n  ワザタイプ=%d, 前のワザ=%d, 今回成功flg=%d, counter=%d\n";
  case DBGSTR_BPP_BatonTouch:        return "[%d]->[%d]へバトンタッチで引き継がれた:攻撃ランク=%d, 防御ランク=%d\n";

  case DBGSTR_VCORE_PokeListStart:      return "交換リスト画面 モード=%d すでに%d体選択, 今回選択=%d\n";
  case DBGSTR_VCORE_SelPokeEnd:         return "ポケ選択おわった\n";
  case DBGSTR_VCORE_SelPokeEnd_Sel:     return "  [%d]番目 選択された -> Idx=%d, (%d体 格納)\n";
  case DBGSTR_VCORE_SelPokeEnd_Unsel:   return "  [%d]番目 選択されていない\n";
  case DBGSTR_VCORE_PluralDamageInfo:   return "複数体ダメージ処理 (%d体) wazaID=%d\n";
  case DBGSTR_VCORE_PluralDamagePoke:   return "  対象ポケID=%d\n";

  case DBGSTR_NET_Startup:              return "自分のネットID=%d, 接続メンバー数=%d\n";
  case DBGSTR_NET_DecideClientID:       return "NetID:%d -> clientID=%d\n";
  case DBGSTR_NET_SendSucceed:          return "  送信成功\n";
  case DBGSTR_NET_RecvedServerParam:    return "netID=%d, サーバパラメータ受信しました。\n";
  case DBGSTR_NET_RecvedPartyData:      return "netID=%d, clientID=%d のパーティデータ受信完了, pData=%p, buf=%p\n";
  case DBGSTR_NET_PartyDataComplete:    return "パーティデータ相互受信完了  member=%d\n";
  case DBGSTR_NET_PerappVoiceComplete:  return "ペラップボイス相互受信完了  member=%d\n";
  case DBGSTR_NET_SendCmdDone:          return "マシン(%d) に %d bytes 送信完了\n";
  case DBGSTR_NET_RecvedAllClientsData: return "全クライアントからのデータ返ってきた\n";
  case DBGSTR_NET_ReturnToServerTrying: return "Try Return to Server %d byte ...";
  case DBGSTR_NET_RecvedClientData:     return "[BTLNET] recv from netID[%d], size=%d\n";
  case DBGSTR_NET_RecvedServerVersion:  return "サーババージョン受信 ... netID=%d, version=%d, clientID=%d\n";
  case DBGSTR_NET_ServerDetermine:      return "サーバは netID=%d のマシンに決定\n";
  case DBGSTR_NET_SendAITrainerData:    return "AIトレーナーデータ送信完了, trID=%d\n";
  case DBGSTR_NET_RecvAITrainerData:    return "AIトレーナーデータ受信完了, trID=%d\n";
  case DBGSTR_NET_AIPartyInfo:          return "Client(%d)=AIパーティと判断する (pokeCnt=%d)\n  monsno=";
  case DBGSTR_NET_CreateAIPartyRecvBuffer: return "AIパーティデータ受信バッファを生成\n";
  case DBGSTR_NET_RecvedAIPartyData:       return "AIパーティデータ受信 clientID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufDisable:  return "ペラップボイス受信バッファ構築（無効データ）netID=%d\n";
  case DBGSTR_NET_PrappVoiceGetBufEnable:   return "ペラップボイス受信バッファ構築（有効データ）netID=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedEnable:  return "ペラップボイス受信完了（有効データ）netID=%d, size=%d\n";
  case DBGSTR_NET_PerappVoiceRecvedDisable: return "ペラップボイス受信完了（無効データ）netID=%d, empty=%d\n";
  case DBGSTR_NET_PerappVoiceCheckRaw:      return "ペラップボイス受信チェック: ClientID:%d = netID:%d\n";

  case DBGSTR_HEM_Push:     return "[STATE PUSH -%5d] << sp=%d, rp=%d    \n";
  case DBGSTR_HEM_Pop:      return "[STATE POP  -%5d]    sp=%d, rp=%d >> \n";
  case DBGSTR_HEM_PushWork: return "[work PUSH]: type=%d, pokeID=%d, size=%3d, sp=%d\n";
  case DBGSTR_HEM_PophWork: return "[work  POP]: type=%d, pokeID=%d, size=%3d, sp=%d, myPos=%d,\n";

  case DBGSTR_CALCDMG_WazaParam:        return "ワザ情報：ID=%d, Type=%d\n";
  case DBGSTR_CALCDMG_BaseDamage:       return "基礎ダメージ値 (%d)\n";
  case DBGSTR_CALCDMG_RangeHosei:       return "対象数によるダメージ補正:%d  (ratio=%08x)\n";
  case DBGSTR_CALCDMG_WeatherHosei:     return "天候による補正が発生, 補正率=%08x, dmg=%d->%d\n";
  case DBGSTR_CALCDMG_RAW_DAMAGE:       return "威力:%d, Lv:%d, こうげき:%d, ぼうぎょ:%d,  ... 素ダメ:%d\n";
  case DBGSTR_CALCDMG_Critical:         return "クリティカルだから素ダメ->%d\n";
  case DBGSTR_CALCDMG_RandomHosei:      return "ランダム補正:%d%%  -> 素ダメ=%d\n";
  case DBGSTR_CALCDMG_TypeMatchHosei:   return "タイプ一致補正:%d%%  -> 素ダメ=%d\n";
  case DBGSTR_CALCDMG_TypeAffInfo:      return "タイプ相性:%02d -> ダメージ値：%d\n";
  case DBGSTR_CALCDMG_DamageResult:     return "ダメ受けポケモン=%d, ratio=%08x, Damage=%d -> %d\n";
  case DBGSTR_CALCDMG_DamageMarume:     return "ダメ受けポケモンのＨＰ値にまるめ->%d\n";
  case DBGSTR_STR_StdStrID:             return " STD:strID=%d\n";
  case DBGSTR_STR_SetStrID:             return " SET:strID=%d\n";
  case DBGSTR_STR_StdStrInfo:           return "STD STR ID=%d, argCnt=%d\n";
  case DBGSTR_STR_Arg:                  return "  arg(%d)=%d\n";
  case DBGSTR_STR_SetPoke1:             return "ms set_poke1 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_STR_SetPoke2:             return "ms set_poke2 strID=%d, args[0]=%d, args[1]=%d\n";
  case DBGSTR_PRINT_BufIdx:             return "bufIdx=%d ....\n";
  case DBGSTR_STR_TagSetNumber:         return "[TAG] SetNumber : keta=%d, value=%d\n";
  case DBGSTR_STR_SetPokeNickname:      return "setPokeNickName ID=%d \n";
  case DBGSTR_STR_SetPokeNicknameTruth: return "setPokeNickName Truth ID=%d\n";
  case DBGSTR_STR_SetTagWazaName:       return "[TAG] Set WazaName ... waza=%d\n";
  case DBGSTR_STR_SetItemName:          return "set Item Name argIdx=%d, ID=%d ....\n";
  case DBGSTR_SVFL_ActOrderStart:       return ">>>> ActProc [開始] ポケ[ID:%d = monsno:%d] (ptr=%p)\n";
  case DBGSTR_SVFL_ActOrderEnd:         return "<<<< ActProc [終了] ポケ[ID:%d = monsno:%d] (ptr=%p)\n";
  case DBGSTR_SVFL_ActOrder_Fight:      return "【たたかう】ポケ[%d]のワザ[%d]を、位置[%d]の相手に\n";
  case DBGSTR_SVFL_ActOrder_Item:       return "【どうぐ】を処理。アイテム%dを、%d番の相手に。\n";
  case DBGSTR_SVFL_ActOrder_Change:     return "【ポケモン】を処理。位置%d <- ポケ%d \n";
  case DBGSTR_SVFL_ActOrder_Escape:     return "【にげる】を処理。\n";
  case DBGSTR_SVFL_ActOrder_Dead:       return "【ひんし】なので何もせず\n";
  case DBGSTR_SVFL_ActOrder_Rotation:   return "【ローテーション】を処理  ClientID=%d, 回転方向=%d\n";
  case DBGSTR_SVFL_CorrectTarget_Info:  return "攻撃ポケ[%d]（位置=%d） -> 狙ったポケ[%d]（位置=%d）死んでいるので補正する\n";
  case DBGSTR_SVFL_CorrectHitFarOn:     return "ワザ[%d] 遠隔ON の補正対象決定\n";
  case DBGSTR_SVFL_CorrectHitFarOff:    return "ワザ[%d] 遠隔OFF の補正対象決定\n";
  case DBGSTR_SVFL_CorrectTargetNum:    return "補正対象数  %d / %d \n";
  case DBGSTR_SVFL_CorrectTargetDiff:   return "対象候補の距離 Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectTargetHP:     return "対象候補の残HP Poke[%d]->%d, Poke[%d]->%d\n";
  case DBGSTR_SVFL_CorrectResult:       return "補正後の対象ポケモンは 位置=%d, ID=%d\n";
  case DBGSTR_SVFL_AtkPower:            return "攻撃力=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_DefGuard:            return "防御力=%d  (Critical=%d, ratio=%08x\n";
  case DBGSTR_SVFL_WazaPower:           return "ワザID=%d : 威力=%d  (ratio=%08x)\n";
  case DBGSTR_SVFL_UncategoryWazaInfo:  return "未分類ワザ 攻撃PokeID=%d, 対象ポケ数=%d/%d\n";
  case DBGSTR_SVFL_QueWritePtr:         return "[* SVF *]         Que WritePtr=%d\n";
  case DBGSTR_SVFL_ClientPokeStillAlive:return "クライアント_%d (SIDE:%d) のポケはまだ%d体生きている\n";
  case DBGSTR_SVFL_ClientPokeDeadAll:   return "クライアント_%d (SIDE:%d) のポケは全滅した\n";
  case DBGSTR_SVFL_PosEffAdd:           return "位置エフェクトハンドラ生成: pos=%d, effect=%d, pokeID=%d\n";
  case DBGSTR_SVFL_PosEffDupFail:       return "位置(%d)にPOSEffect[%d]ハンドラを追加失敗（使用ポケ=%d）\n";
  case DBGSTR_SVFL_HitCheckInfo1:       return "攻撃ポケ[%d]  命中Rank=%d  ワザ的中率=%d\n";
  case DBGSTR_SVFL_HitCheckInfo2:       return "防御ポケ[%d]  回避Rank=%d\n";
  case DBGSTR_SVFL_HitCheckInfo3:       return "命中ランク=%d, 命中率=%d, 命中率補正=%08x\n";
  case DBGSTR_SVFL_HitCheckInfo4:       return "最終命中率 = %d\n";
  case DBGSTR_SVFL_StartAfterPokeChange:return "ターン途中ポケモン入れ替え後のサーバーコマンド生成\n";
  case DBGSTR_SVFL_PokeChangeRootInfo:  return "クライアント(%d)のポケモン(位置%d) を、%d番目のポケといれかえる\n";
  case DBGSTR_SVFL_UseItemCall:         return "シューター「アイテムコール」使用  対象Poke=%d\n";
  case DBGSTR_SVFL_ChangePokeReqInfo:   return "クライアント[%d]   空いている位置の数=%d, 位置ID= ";
  case DBGSTR_SVFL_TurnStart_Result:    return "【ターン開始コマンド】処理アクション %d / %d\n";
  case DBGSTR_SVFL_ActOrderMainStart:   return "コマンド生成コア 処理アクション開始Index=%d  numAction=%d\n";
  case DBGSTR_SVFL_ActOrderMainDropOut: return "Result=%dによる中途サーバ返信\n";
  case DBGSTR_SVFL_ExpCalc_Base:        return "基本となる経験値=%d\n";
  case DBGSTR_SVFL_ExpCalc_MetInfo:     return "死亡ポケ[%d]が対面した相手ポケの数=%d, その内、生きてる数=%d\n";
  case DBGSTR_SVFL_ExpCalc_DivideInfo:  return "メンバーIdx[%d]のポケに経験値%dを分配\n";
  case DBGSTR_SVFL_ExpCalc_Result:      return "メンバーIdx[%d]のポケに対し、最終経験値=%d\n";
  case DBGSTR_SVFL_ExpAdjustCalc:       return "自分Lv=%d, 敵Lv=%d, num=%d, denom=%d, 基本経験値=%d -> 補正後経験値=%d\n";
  case DBGSTR_SVFL_RecDataSendComped:   return "操作記録データの送信完了\n";
  case DBGSTR_SVFL_SendServerCmd:       return "サーバコマンド送信します ... result=%d\n";
  case DBGSTR_SVFL_AllClientCmdPlayComplete:  return "全クライアントのコマンド再生終了...result=%d\n";
  case DBGSTR_SVFL_GotoQuit:            return "バトル終了へ\n";
  case DBGSTR_SVFL_ReqWazaCallActOrder: return "他ワザ呼び出し [%d] --> [%d], target=%d\n";
  case DBGSTR_SVFL_WazaExeFail_1:       return "ワザ出し失敗（１）  ポケID=%d, 失敗コード=%d\n";
  case DBGSTR_SVFL_WazaExeFail_2:       return "ワザ出し失敗（２）  ポケID=%d, 失敗コード=%d\n";
  case DBGSTR_SVFL_HandEx_AddSick:      return "状態異常 poke=%d, code=%d, cont=%08x, AlmostFlag=%d\n";
  case DBGSTR_SVFL_CombiWazaCheck:      return "ワザ[%d]は合体ワザのひとつです\n";
  case DBGSTR_SVFL_CombiWazaFound:      return "  自分より後に、同じ位置(%d）にワザ(%d)を打つ味方（%d）が見つかった\n";
  case DBGSTR_SVFL_CombiDecide:         return "  ポケ(%d）が先発。ポケ(%d)と合体ワザ予約！\n";
  case DBGSTR_SVFL_StartAfterPokeIn:    return "空き位置にポケモン投入後のサーバーコマンド生成を開始する\n";
  case DBGSTR_SVFL_AfterPokeIn_Alive:   return "生きているポケモンとの入れ替えが発生 Cleint=%d, Pos %d <-> %d\n";
  case DBGSTR_SVFL_AfterPokeIn_Dead:    return "ひんしポケモンの空きを埋める入れ替えが発生 Pos %d <-> %d\n";
  case DBGSTR_SVFL_PokeDead:            return "ポケ(%d) しにます\n";
  case DBGSTR_SVFL_AddSickFailCode:     return "状態異常失敗コード=%d\n";
  case DBGSTR_SVFL_AddSickFixed:        return "状態異常決定：コード=%d\n";
  case DBGSTR_SVFL_WazaExeStart:        return "ポケ[%d], waza=%dのワザ出し処理開始\n";
  case DBGSTR_SVFL_ActionSortInfo:      return "アクションソート:Client(%d), actionCnt=%d\n";
  case DBGSTR_SVFL_ActionSortRotation:  return ".. Action Sort Rot nextPokeIdx = %d\n";
  case DBGSTR_SVFL_ActionSortGen:       return ".. Action Sort pokeIdx=%d, pokeID=%d, action=%d\n";
  case DBGSTR_SVFL_TrainerItemTarget:   return "トレーナーアイテム使用：tgtIdx=%d, tgtPokeID=%d, pos=%d\n";
  case DBGSTR_SVFL_DoubleTargetIntr:    return "ダブル敵１体ターゲット…割り込みポケ=%d\n";
  case DBGSTR_SVFL_DoubleTargetRegister:return "ダブル敵１体ターゲット…確定ポケ=%d\n";
  case DBGSTR_SVFL_DeadAlready:         return "ポケ(%d)既に死んでるので死亡コマンドを打ちません\n";
  case DBGSTR_SVFL_DeadDiffLevelCheck:  return "ポケ死亡、自分lv=%d, 敵Lv=%d\n";
  case DBGSTR_SVFL_HandExRelive:        return "HandEx : ポケ生き返り pokeID=%d, HP=%d\n";
  case DBGSTR_SVFL_PutWazaEffect:       return "ワザエフェクトコマンド生成: reservedPos=%d, wazaID=%d, atkPos(%d)->defPos(%d), effIndex=%d\n";
  case DBGSTR_SVFL_WazaDmgCmd:          return "ワザダメージコマンド wazaID=%d, pokeCnt=%d pokeID= ";
  case DBGSTR_SVFL_ActIntr:             return "割り込みアクション adrs=%p, pokeID=%d\n";
  case DBGSTR_SVFL_RelivePokeStart:     return "ClientID=%d のポケが生き返った..";
  case DBGSTR_SVFL_ReliveEndIn:         return "空きがあるので入場させる\n";
  case DBGSTR_SVFL_ReliveEndIgnore:     return "空きがないので無視する\n";
  case DBGSTR_SVFL_RankEffCore:         return "ポケ[%d]の能力ランク(%d)を %d 段階変化 (ワザ使用ポケ=%d）\n";
  case DBGSTR_SVFL_IntrTargetPoke:      return "割り込みターゲットポケ=%d\n";
  case DBGSTR_SVFL_CheckItemReaction:   return "ポケ[%d]の装備アイテム発動チェックします\n";
  case DBGSTR_SVFL_MagicCoatStart:      return "マジックコート跳ね返し %d 件\n";
  case DBGSTR_SVFL_MagicCoatReflect:    return "マジックコート跳ね返し poke(%d) -> (%d)_%d\n";
  case DBGSTR_SVFL_YokodoriDetermine:   return "ポケ[%d]のワザ[%d]をよこどります\n";
  case DBGSTR_SVFL_YokodoriExe:         return "よこどりしたポケ=[%d], 対象位置=%d\n";
  case DBGSTR_SVFL_YokodoriInfo:        return "よこどり対象PokeID=%d, 位置=%d\n";
  case DBGSTR_SVFL_HandExRoot:          return "ProcHandEX : ADRS=0x%p, type=%d, fFailSkip=%d, fPrevSucceed=%d\n";
  case DBGSTR_SVFL_HandExContFail:      return  "  skip (prev handEx failed)\n";
  case DBGSTR_SVFL_HandExContDead:      return  "  skip (user pokemon dead)\n";
  case DBGSTR_SVFL_HandExSetResult:     return  "  set result = %d\n";
  case DBGSTR_SVFL_HandExTokChangeAdd:  return  "とくせい書き換えハンドラ pokeID=%d, factor=%p\n";
  case DBGSTR_SVFL_HandExTokChangeEventCall: return "とくせい書き換え後イベントコール pokeID=%d\n";
  case DBGSTR_SVFL_HandExTokChangeEventCallEnd : return "とくせい書き換え後イベント終了\n";
  case DBGSTR_SVFL_MemberInEventBegin: return "メンバー入場イベント pokeID=%d 開始\n";
  case DBGSTR_SVFL_MemberInEventEnd :  return "メンバー入場イベント pokeID=%d 終了\n";
  case DBGSTR_SVFS_RegTargetDouble:     return "ダブルターゲット：対象範囲=%d, 攻撃ポケ=%d, 選択位置=%d\n";
  case DBGSTR_SVFS_Result_RestCnt:      return "残りポケ数  side0=%d, side1=%d, side2=%d, side3=%d\n";
  case DBGSTR_SVFS_LastPokeID:          return "最後まで生きてたポケID=%d\n";
  case DBGSTR_SVFL_DeadPokeCount:       return "倒したポケ数= %d vs %d vs %d vs %d\n";
  case DBGSTR_SVFL_HPRatio:             return "残HP [side:%d]  総和=%d, 割合=%d\n";

  case DBGSTR_POSPOKE_Out:              return " poke[%d] out from pos[%d]\n";
  case DBGSTR_POSPOKE_In:               return " poke[%d] in to pos[%d]\n";

  case DBGSTR_CALC_EscInfoAdd:       return "逃げ情報：ClientID=%d, 追加、トータル%d名\n";
  case DBGSTR_CALC_EscCheckMyTeam:   return "ClientID=%d が逃げた ... 自分のチーム\n";
  case DBGSTR_CALC_EscCheckEnemy:    return "ClientID=%d が逃げた ... 相手のチーム\n";
  case DBGSTR_CALC_EscResultBoth1:   return "両チーム逃げ：野生か通常トレーナーなので自分が逃げたのと同じ\n";
  case DBGSTR_CALC_EscResultBoth2:   return "両チーム逃げ：通信・サブウェイなら引き分け\n";
  case DBGSTR_CALC_EscResultLose:    return "自分チームが逃げたから負け\n";
  case DBGSTR_CALC_EscResultWin:     return "相手チームが逃げたから勝ち\n";


  case DBGSTR_SERVER_FlowResult:              return "サーバー処理結果=%d\n";
  case DBGSTR_SERVER_SendShooterChargeCmd:    return "シューターチャージコマンド発行\n";
  case DBGSTR_SERVER_ShooterChargeCmdDoneAll: return "全クライアントでシューターチャージコマンド処理終了\n";
  case DBGSTR_SERVER_SendActionSelectCmd:     return "アクション選択コマンド発行\n";
  case DBGSTR_SERVER_ActionSelectDoneAll:     return "アクション受け付け完了\n";
  case DBGSTR_SV_ChangePokeOnTheTurn:         return "ターン途中のポケ入れ替え発生\n";
  case DBGSTR_SV_PokeInReqForEmptyPos:        return "空き位置への新ポケ投入リクエスト受け付け\n";
  case DBGSTR_SV_StartChangePokeInfo:         return "入れ替えポケモン選択へ  交替されるポケ数=%d\n";
  case DBGSTR_SV_SendQuitACmad:               return "全クライアントにバトル終了コマンド(%d)送信開始\n";
  case DBGSTR_SV_ReplyQuitACmad:              return "全クライアントからバトル終了コマンド受信応答あり\n";
  case DBGSTR_SV_RestoreAction:               return "整合性チェック用サーバ：ClientID=%d, numAction:%d 復元\n";
  case DBGSTR_SV_CmdCheckOK:                  return "整合性チェック用サーバ：チェックOK\n";
  case DBGSTR_SV_CmdCheckNG:                  return "整合性チェック用サーバ：チェックNG!! cmdSize= %d , %d\n";
  case DBGSTR_SV_SendActRecordFailed:         return "アクション記録送信に失敗した!!\n";
  case DBGSTR_SV_SendActRecord:               return "アクション記録データを送信する (%dbytes)\n";
  case DBGSTR_SV_SendBtlInRecord:             return "バトル開始チャプタデータを送信する (%dbytes)\n";
  case DBGSTR_SV_ExitBattle:                  return "ExitBattle .. competitor = %d\n";
  case DBGSTR_SV_CommError:                   return "通信エラーによる終了 line=%d\n";

  case DBGSTR_SC_PutCmd:                return "[QUE]PutCmd=%d, WP=%d, Format=%02x, argCnt=%d, args=";
  case DBGSTR_SC_ReservedPos:           return "[QUE]reserved cmd=%d, size=%d, pos=%d, wp=%d\n";
  case DBGSTR_SC_WriteReservedPos:      return "[QUE]Write Reserved Pos ... pos=%d, cmd=%d, arg_cnt=%d\n";
  case DBGSTR_SC_ReservedInfo:          return "[QUE]Write Reserved Info .. cmd=%d, size=%d\n";
  case DBGSTR_SC_PutMsgParam:           return "[QUE] PUT  MSG SC=%d, StrID=%d";
  case DBGSTR_SC_ReadMsgParam:          return "[QUE] READ MSG SC=%d, StrID=%d";
  case DBGSTR_SC_PutMsg_SE:             return "  SE_ID=%d\n";
  case DBGSTR_SC_ArgsEqual:             return " args = ";
  case DBGSTR_SC_WPEqual:               return " WP = ";
  case DBGSTR_SC_RPEqual:               return " RP = ";
  case DBGSTR_SC_ReadCmd:               return "Read cmd=%d, fmt=%02x, rp=%d, SCEX_RESERVE=%d\n";
  case DBGSTR_SC_ReserveCmd:            return " -> reserved cmd=%d, size=%d, rp=%d\n";


  case DBGSTR_EVENT_RmvFactorCalling:   return " 呼び出し中に削除されたFactor Depend-Type = [%2d-%d], adrs=%p\n";
  case DBGSTR_EVENT_CallFactorStart:    return " ** %p 呼び出し開始 Depend-Type = [%2d-%d] **\n";
  case DBGSTR_EVENT_CheckFactorStart:   return "呼び出しテーブルチェック type=%d, dependID=%d, numHandlers=%d, eventID=%d\n";
  case DBGSTR_EVENT_CheckFactorTbl:     return "   テーブル登録EventID(%d) = %d\n";
  case DBGSTR_EVENT_CallFactorEnd:      return " ** %p 呼び出し終了 Depend-Type = [%2d-%d] **\n";
  case DBGSTR_EVENT_SkipByNewComerFlg:  return " 新入りにつきスキップ Factor pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_ClearNewComFlag:    return " 新入りフラグクリア pokeID=%d, adrs=%p\n";
  case DBGSTR_EVENT_SkipByCallingFlg:   return " 自己呼び出し中につきスキップ Depend-Type = [%2d-%d], adrs=%p\n";
  case DBGSTR_EVENT_SkipByStackLayer:   return " 同階層での登録なのでスキップ Depend-Type = [%2d-%d], adrs=%p\n";
  case DBGSTR_EVENT_SkipByCheckHandler: return " ハンドラによるスキップ Depend-Type = [%2d-%d]\n";
  case DBGSTR_EVENT_NextFactorInfo:     return "   次にチェックする要素 Depend-Type = [%2d-%d], adrs=%p\n";
  case DBGSTR_EVENT_NextFactorInfo2:    return " ..next, Depend-Type = [%2d-%d], adrs=%p\n";

  case DBGSTR_EVENT_AddFactorInfo:      return "[ADD-0] DependType=[%2d-%d]  pri=%08x, adrs=%p\n";
  case DBGSTR_EVENT_AddFactor_Case0:    return "  最初の登録\n";
  case DBGSTR_EVENT_AddFactor_Case1:    return "  先頭への登録：次の要素 = [%2d-%d:%p]\n";
  case DBGSTR_EVENT_AddFactor_Case2:    return "  途中への登録：[%2d-%d:%p]->[%2d-%d:%p]->[%2d-%d:%p]\n";
  case DBGSTR_EVENT_AddFactor_Case3:    return "  末尾への登録：[%2d-%d:%p]->[%2d-%d:%p]->%p, ..FirstFactor=%p\n";


  case DBGSTR_EV_AddFactor:             return "[ADD-2] Depend-Type = [%2d-%d] Factor=%p, Pri=%08x\n";
  case DBGSTR_EV_DelFactor:             return "[DEL] Depend-Type = [%2d-%d] Factor=%p, prev=%p, next=%p\n";
  case DBGSTR_EV_DelReserveFactor:      return "[Drq] Depend-Type = [%2d-%d] Factor=%p\n";
  case DBGSTR_EV_LinkInfoHeader:        return "***** [[EV Chain]] *****\n";
  case DBGSTR_EV_LinkInfoFooder:        return "************************\n\n";
  case DBGSTR_EV_LinkPtr:               return "%p->";
  case DBGSTR_EV_LinkEmpty:             return " empty...\n";
  case DBGSTR_EVAR_Push:                return "PUSH [%5d] SP=%d\n";
  case DBGSTR_EVAR_Pop:                 return "Pop! [%5d] SP=%d\n";
  case DBGSTR_DEADREC_Add:              return " 死亡記録 ターン%d件目 ... pokeID=%d\n";
  case DBGSTR_POKESET_RemoveDeadPoke:   return "ポケ(%d) 死んだのでセットから除外\n";
  case DBGSTR_SICK_ContProc:            return "ポケ[%d], 状態異常[%d] が継続中...\n";

  case DBGSTR_SIDE_AddFirst:            return "サイドエフェクト初回登録 side=%d, eff=%d, pCnt=%p, cnt=%d\n";
  case DBGSTR_SIDE_NoMoreAdd:           return "サイドエフェクトこれ以上登録できない side=%d, eff=%d\n";

  case DBGSTR_SCU_RelivePokeAct:        return "Pos[%d]=Vpos[%d] ポケ(%d)が生き返り\n";
  case DBGSTR_SCD_DecideTargetPos:      return "ターゲット決定 ... hitBtn=%d, hitPos=%d, targetPos=%d\n";
  case DBGSTR_SCD_TargetHitPosConvert:  return "  HitPos:%d -> vpos:%d -> btlPos:%d\n";
  case DBGSTR_SCD_TargetHitPosFix:      return "   btlPos:%d  Selectable !\n";

  case DBGSTR_Item_PinchReactItem:      return "PokeID=%d, 最大HP=%d, 現HP=%d, n=%d ... ";
  case DBGSTR_Item_PinchReactOn:        return "アイテム使います\n";
  case DBGSTR_HANDWAZA_CombiWazaExe:    return "ポケ(%d)がポケ(%d）のワザ(%d)に続けて合体ワザ発動->効果=%d\n";
  case DBGSTR_HANDWAZA_AlreadyRegistered: return "ポケ[%d]のワザハンドラ[%d]はすでに登録済み\n";
  case DBGSTR_HANDWAZA_YokodoriDecide:  return "ポケ(%d), よこどりします\n";
  case DBGSTR_HANDITEM_OujaCheck:       return "おうじゃのしるしチェック：デフォルトひるみ率=%d\n";
  case DBGSTR_HANDITEM_OujaEffective:   return "おうじゃのしるし有効：ひるみ率=%d\n";
  case DBGSTR_HANDTOK_TRACE_Add:        return "トレース発動します pokeID=%d\n";
  case DBGSTR_HANDTOK_PressureIn:       return "プレッシャー発動します  pokeID=%d, factor=%p\n";


  case DBGSTR_REC_ReadActStart:         return "rec seek start RP= %d\n";
  case DBGSTR_REC_ReadActSkip:          return "rec seek RotateData skip %d byte .. RP=%d \n";
  case DBGSTR_REC_SeekClient:           return "rec seek numClient=%d\n";
  case DBGSTR_REC_ReadActParam:         return "rec ReadPtr=%d, act=%d, waza=%d\n";
  case DBGSTR_REC_CheckMatchClient:     return "rec Read-ClientID=%d, seeking-clientID=%d,  numAction=%d\n";
  case DBGSTR_REC_ReadTimeOverCmd:      return "時間制限による終了コマンドを読み取った  RP=%d\n";
  case DBGSTR_REC_TimeOverCmdWrite:     return "時間制限終了コマンドを書き込んだ\n";
  case DBGSTR_REC_CheckReadComp:        return "録画読み込み完了チェック：clientID=%d, RP=%d, dataSize=%d\n";;

  case DBGSTR_line: return " line=%d\n";
  case DBGSTR_csv:  return "%d,";
  case DBGSTR_done: return "done!";
  case DBGSTR_LF:   return "\n";

  }
  return NULL;
}
//=============================================================================================
/**
 * デバッグ用ヘッダPrint
 *
 * @param   fileName
 * @param   line
 */
//=============================================================================================
void BTL_DEBUGPRINT_PrintHeader( const char* fileName, u32 line )
{
  print_type();
  print_file_info( fileName, line );
}

//=============================================================================================
/**
 * 出力有効状態のファイルか判定
 *
 * @param   filename
 *
 * @retval  bool
 */
//=============================================================================================
bool BTL_DEBUGPRINT_IsEnable( const char* filename )
{
  return printCtrl_isEnableFile( filename );
}
void BTL_DEBUGPRINT_PrintDump( const char* caption, const void* data, u32 size )
{
  NN_LOG( "[DUMP] %s (%dbytes)\n", caption, size );

  if( size > 8 ){ size = 8; }

  {
    const u8* p = reinterpret_cast<const u8*>( data );
    u32 i;
    for(i=0; i<size; i++)
    {
      NN_LOG("%02x ", p[i]);
    }
    NN_LOG("\n");
  }
}








#ifdef BTL_PRINT_SYSTEM_ENABLE

void BTL_UTIL_PRINTSYS_Init( void )
{
  nn::os::TargetHardware  target = nn::os::GetRunningTargetHardware();

  if( (target == nn::os::TARGET_HARDWARE_KMC_DEBUGGER)
  ||  (target == nn::os::TARGET_HARDWARE_IS_DEBUGGER)
  ){
    PrintSysEnableFlag = true;
  }
  else{
    PrintSysEnableFlag = false;
  }
}

void BTL_UTIL_PRINTSYS_Quit( void )
{
  if( PrintSysEnableFlag ){
    PrintSysEnableFlag = false;
  }
}

void BTL_UTIL_SetPrintType( BtlPrintType type )
{
  if( PrintSysEnableFlag )
  {
    BTL_DEBUGPRINT_SetType( type );
  }
}

void BTL_UTIL_Printf( const char* filename, int line, u32 channel, u32 strID, ... )
{
  if( PrintSysEnableFlag )
  {
    if( channel == 0 ){
      return;
    }else{
      if( --channel ){
          //  CTR では意味ないのでコメントアウト
          //  OS_SetPrintOutput_Arm9( channel );
      }
    }

    {
      const char* fmt_str = BTL_DEBUGPRINT_GetFormatStr( static_cast<BtlDebugStrID>(strID) );
      if( fmt_str )
      {
        if( BTL_DEBUGPRINT_IsEnable(filename) )
        {
          va_list vlist;

          BTL_DEBUGPRINT_PrintHeader( filename, line );
          va_start( vlist, strID );
          (void)nn::dbg::detail::VPrintf( fmt_str, vlist );
          va_end( vlist );
        }
      }
    }

    /*  CTRでは意味ないのでコメントアウト
    if( channel ){
      OS_SetPrintOutput_Arm9( 0 );
    }
    */
  }
}

void BTL_UTIL_PrintfSimple( const char* filename, u32 channel, u32 strID, ... )
{
  if( PrintSysEnableFlag )
  {
    if( channel == 0){
      return;
    }else{
      if( --channel ){
        /** CTRでは出力チャンネルないみたい
        OS_SetPrintOutput_Arm9( channel );
        */
      }
    }

    {
      const char* fmt_str = BTL_DEBUGPRINT_GetFormatStr( static_cast<BtlDebugStrID>(strID) );

      if( fmt_str )
      {
        if( BTL_DEBUGPRINT_IsEnable(filename) )
        {
          va_list vlist;

          va_start( vlist, strID );
          (void)nn::dbg::detail::VPrintf( fmt_str, vlist );
          va_end( vlist );
        }
      }
    }

    /** CTRでは出力チャンネルないみたい
    if( channel ){
      OS_SetPrintOutput_Arm9( 0 );
    }
    */
  }
}

void BTL_UTIL_DumpPrintf( const char* caption, const void* data, u32 size )
{
  if( PrintSysEnableFlag )
  {
    BTL_DEBUGPRINT_PrintDump( caption, data, size );
  }
}

#endif  /* #ifdef BTL_PRINT_SYSTEM_ENABLE */
GFL_NAMESPACE_END(btl)



