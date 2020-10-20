//=============================================================================
/**
 * @file	 BtlvUIWatchFrame.cpp
 * @brief	 バトル下画面の観戦画面の制御フレーム
 * @author yuto_uchida
 * @date	 2015.05.07
 */
//=============================================================================
#pragma once

#include "BtlvUIWatchFrame.h"

#include "arc_def_index/arc_def.h"
#include "arc_index/battleLowwerMenu.gaix"

#include <system/include/HeapDefine.h>

#include <Battle/source/btlv/BtlvUIBGFrame.h>

GFL_NAMESPACE_BEGIN( btl )

//  arcID
const s32 BattleUIWatchFrame::s_arcId = ARCID_BATTLE_LOWER_MENU;

GFL_NAMESPACE_BEGIN( app )

/**
* @brief バトル下画面の観戦画面の入力制御クラス
*/
class BattleUIWatchLowerInputNormal : public app::BattleUIWatchLowerInputInterface
{
public:
    BattleUIWatchLowerInputNormal( app::BattleUIWatchLowerDraw* pDrawCore ) :
    BattleUIWatchLowerInputInterface( pDrawCore ) {}

     //------------------------------------------------------------------
     /**
      * @brief   もどるボタンを押した
      */
    //------------------------------------------------------------------
    virtual void PushBtnBack()
    {
      if( m_pDrawCore->GetButtonManager()->IsButtonVisible( BattleUIWatchLowerDraw::BUTTON_ID_BACK ) )
      {
        //  ボタンを押した処理を走らせる
        m_pDrawCore->GetButtonManager()->StartSelectedAct( BattleUIWatchLowerDraw::BUTTON_ID_BACK );

        //  コールバックで入力結果を渡す
        if( m_pDrawCore->m_initParam.pDelegate )
        {
          m_pDrawCore->m_initParam.pDelegate->OnEnterInput( m_pDrawCore, BattleUIWatchLowerDraw::BUTTON_ID_BACK );
        }
      }
    }
};

GFL_NAMESPACE_END( app )

/** シングル用のコアクラス( 1 vs 1 の構図になる ) */
class BattleUIWatchFrameSingleCore : public BattleUIWatchFrame::CoreImp
{
public:
  BattleUIWatchFrameSingleCore()
  : CoreImp() {}

  virtual void Initialize( BattleUIWatchFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
  {
    CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

    app::BattleUIWatchLowerDraw::INIT_PARAM     initParam;

    app::BattleUIDataManager* pUIDataManager  = pAppParam->in_param.pBtlParamAccessror;
    GFL_ASSERT( pUIDataManager );

    {
      //  初期化パラメータ作成
      CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

      //  専用パラメータ設定
      {
        //  画面の状態を設定
        initParam.screenType = app::BattleUIWatchLowerDraw::SCREEN_TYPE_NORMAL;

        //  味方の配置
        {
          gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
          //  一人しかない
          GFL_ASSERT_STOP( posList.Size() == 1 );

          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          //  リストがひとつしかないのでループするまでもないが、リスト処理の形式として
          for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            GFL_ASSERT_STOP( posType == BTL_VPOS_NEAR_1 );
            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param      = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type                  = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
            pSetPokeData->p_btl_poke_status     = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;

              //  test
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_HATAKU;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 10;
              pSetPokeData->last_waza_data.pp_now           = 1;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
            pSetPokeData->p_player_name->SetStr( L"Test" );
          }
        }

        //  敵の配置
        {
          gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetEnemyPokePosPlaceList();
          //  敵が一人しかない
          GFL_ASSERT_STOP( posList.Size() == 1 );

          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          //  リストがひとつしかないのでループするまでもないが、リスト処理の形式として
          for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            GFL_ASSERT_STOP( posType == BTL_VPOS_FAR_1 );
            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param      = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type                  = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
            pSetPokeData->p_btl_poke_status     = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
          }
        }
      }

      //  描画クラス生成
      m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerDraw( initParam );
      GFL_ASSERT( m_pDraw );

      //  画面入力クラス生成
      m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerInputNormal( m_pDraw );
      GFL_ASSERT( m_pInput );

      m_pDraw->Initialize();
    }
  }
};

/** ダブル用のコアクラス( 2 vs 2 の構図になる ) */
class BattleUIWatchFrameDoubleCore : public BattleUIWatchFrame::CoreImp
{
public:
  BattleUIWatchFrameDoubleCore()
    : CoreImp()
  {
  }

