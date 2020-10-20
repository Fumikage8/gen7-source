#if !defined(__NETAPP_JOINFESTA_ROTOM_FADE_H_INCLUDED__)
#define __NETAPP_JOINFESTA_ROTOM_FADE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    RotomFade.h
 * @date    2016/03/17 17:29:57
 * @author  ichiraku_katsuhiko
 * @brief   ロトムフェード (RotomFadeのコピー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gflib2
#include <AppLib/include/ui/UIView.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(View)

//------------------------------------------------------------------------------
// @brief   ロトムフェード
//------------------------------------------------------------------------------
class RotomFade
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( RotomFade );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  RotomFade( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~RotomFade( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   アニメの開始
   */
  //------------------------------------------------------------------
  void StartRotomFade( void );

  //------------------------------------------------------------------
  /**
   * @brief   アニメの終了
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsEndRotomFade( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( void* pAppLytBuff );

  void StartRotomInAnime();
  bool InEndRotomInAnime();

  /**
  * @enum LayoutWorkID
  * レイアウトID
  */
  enum LayoutWorkID {
    LAYOUT_WORK_ID_UPPER,
    LAYOUT_WORK_ID_LOWER,
    LAYOUT_WORK_ID_MAX,
  };

  enum {
    SEQ_IDLE,
    SEQ_FADE_IN,
    SEQ_FADE_WAIT,
    SEQ_ROTOM_IN_ANIME_START,
    SEQ_ROTOM_IN_ANIME_WAIT,
    SEQ_END,
  };

private:
  app::util::Heap*                  m_pHeap;                  //!< ヒープ
  app::util::AppRenderingManager*   m_pAppRenderingManager;
  u8                                m_seq;


};

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)


#endif // __NETAPP_JOINFESTA_ROTOM_FADE_H_INCLUDED__