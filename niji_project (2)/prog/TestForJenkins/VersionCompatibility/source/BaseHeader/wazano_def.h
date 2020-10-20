//==============================================================
/**
 * @file	wazano_def.h
 * @bfief	技NoのDefine定義ファイル
 * @author	WazaTableConverter
 * @note waza_tbl_conv.rb から生成されました
*/
//==============================================================
#if !defined __WAZANODEF_H__
#define __WAZANODEF_H__

enum WazaNo {
  WAZANO_NULL											= 0,  // －－－－－－－
  WAZANO_HATAKU										= 1,  // はたく
  WAZANO_KARATETYOPPU							= 2,  // からてチョップ
  WAZANO_OUHUKUBINTA							= 3,  // おうふくビンタ
  WAZANO_RENZOKUPANTI							= 4,  // れんぞくパンチ
  WAZANO_MEGATONPANTI							= 5,  // メガトンパンチ
  WAZANO_NEKONIKOBAN							= 6,  // ネコにこばん
  WAZANO_HONOONOPANTI							= 7,  // ほのおのパンチ
  WAZANO_REITOUPANTI							= 8,  // れいとうパンチ
  WAZANO_KAMINARIPANTI						= 9,  // かみなりパンチ
  WAZANO_HIKKAKU									= 10,  // ひっかく
  WAZANO_HASAMU										= 11,  // はさむ
  WAZANO_HASAMIGIROTIN						= 12,  // ハサミギロチン
  WAZANO_KAMAITATI								= 13,  // かまいたち
  WAZANO_TURUGINOMAI							= 14,  // つるぎのまい
  WAZANO_IAIGIRI									= 15,  // いあいぎり
  WAZANO_KAZEOKOSI								= 16,  // かぜおこし
  WAZANO_TUBASADEUTU							= 17,  // つばさでうつ
  WAZANO_HUKITOBASI								= 18,  // ふきとばし
  WAZANO_SORAWOTOBU								= 19,  // そらをとぶ
  WAZANO_SIMETUKERU								= 20,  // しめつける
  WAZANO_TATAKITUKERU							= 21,  // たたきつける
  WAZANO_TURUNOMUTI								= 22,  // つるのムチ
  WAZANO_HUMITUKE									= 23,  // ふみつけ
  WAZANO_NIDOGERI									= 24,  // にどげり
  WAZANO_MEGATONKIKKU							= 25,  // メガトンキック
  WAZANO_TOBIGERI									= 26,  // とびげり
  WAZANO_MAWASIGERI								= 27,  // まわしげり
  WAZANO_SUNAKAKE									= 28,  // すなかけ
  WAZANO_ZUTUKI										= 29,  // ずつき
  WAZANO_TUNODETUKU								= 30,  // つのでつく
  WAZANO_MIDAREDUKI								= 31,  // みだれづき
  WAZANO_TUNODORIRU								= 32,  // つのドリル
  WAZANO_TAIATARI									= 33,  // たいあたり
  WAZANO_NOSIKAKARI								= 34,  // のしかかり
  WAZANO_MAKITUKU									= 35,  // まきつく
  WAZANO_TOSSIN										= 36,  // とっしん
  WAZANO_ABARERU									= 37,  // あばれる
  WAZANO_SUTEMITAKKURU						= 38,  // すてみタックル
  WAZANO_SIPPOWOHURU							= 39,  // しっぽをふる
  WAZANO_DOKUBARI									= 40,  // どくばり
  WAZANO_DABURUNIIDORU						= 41,  // ダブルニードル
  WAZANO_MISAIRUBARI							= 42,  // ミサイルばり
  WAZANO_NIRAMITUKERU							= 43,  // にらみつける
  WAZANO_KAMITUKU									= 44,  // かみつく
  WAZANO_NAKIGOE									= 45,  // なきごえ
  WAZANO_HOERU										= 46,  // ほえる
  WAZANO_UTAU											= 47,  // うたう
  WAZANO_TYOUONPA									= 48,  // ちょうおんぱ
  WAZANO_SONIKKUBUUMU							= 49,  // ソニックブーム
  WAZANO_KANASIBARI								= 50,  // かなしばり
  WAZANO_YOUKAIEKI								= 51,  // ようかいえき
  WAZANO_HINOKO										= 52,  // ひのこ
  WAZANO_KAENHOUSYA								= 53,  // かえんほうしゃ
  WAZANO_SIROIKIRI								= 54,  // しろいきり
  WAZANO_MIZUDEPPOU								= 55,  // みずでっぽう
  WAZANO_HAIDOROPONPU							= 56,  // ハイドロポンプ
  WAZANO_NAMINORI									= 57,  // なみのり
  WAZANO_REITOUBIIMU							= 58,  // れいとうビーム
  WAZANO_HUBUKI										= 59,  // ふぶき
  WAZANO_SAIKEKOUSEN							= 60,  // サイケこうせん
  WAZANO_BABURUKOUSEN							= 61,  // バブルこうせん
  WAZANO_OORORABIIMU							= 62,  // オーロラビーム
  WAZANO_HAKAIKOUSEN							= 63,  // はかいこうせん
  WAZANO_TUTUKU										= 64,  // つつく
  WAZANO_DORIRUKUTIBASI						= 65,  // ドリルくちばし
  WAZANO_ZIGOKUGURUMA							= 66,  // じごくぐるま
  WAZANO_KETAGURI									= 67,  // けたぐり
  WAZANO_KAUNTAA									= 68,  // カウンター
  WAZANO_TIKYUUNAGE								= 69,  // ちきゅうなげ
  WAZANO_KAIRIKI									= 70,  // かいりき
  WAZANO_SUITORU									= 71,  // すいとる
  WAZANO_MEGADOREIN								= 72,  // メガドレイン
  WAZANO_YADORIGINOTANE						= 73,  // やどりぎのタネ
  WAZANO_SEITYOU									= 74,  // せいちょう
  WAZANO_HAPPAKATTAA							= 75,  // はっぱカッター
  WAZANO_SOORAABIIMU							= 76,  // ソーラービーム
  WAZANO_DOKUNOKONA								= 77,  // どくのこな
  WAZANO_SIBIREGONA								= 78,  // しびれごな
  WAZANO_NEMURIGONA								= 79,  // ねむりごな
  WAZANO_HANABIRANOMAI						= 80,  // はなびらのまい
  WAZANO_ITOWOHAKU								= 81,  // いとをはく
  WAZANO_RYUUNOIKARI							= 82,  // りゅうのいかり
  WAZANO_HONOONOUZU								= 83,  // ほのおのうず
  WAZANO_DENKISYOKKU							= 84,  // でんきショック
  WAZANO_10MANBORUTO							= 85,  // １０まんボルト
  WAZANO_DENZIHA									= 86,  // でんじは
  WAZANO_KAMINARI									= 87,  // かみなり
  WAZANO_IWAOTOSI									= 88,  // いわおとし
  WAZANO_ZISIN										= 89,  // じしん
  WAZANO_ZIWARE										= 90,  // じわれ
  WAZANO_ANAWOHORU								= 91,  // あなをほる
  WAZANO_DOKUDOKU									= 92,  // どくどく
  WAZANO_NENRIKI									= 93,  // ねんりき
  WAZANO_SAIKOKINESISU						= 94,  // サイコキネシス
  WAZANO_SAIMINZYUTU							= 95,  // さいみんじゅつ
  WAZANO_YOGANOPOOZU							= 96,  // ヨガのポーズ
  WAZANO_KOUSOKUIDOU							= 97,  // こうそくいどう
  WAZANO_DENKOUSEKKA							= 98,  // でんこうせっか
  WAZANO_IKARI										= 99,  // いかり
  WAZANO_TEREPOOTO								= 100,  // テレポート
  WAZANO_NAITOHEDDO								= 101,  // ナイトヘッド
  WAZANO_MONOMANE									= 102,  // ものまね
  WAZANO_IYANAOTO									= 103,  // いやなおと
  WAZANO_KAGEBUNSIN								= 104,  // かげぶんしん
  WAZANO_ZIKOSAISEI								= 105,  // じこさいせい
  WAZANO_KATAKUNARU								= 106,  // かたくなる
  WAZANO_TIISAKUNARU							= 107,  // ちいさくなる
  WAZANO_ENMAKU										= 108,  // えんまく
  WAZANO_AYASIIHIKARI							= 109,  // あやしいひかり
  WAZANO_KARANIKOMORU							= 110,  // からにこもる
  WAZANO_MARUKUNARU								= 111,  // まるくなる
  WAZANO_BARIAA										= 112,  // バリアー
  WAZANO_HIKARINOKABE							= 113,  // ひかりのかべ
  WAZANO_KUROIKIRI								= 114,  // くろいきり
  WAZANO_RIHUREKUTAA							= 115,  // リフレクター
  WAZANO_KIAIDAME									= 116,  // きあいだめ
  WAZANO_GAMAN										= 117,  // がまん
  WAZANO_YUBIWOHURU								= 118,  // ゆびをふる
  WAZANO_OUMUGAESI								= 119,  // オウムがえし
  WAZANO_ZIBAKU										= 120,  // じばく
  WAZANO_TAMAGOBAKUDAN						= 121,  // タマゴばくだん
  WAZANO_SITADENAMERU							= 122,  // したでなめる
  WAZANO_SUMOGGU									= 123,  // スモッグ
  WAZANO_HEDOROKOUGEKI						= 124,  // ヘドロこうげき
  WAZANO_HONEKONBOU								= 125,  // ホネこんぼう
  WAZANO_DAIMONZI									= 126,  // だいもんじ
  WAZANO_TAKINOBORI								= 127,  // たきのぼり
  WAZANO_KARADEHASAMU							= 128,  // からではさむ
  WAZANO_SUPIIDOSUTAA							= 129,  // スピードスター
  WAZANO_ROKETTOZUTUKI						= 130,  // ロケットずつき
  WAZANO_TOGEKYANON								= 131,  // とげキャノン
  WAZANO_KARAMITUKU								= 132,  // からみつく
  WAZANO_DOWASURE									= 133,  // ドわすれ
  WAZANO_SUPUUNMAGE								= 134,  // スプーンまげ
  WAZANO_TAMAGOUMI								= 135,  // タマゴうみ
  WAZANO_TOBIHIZAGERI							= 136,  // とびひざげり
  WAZANO_HEBINIRAMI								= 137,  // へびにらみ
  WAZANO_YUMEKUI									= 138,  // ゆめくい
  WAZANO_DOKUGASU									= 139,  // どくガス
  WAZANO_TAMANAGE									= 140,  // たまなげ
  WAZANO_KYUUKETU									= 141,  // きゅうけつ
  WAZANO_AKUMANOKISSU							= 142,  // あくまのキッス
  WAZANO_GODDOBAADO								= 143,  // ゴッドバード
  WAZANO_HENSIN										= 144,  // へんしん
  WAZANO_AWA											= 145,  // あわ
  WAZANO_PIYOPIYOPANTI						= 146,  // ピヨピヨパンチ
  WAZANO_KINOKONOHOUSI						= 147,  // キノコのほうし
  WAZANO_HURASSYU									= 148,  // フラッシュ
  WAZANO_SAIKOWHEEBU							= 149,  // サイコウェーブ
  WAZANO_HANERU										= 150,  // はねる
  WAZANO_TOKERU										= 151,  // とける
  WAZANO_KURABUHANMAA							= 152,  // クラブハンマー
  WAZANO_DAIBAKUHATU							= 153,  // だいばくはつ
  WAZANO_MIDAREHIKKAKI						= 154,  // みだれひっかき
  WAZANO_HONEBUUMERAN							= 155,  // ホネブーメラン
  WAZANO_NEMURU										= 156,  // ねむる
  WAZANO_IWANADARE								= 157,  // いわなだれ
  WAZANO_HISSATUMAEBA							= 158,  // ひっさつまえば
  WAZANO_KAKUBARU									= 159,  // かくばる
  WAZANO_TEKUSUTYAA								= 160,  // テクスチャー
  WAZANO_TORAIATAKKU							= 161,  // トライアタック
  WAZANO_IKARINOMAEBA							= 162,  // いかりのまえば
  WAZANO_KIRISAKU									= 163,  // きりさく
  WAZANO_MIGAWARI									= 164,  // みがわり
  WAZANO_WARUAGAKI								= 165,  // わるあがき
  WAZANO_SUKETTI									= 166,  // スケッチ
  WAZANO_TORIPURUKIKKU						= 167,  // トリプルキック
  WAZANO_DOROBOU									= 168,  // どろぼう
  WAZANO_KUMONOSU									= 169,  // クモのす
  WAZANO_KOKORONOME								= 170,  // こころのめ
  WAZANO_AKUMU										= 171,  // あくむ
  WAZANO_KAENGURUMA								= 172,  // かえんぐるま
  WAZANO_IBIKI										= 173,  // いびき
  WAZANO_NOROI										= 174,  // のろい
  WAZANO_ZITABATA									= 175,  // じたばた
  WAZANO_TEKUSUTYAA2							= 176,  // テクスチャー２
  WAZANO_EAROBURASUTO							= 177,  // エアロブラスト
  WAZANO_WATAHOUSI								= 178,  // わたほうし
  WAZANO_KISIKAISEI								= 179,  // きしかいせい
  WAZANO_URAMI										= 180,  // うらみ
  WAZANO_KONAYUKI									= 181,  // こなゆき
  WAZANO_MAMORU										= 182,  // まもる
  WAZANO_MAHHAPANTI								= 183,  // マッハパンチ
  WAZANO_KOWAIKAO									= 184,  // こわいかお
  WAZANO_DAMASIUTI								= 185,  // だましうち
  WAZANO_TENSINOKISSU							= 186,  // てんしのキッス
  WAZANO_HARADAIKO								= 187,  // はらだいこ
  WAZANO_HEDOROBAKUDAN						= 188,  // ヘドロばくだん
  WAZANO_DOROKAKE									= 189,  // どろかけ
  WAZANO_OKUTANHOU								= 190,  // オクタンほう
  WAZANO_MAKIBISI									= 191,  // まきびし
  WAZANO_DENZIHOU									= 192,  // でんじほう
  WAZANO_MIYABURU									= 193,  // みやぶる
  WAZANO_MITIDURE									= 194,  // みちづれ
  WAZANO_HOROBINOUTA							= 195,  // ほろびのうた
  WAZANO_KOGOERUKAZE							= 196,  // こごえるかぜ
  WAZANO_MIKIRI										= 197,  // みきり
  WAZANO_BOONRASSYU								= 198,  // ボーンラッシュ
  WAZANO_ROKKUON									= 199,  // ロックオン
  WAZANO_GEKIRIN									= 200,  // げきりん
  WAZANO_SUNAARASI								= 201,  // すなあらし
  WAZANO_GIGADOREIN								= 202,  // ギガドレイン
  WAZANO_KORAERU									= 203,  // こらえる
  WAZANO_AMAERU										= 204,  // あまえる
  WAZANO_KOROGARU									= 205,  // ころがる
  WAZANO_MINEUTI									= 206,  // みねうち
  WAZANO_IBARU										= 207,  // いばる
  WAZANO_MIRUKUNOMI								= 208,  // ミルクのみ
  WAZANO_SUPAAKU									= 209,  // スパーク
  WAZANO_RENZOKUGIRI							= 210,  // れんぞくぎり
  WAZANO_HAGANENOTUBASA						= 211,  // はがねのつばさ
  WAZANO_KUROIMANAZASI						= 212,  // くろいまなざし
  WAZANO_MEROMERO									= 213,  // メロメロ
  WAZANO_NEGOTO										= 214,  // ねごと
  WAZANO_IYASINOSUZU							= 215,  // いやしのすず
  WAZANO_ONGAESI									= 216,  // おんがえし
  WAZANO_PUREZENTO								= 217,  // プレゼント
  WAZANO_YATUATARI								= 218,  // やつあたり
  WAZANO_SINPINOMAMORI						= 219,  // しんぴのまもり
  WAZANO_ITAMIWAKE								= 220,  // いたみわけ
  WAZANO_SEINARUHONOO							= 221,  // せいなるほのお
  WAZANO_MAGUNITYUUDO							= 222,  // マグニチュード
  WAZANO_BAKURETUPANTI						= 223,  // ばくれつパンチ
  WAZANO_MEGAHOON									= 224,  // メガホーン
  WAZANO_RYUUNOIBUKI							= 225,  // りゅうのいぶき
  WAZANO_BATONTATTI								= 226,  // バトンタッチ
  WAZANO_ANKOORU									= 227,  // アンコール
  WAZANO_OIUTI										= 228,  // おいうち
  WAZANO_KOUSOKUSUPIN							= 229,  // こうそくスピン
  WAZANO_AMAIKAORI								= 230,  // あまいかおり
  WAZANO_AIANTEERU								= 231,  // アイアンテール
  WAZANO_METARUKUROO							= 232,  // メタルクロー
  WAZANO_ATEMINAGE								= 233,  // あてみなげ
  WAZANO_ASANOHIZASI							= 234,  // あさのひざし
  WAZANO_KOUGOUSEI								= 235,  // こうごうせい
  WAZANO_TUKINOHIKARI							= 236,  // つきのひかり
  WAZANO_MEZAMERUPAWAA						= 237,  // めざめるパワー
  WAZANO_KUROSUTYOPPU							= 238,  // クロスチョップ
  WAZANO_TATUMAKI									= 239,  // たつまき
  WAZANO_AMAGOI										= 240,  // あまごい
  WAZANO_NIHONBARE								= 241,  // にほんばれ
  WAZANO_KAMIKUDAKU								= 242,  // かみくだく
  WAZANO_MIRAAKOOTO								= 243,  // ミラーコート
  WAZANO_ZIKOANZI									= 244,  // じこあんじ
  WAZANO_SINSOKU									= 245,  // しんそく
  WAZANO_GENSINOTIKARA						= 246,  // げんしのちから
  WAZANO_SYADOOBOORU							= 247,  // シャドーボール
  WAZANO_MIRAIYOTI								= 248,  // みらいよち
  WAZANO_IWAKUDAKI								= 249,  // いわくだき
  WAZANO_UZUSIO										= 250,  // うずしお
  WAZANO_HUKURODATAKI							= 251,  // ふくろだたき
  WAZANO_NEKODAMASI								= 252,  // ねこだまし
  WAZANO_SAWAGU										= 253,  // さわぐ
  WAZANO_TAKUWAERU								= 254,  // たくわえる
  WAZANO_HAKIDASU									= 255,  // はきだす
  WAZANO_NOMIKOMU									= 256,  // のみこむ
  WAZANO_NEPPUU										= 257,  // ねっぷう
  WAZANO_ARARE										= 258,  // あられ
  WAZANO_ITYAMON									= 259,  // いちゃもん
  WAZANO_ODATERU									= 260,  // おだてる
  WAZANO_ONIBI										= 261,  // おにび
  WAZANO_OKIMIYAGE								= 262,  // おきみやげ
  WAZANO_KARAGENKI								= 263,  // からげんき
  WAZANO_KIAIPANTI								= 264,  // きあいパンチ
  WAZANO_KITUKE										= 265,  // きつけ
  WAZANO_KONOYUBITOMARE						= 266,  // このゆびとまれ
  WAZANO_SIZENNOTIKARA						= 267,  // しぜんのちから
  WAZANO_ZYUUDEN									= 268,  // じゅうでん
  WAZANO_TYOUHATU									= 269,  // ちょうはつ
  WAZANO_TEDASUKE									= 270,  // てだすけ
  WAZANO_TORIKKU									= 271,  // トリック
  WAZANO_NARIKIRI									= 272,  // なりきり
  WAZANO_NEGAIGOTO								= 273,  // ねがいごと
  WAZANO_NEKONOTE									= 274,  // ねこのて
  WAZANO_NEWOHARU									= 275,  // ねをはる
  WAZANO_BAKADIKARA								= 276,  // ばかぢから
  WAZANO_MAZIKKUKOOTO							= 277,  // マジックコート
  WAZANO_RISAIKURU								= 278,  // リサイクル
  WAZANO_RIBENZI									= 279,  // リベンジ
  WAZANO_KAWARAWARI								= 280,  // かわらわり
  WAZANO_AKUBI										= 281,  // あくび
  WAZANO_HATAKIOTOSU							= 282,  // はたきおとす
  WAZANO_GAMUSYARA								= 283,  // がむしゃら
  WAZANO_HUNKA										= 284,  // ふんか
  WAZANO_SUKIRUSUWAPPU						= 285,  // スキルスワップ
  WAZANO_HUUIN										= 286,  // ふういん
  WAZANO_RIHURESSYU								= 287,  // リフレッシュ
  WAZANO_ONNEN										= 288,  // おんねん
  WAZANO_YOKODORI									= 289,  // よこどり
  WAZANO_HIMITUNOTIKARA						= 290,  // ひみつのちから
  WAZANO_DAIBINGU									= 291,  // ダイビング
  WAZANO_TUPPARI									= 292,  // つっぱり
  WAZANO_HOGOSYOKU								= 293,  // ほごしょく
  WAZANO_HOTARUBI									= 294,  // ほたるび
  WAZANO_RASUTAAPAAZI							= 295,  // ラスターパージ
  WAZANO_MISUTOBOORU							= 296,  // ミストボール
  WAZANO_FEZAADANSU								= 297,  // フェザーダンス
  WAZANO_HURAHURADANSU						= 298,  // フラフラダンス
  WAZANO_BUREIZUKIKKU							= 299,  // ブレイズキック
  WAZANO_DOROASOBI								= 300,  // どろあそび
  WAZANO_AISUBOORU								= 301,  // アイスボール
  WAZANO_NIIDORUAAMU							= 302,  // ニードルアーム
  WAZANO_NAMAKERU									= 303,  // なまける
  WAZANO_HAIPAABOISU							= 304,  // ハイパーボイス
  WAZANO_DOKUDOKUNOKIBA						= 305,  // どくどくのキバ
  WAZANO_BUREIKUKUROO							= 306,  // ブレイククロー
  WAZANO_BURASUTOBAAN							= 307,  // ブラストバーン
  WAZANO_HAIDOROKANON							= 308,  // ハイドロカノン
  WAZANO_KOMETTOPANTI							= 309,  // コメットパンチ
  WAZANO_ODOROKASU								= 310,  // おどろかす
  WAZANO_WHEZAABOORU							= 311,  // ウェザーボール
  WAZANO_AROMASERAPII							= 312,  // アロマセラピー
  WAZANO_USONAKI									= 313,  // うそなき
  WAZANO_EAKATTAA									= 314,  // エアカッター
  WAZANO_OOBAAHIITO								= 315,  // オーバーヒート
  WAZANO_KAGIWAKERU								= 316,  // かぎわける
  WAZANO_GANSEKIHUUZI							= 317,  // がんせきふうじ
  WAZANO_GINIRONOKAZE							= 318,  // ぎんいろのかぜ
  WAZANO_KINZOKUON								= 319,  // きんぞくおん
  WAZANO_KUSABUE									= 320,  // くさぶえ
  WAZANO_KUSUGURU									= 321,  // くすぐる
  WAZANO_KOSUMOPAWAA							= 322,  // コスモパワー
  WAZANO_SIOHUKI									= 323,  // しおふき
  WAZANO_SIGUNARUBIIMU						= 324,  // シグナルビーム
  WAZANO_SYADOOPANTI							= 325,  // シャドーパンチ
  WAZANO_ZINTUURIKI								= 326,  // じんつうりき
  WAZANO_SUKAIAPPAA								= 327,  // スカイアッパー
  WAZANO_SUNAZIGOKU								= 328,  // すなじごく
  WAZANO_ZETTAIREIDO							= 329,  // ぜったいれいど
  WAZANO_DAKURYUU									= 330,  // だくりゅう
  WAZANO_TANEMASINGAN							= 331,  // タネマシンガン
  WAZANO_TUBAMEGAESI							= 332,  // つばめがえし
  WAZANO_TURARABARI								= 333,  // つららばり
  WAZANO_TEPPEKI									= 334,  // てっぺき
  WAZANO_TOOSENBOU								= 335,  // とおせんぼう
  WAZANO_TOOBOE										= 336,  // とおぼえ
  WAZANO_DORAGONKUROO							= 337,  // ドラゴンクロー
  WAZANO_HAADOPURANTO							= 338,  // ハードプラント
  WAZANO_BIRUDOAPPU								= 339,  // ビルドアップ
  WAZANO_TOBIHANERU								= 340,  // とびはねる
  WAZANO_MADDOSYOTTO							= 341,  // マッドショット
  WAZANO_POIZUNTEERU							= 342,  // ポイズンテール
  WAZANO_HOSIGARU									= 343,  // ほしがる
  WAZANO_BORUTEKKAA								= 344,  // ボルテッカー
  WAZANO_MAZIKARURIIHU						= 345,  // マジカルリーフ
  WAZANO_MIZUASOBI								= 346,  // みずあそび
  WAZANO_MEISOU										= 347,  // めいそう
  WAZANO_RIIHUBUREEDO							= 348,  // リーフブレード
  WAZANO_RYUUNOMAI								= 349,  // りゅうのまい
  WAZANO_ROKKUBURASUTO						= 350,  // ロックブラスト
  WAZANO_DENGEKIHA								= 351,  // でんげきは
  WAZANO_MIZUNOHADOU							= 352,  // みずのはどう
  WAZANO_HAMETUNONEGAI						= 353,  // はめつのねがい
  WAZANO_SAIKOBUUSUTO							= 354,  // サイコブースト
  WAZANO_HANEYASUME								= 355,  // はねやすめ
  WAZANO_ZYUURYOKU								= 356,  // じゅうりょく
  WAZANO_MIRAKURUAI								= 357,  // ミラクルアイ
  WAZANO_MEZAMASIBINTA						= 358,  // めざましビンタ
  WAZANO_AAMUHANMAA								= 359,  // アームハンマー
  WAZANO_ZYAIROBOORU							= 360,  // ジャイロボール
  WAZANO_IYASINONEGAI							= 361,  // いやしのねがい
  WAZANO_SIOMIZU									= 362,  // しおみず
  WAZANO_SIZENNOMEGUMI						= 363,  // しぜんのめぐみ
  WAZANO_FEINTO										= 364,  // フェイント
  WAZANO_TUIBAMU									= 365,  // ついばむ
  WAZANO_OIKAZE										= 366,  // おいかぜ
  WAZANO_TUBOWOTUKU								= 367,  // つぼをつく
  WAZANO_METARUBAASUTO						= 368,  // メタルバースト
  WAZANO_TONBOGAERI								= 369,  // とんぼがえり
  WAZANO_INFAITO									= 370,  // インファイト
  WAZANO_SIPPEGAESI								= 371,  // しっぺがえし
  WAZANO_DAMEOSI									= 372,  // ダメおし
  WAZANO_SASIOSAE									= 373,  // さしおさえ
  WAZANO_NAGETUKERU								= 374,  // なげつける
  WAZANO_SAIKOSIHUTO							= 375,  // サイコシフト
  WAZANO_KIRIHUDA									= 376,  // きりふだ
  WAZANO_KAIHUKUHUUZI							= 377,  // かいふくふうじ
  WAZANO_SIBORITORU								= 378,  // しぼりとる
  WAZANO_PAWAATORIKKU							= 379,  // パワートリック
  WAZANO_IEKI											= 380,  // いえき
  WAZANO_OMAZINAI									= 381,  // おまじない
  WAZANO_SAKIDORI									= 382,  // さきどり
  WAZANO_MANEKKO									= 383,  // まねっこ
  WAZANO_PAWAASUWAPPU							= 384,  // パワースワップ
  WAZANO_GAADOSUWAPPU							= 385,  // ガードスワップ
  WAZANO_OSIOKI										= 386,  // おしおき
  WAZANO_TOTTEOKI									= 387,  // とっておき
  WAZANO_NAYAMINOTANE							= 388,  // なやみのタネ
  WAZANO_HUIUTI										= 389,  // ふいうち
  WAZANO_DOKUBISI									= 390,  // どくびし
  WAZANO_HAATOSUWAPPU							= 391,  // ハートスワップ
  WAZANO_AKUARINGU								= 392,  // アクアリング
  WAZANO_DENZIHUYUU								= 393,  // でんじふゆう
  WAZANO_HUREADORAIBU							= 394,  // フレアドライブ
  WAZANO_HAKKEI										= 395,  // はっけい
  WAZANO_HADOUDAN									= 396,  // はどうだん
  WAZANO_ROKKUKATTO								= 397,  // ロックカット
  WAZANO_DOKUDUKI									= 398,  // どくづき
  WAZANO_AKUNOHADOU								= 399,  // あくのはどう
  WAZANO_TUZIGIRI									= 400,  // つじぎり
  WAZANO_AKUATEERU								= 401,  // アクアテール
  WAZANO_TANEBAKUDAN							= 402,  // タネばくだん
  WAZANO_EASURASSYU								= 403,  // エアスラッシュ
  WAZANO_SIZAAKUROSU							= 404,  // シザークロス
  WAZANO_MUSINOSAZAMEKI						= 405,  // むしのさざめき
  WAZANO_RYUUNOHADOU							= 406,  // りゅうのはどう
  WAZANO_DORAGONDAIBU							= 407,  // ドラゴンダイブ
  WAZANO_PAWAAJEMU								= 408,  // パワージェム
  WAZANO_DOREINPANTI							= 409,  // ドレインパンチ
  WAZANO_SINKUUHA									= 410,  // しんくうは
  WAZANO_KIAIDAMA									= 411,  // きあいだま
  WAZANO_ENAZIIBOORU							= 412,  // エナジーボール
  WAZANO_BUREIBUBAADO							= 413,  // ブレイブバード
  WAZANO_DAITINOTIKARA						= 414,  // だいちのちから
  WAZANO_SURIKAE									= 415,  // すりかえ
  WAZANO_GIGAINPAKUTO							= 416,  // ギガインパクト
  WAZANO_WARUDAKUMI								= 417,  // わるだくみ
  WAZANO_BARETTOPANTI							= 418,  // バレットパンチ
  WAZANO_YUKINADARE								= 419,  // ゆきなだれ
  WAZANO_KOORINOTUBUTE						= 420,  // こおりのつぶて
  WAZANO_SYADOOKUROO							= 421,  // シャドークロー
  WAZANO_KAMINARINOKIBA						= 422,  // かみなりのキバ
  WAZANO_KOORINOKIBA							= 423,  // こおりのキバ
  WAZANO_HONOONOKIBA							= 424,  // ほのおのキバ
  WAZANO_KAGEUTI									= 425,  // かげうち
  WAZANO_DOROBAKUDAN							= 426,  // どろばくだん
  WAZANO_SAIKOKATTAA							= 427,  // サイコカッター
  WAZANO_SINENNOZUTUKI						= 428,  // しねんのずつき
  WAZANO_MIRAASYOTTO							= 429,  // ミラーショット
  WAZANO_RASUTAAKANON							= 430,  // ラスターカノン
  WAZANO_ROKKUKURAIMU							= 431,  // ロッククライム
  WAZANO_KIRIBARAI								= 432,  // きりばらい
  WAZANO_TORIKKURUUMU							= 433,  // トリックルーム
  WAZANO_RYUUSEIGUN								= 434,  // りゅうせいぐん
  WAZANO_HOUDEN										= 435,  // ほうでん
  WAZANO_HUNEN										= 436,  // ふんえん
  WAZANO_RIIHUSUTOOMU							= 437,  // リーフストーム
  WAZANO_PAWAAWHIPPU							= 438,  // パワーウィップ
  WAZANO_GANSEKIHOU								= 439,  // がんせきほう
  WAZANO_KUROSUPOIZUN							= 440,  // クロスポイズン
  WAZANO_DASUTOSYUUTO							= 441,  // ダストシュート
  WAZANO_AIANHEDDO								= 442,  // アイアンヘッド
  WAZANO_MAGUNETTOBOMU						= 443,  // マグネットボム
  WAZANO_SUTOONEZZI								= 444,  // ストーンエッジ
  WAZANO_YUUWAKU									= 445,  // ゆうわく
  WAZANO_SUTERUSUROKKU						= 446,  // ステルスロック
  WAZANO_KUSAMUSUBI								= 447,  // くさむすび
  WAZANO_OSYABERI									= 448,  // おしゃべり
  WAZANO_SABAKINOTUBUTE						= 449,  // さばきのつぶて
  WAZANO_MUSIKUI									= 450,  // むしくい
  WAZANO_TYAAZIBIIMU							= 451,  // チャージビーム
  WAZANO_UDDOHANMAA								= 452,  // ウッドハンマー
  WAZANO_AKUAJETTO								= 453,  // アクアジェット
  WAZANO_KOUGEKISIREI							= 454,  // こうげきしれい
  WAZANO_BOUGYOSIREI							= 455,  // ぼうぎょしれい
  WAZANO_KAIHUKUSIREI							= 456,  // かいふくしれい
  WAZANO_MOROHANOZUTUKI						= 457,  // もろはのずつき
  WAZANO_DABURUATAKKU							= 458,  // ダブルアタック
  WAZANO_TOKINOHOUKOU							= 459,  // ときのほうこう
  WAZANO_AKUUSETUDAN							= 460,  // あくうせつだん
  WAZANO_MIKADUKINOMAI						= 461,  // みかづきのまい
  WAZANO_NIGIRITUBUSU							= 462,  // にぎりつぶす
  WAZANO_MAGUMASUTOOMU						= 463,  // マグマストーム
  WAZANO_DAAKUHOORU								= 464,  // ダークホール
  WAZANO_SIIDOHUREA								= 465,  // シードフレア
  WAZANO_AYASIIKAZE								= 466,  // あやしいかぜ
  WAZANO_SYADOODAIBU							= 467,  // シャドーダイブ
  WAZANO_TUMETOGI									= 468,  // つめとぎ
  WAZANO_WAIDOGAADO								= 469,  // ワイドガード
  WAZANO_GAADOSHEA								= 470,  // ガードシェア
  WAZANO_PAWAASHEA								= 471,  // パワーシェア
  WAZANO_WANDAARUUMU							= 472,  // ワンダールーム
  WAZANO_SAIKOSYOKKU							= 473,  // サイコショック
  WAZANO_BENOMUSYOKKU							= 474,  // ベノムショック
  WAZANO_BODHIPAAZI								= 475,  // ボディパージ
  WAZANO_IKARINOKONA							= 476,  // いかりのこな
  WAZANO_TEREKINESISU							= 477,  // テレキネシス
  WAZANO_MAZIKKURUUMU							= 478,  // マジックルーム
  WAZANO_UTIOTOSU									= 479,  // うちおとす
  WAZANO_YAMAARASI								= 480,  // やまあらし
  WAZANO_HAZIKERUHONOO						= 481,  // はじけるほのお
  WAZANO_HEDOROWHEEBU							= 482,  // ヘドロウェーブ
  WAZANO_TYOUNOMAI								= 483,  // ちょうのまい
  WAZANO_HEBIIBONBAA							= 484,  // ヘビーボンバー
  WAZANO_SINKURONOIZU							= 485,  // シンクロノイズ
  WAZANO_EREKIBOORU								= 486,  // エレキボール
  WAZANO_MIZUBITASI								= 487,  // みずびたし
  WAZANO_NITOROTYAAZI							= 488,  // ニトロチャージ
  WAZANO_TOGUROWOMAKU							= 489,  // とぐろをまく
  WAZANO_ROOKIKKU									= 490,  // ローキック
  WAZANO_ASIDDOBOMU								= 491,  // アシッドボム
  WAZANO_IKASAMA									= 492,  // イカサマ
  WAZANO_SINPURUBIIMU							= 493,  // シンプルビーム
  WAZANO_NAKAMADUKURI							= 494,  // なかまづくり
  WAZANO_OSAKINIDOUZO							= 495,  // おさきにどうぞ
  WAZANO_RINSYOU									= 496,  // りんしょう
  WAZANO_EKOOBOISU								= 497,  // エコーボイス
  WAZANO_NASIKUZUSI								= 498,  // なしくずし
  WAZANO_KURIASUMOGGU							= 499,  // クリアスモッグ
  WAZANO_ASISUTOPAWAA							= 500,  // アシストパワー
  WAZANO_FASUTOGAADO							= 501,  // ファストガード
  WAZANO_SAIDOTHENZI							= 502,  // サイドチェンジ
  WAZANO_NETTOU										= 503,  // ねっとう
  WAZANO_KARAWOYABURU							= 504,  // からをやぶる
  WAZANO_IYASINOHADOU							= 505,  // いやしのはどう
  WAZANO_TATARIME									= 506,  // たたりめ
  WAZANO_HURIIFOORU								= 507,  // フリーフォール
  WAZANO_GIATHENZI								= 508,  // ギアチェンジ
  WAZANO_TOMOENAGE								= 509,  // ともえなげ
  WAZANO_YAKITUKUSU								= 510,  // やきつくす
  WAZANO_SAKIOKURI								= 511,  // さきおくり
  WAZANO_AKUROBATTO								= 512,  // アクロバット
  WAZANO_MIRAATAIPU								= 513,  // ミラータイプ
  WAZANO_KATAKIUTI								= 514,  // かたきうち
  WAZANO_INOTIGAKE								= 515,  // いのちがけ
  WAZANO_GIHUTOPASU								= 516,  // ギフトパス
  WAZANO_RENGOKU									= 517,  // れんごく
  WAZANO_MIZUNOTIKAI							= 518,  // みずのちかい
  WAZANO_HONOONOTIKAI							= 519,  // ほのおのちかい
  WAZANO_KUSANOTIKAI							= 520,  // くさのちかい
  WAZANO_BORUTOTHENZI							= 521,  // ボルトチェンジ
  WAZANO_MUSINOTEIKOU							= 522,  // むしのていこう
  WAZANO_ZINARASI									= 523,  // じならし
  WAZANO_KOORINOIBUKI							= 524,  // こおりのいぶき
  WAZANO_DORAGONTEERU							= 525,  // ドラゴンテール
  WAZANO_HURUITATERU							= 526,  // ふるいたてる
  WAZANO_EREKINETTO								= 527,  // エレキネット
  WAZANO_WAIRUDOBORUTO						= 528,  // ワイルドボルト
  WAZANO_DORIRURAINAA							= 529,  // ドリルライナー
  WAZANO_DABURUTYOPPU							= 530,  // ダブルチョップ
  WAZANO_HAATOSUTANPU							= 531,  // ハートスタンプ
  WAZANO_UDDOHOON									= 532,  // ウッドホーン
  WAZANO_SEINARUTURUGI						= 533,  // せいなるつるぎ
  WAZANO_SHERUBUREEDO							= 534,  // シェルブレード
  WAZANO_HIITOSUTANPU							= 535,  // ヒートスタンプ
  WAZANO_GURASUMIKISAA						= 536,  // グラスミキサー
  WAZANO_HAADOROORAA							= 537,  // ハードローラー
  WAZANO_KOTTONGAADO							= 538,  // コットンガード
  WAZANO_NAITOBAASUTO							= 539,  // ナイトバースト
  WAZANO_SAIKOBUREIKU							= 540,  // サイコブレイク
  WAZANO_SUIIPUBINTA							= 541,  // スイープビンタ
  WAZANO_BOUHUU										= 542,  // ぼうふう
  WAZANO_AHUROBUREIKU							= 543,  // アフロブレイク
  WAZANO_GIASOOSAA								= 544,  // ギアソーサー
  WAZANO_KAENDAN									= 545,  // かえんだん
  WAZANO_TEKUNOBASUTAA						= 546,  // テクノバスター
  WAZANO_INISIENOUTA							= 547,  // いにしえのうた
  WAZANO_SINPINOTURUGI						= 548,  // しんぴのつるぎ
  WAZANO_KOGOERUSEKAI							= 549,  // こごえるせかい
  WAZANO_RAIGEKI									= 550,  // らいげき
  WAZANO_AOIHONOO									= 551,  // あおいほのお
  WAZANO_HONOONOMAI								= 552,  // ほのおのまい
  WAZANO_HURIIZUBORUTO						= 553,  // フリーズボルト
  WAZANO_KOORUDOHUREA							= 554,  // コールドフレア
  WAZANO_BAAKUAUTO								= 555,  // バークアウト
  WAZANO_TURARAOTOSI							= 556,  // つららおとし
  WAZANO_vJENEREETO								= 557,  // Ｖジェネレート
  WAZANO_KUROSUHUREIMU						= 558,  // クロスフレイム
  WAZANO_KUROSUSANDAA							= 559,  // クロスサンダー
  WAZANO_HURAINGUPURESU						= 560,  // フライングプレス
  WAZANO_TATAMIGAESI							= 561,  // たたみがえし
  WAZANO_GEPPU										= 562,  // ゲップ
  WAZANO_TAGAYASU									= 563,  // たがやす
  WAZANO_NEBANEBANETTO						= 564,  // ねばねばネット
  WAZANO_TODOMEBARI								= 565,  // とどめばり
  WAZANO_GOOSUTODAIBU							= 566,  // ゴーストダイブ
  WAZANO_HAROWHIN									= 567,  // ハロウィン
  WAZANO_OTAKEBI									= 568,  // おたけび
  WAZANO_PURAZUMASYAWAA						= 569,  // プラズマシャワー
  WAZANO_PARABORATYAAZI						= 570,  // パラボラチャージ
  WAZANO_MORINONOROI							= 571,  // もりののろい
  WAZANO_HANAHUBUKI								= 572,  // はなふぶき
  WAZANO_HURIIZUDORAI							= 573,  // フリーズドライ
  WAZANO_TYAAMUBOISU							= 574,  // チャームボイス
  WAZANO_SUTEZERIHU								= 575,  // すてゼリフ
  WAZANO_HIKKURIKAESU							= 576,  // ひっくりかえす
  WAZANO_DOREINKISSU							= 577,  // ドレインキッス
  WAZANO_TORIKKUGAADO							= 578,  // トリックガード
  WAZANO_HURAWAAGAADO							= 579,  // フラワーガード
  WAZANO_GURASUFIIRUDO						= 580,  // グラスフィールド
  WAZANO_MISUTOFIIRUDO						= 581,  // ミストフィールド
  WAZANO_SOUDEN										= 582,  // そうでん
  WAZANO_ZYARETUKU								= 583,  // じゃれつく
  WAZANO_YOUSEINOKAZE							= 584,  // ようせいのかぜ
  WAZANO_MUUNFOOSU								= 585,  // ムーンフォース
  WAZANO_BAKUONPA									= 586,  // ばくおんぱ
  WAZANO_FEARIIROKKU							= 587,  // フェアリーロック
  WAZANO_KINGUSIIRUDO							= 588,  // キングシールド
  WAZANO_NAKAYOKUSURU							= 589,  // なかよくする
  WAZANO_NAISYOBANASI							= 590,  // ないしょばなし
  WAZANO_DAIYASUTOOMU							= 591,  // ダイヤストーム
  WAZANO_SUTIIMUBAASUTO						= 592,  // スチームバースト
  WAZANO_IZIGENHOORU							= 593,  // いじげんホール
  WAZANO_MIZUSYURIKEN							= 594,  // みずしゅりけん
  WAZANO_MAZIKARUHUREIMU					= 595,  // マジカルフレイム
  WAZANO_NIIDORUGAADO							= 596,  // ニードルガード
  WAZANO_AROMAMISUTO							= 597,  // アロマミスト
  WAZANO_KAIDENPA									= 598,  // かいでんぱ
  WAZANO_BENOMUTORAPPU						= 599,  // ベノムトラップ
  WAZANO_HUNZIN										= 600,  // ふんじん
  WAZANO_ZIOKONTOROORU						= 601,  // ジオコントロール
  WAZANO_ZIBASOUSA								= 602,  // じばそうさ
  WAZANO_HAPPIITAIMU							= 603,  // ハッピータイム
  WAZANO_EREKIFIIRUDO							= 604,  // エレキフィールド
  WAZANO_MAZIKARUSYAIN						= 605,  // マジカルシャイン
  WAZANO_OIWAI										= 606,  // おいわい
  WAZANO_TEWOTUNAGU								= 607,  // てをつなぐ
  WAZANO_TUBURANAHITOMI						= 608,  // つぶらなひとみ
  WAZANO_HOPPESURISURI						= 609,  // ほっぺすりすり
  WAZANO_TEKAGEN									= 610,  // てかげん
  WAZANO_MATOWARITUKU							= 611,  // まとわりつく
  WAZANO_GUROUPANTI								= 612,  // グロウパンチ
  WAZANO_DESUUINGU								= 613,  // デスウイング
  WAZANO_SAUZANAROO								= 614,  // サウザンアロー
  WAZANO_SAUZANWHEEBU							= 615,  // サウザンウェーブ
  WAZANO_GURANDOFOOSU							= 616,  // グランドフォース
  WAZANO_HAMETUNOHIKARI						= 617,  // はめつのひかり
  WAZANO_KONGENNOHADOU						= 618,  // こんげんのはどう
  WAZANO_DANGAINOTURUGI						= 619,  // だんがいのつるぎ
  WAZANO_GARYOUTENSEI							= 620,  // ガリョウテンセイ
  WAZANO_IZIGENRASSYU							= 621,  // いじげんラッシュ
  WAZANO_URUTORADASSYUATAKKU			= 622,  // ウルトラダッシュアタック
  WAZANO_NOOMARUZENRYOKU					= 623,  // ノーマルゼンリョク
  WAZANO_ZENRYOKUMUSOUGEKIRETUKEN	= 624,  // ぜんりょくむそうげきれつけん
  WAZANO_KAKUTOUZENRYOKU					= 625,  // かくとうゼンリョク
  WAZANO_FAINARUDAIBUATAKKU				= 626,  // ファイナルダイブアタック
  WAZANO_HIKOUZENRYOKU						= 627,  // ひこうゼンリョク
  WAZANO_ASIDDOPOIZUNDERIITO			= 628,  // アシッドポイズンデリート
  WAZANO_DOKUZENRYOKU							= 629,  // どくゼンリョク
  WAZANO_RAIZINGURANDOOOBAA				= 630,  // ライジングランドオーバー
  WAZANO_ZIMENZENRYOKU						= 631,  // じめんゼンリョク
  WAZANO_WAARUZUENDOFOORU					= 632,  // ワールズエンドフォール
  WAZANO_IWAZENRYOKU							= 633,  // いわゼンリョク
  WAZANO_ZETTAIHOSYOKUKAITENZAN		= 634,  // ぜったいほしょくかいてんざん
  WAZANO_MUSIZENRYOKU							= 635,  // むしゼンリョク
  WAZANO_MUGENANYAHENOIZANAI			= 636,  // むげんあんやへのいざない
  WAZANO_GOOSUTOZENRYOKU					= 637,  // ゴーストゼンリョク
  WAZANO_TYOUZETURASENRENGEKI			= 638,  // ちょうぜつらせんれんげき
  WAZANO_HAGANEZENRYOKU						= 639,  // はがねゼンリョク
  WAZANO_DAINAMIKKUHURUHUREIMU		= 640,  // ダイナミックフルフレイム
  WAZANO_HONOOZENRYOKU						= 641,  // ほのおゼンリョク
  WAZANO_HAIPAAAKUATORUNEEDO			= 642,  // ハイパーアクアトルネード
  WAZANO_MIZUZENRYOKU							= 643,  // みずゼンリョク
  WAZANO_BURUUMUSYAINEKUSUTORA		= 644,  // ブルームシャインエクストラ
  WAZANO_KUSAZENRYOKU							= 645,  // くさゼンリョク
  WAZANO_SUPAAKINGUGIGABORUTO			= 646,  // スパーキングギガボルト
  WAZANO_DENKIZENRYOKU						= 647,  // でんきゼンリョク
  WAZANO_MAKISIMAMUSAIBUREIKAA		= 648,  // マキシマムサイブレイカー
  WAZANO_ESUPAAZENRYOKU						= 649,  // エスパーゼンリョク
  WAZANO_REIZINGUZIOHURIIZU				= 650,  // レイジングジオフリーズ
  WAZANO_KOORIZENRYOKU						= 651,  // こおりゼンリョク
  WAZANO_ARUTHIMETTODORAGONBAAN		= 652,  // アルティメットドラゴンバーン
  WAZANO_DORAGONZENRYOKU					= 653,  // ドラゴンゼンリョク
  WAZANO_BURAKKUHOORUIKURIPUSU		= 654,  // ブラックホールイクリプス
  WAZANO_AKUZENRYOKU							= 655,  // あくゼンリョク
  WAZANO_RABURIISUTAAINPAKUTO			= 656,  // ラブリースターインパクト
  WAZANO_FEARIIZENRYOKU						= 657,  // フェアリーゼンリョク
  WAZANO_HISSATUNOPIKATYUUTO			= 658,  // ひっさつのピカチュート
  WAZANO_SUNAATUME								= 659,  // すなあつめ
  WAZANO_DEAIGASIRA								= 660,  // であいがしら
  WAZANO_TOOTIKA									= 661,  // トーチカ
  WAZANO_KAGENUI									= 662,  // かげぬい
  WAZANO_ddRARIATTO								= 663,  // ＤＤラリアット
  WAZANO_UTAKATANOARIA						= 664,  // うたかたのアリア
  WAZANO_AISUHANMAA								= 665,  // アイスハンマー
  WAZANO_HURAWAAHIIRU							= 666,  // フラワーヒール
  WAZANO_10MANBARIKI							= 667,  // １０まんばりき
  WAZANO_TIKARAWOSUITORU					= 668,  // ちからをすいとる
  WAZANO_SOORAABUREEDO						= 669,  // ソーラーブレード
  WAZANO_KONOHA										= 670,  // このは
  WAZANO_SUPOTTORAITO							= 671,  // スポットライト
  WAZANO_DOKUNOITO								= 672,  // どくのいと
  WAZANO_TOGISUMASU								= 673,  // とぎすます
  WAZANO_ASISUTOGIA								= 674,  // アシストギア
  WAZANO_ZIGOKUDUKI								= 675,  // じごくづき
  WAZANO_KAHUNDANGO								= 676,  // かふんだんご
  WAZANO_ANKAASYOTTO							= 677,  // アンカーショット
  WAZANO_SAIKOFIIRUDO							= 678,  // サイコフィールド
  WAZANO_TOBIKAKARU								= 679,  // とびかかる
  WAZANO_HONOONOMUTI							= 680,  // ほのおのむち
  WAZANO_TUKEAGARU								= 681,  // つけあがる
  WAZANO_MOETUKIRU								= 682,  // もえつきる
  WAZANO_SUPIIDOSUWAPPU						= 683,  // スピードスワップ
  WAZANO_SUMAATOHOON							= 684,  // スマートホーン
  WAZANO_ZYOUKA										= 685,  // じょうか
  WAZANO_MEZAMERUDANSU						= 686,  // めざめるダンス
  WAZANO_KOAPANISSYAA							= 687,  // コアパニッシャー
  WAZANO_TOROPIKARUKIKKU					= 688,  // トロピカルキック
  WAZANO_SAIHAI										= 689,  // さいはい
  WAZANO_KUTIBASIKYANON						= 690,  // くちばしキャノン
  WAZANO_SUKEIRUNOIZU							= 691,  // スケイルノイズ
  WAZANO_DORAGONHANMAA						= 692,  // ドラゴンハンマー
  WAZANO_BUNMAWASU								= 693,  // ぶんまわす
  WAZANO_OORORABEERU							= 694,  // オーロラベール
  WAZANO_SYADOUAROOZUSUTORAIKU		= 695,  // シャドウアローズストライク
  WAZANO_HAIPAADAAKUKURASSYAA			= 696,  // ハイパーダーククラッシャー
  WAZANO_WADATUMINOSINFONIA				= 697,  // わだつみのシンフォニア
  WAZANO_GAADHIANDEAROORA					= 698,  // ガーディアン・デ・アローラ
  WAZANO_HAISINSENYOUZENRYOKU			= 699,  // はいしんせんようぜんりょく
  WAZANO_RAITONINGUSAAHURAIDO			= 700,  // ライトニングサーフライド
  WAZANO_HONKIWODASUKOUGEKI				= 701,  // ほんきをだす　こうげき
  WAZANO_SEBUNEBORUBUUSUTO				= 702,  // セブンエボルブースト
  WAZANO_ORIZINZUSUUPAANOVHA			= 703,  // オリジンズスーパーノヴァ
  WAZANO_TORAPPUSHERU							= 704,  // トラップシェル
  WAZANO_HURUURUKANON							= 705,  // フルールカノン
  WAZANO_SAIKOFANGU								= 706,  // サイコファング
  WAZANO_ZIDANDA									= 707,  // じだんだ
  WAZANO_SYADOOBOON								= 708,  // シャドーボーン
  WAZANO_AKUSERUROKKU							= 709,  // アクセルロック
  WAZANO_AKUABUREIKU							= 710,  // アクアブレイク
  WAZANO_PURIZUMUREEZAA						= 711,  // プリズムレーザー
  WAZANO_SYADOOSUTIIRU						= 712,  // シャドースチール
  WAZANO_METEODORAIBU							= 713,  // メテオドライブ
  WAZANO_SYADOOREI								= 714,  // シャドーレイ
  WAZANO_NAMIDAME									= 715,  // なみだめ
  WAZANO_BIRIBIRITIKUTIKU					= 716,  // びりびりちくちく
  WAZANO_SIZENNOIKARI							= 717,  // しぜんのいかり
  WAZANO_MARUTIATAKKU							= 718,  // マルチアタック
  WAZANO_SATOPIKASENYOUZENRYOKU		= 719,  // サトピカせんようぜんりょく
  WAZANO_HANABISENYOU							= 720,  // はなびせんよう
  WAZANO_MABOROSIWAZA							= 721,  // マボロシ18わざ
  WAZANO_HIKARIDORAGON						= 722,  // ひかりドラゴン
  WAZANO_SINNEKUROZUMAZETTO				= 723,  // シンネクロズマゼット
  WAZANO_ZENRYOKUMETEODORAIBU			= 724,  // ゼンリョクメテオドライブ
  WAZANO_ZENRYOKUSYADOOREI				= 725,  // ゼンリョクシャドーレイ
  WAZANO_ZENRYOKUZYARETUKU				= 726,  // ぜんりょくじゃれつく
  WAZANO_ZENRYOKUSUTOONEZZI				= 727,  // ぜんりょくストーンエッジ
  WAZANO_ZENRYOKUSUKEIRUNOIZU			= 728,  // ぜんりょくスケイルノイズ
  WAZANO_MAX											= 729,
};

#endif  // __WAZANODEF_H__
