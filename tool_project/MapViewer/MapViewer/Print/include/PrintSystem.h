//=======================================================================================
/**
 * @file    PrintSystem.h
 * @brief   文字列描画システム
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.08
 *
 * @li sangoのprintsys.hを移植
 */
//=======================================================================================
#if !defined( PRINTSYSTEM_H_INCLUDED )
#define PRINTSYSTEM_H_INCLUDED
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include "../../System/include/PokemonLanguage.h"


GFL_NAMESPACE_BEGIN( print )

/**
 *  タググループID
 */
enum TagGroup {
  TAGGROUP_NULL           = 0x00,
  TAGGROUP_WORD           = 0x01,
  TAGGROUP_NUMBER         = 0x02,  ///< 数値挿入

  TAGGROUP_GRAMMAR_FORCE  = 0x10,  ///< 文法特性強制タグ
  TAGGROUP_STRING_SELECT  = 0x11,  ///< 文字列選択タグ
  TAGGROUP_WORDFORM_START = 0x11,  ///< 語形選択タググループの最初
  TAGGROUP_WORDFORM_MAX   = 0x19,  ///< 語形選択タググループの最後（含まず）
  TAGGROUP_KOREAN         = 0x19,  ///< 韓国語助詞

  TAGGROUP_GENERAL_CTRL   = 0xbd,  ///< 汎用コントロール処理
  TAGGROUP_STREAM_CTRL    = 0xbe,  ///< 流れるメッセージ用コントロール処理
  TAGGROUP_SYSTEM         = 0xff,  ///< エディタシステムタグ用コントロール処理
};

/**
 *  TAGGROUP_WORD 内 Index
 */
enum WordTagIdx {
  TAGIDX_WORD_TRAINER_NAME          = 0,
  TAGIDX_WORD_POKE_NAME             = 1,
  TAGIDX_WORD_POKE_NICKNAME         = 2,
  TAGIDX_WORD_POKE_TYPE             = 3,
  TAGIDX_WORD_TOKUSEI_NAME          = 6,
  TAGIDX_WORD_WAZA_NAME             = 7,
  TAGIDX_WORD_ITEM_NAME             = 9,
  TAGIDX_WORD_POKE_NICKNAME_TRUTH   = 12,
  TAGIDX_WORD_TRAINER_TYPE          = 14,
  TAGIDX_WORD_POKETYPE_NAME         = 15,
  TAGIDX_WORD_APOWER_NAME           = 16,
  TAGIDX_WORD_TRAINER_NAME_FIELD    = 20,
  TAGIDX_WORD_APOWER_USER_NAME      = 141,
  TAGIDX_WORD_TRAINER_TYPE_AND_NAME = 158,
};

/**
 *  文法特性強制タグ内 Index
 */
enum GrammarForceTagIndex {
  TAGIDX_GRAMMAR_FORCE_SINGULAR   = 0,  ///< 強制単数
  TAGIDX_GRAMMAR_FORCE_PLURAL     = 1,  ///< 強制複数
  TAGIDX_GRAMMAR_FORCE_MASCULINE  = 2,  ///< 強制男性
  //TAGIDX_GRAMMAR_FORCE_FEMININE   = 3,  ///< 強制女性（今のところ不要なので定義しない）
  //TAGIDX_GRAMMAR_FORCE_NEUTER     = 4,  ///< 強制中性（今のところ不要なので定義しない）
};

/**
 *  文字列選択タグ内 Index
 */
enum StringSelectTagIndex {
  TAGIDX_STRING_SELECT_BY_GENDER                 = 0,  ///< 性による選択
  TAGIDX_STRING_SELECT_BY_QUANTITY               = 1,  ///< 数による選択
  TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY        = 2,  ///< 数と性による選択
  TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY_GERMAN = 3,  ///< 数と性による選択（ドイツ語）
};

/**
 *  TAGGROUP_KOREAN 内 Index
 */
enum KoreanTagIdx {
  TAGIDX_KOREAN_PARTICLE = 0, // 助詞
};

/**
 *  TAGGROUP_GENERAL_CTRL 内 Index
 */
