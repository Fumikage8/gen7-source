//==============================================================================
/**
 * @file	JoinFestaPawnDefine.h
 * @brief	ジョインフェスタPawnから参照する各種定義
 * @author	ichiraku_katsuhiko
 * @date	2015/11/09　12:47
 */
// =============================================================================
#if !defined (__JOIN_FESTA_PAWN_DEFINE_H__)
#define __JOIN_FESTA_PAWN_DEFINE_H__


#if USE_PAWN == 0
namespace JoinFestaScript {
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

  enum{
    //! ポケマメのアイテムID（アイテムではないが、抽選テーブルに入っている為定義切る）
    POKEMAME_ITEM_ID = 9999,
  };

  /**
   * @enum PersonalEventType
   *       パーソナルイベント種類
   */
  enum PersonalEventType{
    PERSONAL_EVENT_NULL,                //! イベント無効値
    PERSONAL_EVENT_RECENT,              //! 最近起きた事をはなす
    PERSONAL_EVENT_RECORD_BOAST,        //! レコード自慢
    PERSONAL_EVENT_RECORD_GAME,         //! レコード勝負
    PERSONAL_EVENT_RECORD_INTRODUCTION, //! レコード紹介
    PERSONAL_EVENT_RECORD_GUESS,        //! ズバリ当てろ・レコード
    PERSONAL_EVENT_RECOMMENDATION_SHOP0,//! オススメお店案内（おなかがへったよ）
    PERSONAL_EVENT_RECOMMENDATION_SHOP1,//! オススメお店案内（かいものしたい）
    PERSONAL_EVENT_RECOMMENDATION_SHOP2,//! オススメお店案内（ポケモン楽しませたい）
    PERSONAL_EVENT_RECOMMENDATION_SHOP3,//! オススメお店案内（すごいわざを　たいけんしたい）
    PERSONAL_EVENT_RECOMMENDATION_SHOP4,//! オススメお店案内（自分へのごほうび）
    PERSONAL_EVENT_RECOMMENDATION_SHOP5,//! オススメお店案内（ドキドキしたい）
    PERSONAL_EVENT_RECOMMENDATION_SHOP6,//! オススメお店案内（わたしのきもちわかる？店員男）
    PERSONAL_EVENT_RECOMMENDATION_SHOP7,//! オススメお店案内（わたしのきもちわかる？店員女）
    PERSONAL_EVENT_RECOMMENDATION_SHOP8,//! オススメお店案内（ポケモンを遊ばせたい）
    PERSONAL_EVENT_RECOMMENDATION_PERSON,//! オススメ人案内
    PERSONAL_EVENT_QUESTIONNAIRE_GUESS, //! ズバリ当てろ・アンケート
    PERSONAL_EVENT_QUESTIONNAIRE_Q,     //! アンケート質問
    PERSONAL_EVENT_THANKS,              //! さっきはありがとう
    PERSONAL_EVENT_DRESSUP_BOAST,       //! 着せ替え自慢
    PERSONAL_EVENT_DRESSUP_Q,           //! 着せ替え質問
    PERSONAL_EVENT_DRESSUP_GUESS,       //! ズバリ当てろ・着せ替え
    PERSONAL_EVENT_SECRET,              //! 私のヒミツがしりたい？
    PERSONAL_EVENT_FES_BOAST,           //! フェス自慢

    PERSONAL_EVENT_MAX
  };

  /**
   * @enum RecordCategory
   *       レコードのカテゴリ
   */
  enum RecordCategory
  {
    RECORD_CATEGORY_BATTLE,
    RECORD_CATEGORY_TRADE,
    RECORD_CATEGORY_GREED,
    RECORD_CATEGORY_POKEMON,
    RECORD_CATEGORY_DAILY,
    RECORD_CATEGORY_CIRCLE,
    RECORD_CATEGORY_VARIETY,
    RECORD_CATEGORY_TOTAL,
    RECORD_CATEGORY_MAX
  };

