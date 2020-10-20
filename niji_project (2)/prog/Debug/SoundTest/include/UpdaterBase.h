//======================================================================
/**
 * @file UpdaterBase.h
 * @date 2015/10/28 20:01:23
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __UPDATER_BASE_H_INCLUDED__
#define __UPDATER_BASE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include "Debug/SoundTest/include/SoundContainer.h"
#include <util/include/gfl2_DrawUtilText.h>
#include <Print/include/WordSet.h>
#include "Debug/SoundTest/include/SoundTestDefine.h"

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

  enum{
    STRLEN_LABEL_BUFFER = 256,

  };
  /**
   * @class UpdaterBase
   * @brief 更新クラス基底
   */
  class UpdaterBase
  {
      GFL_FORBID_COPY_AND_ASSIGN(UpdaterBase);

  public:
    struct SetupParam
    {
      gfl2::heap::HeapBase*        m_DevHeap; //! ヒープ
      gfl2::lyt::LytSys* m_pLytSys;           //! レイアウトシステムへのポインタ
      gfl2::str::StrBuf* m_pStr;              //! 文字列バッファ
      gfl2::str::StrBuf* m_pSearchStr;        //! 検索文字列バッファ
      gfl2::str::MsgData* m_pMsg;             //! メッセージ
      gfl2::str::MsgData* m_pDebugMsg;        //! デバッグメッセージ
      print::WordSet*     m_pWordSet;         //! ワードセット
      SoundContainer::Iterator m_SoundIteratorEnd;    //! イテレーター（終了）
      SoundContainer::Iterator m_SoundIteratorBegin;  //! イテレーター（開始）
      Page m_Page;                            //! ページ
    };

    /**
     * @brief コンストラクタ
     * @param param セットアップパラメータ
     */
    UpdaterBase(SetupParam &param);

    /**
     * @brief デストラクタ
     */
    virtual ~UpdaterBase(){}

    /**
     * @brief 入力更新
     * @param page ページ
     */
    void UpdateInput(Page &page);

    /**
     * @brief ページ取得
     * @return page
     */
    Page GetPage(void){return m_Page;}

    /*
     * @brief 表示情報のセット
     */
    void SetDispInfo(void);

    /**
     * @brief 更新処理
     * @return bool true で入力した
     */
    virtual bool UpdateFunc(void){return false;}

    /**
     * @brief Aボタン押下時の処理
     */
    virtual void FuncButtonA(void){}

    /**
     * @brief Bボタン押下時の処理
     */
    virtual void FuncButtonB(void){}

    /**
     * @brief 右キー押下時の処理
     */
    virtual void FuncKeyRight(void){}

    /**
     * @brief 左キー押下時の処理
     */
    virtual void FuncKeyLeft(void){}

    /**
     * @brief 上キー押下時の処理
     */
    virtual void FuncKeyUp(void){}

    /**
     * @brief 下キー押下時の処理
     */
    virtual void FuncKeyDown(void){}

    /**
     * @brief タッチ時の処理
     * @param x Ｘ座標
     * @param y Ｙ座標
     */
    virtual void FuncTouch(int x, int y){}

    /*
     * @brief ページ変更時処理
     */
    virtual void ChangePageFunc(void){}

    /**
     * @brief 上画面描画
     * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
     */
    virtual void DrawTop(gfl2::util::DrawUtilText* pDrawUtilText){}

    /**
     * @brief 下画面描画
     * @param pDrawUtilText  テキストユーティリティクラスへのポインタ
     */
    virtual void DrawBottom(gfl2::util::DrawUtilText* pDrawUtilText){}

    /**
     * @brief 検索後処理
     * @param page ページ
     */
    virtual void SearchAfterFunc(Page &page);

    /**
     * @brief ページ切り替え可能か
     * @return bool true で可能
     */
    virtual bool CanPageChange(void){return true;}

  private:
      /**
     * @brief 検索後処理
     * @param page ページ
     */
    void SearchAfter(Page &page);

  protected:
    gfl2::heap::HeapBase*        m_DevHeap;  //! ヒープ
    gfl2::lyt::LytSys* m_pLytSys;            //! レイアウトシステムへのポインタ
    gfl2::str::StrBuf* m_pStr;               //! 文字列バッファ
    gfl2::str::StrBuf* m_pSearchStr;         //! 検索文字列バッファ
    gfl2::str::MsgData* m_pMsg;              //! メッセージ
    gfl2::str::MsgData* m_pDebugMsg;         //! デバッグメッセージ
    print::WordSet*     m_pWordSet;          //! ワードセット

    bool m_UseSoundIterator;                  //! イテレーターを使用するか
    SoundContainer::Iterator m_SoundIterator; //! イテレーター
    Page m_Page;                              //! ページ

    int m_Search;                             //! 検索が終了した場合、入力のインデックスが入る変数
  
  private:
    SoundContainer::Iterator m_SoundIteratorEnd;    //! イテレーター（終了）
    SoundContainer::Iterator m_SoundIteratorBegin;  //! イテレーター（開始）
  };


GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __UPDATER_BASE_H_INCLUDED__

#endif //PM_DEBUG