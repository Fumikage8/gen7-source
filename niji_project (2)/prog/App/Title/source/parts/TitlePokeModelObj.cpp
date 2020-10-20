//=============================================================================
/**
 * @file TitlePokeModelDraw.h
 * @brief タイトルのポケモンモデルオブジェクト
 * @author yuto_uchida
 * @date 2015.05.15
 */
//=============================================================================
#include "App/Title/source/parts/TitlePokeModelObj.h"

#include "PokeTool/include/PokeModelSystem.h"

// nijiのインクルード
#include <AppLib/include/Util/AppRenderingManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  PokeModelObj::PokeModelObj( const INIT_PARAM& param )
  {
    GFL_ASSERT( param.pPokeModelSys != NULL );
    GFL_ASSERT( param.pRenderingManager != NULL );

    //  初期化
    _Clear();

    m_initParam = param;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  PokeModelObj::~PokeModelObj()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  void PokeModelObj::Initialize()
  {
    //  ポケモンモデル作成
    m_pPokeModel  = m_initParam.pPokeModelSys->CreateModelSimple( &m_initParam.setupParam, m_initParam.setupOpt );
    GFL_ASSERT( m_pPokeModel );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void PokeModelObj::Terminate()
  {
    _SetState( STATE_TYPE_END );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 破棄可能か
    */
  //-------------------------------------------------------------------------
  bool PokeModelObj::IsDelete()
  {
    //  終了ステートでモデルが破棄されているのであれば破棄可能
    if( m_state == STATE_TYPE_END )
    {
      return  ( m_pPokeModel == NULL );
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void PokeModelObj::Update()
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      GFL_ASSERT( m_pPokeModel );
      if( m_pPokeModel->IsFinishLoad() )
      {
        m_pPokeModel->Create();

        m_pPokeModel->SetPosition(0, 0, 0);
        m_pPokeModel->ChangeAnimation(PokeTool::MODEL_ANIME_BUTURI1, true);
        m_pPokeModel->SetAnimationIsLoop(true);

        m_initParam.pRenderingManager->AddPokeModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pPokeModel );

        _SetState( STATE_TYPE_UPDATE );
      }

      return;
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pPokeModel->UpdateAnimation();
    }

    if( m_state == STATE_TYPE_END )
    {
      if( m_subStep == 0 )
      {
        if( m_pPokeModel != NULL )
        {
          m_initParam.pRenderingManager->RemovePokeModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pPokeModel );
          m_subStep = 1;
        }
      }
      else if( m_subStep == 1 )
      {
        if( m_pPokeModel != NULL )
        {
          m_initParam.pPokeModelSys->DeleteModel( m_pPokeModel );
          m_pPokeModel  = NULL;
        }

        m_subStep = 2;
      }
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void PokeModelObj::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( IsReady() == false )
    {
      //  まだ使えないので描画しない
      return;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメ再生
    */
  //------------------------------------------------------------------
  void PokeModelObj::SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop )
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( m_pPokeModel );

    m_pPokeModel->ChangeAnimation( animeId );
    m_pPokeModel->SetAnimationIsLoop( bLoop );
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void PokeModelObj::_SetState( const StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_END )
    {
      m_subStep = 0;
    }
  }

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )
