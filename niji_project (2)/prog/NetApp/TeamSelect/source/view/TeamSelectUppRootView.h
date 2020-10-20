//======================================================================
/**
 * @file TeamSelectUppRootView.h
 * @date 2015/06/30 19:29:18
 * @author uchida_yuto
 * @brief チーム選択の上画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_UPP_ROOT_VIEW_H_INCLUDED__
#define __TEAM_SELECT_UPP_ROOT_VIEW_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "AppLib/include/ui/UIView.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(print)

  class WordSet;

GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @class TeamSelectUppRootView
   * @brief チーム選択の上画面ルートView
   */
  class TeamSelectUppRootView : public app::ui::UIView
  {
      GFL_FORBID_COPY_AND_ASSIGN(TeamSelectUppRootView);

  public:
    /**
     * @fn
     * ここに関数の説明を書く
     * @brief 要約説明
     * @param[in] var1 var1の説明
     * @param[out] var2 var2の説明
     * @param[in,out] var3 var3の説明
     * @return 戻り値の説明
     * @sa 参照すべき関数を書けばリンクが貼れる
     * @detail 詳細な説明
     */

    //------------------------------------------------------------------
    /**
      * @brief   コンストラクタ
      * @param[in] pHeap 使用するメモリ
      */
    //------------------------------------------------------------------
    TeamSelectUppRootView( app::util::Heap* pHeap, app::util::AppRenderingManager* pRenderManager );

    virtual ~TeamSelectUppRootView();

    //------------------------------------------------------------------
    /**
      * @brief   初期化
      */
    //------------------------------------------------------------------
    void Initialize( void* pLytBinData, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   終了処理
      */
    //------------------------------------------------------------------
    void Terminate();

    //------------------------------------------------------------------
    /**
      * @brief   破棄可能か
      */
    //------------------------------------------------------------------
    bool IsDelete();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  メッセージ表示
    */
    //--------------------------------------------------------------------------------------------
    void ShowMessageWindow( const u32 msgId );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  メッセージ非表示
    */
    //--------------------------------------------------------------------------------------------
    void HideMesssageWindow();

  private:
    //  作成するレイアウトワークの種類
    enum
    {
      LYT_WORK_NORMAL  = 0,
      LYT_WORK_MAX
    };

    //------------------------------------------------------------------
    /**
      * @brief   2D画面の作成
      * @param   pAppLytBuff レイアウトデータのバイナリポインタ
      */
    //------------------------------------------------------------------
    void _Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet );

    //------------------------------------------------------------------
    /**
      * @brief   変数の初期化はここに
      */
    //------------------------------------------------------------------
    void _Clear()
    {
      m_pHeap             = NULL;
      m_pRenderManager    = NULL;
      m_pWindowPartsPane  = NULL;
      m_pWindowTextBox    = NULL;
    }

    app::util::Heap*                m_pHeap;
    app::util::AppRenderingManager* m_pRenderManager;

    gfl2::lyt::LytParts*            m_pWindowPartsPane;
    gfl2::lyt::LytTextBox*          m_pWindowTextBox;
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_UPP_ROOT_VIEW_H_INCLUDED__