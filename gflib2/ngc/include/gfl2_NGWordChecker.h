//================================================================================
/**
 * @file   gfl2_NGWordChecker.h
 * @brief  NGワードチェック用クラス
 * @author hisanao suzuki
 * @date   2012.2.15
 */
//================================================================================
#ifndef __GFL2_NG_WORD_CHECKER_H__
#define __GFL2_NG_WORD_CHECKER_H__

#pragma once

#ifdef GF_PLATFORM_CTR
#include <nn.h>
#endif

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <ro/include/gfl2_RoManager.h>

#include "gfl2_NGWordCheckListener.h"

namespace gfl2 {

/**
* @namespace gfl2::ngc
* @brief NGワードチェック用ネームスペース
*/
namespace ngc {

  class NGWordCheckThread;

  //-------------------------------------------------------------------------------
  /**
  * @brief NGワードチェック用クラス
  */
  //-------------------------------------------------------------------------------
  class NGWordChecker : public gfl2::base::Singleton<NGWordChecker>
  {
    GFL_FORBID_COPY_AND_ASSIGN( NGWordChecker );  // コピーを禁止

  public:
#ifdef GF_PLATFORM_CTR
    typedef nn::ngc::CTR::ProfanityFilterPatternList ProfanityFilterPatternList;

    /**
     * @brief 全言語チェック指定用定数
     */
    static const ProfanityFilterPatternList ALL_REGION = (ProfanityFilterPatternList)(
                                                          (1<<nn::ngc::CTR::PATTERNLIST_JAPAN_JAPANESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_ENGLISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_FRENCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_SPANISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_ENGLISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_FRENCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_GERMAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_ITALIAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_SPANISH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_DUTCH)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_KOREA_KOREAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_CHINA_SIMP_CHINESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_PORTUGUESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_EUROPE_RUSSIAN)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_AMERICA_PORTUGUESE)|
                                                          (1<<nn::ngc::CTR::PATTERNLIST_TAIWAN_TRAD_CHINESE));
#endif

  public:
    static const u32  ARROW_NUMBER_COUNT        = 5;  //!< 画面中に含んでも良い数字文字数
    static const u32  ARROW_FREEWORD_CHAR_COUNT = 16; //!< フリーワード文字数

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  コンストラクタ
    */
    //-------------------------------------------------------------------------------
    NGWordChecker( void );

    //-------------------------------------------------------------------------------
    /**
    * @brief  デストラクタ
    */
    //-------------------------------------------------------------------------------
    virtual ~NGWordChecker();

  public:
    //-------------------------------------------------------------------------------
    /**
    * @brief  初期化処理
    *
    * @param[in]  heap ワーク領域として使用されるヒープエリア。およそ64KBが消費される。
    *
    * @retval true  初期化成功
    * @retval false 失敗
    */
    //-------------------------------------------------------------------------------
    bool Initialize( gfl2::heap::HeapBase *heap );

    //-------------------------------------------------------------------------------
    /**
    * @brief  終了処理
    *
    * @retval true  正常終了
    * @retval false 異常終了
    */
    //-------------------------------------------------------------------------------
    bool Finalize( void );

    //---------------------------------------------------------------------
    /**
    * @brief  任天堂のレギュレーションと照らし合わせ、フリーワードが問題ないかチェックする(同期関数）
    *
    * @par
    *         - CheckProfanityWords()でNGとならない
    *         - MaskProfanityWordsInText()でNGとならない
    *         - 16文字以内
    *         - 数字が5文字以内
    *
    * @param[in]  nPatternCode   チェックを行うリージョンをビットで指定する
    * @param[in]  pText          検査対象となるNULL終端文字列
    *
    * @return 検査結果
    */
    // ---------------------------------------------------------------------
#ifdef GF_PLATFORM_CTR
    Result CheckFreewordWithRegulation( ProfanityFilterPatternList nPatternCode, wchar_t *pText );
#else
    Result CheckFreewordWithRegulation( wchar_t *pText );
#endif

    //-------------------------------------------------------------------------------
    /**
    * @brief  任天堂のレギュレーションと照らし合わせ、フリーワードが問題ないかチェックする(非同期関数)
    *
    * @par
    *         - CheckProfanityWords()でNGとならない
    *         - MaskProfanityWordsInText()でNGとならない
    *         - 16文字以内
    *         - 数字が5文字以内
    *
    * @attention 終了イベント通知後、必ずDeleteAsync()をコールしてください。
    *
    * @param[in]  heap            スレッドに使用するヒープ
    * @param[in]  nPatternCode    チェックを行うリージョンをビットで指定する
    * @param[in]  pText           検査対象となるNULL終端文字列
    * @param[in]  listener        検査完了を通知するためのリスナ
    * @param[in]  thread_priority スレッドの実行優先度(上0〜31下)
    *
    * @retval true   検査が開始できた
    * @retval false  検査に失敗した
    */
    //-------------------------------------------------------------------------------
    //bool CheckFreewordWithRegulationASync( gfl2::heap::HeapBase* heap,
    //                                       ProfanityFilterPatternList nPatternCode, wchar_t *pText,
    //                                       NGWordCheckListener* listener,
    //                                       gfl2::base::Thread::Priority thread_priority );
#ifdef GF_PLATFORM_CTR
    bool CheckFreewordWithRegulationASync( gfl2::heap::HeapBase* heap,
                                            ProfanityFilterPatternList nPatternCode, wchar_t *pText,
                                            NGWordCheckListener* listener,
                                            u8 thread_priority );
#else
    bool CheckFreewordWithRegulationASync(
      gfl2::heap::HeapBase* heap, wchar_t *pText,
      NGWordCheckListener* listener,
      u8 thread_priority );
#endif
    //-------------------------------------------------------------------------------
    /**
    * @brief  MaskProfanityWordsInText()実行時、問題のある語句を発見した時の挙動を設定
    *
    * @param  bOverWrite[in] 問題語句の文字が発見された際の動作モード。
    *         @arg true   問題語句の文字すべてをアスタリスク記号で上書き。
    *         @arg false  問題語句を文字数によらず1文字のアスタリスク記号に置換。
    */
    //-------------------------------------------------------------------------------
    void SetMaskMode( bool bOverWrite );