enum GenTagIdx {
  TAGIDX_GENERAL_CHANGE_COLOR = 0,  ///< 色を変更
  TAGIDX_GENERAL_BACK_COLOR,        ///< 色を戻す
  TAGIDX_GENERAL_X_CENTERING,       ///< 文字描画Ｘ座標 センタリング
  TAGIDX_GENERAL_X_FIT_RIGHT,       ///< 文字描画Ｘ座標 右寄せ
  TAGIDX_GENERAL_X_ADD,             ///< 文字描画Ｘ座標 加算
  TAGIDX_GENERAL_X_MOVE,            ///< 文字描画Ｘ座標 指示
  TAGIDX_GENERAL_FIX_WIDTH_START,   ///< 文字描画Ｘサイズ固定化処理の開始
  TAGIDX_GENERAL_FIX_WIDTH_DIRECT,  ///< 文字描画Ｘサイズ固定化処理の開始（計算済み）
  TAGIDX_GENERAL_FIX_WIDTH_END,     ///< 文字描画Ｘサイズ固定化処理の終了

  TAGIDX_GENERAL_NOTUSED_MESSAGE = 255, ///< デバッグ用。未使用メッセージであることを示す。
};

/**
 *  TAGGROUP_STREAM_CTRL 内 Index
 */
enum StreamTagIdx {
  TAGIDX_STREAM_LINE_FEED = 0,  ///< ページ切り替え(行送り
  TAGIDX_STREAM_PAGE_CLEAR,     ///< ページ切り替え(クリア
  TAGIDX_STREAM_WAIT_ONE,       ///< ウェイト変更(単発
  TAGIDX_STREAM_WAIT_CONT,      ///< ウェイト変更(継続
  TAGIDX_STREAM_WAIT_RESET,     ///< ウェイトリセット
  TAGIDX_STREAM_CALLBACK_ONE,   ///< コールバック引数変更(単発
  TAGIDX_STREAM_CALLBACK_CONT,  ///< コールバック引数変更(継続
  TAGIDX_STREAM_CALLBACK_RESET, ///< コールバック引数リセット
  TAGIDX_STREAM_CLEAR_WIN,      ///< 描画領域クリア
  TAGIDX_STREAM_CTRL_SPEED,     ///< 速度コントロール
};

/**
 *  TAGGROUP_SYSTEM 内 Index
 */
enum SystemTagIdx {
  TAGIDX_SYSTEML_CHANGE_COLOR = 0,  //MessageStudioによる色変更
};

//EFIGS時に横方向につぶす倍率
//デフォルトが0.64倍。最低値は0.5倍。
extern const f32 PRINT_EFIGS_SCALE;

// テキストボックスペインに指定されている文字のスケール値
// DrawUtilで文字を描画する際に使用してください
extern const f32 LAYOUT_STRING_SCALE_X;    ///< 文字のXスケール値
extern const f32 LAYOUT_STRING_SCALE_Y;    ///< 文字のYスケール値
extern const f32 LAYOUT_NUMBER_SCALE_X;    ///< 数字のXスケール値（数字のみの場合はこちら）
extern const f32 LAYOUT_NUMBER_SCALE_Y;    ///< 数字のYスケール値（数字のみの場合はこちら）


//------------------------------------------------------------------
/**
 * 数値を文字列化する時の整形タイプ
 */
//------------------------------------------------------------------
enum NumberDispType {
  NUM_DISP_LEFT,    ///< 左詰め
  NUM_DISP_SPACE,   ///< 右詰め（スペース埋め）
  NUM_DISP_ZERO,    ///< 右詰め（ゼロ埋め）
};


//------------------------------------------------------------------
/**
 * 数値を文字列化する時の文字コード指定
 */
//------------------------------------------------------------------
enum NumberCodeType {
  NUM_CODE_ZENKAKU,   ///< 全角
  NUM_CODE_HANKAKU,   ///< 半角
  // ローカライズ時
  NUM_CODE_DEFAULT = NUM_CODE_HANKAKU,
};

