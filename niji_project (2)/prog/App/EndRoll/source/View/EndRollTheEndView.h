// ============================================================================
/*
 * @file EndRollTheEndView.h
 * @brief エンドロールアプリのTheEnd表示
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( ENDROLL_THE_END_VIEW_H_INCLUDE )
#define ENDROLL_THE_END_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/EndRoll/source/System/EndRollWork.h"
#include "App/EndRoll/source/EndRollResourceID.h"

#include "System/include/DressUp.h"

//  リソースのインクルード

//  前方宣言
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(View)

  class EndRollTheEndView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( EndRollTheEndView );
  public:
    //  アニメ一覧
    enum Anime
    {
      ANIME_VIEW_IN  = 0,
      ANIME_VIEW_LOOP,
      ANIME_VIEW_IMG,
      ANIME_VIEW_OUT,
      ANIME_MAX,
    };

    EndRollTheEndView( App::EndRoll::System::EndRollWork* pWork, const App::EndRoll::EndRollResourceID resId );
    virtual~EndRollTheEndView();

    //  一枚絵表示設定
    void SetImg( poke_3d::model::DressUpParam dressUpParam );

    //  アニメ再生
    void PlayAnime( const Anime animeId );

    //  アニメ再生フレーム最大数を取得
    u32 GetPlayAnimeLastFrame( const Anime animeId );

    //  アニメ再生中か
    bool IsPlayAnime( const Anime animeId );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update(void);

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID  = 0,
    };

    //------------------------------------------------------------------
    /**
      *  @func     _Create2DOneLayout
      *  @brief    ひとつのLayoutリソースに特化するかわりに必要なパラメータを減らしたCreate2D簡易版
      *  @note     グラフィックフォントを使っているので専用でレイアウト作成が必要
      */
    //------------------------------------------------------------------
    void  _Create2DOneLayout(
      NetAppLib::System::ApplicationWorkBase* pWork,
      NetAppLib::System::ResourceID           layoutResourceID,
      gfl2::lyt::LytArcIndex                  layoutArcIndex,
      u32                                     animationMax,
      app::util::G2DUtil::SETUP_DISPLAY_TYPE  setupDisplayType,
      NetAppLib::Message::MessageUtility*     pMessageUtility,
      gfl2::fs::ArcFile::ARCDATID             messageArcDatID
    );

    void _Clear()
    {
      m_pWork           = NULL;
    }

    App::EndRoll::System::EndRollWork*     m_pWork;

    //  アニメid一覧
    static const u32 ms_aAnimeId[ ANIME_MAX ];
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)

#endif // ENDROLL_THE_END_VIEW_H_INCLUDE
