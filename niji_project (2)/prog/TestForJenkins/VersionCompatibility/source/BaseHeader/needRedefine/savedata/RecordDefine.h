//==============================================================================
/**
 * @file	  record_define.h
 * @brief		ゲーム内カウント処理のレコード名定義
 * @author  mori
 * @date    2012.10.16
 *
 * @note セット関数の返り値は全て実際に登録された値です。
 * @note record_define.hnamespaceの無い書式で書かれていますが、これはスクリプトから
 *       直接参照するためにrecord.hと分けてあります。
 *       ソースコード上ではrecord.hでincludeされているため、savedata::Record::RECID_～
 *       記述することになります。
 *
 * @note ※注意！新しいレコードを増やした時はreocrd.cppのlimit_tblも変更する事
 */
// =============================================================================

#if !defined( __RECORD_DEFINE_H__ )
#define	__RECORD_DEFINE_H__


enum RECORD_ID {
  // 4バイトレコード
  RECID_WALK_COUNT = 0,         ///< 歩数                                     999,999,999  0
  RECID_REPORT_COUNT,           ///< レポート回数                             999,999,999
  RECID_CLEAR_TIME,             ///< クリアタイム                             999,59,59
  RECID_BATTLE_COUNT,           ///< 全戦闘回数                               999,999,999
  RECID_BTL_ENCOUNT,            ///< エンカウント戦闘回数                     999,999,999
  RECID_BTL_TRAINER,            ///< トレーナー戦闘回数                       999,999,999
  RECID_CAPTURE_POKE,           ///< 捕獲したポケモンの数                     999,999
  RECID_FISHING_POKEMON,        ///< 釣り上げたポケモンの数                   999,999
  RECID_TAMAGO_HATCHING,        ///< タマゴを孵化させた回数                   999,999
  RECID_POKE_EVOLUTION,         ///< 進化させたポケモンの匹数                 999,999,999  
  RECID_RECOVERY_PC,            ///< ポケセンで休んだ回数                     999,999,999  10
  RECID_COMM_TRADE,             ///< P2P交換をした回数             999,999,999
  RECID_COMM_BATTLE,            ///< P2P対戦をした回数             999,999,999
  RECID_COMM_BTL_WIN,           ///< P2P対戦の勝利数               999,999,999
  RECID_COMM_BTL_LOSE,          ///< P2P対戦の敗北数               999,999,999
  RECID_WIFI_BATTLE,            ///< バトルスポットをした回数           999,999,999
  RECID_WIFI_BTL_WIN,           ///< バトルスポットの勝利数             999,999,999
  RECID_WIFI_BTL_LOSE,          ///< バトルスポットの敗北数             999,999,999
  RECID_SHOPPING_CNT,           ///< 買い物をした回数                         999,999,999
  RECID_SHOPPING_MONEY,         ///< 買い物で使った合計金額                   999,999,999
  RECID_SODATEYA_CNT,           ///< 育て屋にポケモンを預けた回数             999,999,999  20
  RECID_KILL_POKE,              ///< 倒したポケモンの数                       999,999,999
  RECID_DAYMAX_EXP,             ///< 1日に稼いだ経験値最大値                  999,999,999
  RECID_DAYCNT_EXP,             ///< 1日に稼いだ経験値                        999,999,999  
  RECID_GTS_PUT,                ///< GTSにポケモンを預けた回数                999,999
  RECID_NICKNAME,               ///< ポケモンにニックネームを付けた回数       999,999
  RECID_PREMIUM_BALL,           ///< もらったプレミアボールの数               999,999
  RECID_WIN_BP,                 ///< 勝ち取ったBP累計                         999,999,999
  RECID_USE_BP,                 ///< 使ったBP                                 999,999,999
  RECID_WEAK_ATTACK,            ///< 効果がバツグンのわざが命中した回数       999,999,999  
  RECID_CLOTHING_CNT,           ///< 着替えた回数                             999,999,999  30
  RECID_HAIR_ARRANGE,           ///< 美容室でヘアアレンジした回数             999,999,999
  RECID_BERRY_HARVEST,          ///< 畑で木の実を収穫した回数                 999,999,999
  RECID_GTS_TRADE_COUNT,        ///< GTSで通信交換をした回数                  999,999,999
  RECID_MIRACLE_TRADE_COUNT,    ///< ミラクル交換で通信交換をした回数         999,999,999
  RECID_QUICKMATCH_COUNT,       ///< クイックマッチ回数                       999,999,999
  RECID_RIDE_POKE,              ///< ポケモンライド回数                       999,999,999
  RECID_MAME_PRESENT_CNT,       ///< ポケモンに豆をあげた回数                 999,999,999
  RECID_JF_USE_JCOIN,           ///< ジョインフェスタで使ったコイン累計       999,999,999
  RECID_MAME_GET_CNT,           ///< ポケマメを集めた累計                     999,999,999    
  RECID_BTREE_CHALLENGE,        ///< バトルツリーに挑戦した回数               999,999,999  40    
  RECID_ZWAZA_CNT,				      ///< Ｚワザをくりだした回数                             999,999
  RECID_THROW_BALL,				      ///< モンスターボールを投げた回数                       999,999
  RECID_STEAL_ITEM,				      ///< 野生のポケモンからアイテムを盗んだ回数             999,999
  RECID_WAZA_CNT,				        ///< 技を使用した回数                                   999,999,999
  RECID_SODATEYA_LVUP_CNT,			///< 育て屋に預けたポケモンの累計ＬＶアップ数           999,999,999
  RECID_ESCAPE,				          ///< 野生のポケモンから逃げた回数                       999,999,999
  RECID_BRAKE_ITEM,				      ///< 岩を壊してアイテムを入手した回数                   999,999
  RECID_HEAL_KIDOU,				      ///< 回復アイテムを使用した回数                         999,999
  RECID_NEKONIKOBAN,				    ///< ネコに小判で取得したお金の金額                     999,999,999
  RECID_FINDER_POINT,				    ///< ポケファインダーの総得点                           999,999,999  50
  RECID_GURUGURU,				        ///< ぐるぐる回った回数                                 999,999
  RECID_FINDER_RECORD,				  ///< １枚の写真の最高得点                               999,999
  RECID_PARLE_NADE,				      ///< パルレでなでた回数                                 999,999
  RECID_POKERESORT,				      ///< ポケリゾートの起動回数                             999,999
  RECID_POKEMAME_TRADE,				  ///< ポケリゾートでポケマメを交換した数                 999,999
  RECID_POKEMON_TOUCH,				  ///< ポケリゾートでポケモンをタッチした数               999,999
  RECID_POKEMAME_IN,				    ///< ポケマメいれにポケマメを入れた数                   999,999
  RECID_POKERESORT_LVUP,				///< ポケリゾートでＬＶをあげたときの累計               999,999
  RECID_POKERESORT_NATSUKI,		  ///< ポケリゾートでなつき度を上げた累計                 999,999
  RECID_POKERESORT_TAMAGOFUKA,	///< ポケリゾートでタマゴ孵化歩数を下げた累計           999,999  60
  RECID_POKERESORT_KISO,				///< ポケリゾートで基礎ポイントをあげたときの累計       999,999
  RECID_QR_READ_BATTLE,				  ///< ＱＲ読み込みアプリ：ＱＲバトルチームを読み込んだ数 999,999
  RECID_BATTLEMOVIE,				    ///< バトルビデオの再生回数                             999,999
  RECID_BATTLEMOVIE_FIGHT,			///< バトルビデオで模擬線をした回数                     999,999
  RECID_ROTOM,				          ///< ロトム図鑑を触った回数                             999,999  65
  RECID_JF_GEST_TALK_CNT,       ///< ジョインフェスタで交流イベントをした回数         9,999,999
  RECID_NUTS,                   ///< 集まった木の実を調べた回数                         999,999
  RECID_MANY_NUTS,              ///< すごく集まった木の実を調べた回数                   999,999
  RECID_FISHING_ITEM,           ///< アイテムを釣り上げた回数                           999,999
  RECID_ROTOPON_CNT,            ///< ロトポンした回数                                   999,999,999
  RECID_RR_EVENT_FLAG,          ///< レインボーロケット団イベントクリアフラグ           999,999,999
  RECID_NUSIIRU_CNT,            ///< ヌシール取得数                                     999,999,999
  RECID_LARGE_MANTAINBP,        ///< マンタインサーフで稼いだBP                         999,999,999
  RECID_LARGE_BATTLEFES_WIN,    ///< バトルフェスで勝利した回数                         999,999,999



