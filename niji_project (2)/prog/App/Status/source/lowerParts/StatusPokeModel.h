#if !defined( __APP_STATUS_POKE_MODEL_H_INCLUDED__ )
#define __APP_STATUS_POKE_MODEL_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    StatusPokeModel.h
 * @date    2015/12/03 18:32:32
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ポケモンモデル(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeModel.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>
#include <fs/include/gfl2_Fs.h>
#include <ui/include/gfl2_UI.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)
class DrawerBase;
GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN( tool )
class PokeSimpleModelInFrame;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN(util)
class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(PokeTool)
class PokeModelSystem;
class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(pml)
GFL_NAMESPACE_BEGIN(pokepara)
class PokemonParam;
GFL_NAMESPACE_END(pokepara)
GFL_NAMESPACE_END(pml)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   ポケモンモデル(下)
//------------------------------------------------------------------------------
class PokeModel
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeModel );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  PokeModel( DrawerBase* pBase, gfl2::fs::AsyncFileManager* pAsyncFileManager, app::util::AppRenderingManager* pRenderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~PokeModel( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  bool Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   */
  //------------------------------------------------------------------
  bool Terminate( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   モデルシステムの生成
   */
  //------------------------------------------------------------------
  void CreateModelSystem( void );

  //------------------------------------------------------------------
  /**
   * @brief   モデルシステムの準備完了か
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsReadyModelSystem( void );

  //------------------------------------------------------------------
  /**
   * @brief   モデルシステムの初期化
   */
  //------------------------------------------------------------------
  void InitModelSystem( void );

  //------------------------------------------------------------------
  /**
   * @brief   モデルの生成
   */
  //------------------------------------------------------------------
  void CreateModel( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   モデルのセット
   *
   * @param   pp    ポケモンパラメータ
   */
  //------------------------------------------------------------------
  void SetPokeModel( pml::pokepara::PokemonParam* pp );

  //------------------------------------------------------------------
  /**
   * @brief   モデルの準備チェック
   */
  //------------------------------------------------------------------
  bool IsModelReady( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief   モデルの表示
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisibleModel( bool visible );

private:
  //------------------------------------------------------------------
  /**
   * @brief   デフォルトアニメの再生
   */
  //------------------------------------------------------------------
  void StartDefaultAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンのボイス再生
   */
  //------------------------------------------------------------------
  void PlayPokeVoice( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   モデルのタッチ判定
   *
   * @param   touch   タッチパネル
   *
   * @note    内部でStartTouchAnimを呼ぶための関数
   */
  //------------------------------------------------------------------
  void CheckTouchModel( gfl2::ui::TouchPanel* touch );

private:
  //------------------------------------------------------------------
  /**
   * @brief   タッチアニメの再生
   */
  //------------------------------------------------------------------
  void StartTouchAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   タッチアニメの更新
   */
  //------------------------------------------------------------------
  void UpdateTouchAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   タッチアニメの終了判定
   *
   * @return  "true  = 終了"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsTouchAnimEnd( void );

  //------------------------------------------------------------------
  /**
   * @brief   利用可能なアニメのセット
   */
  //------------------------------------------------------------------
  void SetAvailableAnim( void );

private:
  static const u32 AVAILABLE_ANIM_MAX = 8;    //!< 8はBUTURI1~4 + TOKUSHU1~4

private:
  DrawerBase*   m_pBase;

  gfl2::fs::AsyncFileManager*           m_pAsyncFileManager;

  app::util::AppRenderingManager*       m_pAppRenderingManager;

  PokeTool::PokeModelSystem*            m_pPokeModelSystem;

  app::tool::PokeSimpleModelInFrame*    m_pInFrameModel;

  pml::pokepara::PokemonParam*          m_pPokeParam;

  PokeTool::SimpleParam                 m_SimpleParam;

  PokeTool::MODEL_ANIME                 m_ModeAnimID;

private:
  u8    m_InitSeq;

  u8    m_DeleteSeq;

  bool  m_bReqStartDefAnim;

  bool  m_bReqPlayVoice;

  u32   m_AvailableAnimNum;

  PokeTool::MODEL_ANIME     m_AvailableAnimTable[AVAILABLE_ANIM_MAX];

  bool  m_bReqAvailableAnim;

private:
  gfl2::lyt::LytPane*       m_pModelAnimArea;
};

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)

#endif // __APP_STATUS_POKE_MODEL_H_INCLUDED__
