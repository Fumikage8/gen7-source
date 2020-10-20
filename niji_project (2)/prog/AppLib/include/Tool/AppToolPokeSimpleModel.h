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
#if !defined( NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_H_INCLUDED )
#define NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

#include <PokeTool/include/PokeModel.h>

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"

//前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  /** ポケモン簡易モデル*/
  class PokeSimpleModel
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeSimpleModel );  // コピーを禁止

    protected:
      enum StateType
      {
        STATE_TYPE_IDLE   = 0,
        STATE_TYPE_LOAD,
        STATE_TYPE_UPDATE,
        STATE_TYPE_END,
      };

    public:
      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        _tag_init_param()
        {
          pPokeModelSys     = NULL;
          
          renderPlace       = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
          layerNo           = 0;

          pRenderingManager = NULL;
        }

        PokeTool::SimpleParam             setupParam;
        PokeTool::PokeModel::SetupOption  setupOpt;
        PokeTool::PokeModelSystem*        pPokeModelSys;

        app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace;
        u32                                                  layerNo;

        //  ポケモン用パイプライン
        app::util::AppRenderingManager*   pRenderingManager;

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      PokeSimpleModel( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~PokeSimpleModel();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      アニメタイプ設定
      * @param[in]  animeType  : アニメタイプ
      * @note
      *             必ずStartLoad()関数前に呼んでください！！！
      */
      //--------------------------------------------------------------------------------------------
      void SetAnimeType( const PokeTool::MODEL_ANIMETYPE animeType );

      PokeTool::MODEL_ANIMETYPE GetAnimeType() const { return m_initParam.setupOpt.animeType; }

      /**
      * @brief      実際に割り当てられるアニメタイプ取得
      * @note       SetAnimeTypeでMODEL_ANIMETYPE_APPを指定しているときに
      *             MODEL_ANIMETYPE_BATTLEかMODEL_ANIMETYPE_KAWAIGARIどちらが実際には割り当てられるかを返す。
      */
      PokeTool::MODEL_ANIMETYPE GetRealAnimeType() const;

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      ポケモンロード
      * @param[in]  monsNo       : ロードするポケモンを指定
      */
      //--------------------------------------------------------------------------------------------
      void StartLoad( const MonsNo monsNo );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      ポケモンロード
      * @param[in]  param       : ロードするポケモンを指定
      */
      //--------------------------------------------------------------------------------------------
      void StartLoad( const PokeTool::SimpleParam& rParam );

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      virtual void Terminate();

      //  破棄可能か
      virtual bool IsDelete();

      //  座標取得
      const gfl2::math::Vector3& GetPos() const { return m_pos; }

      //  座標設定
      void SetPos( const gfl2::math::Vector3& pos )
      {
        m_pos = pos;
        if( m_pPokeModel != NULL )
        {
          m_pPokeModel->SetPosition( pos );
        }
      }

      // 回転設定
      void SetRotate( f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ )
      {
        if( m_pPokeModel != NULL )
        {
          m_pPokeModel->SetRotation(rotationRadX, rotationRadY, rotationRadZ);
        }
      }

      // スケール設定
      void SetScale( f32 scale )
      {
        if( m_pPokeModel != NULL )
        {
          m_pPokeModel->SetScale(scale,scale,scale);
        }
      }

      //モーションのAABB取得
      bool GetMotionCollision( gfl2::math::AABB *box )
      {
        if( m_pPokeModel != NULL )
        {
          return m_pPokeModel->GetMotionCollision(box);
        }
        return false;
      }

      //設定データの取得
      const PokeTool::PokeSettingData * GetSettingData(void) const 
      {
        return GetPokeModel()->GetSettingData();
      }

      void SetSortPriority( u8 value )
      {
        GetPokeModel()->GetModelInstanceNode()->SetSortPriority( value );
      }

      void SetStencilReferenceOffset(u8 ofs)
      {
        GetPokeModel()->SetStencilReferenceOffset(ofs);
      }

      //  表示有効取得  ※これがtrueを返しても生成できていないときは表示されていないので注意！
      bool IsVisible( void ) const { return m_bVisible; }

      //  表示有効設定
      virtual void SetVisible( const bool bFlg );

      //  アニメ再生
      void SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop, const bool bForce = false );

      //------------------------------------------------------------------
      /**
        * @brief   アニメが終了しているか
        * @note
        *         アニメがループ、IsReady() == false、アニメ再生していないときは true を返す
        */
      //------------------------------------------------------------------
      bool IsEndAnime();

      //------------------------------------------------------------------
      /**
        * @brief   引数設定したアニメidが有効か
        * @note
        *         IsReady() == false の場合は強制的に false を返す（モデルができていないから）
        */
      //------------------------------------------------------------------
      bool IsAvailableAnimation( const PokeTool::MODEL_ANIME animeId );

      //------------------------------------------------------------------
      /**
        * @brief   引数設定したアニメidがランダム再生時に再生して良いモーションか？(待機で始まり、待機で戻るか？)
        * @note
        *         IsReady() == false の場合は強制的に false を返す（モデルができていないから）
        */
      //------------------------------------------------------------------
      bool CheckCanOneshotMotion( const PokeTool::MODEL_ANIME animeId );

      //  アニメ一時停止
      void PauseAnime( const bool bPause );
      bool IsPauseAnime( void ) const { return m_bAnimePause; }

      //  使える準備ができているかどうか
      bool IsReady() const
      {
        return  ( STATE_TYPE_UPDATE == m_state );
      }

      /**
      * @brief 更新
      */
      virtual void Update();


      /**
      * @brief メガゲンガーシェーダーに指定する位置を更新する
      * @note  モデル生成/破棄中に呼んでも問題ありません。
      *        メガゲンガーでないときに呼んでも問題ありません。
      */
      void UpdateMegaGangarEffectState(void);


    protected:
      /**
      * @brief  現在のステート
      * @note   派生クラスだけアクセスできる
      */
      const StateType GetState() const { return m_state; }

      /**
      * @brief  ポケモデル取得
      * @note   派生クラスだけアクセスできる
      */
      PokeTool::PokeModel* GetPokeModel(void) const { return m_pPokeModel; }
      /**
      * @brief  初期化パラメータ取得
      * @note   派生クラスだけアクセスできる
      */
      INIT_PARAM* GetInitParam(void) { return &m_initParam; }

    private:
      //  変数のクリア
      void _Clear()
      {
        m_pPokeModel  = NULL;
        m_state       = STATE_TYPE_IDLE;
        m_subStep     = 0;
        m_loadMonsNo  = MONSNO_NULL;
        m_bTerminate  = false;
        m_animeId     = PokeTool::MODEL_ANIME_MAX;
        m_bAnimeLoop  = false;
        m_bAnimePause = false;
        m_bAnimeForce = false;
        m_bVisible    = true;
      }

      void _SetState( const StateType newStateType );

      void _SetAnime( const PokeTool::MODEL_ANIME animeId, const bool bLoop, const bool bForce );
      void _PauseAnime( const bool bPause );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;

      u8          m_subStep;

      MonsNo      m_loadMonsNo;
      bool        m_bTerminate;
      bool        m_bVisible;

      PokeTool::MODEL_ANIME       m_animeId;
      bool                        m_bAnimeLoop;
      bool                        m_bAnimePause;
      bool                        m_bAnimeForce;

      gfl2::math::Vector3         m_pos;
      PokeTool::PokeModel*        m_pPokeModel;
  };

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_APP_TOOL_POKE_SIMPLE_MODEL_H_INCLUDED 
