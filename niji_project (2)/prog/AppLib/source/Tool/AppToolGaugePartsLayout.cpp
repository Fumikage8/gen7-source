//=============================================================================
/**
 * @file AppToolGaugePartsLayout.h
 * @brief パーツレイアウトゲージの制御
 * @author yuto_uchida
 * @date 2015.03.27
 */
//=============================================================================
#include "AppLib/include/Tool/AppToolGaugePartsLayout.h"
#include "AppLib/include/Tool/app_tool_Gauge.h"
#include "AppLib/include/Util/app_util_2d.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  /**
    * @brief ゲージパーツレイアウトの共通制御クラス
   */
   class AppToolGaugePartsLayoutProc
   {
   public:
     // ステート
     enum StateType
     {
       STATE_TYPE_NONE         = 0,
       STATE_TYPE_ANIM_IN,
       STATE_TYPE_IDLE,
     };

     AppToolGaugePartsLayoutProc()
     {
       m_nowNumber    = 0;
       m_maxNumber    = 0;
       m_nextNumber   = 0;
       m_startNumber  = 0;

       m_nowFrame     = 0;
       m_startFrame   = 0;
       m_endFrame     = 0;
       m_maxFrame     = 0;
       m_gaugeSpeed   = 0;
       m_bEndAnime    = false;
       m_state        = STATE_TYPE_NONE;
     }

     void Update( void )
     {
       if( m_state == STATE_TYPE_ANIM_IN )
       {
         if( m_bEndAnime == false )
         {
           bool  bEnd  = false;

           m_nowFrame += m_gaugeSpeed;
           if( m_gaugeSpeed < 0 )
           {
             if( m_nowFrame <= 0 )
             {
               m_nowNumber  = 0;
               m_nowFrame   = 0;

               bEnd  = true;
             }
             else if( m_nowFrame < m_endFrame )
             {
               m_nowFrame   = m_endFrame;
               m_nowNumber  = m_nextNumber;

               bEnd  = true;
             }
             else
             {

             }
           }
           else if( 0 < m_gaugeSpeed )
           {
             if( m_maxFrame <= m_nowFrame )
             {
               m_nowFrame   = m_maxFrame;
               m_nowNumber  = m_maxNumber;

               bEnd  = true;
             }
             else if( m_endFrame < m_nowFrame )
             {
               m_nowFrame = m_endFrame;
               m_nowNumber  = m_nextNumber;
               bEnd  = true;
             }
             else
             {
             }
           }
           else
           {
              // @fix NMCat[224] 現在のフレームと目的フレームが同じだとフレーム加算値が０になるので即終了させる
              m_nowNumber  = m_nextNumber;

              bEnd  = true;
           }

           // フレーム値から数字を計算する
           if( bEnd == false )
           {
              // @fix NMCat[226] 数値計算にミスがあった
              f32  rate  = gfl2::math::Abs( m_nowFrame - m_startFrame ) / gfl2::math::Abs( m_endFrame - m_startFrame );
              m_nowNumber  = (f32)m_startNumber + ( (f32)( m_nextNumber - m_startNumber ) * rate );
           }

           m_bEndAnime  = bEnd;
         }
       }
     }

    //-----------------------------------------------------------------------------------------
    /**
      * @brief  ゲージアニメを開始する
      * @param  nextNumber      : 数の最終値（この数になるまでゲージアニメが進行する）
      * @param  nowNumber       : 数基準となる現在値を設定
      * @param  maxNumber       : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
      * @param  playAnimeFrame  : アニメを再生してから終了するまでのフレーム数を指定
      */
    //-----------------------------------------------------------------------------------------
    bool StartAnime( const s32 nextNumber, const s32 nowNumber, const s32 maxNumber, const s32 maxGaugeFrame, const s32 playAnimeFrame )
    {
      if( nowNumber != nextNumber )
      {
        m_startNumber = m_nowNumber   = nowNumber;
        m_nextNumber  = nextNumber;
        m_maxNumber   = maxNumber;

        //  アニメのフレームでゲージの長さを設定
        m_startFrame  = m_nowFrame  = ::app::tool::Gauge::GetDottoNumF32( nowNumber , maxNumber , maxGaugeFrame );

        m_maxFrame  = maxGaugeFrame;
        m_endFrame  = ::app::tool::Gauge::GetDottoNumF32( nextNumber , maxNumber , maxGaugeFrame );

        m_gaugeSpeed  = 0;

        // @fix NMCat[224] 現在のフレームと目的フレームが同じケースがある
        if( m_nowFrame == m_endFrame )
        {

        }
        else
        {
          //  ゲージを進める(再生するフレーム範囲値から１フレームで加算するフレーム数を計算)
          m_gaugeSpeed  = gfl2::math::Abs( m_nowFrame - m_endFrame ) / (f32)playAnimeFrame;
          if( m_nowFrame < m_endFrame )
          {
          }
          //  ゲージが戻る
          else if( m_endFrame < m_nowFrame )
          {
            m_gaugeSpeed  = -m_gaugeSpeed;
          }
        }

        return  true;
      }
      else
      {
        //  同じ値なのでゲージに変化なし
      }

      return  false;
    }

    StateType GetState() const { return m_state; }

    void SetState( const StateType newStateType )
    {
      m_bEndAnime = false;
      m_state     = newStateType;
    }

    f32         m_nowNumber;
    s32         m_nextNumber;
    s32         m_maxNumber;
    bool        m_bEndAnime;

   private:
     StateType   m_state;

     s32         m_startNumber;
     f32         m_gaugeSpeed;

     f32         m_nowFrame;
     f32         m_maxFrame;
     f32         m_endFrame;
     f32         m_startFrame;
   };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  AppToolHPGaugePartsLayout::AppToolHPGaugePartsLayout( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( param.p_g2d_util );
    GFL_ASSERT( param.p_left_number_text_box_pane );
    GFL_ASSERT( param.p_right_number_text_box_pane );
    GFL_ASSERT( param.p_center_number_text_box_pane );
    GFL_ASSERT( pHeap );

    m_pCoreProc = GFL_NEW( pHeap ) AppToolGaugePartsLayoutProc();
    GFL_ASSERT( m_pCoreProc );

    m_initParam = param;
    m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx, false, false );
    // @fix GFNMCat[1731] アニメ最大を固定していたが、アニメデータから最大数を取得するように変更
    m_gaugeFrameMax = (s32)m_initParam.p_g2d_util->GetAnimeMaxFrame( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx );

    m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  AppToolHPGaugePartsLayout::~AppToolHPGaugePartsLayout()
  {
    GFL_SAFE_DELETE( m_pCoreProc );

    m_initParam.p_g2d_util->StopAnime( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメを開始する
    * @param  nextNumber      : 数の最終値（この数になるまでゲージアニメが進行する）
    * @param  nowNumber       : 数基準となる現在値を設定
    * @param  maxNumber       : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    * @param  playAnimeFrame  : アニメを再生してから終了するまでのフレーム数を指定
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::StartAnime( const s32 nextNumber, const s32 nowNumber, const s32 maxNumber, const s32 playAnimeFrame )
  {
    if( m_pCoreProc->StartAnime( nextNumber, nowNumber, maxNumber, m_gaugeFrameMax, playAnimeFrame ) )
    {
      //  初期ゲージを設定
      _SetNumber( m_pCoreProc->m_nowNumber, m_pCoreProc->m_maxNumber );

      m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN );
    }
    else
    {
      //  同じ値なのでゲージに変化なし
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメスキップ
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::SkipAnime()
  {
    if( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN )
    {
      SetNumber( m_pCoreProc->m_nextNumber, m_pCoreProc->m_maxNumber );
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメ実行中か
    */
  //-----------------------------------------------------------------------------------------
  bool AppToolHPGaugePartsLayout::IsAnime() const
  {
    return ( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージの値を設定（アニメせずにいきなり反映）
    * @param  nowNumber   : 数基準となる現在値を設定
    * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::SetNumber( const s32 nowNumber, const s32 maxNumber )
  {
    _SetNumber( (f32)nowNumber, maxNumber );
    m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  更新処理
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::Update( void )
  {
    m_pCoreProc->Update();
    if( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN )
    {
      _SetNumber( m_pCoreProc->m_nowNumber, m_pCoreProc->m_maxNumber );

      if( m_pCoreProc->m_bEndAnime )
      {
        m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
      }
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  テキスト表示設定
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::SetTextVisible( const bool bFlg )
  {
    m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_center_number_text_box_pane, bFlg );
    m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_left_number_text_box_pane, bFlg );
    m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_right_number_text_box_pane, bFlg );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージの値を設定（アニメせずにいきなり反映）
    * @param  nowNumber   : 数基準となる現在値を設定
    * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayout::_SetNumber( const f32 nowNumber, const s32 maxNumber )
  {
    GFL_ASSERT( nowNumber <= maxNumber );

    //  ゲージの長さを指定
    {
      //  アニメのフレームでゲージの長さを設定
      f32 frame = ::app::tool::Gauge::GetDottoNumF32( nowNumber , maxNumber , m_gaugeFrameMax );

      //  最大値だとFrameMaxになるが、アニメでは0フレームがゲージ最大、Maxフレームがゲージ最小になっているので値を反転させる
      //  @fix GFNMCat[2305] 最大ＨＰが少ない時に整数値だとアニメゲージがカクつくので小数対応
      m_initParam.p_g2d_util->SetAnimeFrame( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx, (f32)m_gaugeFrameMax - frame );
    }

    //  数字指定
    {
      //  左側の数字(大きいサイズ)
      m_initParam.p_g2d_util->SetTextBoxPaneNumber( m_initParam.p_left_number_text_box_pane, m_initParam.number_message_id, nowNumber, m_initParam.number_keta, 0, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );

      //  右側の数字(小さいサイズ)
      m_initParam.p_g2d_util->SetTextBoxPaneNumber( m_initParam.p_right_number_text_box_pane, m_initParam.number_message_id, maxNumber, m_initParam.number_keta, 0, print::NUM_DISP_LEFT, print::NUM_CODE_ZENKAKU );
    }

    //  ゲージの色をアニメで切り替え
    {
      ::app::tool::Gauge::GaugeColor gaugeCol = ::app::tool::Gauge::GetDottoColor( nowNumber , maxNumber );
      switch( gaugeCol )
      {
      case ::app::tool::Gauge::GAUGE_COLOR_GREEN:
        m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, m_initParam.color_normal_anime_idx );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_YELLOW:
        m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, m_initParam.color_half_anime_idx );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_RED:
      case ::app::tool::Gauge::GAUGE_COLOR_NULL:
        m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, m_initParam.color_few_anime_idx );
        break;
      }
    }
  }


  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  AppToolGaugePartsLayout::AppToolGaugePartsLayout( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap )
  {
    GFL_ASSERT( param.p_g2d_util );
    GFL_ASSERT( pHeap );
    GFL_ASSERT( param.p_parts_pane );

    m_pCoreProc = GFL_NEW( pHeap ) AppToolGaugePartsLayoutProc();
    GFL_ASSERT( m_pCoreProc );

    m_initParam = param;
    m_initParam.p_g2d_util->StartAnime( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx, false, false );
    // @fix GFNMCat[1731] アニメ最大を固定していたが、アニメデータから最大数を取得するように変更
    m_gaugeFrameMax = (s32)m_initParam.p_g2d_util->GetAnimeMaxFrame( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx );

    m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  AppToolGaugePartsLayout::~AppToolGaugePartsLayout()
  {
    GFL_SAFE_DELETE( m_pCoreProc );

    m_initParam.p_g2d_util->StopAnime( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメを開始する
    * @param  nextNumber      : 数の最終値（この数になるまでゲージアニメが進行する）
    * @param  nowNumber       : 数基準となる現在値を設定
    * @param  maxNumber       : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    * @param  playAnimeFrame  : アニメを再生してから終了するまでのフレーム数を指定
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::StartAnime( const s32 nextNumber, const s32 nowNumber, const s32 maxNumber, const s32 playAnimeFrame )
  {      
    if( m_pCoreProc->StartAnime( nextNumber, nowNumber, maxNumber, m_gaugeFrameMax, playAnimeFrame ) )
    {
      //  初期ゲージを設定
      _SetNumber( m_pCoreProc->m_nowNumber, m_pCoreProc->m_maxNumber );

      m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN );
    }
    else
    {
      //  同じ値なのでゲージに変化なし
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメスキップ
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::SkipAnime()
  {
    if( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN )
    {
      SetNumber( m_pCoreProc->m_nextNumber, m_pCoreProc->m_maxNumber );
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージアニメ実行中か
    */
  //-----------------------------------------------------------------------------------------
  bool AppToolGaugePartsLayout::IsAnime() const
  {
    return ( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージの値を設定（アニメせずにいきなり反映）
    * @param  nowNumber   : 数基準となる現在値を設定
    * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::SetNumber( const s32 nowNumber, const s32 maxNumber )
  {
    _SetNumber( (f32)nowNumber, maxNumber );
    m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  更新処理
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::Update( void )
  {
    m_pCoreProc->Update();
    if( m_pCoreProc->GetState() == AppToolGaugePartsLayoutProc::STATE_TYPE_ANIM_IN )
    {
      _SetNumber( m_pCoreProc->m_nowNumber, m_pCoreProc->m_maxNumber );

      if( m_pCoreProc->m_bEndAnime )
      {
        m_pCoreProc->SetState( AppToolGaugePartsLayoutProc::STATE_TYPE_IDLE );
      }
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  表示設定
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::SetVisible( const bool bFlg )
  {
    m_initParam.p_g2d_util->SetPaneVisible( m_initParam.p_parts_pane, bFlg );
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージの値を設定（アニメせずにいきなり反映）
    * @param  nowNumber   : 数基準となる現在値を設定
    * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    */
  //-----------------------------------------------------------------------------------------
  void AppToolGaugePartsLayout::_SetNumber( const f32 nowNumber, const s32 maxNumber )
  {
    GFL_ASSERT( nowNumber <= maxNumber );

    if( (f32)maxNumber <= nowNumber )
    {
      //  経験値が最大の場合は０に（レベル１００の場合は経験値が０なので）
      m_initParam.p_g2d_util->SetAnimeFrame( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx, (f32)m_gaugeFrameMax );
    }
    else
    //  ゲージの長さを指定
    {
      //  アニメのフレームでゲージの長さを設定
      //  @fix GFNMCat[2305] 最大ＨＰが少ない時に整数値だとアニメゲージがカクつくので小数対応
      f32 frame = ::app::tool::Gauge::GetDottoNumF32( nowNumber , maxNumber , m_gaugeFrameMax );

      //  最大値だとFrameMaxになるが、アニメでは0フレームがゲージ最大、Maxフレームがゲージ最小になっているので値を反転させる
      m_initParam.p_g2d_util->SetAnimeFrame( m_initParam.layout_id, m_initParam.gauge_hp_anime_idx, (f32)m_gaugeFrameMax - frame );
    }
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  コンストラクタ
    */
  //-----------------------------------------------------------------------------------------
  AppToolHPGaugePartsLayoutGAG_002::AppToolHPGaugePartsLayoutGAG_002( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap ) :
  AppToolGaugePartsLayout( param.baseInitParam, pHeap )
  {
    m_initParam = param;
  }

  //-----------------------------------------------------------------------------------------
  /**
    * @brief  ゲージの値を設定（アニメせずにいきなり反映）
    * @param  nowNumber   : 数基準となる現在値を設定
    * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
    */
  //-----------------------------------------------------------------------------------------
  void AppToolHPGaugePartsLayoutGAG_002::_SetNumber( const f32 nowNumber, const s32 maxNumber )
  {
    AppToolGaugePartsLayout::_SetNumber( nowNumber, maxNumber );

    //  ゲージの色をアニメで切り替え
    {
      ::app::tool::Gauge::GaugeColor gaugeCol = ::app::tool::Gauge::GetDottoColor( nowNumber , maxNumber );
      switch( gaugeCol )
      {
      case ::app::tool::Gauge::GAUGE_COLOR_GREEN:
        m_initParam.baseInitParam.p_g2d_util->StartAnime( m_initParam.baseInitParam.layout_id, m_initParam.color_normal_anime_idx );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_YELLOW:
        m_initParam.baseInitParam.p_g2d_util->StartAnime( m_initParam.baseInitParam.layout_id, m_initParam.color_half_anime_idx );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_RED:
      case ::app::tool::Gauge::GAUGE_COLOR_NULL:
        m_initParam.baseInitParam.p_g2d_util->StartAnime( m_initParam.baseInitParam.layout_id, m_initParam.color_few_anime_idx );
        break;
      }
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
