#if !defined __TR_AI_CMD_H__
#define __TR_AI_CMD_H__



/**
 *  スクリプト側から呼び出し可能なコマンド群
 */
enum  AICmd {

  CMD_IF_RND_UNDER,
  CMD_IF_RND_OVER,
  CMD_IF_RND_EQUAL,
  CMD_IFN_RND_EQUAL,

  // HPでの分岐
  CMD_IF_HP_UNDER,
  CMD_IF_HP_OVER,
  CMD_IF_HP_EQUAL,
  CMD_IFN_HP_EQUAL,

  // 状態異常をチェック
  CMD_IF_POKESICK,      // なにかしらの状態異常になっている
  CMD_IFN_POKESICK,
  CMD_IF_WAZASICK,      // 状態異常の種類を選択して調べる
  CMD_IFN_WAZASICK,
  CMD_IF_DOKUDOKU,      // どくどく状態を調べる
  CMD_IFN_DOKUDOKU,
  CMD_IF_CONTFLG,       // 技効果の種類を選択して調べる
  CMD_IFN_CONTFLG,
  CMD_IF_SIDEEFF,
  CMD_IFN_SIDEEFF,
  CMD_GET_HOROBINOUTA_TURN_MAX, //「ほろびのうた」発動ターン数
  CMD_GET_HOROBINOUTA_TURN_NOW, //「ほろびのうた」現在の経過ターン数
  CMD_GET_KODAWARI_WAZA,        // こだわっている技を取得( こだわっていないなら WAZANO_NULL を返す )

  // 自分がダメージ技を持っているか判断して分岐
  CMD_IF_HAVE_DAMAGE_WAZA,
  CMD_IFN_HAVE_DAMAGE_WAZA,

  // ターンのチェック
  CMD_CHECK_TURN,

  // タイプのチェック
  CMD_CHECK_TYPE,

  // 技を使用可能かどうかチェック
  CMD_CHECK_WAZA_USABLE,

  // ダメージワザかどうかのチェック
  CMD_CHECK_DAMAGE_WAZA,

  // 攻撃技かどうかのチェック
  CMD_CHECK_IRYOKU,

  // 威力が一番高いかのチェック
  CMD_COMP_POWER,

  // 前のターンに使った技のチェック（攻撃側／防御側を指定します）
  CMD_CHECK_LAST_WAZA,

  // どちらが先行かのチェック
  CMD_IF_FIRST,

  // 控えが何体いるかのチェック
  CMD_CHECK_BENCH_COUNT,

  // 現在の技ナンバーのシーケンスナンバーのチェック
  CMD_CHECK_WAZASEQNO,

  // 特殊能力のチェック
  CMD_CHECK_TOKUSEI,

  // 自分と相手の相性チェック
  CMD_CHECK_WAZA_AISYOU,         // 技を撃ったときの相性をチェックする
  CMD_GET_WAZA_AISYOU,           // 技を撃ったときの相性を取得する
  CMD_IF_HAVE_WAZA_AISYOU_OVER,  // 指定した相性より良い相性の技を持っているか？
  CMD_IF_HAVE_WAZA_AISYOU_EQUAL, // 指定した相性の技を持っているか？

  // 控えの状態チェック
  CMD_IF_BENCH_COND,
  CMD_IFN_BENCH_COND,

  // 天候チェック
  CMD_CHECK_WEATHER,

  // 自分や相手のパラメータ変化値を参照して分岐
  CMD_IF_PARA_UNDER,
  CMD_IF_PARA_OVER,
  CMD_IF_PARA_EQUAL,
  CMD_IFN_PARA_EQUAL,

  // 技のダメージ計算をして相手が瀕死になるかチェック
  CMD_IF_WAZA_HINSHI,
  CMD_IFN_WAZA_HINSHI,

  // 特定の技を持っているかのチェックをして分岐
  CMD_IF_HAVE_WAZA,
  CMD_IFN_HAVE_WAZA,

  // 特定の技シーケンスを持っているかのチェックをして分岐
  CMD_IF_HAVE_WAZA_SEQNO,
  CMD_IFN_HAVE_WAZA_SEQNO,

  // にげるをせんたく
  CMD_ESCAPE,

  // 装備アイテムのチェック
  CMD_CHECK_SOUBI_ITEM,

  // 装備効果のチェック
  CMD_CHECK_SOUBI_EQUIP,

  // ポケモンの性別のチェック
  CMD_CHECK_POKESEX,

  // ねこだましカウンタのチェック
  CMD_CHECK_NEKODAMASI,