  /**
   * @enum PatternType
   *       定型文種類
   */
  enum PatternType{
    PATTERN_HELLO,    //!< 定型：あいさつ
    PATTERN_PARTING,  //!< 定型：別れ
    PATTERN_EMOTION,  //!< 定型：感動
    PATTERN_REGRET,   //!< 定型：残念
    PATTERN_SECRET,   //!< 定型：ナイショ
  };

  /**
   * @enum RewardType
   *       ご褒美種類
   */
  enum RewardType{
    REWARD_ATTRACTION,          //!< アトラクション開放
    REWARD_COMMAND_SHOP,        //!< コマンド追加：「いいみせない？」
    REWARD_COMMAND_DRESSUP,     //!< コマンド追加：「そのふくすてき」
    REWARD_NAMING,              //!< ネーミングライツ
    REWARD_PREMIUM,             //!< プレミアムサービス追加
    REWARD_BGM,                 //!< BGM変更機能追加
    REWARD_THEME1,              //!< ビルド機能テーマ１追加
    REWARD_THEME2,              //!< ビルド機能テーマ２追加
    REWARD_THEME3,              //!< ビルド機能テーマ３追加
    REWARD_SELECT_WORD,         //!< 定型文授与
    REWARD_PRESENT_SHOP,        //!< お店の授与

    REWARD_MAX,
  };

  /**
   * @enum RewardStateType
   *       ご褒美状態
   */
  enum RewardStateType{
    REWARD_STATE_NULL,                  //!< 初期状態
    REWARD_STATE_SATISFY_CONDITION,     //!< 条件を満たした
    REWARD_STATE_FINISH,                //!< ご褒美イベントをこなした
  };

  /**
   * @enum CommRouteType
   *       通信経路
   */
  enum CommRouteType{
    COMM_ROUTE_BEACON,          //!< すれ違い（ビーコン）
    COMM_ROUTE_SUBSCRIPTION,    //!< すれ違い（サブスクリプション）
    COMM_ROUTE_GTS,             //!< GTS
    COMM_ROUTE_MIRACLE_TRADE,   //!< ミラクル交換
    COMM_ROUTE_RANDOM_BATTLE,   //!< ランダムマッチ
    COMM_ROUTE_P2P_TRADE,       //!< 通信交換
    COMM_ROUTE_P2P_BATTLE,      //!< 通信対戦
    COMM_ROUTE_QUICK,           //!< クイック通信（GFNMCat[1628]対処）

    COMM_ROUTE_MAX,
    COMM_ROUTE_NULL = COMM_ROUTE_MAX, //!< 無効値
  };

  /**
   * @enum FacilitiesType
   *       施設種類
   */
  enum FacilitiesType{
    FACILITIES_TYPE_LOTTERY,    //! くじ引き屋
    FACILITIES_TYPE_AIR_TOY,    //! エア遊具
    FACILITIES_TYPE_FUN_HOUSE,  //! ビックリハウス
    FACILITIES_TYPE_STALL,      //! 屋台
    FACILITIES_TYPE_GIFT_SHOP,  //! ギフトショップ
    FACILITIES_TYPE_FORTUNE,    //! 占い
    FACILITIES_TYPE_DYEING,     //! 染物屋
    FACILITIES_TYPE_TRADE,      //! こうかんじょ(momiji追加)

    FACILITIES_TYPE_MAX
  };

  /**
   * @enum FacilitiesIndex
   *       施設インデックス（逆時計回りで0オリジン)
   */
  enum FacilitiesIndex{
    FACILITIES_INDEX_0,
    FACILITIES_INDEX_1,
    FACILITIES_INDEX_2,
    FACILITIES_INDEX_3,
    FACILITIES_INDEX_4,
    FACILITIES_INDEX_5,
    FACILITIES_INDEX_6,

    FACILITIES_INDEX_MAX,
    FACILITIES_INDEX_NULL = FACILITIES_INDEX_MAX, //! 無効値
  };

  /**
   * @enum FacilitiesColor
   *       施設カラー
   */
  enum FacilitiesColor{
    FACILITIES_COLOR_1,
    FACILITIES_COLOR_2,
    FACILITIES_COLOR_3,
    FACILITIES_COLOR_4,

