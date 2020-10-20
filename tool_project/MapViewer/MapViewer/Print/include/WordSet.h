//============================================================================================
/**
 * @file    WordSet.h
 * @brief   文字列中の単語埋め込み処理クラス
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.03
 *
 * @li sangoのwordset.hを移植
 */
//============================================================================================
#if !defined( WORDSET_H_INCLUDED )
#define WORDSET_H_INCLUDED
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <pml/include/pmlib.h>

#include <System/include/PokemonVersion.h>
#include <PrintSystem.h>
#include <WordSetLoader.h>
#include <GrammaticalAttributeProvider.h>
///#include <Savedata/include/MyStatus.h>


// Register 関数は、以下のうち番号が若いものを優先して使用する。
// 1か2を使用しておけば、EFIGS文法対応OK。ただし、アイテムなど個数があるものは、単数か複数かを渡す必要がある。
//
// 1) 専用の Register* 関数（RegisterPokeNickName など）
//
// 2) void  RegisterWord( u32 bufID, gfl::str::MsgData* man, u32 strID, Form form = FORM_DEFAULT );
//      専用の Register* 関数を作るまでもない場合などはこれ。
//
// 3) void  RegisterWord( u32 bufID, const gfl::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form = FORM_DEFAULT );
//      MsgData から取得した文字列を保存して使いまわすときなど。
//      EFIGS文法対応するには、 print::GrammaticalAttribute もセットで保存しておく必要がある（文字列の種類によっては、対応しないという判断も可能）。
//
// 4) void  RegisterWord( u32 bufID, const gfl::str::StrBuf& word, Grammer grm = GRAMMER_DEFAULT, Form form = FORM_DEFAULT );
//      任意文字列（フリーワードを含む）。個別にローカライズ対応を考慮する必要がある。

GFL_NAMESPACE_BEGIN(print)

class WordSet {
  GFL_FORBID_COPY_AND_ASSIGN( WordSet );

public:
  enum {
    DEFAULT_WORD_MAX  = 8,    ///< 登録できる単語数のデフォルト値
    DEFAULT_BUF_LEN   = 64,   ///< 単語１つあたり確保されるバッファ長（文字数）のデフォルト値
  };

  /*----------------------------------*/
  /** 文法性                          */
  /*----------------------------------*/
  typedef enum {
    GRAMMER_MALE,     ///< 男性
    GRAMMER_FEMALE,   ///< 女性
    GRAMMER_NEUTRAL,  ///< 中性
    GRAMMER_DEFAULT = GRAMMER_MALE,
  }Grammer;

  /*----------------------------------*/
  /** 単複                            */
  /*----------------------------------*/
  typedef enum {
    FORM_SINGLE,      ///< 単数
    FORM_PLURAL,      ///< 複数
    FORM_UNCOUNTABLE, ///< 非可算、使用禁止（WordSet内部でのみ使用します）
    FORM_DEFAULT = FORM_SINGLE,
  }Form;

  //=============================================================================================
  /**
   * コンストラクタ：登録できる単語数＆単語の最大文字長をデフォルトで作成
   *
   * @param   pHeap   使用ヒープハンドル
   */
  //=============================================================================================
  WordSet( gfl2::heap::HeapBase* pHeap );

  //=============================================================================================
  /**
   * コンストラクタ：登録できる単語数＆単語の最大文字長を指定して作成
   *
   * @param   wordMax
   * @param   bufLen
   * @param   pHeap       使用ヒープハンドル
   */
  //=============================================================================================
  WordSet( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap );

  /** デストラクタ */
  ~WordSet();

  //=============================================================================================
  /**
   * 内部バッファを全て空文字列でクリアする
   */
  //=============================================================================================
  void ClearAllBuffer( void );


  //=============================================================================================
  /**
   * 登録された単語を使って文字列生成する
   *
   * @param[out]   dstbuf   生成後の文字列を格納するバッファ
   * @param[in]    srcbuf   生成元となる（タグコードが埋め込まれた）文字列
   */
  //=============================================================================================
  void Expand( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf );

  void SetWordSetLoader( WordSetLoader * pLoader );

#if 0
  //=============================================================================================
  /**
   * @param[in] p_readman
   * @param[in] pHeap
   * @param[in] read_id_bits
   */
  //=============================================================================================
  void PreLoad( gfl::heap::HeapBase* pHeap, gfl::fs::FileReadManager* p_readman, u32 read_id_bits );