    //-------------------------------------------------------------------------------
    /**
    * @brief  文字列に含まれる、数を表す文字の個数を取得する
    *
    * @param[in]  pString 調べる対象のNULL終端文字列
    *
    * @return 数を表す文字の個数
    */
    //-------------------------------------------------------------------------------
    static int CountNumbers( const wchar_t * pString );

  private:
    //-------------------------------------------------------------------------------
    /**
    * @brief  文章をチェックし、問題のある語句を発見した時の挙動を設定
    *
    * @param[out]  pProfanityWordCount 文章中に不正文字列が何回現れたかを取得するためのポインタ
    * @param[in]   nPatternCode        チェックを行うリージョンをビットで指定する
    * @param[out]  pText               検査対象となるNULL終端文字列。NG箇所が*に置き換えられる
    *
    * @return 実行結果
    */
    //-------------------------------------------------------------------------------
#ifdef GF_PLATFORM_CTR
    nn::Result MaskProfanityWordsInText( int* pProfanityWordCount, ProfanityFilterPatternList nPatternCode, wchar_t *pText );
#endif

    //-------------------------------------------------------------------------------
    /**
    * @brief  単語をチェックし、問題のある語句を発見した時の挙動を設定
    *
    * @param[out] checkResult    NGワードが存在するならtrueを格納
    * @param[in]  nPatternCode   チェックを行うリージョンをビットで指定する
    * @param[in]  ppWords        検査対象となるNULL終端文字列の配列（文字コード：UTF16リトルエンディアン）
    * @param[in]  nWordCount     検査対象文字列の配列数(最大16個まで）
    *
    * @return 実行結果
    */
    //-------------------------------------------------------------------------------
#ifdef GF_PLATFORM_CTR
    nn::Result CheckProfanityWords( bool* checkResult, ProfanityFilterPatternList nPatternCode, const wchar_t ** ppWords, size_t nWordCount );
#endif
  public:
    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期通信が実行中かどうかを調べる
     * @retval true   実行中
     * @retval false  実行していない
     */
    //-------------------------------------------------------------------------------
    bool CheckAsync( void ) const;

    //-------------------------------------------------------------------------------
    /**
     * @brief 実行中の非同期通信をキャンセルする
     *
     * @note CheckAsync()で非同期処理の終了を確認した後、DeleteAsync()を呼んでください。
     */
    //-------------------------------------------------------------------------------
    void CancelAsync( void );

    //-------------------------------------------------------------------------------
    /**
     * @brief スレッドの終了待ちを行います。
     */
    //-------------------------------------------------------------------------------
    void WaitAsyncFinished( void );

    //-------------------------------------------------------------------------------
    /**
     * @brief 非同期処理スレッドを破棄する
     */
    //-------------------------------------------------------------------------------
    void DeleteAsync( void );

  private:
#ifdef GF_PLATFORM_CTR
    static const u32  WORKMEMORY_SIZE   = nn::ngc::CTR::ProfanityFilter::WORKMEMORY_SIZE; //!< 消費されるワークメモリサイズ
#else
    static const u32  WORKMEMORY_SIZE   = 64 * 1024; //!< 消費されるワークメモリサイズ
#endif
    static const u32  NGWORD_STACK_SIZE = 8192;                                           //!< スレッドで使用されるスタックサイズ

  private:
#ifdef GF_PLATFORM_CTR
    nn::ngc::CTR::ProfanityFilter m_filter;       //!< フィルタークラス
#endif
    bool                          m_isInit;       //!< 初期化フラグ
    void*                         m_workMemory;   //!< ワークメモリ
    bool                          m_isOverWrite;  //!< NGワード変換モード
    wchar_t                       m_tempArea[32]; //!< ワーク領域
    NGWordCheckThread*            m_thread;       //!< 非同期処理通知用スレッド
  };

} // ngc
} // gfl2



/** ---------------------------------------------------------------------
* @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl2::ngc::NGWordChecker );

#endif  // __GFL_NG_WORD_CHECKER_H__
