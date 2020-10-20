//======================================================================
/**
 * @file FingerCursor.h
 * @date 2015/10/22 15:31:12
 * @author uchida_yuto
 * @brief ゆびカーソルクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if !defined __APP_TOOL_FINGER_CURSOR_H_INCLUDED__
#define __APP_TOOL_FINGER_CURSOR_H_INCLUDED__
#pragma once

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Ui/UIView.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class FingerCursor : public app::ui::UIView
  {
  public:
    enum FormType
    {
      //  指先が下向き
      FORM_TYPE_DOWN  = 0,
      //  指先が上向き
      FORM_TYPE_UP,
    };

    //  レイアウトデータ作成
    static void*  CreateLytBinData( app::util::Heap * heap );
    //  レイアウトデータ破棄
    static void   DeleteLytBinData( void* pBinData );

    //-----------------------------------------------------------------------------
    /**
     * @brief   コンストラクタ
     *
     * @param   heap  ヒープ
     * @param   buf   リソースの確保場所
     */
    //-----------------------------------------------------------------------------
    FingerCursor( app::util::Heap * heap, void * pBuf, app::util::AppRenderingManager* pRenderingManager, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType, const u8 drawOrder );

    virtual ~FingerCursor();

    //-----------------------------------------------------------------------------
    /**
     * @brief   アニメ開始
     */
    //-----------------------------------------------------------------------------
    void StartAnime( const FormType formType );

    //-----------------------------------------------------------------------------
    /**
     * @brief   押すアニメ開始
     */
    //-----------------------------------------------------------------------------
    void PushAnime( const FormType formType );

    //-----------------------------------------------------------------------------
    /**
     * @brief   押すアニメ終了しているか
     */
    //-----------------------------------------------------------------------------
    bool IsEndPushAnime( const FormType formType );

    //-----------------------------------------------------------------------------
    /**
     * @brief   座標設定
     */
    //-----------------------------------------------------------------------------
    void SetPos( gfl2::math::VEC3& rPos );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    struct ANIME_DATA
    {
      u32 idleAnimeId;
      u32 pushAnimeId;
    };

    app::util::AppRenderingManager* m_pRenderingManager;

    u8 m_drawOrder;

    static const ANIME_DATA s_aAnimeData[];
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_FINGER_CURSOR_H_INCLUDED__