  //=============================================================================================
  /**
   * @brief   事前読み込み処理終了待ち
   * @return  bool  trueのとき、セットアップ完了
   */
  //=============================================================================================
  bool WaitLoad( void );
#endif

  //------------------------------------------------------------------
  /**
   * PM_MALE PM_FEMALE PM_NEUTRAL から Grammer に変換
   */
  //------------------------------------------------------------------
  static Grammer GrammerFromSex(u8 sex)
  {
    // Grammer の値が範囲外でも文字列選択で選択される文字列、語形選択で選択される語形がランダムになるだけなのでASSERTのみ。
    GFL_ASSERT( sex <= PM_NEUTRAL );
    return static_cast<Grammer>( sex );
  }

  //------------------------------------------------------------------
  /**
   * pml::Sex から Grammer に変換。ポケモンの性別にはこれを使用する。
   */
  //------------------------------------------------------------------
  static Grammer GrammerFromPmlSex(pml::Sex sex);

  //------------------------------------------------------------------
  /**
   * （アイテム相当のものの）個数から Form に変換。
   */
  //------------------------------------------------------------------
  static Form FormFromCount(u32 count);

  //------------------------------------------------------------------
  /**
   * MsgDataから文字列を取得して登録。これを使うと、MsgData の userParam から文法パラメータをセットします。
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, gfl2::str::MsgData* man, u32 strID, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * 任意文字列をパラメータ付きで登録。MsgDataから取得した場合これを使う。
   *
   * GrammaticalAttribute は、gfl::MsgData::GetString で得られた userParam を
   * print::GrammaticalAttribute::FromMsgDataUserParam に渡して生成する。
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * 任意文字列をパラメータ付きで登録。ローカライズ対応を考慮すること。
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, Grammer grm = GRAMMER_DEFAULT, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンのニックネームを登録 (CoreParam)
   */
  //------------------------------------------------------------------
  void   RegisterPokeNickName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンのニックネームを登録 (文字列、性別)
   */
  //------------------------------------------------------------------
  void   RegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンのモンスター名を登録 (CoreParam)
   */
  //------------------------------------------------------------------
  void   RegisterPokeMonsName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモン種族名を登録 (MonsNo)
   */
  //------------------------------------------------------------------
  void   RegisterPokeMonsNameNo( u32 bufID, MonsNo monsno );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンの親名を登録
   */
  //------------------------------------------------------------------
  void   RegisterPokeOyaName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンの過去親名を登録
   */
  //------------------------------------------------------------------
  void   RegisterPokePastOyaName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * 指定バッファにトレイ名を登録
   */
  //------------------------------------------------------------------
#if 0 // @todo savedata待ち
  void   RegisterBoxTrayName( u32 bufID, const savedata::BOX* p_box, const u32 tray_no );
#endif

  //------------------------------------------------------------------
  /**
   * 指定バッファにトレーナータイプ名を登録
   *
   * @param   bufID   バッファID
   * @param   trID    トレーナータイプ指定
   */
  //------------------------------------------------------------------
  void   RegisterTrTypeName( u32 bufID, u16 trType );

