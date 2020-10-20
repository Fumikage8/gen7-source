#if !defined( __RESORT_CHARA_MODEL_H_INCLUDED__ )
#define __RESORT_CHARA_MODEL_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ResortCharaModel.h
 * @date    2016/03/04 10:48:28
 * @author  fukushima_yuya
 * @brief   ポケリゾート：管理人モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Tool/AppToolCharaSimpleModel.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
class CharaSimpleModel;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_BEGIN(util)
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
class CharaModelFactory;
class DressUpModelResourceManager;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(instance)
class DrawEnvNode;
GFL_NAMESPACE_END(instance)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   キャラモデルリスナー
//------------------------------------------------------------------------------
class ResortCharaModelListener
{
public:
  //------------------------------------------------------------------
  /**
   * @brief   キャラアニメの開始リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestStartCharaAnim( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   キャラアニメの終了リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestEndCharaAnim( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   口パクの開始リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestStartMouthAnim( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   口を閉じるリクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestCloseMouth( void ) = 0;
};

//------------------------------------------------------------------------------
// @brief   ポケリゾート：管理人モデル
//------------------------------------------------------------------------------
class ResortCharaModel
  : public ResortCharaModelListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortCharaModel);

public:
  struct SETUP_PARAM
  {
    app::util::Heap*                                      heap;
    app::util::AppRenderingManager*                       renMan;
    app::util::AppRenderingManager::RenderPlaceType::Tag  placeType[ app::util::AppRenderingManager::RenderPlaceType::NUM ];
    gfl2::fs::AsyncFileManager*                           afm;
    u32 arcID;

    SETUP_PARAM( void )
      : heap(NULL)
      , renMan(NULL)
      , afm(NULL)
      , arcID(0)
    {
      for( u32 i=0; i<GFL_NELEMS(placeType); ++i )
      {
        placeType[i] = app::util::AppRenderingManager::RenderPlaceType::NUM;
      }
    }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  ResortCharaModel( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ResortCharaModel( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   *
   * @param   param   セットアップパラメータ
   */
  //------------------------------------------------------------------
  void Initialize( SETUP_PARAM& param );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  void Terminate( void );

  //------------------------------------------------------------------
  /**
   * @brief   破棄可能か？
   *
   * @return  "true  == 破棄可"
   * @return  "false == 破棄不可"
   */
  //------------------------------------------------------------------
  bool CanTerminate( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   ロードリクエスト
   *
   * @param   charaID   キャラID
   */
  //------------------------------------------------------------------
  void RequestLoad( const s32 charaID );

  //------------------------------------------------------------------
  /**
   * @brief   モデルの準備が完了したか？
   *
   * @return  "true  == 完了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsModelReady( void ) { return (m_seqMain == 2); }


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   表示切替
   *
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible( bool visible );

private:
  //------------------------------------------------------------------
  /**
   * @brief   現在位置の設定
   *
   * @param   pos   位置座標
   */
  //------------------------------------------------------------------
  void SetPosition( gfl2::math::Vector3 pos );

  //------------------------------------------------------------------
  /**
   * @brief   現在位置の取得
   *
   * @return  現在位置座標
   */
  //------------------------------------------------------------------
  gfl2::math::Vector3 GetPosition( void ) const { return m_position; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   モーションの再生
   *
   * @param   animID    アニメID
   * @param   loop      ループフラグ
   */
  //------------------------------------------------------------------
  void PlayMotion( const u32 animID, const bool loop );

  //------------------------------------------------------------------
  /**
   * @brief   モーションの停止
   */
  //------------------------------------------------------------------
  void StopMotion( void );


private:
  //------------------------------------------------------------------
  /**
   * @brief   キャラアニメの更新
   */
  //------------------------------------------------------------------
  void UpdateAnimChara( void );

  //------------------------------------------------------------------
  /**
   * @brief   口パクアニメの更新
   */
  //------------------------------------------------------------------
  void UpdateAnimMouth( void );


private:  // ResortCharaModelListener
  //------------------------------------------------------------------
  /**
   * @brief   キャラアニメの開始リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestStartCharaAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   キャラアニメの終了リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestEndCharaAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   口パクの開始リクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestStartMouthAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   口を閉じるリクエスト
   */
  //------------------------------------------------------------------
  virtual void RequestCloseMouth( void );


private:
  gfl2::heap::HeapBase*               m_heap;

  gfl2::util::GLHeapAllocator*        m_modelAllocator;

  app::util::AppRenderingManager*     m_renMan;

private:
  poke_3d::model::CharaModelFactory*  m_modelFactory;

  gfl2::heap::HeapBase*               m_modelFactoryHeap;

  poke_3d::model::CharaModel*         m_charaModel;

private:
  SETUP_PARAM   m_setupParam;

private:
  u32     m_arcID;

  bool    m_terminate;

  u8      m_seqMain;

  u8      m_seqSub;

  u8      m_seqDel;

  s32     m_charaID;

  s32     m_loadCharaID;

  u8      m_autoAnimSeq;

  u32     m_autoAnimFrame;

  u32     m_autoAnimInterval;

  bool    m_stopMouthAnim;

  u8      m_charaAnimSeq;

private:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_cameraEnvNode;   //!< カメラを含む環境

  poke_3d::model::BaseCamera*                                 m_baseCamera;      //!< カメラ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_light;           //!< 上画面用ライト

  gfl2::math::Vector3                                         m_position;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_CHARA_MODEL_H_INCLUDED__
