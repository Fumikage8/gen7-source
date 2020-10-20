//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		StringWaza.cpp
 *	@brief  技名ソート
 *	@author	Toru=Nagihashi
 *	@date		2012.09.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// module
#include "AppLib/include/Sort/StringWaza.h"
// resource
#include "arc_index/sort_string.gaix"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(sort)

//-----------------------------------------------------------------------
// 定数
//-----------------------------------------------------------------------
const WazaNo StringWaza::SECRET_WAZA_TABLE[ SECRET_WAZA_TABLE_MAX ] =
{
  WAZANO_DAIYASUTOOMU,    // ダイヤストーム
  WAZANO_SUTIIMUBAASUTO,  // スチームバースト
  WAZANO_IZIGENHOORU,     // いじげんホール
//  WAZANO_SAUZANAROO,      // サウザンアロー     // @fix MMCat[235]: momijiでは表示
//  WAZANO_SAUZANWHEEBU,    // サウザンウェーブ   // @fix MMCat[235]: momijiでは表示
  WAZANO_HAMETUNOHIKARI,  // はめつのひかり @fix BTS10223
  WAZANO_WARUAGAKI,       // わるあがき @fix 6723
  WAZANO_HAPPIITAIMU,     // ハッピータイム（配布技）
  WAZANO_OIWAI,           // おいわい（配布技）
  WAZANO_TEWOTUNAGU,      // てをつなぐ（配布技）
  WAZANO_TEKAGEN,         // てかげん（配布技）
  WAZANO_IZIGENRASSYU,    // いじげんラッシュ ( add sango )
  WAZANO_HURUURUKANON,    // フルールカノン(add niji)
  WAZANO_SYADOOSUTIIRU,   // シャドースチール(add niji)
  // momiji追加分
  WAZANO_MABOROSIWAZA,    // マボロシ18わざ
};

// Z技テーブル(add niji)
const WazaNo StringWaza::SECRET_Z_WAZA_TABLE[ SECRET_Z_WAZA_TABLE_MAX ] =
{
  WAZANO_URUTORADASSYUATAKKU,       // ウルトラダッシュアタック
  WAZANO_NOOMARUZENRYOKU,           // ノーマルゼンリョク
  WAZANO_ZENRYOKUMUSOUGEKIRETUKEN,  // ぜんりょくむそうげきれつけん
  WAZANO_KAKUTOUZENRYOKU,           // かくとうゼンリョク
  WAZANO_FAINARUDAIBUATAKKU,        // ファイナルダイブアタック
  WAZANO_HIKOUZENRYOKU,             // ひこうゼンリョク
  WAZANO_ASIDDOPOIZUNDERIITO,       // アシッドポイズンデリート
  WAZANO_DOKUZENRYOKU,              // どくゼンリョク
  WAZANO_RAIZINGURANDOOOBAA,        // ライジングランドオーバー
  WAZANO_ZIMENZENRYOKU,             // じめんゼンリョク
  WAZANO_WAARUZUENDOFOORU,          // ワールズエンドフォール
  WAZANO_IWAZENRYOKU,               // いわゼンリョク
  WAZANO_ZETTAIHOSYOKUKAITENZAN,    // ぜったいほしょくかいてんざん
  WAZANO_MUSIZENRYOKU,              // むしゼンリョク
  WAZANO_MUGENANYAHENOIZANAI,       // むげんあんやへのいざない
  WAZANO_GOOSUTOZENRYOKU,           // ゴーストゼンリョク
  WAZANO_TYOUZETURASENRENGEKI,      // ちょうぜつらせんれんげき
  WAZANO_HAGANEZENRYOKU,            // はがねゼンリョク
  WAZANO_DAINAMIKKUHURUHUREIMU,     // ダイナミックフルフレイム
  WAZANO_HONOOZENRYOKU,             // ほのおゼンリョク
  WAZANO_HAIPAAAKUATORUNEEDO,       // ハイパーアクアトルネード
  WAZANO_MIZUZENRYOKU,              // みずゼンリョク
  WAZANO_BURUUMUSYAINEKUSUTORA,     // ブルームシャインエクストラ
  WAZANO_KUSAZENRYOKU,              // くさゼンリョク
  WAZANO_SUPAAKINGUGIGABORUTO,      // スパーキングギガボルト
  WAZANO_DENKIZENRYOKU,             // でんきゼンリョク
  WAZANO_MAKISIMAMUSAIBUREIKAA,     // マキシマムサイブレイカー
  WAZANO_ESUPAAZENRYOKU,            // エスパーゼンリョク
  WAZANO_REIZINGUZIOHURIIZU,        // レイジングジオフリーズ
  WAZANO_KOORIZENRYOKU,             // こおりゼンリョク
  WAZANO_ARUTHIMETTODORAGONBAAN,    // アルティメットドラゴンバーン
  WAZANO_DORAGONZENRYOKU,           // ドラゴンゼンリョク
  WAZANO_BURAKKUHOORUIKURIPUSU,     // ブラックホールイクリプス
  WAZANO_AKUZENRYOKU,               // あくゼンリョク
  WAZANO_RABURIISUTAAINPAKUTO,      // ラブリースターインパクト
  WAZANO_FEARIIZENRYOKU,            // フェアリーゼンリョク
  WAZANO_HISSATUNOPIKATYUUTO,       // ひっさつのピカチュート 
  WAZANO_SYADOUAROOZUSUTORAIKU,     // シャドウアローズストライク
  WAZANO_HAIPAADAAKUKURASSYAA,      // ハイパーダーククラッシャー
  WAZANO_WADATUMINOSINFONIA,        // わだつみのシンフォニア
  WAZANO_GAADHIANDEAROORA,          // ガーディアン・デ・アローラ
  WAZANO_HAISINSENYOUZENRYOKU,      // はいしんせんようぜんりょく
  WAZANO_RAITONINGUSAAHURAIDO,      // ライトニングサーフライド
  WAZANO_HONKIWODASUKOUGEKI,        // ほんきをだす　こうげき
  WAZANO_SEBUNEBORUBUUSUTO,         // セブンエボルブースト
  WAZANO_ORIZINZUSUUPAANOVHA,       // オリジンズスーパーノヴァ
  WAZANO_SATOPIKASENYOUZENRYOKU,    // サトピカせんようぜんりょく
  // momiji追加分
  WAZANO_SINNEKUROZUMAZETTO,        // シンネクロズマゼット
  WAZANO_ZENRYOKUMETEODORAIBU,      // ゼンリョクメテオドライブ
  WAZANO_ZENRYOKUSYADOOREI,         // ゼンリョクシャドーレイ
  WAZANO_ZENRYOKUZYARETUKU,         // ぜんりょくじゃれつく
  WAZANO_ZENRYOKUSUTOONEZZI,        // ぜんりょくストーンエッジ
  WAZANO_ZENRYOKUSUKEIRUNOIZU,      // ぜんりょくスケイルノイズ
};