  //------------------------------------------------------------------
  /**
   * 指定バッファにトレーナー名を登録
   *
   * @param   bufID   バッファID
   * @param   trID    トレーナーID
   */
  //------------------------------------------------------------------
  void   RegisterTrainerName( u32 bufID, u32 trID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにトレーナータイプ名とトレーナー名を結合したものを登録（仏語、西語のバトル中トレーナータイプ表示に対応）
   *
   * @param   dstBufID           設定先バッファID。trTypeNameBufID、trainerNameBufIDと同じでもよい。
   * @param   trTypeNameBufID    トレーナータイプ名をセットしたバッファID
   * @param   trainerNameBufID   トレーナー名をセットしたバッファID
   */
  //------------------------------------------------------------------
  void   RegisterTrTypeNameAndTrainerName(u32 dstBufID, u32 trTypeNameBufID, u32 trainerNameBufID);

  //------------------------------------------------------------------
  /**
   * プレイヤー名を登録
   *
   * @param   bufID   バッファID
   * @param   status  プレイヤーステータス
   */
  //------------------------------------------------------------------
  void RegisterPlayerName( u32 bufID, const Savedata::MyStatus* status );

  //------------------------------------------------------------------
  /**
   * プレイヤーニックネーム接頭子を登録
   *
   * @param   bufID   バッファID
   * @param   status  プレイヤーステータス
   */
  //------------------------------------------------------------------
#if 0   // @todo  使用することになったら有効にしてください 2015/03/11
  void RegisterPlayerNickNamePrefix( u32 bufID, const Savedata::MyStatus* status );
#endif

  //------------------------------------------------------------------
  /**
   * ライバル名を登録
   *
   * @param   bufID   バッファID
   * @param   misc  ライバル名が設定されている misc
   */
  //------------------------------------------------------------------
#if 0 // @todo savedata待ち
  void RegisterRivalName( u32 bufID, const Savedata::Misc* misc );
#endif

  //------------------------------------------------------------------
  /**
   * ポケモン（ワザ）タイプ名を登録
   *
   * @param   bufID     バッファID
   * @param   typeID    タイプID POKETYPE_NORMALなど
   */
  //------------------------------------------------------------------
  void RegisterPokeTypeName( u32 bufID, u32 typeID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンのとくせい名を登録
   *
   * @param   bufID     バッファID
   * @param   tokuseiID   とくせいID
   */
  //------------------------------------------------------------------
  void RegisterTokuseiName( u32 bufID, u32 tokuseiID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケモンのせいかく名を登録
   *
   * @param   bufID       バッファID
   * @param   seikakuID   せいかくID
   */
  //------------------------------------------------------------------
  void RegisterSeikakuName( u32 bufID, u32 seikakuID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにワザ名を登録
   *
   * @param   bufID     バッファID
   * @param   wazaID      ワザID
   */
  //------------------------------------------------------------------
  void RegisterWazaName( u32 bufID, u32 wazaID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにアイテム名を登録
   *
   * @param   bufID     バッファID
   * @param   itemID    アイテムID
   * @param   itemCount アイテムの個数（単数形と複数形を切り替えるため）
   */
  //------------------------------------------------------------------
  void RegisterItemName( u32 bufID, u32 itemID, u32 itemCount);

  //------------------------------------------------------------------
  /**
   * 指定バッファに単数形のアイテム名を登録
   *
   * @param   bufID     バッファID
   * @param   itemID    アイテムID
   *
   * @note  ForceSingular ForcePlural タグ指定でアイテム名の単複を切り替える仕組みを導入したため、
   *        プログラム側で RegisterItemNameSingular と RegisterItemNamePlural を呼び分ける必要はなくなった。
   *        「（ショップで購入個数をたずねるときなど）数を指定しないとき」は、とりあえず単数形を入れておくようにする。
   */
  //------------------------------------------------------------------
  //void RegisterItemNameSingular( u32 bufID, u32 itemID );

  //------------------------------------------------------------------
  /**
   * 指定バッファにポケット名を登録
   *
   * @param   bufID     バッファID
   * @param   pocketNo    ポケットNO
   */
  //------------------------------------------------------------------
  void RegisterFieldPocketName( u32 bufID, u16 pocketNo );

  /* ------------------------------------------------------------------------- */
  /**
   * 指定バッファに地名を登録
   *
   * @param bufID バッファID
   * @param placeNameID 地名ID（ゾーンからとれます）
   */
  /* ------------------------------------------------------------------------- */
  void RegisterPlaceName( u32 bufID, u32 placeNameID );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 指定バッファに天気名を登録
   *
   * @param bufID バッファID
   * @param weather_type 天候タイプ
   */
  /* ------------------------------------------------------------------------- */
#if 0 // @todo 天候実装待ち
  void RegisterWeatherName( u32 bufID, xy_system::weather::Type weather_type );
#endif

  //------------------------------------------------------------------
  /**
   * 指定バッファにリボン名を登録
   *
   * @param   bufID     バッファID
   * @param   ribbonID  リボンID
   */
  //------------------------------------------------------------------
  void RegisterRibbonName( u32 bufID, u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * 数値を登録
   *
   * @param   bufID
   * @param   number      数値
   * @param   keta        最大ケタ数
   * @param   dispType    整形タイプ
   * @param   codeType    文字コードタイプ
   */
  //------------------------------------------------------------------
  void RegisterNumber( u32 bufID, s32 number, u32 keta, print::NumberDispType dispType, print::NumberCodeType codeType );


  //-------------------------------------------------------------------------------------------------
  /**
   * 特定タグ（ポケモンニックネーム、トレーナー名）の表示幅に制限をかけるためのフラグを設定します
   *
   * @param   flag    true を指定することで、Expand時に 表示制限タグを埋め込むようになります。
   *
   */
  //-------------------------------------------------------------------------------------------------
  void SetAreaWidthTagEnable( bool flag ){ m_bAreaWidthTagEnable = flag; }


private:

  /*----------------------------------*/
  /** 単語パラメータ                  */
  /*----------------------------------*/
  union WORD_PARAM
  {
    struct {
      print::GrammaticalAttribute attribute;   ///< 文法特性
      struct {
        u8   space_flag                   : 1; ///< スペースフラグ
        u8   is_initialsound_calculated   : 1; ///< 最初の音の判定済みフラグ（判定を遅延させるため）
        u8   form                         : 2; ///< 文法数
        u8                                : 4; ///< 未使用
      };
      u16  width;                              ///< スペース幅（スペースフラグが1の場合）
    };
    u32   raw;
  };

  /*----------------------------------*/
  /** 単語                            */
  /*----------------------------------*/
  struct WORD
  {
    gfl2::str::StrBuf*    str;
    WORD_PARAM            param;
    u16                   areaWidthChars;  ///< 描画領域計算（0=計算せずそのまま / !0 = 描画領域文字数（8bit固定小数）
  };

  /*----------------------------------*/
  /** 定数                            */
  /*----------------------------------*/
  static const f32  DEFAULT_LIMIT_WIDTH_POKEMON;
  static const f32  DEFAULT_LIMIT_WIDTH_PERSON;
  static const u16  REF_WORD_ID_HERO;   // 主人公の文法特性を参照する
  static const u16  REF_WORD_ID_EXPANDING_WORD; // 現在展開中の単語の文法特性を参照する

  /*---------------------------------------------------------------------------*/
  /* メンバー                                                                  */
  /*---------------------------------------------------------------------------*/
  gfl2::str::StrBuf*     m_tmpBuf;
  u32                    m_wordMax;
  WORD*                  m_word;
  gfl2::heap::HeapBase*  m_heapHandle;
  bool                   m_bAreaWidthTagEnable;
  WordSetLoader*         m_wordSetLoader;
  u32                    m_expandingWordId;   // （再帰的な展開により）現在展開中の単語
  gfl2::str::STRCODE     m_lastWordLastChar;  // 最後に展開した単語の最後の文字（韓国の助詞変化用）
  WORD_PARAM             m_forceWordParam;    // m_forceWordParam で文法特性を上書きする
  WORD_PARAM             m_forceWordParamMask;// m_forceWordParamMask が 1 のビットだけ上書きする

  static inline WORD_PARAM defaultWordParam();
  static inline WORD_PARAM createWordParam(print::GrammaticalAttribute attribute, Form form = FORM_SINGLE);
  static inline WORD_PARAM createWordParam(Grammer grm, Form form = FORM_SINGLE);

  /** メンバ構築（コンストラクタから呼び出し） */
  void       setupMembers( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap );

  /** 単語ワーク初期化 */
  void       clearWord( u32 wordIdx );

  /** 単語１件登録 */
  void        registerWord( u32 bufIdx, const gfl2::str::StrBuf& str, WORD_PARAM param, u16 areaWidthChars=0 );
  inline void registerWordFromMsgArc( u32 bufID, u32 arc_id, u32 msg_id, Form form = FORM_DEFAULT);
  u16         loadStringFromMsgArc( gfl2::str::StrBuf& dstbuf, u32 dat_id, u32 msg_id );

  /** タグをのぞいた最後の文字を取得する **/
  gfl2::str::STRCODE getLastChar(const gfl2::str::StrBuf* strbuf);
  /** dstbuf に、srcptr の指すEOM終端の文字列を追加する  **/
  static void addStr(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* srcptr);

  /** Expand のサブ処理 **/
  void expandCore( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf, bool recursed );
  void expandNumber( gfl2::str::StrBuf* dstbuf, uint word_id, gfl2::str::STRCODE separator );
  const gfl2::str::STRCODE* processWordSetTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src, bool recursed);
  const gfl2::str::STRCODE* processKoreanParticleTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processStringSelectTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processWordFormTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processGrammarForceTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  uint getStringSelectIndex(uint tag_index, uint ref_word_id) const;
  uint getWordFormIndex(uint ref_word_id) const;
  WORD_PARAM getRefWordParam(uint ref_word_id) const;
};  // class WordSet


GFL_NAMESPACE_END(print)

#endif  // WORDSET_H_INCLUDED