  virtual void Initialize( BattleUIWatchFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
  {
    CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

    app::BattleUIDataManager* pUIDataManager  = pAppParam->in_param.pBtlParamAccessror;
    GFL_ASSERT( pUIDataManager );

    app::BattleUIWatchLowerDraw::INIT_PARAM     initParam;
    {
      //  初期化パラメータ作成
      CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

      //  専用パラメータ設定
      {
        //  画面の状態を設定
        initParam.screenType = app::BattleUIWatchLowerDraw::SCREEN_TYPE_DOUBLE;

        //  味方
        {
          gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
          //  二人しかない
          GFL_ASSERT_STOP( posList.Size() == 2 );

          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
            if( posType == BTL_VPOS_NEAR_1 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
            }
            else if( posType == BTL_VPOS_NEAR_2 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;
            }
            else
            {
              GFL_ASSERT_STOP( 0 );
            }

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param      = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type                  = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
            pSetPokeData->p_btl_poke_status     = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType );

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
          }
        }

        //  敵
        {
          gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetEnemyPokePosPlaceList();
          //  敵は必ず二人
          GFL_ASSERT_STOP( posList.Size() == 2 );

          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;

            if( posType == BTL_VPOS_FAR_1 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP;
            }
            else if( posType == BTL_VPOS_FAR_2 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;
            }
            else
            {
              GFL_ASSERT_STOP( 0 );
            }

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param      = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type                  = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
            pSetPokeData->p_btl_poke_status     = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType ); 

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
          }
        }
      }

      //  描画クラス生成
      {
        m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerDraw( initParam );
        GFL_ASSERT( m_pDraw );

        m_pDraw->Initialize();
      }

      //  画面入力クラス生成
      {
        m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerInputNormal( m_pDraw );
        GFL_ASSERT( m_pInput );
      }
    }
  }
};

/** バトルロワイヤル用のコアクラス( 1 vs 3 の構図になる ) */
class BattleUIWatchFrameRoyaleCore : public BattleUIWatchFrame::CoreImp
{
public:
  BattleUIWatchFrameRoyaleCore()
    : CoreImp()
  {
  }