    // 染物屋用
    FACILITIES_COLOR_RED = FACILITIES_COLOR_1,
    FACILITIES_COLOR_GREEN,
    FACILITIES_COLOR_ORANGE,
    FACILITIES_COLOR_PURPLE,
    FACILITIES_COLOR_YELLOW,
    FACILITIES_COLOR_BLUE,
    FACILITIES_COLOR_NAVY,
    FACILITIES_COLOR_PINK,

    FACILITIES_COLOR_MAX,
  };

  /**
   * @enum FortuneResultHit
   *       占いの結果：当たり
   */
  enum FortuneResultHit{
    FORTUNE_RESULT_HIT_NULL,          //! 無効値（占いやっていない）
    FORTUNE_RESULT_HIT_SMALL,         //! 当たり
    FORTUNE_RESULT_HIT_MIDDLE,        //! 中当たり
    FORTUNE_RESULT_HIT_BIG,           //! 大当たり
  };

  /**
   * @enum FortuneResultAction
   *       占い結果:行動
   */
  enum FortuneResultAction{
    FORTUNE_RESULT_ACTION_NULL,       //! 無効値（占いやっていない）
    FORTUNE_RESULT_ACTION_GTS,        //! GTS
    FORTUNE_RESULT_ACTION_MIRACLE,    //! ミラクル交換
    FORTUNE_RESULT_ACTION_BTLSPOT,    //! バトルスポット
    FORTUNE_RESULT_ACTION_FESCIRCLE,  //! フェスサークル
    FORTUNE_RESULT_ACTION_ATTRACTION, //! アトラクション
    FORTUNE_RESULT_ACTION_LOTTERY,    //! くじや
    FORTUNE_RESULT_ACTION_FUN_HOUSE,  //! ビックリハウス

    FORTUNE_RESULT_ACTION_MAX,
  };


