//======================================================================
/**
 * @file TimerIconView.h
 * @date 2015/10/13 13:33:25
 * @author uchida_yuto
 * @brief タイマーアイコンView
 * @note
 *        動作実績ができたら AppLibのtoolに昇格する
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_TIMER_ICON_VIEW_HEADER_H_INCLUDED__
#define __APP_TOOL_TIMER_ICON_VIEW_HEADER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Ui/UIView.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  /** タイマーアイコンUIView */
  class TimerIconUIView: public app::ui::UIView
  {
  public:
    //  タイマーアイコンのリソースのarcidとdataidを取得
    static u32 GetArcId();
    static u32 GetArcDataId();
    static bool IsArcDataComp();

    TimerIconUIView( 
    app::util::Heap * pHeap,
    app::util::AppRenderingManager* pRenderingManager,
    void* pLytBuff,
    app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType );
    virtual ~TimerIconUIView();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    //  レイアウトパラメータ設定
    void SetLayoutParam( const u32 layoutOrder, const u32 layoutNo );

    //  アニメ開始
    //  終了のEndAnime()を呼んでないと連続で呼べない
    void StartAnime();

    //  アニメ停止
    void EndAnime();

    //  表示位置設定
    void SetIconPos( gfl2::math::VEC3& rPos );

    //  STR行列を設定
    void SetSRTMtx( const gfl2::math::MTX34& rMat );

    void AttachPane( gfl2::lyt::LytPane* pAttachPane );
    void RemovePane();

  private:
    void _Clear()
    {
      m_pRenderingManager = NULL;
      m_bVisible    = false;
      m_pRootPane   = NULL;
      m_pIconPane   = NULL;
      m_pCenterLPane  = NULL;
      m_pCenterSPane  = NULL;

      m_layoutNo    = 0;
      m_layoutOrder = gfl2::lyt::ORDER_NORMAL;

      gfl2::std::MemClear( &m_fadeInAnimeState, sizeof( m_fadeInAnimeState ) );
      gfl2::std::MemClear( &m_fadeOutAnimeState, sizeof( m_fadeOutAnimeState ) );
      gfl2::std::MemClear( &m_keepAnimeState, sizeof( m_keepAnimeState ) );
    }

    struct STATE_DATA
    {
      u32 state;

      union
      {
        u32 work[ 8 ];
        struct FADE_ANIME
        {
          u32 nowFrame;
          u32 maxFrame;
        } fadeAnime;

        struct KEEP_ANIME
        {
          f32 CenterLRotateMaxZ;
          f32 CenterSRotateMaxZ;

          u32 nowFrame;
          u32 maxFrame;

        } keepAnime;

      } data;
    };

    app::util::AppRenderingManager* m_pRenderingManager;
    u32   m_layoutNo;
    u32   m_layoutOrder;
    bool  m_bVisible;

    gfl2::lyt::LytPane* m_pIconPane;
    gfl2::lyt::LytPane* m_pRootPane;
    gfl2::lyt::LytPane* m_pCenterLPane;
    gfl2::lyt::LytPane* m_pCenterSPane;

    gfl2::math::MTX34   m_SRTMat;

    STATE_DATA  m_fadeInAnimeState;
    STATE_DATA  m_fadeOutAnimeState;
    STATE_DATA  m_keepAnimeState;
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_TIMER_ICON_VIEW_HEADER_H_INCLUDED__