  // たくわえるカウンタのチェック
  CMD_CHECK_TAKUWAERU,

  // 対戦ルールのチェック
  CMD_CHECK_BTL_RULE,

  // 対戦相手のチェック
  CMD_CHECK_BTL_COMPETITOR,

  // リサイクルできるアイテムのチェック
  CMD_CHECK_RECYCLE_ITEM,

  // ワークに入っている技ナンバーのタイプをチェック
  CMD_CHECK_WORKWAZA_TYPE,

  // ワークに入っている技ナンバーの威力をチェック
  CMD_CHECK_WORKWAZA_POW,

  // ワークに入っている技ナンバーのシーケンスナンバーをチェック
  CMD_CHECK_WORKWAZA_SEQNO,

  // まもるカウンタをチェック
  CMD_CHECK_MAMORU_COUNT,

  // お互いのレベルをチェックして分岐
  CMD_IF_LEVEL,

  // 挑発状態かチェックして分岐
  CMD_IF_CHOUHATSU,
  CMD_IFN_CHOUHATSU,

  // 攻撃対象が味方がどうかチェック
  CMD_IF_MIKATA_ATTACK,

  // 指定タイプを持っているかどうかチェック
  CMD_CHECK_HAVE_TYPE,

  // 指定とくせいを持っているかどうかチェック
  CMD_CHECK_HAVE_TOKUSEI,   //0x50

  // 相方がもらいびパワーアップ状態かチェックして分岐
  CMD_IF_ALREADY_MORAIBI,

  // アイテムを持っているかチェックして分岐
  CMD_IF_HAVE_ITEM,

  // FLDEFFチェック
  CMD_FLDEFF_CHECK,

  // SIDE_CONDITIONのカウントを取得
  CMD_CHECK_SIDEEFF_COUNT,

  // 控えポケモンのHP減少をチェック
  CMD_IF_BENCH_HPDEC,

  // 控えポケモンのPP減少をチェック
  CMD_IF_BENCH_PPDEC,

  // 装備アイテムのなげつける威力を取得
  CMD_CHECK_NAGETSUKERU_IRYOKU,

  // PPを取得
  CMD_CHECK_PP_REMAIN,

  // とっておきチェック
  CMD_IF_TOTTEOKI,

  // 技のダメージタイプチェック
  CMD_CHECK_WAZA_KIND,

  // 相手が最後に出した技のダメージタイプチェック
  CMD_CHECK_LAST_WAZA_KIND,

  // 素早さで指定した側が何位かチェック
  CMD_CHECK_AGI_RANK,

  // スロースタート何ターン目か
  CMD_CHECK_SLOWSTART_TURN,

  // 控えにいる方がダメージを与えるかどうかチェック
  CMD_IF_BENCH_DAMAGE_MAX,

  // 抜群の技を持っているかチェック
  CMD_IF_HAVE_BATSUGUN,

  // 指定した相手の最後に出した技と自分の技とのダメージをチェック
  CMD_IF_LAST_WAZA_DAMAGE_CHECK,

  // 指定した相手のステータス上昇分の値を取得
  CMD_CHECK_STATUS_UP,

  // 指定した相手とのステータス差分を取得
  CMD_CHECK_STATUS_DIFF,

  // 指定した相手の、指定ステータスの変化量を取得
  CMD_CHECK_STATUS,

  // 威力が一番高いかのチェック（パートナーも含む）
  CMD_COMP_POWER_WITH_PARTNER,

  // 指定した相手が瀕死かチェックして分岐
  CMD_IF_HINSHI,
  CMD_IFN_HINSHI,

  // 技効果を考慮した特性取得（移動ポケモン専用それ以外の用途で使用するのは厳禁）
  CMD_GET_TOKUSEI,

  // みがわり中か？
  CMD_IF_MIGAWARI,

  // ポケモンナンバーのチェック
  CMD_CHECK_MONSNO,
  // フォルムナンバーのチェック
  CMD_CHECK_FORMNO,

  // 連動ランダム分岐
  CMD_IF_COMMONRND_UNDER,
  CMD_IF_COMMONRND_OVER,
  CMD_IF_COMMONRND_EQUAL,
  CMD_IFN_COMMONRND_EQUAL, 

  // みらいよち中かチェックして分岐
  CMD_IF_MIRAIYOCHI,

  // 攻撃と特攻を比べて分岐
  CMD_IF_DMG_PHYSIC_UNDER,
  CMD_IF_DMG_PHYSIC_OVER,
  CMD_IF_DMG_PHYSIC_EQUAL,