  /**
   * @enum AttractionId
   *       アトラクションID
   */
  enum AttractionId{
    ATTRACTION_RIDE_KENTAROSU_1,            //!< いわをこわせ！　スプリント！
    ATTRACTION_RIDE_KAIRIKY_1,              //!< みんなに　みせつけろ！　スプリント！
    ATTRACTION_RECOMMENDATION_SHOP_1,       //!< くじやへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_2,       //!< エアゆうぐへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_3,       //!< ビックリハウスへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_4,       //!< やたいへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_5,       //!< ギフトショップへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_6,       //!< うらないやへ　おきゃくを　おくれ！
    ATTRACTION_RECOMMENDATION_SHOP_7,       //!< そめものやへ　おきゃくを　おくれ！
    ATTRACTION_LANGUAGE_LESSON_JP,          //!< にほんご　レッスン
    ATTRACTION_LANGUAGE_LESSON_EN,          //!< えいご　レッスン
    ATTRACTION_LANGUAGE_LESSON_FR,          //!< フランスご　レッスン
    ATTRACTION_LANGUAGE_LESSON_IT,          //!< イタリアご　レッスン
    ATTRACTION_LANGUAGE_LESSON_GE,          //!< ドイツご　レッスン
    ATTRACTION_LANGUAGE_LESSON_SP,          //!< スペインご　レッスン
    ATTRACTION_LANGUAGE_LESSON_KR,          //!< かんこくご　レッスン
    ATTRACTION_LANGUAGE_LESSON_CT,          //!< はんたいじ　かんたいじ　レッスン
    ATTRACTION_LANGUAGE_1,                  //!< いぶんか　こうりゅう！
    ATTRACTION_AFFINITY_1,                  //!< タイプあいしょう しんだん！
    ATTRACTION_AFFINITY_SAKASA_1,           //!< サカサプあいしょう しんだん！
    ATTRACTION_TYPE_SEARCH_ALOLA_1,         //!< ノーマルタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_1,               //!< ノーマルタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_2,         //!< かくとうタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_2,               //!< かくとうタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_3,         //!< ひこうタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_3,               //!< ひこうタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_4,         //!< どくタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_4,               //!< どくタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_5,         //!< じめんタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_5,               //!< じめんタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_6,         //!< いわタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_6,               //!< いわタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_7,         //!< むしタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_7,               //!< むしタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_8,         //!< ゴーストタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_8,               //!< ゴーストタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_9,         //!< はがねタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_9,               //!< はがねタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_10,         //!< ほのおタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_10,              //!< ほのおタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_11,        //!< みずタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_11,              //!< みずタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_12,        //!< くさタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_12,              //!< くさタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_13,        //!< でんきタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_13,              //!< でんきタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_14,        //!< エスパータイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_14,              //!< エスパータイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_15,        //!< こおりタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_15,              //!< こおりタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_16,        //!< ドラゴンタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_16,              //!< ドラゴンタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_17,        //!< あくタイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_17,              //!< あくタイプすきを　さがせ！
    ATTRACTION_TYPE_SEARCH_ALOLA_18,        //!< フェアリータイプすきを　さがせ！（アローラずかん）
    ATTRACTION_TYPE_SEARCH_18,              //!< フェアリータイプすきを　さがせ！
    ATTRACTION_DRESSUP_DIFFERENCE_1,        //!< ファンを探せ！
    ATTRACTION_DRESSUP_DIFFERENCE_2,        //!< だいかそうたいかい！
    ATTRACTION_QUESTIONNAIRE_1,             //!< ベテラントレーナーに　あいさつ！
    ATTRACTION_QUESTIONNAIRE_2,             //!< しんじんトレーナーに　ひとこと！
    ATTRACTION_QUESTIONNAIRE_3,             //!< ラブラブな　ひとを　あつめろ！
    ATTRACTION_QUESTIONNAIRE_4,             //!< クールなひとを　あつめろ！
    ATTRACTION_QUESTIONNAIRE_5,             //!< おとこともだち　ちょうさ！
    ATTRACTION_QUESTIONNAIRE_6,             //!< おんなともだち　ちょうさ！
    ATTRACTION_QUESTIONNAIRE_7,             //!< あにき　あねきを　みつけろ！
    ATTRACTION_QUESTIONNAIRE_8,             //!< まんなかっこを　みつけろ！
    ATTRACTION_QUESTIONNAIRE_9,             //!< すえっこを　みつけろ！
    ATTRACTION_QUESTIONNAIRE_10,            //!< ひとりっこを　みつけろ！
    ATTRACTION_QUESTIONNAIRE_11,            //!< なかみが　だいじなひとを　さがせ！
    ATTRACTION_QUESTIONNAIRE_12,            //!< がいけんが　だいじなひとを　さがせ！
    ATTRACTION_QUESTIONNAIRE_13,            //!< ひみつはまもるもの
    ATTRACTION_QUESTIONNAIRE_14,            //!< ひみつはもらすもの
    ATTRACTION_QUESTIONNAIRE_15,            //!< 休日はまったり
    ATTRACTION_QUESTIONNAIRE_16,            //!< 休日はアクティブ
    ATTRACTION_DRESSUP_DISGUISE_1,          //!< へんそうを　みやぶれ！
    ATTRACTION_DRESSUP_DISGUISE_2,          //!< すがおが　いちばん！
    ATTRACTION_DRESSUP_RESEARCH_1,          //!< レッドリサーチ
    ATTRACTION_DRESSUP_RESEARCH_2,          //!< グリーンリサーチ
    ATTRACTION_DRESSUP_RESEARCH_3,          //!< オレンジリサーチ
    ATTRACTION_DRESSUP_RESEARCH_4,          //!< パープルリサーチ
    ATTRACTION_DRESSUP_RESEARCH_5,          //!< イエローリサーチ
    ATTRACTION_DRESSUP_RESEARCH_6,          //!< ブル－リサーチ
    ATTRACTION_DRESSUP_RESEARCH_7,          //!< ネイビーリサーチ
    ATTRACTION_DRESSUP_RESEARCH_8,          //!< ピンクリサーチ

    ATTRACTION_NUM,
    ATTRACTION_NULL = ATTRACTION_NUM,//!< 無効値
  };



#if USE_PAWN == 0
} // namespace JoinFestaScript
#endif

#endif // __JOIN_FESTA_PAWN_DEFINE_H__
