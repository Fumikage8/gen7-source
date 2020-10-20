//=============================================================================
/**
 * @file TitleLowerUIDraw.h
 * @brief タイトルの下UI画面
 * @author yuto_uchida
 * @date 2015.06.02
 */
//=============================================================================
#if !defined( NIJI_PROJECT_TITLE_LOWER_UI_DRAW_H_INCLUDED )
#define NIJI_PROJECT_TITLE_LOWER_UI_DRAW_H_INCLUDED
#pragma once

#include "AppLib/include/ui/UIView.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( title )

  /** タイトル下UI画面 */
  class TitleLowerUIDraw : public ::app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN( TitleLowerUIDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_IDLE,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        ::app::util::Heap*                pUtilHeap;
        ::app::util::AppRenderingManager* pRenderingManager;

        u32 lytId;
        u32 lytAnimeMax;
        u32 animeInId;
        u32 animeKeepId;

        //  レイアウトバイナリ(あらかじめ使う側がレイアウトデータを作成して渡す。)
        void* pLytBuff;

        _tag_init_param()
        {
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pLytBuff              = NULL;

          lytId                 = 0;
          lytAnimeMax           = 0;
          animeInId             = 0;
          animeKeepId           = 0;
        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
      * @brief コンストラクタ
      */
      TitleLowerUIDraw( const INIT_PARAM& param );

      /**
      * @brief デストラクタ
      */
      virtual ~TitleLowerUIDraw();

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

      /**
        * @brief 開始
        */
      void Start( void ) { _SetState( STATE_TYPE_IN ); }

      /**
        * @brief 終了
        */
      void End( void ) { _SetState( STATE_TYPE_OUT ); }

      /**
        * @brief 更新処理
        */
      virtual void Update( void );

      /**
      * @brief 描画
      */
      virtual void Draw(  gfl2::gfx::CtrDisplayNo displayNo );

      /**
      * @brief  現在のステート
      */
      const StateType GetState() const { return m_state; }

    private:
      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      StateType   m_state;
  };

GFL_NAMESPACE_END( title )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_TITLE_LOWER_UI_DRAW_H_INCLUDED 