  //  きのみを食べたかどうか
  CMD_IF_ATE_KINOMI,
  // 特定の拡張タイプが設定されているかどうか
  CMD_IF_TYPE_EX,
  // 特定の○○グラウンドが発動しているかどうか
  CMD_IF_EXIST_GROUND,
  // ポケモンの体重を取得（アイテム・とくせいハンドラをチェックしない）
  CMD_GET_WEIGHT,
  // マルチバトルかどうか判定
  CMD_IF_MULTI,
  // 対象がメガシンカしたポケモンかどうか判定
  CMD_IF_MEGAEVOLVED,
  // 対象がメガシンカ可能かどうか判定
  CMD_IF_CAN_MEGAEVOLVE,
  // 対象がワザ効果により場から消えているかどうか判定（そらをとぶ等）
  CMD_IF_WAZAHIDE,
  // 対象のめざめるパワータイプを取得する
  CMD_GET_MEZAME_TYPE,

  // 戦闘AI( コマンドコールした本人 )がシナリオトレーナーかどうか判定
  CMD_IF_I_AM_SENARIO_TRAINER,

  // 対象が持つダメージ技のうち、最も高い威力で撃つことが出来る技の、威力値を取得する( ターゲットとのタイプ相性のみを考慮 )
  CMD_GET_MAX_WAZA_POWER_INCLUDE_AFFINITY,

  // 特性により技効果を無効化できるか？
  CMD_CHECK_WAZA_NO_EFFECT_BY_TOKUSEI, 

  // 前ターンで最後に受けた技の技No.を取得する
  CMD_GET_LAST_DAMAGED_WAZA_AT_PREV_TURN, 

  // 現在採点中の技No.を取得する
  CMD_GET_CURRENT_WAZANO, 

  // 現在採点中の道具No.を取得する
  CMD_GET_CURRENT_ITEMNO, 

  // 技「じだんだ」を高威力で撃てる状態か？
  CMD_IF_ZIDANDA_POWERUP, 

  // バトルロイヤルの現在順位を取得する
  CMD_GET_BATTLEROYAL_RANKING,

  // 指定クライアントが倒したポケモンの数を取得する
  CMD_GET_CLIENT_KILL_COUNT,

  // 現在採点中の技の対象範囲( WazaTarget )を取得する
  CMD_GET_WAZA_TARGET,

  // 抜群の技を持っているかチェック（控えポケモンも正確）
  CMD_IF_HAVE_BATSUGUN_CAN_BENCH,

  // コマンド総数
  NUM_AI_CMD,
};


enum {
  LOSS_CALC_OFF = 0,    // ダメージロス計算のぶれなし
  LOSS_CALC_ON = 1,     // ダメージロス計算のぶれあり
};

//=========================================================================
//  AI_STATUSFLAG宣言
//=========================================================================
enum {
  AI_STATUSFLAG_END           =  0x01,      //AIシーケンスの終了
  AI_STATUSFLAG_ESCAPE        =  0x02,      //にげるを選択
  AI_STATUSFLAG_SAFARI        =  0x04,      //サファリゾーン特有のアクション
  AI_STATUSFLAG_FINISH        =  0x08,      //チェックする技が残っていても終了
  AI_STATUSFLAG_CONTINUE      =  0x10,      //AI計算継続
  AI_STATUSFLAG_END_OFF       =  (0x01^0xff),
  AI_STATUSFLAG_ESCAPE_OFF    =  (0x01^0xff),
  AI_STATUSFLAG_SAFARI_OFF    =  (0x04^0xff),
  AI_STATUSFLAG_CONTINUE_OFF  =  (0x10^0xff),     //AI計算継続

  AI_ENEMY_ESCAPE     = 4,
  AI_ENEMY_SAFARI     = 5,
  AI_ENEMY_RESHUFFLE  = 6,

//tr_ai.s waza_ai.cで使用（マクロでsideに指定する値）
  CHECK_DEFENCE         = 0,
  CHECK_ATTACK          = 1,
  CHECK_DEFENCE_FRIEND  = 2,
  CHECK_ATTACK_FRIEND   = 3,
  CHECK_BENCH           = 4,

  CHECK_DEFENCE_TYPE1   = 0,
  CHECK_ATTACK_TYPE1    = 1,
  CHECK_DEFENCE_TYPE2   = 2,
  CHECK_ATTACK_TYPE2    = 3,
  CHECK_WAZA            = 4,
  CHECK_DEFENCE_FRIEND_TYPE1  = 5,
  CHECK_ATTACK_FRIEND_TYPE1   = 6,
  CHECK_DEFENCE_FRIEND_TYPE2  = 7,
  CHECK_ATTACK_FRIEND_TYPE2   = 8,
};