  /// 4バイトレコード余り。追加する時はここを書き換えていく事。
  /// ↓↓↓ レコード定義を変更したらrecord.cppのlimit_tblも変更すること！
  RECID_LARGE_75,  ///<
  RECID_LARGE_76,  ///<
  RECID_LARGE_77,  ///<
  RECID_LARGE_78,  ///<
  RECID_LARGE_79,  ///<
  RECID_LARGE_80,  ///<
  RECID_LARGE_81,  ///<
  RECID_LARGE_82,  ///<
  RECID_LARGE_83,  ///<
  RECID_LARGE_84,  ///<
  RECID_LARGE_85,  ///<
  RECID_LARGE_86,  ///<
  RECID_LARGE_87,  ///<
  RECID_LARGE_88,  ///<
  RECID_LARGE_89,  ///<
  RECID_LARGE_90,  ///<
  RECID_LARGE_91,  ///<
  RECID_LARGE_92,  ///<
  RECID_LARGE_93,  ///<
  RECID_LARGE_94,  ///<
  RECID_LARGE_95,  ///<
  RECID_LARGE_96,  ///<
  RECID_LARGE_97,  ///<
  RECID_LARGE_98,  ///<
  RECID_LARGE_99,  ///<

  RECID_LARGE_MAX, ///< RECID_LARGE総数 100