//-----------------------------------------------------------------------
// 生成、破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
StringWaza::StringWaza()
  : StringBinary()
{
}
/**
 * @brief デストラクタ
 */
StringWaza::~StringWaza()
{
}

//-----------------------------------------------------------------------
// 読み込み
//-----------------------------------------------------------------------
/**
 * @brief 同期読み込み
 *
 * @param heap  ヒープ
 */
void StringWaza::Load( gfl2::heap::HeapBase * heap )
{
  this->StringBinary::LoadSync( heap,GARC_sort_string_waza_initial_index_DAT,GARC_sort_string_waza_sort_table_DAT, GARC_sort_string_waza_initial_to_sort_DAT );
}

/**
 * @brief 非同期読み込み設定
 *
 * @param heap  ヒープ
 * @param work_heap 作業用ヒープ
 *
 * @note  LoadMain()で読み込んでください
 */
void StringWaza::LoadSetup( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * work_heap )
{
  this->StringBinary::LoadSetup(
    heap, work_heap, GARC_sort_string_waza_initial_index_DAT, GARC_sort_string_waza_sort_table_DAT, GARC_sort_string_waza_initial_to_sort_DAT );
}

/**
 * @brief 隠す技かどうかチェック
 *
 * @param waza  技
 *
 * @return  trueならば隠す技  falseならば隠さない技
 */
bool StringWaza::IsSecretWaza( WazaNo waza ) const
{
  for( int i = 0; i < SECRET_WAZA_TABLE_MAX; ++i )
  {
    if( SECRET_WAZA_TABLE[i] == waza )
    {
      return true;
    }
  }
  for( int i = 0; i < SECRET_Z_WAZA_TABLE_MAX; ++i )
  {
    // Z技なら隠す
    if( SECRET_Z_WAZA_TABLE[i] == waza )
    {
      return true;
    }
  }

  return false;
}

GFL_NAMESPACE_END(sort)
GFL_NAMESPACE_END(app)