//レイアウトに右寄せ・センタリングを使う時の特殊ワーク
class FontTagProc;
struct LYT_TEXT_EX_WORK
{
  FontTagProc *tagProc;
};


//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタWordSetの単語埋め込み対象タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool    単語埋め込み対象タグであればtrue
 */
//=============================================================================================
bool IsWordSetTagGroup( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタWordSetの数値挿入タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool    数値挿入タグであればtrue
 */
//=============================================================================================
bool IsWordSetNumberTagGroup( const gfl2::str::STRCODE* sp );


//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタが韓国語の助詞タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool    助詞タグであればtrue
 */
//=============================================================================================
bool IsKoreanParticleTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタが文法特性強制タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool  文法特性強制タグであればtrue
 */
//=============================================================================================
bool IsGrammarForceTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタが文字列選択タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool  文字列選択タグであればtrue
 */
//=============================================================================================
bool IsStringSelectTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タグ開始コードをチェックし、文字列ポインタが語形変化タグかどうか判定
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指していること）
 *
 * @retval  bool  語形変化タグであればtrue
 */
//=============================================================================================
bool IsWordFormTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * 語形変化タグを指している文字列から、単語番号を取得
 *
 * @param   sp    文字列ポインタ（語形変化タグを指している必要がある）
 *
 * @retval  u8    語形番号
 */
//=============================================================================================
u8 GetWordFormTagWordIndex( const gfl2::str::STRCODE* sp );


//=============================================================================================
/**
 * タグコードからタググループを取得
 *
 * @param   tagCode   タグコード
 *
 * @retval  TagGroup    タググループ
 */
//=============================================================================================
TagGroup TagCodeToGroup( const gfl2::str::STRCODE tagCode );

//=============================================================================================
/**
 * タグコードからタグIndexを取得
 *
 * @param   tagCode   タグコード
 *
 * @retval  u8        タグIndex（グループ内の番号）
 */
//=============================================================================================
u8 TagCodeToIndex( const gfl2::str::STRCODE tagCode );


//=============================================================================================
/**
 * タグ開始コードを指している文字列から、そのタグのグループを取得
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指している必要がある）
 *
 * @retval  TagGroup    タググループ
 */
//=============================================================================================
extern TagGroup GetTagGroup( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タグ開始コードを指している文字列から、そのタグのグループ内インデックスを返す
 *
 * @param   sp    文字列ポインタ（タグ開始コードを指している必要がある）
 *
 * @retval  u8    グループ内インデックス
 */
//=============================================================================================
extern u8 GetTagIndex( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * タググループID、グループ内Index を合成してタグコード化する
 *
 * @param   groupID
 * @param   index
 *
 * @retval  gfl2::str::STRCODE
 */
//=============================================================================================
extern gfl2::str::STRCODE CreateTagCode( TagGroup groupID, u8 index );

//=============================================================================================
/**
 * タグ開始コードを指している文字列から、そのタグのタイプを取得
 *
 * @param[in]   sp            文字列ポインタ（タグ開始コードを指している必要がある）
 * @param[in]   paramIndex    パラメータインデックス
 *
 * @retval  u16   タグタイプ
 */
//=============================================================================================
u16 GetTagParam( const gfl2::str::STRCODE* sp, u32 paramIndex );

//=============================================================================================
/**
 * 数値の文字列化（符号付き32bit）
 *
 * @param[out]   dst
 * @param[in]    number   数値
 * @param[in]    keta     最大ケタ数
 * @param[in]    dispType 整形タイプ
 * @param[in]    codeType 文字コードタイプ
 */
//=============================================================================================
void StrNumber( gfl2::str::StrBuf* dst, s32 number, u32 keta, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * 数値の文字列化（符号付き double ）
 *
 * @param[out]   dst
 * @param[in]    number    数値
 * @param[in]    keta_int  整数部最大ケタ数
 * @param[in]    keta_dec  小数部ケタ数
 * @param[in]    dispType  整形タイプ
 * @param[in]    codeType  文字コードタイプ
 */
//----------------------------------------------------------------------------------
void StrRealNumber( gfl2::str::StrBuf* dst, double number, u32 keta_int, u32 keta_dec, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定する（タグ考慮）
 * ※右寄せ・センタリングを使う場合は下にあるワーク生成＆ワーク渡し版を使う事
 *
 * @param   target
 * @param   str
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数でMsgDataから自動取得
 */
//----------------------------------------------------------------------------------
void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , System::MSGLANGID langId = System::MSGLANGID_MAX );
void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const wchar_t* str , System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定するためのワーク作成
 * 特殊タグ用(右寄せタグ、センタリングタグに必要)
 *
 * @param   gfl::heap::HeapBase *heap ヒープ
 *
 * @retval  LYT_TEXT_EX_WORK*   ワーク
 */
//----------------------------------------------------------------------------------
LYT_TEXT_EX_WORK* CreateLytTextExWork( gfl2::heap::HeapBase *heap );

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定するためのワーク削除
 * 特殊タグ用(右寄せタグ、センタリングタグに必要)
 *
 * @param   LYT_TEXT_EX_WORK*   ワーク
 */
//----------------------------------------------------------------------------------
void DeleteLytTextExWork( LYT_TEXT_EX_WORK* work );

//----------------------------------------------------------------------------------
/**
 * レイアウトのテキストボックスに、文字列を設定する（特殊タグ考慮）右寄せタグ、センタリングタグに必要
 *
 * @param   target
 * @param   str
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数でMsgDataから自動取得
 */
//----------------------------------------------------------------------------------
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId = System::MSGLANGID_MAX );
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const wchar_t* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * 文字列の長さを取得する。(改行解釈済み)
 *
 * @param   str       文字列
 * @param   pane      テキストボックスペイン
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数で自動取得
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, gfl2::lyt::LytTextBox * pane, System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * 文字列の長さを取得する。(改行解釈済み)
 *  printの内部用の関数です。基本↑の関数を使ってください！！！
 *
 * @param   str       文字列
 * @param   font      フォントファイル
 * @param   fontWidth フォント幅(スケール済みのサイズを入れてください)
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, const nw::font::Font *font , f32 fontWidth , int *retLine = NULL );
int GetStringWidth( const wchar_t* str, const nw::font::Font *font , f32 fontWidth , int *retLine = NULL );

//----------------------------------------------------------------------------------
/**
 * 文字列のスケール値を取得する(DrawUtil等独自の文字描画をする時用!)
 *
 * @param   langId 言語ID(EFIGSの縮小対応)sv_config内の言語IDを入れる
 *                 デフォルト引数で自動取得
 */
//----------------------------------------------------------------------------------
f32 GetStringScale( System::MSGLANGID langId = System::MSGLANGID_MAX );


//Msgファイル関係
//----------------------------------------------------------------------------------
/**
 * メッセージの言語IDを設定する。
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
void SetMessageLangId( System::MSGLANGID langId );

//----------------------------------------------------------------------------------
/**
 * メッセージの言語IDを取得する。
 *
 * @return  言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
System::MSGLANGID GetMessageLangId(void);

//----------------------------------------------------------------------------------
/**
 * 通常のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId( System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * デバッグ用のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
#if PM_DEBUG
u32 GetMessageArcId_Debug( System::MSGLANGID langId = System::MSGLANGID_MAX );
#endif
//----------------------------------------------------------------------------------
/**
 * スクリプト用のメッセージのARCIDを取得する
 *
 * @param   langId       言語ID(ROMの本体の言語IDとは違います！)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId_Script( System::MSGLANGID langId = System::MSGLANGID_MAX );

//内部用
//----------------------------------------------------------------------------------
/**
 * 文字列を縮めるかを判定する
 *
 * @param   target      テキストボックスペイン
 * @param   langId      言語ID(sv_config)
 */
//----------------------------------------------------------------------------------
bool CheckEnableShortStr( gfl2::lyt::LytTextBox * target, System::MSGLANGID langIdTemp );
bool CheckEnableShortStrLang( System::MSGLANGID langId );


GFL_NAMESPACE_END( print )

#endif  // PRINTSYSTEM_H_INCLUDED
