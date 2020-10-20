//=============================================================================
/**
 * @file   gfl_NGWordCheckThread.h
 * @author suzuki_hisanao
 */
//=============================================================================

#ifndef __GFL_NG_WORD_CHECK_THREAD_H__
#define __GFL_NG_WORD_CHECK_THREAD_H__

#pragma once

#include <nn.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>


namespace gfl {
namespace ngc {

class NGWordCheckListener;
class NGWordChecker;


/**
 * @brief ＮＧワードチェック用内部スレッドクラス。<p>
 *        チェック処理を非同期実行する為に準備されたクラスです。
 *
 */
class NGWordCheckThread : public gfl::base::Thread
{
  GFL_FORBID_COPY_AND_ASSIGN(NGWordCheckThread); //コピーコンストラクタ＋代入禁止

private:
  nn::ngc::CTR::ProfanityFilterPatternList  m_nPatternCode; //!< 他のリージョンとのデータ交換を行うか
  NGWordCheckListener*                      m_listener;     //!< イベントリスナ
  wchar_t*                                  m_pText;        //!< 検査対象文字へのポインタ
  NGWordChecker*                            m_checker;      //!< NGワードチェッカー

public:
  //--------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param[in] heap           スレッドのスタック領域に使用するヒープ
   * @param[in] stack_size     スレッドのスタック領域のサイズ
   * @param[in] listener       チェック完了通知用リスナ
   */
  //--------------------------------------------------------------------------------
  NGWordCheckThread( gfl::heap::HeapBase* heap, u32 stack_size, NGWordCheckListener *listener );

  //--------------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //--------------------------------------------------------------------------------
  virtual ~NGWordCheckThread();

  //--------------------------------------------------------------------------------
  /**
   * @brief  文章をチェックし、問題のある語句を発見した時の挙動を設定
   *
   * @param[in]      nPatternCode    チェックを行うリージョンをビットで指定する
   * @param[in,out]  pText           検査対象となるNULL終端文字列。NG箇所が*に置き換えられる
   * @param[in]      checker         ＮＧワードチェッカー
   */
  //--------------------------------------------------------------------------------
  void Setup( nn::ngc::CTR::ProfanityFilterPatternList nPatternCode, wchar_t *pText, NGWordChecker* checker );

public:
  virtual void Initialize( void );
  virtual void Finalize( void );
  virtual void OnKill( void );
  virtual void Main( void );
};

} // ngc
} // gfl


#endif

