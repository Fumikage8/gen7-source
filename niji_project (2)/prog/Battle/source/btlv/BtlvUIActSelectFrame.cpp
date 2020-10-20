//=============================================================================
/**
 * @file	 BtlvUIActSelectFrame.h
 * @brief	 バトル下画面の行動選択画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.03.10
 */
//=============================================================================
#pragma once

#include "BtlvUIActSelectFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"
#include "niji_conv_header/message/msg_btl_app.h"

//  nijiのインクルード
#include <system/include/HeapDefine.h>
#include <Sound/include/Sound.h>

#include "./app/BattleUIDataManager.h"

GFL_NAMESPACE_BEGIN( btl )

  //  arcID
  const s32 BattleUIActSelectFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

GFL_NAMESPACE_BEGIN( app )

  /**
  * @brief バトル下画面の行動選択画面の入力制御クラス
  */
  class BattleUIActSelectLowerInputNormal : public app::BattleUIActSelectLowerInputInterface
  {
  public:
      BattleUIActSelectLowerInputNormal( app::BattleUIActSelectLowerDraw* pDrawCore ) :
      BattleUIActSelectLowerInputInterface( pDrawCore ) {}

      //------------------------------------------------------------------
      /**
        * @brief   カーソル移動
        */
      //------------------------------------------------------------------
      virtual bool MoveUpCursor()
      {
        s32 currentCursorIdx  = m_pDrawCore->GetCurrentCursorIdx();

        bool  bRet  = false;

        //  たたかうとポケモンにカーソルがあるときは何もしない
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT )
        {
          return  bRet;
        }
        else
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE )
        {
          return  bRet;
        }

        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG )
        {
          //  バッグボタンならポケモンボタンへ
          bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE );
        }
        else
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN )
        {
          //  前の選択にもどる
          bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, m_pDrawCore->GetOldCursorIdx() );
        }
        else
        {
          GFL_ASSERT( 0 );
          return  false;
        }

        return  bRet;
      }

      virtual bool MoveDownCursor()
      {
        s32 currentCursorIdx  = m_pDrawCore->GetCurrentCursorIdx();

        bool  bRet  = false;

        //  にげる項目にカーソルがあるときは何もしない
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN )
        {
          return  bRet;
        }
        
        //  現在ポケモン項目に移動しているなら、バッグに移動
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE )
        {
          bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG );
          if( bRet == false )
          {
            //  バッグボタンがなければにげるボタンへ
            //  にげるボタンは必ずあるのでにげるボタンのチェックは不要
            bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN );
          }
        }
        else
        {
          //  たたかう・バッグボタンならにげるボタンへ
          bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN );
        }

        return  bRet;
      }

      virtual bool MoveLeftCursor()
      {
        s32 currentCursorIdx  = m_pDrawCore->GetCurrentCursorIdx();

        bool  bRet  = false;

        //  バッグ、ポケモン項目にカーソルがあるときは何もしない
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE )
        {
          return  bRet;
        }
        else
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG )
        {
          return  bRet;
        }

        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT )
        {
          u32 oldCursor = m_pDrawCore->GetOldCursorIdx();
          if( oldCursor == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG )
          {
            bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, oldCursor );
          }
          else
          if( oldCursor == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE )
          {
            bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, oldCursor );
          }
          else
          {
            bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE );
          }
        }
        else
        if( currentCursorIdx == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN )
        {
          bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG );
          if( bRet == false )
          {
            //  バッグボタンがなければポケモンボタンへ
            //  ポケモンボタンは絶対にあるのこのボタンのチェックは不要
            bRet  = m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE );
          }
        }
        else
        {
          GFL_ASSERT( 0 );
          return  false;
        }

        return  bRet;
      }

      virtual bool MoveRightCursor()
      {
        //  たたかう項目にカーソルがあるときは何もしない
        if( m_pDrawCore->GetCurrentCursorIdx() == BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT )
        {
          return  false;
        }

        return ( m_pDrawCore->SetActiveSelectItemCursor( true, BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT ) );
      }

      //------------------------------------------------------------------
      /**
        * @brief   Yボタンを押す
        */
      //------------------------------------------------------------------
      virtual void PushBtnY()
      {
        //  描画状態で押すのを変える
        const BattleUIActSelectLowerDraw::DrawMode  drawMode  = m_pDrawCore->GetDrawMode();
        if( drawMode == BattleUIActSelectLowerDraw::DRAW_MODE_NORMAL )
        {
          ::app::tool::ButtonManager* pBtnManager  = m_pDrawCore->GetButtonManager();
          GFL_ASSERT( pBtnManager );

          //  モンスターボールを投げる
          if( pBtnManager->IsButtonVisible( BattleUIActSelectLowerDraw::BUTTON_ID_BALL ) )
          {
            //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
            pBtnManager->StartSelectedAct( BattleUIActSelectLowerDraw::BUTTON_ID_BALL );
          }
        }
        else
        if( drawMode == BattleUIActSelectLowerDraw::DRAW_MODE_NET )
        {
          ::app::tool::ButtonManager* pBtnManager  = m_pDrawCore->GetButtonManager();
          GFL_ASSERT( pBtnManager );

          const BattleUIActSelectLowerDraw::ButtonId  aChkBtnDataType[] = 
          {
            BattleUIActSelectLowerDraw::BUTTON_ID_USER_NAME,
          };

          for( s32 i = 0; i < GFL_NELEMS( aChkBtnDataType ); ++i )
          {
            if( pBtnManager->IsButtonVisible( aChkBtnDataType[ i ] ) )
            {
              //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
              pBtnManager->StartSelectedAct( aChkBtnDataType[ i ] );
              break;
            }
          }

          // @fix GFNMCat[1364] 通信でマルチの時は相手のパーティー表示ボタンがないので必ず bPushBtn == true にはならない
          //GFL_ASSERT( bPushBtn == true );
        }
        else
        if( drawMode == BattleUIActSelectLowerDraw::DRAW_MODE_BTL_FACLIITY )
        {
          //  ボールを投げることはできない
        }
        else
        {
          GFL_ASSERT_STOP( 0 );
        }
      }

      //------------------------------------------------------------------
      /**
        * @brief   カーソル選択している項目を押す
        */
      //------------------------------------------------------------------
      virtual void PushBtnCurrentCursorItem()
      {
        m_pDrawCore->PushBtnCurrentCursorItem();
      }

      //------------------------------------------------------------------
      /**
        * @brief   指定したカーソルを選択して項目を押す
        */
      //------------------------------------------------------------------
      virtual void PushBtnSelectCursorItem( const s32 newCursorType )
      {
        m_pDrawCore->PushBtnSelectCursorItem( (BattleUIActSelectLowerDraw::CursorType)newCursorType );
      }

       //------------------------------------------------------------------
       /**
        * @brief   もどるボタンを押した
        */
      //------------------------------------------------------------------
      virtual void PushBtnBack()
      {
        if( m_pDrawCore->GetButtonManager()->IsButtonVisible( BattleUIActSelectLowerDraw::BUTTON_ID_BACK ) )
        {
          //  コールバックの OnLayoutPaneEvent 関数が呼ばれる
          m_pDrawCore->GetButtonManager()->StartSelectedAct( BattleUIActSelectLowerDraw::BUTTON_ID_BACK );
        }
      }
  };

  GFL_NAMESPACE_END( app )

  /** シングル用のコアクラス( 1 vs 1 の構図になる ) */
  class BattleUIActSelectFrameSingleCore : public BattleUIActSelectFrame::CoreImp
  {
  public:
    BattleUIActSelectFrameSingleCore()
    : CoreImp() {}

    virtual void Initialize( BattleUIActSelectFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
    {
      CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

      app::BattleUIDataManager* pUIDataManager  = m_pAppParam->in_param.pBtlParamAccessror;
      GFL_ASSERT( pUIDataManager );

      app::BattleUIActSelectLowerDraw::INIT_PARAM     initParam;
      {
        //  初期化パラメータ作成
        CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

        //  専用パラメータ設定
        {
          //  画面の状態を設定
          initParam.screenType = app::BattleUIActSelectLowerDraw::SCREEN_TYPE_NORMAL;

          //  味方の配置
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
            //  一人しかない
            GFL_ASSERT_STOP( posList.Size() == 1 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            //  リストがひとつしかないのでループするまでもないが、リスト処理の形式として
            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              GFL_ASSERT_STOP( posType == BTL_VPOS_NEAR_1 );
              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }

          //  敵の配置
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetEnemyPokePosPlaceList();
            //  敵が一人しかない
            GFL_ASSERT_STOP( posList.Size() == 1 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            //  リストがひとつしかないのでループするまでもないが、リスト処理の形式として
            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              GFL_ASSERT_STOP( posType == BTL_VPOS_FAR_1 );
              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }
        }

        //  描画クラス生成
        m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        //  画面入力クラス生成
        m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerInputNormal( m_pDraw );
        GFL_ASSERT( m_pInput );

        m_pDraw->Initialize();

        m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_RUN, false );
      }
    }
  };

  /** ダブル用のコアクラス( 2 vs 2 の構図になる ) */
  class BattleUIActSelectFrameDoubleCore : public BattleUIActSelectFrame::CoreImp
  {
  public:
    BattleUIActSelectFrameDoubleCore()
      : CoreImp()
    {
      m_bFirstUpdateState = true;
      m_bActReturn        = false;
    }

    virtual void Initialize( BattleUIActSelectFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
    {
      CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

      app::BattleUIDataManager* pUIDataManager  = m_pAppParam->in_param.pBtlParamAccessror;
      GFL_ASSERT( pUIDataManager );

      app::BattleUIActSelectLowerDraw::INIT_PARAM     initParam;
      {
        //  初期化パラメータ作成
        CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

        //  専用パラメータ設定
        {
          //  画面の状態を設定
          initParam.screenType = app::BattleUIActSelectLowerDraw::SCREEN_TYPE_DOUBLE;

          //  味方
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
            //  二人しかない
            GFL_ASSERT_STOP( posList.Size() == 2 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              if( posType == BTL_VPOS_NEAR_1 )
              {
              //  左下
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              }
              else if( posType == BTL_VPOS_NEAR_2 )
              {
              //  右下
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }

          //  敵
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetEnemyPokePosPlaceList();
            //  敵は必ず二人
            GFL_ASSERT_STOP( posList.Size() == 2 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;

              if( posType == BTL_VPOS_FAR_1 )
              {
              //  右上
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;
              }
              else if( posType == BTL_VPOS_FAR_2 )
              {
              //  左上
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }
        }

        //  描画クラス生成
        {
          m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerDraw( initParam );
          GFL_ASSERT( m_pDraw );

          m_pDraw->Initialize();
        }

        //  画面入力クラス生成
        {
          m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerInputNormal( m_pDraw );
          GFL_ASSERT( m_pInput );
        }

        m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_RUN, false );
      }
    }

    /// キー入力通知用リスナ(入力制御はクラス側にまかせているので、処理を変更する時は参照するクラスを差し替えるだけでOK)
    virtual bool OnKeyAction( app::BattleUIActSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
    {
      if( BattleUIActSelectFrame::CoreImp::OnKeyAction( pDrawInst, pButton, pKey, pStick ) == false )
      {
        if( m_pDraw->GetCoomandMode() == app::BattleUIActSelectLowerDraw::COMMAND_MODE_BACK )
        {
          if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
          {
            m_pInput->PushBtnBack();
            return  true;
          }
        }
      }

      return  false;
    }

    virtual void SetState( const StateCoreType newState )
    {
      CoreImp::SetState( newState );

      if( newState == STATE_CORE_TYPE_UPDATE )
      {
        //  初回はポケモンアイコンの変化はしない
        if( m_bFirstUpdateState )
        {
          m_bFirstUpdateState = false;
        }
      }


      /**
        @fix  MMCat[531]   通信対戦のマルチバトルの戦闘画面にて、下画面味方立ち位置Cのポケモンが操作中にパッシブ状態（半透明）で表示されている
          m_targetPokeIndex だけでは通信マルチで右側に立った場合が考慮できていなかった
          ここでの分岐パターンは次の通り
            1) pokeListType == MEMBER_SINGLE_PARTY,   m_targetPokeIndex = (0,1) : 通常ダブルバトル（一人2体）、0:左 1:右
            2) pokeListType == MEMBER_MULTI_PARTY_L,  m_targetPokeIndex = (0)   : 通信マルチバトル（二人1体ずつ）、左
            3) pokeListType == MEMBER_MULTI_PARTY_R,  m_targetPokeIndex = (0)   : 通信マルチバトル（二人1体ずつ）、右
      */
      /*  pokeiconのalpha  */
      {
        const u8                                      alpha_Active    = 0xff;
        const u32                                     alpha_Inactive  = 0x80;
        const app::BattleUIDataManager::PokeListType  pokeListType    = m_pAppParam->in_param.pBtlParamAccessror->GetPokeListType();
        const bool                                    bTargetLeftPoke = (
                                                                          ((pokeListType == app::BattleUIDataManager::MEMBER_SINGLE_PARTY) && (!m_targetPokeIndex))   /*  ダブルバトル1体目  */
                                                                          ||  (pokeListType == app::BattleUIDataManager::MEMBER_MULTI_PARTY_L)                        /*  マルチバトル左     */
                                                                        );
        
        m_pDraw->GetPokeObj( app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN )->SetAlpha(   bTargetLeftPoke     ? alpha_Active : alpha_Inactive );
        m_pDraw->GetPokeObj( app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN )->SetAlpha(  (!bTargetLeftPoke)  ? alpha_Active : alpha_Inactive );
      }


      //  行動状態が変わった時のみアイコン表示内容を変える
      if( m_bActReturn )
      {
        if( m_pAppParam->in_param.bActReturn == false )
        {
          m_bActReturn  = m_pAppParam->in_param.bActReturn;

#if 0
          //  味方の左のアイコンを暗くなっているのを明るくする
          {
            app::BattleUIActSelectLowerDraw::PokeObj* pPokeObj  = m_pDraw->GetPokeObj( app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN );
            pPokeObj->SetAlpha( 255 );
          }
#endif
          m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_RUN, true );
        }
      }
      else
      {
        if( m_pAppParam->in_param.bActReturn )
        {
          m_bActReturn  = m_pAppParam->in_param.bActReturn;

#if 0
          //  味方の左のアイコンを暗くする
          {
            app::BattleUIActSelectLowerDraw::PokeObj* pPokeObj  = m_pDraw->GetPokeObj( app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN );
            pPokeObj->SetAlpha( 128 );
          }
#endif

          m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_BACK, true );
        }
      }
    }

  private:
    bool  m_bActReturn;
    bool  m_bFirstUpdateState;
  };

  /** 乱入用のコアクラス( 1 vs 2 の構図になる ) */
  class BattleUIActSelectFrameIntrusionCore : public BattleUIActSelectFrame::CoreImp
  {
  public:
    BattleUIActSelectFrameIntrusionCore()
      : CoreImp()
    {
    }

    virtual void Initialize( BattleUIActSelectFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
    {
      CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

      app::BattleUIDataManager* pUIDataManager  = m_pAppParam->in_param.pBtlParamAccessror;
      GFL_ASSERT( pUIDataManager );

      app::BattleUIActSelectLowerDraw::INIT_PARAM     initParam;
      {
        //  初期化パラメータ作成
        CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

        //  専用パラメータ設定
        {
          //  画面の状態を設定
          initParam.screenType = app::BattleUIActSelectLowerDraw::SCREEN_TYPE_INTRUSION;

          //  味方
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
            //  一人しかない
            GFL_ASSERT_STOP( posList.Size() == 1 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              if( posType == BTL_VPOS_NEAR_1 )
              {
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }

          //  敵
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetEnemyPokePosPlaceList();
            //  敵は必ず二人
            GFL_ASSERT_STOP( posList.Size() == 2 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;

              if( posType == BTL_VPOS_FAR_1 )
              {
              //  右上
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;
              }
              else
              if( posType == BTL_VPOS_FAR_2 )
              {
              //  左上
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }
        }

        //  画像描画クラス生成
        m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        //  画面入力クラス生成
        m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerInputNormal( m_pDraw );
        GFL_ASSERT( m_pInput );

        m_pDraw->Initialize();

        m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_RUN, false );
      }
    }
  };

  /** バトルロワイヤル用のコアクラス( 1 vs 3 の構図になる ) */
  class BattleUIActSelectFrameRoyaleCore : public BattleUIActSelectFrame::CoreImp
  {
  public:
    BattleUIActSelectFrameRoyaleCore()
      : CoreImp()
    {
    }

    virtual void Initialize( BattleUIActSelectFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
    {
      CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

      app::BattleUIDataManager* pUIDataManager  = m_pAppParam->in_param.pBtlParamAccessror;
      GFL_ASSERT( pUIDataManager );

      app::BattleUIActSelectLowerDraw::INIT_PARAM     initParam;
      {
        //  初期化パラメータ作成
        CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

        //  専用パラメータ設定
        {
          //  画面の状態を設定
          initParam.screenType = app::BattleUIActSelectLowerDraw::SCREEN_TYPE_ROYALE;

          //  味方
          {
            gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
            //  一人しかない
            GFL_ASSERT_STOP( posList.Size() == 1 );

            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              if( posType == BTL_VPOS_ROYAL_NEAR )
              {
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }

          //  敵
          {
            app::BattleUIActSelectLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

            gfl2::util::List<BtlvPos>& enemyPosList = pUIDataManager->GetEnemyPokePosPlaceList();
            for( gfl2::util::ListIterator<BtlvPos>  ite = enemyPosList.Begin(); ite != enemyPosList.End(); ++ite )
            {
              BtlvPos posType             = *ite;

              app::BattleUIActSelectLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;

              if( posType == BTL_VPOS_ROYAL_RIGHT )
              {
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;
              }
              else
              if( posType == BTL_VPOS_ROYAL_LEFT )
              {
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP;
              }
              else
              if( posType == BTL_VPOS_ROYAL_FAR )
              {
                pokeDataIdx = app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;
              }
              else
              {
                GFL_ASSERT_STOP( 0 );
              }

              pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

              pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
              pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
              pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 
            }
          }
        }

        //  画像描画クラス生成
        m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        //  画面入力クラス生成
        m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIActSelectLowerInputNormal( m_pDraw );
        GFL_ASSERT( m_pInput );

        m_pDraw->Initialize();

        m_pDraw->SetCommandMode( app::BattleUIActSelectLowerDraw::COMMAND_MODE_RUN, false );
      }
    }
  };

  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  BattleUIActSelectFrame::BattleUIActSelectFrame()
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  BattleUIActSelectFrame::~BattleUIActSelectFrame()
  {
  }

  void BattleUIActSelectFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

  #ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIActSelectFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIActSelectFrame(Devcie) Initialize\n" );
  #endif

    m_pAppParam->out_param.Clear();

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIActSelectFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( pAppParam->in_param.pParentAppHeap, pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIActSelectFrame::InitFunc()
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );
    GFL_ASSERT( m_pAppParam->in_param.pAsyncFileManager );

    gfl2::fs::AsyncFileManager* pAsyncFileMan = m_pAppParam->in_param.pAsyncFileManager;

    int seq = applib::frame::CellSuper::GetSubSeq();

    enum
    {
      SEQ_OPEN_FILE       = 0,
      SEQ_WAIT_FILE_OPEN,

      SEQ_LOAD_FILE,
      SEQ_WAIT_FILE_LOAD,

      SEQ_CLOSE_FILE,
      SEQ_WAIT_FILE_CLOSE,

      SEQ_CREATE_VIEW,

      SEQ_END
    };

    ///読み込み開始をここに記述
    if( seq == SEQ_OPEN_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq oreq;
      oreq.arcId       = s_arcId;
      oreq.heapForFile = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      oreq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileMan->AddArcFileOpenReq(oreq);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_OPEN )
    {
      if( pAsyncFileMan->IsArcFileOpenFinished( s_arcId ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_LOAD_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

      req.arcId             = s_arcId;
      req.datId             = GARC_battleLowwerMenu_minlow_APPLYT;
      req.ppBuf             = &m_pArcReadBuff;
      req.heapForBuf        = m_pAppHeap->GetDeviceHeap();
      req.heapForReq        = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      req.heapForCompressed = NULL;
      req.align             = 128;

      pAsyncFileMan->AddArcFileLoadDataReq(req);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_LOAD )
    {
      if(pAsyncFileMan->IsArcFileLoadDataFinished( &m_pArcReadBuff ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_CLOSE_FILE )
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq creq;
      creq.arcId       = s_arcId;
      creq.heapForReq  = m_pAppHeap->GetDeviceHeap()->GetLowerHandle();
      pAsyncFileMan->AddArcFileCloseReq(creq);

      ++seq;
    }

    if( seq == SEQ_WAIT_FILE_CLOSE )
    {
      if(pAsyncFileMan->IsArcFileCloseFinished( s_arcId ) )
      {
        ++seq;
      }
    }

    if( seq == SEQ_CREATE_VIEW )
    {
      //  画面クラスを作成
      {
        //  バトルのルールによって制御方法を変える
        BtlRule rule  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleRule();

        //  バトルシングル用
        if( rule == BTL_RULE_SINGLE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameSingleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトル2VS2用
        else
        if( rule == BTL_RULE_DOUBLE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameDoubleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトルロワイヤル用
        else
        if( rule == BTL_RULE_ROYAL )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameRoyaleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトル乱入用( 1 vs 2 )
        else
        if( rule == BTL_RULE_INTRUDE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameIntrusionCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        else
        {
          //  該当するルールがない場合は論理バグ
          GFL_ASSERT( 0 );
        }

        //  ボールショートカットボタンを非表示にするか決める
        if( m_pAppParam->in_param.pBtlParamAccessror->IsShortCutUseItemBall() == true )
        {
          m_pCore->SetUseShortCutItemBall( true );
        }
        else
        {
          m_pCore->SetUseShortCutItemBall( false );
        }
      }

  #ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIActSelectFrame(System) Create View\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIActSelectFrame(Devcie) Create View\n" );
  #endif

      //  ボールショートカットボタンを非表示にするか決める
      if( m_pAppParam->in_param.pBtlParamAccessror->IsShortCutUseItemBall() == true )
      {
        m_pCore->SetUseShortCutItemBall( true );
      }
      else
      {
        m_pCore->SetUseShortCutItemBall( false );
      }

      ++seq;
    }

    if( seq == SEQ_END )
    {
      //  ロードを終了して更新処理へ
      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIActSelectFrame::UpdateFunc()
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_CREATE );
    }

    if( m_state == STATE_TYPE_CREATE )
    {
      _SetState( STATE_TYPE_IDLE );
    }

    if( m_pCore != NULL )
    {
      m_pCore->Update();
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_pCore == NULL )
      {
        GFL_ASSERT( 0 );
      }
      else
      if( m_pCore->GetNowState() == CoreImp::STATE_CORE_TYPE_PAUSE )
      {
        _SetState( STATE_TYPE_SUSPEND );
      }
    }

    if( m_state == STATE_TYPE_SUSPEND )
    {
    }

    _ChkEndUpdate();

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
    if( no == gfl2::gfx::CtrDisplayNo::DOWN )
    {
      if( m_pCore )
      {
        m_pCore->Draw( no );
      }
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result BattleUIActSelectFrame::EndFunc()
  {
    enum
    {
      //  破棄開始
      SEQ_START  = 0,
      //  破棄完了待ち
      SEQ_WAIT,
      //  破棄
      SEQ_END
    };

    m_pAppParam->out_param.bDie = true;

    int seq = applib::frame::CellSuper::GetSubSeq();
    if( seq == SEQ_START )
    {
      if( m_pCore != NULL )
      {
        m_pCore->Terminate();
      }

      ++seq;
    }

    if( seq == SEQ_WAIT )
    {
      if( m_pCore != NULL )
      {
        m_pCore->Update();

        if( m_pCore->IsDelete() )
        {
          ++seq;
        }
      }
    }

    if( seq == SEQ_END )
    {
      GFL_SAFE_DELETE( m_pCore );
      GflHeapSafeFreeMemory( m_pArcReadBuff );
      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

  #if GFL_DEBUG_PRINT
      m_pAppParam->in_param.pAsyncFileManager->PrintArcFileList();
  #endif

      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
    * @brief フレームが使えるようになっているかどうか
    */
  //------------------------------------------------------------------
  bool BattleUIActSelectFrame::IsReady()
  {
    //  ステートが待機以降になっているのであればOK!
    if( STATE_TYPE_IDLE <= m_state  )
    {
      return true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief イベント中か
    */
  //------------------------------------------------------------------
  bool BattleUIActSelectFrame::IsEvent()
  {
    if( m_pCore != NULL )
    {
      //  更新ステート以外はすべてイベント扱い
      //  ステータス・プレイヤーレート表示もイベント扱い
      CoreImp::StateCoreType  state = m_pCore->GetNowState();
      if( state == CoreImp::STATE_CORE_TYPE_UPDATE )
      {
        return  false;
      }
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief 開始通知
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::Start(const u32 targetPokeIndex)
  {
    //  しっかり準備が整っていない状態で呼ぶとバグる
    GFL_ASSERT( IsReady() == true );

    if( m_bStartup )
    {
      return;
    }

    m_bStartup  = true;

    GFL_ASSERT( m_pAppParam );
    m_pAppParam->out_param.Clear();

    if( m_pCore == NULL )
    {
      //  まだコアが作成していないのでデータのみ処理する
      return;
    }

    m_pCore->SetTargetPokeIndex(targetPokeIndex);

    //  すでにコアが作成されているのであれば、即時実行が可能
    _SetState( STATE_TYPE_UPDATE );
  }

  //------------------------------------------------------------------
  /**
    * @brief 再度開始通知
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::Restart(const u32 targetPokeIndex)
  {
    GFL_ASSERT( IsReady() == true );
    GFL_ASSERT( m_pCore );

    GFL_ASSERT( m_pAppParam );
    m_pAppParam->out_param.Clear();

    m_pCore->SetTargetPokeIndex(targetPokeIndex);
    m_pCore->ReStart();
    m_pCore->SetMaskDraw( false );
  }

  //------------------------------------------------------------------
  /**
    * @brief 終了通知
    * @note
    *  画面が閉じた後にインスタンスが破棄されるので注意
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::End()
  {
    if( m_pAppParam->out_param.bDie == true )
    {
      return;
    }

    m_pAppParam->out_param.bDie = true;

    if( m_state <= STATE_TYPE_LOAD )
    {
      //  まだ準備が整っていない時はいきなり殺す
      _SetState( STATE_TYPE_END );
    }
    else
    {
      GFL_ASSERT( m_pCore );

      //  終了
      m_pCore->End();
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 強制終了通知(子フレームのみ)
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::EndChildFrame()
  {
    if( m_pAppParam->out_param.bDie == true )
    {
      return;
    }

    if( m_state <= STATE_TYPE_LOAD )
    {
    }
    else
    {
      GFL_ASSERT( m_pCore );

      //  終了(行動選択はポーズする)
      m_pCore->EndChildFrame( true );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief コマンド通知
    */
  //------------------------------------------------------------------
  bool BattleUIActSelectFrame::PushCmd( const CmdType type )
  {
    if( IsReady() == false )
    {
      return  false;
    }
    else
    if( IsEvent() == true )
    {
      return  false;
    }

    if( type == CMD_TYPE_PUSH_BUTTON_FIGHT )
    {
      m_pCore->GetInput()->PushBtnSelectCursorItem( app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT );
    }
    else
    if( type == CMD_TYPE_PUSH_BUTTON_BAG )
    {
      m_pCore->GetInput()->PushBtnSelectCursorItem( app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG );
    }

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief 入力設定
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::SetEnableInput( const bool bFlg )
  {
    if( IsReady() == false )
    {
      return;
    }
    else
    if( IsEvent() == true )
    {
      return;
    }

    //  更新時に入力可能
    m_pCore->SetEnableInput( bFlg );
  }

  //------------------------------------------------------------------
  /**
    * @brief 再開通知
    * @note
    *  config_paramのパラメータを参照して設定およびリザルトをクリアして再開
    */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::_ReStart()
  {
    //  スタート処理の設定している場合はリスタートをせずにスタート処理を優先に
    if( m_bStartup )
    {
      return;
    }

    m_resultData.Clear();
    m_pAppParam->out_param.Clear();

    GFL_ASSERT( m_pCore );
    m_pCore->ReStart();
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::_SetState( const BattleUIActSelectFrame::StateType newStateType )
  {
    m_state = newStateType;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_CREATE )
    {
    /*
      //  画面クラスを作成
      {
        //  バトルのルールによって制御方法を変える
        BtlRule rule  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleRule();

        //  バトルシングル用
        if( rule == BTL_RULE_SINGLE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameSingleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトル2VS2用
        else
        if( rule == BTL_RULE_DOUBLE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameDoubleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトルロワイヤル用
        else
        if( rule == BTL_RULE_ROYAL )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameRoyaleCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        //  バトル乱入用( 1 vs 2 )
        else
        if( rule == BTL_RULE_INTRUDE )
        {
          m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIActSelectFrameIntrusionCore();
          m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
        }
        else
        {
          //  該当するルールがない場合は論理バグ
          GFL_ASSERT( 0 );
        }

        //  ボールショートカットボタンを非表示にするか決める
        if( m_pAppParam->in_param.pBtlParamAccessror->IsShortCutUseItemBall() == true )
        {
          m_pCore->SetUseShortCutItemBall( true );
        }
        else
        {
          m_pCore->SetUseShortCutItemBall( false );
        }
      }

  #ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIActSelectFrame(System) Create View\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIActSelectFrame(Devcie) Create View\n" );
  #endif
  */
    }
    else
    if( m_state == STATE_TYPE_IDLE )
    {
      GFL_ASSERT( m_pCore );
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      //  開始
      m_pCore->Start();

      //  test
      // ShowWaitComMesssageWindow();

      m_bStartup  = false;
    }
    else
    if( m_state == STATE_TYPE_SUSPEND )
    {

    }
    else
    if( m_state == STATE_TYPE_END )
    {
      m_pAppParam->out_param.bDie = true;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief 処理終了チェック
    * @note
    *   処理中にCore処理が終了しているかチェックして、終了していればステートを終了に切り替えて終わらせる
  */
  //------------------------------------------------------------------
  void BattleUIActSelectFrame::_ChkEndUpdate()
  {
    if( m_pCore == NULL )
    {
      return;
    }

    //  描画の閉じる演出が終わったので、frame処理を終了させる
    if( m_pCore->GetNowState() == CoreImp::STATE_CORE_TYPE_END )
    {
      //  処理終了通知
      m_pCore->SendProcEnd();

      _SetState( STATE_TYPE_END );
    }
  }

  //  ここからは 先は BattleUIActSelectFrame::CoreImp の定義

  //  Drawクラスの初期化パラメータ作成
  void BattleUIActSelectFrame::CoreImp::CreateDrawInitParam( app::BattleUIActSelectLowerDraw::INIT_PARAM* pOutParam, void* pLytDataBuff )
  {
    GFL_ASSERT( pOutParam );

    app::BattleUIDataManager* pDataManager  = m_pAppParam->in_param.pBtlParamAccessror;

    //  初期化パラメータ設定
    {
      pOutParam->pDelegate            = this;
      pOutParam->pLytBuff             = pLytDataBuff;
      pOutParam->pRenderingManager    = m_pAppParam->in_param.pRenderingManager;
      pOutParam->pUtilHeap            = m_pUtilHeap;
      pOutParam->selectItemIdx        = m_pAppParam->config_param.selectCursorType;
      pOutParam->pMenuCursorLytBuff   = pDataManager->GetMenuCusrorLytResourceBuffer();
      pOutParam->bUseInputSE          = pDataManager->IsPlayInputSE();
      pOutParam->bFriendMuliti        = pDataManager->IsMultiMode();
      pOutParam->pBtlAppMsgData       = pDataManager->GetAppMesssageData();
      pOutParam->pBtlWordSet          = pDataManager->GetWordSet();
    }

    //  バッグが使えるかどうか
    BtlCompetitor competitor  = pDataManager->GetBattleCompetitor();

    if( competitor == BTL_COMPETITOR_COMM )
    {
      //  通信対戦の場合
      pOutParam->pEnemyPlayerName = NULL;

      //  対戦相手のトレーナー情報を取得
      if( pOutParam->bFriendMuliti == false )
      {
        //  マルチでなく対戦相手が１人のみトレーナー情報を取得して名前を表示する
        if( pDataManager->GetEnemyTrainerNum() == 1 )
        {
          app::BattleUIDataManager::ENEMY_TRAINER_PARAM trainerParam;

          pDataManager->OutputEnemyTrainerParam( &trainerParam, 0 );

          pOutParam->pEnemyPlayerName   = trainerParam.pName;
          pOutParam->enemyPlayerGrammer = trainerParam.grammer;
        }
      }
      else
      {
        //  マルチ対戦の場合は対戦情報を表示しないのでボタンは出さない
      }

      pOutParam->drawMode = app::BattleUIActSelectLowerDraw::DRAW_MODE_NET;
    }
    else
    if( pDataManager->IsUseBag() == false )
    {
      //  バッグが使えない場合(バッグコマンドがないバージョン)
      pOutParam->drawMode = app::BattleUIActSelectLowerDraw::DRAW_MODE_BTL_FACLIITY;
    }
    else
    {
      pOutParam->drawMode = app::BattleUIActSelectLowerDraw::DRAW_MODE_NORMAL;
    }
  }

  //  終了処理
  void BattleUIActSelectFrame::CoreImp::Terminate()
  {
    //  すでにステートが終了状態なら何もしない
    if( m_state == STATE_CORE_TYPE_END )
    {
      return;
    }

    SetState( STATE_CORE_TYPE_END );
  }

  //  更新処理(共通のは実装)
  void BattleUIActSelectFrame::CoreImp::Update()
  {
    //  開くと閉じるはすべて共通なので一括で処理をまとめる
    if( m_state == STATE_CORE_TYPE_OPEN )
    {
      m_pDraw->UpdateTree();

      if( m_pDraw->GetState() == app::BattleUIActSelectLowerDraw::STATE_TYPE_IDLE )
      {
        SetState( STATE_CORE_TYPE_UPDATE );
      }
    }

    if( m_state == STATE_CORE_TYPE_UPDATE )
    {
      m_pDraw->UpdateTree();
    }
  
    if( m_state == STATE_CORE_TYPE_ENTER_EVENT )
    {
      m_pDraw->UpdateTree();

        //  入力したボタンアニメ終了を待つ
      if( m_pDraw->IsButtonAnimation() == false )
      {
        //  ボタン終了
        if( m_resultData.type == RESULT_TYPE_MONS_ICON )
        {
          SetState( STATE_CORE_TYPE_POKE_STATUS );
        }
        else
        if( m_resultData.type == RESULT_TYPE_USER_INFO )
        {
          //  通信状態でプレイヤー名を押したので、プレイヤー参加確認情報を表示
          SetState( STATE_CORE_TYPE_USER_INFO );
        }
        else
        {
          //  完了したのでリザルトデータを設定
          m_pAppParam->out_param.resultData  = m_resultData;

          //  ポーズする
          SetState( STATE_CORE_TYPE_PAUSE );
        }
      }
    }
  
    if( m_state == STATE_CORE_TYPE_POKE_STATUS )
    {
      if( m_uiPokeStatusFrameAppParam.out_param.bEnd )
      {
        m_pUIPokeStatusFrame  = NULL;

        if( m_bNextPause == true )
        {
          SetState( STATE_CORE_TYPE_PAUSE );
        }
        else
        {
          SetState( STATE_CORE_TYPE_UPDATE );
        }
      }
    }

    if( m_state == STATE_CORE_TYPE_USER_INFO )
    {
      if( m_uiPokeEntryConfirmFrameAppParam.out_param.bEnd )
      {
        m_pUIPokeEntryConfirmFrame  = NULL;
        if( m_bNextPause == true )
        {
          SetState( STATE_CORE_TYPE_PAUSE );
        }
        else
        {
          SetState( STATE_CORE_TYPE_UPDATE );
        }
      }
    }
  
    if( m_state == STATE_CORE_TYPE_PAUSE )
    {
      enum
      {
        STATE_PAUSE_SEQ_INIT  = 0,
        STATE_PAUSE_SEQ_UPDATE,
        STATE_PAUSE_SEQ_END
      };

      if( m_subSeq == STATE_PAUSE_SEQ_INIT )
      {
        ++m_subSeq;
      }

      if( m_subSeq == STATE_PAUSE_SEQ_UPDATE )
      {
        {
          //  使う側の手で終了させる（フレーム存在しているが、処理としては終了しているので結果反映させる）
          SendProcEnd();

          ++m_subSeq;
        }
      }

      if( m_subSeq == STATE_PAUSE_SEQ_END )
      {
      }

      //  使う側が終了通知するまて待つ
      m_pDraw->UpdateTree();
    }
  
    if( m_state == STATE_CORE_TYPE_CLOSE )
    {
      m_pDraw->UpdateTree();

      if( m_pDraw->GetState() == app::BattleUIActSelectLowerDraw::STATE_TYPE_END )
      {
        //  背景にBGカバーを表示
        m_pAppParam->in_param.pBGFrame->SetVisibleCurverBG( true );

        SetState( STATE_CORE_TYPE_END );
      }
    }

    if( m_state == STATE_CORE_TYPE_END )
    {
      m_pDraw->UpdateTree();
    }
  }

  //  開始
  void BattleUIActSelectFrame::CoreImp::Start()
  {
    SetState( STATE_CORE_TYPE_OPEN );
  }

  //  再開
  void BattleUIActSelectFrame::CoreImp::ReStart()
  {
    //  開始中であれば何もしない
    if( m_state == STATE_CORE_TYPE_OPEN )
    {
      GFL_PRINT( "行動選択画面が開いている途中\n" );
    }
    else
    if( m_state == STATE_CORE_TYPE_UPDATE )
    {
      //  更新中であれば、使う側でデータの再設定をしているのでこちらで今することはない
      GFL_PRINT( "行動選択画面がすでに開いているのでデータ設定のみ\n" );
    }
    else
    if( m_state == STATE_CORE_TYPE_ENTER_EVENT )
    {
      //  データを再設定して更新に戻す
      SetState( STATE_CORE_TYPE_UPDATE );
    }
    else
    if( m_state == STATE_CORE_TYPE_POKE_STATUS )
    {
    }
    else
    if( m_state == STATE_CORE_TYPE_USER_INFO )
    {

    }
    else
    if( m_state == STATE_CORE_TYPE_PAUSE )
    {
      //  データを再設定して開始からやり直す
      SetState( STATE_CORE_TYPE_UPDATE );
    }
    else
    if( m_state == STATE_CORE_TYPE_CLOSE )
    {
      //  画面が閉じている途中なので開くところから再開(ここにくるケースはないと思うが)
      SetState( STATE_CORE_TYPE_OPEN );
    }
    else
    if( m_state == STATE_CORE_TYPE_END )
    {
      //  画面が閉じている状態なので開くところから再開
      SetState( STATE_CORE_TYPE_OPEN );
    }
    else
    {
      //  現在のステート以外の場合はバグなのでとめる
      GFL_ASSERT( 0 );
    }
  }

  //  終了
  void BattleUIActSelectFrame::CoreImp::End()
  {
    EndChildFrame( false );
    SetState( STATE_CORE_TYPE_CLOSE );
  }

  //  内部で作成した子フレームのみ削除
  void BattleUIActSelectFrame::CoreImp::EndChildFrame( const bool bNextPause )
  {
    //  ポケモン詳細フレームが開いているなら閉じる
    if( ( m_uiPokeStatusFrameAppParam.out_param.bEnd == false ) && ( m_pUIPokeStatusFrame != NULL ) )
    {
      m_pUIPokeStatusFrame->End();
      m_pUIPokeStatusFrame  = NULL;
    }

    //  ポケモンレートフレームが開いているなら閉じる
    if( ( m_uiPokeEntryConfirmFrameAppParam.out_param.bEnd == false ) && ( m_pUIPokeEntryConfirmFrame != NULL ) )
    {
      m_pUIPokeEntryConfirmFrame->End();
      m_pUIPokeEntryConfirmFrame  = NULL;
    }

    //  
    m_bNextPause  = bNextPause;
  }

  /** ポケモンショートカット使用設定 */
  void BattleUIActSelectFrame::CoreImp::SetUseShortCutItemBall( const bool bFlg )
  {
    m_useShortCutItemBall = bFlg;

    //  画面で使えるかチェック
    if( m_pDraw->IsBtnUseBallShortCut() == false )
    {
      m_useShortCutItemBall = false;
    }

    m_pDraw->SetVisibleButton( app::BattleUIActSelectLowerDraw::BUTTON_ID_BALL, bFlg );
  }

  /** マスク設定 */
  void BattleUIActSelectFrame::CoreImp::SetMaskDraw( const bool bFlg )
  {
    GFL_ASSERT( m_pAppParam->in_param.pBGFrame );
    m_pAppParam->in_param.pBGFrame->SetEnableDimFilterLayout( bFlg );
  }

  //  入力設定
  void BattleUIActSelectFrame::CoreImp::SetEnableInput( const bool bFlg )
  {
    if( m_pDraw == NULL )
    {
      return;
    }

    m_pDraw->SetInputEnabled( bFlg );
  }

  //  スタート設定
  void BattleUIActSelectFrame::CoreImp::SetState( const StateCoreType newState )
  {
    StateCoreType oldState  = m_state;

    m_subSeq  = 0;
    m_state   = newState;

    if( newState == STATE_CORE_TYPE_OPEN )
    {
      //  背景処理を開始させる(連続で開始しても内部で連続処理させないようにしている)
      {
        if( m_pAppParam->in_param.pBGFrame != NULL )
        {
          if( m_pAppParam->in_param.pBGFrame->Start() == false )
          {
            //  すでに開始している場合は、こちらでBGカバー表示をOFF
            m_pAppParam->in_param.pBGFrame->SetVisibleCurverBG( false );
          }
        }
      }

      m_pDraw->Start();

      //  画面を閉じる時はマスクは不要
      SetMaskDraw( false );

      _UpdateMonsterBallIconState();

      //  開始時は入力はOFF
      SetEnableInput( false );

      m_pDraw->SetInputEnabled( false );
      m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
    }
    else
    if( newState == STATE_CORE_TYPE_UPDATE )
    {
      //  ボールアイコン更新
      _UpdateMonsterBallIconState();

      SetMaskDraw( false );

      //  更新時に入力可能
      SetEnableInput( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
    }
    else
    if( newState == STATE_CORE_TYPE_ENTER_EVENT )
    {
      SetEnableInput( false );
    }
    else
    if( newState == STATE_CORE_TYPE_POKE_STATUS )
    {
      GFL_ASSERT( m_resultData.type == RESULT_TYPE_MONS_ICON );

      GFL_ASSERT( m_pUIPokeStatusFrame == NULL );

      //  画面全体をマスク表示
      SetMaskDraw( true );

        //  バトル専用ポケモン詳細画面へ遷移
      m_pUIPokeStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIPokeStatusFrame>( m_pFrameManager );
      GFL_ASSERT( m_pUIPokeStatusFrame );

      BattleUIPokeStatusFrame::APP_PARAM::_tag_in_param*        pInParam  = &m_uiPokeStatusFrameAppParam.in_param;
      const app::BattleUIActSelectLowerDraw::POKE_DATA*         pPokeData = m_resultData.data.mons.pPokeData;

      pInParam->pAsyncFileManager   = m_pAppParam->in_param.pAsyncFileManager;
      pInParam->pRenderingManager   = m_pAppParam->in_param.pRenderingManager;
      pInParam->pParentAppHeap      = m_pAppParam->in_param.pParentAppHeap;
      pInParam->pParentDevHeap      = m_pAppParam->in_param.pParentDevHeap;
      pInParam->statusType          = (BattleUIPokeStatusFrame::StatusType)pPokeData->type;
      pInParam->pPokeParam          = pPokeData->p_btl_poke_param;
      pInParam->pStatusDataList     = pPokeData->p_btl_poke_status;
      pInParam->pBtlParamAccessror  = m_pAppParam->in_param.pBtlParamAccessror;
      pInParam->pTmpDevHeap         = m_pAppParam->in_param.pTmpDevHeap;

      m_pUIPokeStatusFrame->Initialize( &m_uiPokeStatusFrameAppParam );
    }
    else
    if( newState == STATE_CORE_TYPE_USER_INFO )
    {
      GFL_ASSERT( m_resultData.type == RESULT_TYPE_USER_INFO );
      GFL_ASSERT( m_pAppParam->in_param.pBtlParamAccessror->GetBattleCompetitor() == BTL_COMPETITOR_COMM );

      //  通信時のユーザー情報画面へ遷移

      //  画面全体をマスク表示
      SetMaskDraw( true );

      GFL_ASSERT( m_pUIPokeStatusFrame == NULL );

      //  バトル専用ポケモン詳細画面へ遷移
      m_pUIPokeEntryConfirmFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIPokeEntryConfirmFrame>( m_pFrameManager );
      GFL_ASSERT( m_pUIPokeEntryConfirmFrame );

      BattleUIPokeEntryConfirmFrame::APP_PARAM::_tag_in_param*  pInParam  = &m_uiPokeEntryConfirmFrameAppParam.in_param;

      pInParam->pAsyncFileManager   = m_pAppParam->in_param.pAsyncFileManager;
      pInParam->pRenderingManager   = m_pAppParam->in_param.pRenderingManager;
      pInParam->pParentAppHeap      = m_pAppParam->in_param.pParentAppHeap;
      pInParam->pParentDevHeap      = m_pAppParam->in_param.pParentDevHeap;
      pInParam->pBtlParamAccessror  = m_pAppParam->in_param.pBtlParamAccessror;
      pInParam->pTmpDevHeap         = m_pAppParam->in_param.pTmpDevHeap;

      m_pUIPokeEntryConfirmFrame->Initialize( &m_uiPokeEntryConfirmFrameAppParam );
    }
    else
    if( newState == STATE_CORE_TYPE_PAUSE )
    {
      //  ポーズ画面設定
      SetEnableInput( false );
    }
    else
    if( newState == STATE_CORE_TYPE_CLOSE )
    {
      //  ポケモンステートが開いている場合は一緒に閉じる
      if( ( oldState == STATE_CORE_TYPE_POKE_STATUS ) && m_pUIPokeStatusFrame )
      {
        m_pUIPokeStatusFrame->End();
        m_pUIPokeStatusFrame  = NULL;
      }

      //  ユーザー情報画面が開いているのであれば一緒に閉じる
      if( oldState == STATE_CORE_TYPE_USER_INFO && m_pUIPokeEntryConfirmFrame )
      {
        m_pUIPokeEntryConfirmFrame->End();
        m_pUIPokeEntryConfirmFrame  = NULL;
      }

      //  すでに終了しているならスキップ
      if( m_pDraw->GetState() != app::BattleUIActSelectLowerDraw::STATE_TYPE_END )
      {
        m_pDraw->End();
      }

      //  画面を閉じる時はマスクは不要
      SetMaskDraw( false );

      //  閉じる時は入力はOFF
      SetEnableInput( false );
    }
    else
    if( newState == STATE_CORE_TYPE_END )
    {
      //  閉じる時は入力はOFF
      SetEnableInput( false );
      m_pDraw->Terminate();
    }
  }

  //  決定入力用リスナ
  bool BattleUIActSelectFrame::CoreImp::OnEnterInput( app::BattleUIActSelectLowerDraw* pDrawInst, s32 buttonId )
  {
    if( m_state != STATE_CORE_TYPE_UPDATE )
    {
      return  false;
    }

    //  入力後のデータテーブル
    typedef struct
    {
      app::BattleUIActSelectLowerDraw::ButtonId   button_id;
      ResultType                                  result_type;

      union
      {
        s32 work[ 4 ];

        struct
        {
          app::BattleUIActSelectLowerDraw::CursorType type;
        } cursor_data;

        struct
        {
          app::BattleUIActSelectLowerDraw::PokeIconPosType  pos_type;
        } select_poke_pos;
      };

    } LOCAL_ENTER_DATA;

    //  ボタンIdと決定入力IDとのパターンマップ
    const LOCAL_ENTER_DATA s_aButtonIdToEnterIdPatternMap[] =
    {
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_FIGHT,                BattleUIActSelectFrame::RESULT_TYPE_FIGHT,     app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_POKE,                 BattleUIActSelectFrame::RESULT_TYPE_POKE,      app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_BAG,                  BattleUIActSelectFrame::RESULT_TYPE_BAG,       app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_RUN,                  BattleUIActSelectFrame::RESULT_TYPE_RUN,       app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_BACK,                 BattleUIActSelectFrame::RESULT_TYPE_RUN,       -1 },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_BALL,                 BattleUIActSelectFrame::RESULT_TYPE_MONS_BALL, -1 },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_POKE_ICON_LEFT_DOWN,  BattleUIActSelectFrame::RESULT_TYPE_MONS_ICON, app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_POKE_ICON_RIGHT_DOWN, BattleUIActSelectFrame::RESULT_TYPE_MONS_ICON, app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_POKE_ICON_LEFT_UP,    BattleUIActSelectFrame::RESULT_TYPE_MONS_ICON, app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_POKE_ICON_RIGHT_UP,   BattleUIActSelectFrame::RESULT_TYPE_MONS_ICON, app::BattleUIActSelectLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP },
      { app::BattleUIActSelectLowerDraw::BUTTON_ID_USER_NAME,            BattleUIActSelectFrame::RESULT_TYPE_USER_INFO, -1 },
    };

    u32 loopNum = GFL_NELEMS( s_aButtonIdToEnterIdPatternMap );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( s_aButtonIdToEnterIdPatternMap[ i ].button_id == buttonId )
      {
        m_resultData.type = (BattleUIActSelectFrame::ResultType)s_aButtonIdToEnterIdPatternMap[ i ].result_type;

        if( m_resultData.type == BattleUIActSelectFrame::RESULT_TYPE_MONS_ICON )
        {
          //  ポケモンデータを取得
          app::BattleUIActSelectLowerDraw::PokeObj* pPokeObj  = m_pDraw->GetPokeObj( (app::BattleUIActSelectLowerDraw::PokeIconPosType)s_aButtonIdToEnterIdPatternMap[ i ].select_poke_pos.pos_type );

          m_resultData.data.mons.pPokeData  = &pPokeObj->GetPokeData();
          GFL_ASSERT( m_resultData.data.mons.pPokeData->p_btl_poke_param );
        }
        else
        {
          //  入力したカーソル位置を外側に保存
          if( s_aButtonIdToEnterIdPatternMap[ i ].cursor_data.type != -1 )
          {
            m_pAppParam->config_param.selectCursorType = (app::BattleUIActSelectLowerDraw::CursorType)s_aButtonIdToEnterIdPatternMap[ i ].cursor_data.type;
          }
        }

        SetState( CoreImp::STATE_CORE_TYPE_ENTER_EVENT );

        return  true;
      }
    }

    return  false;
  }

  /// キー入力通知用リスナ(入力制御はクラス側にまかせているので、処理を変更する時は参照するクラスを差し替えるだけでOK)
  bool BattleUIActSelectFrame::CoreImp::OnKeyAction( app::BattleUIActSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    //  更新以外では入力は受け付けない
    if( m_state != STATE_CORE_TYPE_UPDATE )
    {
      return  false;
    }

    //  決定
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      m_pInput->PushBtnCurrentCursorItem();
      return  true;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
    {
      m_pInput->PushBtnY();
      return  true;
    }

    if( pKey->IsTrigger( gfl2::ui::DIR_DOWN ) )
    {
      if( m_pInput->MoveDownCursor() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }

    if( pKey->IsTrigger( gfl2::ui::DIR_UP ) )
    {
      if( m_pInput->MoveUpCursor() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }

    if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) )
    {
      if( m_pInput->MoveLeftCursor() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }

    if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
    {
      if( m_pInput->MoveRightCursor() == true )
      {
        if( m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE() == true )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );
        }
      }

      return  true;
    }

    return  false;
  }

  //  モンスターボールアイコンの状態を更新
  void BattleUIActSelectFrame::CoreImp::_UpdateMonsterBallIconState()
  {
    GFL_ASSERT( m_pDraw );

    app::BattleUIDataManager* pUIDataManager  = m_pAppParam->in_param.pBtlParamAccessror;
    GFL_ASSERT( pUIDataManager );

    //  通信用描画画面であればボールアイコンは無条件で非表示
    if( m_pDraw->IsBtnUseBallShortCut() == false )
    {
      m_pDraw->SetBallItemId( Savedata::MyItem::ITEM_NONE, false );
    }
    else
    {
      //  ショートカットボールアイテムが使える場合はアイコン画像設定
      //  使えない場合は非表示に
      if( m_useShortCutItemBall == true )
      {
        //  モンスターボールを所持しているならボールアイコンを表示
        bool  bPassive  = false;

        BtlCompetitor competitor  = pUIDataManager->GetBattleCompetitor();
        if( competitor == BTL_COMPETITOR_TRAINER )
        {
          bPassive  = true;
        }
        else
        if( competitor == BTL_COMPETITOR_INST )
        {
          bPassive  = true;
        }

        m_pDraw->SetBallItemId( pUIDataManager->GetHaveFirstListMonsterBallId(), bPassive );
      }
      else
      {
        m_pDraw->SetVisibleButton( app::BattleUIActSelectLowerDraw::BUTTON_ID_BALL, false );
      }
    }
  }

GFL_NAMESPACE_END( btl )