  // ここから２バイトレコード
  RECID_DENDOU_CNT =RECID_LARGE_MAX, ///< 殿堂入りした回数                         9,999  0
  RECID_RECOVERY_HOME,               ///< 自分の家で休んだ回数                     9,999
  RECID_WAZA_HANERU,                 ///< 「はねる」を使った回数                   9,999
  RECID_WAZA_WARUAGAKI,              ///< 「わるあがき」を使った回数               9,999
  RECID_WAZA_MUKOU,                  ///< 無効果わざを出した回数                   9,999
  RECID_TEMOTI_KIZETU,               ///< 気絶させた手持ちの匹数                   9,999
  RECID_TEMOTI_MAKIZOE,              ///< 味方にダメージを与えた回数               9,999
  RECID_NIGERU_SIPPAI,               ///< 逃げそこなった回数                       9,999
  RECID_NIGERARETA_FLAG,             ///< 逃げられたポケモンの数                   9,999
  RECID_FISHING_FAILURE,             ///< つり損なった回数                         9,999
  RECID_DAYMAX_KILL,                 ///< 1日に倒したポケモンの最高数              9,999  10
  RECID_DAYCNT_KILL,                 ///< 1日に倒したポケモンの数                  9,999
  RECID_DAYMAX_CAPTURE,              ///< 1日に捕獲したポケモンの最高数            9,999
  RECID_DAYCNT_CAPTURE,              ///< 1日に捕獲したポケモンの数                9,999
  RECID_DAYMAX_TRAINER_BATTLE,       ///< 1日にトレーナー戦した最高回数            9,999
  RECID_DAYCNT_TRAINER_BATTLE,       ///< 1日にトレーナー戦した回数                9,999
  RECID_DAYMAX_EVOLUTION,            ///< 1日にポケモン進化させた最高回数          9,999
  RECID_DAYCNT_EVOLUTION,            ///< 1日にポケモン進化させた回数              9,999
  RECID_FOSSIL_RESTORE,              ///< ポケモンの化石を復元した回数             9,999
  RECID_CAMERA_CNT,                  ///< カメラで写真撮影した回数                 9,999  
  RECID_BTREE_WIN_SINGLE,            ///< バトルツリー最高連勝数・シングル         9,999  20
  RECID_BTREE_WIN_DOUBLE,            ///< バトルツリー最高連勝数・ダブル           9,999
  RECID_BTREE_WIN_MULTI,             ///< バトルツリー最高連勝数・マルチ           9,999
  RECID_IDKUJI_ATARI_CNT,            ///< ＩＤくじに当たった回数                   9,999
  RECID_POINTUP_USE,                 ///< ポイントアップを使った回数               9,999
  RECID_KAWAIGARI_CNT,               ///< ポケモンかわいがり画面を開いた回数       9,999
  RECID_FISHING_SUCCESSION,          ///< 釣り・連続成功回数最高記録               999
  RECID_RARE_COLOUR_ENC_COUNT,       ///<  色違いのポケモンに出会った回数          65,535  
  RECID_JF_ATT_CNT,                  ///< ジョインフェスタでアトラクションに参加した回数  9,999
  RECID_JF_SHOP_GET_CNT,             ///< ジョインフェスタでお店を建てた回数              9,999
  RECID_QR_SCAN,                     ///< QRスキャンした回数                                          9,999 30
  RECID_LERAN_WAZA,                  ///< 技マシンを覚えさせた回数                                    9,999
  RECID_CAFE,                        ///< ポケモンセンター横のCAFÉで注文した回数                      9,999
  RECID_FINDER,                      ///< 自分のプロフィール写真を取り直した回数                      9,999
  RECID_CANCEL_EVOLUTION,            ///< 進化キャンセルの回数                                        9,999
  RECID_RANNNYU,                     ///< 乱入回数                                                    9,999
  RECID_NAKAYOSHI,                   ///< なかよし度ＭＡＸのポケモンの数                              9,999
  RECID_BATTLEROYAL,                 ///< バトルロイヤルを行った回数                                  9,999
  RECID_MONOHIROI,                   ///< 『ものひろい』でアイテムを拾った回数                        9,999
  RECID_MARASADA,                    ///< マラサダを食べた回数                                        9,999
  RECID_SUGOI_TRAINING,              ///< すごい特訓の回数                                            9,999 40
  RECID_BATTLE_VIKING,               ///< バトルバイキングで食事をした回数                            9,999
  RECID_PARLE_KIDOU,                 ///< ポケパルレ_起動回数                                         9,999
  RECID_BOX_CONTROL,                 ///< ポケモンボックスにアクセスした回数                          9,999
  RECID_LOMILOMI,                    ///< ロミロミをした回数                                          9,999
  RECID_IRIMA_BED,                   ///< イリマのベッドのにおいを嗅いだ回数                          9,999
  RECID_GUZMA_BED,                   ///< グズマのベッドのにおいを嗅いだ回数                          9,999
  RECID_KIAVE_BED,                   ///< キアヴェのベッドのにおいを嗅いだ回数                        9,999
  RECID_SUREIN_BED,                  ///< スレインのベッドのにおいを嗅いだ回数                        9,999
  RECID_MAOS_BED,                    ///< マオのベッドのにおいを嗅いだ回数                            9,999
  RECID_MAIRE_BED,                   ///< マイレのベッドのにおいを嗅いだ回数                          9,999 50
  RECID_HAPU_BED,                    ///< ハプウのベッドのにおいを嗅いだ回数                          9,999
  RECID_ZAIDANBOSS_BED,              ///< 財団代表のベッドのにおいを嗅いだ回数                        9,999
  RECID_DROP_ITEM,                   ///< ドロップアイテム入手回数                                    9,999
  RECID_KIYURE,                      ///< 木揺れエンカウント回数                                      9,999
  RECID_DANSA,                       ///< 段差ジャンプ回数                                            9,999
  RECID_UMI_ENCOUNT,                 ///< 海の特殊エンカウント回数                                    9,999
  RECID_SABAKU_ENCOUNT,              ///< 砂漠の特殊エンカウント回数                                  9,999
  RECID_KISEKAE_CNT,                 ///< 着せ替えの回数                                              9,999
  RECID_BATTLEROYAL_WIN,             ///< バトルロイヤルの勝利数                                      9,999
  RECID_RESORT_ITEM,                 ///< ポケリゾートでどうぐを入手した回数                          9,999 60
  RECID_RESORT_WAIWAI,               ///< ポケリゾート：わいわいリゾートに誘ったポケモンの回数        9,999
  RECID_RESORT_POKAPOKA,             ///< ポケリゾート：ぽかぽかリゾートに誘ったポケモンの回数        9,999
  RECID_QR_READ_PICTURE,             ///< ＱＲスキャン：ＱＲコード（図鑑ＱＲ）を読み込んだ回数        9,999
  RECID_QR_READ_POPULAR,             ///< ＱＲスキャン：ＱＲコード（一般）を読み込んだ回数            9,999
  RECID_QR_READ_OUTSIDE,             ///< ＱＲスキャン：ＱＲコード（外部連携系）を読み込んだ回数      9,999
  RECID_FIELDSCAN,                   ///< ＱＲスキャン：フィールドスキャンを発動した回数              9,999
  RECID_PLANT,                       ///<	植物からポケモンが飛び出してきた回数                        9,999
  RECID_SHADOW,                      ///<	影からポケモンが飛び出してきた回数                          9,999
  RECID_WEED,                        ///<	草のシンボルでポケモンと戦った回数                          9,999
  RECID_GROUND,                      ///<	土のシンボルでポケモンと戦った回数                          9,999 70
  RECID_KOSOKU,                      ///<	逃げるコソクムシに追いついた回数                            9,999
  RECID_YASHIGANI,                   ///<	木の実からポケモンが飛び出してきた回数（ヤシガニ）          9,999
  RECID_FISHING_BUBBLE,              ///<	泡の出ている釣りスポットで釣りを成功させた回数              9,999
  RECID_PLAYER_BED,                  ///< 主人公のベッドのにおいを嗅いだ回数                          9,999 

