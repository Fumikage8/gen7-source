//=============================================================================
/**
 * @file TitlePokeModelDraw.h
 * @brief タイトルのポケモンモデルオブジェクト
 * @author yuto_uchida
 * @date 2015.05.15
 */
//=============================================================================
#if !defined( NIJI_PROJECT_TITLE_POKE_MODEL_OBJ_H_INCLUDED )
#define NIJI_PROJECT_TITLE_POKE_MODEL_OBJ_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

#include <PokeTool/include/PokeModel.h>

//前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )

  class  AppRenderingManager;

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  /** タイトル用ポケモンモデルオブジェクト */
  class PokeModelObj
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeModelObj );  // コピーを禁止

    public:
      enum StateType
      {
        STATE_TYPE_NONE = 0,
        STATE_TYPE_LOAD,
        STATE_TYPE_UPDATE,
        STATE_TYPE_END,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        _tag_init_param()
        {
          pPokeModelSys     = NULL;
          pRenderingManager = NULL;
        }

        PokeTool::SimpleParam             setupParam;
        PokeTool::PokeModel::SetupOption  setupOpt;
        PokeTool::PokeModelSystem*        pPokeModelSys;

        //  ポケモン用パイプライン
        app::util::AppRenderingManager*   pRenderingManager;

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      PokeModelObj( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~PokeModelObj();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize();

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      void Terminate();

      //  破棄可能か
      bool IsDelete();

      //  座標取得
      const gfl2::math::Vector3& GetPos() const
      {
        return m_pPokeModel->GetPosition();
      }

      //  座標設定
      void SetPos( const gfl2::math::Vector3& pos )
      {
        m_pPokeModel->SetPosition( pos );
      }

      //  アニメ再生
      void SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop );

      //  使える準備ができているかどうか
      bool IsReady() const
      {
        return  ( STATE_TYPE_LOAD < m_state );
      }

      /**
      * @brief 更新
      */
      void Update();

      /**
      * @brief 描画
      */
      void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      /**
      * @brief  現在のステート
      */
      const StateType GetState() const { return m_state; }


    private:
      void _Clear()
      {
        m_pPokeModel  = NULL;
        m_state       = STATE_TYPE_NONE;
        m_subStep     = 0;
      }

      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      u8          m_subStep;

      PokeTool::PokeModel*        m_pPokeModel;
  };

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_TITLE_POKE_MODEL_OBJ_H_INCLUDED 