  virtual void Initialize( BattleUIWatchFrame::APP_PARAM* pAppParam, applib::frame::Manager* pFrameManager, ::app::util::Heap* pUtilHeap, void* pLytDataBuff )
  {
    CoreImp::Initialize( pAppParam, pFrameManager, pUtilHeap, pLytDataBuff );

    app::BattleUIDataManager* pUIDataManager  = pAppParam->in_param.pBtlParamAccessror;
    GFL_ASSERT( pUIDataManager );

    app::BattleUIWatchLowerDraw::INIT_PARAM     initParam;
    {
      //  初期化パラメータ作成
      CoreImp::CreateDrawInitParam( &initParam, pLytDataBuff );

      //  専用パラメータ設定
      {
        //  画面の状態を設定
        initParam.screenType = app::BattleUIWatchLowerDraw::SCREEN_TYPE_ROYALE;

        //  味方
        {
          gfl2::util::List<BtlvPos>& posList = pUIDataManager->GetSelfPokePosPlace();
          //  一人しかない
          GFL_ASSERT_STOP( posList.Size() == 1 );

          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          for( gfl2::util::ListIterator<BtlvPos>  ite = posList.Begin(); ite != posList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
            if( posType == BTL_VPOS_NEAR_1 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN;
            }
            else
            {
              GFL_ASSERT_STOP( 0 );
            }

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param      = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type                  = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_SELF;
            pSetPokeData->p_btl_poke_status     = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType );

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
          }
        }

        //  敵
        {
          app::BattleUIWatchLowerDraw::POKE_DATA*  pSetPokeData  = NULL;

          gfl2::util::List<BtlvPos>& enemyPosList = pUIDataManager->GetEnemyPokePosPlaceList();
          for( gfl2::util::ListIterator<BtlvPos>  ite = enemyPosList.Begin(); ite != enemyPosList.End(); ++ite )
          {
            BtlvPos posType             = *ite;

            app::BattleUIWatchLowerDraw::PokeIconPosType pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;

            if( posType == BTL_VPOS_NEAR_2 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN;
            }
            else if( posType == BTL_VPOS_FAR_1 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP;
            }
            else if( posType == BTL_VPOS_FAR_2 )
            {
              pokeDataIdx = app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP;
            }
            else
            {
              GFL_ASSERT_STOP( 0 );
            }

            pSetPokeData  = &initParam.aPokeData[ pokeDataIdx ];

            pSetPokeData->p_btl_poke_param  = pUIDataManager->GetBattlePokeParam( posType );
            pSetPokeData->type              = (s32)btl::BattleUIPokeStatusFrame::STATUS_TYPE_ENEMY;
            pSetPokeData->p_btl_poke_status = m_pAppParam->in_param.pBtlParamAccessror->UpdatePokeStatusList( pSetPokeData->p_btl_poke_param, posType );

            //  最後に使用したわざ設定
            {
              pSetPokeData->last_waza_data.b_zenryoku_waza  = false;
              pSetPokeData->last_waza_data.normal_waza_id   = WAZANO_NULL;
              pSetPokeData->last_waza_data.zenryoku_waza_id = WAZANO_NULL;
              pSetPokeData->last_waza_data.pp_max           = 0;
              pSetPokeData->last_waza_data.pp_now           = 0;
            }

            pSetPokeData->p_player_name         = GFL_NEW( pUtilHeap->GetDeviceHeap() ) gfl2::str::StrBuf( PLYAER_NAME_MAX_LENGH, pUtilHeap->GetDeviceHeap() );
          }
        }
      }

      //  画像描画クラス生成
      m_pDraw = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerDraw( initParam );
      GFL_ASSERT( m_pDraw );

      //  画面入力クラス生成
      m_pInput  = GFL_NEW( pUtilHeap->GetDeviceHeap() ) app::BattleUIWatchLowerInputNormal( m_pDraw );
      GFL_ASSERT( m_pInput );

      m_pDraw->Initialize();
    }
  }
};

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
BattleUIWatchFrame::BattleUIWatchFrame()
{
  _Clear();
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
BattleUIWatchFrame::~BattleUIWatchFrame()
{
}

void BattleUIWatchFrame::Initialize( APP_PARAM* pAppParam )
{
  GFL_ASSERT( pAppParam );
  m_pAppParam = pAppParam;

  m_pAppParam->out_param.Clear();

#ifdef DEBUG_ONLY_FOR_uchida_yuto
      // ヒープ残量チェック
      m_pAppParam->in_param.pParentAppHeap->PrintHeapInfo( "BattleUIWatchFrame(System) Initialize\n" );
      m_pAppParam->in_param.pParentDevHeap->PrintHeapInfo( "BattleUIWatchFrame(Devcie) Initialize\n" );
#endif

  // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
  m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( pAppParam->in_param.pParentAppHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "BattleUIWatchFrame" );

  // このアプリ全体に必要なメモリを作成
  m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) ::app::util::Heap();

  // ローカルヒープを作る
  m_pAppHeap->LocalHeapCreate( pAppParam->in_param.pParentAppHeap, pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

  _SetState( STATE_TYPE_LOAD );
}

//------------------------------------------------------------------
/**
 * @brief	  PROCの初期化を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BattleUIWatchFrame::InitFunc()
{
  GFL_ASSERT( m_pAppHeap );
  GFL_ASSERT( m_pAppParam );
  GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );
  GFL_ASSERT( m_pAppParam->in_param.pAsyncFileManager );

  gfl2::fs::AsyncFileManager* pAsyncFileMan = m_pAppParam->in_param.pAsyncFileManager;

  enum
  {
    SEQ_OPEN_FILE       = 0,
    SEQ_WAIT_FILE_OPEN,

    SEQ_LOAD_FILE,
    SEQ_WAIT_FILE_LOAD,

    SEQ_CLOSE_FILE,
    SEQ_WAIT_FILE_CLOSE,

    SEQ_END
  };

  int seq = applib::frame::CellSuper::GetSubSeq();

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
    if(  pAsyncFileMan->IsArcFileOpenFinished( s_arcId ))
    {
      ++seq;
    }
  }

  if( seq == SEQ_LOAD_FILE )
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;

    req.arcId             = s_arcId;
    req.datId             = GARC_battleLowwerMenu_obslow_APPLYT;
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
      //  ロードを終了して更新処理へ
      return applib::frame::RES_FINISH;
    }
  }

  applib::frame::CellSuper::SetSubSeq( seq );

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result BattleUIWatchFrame::UpdateFunc()
{
  if( m_state == STATE_TYPE_LOAD )
  {
    _SetState( STATE_TYPE_CREATE );
  }

  if( m_state == STATE_TYPE_CREATE )
  {
    if( m_bStart )
    {
      _SetState( STATE_TYPE_UPDATE );
    }
  }

  if( m_state == STATE_TYPE_UPDATE )
  {
    m_pCore->Update();

    //  描画の閉じる演出が終わったので、frame処理を終了させる
    if( m_pCore->GetNowState() == CoreImp::STATE_CORE_TYPE_END )
    {
      //  処理終了通知
      m_pCore->SendProcEnd();

      _SetState( STATE_TYPE_END );
    }
  }

  if( m_state == STATE_TYPE_END )
  {
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void BattleUIWatchFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
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
applib::frame::Result BattleUIWatchFrame::EndFunc()
{
  enum
  {
    SEQ_INIT  = 0,
    SEQ_WAIT,
    SEQ_END
  };

  int seq = applib::frame::CellSuper::GetSubSeq();

  if( seq == SEQ_INIT )
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
    else
    {
      ++seq;
    }
  }

  if( seq == SEQ_END )
  {
    m_pAppParam->out_param.bDie = true;

    GFL_SAFE_DELETE( m_pCore );
    GflHeapSafeFreeMemory( m_pArcReadBuff );
    GFL_SAFE_DELETE( m_pAppHeap );

    gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

#if GFL_DEBUG_PRINT
    m_pAppParam->in_param.pAsyncFileManager->PrintArcFileList();
#endif

    return applib::frame::RES_FINISH;
  }

  return  applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief 開始通知
  */
//------------------------------------------------------------------
void BattleUIWatchFrame::Start()
{
  if( m_bStart )
  {
    return;
  }

  m_bStart  = true;

  GFL_ASSERT( m_pAppParam );
  m_pAppParam->out_param.Clear();

  if( m_pCore == NULL )
  {
    //  まだコアが作成していないのでデータのみ処理する
    return;
  }

  //  すでにコアが作成されているのであれば、即時実行が可能
  _SetState( STATE_TYPE_UPDATE );
}

//------------------------------------------------------------------
/**
  * @brief 再開通知
  * @note
  *  config_paramのパラメータを参照して設定およびリザルトをクリアして再開
  */
//------------------------------------------------------------------
void BattleUIWatchFrame::ReStart()
{
  //  スタート処理の設定している場合はリスタートをせずにスタート処理を優先に
  if( m_bStart )
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
  * @brief 終了通知
  * @note
  *  画面が閉じた後にインスタンスが破棄されるので注意
  */
//------------------------------------------------------------------
void BattleUIWatchFrame::End()
{
  if( m_state == STATE_TYPE_UPDATE )
  {
    GFL_ASSERT( m_pCore );

    m_pAppParam->out_param.bDie = true;

    //  終了
    m_pCore->End();
  }
  else
  {
    //  いきなり殺す
    _SetState( STATE_TYPE_END );
  }
}

//------------------------------------------------------------------
/**
  * @brief 状態ステート設定
*/
//------------------------------------------------------------------
void BattleUIWatchFrame::_SetState( const BattleUIWatchFrame::StateType newStateType )
{
  m_state = newStateType;

  if( m_state == STATE_TYPE_LOAD )
  {
  }
  else if( m_state == STATE_TYPE_CREATE )
  {
    //  画面クラスを作成
    {
      //  バトルのルールによって制御方法を変える
      BtlRule rule  = m_pAppParam->in_param.pBtlParamAccessror->GetBattleRule();

      //  バトルシングル用
      if( rule == BTL_RULE_SINGLE )
      {
        m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIWatchFrameSingleCore();
        m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
      }
      //  バトル2VS2用
      else
      if( rule == BTL_RULE_DOUBLE )
      {
        m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIWatchFrameDoubleCore();
        m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
      }
      //  バトルロワイヤル用
      else
      if( rule == BTL_RULE_ROYAL )
      {
        m_pCore = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) BattleUIWatchFrameRoyaleCore();
        m_pCore->Initialize( m_pAppParam, CellSuper::GetManager(), m_pAppHeap, m_pArcReadBuff );
      }
      else
      if( rule == BTL_RULE_INTRUDE )
      {
        //@memo  乱入は未対応
        GFL_ASSERT( 0 );
        //  なぜならこのフレームは本製品では使わないから
      }
    }
  }
  else if( m_state == STATE_TYPE_UPDATE )
  {
    GFL_ASSERT( m_pCore );

    //  開始
    m_pCore->Start();

    m_bStart  = false;
  }
  else if( m_state == STATE_TYPE_END )
  {
  }
}

//  ここからは 先は BattleUIWatchFrame::CoreImp の定義

//  デストラクタ
BattleUIWatchFrame::CoreImp::~CoreImp()
{
  //  プレイヤー名の文字列バッファを開放
  {
    app::BattleUIWatchLowerDraw::INIT_PARAM& drawInitParam = m_pDraw->GetInitParam();
    u32 loopNum = GFL_NELEMS( drawInitParam.aPokeData );

    for( u32 i = 0; i < loopNum; ++i )
    {
      GFL_SAFE_DELETE( drawInitParam.aPokeData[ i ].p_player_name );
    }
  }

  GFL_SAFE_DELETE( m_pDraw );
  GFL_SAFE_DELETE( m_pInput );
}

//  更新処理(共通のは実装)
void BattleUIWatchFrame::CoreImp::Update()
{
  //  開くと閉じるはすべて共通なので一括で処理をまとめる
  if( m_state == STATE_CORE_TYPE_OPEN )
  {
    m_pDraw->UpdateTree();

    if( m_pDraw->GetState() == app::BattleUIWatchLowerDraw::STATE_TYPE_IDLE )
    {
      SetState( STATE_CORE_TYPE_UPDATE );
    }
  }
  else if( m_state == STATE_CORE_TYPE_UPDATE )
  {
    m_pDraw->UpdateTree();

    UpdateChild();
  }
  else if( m_state == STATE_CORE_TYPE_ENTER_EVENT )
  {
    m_pDraw->UpdateTree();

      //  入力したボタンアニメ終了を待つ
    if( m_pDraw->GetButtonManager()->GetUpdateState() == ::app::tool::ButtonManager::UPDATE_STATE_IDLE )
    {
      //  ボタン終了
      if( m_resultData.type == RESULT_TYPE_MONS_ICON )
      {
        SetState( STATE_CORE_TYPE_POKE_STATUS );
      }
      else if( m_resultData.type == RESULT_TYPE_USER_INFO )
      {
        //  通信状態でプレイヤー名を押したので、プレイヤー参加確認情報を表示
        SetState( STATE_CORE_TYPE_USER_INFO );
      }
      else if( m_resultData.type == RESULT_TYPE_SKILL_INFO )
      {
        SetState( STATE_CORE_TYPE_SKILL_INFO );
      }
      else
      {
        //  完了したのでリザルトデータを設定
        m_pAppParam->out_param.resultData  = m_resultData;

        End();
      }
    }
  }
  else if( m_state == STATE_CORE_TYPE_POKE_STATUS )
  {
    if( m_uiPokeStatusFrameAppParam.out_param.bEnd )
    {
      m_pUIPokeStatusFrame  = NULL;
      SetState( STATE_CORE_TYPE_UPDATE );
    }
  }
  else if( m_state == STATE_CORE_TYPE_USER_INFO )
  {
    if( m_uiPokeEntryConfirmFrameAppParam.out_param.bEnd )
    {
      m_pUIPokeEntryConfirmFrame  = NULL;
      SetState( STATE_CORE_TYPE_UPDATE );
    }
  }
  else if( m_state == STATE_CORE_TYPE_SKILL_INFO )
  {
    if( m_uiSkillStatusFrameAppParam.out_param.bEnd )
    {
      m_pUISkillStatusFrame  = NULL;
      SetState( STATE_CORE_TYPE_UPDATE );
    }
  }
  else if( m_state == STATE_CORE_TYPE_PAUSE )
  {
    //  使う側が終了通知するまて待つ
    m_pDraw->UpdateTree();
  }
  else if( m_state == STATE_CORE_TYPE_CLOSE )
  {
    m_pDraw->UpdateTree();

    if( m_pDraw->GetState() == app::BattleUIWatchLowerDraw::STATE_TYPE_END )
    {
      SetState( STATE_CORE_TYPE_END );
    }
  }
  else if( m_state == STATE_CORE_TYPE_END )
  {
    m_pDraw->UpdateTree();
  }
}

//  開始
void BattleUIWatchFrame::CoreImp::Start()
{
  //  背景処理を開始させる(連続で開始しても内部で連続処理させないようにしている)
  {
    if( m_pAppParam->in_param.pBtlBGFrame )
    {
      m_pAppParam->in_param.pBtlBGFrame->Start();
    }
  }

  m_pDraw->Start();
  SetState( STATE_CORE_TYPE_OPEN );
}

//  再開
void BattleUIWatchFrame::CoreImp::ReStart()
{
  //  開始中であれば何もしない
  if( m_state == STATE_CORE_TYPE_OPEN )
  {
    GFL_PRINT( "行動選択画面が開いている途中\n" );
  }
  else if( m_state == STATE_CORE_TYPE_UPDATE )
  {
    //  更新中であれば、使う側でデータの再設定をしているのでこちらで今することはない
    GFL_PRINT( "行動選択画面がすでに開いているのでデータ設定のみ\n" );
  }
  else if( m_state == STATE_CORE_TYPE_ENTER_EVENT )
  {
    //  データを再設定して更新に戻す
    SetState( STATE_CORE_TYPE_UPDATE );
  }
  else if( m_state == STATE_CORE_TYPE_POKE_STATUS )
  {
  }
  else if( m_state == STATE_CORE_TYPE_USER_INFO )
  {

  }
  else if( m_state == STATE_CORE_TYPE_PAUSE )
  {
    //  データを再設定して更新に戻す
    SetState( STATE_CORE_TYPE_UPDATE );
  }
  else if( m_state == STATE_CORE_TYPE_CLOSE )
  {
    //  画面が閉じている途中なので開くところから再開(ここにくるケースはないと思うが)
    SetState( STATE_CORE_TYPE_OPEN );
  }
  else if( m_state == STATE_CORE_TYPE_END )
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
void BattleUIWatchFrame::CoreImp::End()
{
  if( ( m_uiPokeStatusFrameAppParam.out_param.bEnd == false ) && m_pUIPokeStatusFrame )
  {
    m_pUIPokeStatusFrame->End();
    m_pUIPokeStatusFrame  = NULL;
  }

  if( ( m_uiPokeEntryConfirmFrameAppParam.out_param.bEnd == false ) && m_pUIPokeEntryConfirmFrame )
  {
    m_pUIPokeEntryConfirmFrame->End();
    m_pUIPokeEntryConfirmFrame  = NULL;
  }

  m_pDraw->End();

  SetState( STATE_CORE_TYPE_CLOSE );
}

//  スタート設定
void BattleUIWatchFrame::CoreImp::SetState( const StateCoreType newState )
{
  StateCoreType oldState  = m_state;

  m_state = newState;

  if( newState == STATE_CORE_TYPE_OPEN )
  {
    //  開始時は入力はOFF
    m_pDraw->SetInputEnabled( false );
    m_pDraw->SetInputExclusiveKeyByButtonAnimation( true );
  }
  else if( newState == STATE_CORE_TYPE_UPDATE )
  {
    //  更新時に入力可能
    m_pDraw->SetInputEnabled( m_pAppParam->in_param.pBtlParamAccessror->GetCore()->IsControllable() );
  }
  else if( newState == STATE_CORE_TYPE_ENTER_EVENT )
  {
  }
  else if( newState == STATE_CORE_TYPE_POKE_STATUS )
  {
    GFL_ASSERT( m_resultData.type == RESULT_TYPE_MONS_ICON );

    GFL_ASSERT( m_pUIPokeStatusFrame == NULL );

      //  バトル専用ポケモン詳細画面へ遷移
    m_pUIPokeStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIPokeStatusFrame>( m_pFrameManager );
    GFL_ASSERT( m_pUIPokeStatusFrame );

    BattleUIPokeStatusFrame::APP_PARAM::_tag_in_param*    pInParam  = &m_uiPokeStatusFrameAppParam.in_param;
    const app::BattleUIWatchLowerDraw::POKE_DATA*         pPokeData = m_resultData.data.mons.pPokeData;

    pInParam->pAsyncFileManager   = m_pAppParam->in_param.pAsyncFileManager;
    pInParam->pRenderingManager   = m_pAppParam->in_param.pRenderingManager;
    pInParam->pParentAppHeap      = m_pAppParam->in_param.pParentAppHeap;
    pInParam->pParentDevHeap      = m_pAppParam->in_param.pParentDevHeap;
    pInParam->statusType          = (BattleUIPokeStatusFrame::StatusType)pPokeData->type;
    pInParam->pPokeParam          = pPokeData->p_btl_poke_param;
    pInParam->pStatusDataList     = pPokeData->p_btl_poke_status;
    pInParam->pBtlParamAccessror  = m_pAppParam->in_param.pBtlParamAccessror;

    m_pUIPokeStatusFrame->Initialize( &m_uiPokeStatusFrameAppParam );
  }
  else if( newState == STATE_CORE_TYPE_SKILL_INFO )
  {
      //  フレームの上にさらにわざ説明のフレームを重ねがけ
      m_pUISkillStatusFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, btl::BattleUISkillStatusFrame>( m_pFrameManager );
      GFL_ASSERT( m_pUISkillStatusFrame );

      BattleUISkillStatusFrame::APP_PARAM::_tag_in_param* pInParam  = &m_uiSkillStatusFrameAppParam.in_param;

      pInParam->pAsyncFileManager  = m_pAppParam->in_param.pAsyncFileManager;
      pInParam->pRenderingManager  = m_pAppParam->in_param.pRenderingManager;
      pInParam->pParentAppHeap     = m_pAppParam->in_param.pParentAppHeap;
      pInParam->pParentDevHeap     = m_pAppParam->in_param.pParentDevHeap;
      pInParam->bUseInputSE        = m_pAppParam->in_param.pBtlParamAccessror->IsPlayInputSE();
      pInParam->pTmpDevHeap           = m_pAppParam->in_param.pTmpDevHeap;
      pInParam->pAppCommonGrpIconData = &m_pAppParam->in_param.pBtlParamAccessror->GetAppCommonGrpIconData();
      pInParam->pBtlViewSystem        = m_pAppParam->in_param.pBtlParamAccessror->GetCore();

      //  わざ詳細関連のメッセージデータ設定
      {
        pInParam->pBtlAppMsgData    = m_pAppParam->in_param.pBtlParamAccessror->GetAppMesssageData();
        pInParam->pBtlWordSet       = m_pAppParam->in_param.pBtlParamAccessror->GetWordSet();
        pInParam->pWazaInfoMsgData  = m_pAppParam->in_param.pBtlParamAccessror->GetWazaInfoMesssageData();
      }

      //  わざのデータ設定
      {
        const app::BattleUIWatchLowerDraw::POKE_DATA* pPokeData = m_resultData.data.mons.pPokeData;

        //  通常わざの設定
        {
          pInParam->wazaId          = pPokeData->last_waza_data.normal_waza_id;
          pInParam->ppNowNum        = pPokeData->last_waza_data.pp_now;
          pInParam->ppMaxNum        = pPokeData->last_waza_data.pp_max;
        }

        //  全力わざであれば、全力わざのidを設定
        pInParam->bZenryoku       = pPokeData->last_waza_data.b_zenryoku_waza;
        if( pInParam->bZenryoku )
        {
          pInParam->zenryokuWazaId  = pPokeData->last_waza_data.zenryoku_waza_id;
        }
      }

      m_pUISkillStatusFrame->Initialize( &m_uiSkillStatusFrameAppParam );

  }
  else if( newState == STATE_CORE_TYPE_USER_INFO )
  {
    GFL_ASSERT( m_resultData.type == RESULT_TYPE_USER_INFO );

    //  選択したユーザー情報画面へ遷移
    GFL_ASSERT( m_pUIPokeStatusFrame == NULL );

    //  バトル専用ポケモン詳細画面へ遷移
    m_pUIPokeEntryConfirmFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIPokeEntryConfirmFrame>( m_pFrameManager );
    GFL_ASSERT( m_pUIPokeEntryConfirmFrame );

    m_uiPokeEntryConfirmFrameAppParam.in_param.pAsyncFileManager  = m_pAppParam->in_param.pAsyncFileManager;
    m_uiPokeEntryConfirmFrameAppParam.in_param.pRenderingManager  = m_pAppParam->in_param.pRenderingManager;
    m_uiPokeEntryConfirmFrameAppParam.in_param.pParentAppHeap     = m_pAppParam->in_param.pParentAppHeap;
    m_uiPokeEntryConfirmFrameAppParam.in_param.pParentDevHeap     = m_pAppParam->in_param.pParentDevHeap;

    m_pUIPokeEntryConfirmFrame->Initialize( &m_uiPokeEntryConfirmFrameAppParam );
  }
  else if( newState == STATE_CORE_TYPE_PAUSE )
  {
    m_pDraw->SetInputEnabled( false );

    //  使う側の手で終了させる（フレーム存在しているが、処理としては終了しているので結果反映させる）
    SendProcEnd();
  }
  else if( newState == STATE_CORE_TYPE_CLOSE )
  {
    //  ポケモンステートが開いている場合は一緒に閉じる
    if( ( oldState == STATE_CORE_TYPE_POKE_STATUS ) && m_pUIPokeStatusFrame )
    {
      m_pUIPokeStatusFrame->End();
      m_pUIPokeStatusFrame  = NULL;
    }

    //  ユーザー情報画面が開いているのであれば一緒に閉じる
    if( oldState == STATE_CORE_TYPE_USER_INFO )
    {

    }

    //  閉じる時は入力はOFF
    m_pDraw->SetInputEnabled( false );
  }
  else if( newState == STATE_CORE_TYPE_END )
  {
    //  閉じる時は入力はOFF
    m_pDraw->SetInputEnabled( false );
    m_pDraw->Terminate();
  }
}

//  決定入力用リスナ
bool BattleUIWatchFrame::CoreImp::OnEnterInput( app::BattleUIWatchLowerDraw* pDrawInst, s32 buttonId )
{
  if( m_state != STATE_CORE_TYPE_UPDATE )
  {
    return  false;
  }

  //  ボタンIdと決定入力IDとのパターンマップ
  typedef struct
  {
    app::BattleUIWatchLowerDraw::ButtonId buttonId;
    ResultType                            resultType;

    union
    {
      s32 work[ 4 ];

      struct
      {
        app::BattleUIWatchLowerDraw::PokeIconPosType  posType;
      } select_mons;
    };

  } LOCAL_ENTER_DATA_TABLE;

  const LOCAL_ENTER_DATA_TABLE  s_aButtonIdToEnterIdPatternMap[]  =
  {
    { app::BattleUIWatchLowerDraw::BUTTON_ID_BACK,                    RESULT_TYPE_BACK,       -1 },

    { app::BattleUIWatchLowerDraw::BUTTON_ID_POKE_ICON_LEFT_DOWN,     RESULT_TYPE_MONS_ICON,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_USER_NAME_LEFT_DOWN,     RESULT_TYPE_USER_INFO,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_SKILL_INFO_LEFT_DOWN,    RESULT_TYPE_SKILL_INFO, app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_DOWN },

    { app::BattleUIWatchLowerDraw::BUTTON_ID_POKE_ICON_RIGHT_DOWN,    RESULT_TYPE_MONS_ICON,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_USER_NAME_RIGHT_DOWN,    RESULT_TYPE_USER_INFO,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_SKILL_INFO_RIGHT_DOWN,   RESULT_TYPE_SKILL_INFO, app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_DOWN },

    { app::BattleUIWatchLowerDraw::BUTTON_ID_POKE_ICON_LEFT_UP,     RESULT_TYPE_MONS_ICON,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_USER_NAME_LEFT_UP,     RESULT_TYPE_USER_INFO,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_SKILL_INFO_LEFT_UP,    RESULT_TYPE_SKILL_INFO, app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_LEFT_UP },

    { app::BattleUIWatchLowerDraw::BUTTON_ID_POKE_ICON_RIGHT_UP,    RESULT_TYPE_MONS_ICON,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_USER_NAME_RIGHT_UP,    RESULT_TYPE_USER_INFO,  app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP },
    { app::BattleUIWatchLowerDraw::BUTTON_ID_SKILL_INFO_RIGHT_UP,   RESULT_TYPE_SKILL_INFO, app::BattleUIWatchLowerDraw::POKE_ICON_POS_TYPE_RIGHT_UP },
  };

  u32 loopNum = GFL_NELEMS( s_aButtonIdToEnterIdPatternMap );
  for( u32 i = 0; i < loopNum; ++i )
  {
    if( s_aButtonIdToEnterIdPatternMap[ i ].buttonId == buttonId )
    {
      m_resultData.type = s_aButtonIdToEnterIdPatternMap[ i ].resultType;

      if( m_resultData.type == RESULT_TYPE_MONS_ICON )
      {
        //  ポケモンデータを取得
        m_resultData.data.mons.pPokeData  = &m_pDraw->GetPokeData( (app::BattleUIWatchLowerDraw::PokeIconPosType)s_aButtonIdToEnterIdPatternMap[ i ].select_mons.posType );
        GFL_ASSERT( m_resultData.data.mons.pPokeData->p_btl_poke_param );
      }
      else if( m_resultData.type == RESULT_TYPE_USER_INFO)
      {
        //  プレイヤー情報を取得
      }
      else if( m_resultData.type == RESULT_TYPE_SKILL_INFO )
      {
        //  わざ表示対象のポケモンを取得
        m_resultData.data.mons.pPokeData  = &m_pDraw->GetPokeData( (app::BattleUIWatchLowerDraw::PokeIconPosType)s_aButtonIdToEnterIdPatternMap[ i ].select_mons.posType );
        GFL_ASSERT( m_resultData.data.mons.pPokeData->p_btl_poke_param );
      }

      SetState( CoreImp::STATE_CORE_TYPE_ENTER_EVENT );

      return  true;
    }
  }

  return  false;
}

/// キー入力通知用リスナ(入力制御はクラス側にまかせているので、処理を変更する時は参照するクラスを差し替えるだけでOK)
bool BattleUIWatchFrame::CoreImp::OnKeyAction( app::BattleUIWatchLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  //  更新以外では入力は受け付けない
  if( m_state != STATE_CORE_TYPE_UPDATE )
  {
    return  false;
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_pInput->PushBtnBack();
    return  true;
  }

  return  false;
}

GFL_NAMESPACE_END( btl )