  RECID_GLOBAL_ATTRACTION_CAPTURE,        ///< グローバルアトラクション01　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_GTSTRADE,       ///< グローバルアトラクション02　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_EGG,            ///< グローバルアトラクション03　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_POKEMAME,       ///< グローバルアトラクション04　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_FESCOIN,        ///< グローバルアトラクション05 [momiji] RECID_JF_USE_JCOIN:        フェスサークルで フェスコインを　つかってあそぼう！     //  RECID_GLOBAL_ATTRACTION_FINDERPOINTS,   ///< グローバルアトラクション05　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_FIELDSCAN,      ///< グローバルアトラクション06　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_BATTLEFES,      ///< グローバルアトラクション07 [momiji] RECID_LARGE_BATTLEFES_WIN: バトルフェスで　しょうりしよう！                        //  RECID_GLOBAL_ATTRACTION_YASHIGANI,      ///< グローバルアトラクション07　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_MANTABP,        ///< グローバルアトラクション08 [momiji] RECID_LARGE_MANTAINBP:     マンタインサーフで ＢＰを稼ごう！                       //  RECID_GLOBAL_ATTRACTION_CHAMPION,       ///< グローバルアトラクション08　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_FISHINGPOKE,    ///< グローバルアトラクション09 [momiji] RECID_FISHING_POKEMON:     ポケモンを　たくさん釣り上げよう！                      //  RECID_GLOBAL_ATTRACTION_RAREFISHING,    ///< グローバルアトラクション09　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_BATTLEROYAL,    ///< グローバルアトラクション10 [momiji] RECID_BATTLEROYAL_WIN:     バトルロイヤルでしょうりしよう！（条件変更）            //  RECID_GLOBAL_ATTRACTION_BATTLEROYAL,    ///< グローバルアトラクション10　　　　　　　　　 　　　　　　　9,999 
  RECID_GLOBAL_ATTRACTION_EVOLUTION,      ///< グローバルアトラクション11 [momiji] RECID_POKE_EVOLUTION:      ポケモンをたくさん進化させよう！                        //  RECID_GLOBAL_ATTRACTION_LUCK,           ///< グローバルアトラクション11　　　　　　　　　 　　　　　　　9,999   　　　　　　　　　　
  RECID_GLOBAL_ATTRACTION_BATTLETREE,     ///< グローバルアトラクション12　　　　　　　　　 　　　　　　　9,999                                                                                                       

