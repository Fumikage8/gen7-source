//======================================================================
/**
 * @file BattleUIIntroEventLowerDraw.cpp
 * @date 2015/08/24 18:06:45
 * @author uchida_yuto
 * @brief バトル下画面の導入演出Draw
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "BattleUIIntroEventLowerDraw.h"
#include "BattleUIViewDrawOrder.h"

#include "niji_conv_header/app/battle/res2d/intlow.h"
#include "niji_conv_header/app/battle/res2d/intlow_pane.h"
#include "niji_conv_header/app/battle/res2d/intlow_anim_list.h"

// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>
#include "niji_conv_header/app/battle/BtlLowerDynamic.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

  const struct BattleUIIntroEventLowerDraw::CIRCLE_DATA BattleUIIntroEventLowerDraw::ms_aCircleData[ BattleUIIntroEventLowerDraw::CIRCLE_PANE_NUM ] =
  {
    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_00,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_00_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_01,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_01_ANIME_05
      },
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_02,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_02_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_03,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_03_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_04,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_04_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_05,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_05_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_06,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_06_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_07,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_07_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_08,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_08_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_09,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_09_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_10,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_10_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_11,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_11_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_12,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_12_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_13,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_13_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_14,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_14_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_15,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_15_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_16,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_16_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_17,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_17_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_18,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_18_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_19,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_19_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_20,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_20_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_21,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_21_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_22,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_22_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_23,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_23_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_24,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_24_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_25,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_25_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_26,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_26_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_27,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_27_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_28,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_28_ANIME_05
      }
    },

    {
      PANENAME_BATTLE_INT_LOW_000_PANE_MASK_29,
      {
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_00,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_01,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_02,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_03,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_04,
        LA_INTLOW_BATTLE_INT_LOW_000__MASK_29_ANIME_05
      }
    }
  };

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIIntroEventLowerDraw::BattleUIIntroEventLowerDraw( const INIT_PARAM& param ) :
    UIView( param.pUtilHeap, param.pUtilHeap->GetDeviceAllocator(), param.pUtilHeap->GetDeviceAllocator() )
  {
    GFL_ASSERT( param.pUtilHeap );
    GFL_ASSERT( param.pRenderingManager );
    GFL_ASSERT( param.pLytBuff );

    //  初期化
    _Clear();

    m_initParam = param;

    _Initialize2D( m_initParam.pLytBuff );

    UIView::SetVisible( false );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  BattleUIIntroEventLowerDraw::~BattleUIIntroEventLowerDraw()
  {
    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void BattleUIIntroEventLowerDraw::Update( void )
  {
    _UpdateState();

    UIView::Update();
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 描画
    */
  //-------------------------------------------------------------------------
  void BattleUIIntroEventLowerDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    for( s32 i = 0; i < LYT_WK_TYPE_MAX; ++i )
    {
      pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderingManager, displayNo, i, LOW_VIEW_ORDER_OTHER );
    }
  }

  /**
    * @brief レイアウトワークを生成する
    */
  void BattleUIIntroEventLowerDraw::_Initialize2D( void* pAppLytBuff )
  {
    //  2D画面作成
    {
      // アーカイブIDテーブル
      const ::app::util::G2DUtil::LytResData resTbl[] =
      {
        {  pAppLytBuff, 32, ::app::util::G2DUtil::ATTACH_ROOT },
      };

      //  作成するレイアウトワーク
      const ::app::util::G2DUtil::LytwkData setupData[ LYT_WK_TYPE_MAX ] =
      {
          //  選択画面
          {
            0,
            LYTRES_INTLOW_BATTLE_INT_LOW_000_BFLYT,
            LA_INTLOW_BATTLE_INT_LOW_000___NUM,
            pAppLytBuff,
            true,
            ::app::util::G2DUtil::SETUP_LOWER,
            true
          },
      };

      UIView::Create2D( m_initParam.pUtilHeap, NULL, 32, resTbl, GFL_NELEMS(resTbl), setupData, GFL_NELEMS(setupData), UIView::UNUSE_MESSAGE_DATA );
      UIView::SetVisible( false );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIIntroEventLowerDraw::_UpdateState()
  {
    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_RUN )
    {
      ::app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      u32 maskCircleNum = m_maskCircleNum;

      if( m_subSeq == 0 )
      {
        for( u32 i = 0; i < maskCircleNum; ++i )
        {
          pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, m_aCircleExeData[ i ].animeId );
        }

        pG2DUtil->StartAnime( LYT_WK_TYPE_SCREEN, LA_INTLOW_BATTLE_INT_LOW_000_INVISIBLE );
        ++m_subSeq;
      }

      if( m_subSeq == 1 )
      {
        //  アニメが終了しているか
        u32 animeEndCnt = 0;

        for( u32 i = 0; i < maskCircleNum; ++i )
        {
          if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, m_aCircleExeData[ i ].animeId ) == true )
          {
            //  終了したらカウントを増やす
            ++animeEndCnt;
          }
        }

        if( maskCircleNum <= animeEndCnt )
        {
          ++m_subSeq;
        }
      }

      if( m_subSeq == 2 )
      {
        //  アニメがすべて終わったので終了
        if( pG2DUtil->IsAnimeEnd( LYT_WK_TYPE_SCREEN, LA_INTLOW_BATTLE_INT_LOW_000_INVISIBLE ) == true )
        {
          _SetState( STATE_TYPE_END );
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void BattleUIIntroEventLowerDraw::_SetState( const StateType newStateType )
  {
    GFL_ASSERT( m_state != newStateType );

    m_state   = newStateType;
    m_subSeq  = 0;

    if( m_state == STATE_TYPE_NONE )
    {
    }
    else
    if( m_state == STATE_TYPE_RUN )
    {
      //  各円ペイン配置とアニメ再生
      //  必ず演出は固定のフレームで終わらせる
      {
        u32 maskMax = GFL_VALUE_DEFINE_IN_HEADER( BTL_LOWER_DYNAMIC_INTRO_MASK_MAX );
        if( maskMax <= 0 )
        {
          GFL_ASSERT( 0 );
          _SetState( STATE_TYPE_END );
        }
        else
        if( BattleUIIntroEventLowerDraw::CIRCLE_PANE_NUM < maskMax )
        {
          GFL_ASSERT( 0 );
          _SetState( STATE_TYPE_END );
        }
        else
        {
          gfl2::lyt::LytWk* pLayoutWork   = UIView::GetLayoutWork( LYT_WK_TYPE_SCREEN );

          gfl2::math::Random rand;

          rand.Initialize();
          for( u32 i = 0; i < maskMax; ++i )
          {
            //  円が広がる実行データ設定
            m_aCircleExeData[ i ].animeId   = ms_aCircleData[ i ].aAnimeId[ rand.Next( CIRCLE_ANIME_ID_NUM ) ];

            //  円の配置をランダム(下画面範囲内に)
            {
              gfl2::lyt::LytPane* pPane = pLayoutWork->GetPane( ms_aCircleData[ i ].paneId );
              GFL_ASSERT( pPane );

              gfl2::math::VEC3  pos = pPane->GetTranslate();

              //  レイアウト座標系に直す
              pos.x = (f32)( rand.Next( DISPLAY_LOWER_WIDTH ) ) - (f32)( DISPLAY_LOWER_WIDTH >> 1 );
              pos.y = (f32)( rand.Next( DISPLAY_LOWER_HEIGHT ) ) - (f32)( DISPLAY_LOWER_HEIGHT >> 1 );

              pPane->SetTranslate( pos );
            }
          }

          m_maskCircleNum = maskMax;

          UIView::SetVisible( true );
        }
      }
    }
    else
    if( m_state == STATE_TYPE_END )
    {
      UIView::SetVisible( false );
    }
  }

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )
