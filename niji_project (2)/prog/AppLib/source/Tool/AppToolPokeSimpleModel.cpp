//=============================================================================
/**
 * @file AppPokeSimpleModel.h
 * @brief ポケモン簡易モデル
 * @author yuto_uchida
 * @date 2015.07.23
 * @note
 *      ポケモンモデルのロード処理をクラス内部で管理しているので、再ロードについて使う側が気にする必要はない
 */
//=============================================================================
#include "AppLib/include/Tool/AppToolPokeSimpleModel.h"

#include "PokeTool/include/PokeModelSystem.h"

// nijiのインクルード
#include <AppLib/include/Util/AppRenderingManager.h>

// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

#if PM_DEBUG
  #define POKE_SIMPLE_MODEL_TEST  0
#else
  #define POKE_SIMPLE_MODEL_TEST  0
#endif

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  PokeSimpleModel::PokeSimpleModel( const INIT_PARAM& param )
  {
    GFL_ASSERT( param.pPokeModelSys != NULL );
    GFL_ASSERT( param.pRenderingManager != NULL );

    _Clear();

    //  初期化
    {
      m_initParam = param;
    }
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  PokeSimpleModel::~PokeSimpleModel()
  {
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      アニメタイプ設定
  * @param[in]  animeType  : アニメタイプ
  * @note
  *             必ずStartLoad()関数前に呼んでください！！！
  */
  //--------------------------------------------------------------------------------------------
  void PokeSimpleModel::SetAnimeType( const PokeTool::MODEL_ANIMETYPE animeType )
  {
    GFL_ASSERT_STOP( animeType < PokeTool::MODEL_ANIMETYPE_MAX );
    m_initParam.setupOpt.animeType  = animeType;
  }

  /**
  * @brief      実際に割り当てられるアニメタイプ取得
  * @note       SetAnimeTypeでMODEL_ANIMETYPE_APPを指定しているときに
  *             MODEL_ANIMETYPE_BATTLEかMODEL_ANIMETYPE_KAWAIGARIどちらが実際には割り当てられるかを返す。
  */
  PokeTool::MODEL_ANIMETYPE PokeSimpleModel::GetRealAnimeType() const
  {
    // APP用モーションをMODEL_ANIMETYPE_BATTLEかMODEL_ANIMETYPE_KAWAIGARIで割り当て直す
    if( m_initParam.setupOpt.animeType == PokeTool::MODEL_ANIMETYPE_APP )
    {
      if( PokeTool::CheckUsingKawaigariInApp( m_initParam.setupParam.monsNo , m_initParam.setupParam.formNo ) )
      {
        return PokeTool::MODEL_ANIMETYPE_KAWAIGARI;
      }
      else
      {
        return PokeTool::MODEL_ANIMETYPE_BATTLE;
      }
    }

    return  this->GetAnimeType();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ポケモンロード
  * @param[in]  monsNo       : ロードするポケモンを指定
  */
  //--------------------------------------------------------------------------------------------
  void PokeSimpleModel::StartLoad( const MonsNo monsNo )
  {
    //  もう破棄指定しているのでロードできない
    if( m_bTerminate == true )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( monsNo != MONSNO_NULL );

    if( m_initParam.setupParam.monsNo != monsNo )
    {
      //  現在参照しているモンスターと違うのでロードする
      //  あくまでロードするnoを設定してロードするステートの遷移はUpdate側で行う
      m_loadMonsNo = monsNo;
    }
    else
    {
    // @fix GFNMCat[1020] データが同じでもUpdate()内でまだロードが完了していないケースがあるので、ここで初期化はしない
//      m_loadMonsNo  = MONSNO_NULL;
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ポケモンロード
  * @param[in]  param       : ロードするポケモンを指定
  */
  //--------------------------------------------------------------------------------------------
  void PokeSimpleModel::StartLoad( const PokeTool::SimpleParam& rParam )
  {
    //  もう破棄指定しているのでロードできない
    if( m_bTerminate == true )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( rParam.monsNo != MONSNO_NULL );

    //  データに違いがあればロード
    if( 
         ( m_initParam.setupParam.monsNo != rParam.monsNo )
      || ( m_initParam.setupParam.formNo != rParam.formNo )
      || ( m_initParam.setupParam.isEgg != rParam.isEgg )
      || ( m_initParam.setupParam.sex != rParam.sex )
      || ( m_initParam.setupParam.isRare != rParam.isRare )
      || ( m_initParam.setupParam.perRand != rParam.perRand )
      )
    {
      //  現在参照しているモンスターと違うのでロードする
      //  @fix GFNMCat[1020] ロードするデータはロードした段階で最新にしないとロード時のデータチェックにずれが生まれる
      m_loadMonsNo            = rParam.monsNo;
      m_initParam.setupParam  = rParam;
    }
    else
    {
      // @fix GFNMCat[1020] データが同じでもUpdate()内でまだロードが完了していないケースがあるので、ここで初期化はしない
//      m_loadMonsNo  = MONSNO_NULL;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::Terminate()
  {
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    //
    // 派生クラスからも呼んでいますので、コードを変更する際は注意して下さい。
    //
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    if( m_state == STATE_TYPE_UPDATE )
    {
      _SetState( STATE_TYPE_END );
    }

    m_bTerminate  = true;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 破棄可能か
    */
  //-------------------------------------------------------------------------
  bool PokeSimpleModel::IsDelete()
  {
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    //
    // 派生クラスからも呼んでいますので、コードを変更する際は注意して下さい。
    //
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    
    //  内部で更新をする
    //  １フレームで連続で呼んでも問題ないので使う側が更新を呼んでも大丈夫だし
    //  呼ばなくても大丈夫
    Update();

    //  終了ステートでモデルが破棄されているのであれば破棄可能
    if( m_bTerminate == true )
    {
      if( m_state == STATE_TYPE_IDLE )
      {
        return  ( m_pPokeModel == NULL );
      }
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void PokeSimpleModel::Update()
  {
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    //
    // 派生クラスからも呼んでいますので、コードを変更する際は注意して下さい。
    //
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    if( m_state == STATE_TYPE_IDLE )
    {
      //  待機中
      if( m_bTerminate == false )
      {
        //  このステートでないとロードステートに遷移しない
        if( m_loadMonsNo != MONSNO_NULL )
        {
          m_initParam.setupParam.monsNo = m_loadMonsNo;
          _SetState( STATE_TYPE_LOAD );

          m_loadMonsNo  = MONSNO_NULL;
        }
      }
    }

    if( m_state == STATE_TYPE_LOAD )
    {
      enum
      {
        SEQ_LOAD_CREATE  = 0,
        SEQ_LOAD_WAIT_CREATE
      };

      if( m_subStep == SEQ_LOAD_CREATE )
      {
        //  ポケモンモデル作成
        m_pPokeModel  = m_initParam.pPokeModelSys->CreateModelSimple( &m_initParam.setupParam, m_initParam.setupOpt );
        GFL_ASSERT( m_pPokeModel );

        ++m_subStep;
      }
      
      if( m_subStep == SEQ_LOAD_WAIT_CREATE )
      {
        GFL_ASSERT( m_pPokeModel );
        if( m_pPokeModel->IsFinishLoad() )
        {
          if( m_loadMonsNo != MONSNO_NULL )
          {
            //  ロードするポケモンNoがあるなら再度ロードするために一度破棄ステートに遷移してからidleステートに遷移してロード
            _SetState( STATE_TYPE_END );
          }
          else
          if( m_bTerminate == true )
          {
            //  ロード中に破棄指定しているのであらば破棄ステートに移行
            _SetState( STATE_TYPE_END );
          }
          else
          {
            //  ステートを先に切り替えないとパラメータが反映しない
            _SetState( STATE_TYPE_UPDATE );

            //  モデル作成してAppRenderingに描画登録
            m_pPokeModel->Create();

            m_pPokeModel->SetPosition( m_pos );

            m_initParam.pRenderingManager->AddPokeModel( m_initParam.renderPlace, m_pPokeModel, m_initParam.layerNo );

            SetVisible( m_bVisible );

#if POKE_SIMPLE_MODEL_TEST
            //@ test
            {
              m_bAnimeLoop  = false;
            }
#endif

            //  設定が保管している場合はアニメ設定
            if( m_animeId != PokeTool::MODEL_ANIME_MAX )
            {
              _SetAnime( m_animeId, m_bAnimeLoop, m_bAnimeForce );
            }
            _PauseAnime( m_bAnimePause );

#if POKE_SIMPLE_MODEL_TEST
            //@ test
            UCHIDA_PRINT( "IsAvailableAnimation( (PokeTool::MODEL_ANIME)100000 ): %d\n", IsAvailableAnimation( (PokeTool::MODEL_ANIME)100000 ) );
#endif
          }
        }
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_loadMonsNo != MONSNO_NULL )
      {
        //  ロードするポケモンNoがあるなら再度ロードするために破棄ステートに遷移して待機ステートに
        _SetState( STATE_TYPE_END );
      }
      else
      {
        //  m_initParam.pPokeModelSysのクラスの更新処理でアニメ更新しているのでここではやらない
//        m_pPokeModel->UpdateAnimation();
#if POKE_SIMPLE_MODEL_TEST
        //@ test
        UCHIDA_PRINT( "IsEndAnime(): %d\n", IsEndAnime() );
#endif
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      enum
      {
        SEQ_END_REMOVE_RENDER = 0,
        SEQ_END_MODEL_DELETE
      };

      if( m_subStep == SEQ_END_REMOVE_RENDER )
      {
        if( m_pPokeModel != NULL )
        {
          if( m_pPokeModel->IsCreated() == true )
          {
            m_initParam.pRenderingManager->RemovePokeModel( m_initParam.renderPlace, m_pPokeModel, m_initParam.layerNo );
          }
        }

        ++m_subStep;
      }
      else
      if( m_subStep == SEQ_END_MODEL_DELETE )
      {
        if( m_pPokeModel != NULL )
        {
          if( m_pPokeModel->CanDestroy() == true )
          {
            m_initParam.pPokeModelSys->DeleteModel( m_pPokeModel );
            m_pPokeModel  = NULL;
          }
        }

        if( m_pPokeModel == NULL )
        {
          //  待機ステートに
          _SetState( STATE_TYPE_IDLE );
        }
      }
    }
  }


  //------------------------------------------------------------------
  /**
   * @brief メガゲンガーシェーダーに指定する位置を更新する
   * @note  モデル生成/破棄中に呼んでも問題ありません。
   *        メガゲンガーでないときに呼んでも問題ありません。
   */
  //------------------------------------------------------------------
  void PokeSimpleModel::UpdateMegaGangarEffectState(void)
  {
    if( IsReady() == false )
    {
      // まだ準備ができていない
      return;
    }
    m_pPokeModel->SetMegaGangarEffectState( m_pPokeModel->GetPosition().y );
  }


  //------------------------------------------------------------------
  /**
    * @brief   アニメ再生
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop, const bool bForce )
  {
    if( IsReady() == true )
    {
      _SetAnime( animeId, bLoop, bForce );
      _PauseAnime( m_bAnimePause );  // アニメ変更と一時停止は別物。一時停止フラグが立っているなら、アニメ変更をしても一時停止させる。
    }

    //  設定を保管して、ロード終了時に設定
    m_animeId       = animeId;
    m_bAnimeLoop    = bLoop;
    m_bAnimeForce   = bForce;
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメが終了しているか
    */
  //------------------------------------------------------------------
  bool PokeSimpleModel::IsEndAnime()
  {
    if( IsReady() == false )
    {
      //  まだ準備ができたいないのでチェックなどできない
      return  true;
    }

    if( m_animeId == PokeTool::MODEL_ANIME_MAX )
    {
      //  設定していないのでアウト
      return  true;
    }

    if( m_bAnimeLoop == true )
    {
      //  ループ再生なので終わりはない
      return  false;
    }

    if( m_pPokeModel->IsAnimationLoop() )
    {
      //  モデル上でループ再生
      return  false;
    }

    return  ( m_pPokeModel->IsAnimationLastFrame() == 1 );
  }

  //------------------------------------------------------------------
  /**
    * @brief   引数設定したアニメidが有効か
    */
  //------------------------------------------------------------------
  bool PokeSimpleModel::IsAvailableAnimation( const PokeTool::MODEL_ANIME animeId )
  {
    if( IsReady() == false )
    {
      //  まだ準備ができたいないのでチェックなどできない
      return  false;
    }

    return  m_pPokeModel->IsAvailableAnimation( animeId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   引数設定したアニメidがランダム再生時に再生して良いモーションか？(待機で始まり、待機で戻るか？)
    */
  //------------------------------------------------------------------
  bool PokeSimpleModel::CheckCanOneshotMotion( const PokeTool::MODEL_ANIME animeId )
  {
    if( IsReady() == false )
    {
      //  まだ準備ができたいないのでチェックなどできない
      return  false;
    }

    return  m_pPokeModel->CheckCanOneshotMotion( animeId );
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメ一時停止
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::PauseAnime( const bool bPause )
  {
    if( IsReady() == true )
    {
      _PauseAnime( bPause );
    }

    //  設定を保管して、ロード終了時に設定
    m_bAnimePause    = bPause;
  }

  //------------------------------------------------------------------
  /**
    * @brief   表示有効設定
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::SetVisible( const bool bFlg )
  {
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    //
    // 派生クラスからも呼んでいますので、コードを変更する際は注意して下さい。
    //
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_pPokeModel->SetVisible( bFlg );
    }

    m_bVisible  = bFlg;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::_SetState( const StateType newStateType )
  {
    m_state   = newStateType;
    m_subStep = 0;
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメ再生
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::_SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop, const bool bForce )
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( m_pPokeModel );

    m_pPokeModel->ChangeAnimation( animeId, bForce );
    m_pPokeModel->SetAnimationIsLoop( bLoop );
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメ一時停止
    */
  //------------------------------------------------------------------
  void PokeSimpleModel::_PauseAnime( const bool bPause )
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( m_pPokeModel );

    m_pPokeModel->SetAnimationStepFrame( (bPause==false)?(1.0f):(0.0f) );  // 目ぱちや薄羽も止まる
  }

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