  RECID_LILIE_BED,                        ///< リーリエのベッドでリラックスをした回数                       9,999  
  RECID_LUNASOL_DIVE_CNT,                 ///< ルナソルダイブの回数			9,999
  RECID_MANTASURF_RIDE_CNT,               ///< マンタインサーフに乗った回数			9,999
  RECID_FINDERSTUDIO_FILM_CNT,            ///< ファインダースタジオで写真を保存した回数			9,999
  RECID_BATTLEFES_CHALLENGE_CNT,          ///< バトルフェスに挑戦した回数			9,999
  RECID_FIELD_POKE_CNT,                   ///< フィールドポケモンとふれあった回数			65,535
  RECID_SHIREN_OKAWARI_PAZZLE_CNT,        ///< 島３試練後のパズル挑戦回数			65,535
  RECID_SHIREN_OKAWARI_PAZZLE_SUCCESS_CNT,///< 島３試練後のパズルクリア回数			65,535
  RECID_MYALBUM_DECO_CNT,                 ///< マイアルバムデコった回数			9,999
  RECID_FINDER_STUDIO_SHUT_CNT,           ///< ファインダースタジオで撮影した回数			9,999
  RECID_LUNASOL_BESTSCORE,                ///< ルナソルダイブのベストスコア距離		65,535
  RECID_MANTAIN_SURF_BEST_BP,             ///< マンタインサーフベストBP		9,999

  /// 2バイトレコード余り。追加する時はここを書き換えていく事。
  /// ↓↓↓ レコード定義を変更したらrecord.cppのlimit_tblも変更すること！


  RECID_SMALL_99,   ///<
  RECID_ALL_MAX,

  RECID_4BYTE_LAST = RECID_LARGE_75,
  RECID_2BYTE_LAST = RECID_SMALL_99,
};

#endif    // __RECORD_DEFINE_H__

