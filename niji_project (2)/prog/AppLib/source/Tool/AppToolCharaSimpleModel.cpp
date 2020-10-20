//======================================================================
/**
 * @file AppToolCharaSimpleModel.cpp
 * @date 2015/09/01 20:23:31
 * @author uchida_yuto
 * @brief キャラのモデル表示を簡易版
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>

//  nijiのインクルード
#include "System/include/nijiAllocator.h"
#include "System/include/GflUse.h"

//  キャラモデルのid
#include <niji_conv_header/battle/chara/chara_model_id.h>

// キャラモデルのモーションのインデックス
#include <niji_conv_header/battle/chara/p1_base.h>
#include <niji_conv_header/battle/chara/p2_base.h>

#include <System/include/PokemonVersion.h>

#include <debug/include/gfl2_DebugTick.h>

// garc
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------
enum
{
  CLACTMMODEL_CHARA_TICK_PRINT_WAIT_PARTS_ASYNC,
  CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DESTROY,
  CLACTMMODEL_CHARA_TICK_PRINT_MODEL_CREATE,
  CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DRESS_UP_CREATE,
  CLACTMMODEL_CHARA_TICK_PRINT_MAX,
};

GFL_DEBUG_TICK_SETUP(CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MAX );

#if PM_DEBUG
  
#define CLSIMPLE_CHARA_MODEL_TICK_DUMP (0)
  
#else

#define CLSIMPLE_CHARA_MODEL_TICK_DUMP (0)

#endif

  //  性別からバトルのキャラid取得
  u32 CharaSimpleModel::GetCharaBattleCharaId( const u32 sex )
  {
    if( sex == PM_MALE )
    {
      return  CHARA_MODEL_ID_P1_BASE;
    }

    return  CHARA_MODEL_ID_P2_BASE;
  }

  //  性別からバトルのキャラの待機アニメid取得
  u32 CharaSimpleModel::GetCharaBattltAnimeWaitId( const u32 sex )
  {
    if( sex == PM_MALE )
    {
      return  P1_BASE_BA005_WAIT01;
    }

    return  P2_BASE_BA005_WAIT01;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief コンストラクタ
    */
  //-------------------------------------------------------------------------
  CharaSimpleModel::CharaSimpleModel()
  {
    _Clear();

    GFL_DEBUG_TICK_SET_FLAG( CLACTMMODEL_CHARA, CLSIMPLE_CHARA_MODEL_TICK_DUMP );
  }

  //-------------------------------------------------------------------------
  /**
    * @brief デストラクタ
    */
  //-------------------------------------------------------------------------
  CharaSimpleModel::~CharaSimpleModel()
  {
    if( m_bCreateModelRes == true )
    {
      GFL_SAFE_DELETE( m_setupCreateModelParam.pCharaModelFactory );
      GFL_SAFE_DELETE( m_setupCreateModelParam.pDressUpModelResourceManager );

      GFL_DELETE_HEAP( m_pModelFactoryHeap );
      GFL_DELETE_HEAP( m_pModelResourceManagerHeap );
    }

    GFL_SAFE_DELETE( m_pModelAllocator );
    GFL_DELETE_HEAP( m_pAllocateHeap );
    GFL_DELETE_HEAP( m_pHeapBase );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      セットアップ
  * @param[in]  rSetupParam:  セットアップに必要なパラメータ
  * @param[in]  pFileManager: モデルロード時のファイル管理
  * @param[in]  modelArcId:   キャラモデルのarcID
  * @note
  *   モデル生成に必要なパラメータは内部で確保
  */
  //--------------------------------------------------------------------------------------------
  void CharaSimpleModel::Setup( SETUP_PARAM& rSetupParam, gfl2::fs::AsyncFileManager* pFileManager, const gfl2::fs::ArcFile::ARCID modelArcId, s32* dressupArcIdList )
  {
    GFL_ASSERT( pFileManager );

    if( _SetupBase( rSetupParam ) == false )
    {
      return;
    }

    NAGI_PRINT("useFullHeapSize : %d\n", USE_FULL_HEAP_SIZE);

    m_arcId = modelArcId;

    m_setupCreateModelParam.pCharaModelFactory  = GFL_NEW( m_pHeapBase ) poke_3d::model::CharaModelFactory();
    m_pModelFactoryHeap = GFL_CREATE_LOCAL_HEAP(  m_setupParam.pUseHeap, ONE_MODEL_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

    m_setupCreateModelParam.pCharaModelFactory->Initialize( pFileManager, m_pModelFactoryHeap, modelArcId );

    m_setupCreateModelParam.pDressUpModelResourceManager = GFL_NEW( m_pHeapBase ) poke_3d::model::DressUpModelResourceManager();
    m_pModelResourceManagerHeap = GFL_CREATE_LOCAL_HEAP(  m_setupParam.pUseHeap, ONE_MODEL_DRESS_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

    m_setupCreateModelParam.pDressUpModelResourceManager->Initialize( pFileManager, m_pModelResourceManagerHeap, dressupArcIdList );

    m_bCreateModelRes = true;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      セットアップ
  * @param[in]  rSetupParam:            セットアップに必要なパラメータ
  * @param[in]  rSetupCreateModelParam: モデル生成の外部設定パラメータ
  * @note
  *   この関数はモデル生成に必要なパラメータを外部設定用に用意
  */
  //--------------------------------------------------------------------------------------------
  void CharaSimpleModel::Setup( SETUP_PARAM& rSetupParam, SETUP_CREATE_MODEL_PARAM& rSetupCreateModelParam )
  {
    if( _SetupBase( rSetupParam ) == false )
    {
      return;
    }

    m_setupCreateModelParam = rSetupCreateModelParam;

    //  外部で作成したリソースを使う
    m_bCreateModelRes = false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      ロード
  */
  //--------------------------------------------------------------------------------------------
  void CharaSimpleModel::StartLoad( const s32 charaId, const poke_3d::model::DressUpParam& rDressUpParam )
  {
    //  もう破棄指定しているのでロードできない
    if( m_bTerminate == true )
    {
      GFL_ASSERT( 0 );
      return;
    }

    GFL_ASSERT( charaId != CHARA_ID_NULL );

    //  現在参照しているのと違うのをロードする
    //  あくまでロードするnoを設定してロードするステートの遷移はUpdate側で行う
    m_loadCharId        = charaId;
    m_loadDressUpParam  = rDressUpParam;

    m_bLoad = true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::Terminate()
  {
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
  bool CharaSimpleModel::IsDelete()
  {
    //  内部で更新をする
    //  １フレームで連続で呼んでも問題ないので使う側が更新を呼んでも大丈夫だし
    //  呼ばなくても大丈夫
    Update();

    //  終了ステートでモデルが破棄されているのであれば破棄可能
    if( m_bTerminate == true )
    {
      if( m_seqDelete == 0 )
      {
        if( m_state == STATE_TYPE_IDLE )
        {
          ++m_seqDelete;
        }
      }
      else
      if( m_seqDelete == 1 )
      {
        if( m_bCreateModelRes == true )
        {
          m_setupCreateModelParam.pCharaModelFactory->Finalize();
          m_setupCreateModelParam.pDressUpModelResourceManager->Finalize();

          ++m_seqDelete;
        }
        else
        {
          ++m_seqDelete;
        }
      }
      else
      if( m_seqDelete == 2 )
      {
        return  true;
        
      }
    }

    return  false;
  }

  //-------------------------------------------------------------------------
  /**
    * @brief 更新処理
    */
  //-------------------------------------------------------------------------
  void CharaSimpleModel::Update()
  {
    if( m_state == STATE_TYPE_IDLE )
    {
      //  待機中
      if( m_bTerminate == false )
      {
        //  このステートでないとロードステートに遷移しない
        if( m_loadCharId != CHARA_ID_NULL )
        {
          //  ロード予約に保存していたデータをロードデータに移行
          m_charaId       = m_loadCharId;
          m_dressUpParam  = m_loadDressUpParam;

          _SetState( STATE_TYPE_LOAD );

          m_loadCharId  = CHARA_ID_NULL;
        }
      }
    }

    if( m_state == STATE_TYPE_LOAD )
    {
      enum
      {
        SEQ_LOAD_FILE_LOAD  = 0,
        SEQ_LOAD_FILE_WAIT,
        SEQ_LOAD_DRESS_UP_LOAD,
        SEQ_LOAD_DRESS_UP_LOAD_WAIT,
        SEQ_LOAD_CREATE_MODEL,
        SEQ_LOAD_CREATE_DRESS_UP,
        SEQ_LOAD_SUCCESS,
      };

      //  ファイルロード
      if( m_subStep == SEQ_LOAD_FILE_LOAD )
      {
        // モデル作成
        // リソースを非同期読み込みする
        // niji_conv_header/chara/chara_model_id.hをインクルードしておいてください
        if( m_bUnloadCharaModel == true )
        {
          m_setupCreateModelParam.pCharaModelFactory->LoadModelAsync( m_charaId );
          m_bUnloadCharaModel = false;
        }

        //  ドレスパーツ作成
        if( m_bUnloadParts == true )
        {
          m_setupCreateModelParam.pDressUpModelResourceManager->LoadDressUpPartsAsync( m_dressUpParam );
          m_bUnloadParts  = false;
        }

        ++m_subStep;
      }
      
      //  ファイルロード待ち
      if( m_subStep == SEQ_LOAD_FILE_WAIT )
      {
        bool  bSuccess  = true;

        // リソースの非同期読み込み終了確認待ち
        if( m_bUnloadCharaModel == false )
        {
          if( m_setupCreateModelParam.pCharaModelFactory->IsModelLoaded( m_charaId ) == false )
          {
            bSuccess  = false;
          }
        }

        // 着せ替えパーツの非同期読み込み終了確認待ち
        if( m_bUnloadParts == false )
        {
          if( m_setupCreateModelParam.pDressUpModelResourceManager->IsDressUpPartsLoaded( m_dressUpParam ) == false )
          {
            bSuccess  = false;
          }
        }

        if( bSuccess == true )
        {
          ++m_subStep;
        }
      }
      
      //  きせかえデータ構築非同期開始
      if( m_subStep == SEQ_LOAD_DRESS_UP_LOAD )
      {
        // 読み込んだ着せ替えパーツをセットアップ開始(非同期)
        m_setupCreateModelParam.pDressUpModelResourceManager->SetupDressUpPartsAsync( m_pModelAllocator, m_dressUpParam );

        ++m_subStep;
      }
      else
      //  きせかえデータ非同期待ち
      if( m_subStep == SEQ_LOAD_DRESS_UP_LOAD_WAIT )
      {
GFL_DEBUG_TICK_START( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_WAIT_PARTS_ASYNC, "WaitSetupDressUpPartsAsync" );
        // 読み込んだ着せ替えパーツをセットアップ待ち(非同期)
        if( m_setupCreateModelParam.pDressUpModelResourceManager->WaitSetupDressUpPartsAsync( m_pModelAllocator, m_dressUpParam ) == true )
        {
          ++m_subStep;
        }
GFL_DEBUG_TICK_END( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_WAIT_PARTS_ASYNC );
#if CLSIMPLE_CHARA_MODEL_TICK_DUMP 
  GFL_PRINT( "WaitSetupDressUpPartsAsync %d us\n", GFL_DEBUG_TICK_US_GET( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_WAIT_PARTS_ASYNC ) );
#endif
      }
      else
      if( m_subStep == SEQ_LOAD_CREATE_MODEL )
      {
GFL_DEBUG_TICK_START( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_CREATE, "CreateModel" );

        //  モデルのセットアップ
        {
          // 読み込んだモデルをセットアップ
          if( m_setupCreateModelParam.pCharaModelFactory->SetupModel( m_pModelAllocator, m_charaId ) == false )
          {
            //  ありえない
            GFL_ASSERT_STOP( 0 );
          }
        }

GFL_DEBUG_TICK_END( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_CREATE );
#if CLSIMPLE_CHARA_MODEL_TICK_DUMP 
  GFL_PRINT( "CreateModel %d us\n", GFL_DEBUG_TICK_US_GET( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_CREATE ) );
#endif
        ++m_subStep;
      }
      else
      if( m_subStep == SEQ_LOAD_CREATE_DRESS_UP )
      {
GFL_DEBUG_TICK_START( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DRESS_UP_CREATE, "CreateDressUpModel" );
        // 読み込んだリソースから着せ替えモデルを作成
        if( m_setupCreateModelParam.pCharaModelFactory->CreateDressUpModel( m_pModelAllocator, m_pHeapBase, &m_dressUpModel, m_setupCreateModelParam.pDressUpModelResourceManager, m_charaId ) == false )
        {
          //  ありえない
          GFL_ASSERT_STOP( 0 );
        }
GFL_DEBUG_TICK_END( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DRESS_UP_CREATE );
#if CLSIMPLE_CHARA_MODEL_TICK_DUMP 
  GFL_PRINT( "CreateDressUpModel %d us\n", GFL_DEBUG_TICK_US_GET( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DRESS_UP_CREATE ) );
#endif

        ++m_subStep;
      }

      if( m_subStep == SEQ_LOAD_SUCCESS )
      {
        m_bDestroyModel = false;

        //  レンダーに登録
        {
          u32 loopNum = GFL_NELEMS( m_setupParam.aRenderPlaceType );
          for( u32 i = 0; i < loopNum; ++i )
          {
            if( m_setupParam.aRenderPlaceType[ i ] < app::util::AppRenderingManager::RenderPlaceType::NUM )
            {
              m_setupParam.pRenderingManager->AddDressUpModel( m_setupParam.aRenderPlaceType[ i ], &m_dressUpModel, m_setupParam.layerNo[ i ] );
            }
          }
          m_bRemoveModel  = false;
        }

        if( m_loadCharId != CHARA_ID_NULL )
        {
          //  ロードするCharNoがあるなら再度ロードするために一度破棄ステートに遷移してからidleステートに遷移してロード
          _SetState( STATE_TYPE_END );
        }
        else
        if( m_bTerminate == true )
        {
          //  ロード中に破棄指定しているのであらば破棄ステートに移行
          _SetState( STATE_TYPE_END );
        }
        else
        //  モデル作成してAppRenderingに描画登録
        {
          _SetState( STATE_TYPE_UPDATE );

          //  モデルのパラメータ設定
          SetVisible( m_bVisible );

          if( m_eyeIndex != poke_3d::model::CharaModel::EYE_INDEX_NONE )
          {
            SetEye( m_eyeIndex );
          }

          SetRot( m_rot );
          SetPos( m_pos );
          SetStopAnime( m_bAnimeStop );

          if( m_bAutoEyeAnime == true )
          {
            PlayEyeAnime();
          }

          //  あらかじめアニメデータを確保する
          {
            //  待機アニメはあらかじめ確保しておく
            m_dressUpModel.AllocateAnimation( m_pHeapBase, 1, GetCharaBattltAnimeWaitId( m_dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ] ) );
          }

          //  設定が保管している場合はアニメ設定
          if( m_motionAnimeId != ANIME_ID_NULL )
          {
            _SetMotionAnimeFrame( m_motionAnimeId, m_motionAnimeFrameNum, m_bAnimeLoop );
          }
        }
      }
    }

    if( m_state == STATE_TYPE_UPDATE )
    {
      if( m_loadCharId != CHARA_ID_NULL )
      {
        //  ロードするポケモンNoがあるなら再度ロードするために破棄ステートに遷移して待機ステートに
        _SetState( STATE_TYPE_END );
      }
      else
      {
        // アニメーション更新
        if( m_bAnimeStop == false )
        {
          m_dressUpModel.UpdateEye();
          m_dressUpModel.UpdateMouth();

          m_dressUpModel.UpdateAnimation();
        }

        //  座標などのパラメータとモデルと同期
        m_pos = m_dressUpModel.GetPosition();
        m_rot = m_dressUpModel.GetRotation();
      }
    }

    if( m_state == STATE_TYPE_END )
    {
      enum
      {
        SEQ_END_REMOVE_RENDER = 0,
        SEQ_END_MODEL_DELETE,
        SEQ_END_UNLOAD_DRESS,
        SEQ_END_UNLOAD_MODEL,
        SEQ_END
      };

      if( m_subStep == SEQ_END_REMOVE_RENDER )
      {
        //  レンダーに登録解除
        if( m_bRemoveModel == false )
        {
          u32 loopNum = GFL_NELEMS( m_setupParam.aRenderPlaceType );
          for( u32 i = 0; i < loopNum; ++i )
          {
            if( m_setupParam.aRenderPlaceType[ i ] < app::util::AppRenderingManager::RenderPlaceType::NUM )
            {
              m_setupParam.pRenderingManager->RemoveDressUpModel( m_setupParam.aRenderPlaceType[ i ], &m_dressUpModel, m_setupParam.layerNo[ i ] );
            }
          }
          m_delCnt  = 0;

          m_bRemoveModel  = true;
          ++m_subStep;
        }
        else
        {
          ++m_subStep;
        }
      }
      else
      if( m_subStep == SEQ_END_MODEL_DELETE )
      {
        if( m_bDestroyModel == false )
        {
          if( m_dressUpModel.CanDestroy() == true )
          {
GFL_DEBUG_TICK_START( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DESTROY, "m_dressUpModel.Destroy" );
            m_dressUpModel.Destroy();
GFL_DEBUG_TICK_END( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DESTROY );
#if CLSIMPLE_CHARA_MODEL_TICK_DUMP 
  GFL_PRINT( "m_dressUpModel.Destroy %d us\n", GFL_DEBUG_TICK_US_GET( CLACTMMODEL_CHARA, CLACTMMODEL_CHARA_TICK_PRINT_MODEL_DESTROY ) );
#endif

            m_bDestroyModel = true;
            ++m_subStep;

          }
        }
        else
        {
          ++m_subStep;
        }
      }
      else
      if( m_subStep == SEQ_END_UNLOAD_DRESS )
      {
        if( m_bUnloadParts == false )
        {
          if( m_setupCreateModelParam.pDressUpModelResourceManager->CanUnloadDressUpParts( m_dressUpParam ) == true )
          {
            m_setupCreateModelParam.pDressUpModelResourceManager->UnloadDressUpParts( m_dressUpParam );

            ++m_subStep;

            m_bUnloadParts  = true;
          }
        }
        else
        {
          ++m_subStep;
        }
      }
      else
      if( m_subStep == SEQ_END_UNLOAD_MODEL )
      {
        if( m_bUnloadCharaModel == false )
        {
          bool  bUnload = true;
          if( m_bTerminate == false )  // @fix GFNMCat[5764] 高級ブティックの買い物メニューをBボタンで終了しようとして暗転のまま開けず進行不能   →終了させるときはアンロードに処理がいくようにしておく
          {
            if( m_loadCharId != CHARA_ID_NULL )
            {
              if( m_loadCharId == m_charaId )
              {
                //  同じキャラをロードするなら開放はしない
                bUnload = false;
              }
            }
          }

          if( bUnload == true )
          {
            // 読み込んだリソースの解放ができるか確認
            if( m_setupCreateModelParam.pCharaModelFactory->CanUnloadModel( m_charaId ) == true )
            {
              // 読み込んだリソースの開放
              m_setupCreateModelParam.pCharaModelFactory->UnloadModel( m_charaId );

              m_bUnloadCharaModel = true;
              ++m_subStep;
            }
          }
          else
          {
            ++m_subStep;
          }
        }
        else
        {
          //  すでにアンロードしている
          ++m_subStep;
        }
      }
        
      if( m_subStep == SEQ_END )
      {
        if( m_bTerminate == true )
        {
          //  すでに終了しているので
          //  まだ開放していないのがあればもう一度開放(開放がすべて終るまで繰り返す)
          if( ( m_bUnloadCharaModel == false )
          ||  ( m_bDestroyModel == false )
          ||  ( m_bRemoveModel == false )
          ||  ( m_bUnloadParts == false ) )
          {
            m_subStep = SEQ_END_REMOVE_RENDER;
          }
          else
          {
            //  待機ステートに
            _SetState( STATE_TYPE_IDLE );
          }
        }
        else
        {
          //  ここにきたら再ロード待機するが、レンダーの登録とモデルの破棄、パーツの破棄をしていない場合は再解放に戻る
          if( ( m_bDestroyModel == false )
          ||  ( m_bRemoveModel == false )
          ||  ( m_bUnloadParts == false ) )
          {
            m_subStep = SEQ_END_REMOVE_RENDER;
          }
          else
          {
            //  待機ステートに
            _SetState( STATE_TYPE_IDLE );
          }
        }
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief    ステート取得
    * @return   現在のステート
    */
  //------------------------------------------------------------------
  CharaSimpleModel::StateType CharaSimpleModel::GetState() const { return m_state; }

  //------------------------------------------------------------------
  /**
    * @brief   破棄させる
    * @note
    *         Terminate()のように完全破棄ではなくメモリなど一部破棄する
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::RequestFree()
  {
    //  メモリなどが読まれている状態であれば開放処理に遷移
    if( m_state == STATE_TYPE_UPDATE )
    {
      _SetState( STATE_TYPE_END );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   アニメ再生停止
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetStopAnime( const bool bFlg )
  {
    if( IsReady() == true )
    {
      if( bFlg == true )
      {
        m_dressUpModel.SetAnimationStepFrame( 0.0f );
      }
      else
      {
        m_dressUpModel.SetAnimationStepFrame( 1.0f );
      }
    }

    m_bAnimeStop  = bFlg;
  }

  //------------------------------------------------------------------
  /**
    * @brief   モーションアニメ再生設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetMotionAnime( const s32 animeId, const u32 frameNum, const bool bLoop )
  {
    if( IsReady() == true )
    {
      _SetMotionAnimeFrame( animeId, frameNum, bLoop );
    }

    //  設定を保管して、ロード終了時に設定
    m_motionAnimeId           = animeId;
    m_motionAnimeFrameNum     = frameNum;
    m_bAnimeLoop              = bLoop;
  }

  //------------------------------------------------------------------
  /**
    * @brief   目パチ設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetEye( const poke_3d::model::CharaModel::EyeIndex eyeIndex )
  {
    if( IsReady() == true )
    {
      m_dressUpModel.SetEyeIndex( eyeIndex );

      // @fix GFNMCat[2825] 目パチ更新をしないと即時反映しない
      m_dressUpModel.UpdateEye();
    }

    m_eyeIndex  = eyeIndex;
  }

  //------------------------------------------------------------------
  /**
    * @brief   目パチアニメ再生
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::PlayEyeAnime()
  {
    if( IsReady() == true )
    {
      _PlayEyeAnime();
    }

    m_bAutoEyeAnime = true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   モーションアニメid取得
    */
  //------------------------------------------------------------------
  u32 CharaSimpleModel::GetMotionAnimeId()
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_dressUpModel.GetAnimationId();
  }

  //------------------------------------------------------------------
  /**
    * @brief   モーションアニメフレーム取得
    */
  //------------------------------------------------------------------
  u32 CharaSimpleModel::GetNowMotionAnimeFrame()
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    return  m_dressUpModel.GetAnimationFrame();
  }

  //------------------------------------------------------------------
  /**
    * @brief   現在の目パチ取得
    */
  //------------------------------------------------------------------
  poke_3d::model::CharaModel::EyeIndex CharaSimpleModel::GetEye()
  {
    if( IsReady() == true )
    {
      return  m_dressUpModel.GetEyeIndex();
    }

    return  poke_3d::model::CharaModel::EYE_INDEX_NONE;
  }

  //------------------------------------------------------------------
  /**
    * @brief   表示有効設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetVisible( const bool bFlg )
  {
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_dressUpModel.SetVisible( bFlg );
    }

    m_bVisible  = bFlg;
  }

  //  使える準備ができているかどうか
  bool CharaSimpleModel::IsReady()
  {
    //  更新ステート以外の時は更新処理を呼べるようにする
    //  ロードリクエストして更新処理を呼ばずに IsReady()関数を呼び出すケースに対処するため
    //  更新ステートの状態では使う側で Update()関数を呼んでもらう
    if( m_state != STATE_TYPE_UPDATE )
    {
      Update();
    }

    //  ロード中かどうか
    if( m_bLoad == true )
    {
      return  false;
    }

    return  ( STATE_TYPE_UPDATE == m_state );
  }

  //------------------------------------------------------------------
  /**
    * @brief   ロードしようとしているキャラIdを取得
    * @note
    *         CHARA_ID_NULL値以外が入っているとロードの準備待ちと考える
    */
  //------------------------------------------------------------------
  u32 CharaSimpleModel::GetLoadCharaId() const { return m_loadCharId; }

  //  基本のモデルクラスを取得
  poke_3d::model::BaseModel& CharaSimpleModel::GetBaseModel()
  {
    return m_dressUpModel;
  }

  //------------------------------------------------------------------
  /**
    * @brief   座標設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetPos( const gfl2::math::Vector3& rPos )
  {
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_dressUpModel.SetPosition( rPos );
    }

    m_pos = rPos;
  }

  //------------------------------------------------------------------
  /**
    * @brief   回転のラジアン設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::SetRot( const gfl2::math::Vector3& rRot )
  {
    if( m_state == STATE_TYPE_UPDATE )
    {
      m_dressUpModel.SetRotation( rRot );
    }

    m_rot = rRot;
  }

  //------------------------------------------------------------------
  /**
    * @brief   きせかえのリソースコア取得
    */
  //------------------------------------------------------------------
  poke_3d::model::DressUpModelResourceManagerCore*  CharaSimpleModel::GetDressUpModelResourceManagerCore( const poke_3d::model::DressUpParam::Sex sex )
  {
    poke_3d::model::DressUpModelResourceManagerCore*  pCore = m_setupCreateModelParam.pDressUpModelResourceManager->GetDressUpModelResourceManagerCore( sex );
    if( pCore == NULL )
    {
      return  NULL;
    }

    return  pCore;
  }

  //------------------------------------------------------------------
  /**
    * @brief   きせかえの組み合わせ可能かチェック
    */
  //------------------------------------------------------------------
  bool CharaSimpleModel::CheckDressUpCombination( const poke_3d::model::DressUpParam& rDressUpParam )
  {
    if( m_setupCreateModelParam.pDressUpModelResourceManager == NULL )
    {
      return  true;
    }

    //  組み合わせできるか取得
    poke_3d::model::DressUpParam::Sex sex = (poke_3d::model::DressUpParam::Sex)rDressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ];
    poke_3d::model::DressUpModelResourceManagerCore*  pCore = GetDressUpModelResourceManagerCore( sex );
    if( pCore == NULL )
    {
      return  true;
    }

    return pCore->CheckDressUpCombination( rDressUpParam );
  }

  //------------------------------------------------------------------
  /**
    * @brief   着せ替えの組み合わせで問題ないデータを出力
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::OutPutDressUpCombination( poke_3d::model::DressUpParam* pDressUpParam )
  {
    if( pDressUpParam == NULL )
    {
      return;
    }

    if( CheckDressUpCombination( *pDressUpParam ) == true )
    {
      //  そのままきせることができる
      return;
    }

    //  そのままきせかえができないので着せ替える可能な設定に修正して出力

  }

  //------------------------------------------------------------------
  /**
    * @brief   共通セットアップ
    */
  //------------------------------------------------------------------
  bool CharaSimpleModel::_SetupBase( SETUP_PARAM& rSetupParam )
  {
    GFL_ASSERT( rSetupParam.pUseHeap );
    GFL_ASSERT( rSetupParam.pRenderingManager );

    //  一度しか呼べない
    GFL_ASSERT( m_pHeapBase == NULL );
    if( m_pHeapBase != NULL )
    {
      return  false;
    }

    //  専用のヒープ作成（モデルの再ロードなどを行うのでこのクラス内のみで使うモデルヒープを作成）
    {
      m_pHeapBase = GFL_CREATE_LOCAL_HEAP( rSetupParam.pUseHeap, USE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
      m_pAllocateHeap = GFL_CREATE_LOCAL_HEAP( m_pHeapBase, USE_ALLOCATE_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
    }

    //  モデル用のアロケータを作成
    m_pModelAllocator = GFL_NEW( m_pHeapBase ) gfl2::util::GLHeapAllocator( m_pAllocateHeap );

    m_setupParam  = rSetupParam;

    return  true;
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::_SetState( const StateType newStateType )
  {
    m_state   = newStateType;
    m_subStep = 0;

    if( m_state == STATE_TYPE_UPDATE )
    {
      m_bLoad = false;
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   モーションアニメフレーム設定
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::_SetMotionAnimeFrame( const s32 animeId, const u32 frameNum, const bool bLoop )
  {
    if( IsReady() == false )
    {
      GFL_ASSERT( 0 );
      return;
    }

    // アニメーションを変更する。アニメーションのIDは
    // niji_conv_header/chara/以下に出力されるヘッダーに定義してあるのでインクルードしておいてください
    m_dressUpModel.ChangeAnimation( animeId );

    //  初回フレーム設定
    m_dressUpModel.SetAnimationFrame( frameNum );

    if( bLoop == false )
    {
      // アニメーションデータのループフラグが設定されていなかったので強制的にループさせます。
      // 本来はデータに含まれているはずなので必要ありません
      m_dressUpModel.SetAnimationLoop( false );

      //  フレーム設定
      m_dressUpModel.SetAnimationStartFrame( frameNum );
      m_dressUpModel.SetAnimationEndFrame( frameNum );
    }
    else
    {
      m_dressUpModel.SetAnimationLoop( true );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   目パチアニメ再生
    */
  //------------------------------------------------------------------
  void CharaSimpleModel::_PlayEyeAnime()
  {
    poke_3d::model::CharaModel::AutoBlinkMode autoBlinkMode = poke_3d::model::CharaModel::AUTO_BLINK_MODE_BATTLE;
    m_dressUpModel.SetAutoBlinkMode( autoBlinkMode );

    // @fix NMCat[402] 目パチするには乱数関数設定が必要
    m_dressUpModel.SetupRandomFuc( System::GflUse::GetPublicRand );
  }

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