//COMP_POWERで使用
enum {
  COMP_POWER_NONE,    //威力計算しない技
  COMP_POWER_NOTOP,   //ダメージ量がトップじゃない
  COMP_POWER_TOP,     //ダメージ量がトップ
};

//IF_FIRSTで使用
enum {
  IF_FIRST_ATTACK,      //自分が早い
  IF_FIRST_DEFENCE,     //相手が早い
  IF_FIRST_EQUAL,       //同じ
};

//AI_CHECK_WAZA_AISYOUで使用
enum {
  AISYOU_0BAI,
  AISYOU_1_64BAI,
  AISYOU_1_32BAI,
  AISYOU_1_16BAI,
  AISYOU_1_8BAI,
  AISYOU_1_4BAI,
  AISYOU_1_2BAI,
  AISYOU_1BAI,
  AISYOU_2BAI,
  AISYOU_4BAI,
  AISYOU_8BAI,
  AISYOU_16BAI,
  AISYOU_32BAI,
  AISYOU_64BAI,
};

//AI_IF_PARA_UNDERで使用
//AI_IF_PARA_OVERで使用
//AI_IF_PARA_EQUALで使用
//AI_IFN_PARA_EQUALで使用
enum {
  PARA_HP  =     0x00,  // 体力
  PARA_POW =     1,     // BPP_ATTACK_RANK,     // 攻撃力
  PARA_DEF =     2,     // BPP_DEFENCE_RANK,    // 防御力
  PARA_SPEPOW =  3,     // BPP_SP_ATTACK_RANK,  // 特攻
  PARA_SPEDEF =  4,     // BPP_SP_DEFENCE_RANK, // 特防
  PARA_AGI =     5,     // BPP_AGILITY_RANK,    // 素早さ
  PARA_HIT =     6,     // BPP_HIT_RATIO,       // 命中率
  PARA_AVOID =   7,     // BPP_AVOID_RATIO,     // 回避率
};

//AI_IF_POKE_CHECK_STATE,AI_IF_WAZA_CHECK_STATEで使用
enum {
  STATE_KANASIBARI  =  0x00,     //かなしばり
  STATE_ENCORE      =  0x01      //アンコール
};

//AI_CHECK_WEATHERで使用（BTL_WEATHER_xxx系と同じ並びになるように）
enum {
  WEATHER_NONE = 0,       //天候変化なし
  WEATHER_HARE,           //にほんばれ状態
  WEATHER_AME,            //あまごい状態
  WEATHER_ARARE,          //あられ状態
  WEATHER_SUNAARASHI,     //すなあらし状態
  // @fix GFMMCat[95]【バトルＡＩ】天候書き換えできない天候に対して、ＡＩが天候技を使用してしまう
  WEATHER_OOAME,          //おおあめ状態
  WEATHER_OOHIDERI,       //おおひでり状態
  WEATHER_RANKIRYUU,      //らんきりゅう状態
  WEATHER_HUKAIKIRI = WEATHER_NONE, //ふかいきり状態（現状存在しない）
};


//AI_IF_LEVELで使用
enum {
  LEVEL_ATTACK    = 0x00,     //アタックのレベルが高い
  LEVEL_DEFENCE   = 0x01,     //ディフェンスのレベルが高い
  LEVEL_EQUAL     = 0x02,     //レベルが一緒
};

// AI_HAVE_TOKUSEI, AI_HAVE_WAZA 等で使用
enum {
  HAVE_NO      =  0,
  HAVE_YES     =  1,
  HAVE_UNKNOWN =  2,
};

// AI_IF_TABLE_JUMP, AI_IFN_TABLE_JUMPで使用するテーブルの終端
enum {
  TR_AI_TABLE_END = 0xffffffff,
};

//  AI_TABLE_JUMPで使用するインデックス用ワーク
enum {
  TABLE_JUMP_WAZASEQNO  = 0,    //技のシーケンスナンバーでジャンプ
};


// PMLIB系の定数を PAWN から参照してもらうために同様のものをココに用意している
// 同じ意味のものを別々に定義するのは怖いので、ここに追加した定数は必ず
// COMPILER_ASSERT等を使って、内容が一致することが保証されるようにすること
enum {
  WAZADATA_DMG_PHYSIC  = 1,
  WAZADATA_DMG_SPECIAL = 2,
};


#endif

