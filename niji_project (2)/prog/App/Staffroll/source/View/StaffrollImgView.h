// ============================================================================
/*
 * @file StaffrollImgView.h
 * @brief スタッフロールアプリの１枚絵View
 * @date 2016.01.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( STAFFROLL_IMG_VIEW_H_INCLUDE )
#define STAFFROLL_IMG_VIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"

#include "App/Staffroll/source/System/StaffrollWork.h"
#include "App/Staffroll/source/StaffrollResourceID.h"

//  リソースのインクルード
#include <niji_conv_header/app/staffroll/data/staffroll_data.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class IconObject;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(View)

  class StaffrollImgView :
    public NetAppLib::System::NetApplicationViewBase
  {
    GFL_FORBID_COPY_AND_ASSIGN( StaffrollImgView );
  public:
    //  アニメ一覧
    enum Anime
    {
      ANIME_FADE_00  = 0,
      ANIME_FADE_01,
      ANIME_FADE_02,
      ANIME_FADE_03,
      ANIME_FADE_04,
      ANIME_FADE_05,
      ANIME_FADE_06,
      ANIME_FADE_07,
      ANIME_MAX,
    };

    StaffrollImgView( App::Staffroll::System::StaffrollWork* pWork, const App::Staffroll::StaffrollResourceID resId );
    virtual~StaffrollImgView();

    //  初期化
    //  true で終了
//    bool InitFunc();

    //  終了
    //  true で終了
//    bool EndFunc();

    //  フェードアニメ開始
//    void BeginFadeSkip( const u32 startLabelIdx = 0 );

    //  フェードアニメスキップ終了
//    bool IsEndFadeSkip();

    //  次のフェードアニメ
//    void NextFadeSkip();

    //  フェードアニメ開始
//    void StartFadeAnime();

    //  フェードアニメ中か
//    bool IsFadeAnime();

    //  アニメ再生
//    void PlayAnime( const Anime animeId );

    //  アニメ再生中か
//    bool IsPlayAnime( const Anime animeId );

    //  画像ロード設定
//    void StartTexLoadAsync( const u32 id );
//    bool IsTexLoaded();

    //  2枚のペインに画像反映してフリップ
//    void FlipTexTranslate();

    //  テクスチャー更新
//    void UpdateTex();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update(void);

    // UIView
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );


    u32 GetAnimeNo(void);
    void SetNextAnimeNo(void);
    void SetBgPatern(void);
    void StartBgAnime(void);
    bool IsBgAnime(void);
    void StartCharaAnimeLoop(void);
    void StartCharaAnimeOut(void);
    bool IsCharaAnimeOut(void);
    void StartCharaAnimeIn(void);
    bool IsCharaAnimeIn(void);


  private:
    enum LayoutWorkID
    {
      LAYOUT_WORK_ID_UPPER = 0,
      LAYOUT_WORK_ID_LOWER,
      LAYOUT_WORK_ID_MAX
    };

    void _Clear()
    {
      m_pWork           = NULL;
//      m_pImgObject      = NULL;
/*
      for( u32 i=0; i<LAYOUT_WORK_ID_MAX; i++ )
      {
        m_pBackImgPicturePane[i] = NULL;
        m_pFrontImgPicturePane[i] = NULL;
      }
*/
//      m_currentTexBufIdx  = 0;
//      m_fadeAnimeLabelIdx = -1;
//      m_bFlipImg  = false;

      m_animeNo = 0;

//      gfl2::std::MemClear( m_pLytTexBufData, sizeof( m_pLytTexBufData ) );
    }

    //  画像張替えオブジェクト
//    app::tool::IconObject*  m_pImgObject;
//    gfl2::lyt::LytPicture*  m_pBackImgPicturePane[LAYOUT_WORK_ID_MAX];
//    gfl2::lyt::LytPicture*  m_pFrontImgPicturePane[LAYOUT_WORK_ID_MAX];
//    u32                     m_currentTexBufIdx;
//    s32                     m_fadeAnimeLabelIdx;

//    app::tool::IconObject*  m_pLytTexBufData[ 2 ];

    App::Staffroll::System::StaffrollWork*     m_pWork;

//    bool                    m_bFlipImg;

    u32 m_animeNo;

    //  アニメid一覧
    static const u32 ms_aAnimeId[ LAYOUT_WORK_ID_MAX ][ ANIME_MAX ];

    static const u32 CHARA_MAX = 4;
    static const u32 CHARA_ANIME_LOOP_MAX = 9;

    struct CHARA_ANIME
    {
      u32 in;
      u32 out;
      u32 loop[CHARA_ANIME_LOOP_MAX];
    };

    static const CHARA_ANIME CharaAnimeTable[LAYOUT_WORK_ID_MAX][CHARA_MAX];

  private:
    void Initialize2D(
          App::Staffroll::System::StaffrollWork * work,
          const App::Staffroll::StaffrollResourceID res_id );
    void Terminate2D(void);
  };

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)

#endif // STAFFROLL_IMG_VIEW_H_INCLUDE
