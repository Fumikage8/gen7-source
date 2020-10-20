//======================================================================
/**
 * @file AppToolTrainerIconRendering.cpp
 * @date 2015/08/18 15:34:00
 * @author uchida_yuto
 * @brief プレイヤーアイコン2D表現処理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "AppLib/include/Tool/AppToolCharaTexPaneReplace.h"

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Tool/app_tool_LytTexData.h"
#include "AppLib/include/Tool/TimerIconView.h"
#include "AppLib/include/Tool/IconObject.h"
#include "Trainer/Trainer/include/TrainerTypeData.h"
#include "System/include/nijiAllocator.h"

// モデルインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>

//  timerレイアウトのインクルード
#include "niji_conv_header/app/timer/res2d/timerIcon.h"
#include "niji_conv_header/app/timer/res2d/timerIcon_anim_list.h"
#include "niji_conv_header/app/timer/res2d/timerIcon_pane.h"
#include <arc_index/timerIcon.gaix>

//  trainer_iconテクスチャのインクルード
#include <arc_index/trainer_icon.gaix>

// キャラモデルのモーションのインデックス
#include <niji_conv_header/battle/chara/p1_base.h>
#include <niji_conv_header/battle/chara/p2_base.h>

// garc
#include "System/include/ArcIdGetter.h"
#include "System/include/DressUp.h"
#include <arc_def_index/arc_def.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//  ローカルの定義
GFL_NAMESPACE_BEGIN(localPlayerIconManager)

    //  主人公の男女
    static const u32 s_trainerIdHero      = trainer::TRTYPE_HERO;
    static const u32 s_trainerIdHeroine   = trainer::TRTYPE_HEROINE;

    static const f32 s_camDefaultZPos = 1000.0f;

    //  2Dトレーナーテクスチャのロードデータ
    struct TRAINER_TEX_LOAD_DATA
    {
      //  トレーナーid
      u32 trainerId;
      //  ロードする2dのarcid
      gfl2::fs::ArcFile::ARCDATID texId;
    };

    //  モデルレンダーテクスチャのロードデータ
    struct TRAINER_MODEL_RENDER_LOAD_DATA
    {
      //  トレーナーid
      u32 trainerId;

      //  モデルid
      u32 charalId;

      //  待機アニメid
      u32 waitMotionAnimeId;
    };

    //  トレーナモデルレンダーテクスチャのロードテーブル
    static const TRAINER_MODEL_RENDER_LOAD_DATA s_aTrainerModelRenderLoadDataTable[]  =
    {
      { s_trainerIdHero, CHARA_MODEL_ID_P1_BASE, P1_BASE_BA005_WAIT01 },
      { s_trainerIdHeroine, CHARA_MODEL_ID_P2_BASE, P2_BASE_BA005_WAIT01 },
    };

    //  アイコンオブジェクトクラス
    class TrainerIconObject
    {
      GFL_FORBID_COPY_AND_ASSIGN(TrainerIconObject);

    public:
      enum
      {
        LYT_TEX_IMG_WIDTH_SIZE    = 64,
        LYT_TEX_IMG_HEGHT_SIZE    = 64,
      };

      enum StateType
      {
        //  何もしていない状態
        STATE_TYPE_IDLE = 0,
        //  モデルテクスチャ貼り付け指示待ち
        STATA_TYPE_TEX_MODEL_REPLACE_START_WAIT,
        //  モデルテクスチャ貼り付け中
        STATA_TYPE_TEX_MODEL_REPLACE_RUN
      };

      //  使用するヒープサイズ取得
      static u32 GetUseSystemHeapSize()
      {
        return 1024 * 50;
      }

      static u32 GetUseDeviceHeapSize()
      {
        return ( 1024 * 30 ) + IconObject::GetUseHeapSize( LYT_TEX_IMG_WIDTH_SIZE, LYT_TEX_IMG_HEGHT_SIZE, true );
      }

      TrainerIconObject()
      {
        _Clear();
      }

      ~TrainerIconObject()
      {
        GFL_SAFE_DELETE( m_pIconObject );
        GFL_SAFE_DELETE( m_pFaceIconLayoutFileLoadState );
        GFL_SAFE_DELETE( m_pTimerIconUIView );
        GFL_SAFE_DELETE( m_pCharaModelTexPaneReplace );
      }

      //  初期化
      void Initialize( 
      app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap,
      gfl2::fs::AsyncFileManager* pFileManager, System::nijiAllocator* pVramAllocator, void* pTimerIconLytBuff,
      poke_3d::model::CharaModelFactory* pCharaModelFacltory, poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager,  app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType, const bool bNPCOnly )
      {
        GFL_ASSERT( pTimerIconLytBuff );

        if( m_pCharaModelTexPaneReplace != NULL )
        {
          return;
        }

        m_pUseUtilHeap  = pHeap;

        //  NPCのみの場合はモデルレンダーは貼り付けは不要
        if( bNPCOnly == false )
        {
          //  モデルのレンダー貼り付け制御作成
          m_pCharaModelTexPaneReplace = GFL_NEW( m_pUseUtilHeap->GetDeviceHeap() ) app::tool::AppToolCharaTexPaneReplace( pRenderinManager, m_pUseUtilHeap, pCharaModelFacltory, pDressUpModelResourceManager, pVramAllocator );
        }

        //  タイマーアイコンView作成
        m_pTimerIconUIView  = GFL_NEW( m_pUseUtilHeap->GetDeviceHeap() ) TimerIconUIView( m_pUseUtilHeap, pRenderinManager, pTimerIconLytBuff, displayType ) ;

        //  トレーナー顔アイコンのテクスチャファイル読み込み開始
        m_pFaceIconLayoutFileLoadState  =  GFL_NEW( m_pUseUtilHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( NIJI_ARCID_NUM( ARCID_TRAINER_ICON_DATA ), m_pUseUtilHeap->GetDeviceHeap(), true, 128 );
        m_pFaceIconLayoutFileLoadState->Start();

        //  2Dアイコンオブジェクト
        {
          IconObject::INIT_PARAM  initParam;
          initParam.arcId = NIJI_ARCID_NUM( ARCID_TRAINER_ICON_DATA );
          initParam.bCompFile = true;
          initParam.sizeHeight  = LYT_TEX_IMG_WIDTH_SIZE;
          initParam.sizeWidth   = LYT_TEX_IMG_HEGHT_SIZE;
          m_pIconObject = GFL_NEW( m_pUseUtilHeap->GetDeviceHeap() ) IconObject( initParam, m_pUseUtilHeap->GetDeviceHeap() );
        }
      }

      //  終了通知
      void Terminate()
      {
        m_pTimerIconUIView->RemovePane();

        m_pFaceIconLayoutFileLoadState->End();
        if( m_pCharaModelTexPaneReplace != NULL )
        {
          m_pCharaModelTexPaneReplace->Terminate();
        }

        m_terminateSeq  = 0;
      }

      bool IsReady()
      {
        bool  bRet  = true;
        if( m_pIconObject->InitFunc() == false )
        {
          bRet  = false;
        }

        m_pFaceIconLayoutFileLoadState->Update();
        if( m_pFaceIconLayoutFileLoadState->IsManualIdleState() == false )
        {
          bRet  = false;
        }

        return  bRet;
      }

      bool IsDelete()
      {
        if( m_terminateSeq == 0 )
        {
          bool  bNextSeq  = false;
          if( m_pIconObject->EndFunc() == true )
          {
            bNextSeq  = true;
          }

          if( bNextSeq == true )
          {
            ++m_terminateSeq;
          }
        }
        else
        if( m_terminateSeq == 1 )
        {
          bool  bNextSeq  = true;

          m_pFaceIconLayoutFileLoadState->Update();
          if( m_pFaceIconLayoutFileLoadState->IsEnd() == false )
          {
            bNextSeq  = false;
          }

          if( m_pCharaModelTexPaneReplace != NULL )
          {
            if( m_pCharaModelTexPaneReplace->IsDelete() == false )
            {
              bNextSeq  = false;
            }
          }

          if( bNextSeq == true )
          {
            ++m_terminateSeq;
          }
        }
        else
        if( m_terminateSeq == 2 )
        {
          return  true;
        }

        return  false;
      }

      void SetData( const AppToolTrainerIconRendering::ICON_OBJECT_DATA& rData, const bool bVisibleTimerIcon )
      {
        GFL_ASSERT( rData.pReplacePane != NULL );

        m_bVisibleTimerIcon   = bVisibleTimerIcon;
        m_pOutputTexture      = NULL;

        // @fix GFNMCat[1943] 貼り付けが実行中の場合は貼り付けをできないように終了しても貼り付けないようにとめておく
        //                    こうしないと最新のテクスチャーの貼り付けにならない
        m_pIconObject->SetEnableReplaceTex( false );
        if( m_pCharaModelTexPaneReplace != NULL )
        {
          m_pCharaModelTexPaneReplace->SetEnableReplaceTex( false );
        }

        s32 modelLoadTableIdx = _GetModelLoadTableIdx( rData.trainerId );
        if( modelLoadTableIdx != -1 )
        {
          //  主人公であればモデル作成でのテクスチャ貼り付け処理を走らせる
          //  モデル読みなので１オブジェクトしか走らせないようにする
          m_loadType  = REQUEST_LOAD_TYPE_HERO;
          m_state     = STATA_TYPE_TEX_MODEL_REPLACE_START_WAIT;
        }
        else
        {
          //  主人公以外はポケモンアイコンのように2Dテクスチャを非同期にロードして貼り付ける
          m_loadType = REQUEST_LOAD_TYPE_OTHER;
          //  2Dのアイコンテクスチャーが返ってくる
          m_load2DTexId = trainer::TrainerTypeData::GetTrainerIconID( (trainer::TrType)rData.trainerId );
          m_2DTexLoadSeq  = 0;
        }

        m_data  = rData;

        //  ロード中にvisibleをfalseにするペインがあれば設定
        if( m_data.pInLoadVisibleChangePane != NULL )
        {
          m_data.pInLoadVisibleChangePane->SetVisible( false );
        }

        //  タイマーアイコンを出す
        //  まだタイマーアイコンが処理中かもしれないのでいったん終了させる
        m_pTimerIconUIView->EndAnime();
        m_pTimerIconUIView->StartAnime();
        m_pTimerIconUIView->RemovePane();

        if( ( m_bFixingTimerIconPos == false ) && ( m_bVisibleTimerIcon == true ) )
        {
          if( m_data.pTimerIconRootPane != NULL )
          {
            //  ペインの親子関係の設定
            gfl2::math::VEC3  pos( 0, 0, 0 );
            m_pTimerIconUIView->SetIconPos( pos );
            m_pTimerIconUIView->AttachPane( m_data.pTimerIconRootPane );
          }
        }

        _SyncPaneTimerIcon( m_data.pReplacePane, m_data.pSyncVisiblePane );

        //  貼り付けるテクスチャは非表示に
        m_data.pReplacePane->SetVisible( false );

        m_bLoaded = false;
      }

      //  タイマーアイコンの固定表示設定
      //  この設定をすると顔アイコンペイン位置と同期しない
      void SetTimerIconPos( const gfl2::math::Vector3& rPos )
      {
        m_fixingTimerIconPos  = rPos;
      }

      void SetEnableTimerIconFixPos( const bool bFlg )
      {
        m_bFixingTimerIconPos = bFlg;
      }

      //  テクスチャー出力設定
      void SetTexture( AppToolTrainerIconTexture* pTexture )
      {
        GFL_ASSERT( pTexture );
        m_pOutputTexture  = pTexture;
      }

      //  ロードして終了しているか
      bool IsLoaded() const 
      {
        return  m_bLoaded;
      }

      //  モデルの貼り付け処理が動いているか
      bool IsReplaceModelTexRun()
      {
        if( m_pCharaModelTexPaneReplace != NULL )
        {
          return  m_pCharaModelTexPaneReplace->IsReplaceRun();
        }

        return  false;
      }

      //  更新
      void Update()
      {
        //  ロード発生していない時はタイマーアイコンは非表示
        if( m_loadType == REQUEST_LOAD_TYPE_DONE )
        {
        }
        else
        {
          //  設定しているペインとの位置と同期させる
          _SyncPaneTimerIcon( m_data.pReplacePane, m_data.pSyncVisiblePane );
        }

        m_pIconObject->UpdateFunc();

        m_pFaceIconLayoutFileLoadState->Update();
        if( m_pCharaModelTexPaneReplace != NULL )
        {
          m_pCharaModelTexPaneReplace->Update();
        }

        //  ローディング中
        if( m_loadType != REQUEST_LOAD_TYPE_DONE )
        {
          bool  bLoadEnd  = false;

          //  モデルテクスチャーロードして貼り付け
          if( m_loadType == REQUEST_LOAD_TYPE_HERO )
          {
            //  モデルのレンダーテクスチャ貼り付け中
            if( m_state == STATA_TYPE_TEX_MODEL_REPLACE_RUN )
            {
              //  外部設定したペインワールド座標との同期
              if( m_data.pReplacePane != NULL )
              {
                if( m_pCharaModelTexPaneReplace != NULL )
                {
                  if( m_pCharaModelTexPaneReplace->IsReplacePane( m_data.pReplacePane ) == true )
                  {
                    if( m_pOutputTexture != NULL )
                    {
                      m_pOutputTexture->CreateTex( m_pCharaModelTexPaneReplace->GetTextureBuffer(), m_pCharaModelTexPaneReplace->GetTexInfo() );
                    }

                    bLoadEnd  = true;
                  }
                }
                else
                {
                  bLoadEnd  = true;
                  GFL_ASSERT( 0 );
                }
              }
            }
          }
          else
          if( m_loadType == REQUEST_LOAD_TYPE_OTHER )
          {
            if( m_2DTexLoadSeq == 0 )
            {
              m_pIconObject->SetEnableReplaceTex( true );
              if( m_pIconObject->ReplaceTex( m_data.pReplacePane, m_load2DTexId ) == IconObject::REPLACE_TEX_RESULT_LOAD_DUBLICATION )
              {
                //  すでに同じのをロード済みなので終了
                if( m_pOutputTexture != NULL )
                {
                  m_pOutputTexture->CopyData( m_pIconObject->GetTexData(), m_pIconObject->GetTexSize() );
                }

                bLoadEnd  = true;
              }
              else
              {
                ++m_2DTexLoadSeq;
              }
            }
            else
            if( m_2DTexLoadSeq == 1 )
            {
              if( m_pIconObject->IsReplaceTex() == true )
              {
                if( m_pOutputTexture != NULL )
                {
                  m_pOutputTexture->CopyData( m_pIconObject->GetTexData(), m_pIconObject->GetTexSize() );
                }

                bLoadEnd  = true;
              }
            }
          }

          if( bLoadEnd == true )
          {
            //  ロード終了時にvisibleをtrueにするペインがあれば設定
            if( m_data.pInLoadVisibleChangePane != NULL )
            {
              m_data.pInLoadVisibleChangePane->SetVisible( true );
            }

            //  テクスチャ貼り付け
            m_data.pReplacePane->SetVisible( true );

            m_pTimerIconUIView->EndAnime();

            m_loadType  = REQUEST_LOAD_TYPE_DONE;
            m_state     = STATE_TYPE_IDLE;

            m_bLoaded   = true;
          }
        }

        m_pTimerIconUIView->UpdateTree();
      }

      //  描画
      void Draw( gfl2::gfx::CtrDisplayNo displayNo )
      {
        if( m_pTimerIconUIView != NULL )
        {
          if( m_bVisibleTimerIcon == true )
          {
            m_pTimerIconUIView->DrawTree( displayNo );
          }
        }
      }

      //  タイマーオブジェクトのレイアウト設定
      void SetLayoutParamTimerIconObject( const u32 layoutOrder, const u32 layoutNo )
      {
        if( m_pTimerIconUIView == NULL )
        {
          GFL_ASSERT( 0 );
          return;
        }

        m_pTimerIconUIView->SetLayoutParam( layoutOrder, layoutNo );
      }

      //  オブジェクトの状態取得
      StateType GetState() const { return m_state; }

      //  アイコンオブジェクトデータ取得
      AppToolTrainerIconRendering::ICON_OBJECT_DATA& GetData() { return m_data; }

      void StartTexReplaceModelLoad()
      {
        if( m_state == STATA_TYPE_TEX_MODEL_REPLACE_START_WAIT )
        {
          if( m_loadType  == REQUEST_LOAD_TYPE_HERO )
          {
            //  主人公であればモデル作成でのテクスチャ貼り付け処理を走らせる
            
            if( m_pCharaModelTexPaneReplace == NULL )
            {
              GFL_ASSERT( m_pCharaModelTexPaneReplace );
              return;
            }

            //  テーブルidx取得
            s32 tableIdx  = _GetModelLoadTableIdx( m_data.trainerId );
            if( tableIdx < 0 )
            {
              GFL_ASSERT( 0 );
              return;
            }

            // モデルは非同期読み込み
            app::tool::AppToolCharaTexPaneReplace::REQUEST_REPLACE_DATA requestData;

            if( m_data.heroTrainerData.bDressDefault == true )
            {
              if( m_data.trainerId == s_trainerIdHero )
              {
                System::DressUp::GetDressUpParamDefaultHero( &requestData.dressUpParam );
              }
              else
              if( m_data.trainerId == s_trainerIdHeroine )
              {
                System::DressUp::GetDressUpParamDefaultHeroine( &requestData.dressUpParam );
              }
            }
            else
            {
              gfl2::std::MemCopy( &m_data.heroTrainerData.dressUpParam, &requestData.dressUpParam, sizeof( requestData.dressUpParam ) );
            }

            requestData.charaId   = s_aTrainerModelRenderLoadDataTable[ tableIdx ].charalId;
            requestData.camPos    = m_data.heroTrainerData.camPos;
            requestData.camFovy   = m_data.heroTrainerData.camFovy;

            requestData.pReplacePane    = m_data.pReplacePane;
            requestData.animeId         = m_data.heroTrainerData.animeId;
            requestData.animeFrameNum   = m_data.heroTrainerData.animeFrameNum;
            requestData.modelPos        = m_data.heroTrainerData.modelPos;
            requestData.modelRot        = m_data.heroTrainerData.modelRot;
            requestData.eyeIndex        = (poke_3d::model::CharaModel::EyeIndex)m_data.heroTrainerData.eyeIndex;

            m_pCharaModelTexPaneReplace->SetEnableReplaceTex( true );
            m_pCharaModelTexPaneReplace->RequestReplace( requestData );

            m_state = STATA_TYPE_TEX_MODEL_REPLACE_RUN;
          }
          else
          if( m_loadType == REQUEST_LOAD_TYPE_OTHER )
          {
            //  2Dの場合は複数のオブジェクトの並列で問題ない
            GFL_ASSERT( 0 );
          }
        }
      }

    private:
      enum ReplaceLoadType
      {
        REQUEST_LOAD_TYPE_DONE  = 0,
        REQUEST_LOAD_TYPE_HERO,
        REQUEST_LOAD_TYPE_OTHER
      };

      //  設定したペインとタイマーアイコンを同期
      void _SyncPaneTimerIcon( gfl2::lyt::LytPicture* pPane, gfl2::lyt::LytPane* pSyncVisiblePane )
      {
        GFL_ASSERT( pPane != NULL );

        if( m_bFixingTimerIconPos == false )
        {
          if( m_data.pTimerIconRootPane == NULL )
          {
            //  ペインのSRT行列を設定
            const gfl2::math::MTX34 & global_matrix = pPane->GetGlobalMtx();
            m_pTimerIconUIView->SetSRTMtx( global_matrix );          
          }
        }
        else
        {
          //  固定位置設定があればそっちを優先
          gfl2::math::VEC3 pos = gfl2::math::ConvertGfVec3ToNwVec3( m_fixingTimerIconPos );
          m_pTimerIconUIView->SetIconPos( pos );
        }

        if( pSyncVisiblePane )
        {
          m_pTimerIconUIView->SetVisible( pSyncVisiblePane->IsVisible() );
        }
      }

      //  トレーナーidからモデルレンダーのロードテーブルidxを取得
      s32 _GetModelLoadTableIdx( u32 trainerId )
      {
        u32 loopNum = GFL_NELEMS( s_aTrainerModelRenderLoadDataTable );
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( s_aTrainerModelRenderLoadDataTable[ i ].trainerId == trainerId )
          {
            return  (s32)i;
          }
        }

        return  -1;
      }

      void _Clear()
      {
        m_pCharaModelTexPaneReplace = NULL;
        m_pTimerIconUIView          = NULL;
        m_pUseUtilHeap              = NULL;
        m_pFaceIconLayoutFileLoadState  = NULL;

        m_load2DTexId               = gfl2::fs::ArcFile::ARCDATID_NULL;
        m_2DTexLoadSeq              = 0;
        m_terminateSeq              = 0;
        m_loadType                  = REQUEST_LOAD_TYPE_DONE;
        m_state                     = STATE_TYPE_IDLE;
        m_pIconObject               = NULL;
        m_pOutputTexture            = NULL;
        m_bLoaded                   = false;
        m_bFixingTimerIconPos       = false;
        m_bVisibleTimerIcon         = true;
      }

      //  キャラモデルのテクスチャ貼り付け
      app::tool::AppToolCharaTexPaneReplace*  m_pCharaModelTexPaneReplace;
      //  タイマーアイコンView
      TimerIconUIView*                        m_pTimerIconUIView;
      IconObject*                             m_pIconObject;

      //  テクスチャー出力
      AppToolTrainerIconTexture*              m_pOutputTexture;

      //  トレーナーアイコンテクスチャーロード
      app::util::AppUtilFileControlState*     m_pFaceIconLayoutFileLoadState;
      u32                                     m_load2DTexId;
      u32                                     m_2DTexLoadSeq;

      bool                                    m_bFixingTimerIconPos;
      bool                                    m_bVisibleTimerIcon;
      gfl2::math::Vector3                     m_fixingTimerIconPos;
      

      ReplaceLoadType                         m_loadType;

      app::util::Heap*                        m_pUseUtilHeap;

      StateType                                       m_state;
      AppToolTrainerIconRendering::ICON_OBJECT_DATA   m_data;

      u32   m_terminateSeq;

      bool  m_bLoaded;
    };

GFL_NAMESPACE_END(localPlayerIconManager)

    //  主人公トレーナIDを取得
    u32 AppToolTrainerIconRendering::GetHeroTrainerId( const u32 sex )
    {
      if( sex == PM_MALE )
      {
        return  localPlayerIconManager::s_trainerIdHero;
      }

      return  localPlayerIconManager::s_trainerIdHeroine;
    }

    //  MyStatusのアイコンデータ変数を直接使って変換
    void AppToolTrainerIconRendering::ICON_OBJECT_DATA::ConvFromMyStatusIconData( const Savedata::MyStatus::ICON_DATA& rIcon )
    {
      //  セーブのアイコンからクラスで使えるデータに変換
      rIcon.dressUpParam.ConvertToDressUpParam( &heroTrainerData.dressUpParam, rIcon.dressUpParam );

      // @fix MMCat[337]nijiの場合は、バングルをnijiのに上書きする
      if( rIcon.dressUpParam.isRomVersionNiji == true )
      {
        System::DressUpParamSaveData::OverrideNijiBngl( &heroTrainerData.dressUpParam );
      }

      s16 sex = heroTrainerData.dressUpParam.bodyParams[ poke_3d::model::DressUpParam::BODY_CATEGORY_SEX ];
      if( sex == PM_MALE )
      {
        trainerId = localPlayerIconManager::s_trainerIdHero;
      }
      else
      if( sex == PM_FEMALE )
      {
        trainerId = localPlayerIconManager::s_trainerIdHeroine;
      }
      else
      {
        GFL_ASSERT( 0 );
        return;
      }

      heroTrainerData.animeFrameNum = rIcon.animeFrameNum;
      heroTrainerData.animeId       = rIcon.animeId;
      heroTrainerData.camFovy       = rIcon.camFovy;
      heroTrainerData.camPos        = gfl2::math::Vector3( rIcon.camPosX, rIcon.camPosY, localPlayerIconManager::s_camDefaultZPos );
      heroTrainerData.eyeIndex      = rIcon.eyeIndex;
      heroTrainerData.modelPos      = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
      heroTrainerData.modelRot      = gfl2::math::Vector3( 0.0f, rIcon.modelYRot, 0.0f );
      heroTrainerData.bDressDefault = ( rIcon.bDressDefault == 1 ? true : false );
    }

    //  MyStatusのアイコンデータからTrainerIconクラスで使えるデータに変換
    void AppToolTrainerIconRendering::ICON_OBJECT_DATA::ConvFromMyStatus( const Savedata::MyStatus& rSavedataMyStatus )
    {
      ConvFromMyStatusIconData( rSavedataMyStatus.GetIcon() );
    }

    //  データからアイコンセーブデータへ変換
    void AppToolTrainerIconRendering::ICON_OBJECT_DATA::ConvToMyStatusIcon( Savedata::MyStatus::ICON_DATA* pOutPutSavedataMyStatusIcon )
    {
      GFL_ASSERT( pOutPutSavedataMyStatusIcon );
      if( pOutPutSavedataMyStatusIcon == NULL )
      {
        return;
      }

      pOutPutSavedataMyStatusIcon->animeFrameNum  = heroTrainerData.animeFrameNum;
      pOutPutSavedataMyStatusIcon->animeId        = heroTrainerData.animeId;
      pOutPutSavedataMyStatusIcon->camFovy        = heroTrainerData.camFovy;
      pOutPutSavedataMyStatusIcon->camPosX        = heroTrainerData.camPos.x;
      pOutPutSavedataMyStatusIcon->camPosY        = heroTrainerData.camPos.y;

      pOutPutSavedataMyStatusIcon->dressUpParam.ConvertFromDressUpParam( &pOutPutSavedataMyStatusIcon->dressUpParam, heroTrainerData.dressUpParam );
      pOutPutSavedataMyStatusIcon->eyeIndex       = heroTrainerData.eyeIndex;
      pOutPutSavedataMyStatusIcon->modelYRot      = heroTrainerData.modelRot.y;
    }

    //  トレーナーモデルのArcID取得
    gfl2::fs::ArcFile::ARCID AppToolTrainerIconRendering::GetTrainerModelArcId()
    {
      return  ARCID_CHARA_MODEL_BATTLE_NORMAL;
    }

    //  トレーナーIDからモデルリソースid取得
    u32 AppToolTrainerIconRendering::ConvTrainerIdToModelResID( const u32 trainerId )
    {
      u32 loopNum = GFL_NELEMS( localPlayerIconManager::s_aTrainerModelRenderLoadDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( localPlayerIconManager::s_aTrainerModelRenderLoadDataTable[ i ].trainerId == trainerId )
        {
          return  localPlayerIconManager::s_aTrainerModelRenderLoadDataTable[ i ].charalId;
        }
      }

      GFL_ASSERT( 0 );
      return  0;
    }

    //  トレーナーIDからモデルの待機モーションアニメid取得
    u32 AppToolTrainerIconRendering::ConvTrainerIdToModelWaitMotionAnimeId( const u32 trainerId )
    {
      u32 loopNum = GFL_NELEMS( localPlayerIconManager::s_aTrainerModelRenderLoadDataTable );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( localPlayerIconManager::s_aTrainerModelRenderLoadDataTable[ i ].trainerId == trainerId )
        {
          return  localPlayerIconManager::s_aTrainerModelRenderLoadDataTable[ i ].waitMotionAnimeId;
        }
      }

      GFL_ASSERT( 0 );
      return  0;
    }

    //  MyStatus::ICON_DATAからデフォルトデータ作成
    void AppToolTrainerIconRendering::OutputDefaultMyStatusIconData( const u32 sex, Savedata::MyStatus::ICON_DATA* pOutputData )
    {
      if( pOutputData == NULL )
      {
        GFL_ASSERT( 0 );
        return;
      }

      u32 trainerId = 0;
      if( sex == PM_MALE )
      {
        trainerId = localPlayerIconManager::s_trainerIdHero;
      }
      else
      if( sex == PM_FEMALE )
      {
        trainerId = localPlayerIconManager::s_trainerIdHeroine;
      }
      else
      {
        GFL_ASSERT( 0 );
      }

      ICON_OBJECT_DATA  iconObjData;
      OutputDefaultIconObjectData( trainerId, &iconObjData );

      iconObjData.ConvToMyStatusIcon( pOutputData );

      pOutputData->bDressDefault  = true;
    }

    //  ICON_OBJECT_DATAのデフォルトデータ作成
    void AppToolTrainerIconRendering::OutputDefaultIconObjectData( const u32 trainerId, ICON_OBJECT_DATA* pOutputData )
    {
      if( pOutputData == NULL )
      {
        GFL_ASSERT( 0 );
        return;
      }

      HERO_TRAINER_ICON_DATA* pIconData = &pOutputData->heroTrainerData;

      pOutputData->trainerId  = trainerId;

      //  モーションid
      pIconData->animeId  = ConvTrainerIdToModelWaitMotionAnimeId( trainerId );

      if( localPlayerIconManager::s_trainerIdHero == trainerId )
      {
        pIconData->camFovy       = 0.034907;
        pIconData->camPos        = gfl2::math::Vector3( -4.000000, 118.500000, 1000.000000 );
        pIconData->modelPos      = gfl2::math::Vector3( 0.000000, 0.000000, 0.000000 );
        pIconData->modelRot      = gfl2::math::Vector3( 0.000000, 0.000000, 0.000000 );
        pIconData->animeId       = 4;
        pIconData->animeFrameNum = 41;
        pIconData->eyeIndex      = 1;
      }
      else
      if( localPlayerIconManager::s_trainerIdHeroine == trainerId )
      {
        pIconData->camFovy       = 0.034907;
        pIconData->camPos        = gfl2::math::Vector3( 0.000000, 121.500000, 1000.000000 );
        pIconData->modelPos      = gfl2::math::Vector3( 0.000000, 0.000000, 0.000000 );
        pIconData->modelRot      = gfl2::math::Vector3( 0.000000, 0.209440, 0.000000 );
        pIconData->animeId       = 4;
        pIconData->animeFrameNum = 44;
        pIconData->eyeIndex      = 1;
      }
      else
      {
        //  モーションフレーム値
        pIconData->animeFrameNum  = 0;

        //  キャラの顔情報
        pIconData->eyeIndex = poke_3d::model::CharaModel::EYE_INDEX_OPEN;

        //  カメラ情報
        //  カメラ座標
        pIconData->camPos = gfl2::math::Vector3( 0, 130.0f, localPlayerIconManager::s_camDefaultZPos );

        //  カメラの画角
        pIconData->camFovy  = 0.03490685f;

        //  キャラR情報
        pIconData->modelRot   = gfl2::math::Vector3( 0, 0, 0 );

        //  着せ替えはデフォルトに
        gfl2::std::MemClear( &pOutputData->heroTrainerData.dressUpParam, sizeof( pOutputData->heroTrainerData.dressUpParam ) );
        pIconData->bDressDefault  = true;
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラスが使うシステムヒープサイズ取得
    * @param[in]  drawIconNum: 表示するアイコン数
    * @return     クラスに必要なヒープサイズ
    */
    //--------------------------------------------------------------------------------------------
    u32 AppToolTrainerIconRendering::GetUseSystemHeapSize( const u32 drawIconNum, const bool bNPCOnly )
    {
      u32 iconObjectSysHeapSize  = localPlayerIconManager::TrainerIconObject::GetUseSystemHeapSize();
      if( bNPCOnly == false )
      {
        iconObjectSysHeapSize += AppToolCharaTexPaneReplace::GetUseSystemHeapSize();
      }

      return ( drawIconNum * iconObjectSysHeapSize );
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラスが使うデバイスヒープサイズ取得
    * @param[in]  drawIconNum: 表示するアイコン数
    * @return     クラスに必要なヒープサイズ
    */
    //--------------------------------------------------------------------------------------------
    u32 AppToolTrainerIconRendering::GetUseDeviceHeapSize( const u32 drawIconNum, const bool bNPCOnly )
    {
      u32 iconObjectDevHeapSize  = localPlayerIconManager::TrainerIconObject::GetUseDeviceHeapSize();
      u32 modelDevHeapSize       = 0;
      if( bNPCOnly == false )
      {
        iconObjectDevHeapSize += AppToolCharaTexPaneReplace::GetUseDeviceHeapSize();
        iconObjectDevHeapSize += AppToolCharaTexPaneReplace::GetUseVramAllocatorSize() * 1.5f;
        modelDevHeapSize = AppToolCharaTexPaneReplace::GetUseModelHeapSize();
      }

      return ( ( drawIconNum * iconObjectDevHeapSize ) + ( 1024 * 80 ) + modelDevHeapSize );
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      デストラクタ
    */
    //--------------------------------------------------------------------------------------------
    AppToolTrainerIconRendering::~AppToolTrainerIconRendering()
    {
      GFL_SAFE_DELETE( m_pCharaModelLightDrawEnv );

      //  プレイヤーオブジェクトの開放
      GFL_SAFE_DELETE_ARRAY( m_pIconObjectArray );

      GFL_SAFE_DELETE( m_pTimerIconLayoutFileLoadState );
      GFL_SAFE_DELETE( m_pVramAllocator );

      if( m_pVramHeap != NULL )
      {
        GFL_DELETE_HEAP( m_pVramHeap );
      }

      if( m_pModelFactoryHeap != NULL )
      {
        GFL_DELETE_HEAP( m_pModelFactoryHeap );
      }

      if( m_pModelResourceManagerHeap != NULL )
      {
        GFL_DELETE_HEAP( m_pModelResourceManagerHeap );
      }

      GFL_SAFE_DELETE( m_pLocalHeap );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     * @param[in]  pRenderingManager:  内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  pHeap:              内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  pFileManager:       ファイルロード用( 内部で参照しているのでアドレス先を破棄してはいけない )
     * @param[in]  iconMax:            管理するアイコン最大数
     * @param[in]  displayType:        アイコンの描画先を指定
     * @note
     *      必ず最初に実行しないといけない
     */
    //-----------------------------------------------------------------------------
    void AppToolTrainerIconRendering::Initialize(
     app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, 
     gfl2::fs::AsyncFileManager* pFileManager, const u32 iconMax, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType )
    {
      GFL_ASSERT( pRenderinManager );
      GFL_ASSERT( pHeap );
      GFL_ASSERT( pFileManager );
      GFL_ASSERT( 0 < iconMax );

      m_pRenderinManager  = pRenderinManager;
      m_pFileManager      = pFileManager;

      u32 vramUseHeapSize = AppToolCharaTexPaneReplace::GetUseVramAllocatorSize() * 1.5f;

      //  このクラス内のみで使うローカルヒープを作成
      m_pLocalHeap  = GFL_NEW( pHeap->GetDeviceHeap() ) app::util::Heap();
      m_pLocalHeap->LocalHeapCreate( pHeap->GetSystemHeap(), pHeap->GetDeviceHeap(), GetUseSystemHeapSize( iconMax ), GetUseDeviceHeapSize( iconMax ) );

      UCHIDA_PRINT( "TrainerIconUse SysHeap: %d\n", GetUseSystemHeapSize( iconMax ) );
      UCHIDA_PRINT( "TrainerIconUse DevHeap: %d\n", GetUseDeviceHeapSize( iconMax ) );

      //  ターゲットに転送するVRAMを作成
      {
        m_pVramHeap             = GFL_CREATE_LOCAL_HEAP( m_pLocalHeap->GetDeviceHeap(), vramUseHeapSize, gfl2::heap::HEAP_TYPE_EXP, false );
        m_pVramAllocator        = GFL_NEW( m_pLocalHeap->GetDeviceHeap() ) System::nijiAllocator( m_pVramHeap );
      }

      //  キャラに最適なライト設定
      //  Addしているのでこのクラスのインスタンスが複数だとやばい
      {
        m_pCharaModelLightDrawEnv = app::util::AppRenderingManager::CreateDefaultDrawEnv( m_pLocalHeap, app::util::AppRenderingManager::PresetType::TRAINER );
        pRenderinManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_pCharaModelLightDrawEnv );
      }

      //  タイマーアイコンレイアウトのファイル読み込み開始
      {
        m_pTimerIconLayoutFileLoadState  =  GFL_NEW( m_pLocalHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( TimerIconUIView::GetArcId(), TimerIconUIView::GetArcDataId(), m_pLocalHeap->GetDeviceHeap(), true, 128 );

        //  ファイル制御開始
        m_pTimerIconLayoutFileLoadState->Start();
      }

      //  モデル生成準備
      {
        m_pModelFactoryHeap = GFL_CREATE_LOCAL_HEAP(  m_pLocalHeap->GetDeviceHeap(), CharaSimpleModel::ONE_MODEL_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
        m_charaModelFactory.Initialize( pFileManager, m_pModelFactoryHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL );

        m_pModelResourceManagerHeap = GFL_CREATE_LOCAL_HEAP(  m_pLocalHeap->GetDeviceHeap(), CharaSimpleModel::ONE_MODEL_DRESS_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
        m_dressUpModelResourceManager.Initialize( pFileManager, m_pModelResourceManagerHeap, System::DressUp::GetDressUpArcIdListBattle() );
      }

      //  アイコンオブジェクトリスト作成
      {
        m_pIconObjectArray    = GFL_NEW_ARRAY( m_pLocalHeap->GetDeviceHeap() ) localPlayerIconManager::TrainerIconObject[ iconMax ];
        m_iconObjectArrayNum  = iconMax;
      }

      m_displayType = displayType;

      m_bReady    = false;
      m_bNPCOnly  = false;
    }

    //  NPCアイコンの表示だけの場合はこちらを使うとモデルが使わないのメモリが少なくてすむ
    void AppToolTrainerIconRendering::InitializeByNPCOnly(
     app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap,
     gfl2::fs::AsyncFileManager* pFileManager, const u32 iconMax, app::util::G2DUtil::SETUP_DISPLAY_TYPE displayType )
    {
      GFL_ASSERT( pHeap );
      GFL_ASSERT( pFileManager );
      GFL_ASSERT( 0 < iconMax );

      m_bNPCOnly  = true;
      m_pRenderinManager  = pRenderinManager;
      m_pFileManager      = pFileManager;

      //  このクラス内のみで使うローカルヒープを作成
      m_pLocalHeap  = GFL_NEW( pHeap->GetDeviceHeap() ) app::util::Heap();
      m_pLocalHeap->LocalHeapCreate( pHeap->GetSystemHeap(), pHeap->GetDeviceHeap(), GetUseSystemHeapSize( iconMax, m_bNPCOnly ), GetUseDeviceHeapSize( iconMax, m_bNPCOnly ) );

      UCHIDA_PRINT( "TrainerIconUse NPCOnly SysHeap: %d\n", GetUseSystemHeapSize( iconMax, m_bNPCOnly ) );
      UCHIDA_PRINT( "TrainerIconUse NPCOnly DevHeap: %d\n", GetUseDeviceHeapSize( iconMax, m_bNPCOnly ) );

      //  タイマーアイコンレイアウトのファイル読み込み開始
      {
        m_pTimerIconLayoutFileLoadState  =  GFL_NEW( m_pLocalHeap->GetDeviceHeap() ) app::util::AppUtilFileControlState( TimerIconUIView::GetArcId(), TimerIconUIView::GetArcDataId(), m_pLocalHeap->GetDeviceHeap(), true, 128 );

        //  ファイル制御開始
        m_pTimerIconLayoutFileLoadState->Start();
      }

      //  アイコンオブジェクトリスト作成
      {
        m_pIconObjectArray  = GFL_NEW_ARRAY( m_pLocalHeap->GetDeviceHeap() ) localPlayerIconManager::TrainerIconObject[ iconMax ];
        m_iconObjectArrayNum  = iconMax;
      }

      m_displayType = displayType;

      m_bReady    = false;
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス使用終了通知
    * @note
    *             クラスを破棄する前には必ずこれを実行
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::Terminate()
    {
      if( m_bTerminate == false )
      {
        m_bTerminate  = true;
        m_terminateSeq  = 0;

        {
          u32 loopNum = m_iconObjectArrayNum;
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_pIconObjectArray[ i ].Terminate();
          }
        }
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス破棄可能か
    * @return     使用不可能 false / 使用可能 true
    * @note
    *             Initialize()関数を呼んだ後に true が返ってくるまで呼び続ける
    */
    //--------------------------------------------------------------------------------------------
    bool AppToolTrainerIconRendering::IsReady()
    {
      if( m_bReady == false )
      {
        if( m_readySeq == 0 )
        {
          m_pTimerIconLayoutFileLoadState->Update();

          if( m_pTimerIconLayoutFileLoadState->IsEnd() == true )
          {
            //  アイコンオブジェクトを作成
            u32 loopNum = m_iconObjectArrayNum;
            for( u32 i = 0; i < loopNum; ++i )
            {
              m_pIconObjectArray[ i ].Initialize( 
              m_pRenderinManager, 
              m_pLocalHeap, 
              m_pFileManager, 
              m_pVramAllocator, 
              m_pTimerIconLayoutFileLoadState->GetLoadDataBuffer(), 
              &m_charaModelFactory, 
              &m_dressUpModelResourceManager, 
              m_displayType,
              m_bNPCOnly );
            }

            ++m_readySeq;
          }
        }
        else
        if( m_readySeq == 1 )
        {
          bool  bEnd  = true;

          u32 loopNum = m_iconObjectArrayNum;
          for( u32 i = 0; i < loopNum; ++i )
          {
            if( m_pIconObjectArray[ i ].IsReady() == false )
            {
              bEnd  = false;
            }
          }

          if( bEnd  == true )
          {
            ++m_readySeq;
          }
        }
        else
        if( m_readySeq == 2 )
        {
          m_bReady  = true;
        }
      }
      
      return m_bReady;
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      クラス破棄可能か
    * @return     破棄不可能 false / 破棄可能 true
    * @note
    *             Terminate()関数を呼んだ後に true が返ってくるまで呼び続ける
    */
    //--------------------------------------------------------------------------------------------
    bool AppToolTrainerIconRendering::IsDelete()
    {
      if( m_bTerminate == false )
      {
        return  false;
      }

      bool  bRet  = true;

      if( m_terminateSeq == 0 )
      {
        u32 loopNum = m_iconObjectArrayNum;
        for( u32 i = 0; i < loopNum; ++i )
        {
          if( m_pIconObjectArray[ i ].IsDelete() == false )
          {
            bRet  = false;
          }
        }

        if( bRet == true )
        {
          ++m_terminateSeq;
        }
      }
      else
      if( m_terminateSeq == 1 )
      {
        m_charaModelFactory.Finalize();
        m_dressUpModelResourceManager.Finalize();
        ++m_terminateSeq;
      }
      else
      if( m_terminateSeq == 2 )
      {
        return  true;
      }

      return  false;
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      更新
    * @note
    *             必ず毎フレーム実行しないといけない
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::Update()
    {
      if( IsReady() == false )
      {
        return;
      }

      if( m_bTerminate == true )
      {
        return;
      }

      {
        //  アイコンオブジェクト更新
        u32 loopNum = m_iconObjectArrayNum;
        //  まずすべての更新をする
        for( u32 i = 0; i < loopNum; ++i )
        {
          m_pIconObjectArray[ i ].Update();
        }

        //  その後にモデル処理が可能なオブジェクトを探す
        bool  bReplaceRun = false;
        u32   replaceStartIconObjectIdx = 0xffff;
        for( u32 i = 0; i < loopNum; ++i )
        {
          //  モデルをロードして張り付ける処理は必ず１オブジェクトしか呼ばないようにする
          localPlayerIconManager::TrainerIconObject::StateType  stateType = m_pIconObjectArray[ i ].GetState();

          //  複数並列でロードするとレンダーのモデル登録数が超えることがあるのでやばい
          //  まだモデルテクスチャの貼り付け中か
          if( m_pIconObjectArray[ i ].IsReplaceModelTexRun() == true )
          {
            bReplaceRun = true;
            break;
          }
          else
          if( stateType == localPlayerIconManager::TrainerIconObject::STATA_TYPE_TEX_MODEL_REPLACE_START_WAIT )
          {
            if( replaceStartIconObjectIdx == 0xffff )
            {
              replaceStartIconObjectIdx = i;
            }
          }
        }

        //  どのオブジェクトも貼り付け処理をしていないでかつ貼り付け処理の指示をまっているオブジェクトがいる
        //  この場合１オブジェクトだけモデルの貼り付け処理を許可する
        if( bReplaceRun == false )
        {
          if( replaceStartIconObjectIdx != 0xffff )
          {
            m_pIconObjectArray[ replaceStartIconObjectIdx ].StartTexReplaceModelLoad();
          }
        }
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      描画
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::Draw( gfl2::gfx::CtrDisplayNo displayNo )
    {
      {
        //  アイコンオブジェクト更新
        u32 loopNum = m_iconObjectArrayNum;
        for( u32 i = 0; i < loopNum; ++i )
        {
           m_pIconObjectArray[ i ].Draw( displayNo );
        }
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      タイマーアイコンオブジェクトのレイアウト設定
    * @param[in]  layoutOrder:     設定するアイコンオブジェクトidx
    * @param[in]  layoutNo:        オブジェクトデータ
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::SetLayoutParamTimerIconObject( const u32 layoutOrder, const u32 layoutNo )
    {
      {
        //  アイコンオブジェクト更新
        u32 loopNum = m_iconObjectArrayNum;
        for( u32 i = 0; i < loopNum; ++i )
        {
           m_pIconObjectArray[ i ].SetLayoutParamTimerIconObject( layoutOrder, layoutNo );
        }
      }
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::SetIconObjectData( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon )
    {
      _SetIconObjectDataBasic( idx, rObjectData, bVisibleTimerIcon, false );
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    * @param[in]  idx:              設定するアイコンオブジェクトidx
    * @param[in]  rObjectData:      オブジェクトデータ
    * @param[in]  rTimerIconPos:    タイマーアイコンの表示位置
    * @note
    *             下記の設定したペインと位置を同期させずに rTimerIconPos引数で設定した固定位置にタイマーアイコンを表示させる
    *             rObjectData.pReplacePane
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::SetIconObjectData( const u32 idx, const ICON_OBJECT_DATA& rObjectData, const gfl2::math::Vector3& rTimerIconPos, bool bVisibleTimerIcon )
    {
      if( m_iconObjectArrayNum <= idx )
      {
        GFL_ASSERT( 0 );
        return;
      }

      //  タイマーアイコン表示の固定位置を設定
      localPlayerIconManager::TrainerIconObject* pCurrentIconObject  = &m_pIconObjectArray[ idx ];
      pCurrentIconObject->SetTimerIconPos( rTimerIconPos );

      _SetIconObjectDataBasic( idx, rObjectData, bVisibleTimerIcon, true );
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクト設定
    * @param[in]  idx:                設定するアイコンオブジェクトidx
    * @param[in]  rObjectData:        オブジェクトデータ
    * @param[in]  bVisibleTimerIcon:  ロードする時にTimerIconを出すか
    * @param[out] pOutputTexture:     ロードした時のテクスチャーデータ出力
    * @note
    *       ロードしたテクスチャーデータを受け取る関数
    *       使用側で再ロードする必要がない時に使える(特にリスト関連は)
    */
    //--------------------------------------------------------------------------------------------
    void AppToolTrainerIconRendering::SetIconObjectDataByOutputTexture( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon, AppToolTrainerIconTexture* pOutputTexture )
    {
      if( m_iconObjectArrayNum <= idx )
      {
        GFL_ASSERT( 0 );
        return;
      }

      SetIconObjectData( idx, rObjectData, bVisibleTimerIcon );

      localPlayerIconManager::TrainerIconObject* pCurrentIconObject  = &m_pIconObjectArray[ idx ];
      pCurrentIconObject->SetTexture( pOutputTexture );
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクトロードが終っているか
    */
    //--------------------------------------------------------------------------------------------
    bool AppToolTrainerIconRendering::IsLoadedObject( const u32 idx )
    {
      if( m_bTerminate == true )
      {
        return  false;
      }

      if( m_iconObjectArrayNum <= idx )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      localPlayerIconManager::TrainerIconObject* pCurrentIconObject  = &m_pIconObjectArray[ idx ];
      return  pCurrentIconObject->IsLoaded();
    }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      アイコンオブジェクトデータ取得
    * @param[in]  idx: 表示するアイコンオブジェクトidx
    */
    //--------------------------------------------------------------------------------------------
    AppToolTrainerIconRendering::ICON_OBJECT_DATA* AppToolTrainerIconRendering::GetIconObjectData( const u32 idx )
    {
      if( m_bTerminate == true )
      {
        return  NULL;
      }

      if( m_iconObjectArrayNum <= idx )
      {
        GFL_ASSERT( 0 );
        return  NULL;
      }

      localPlayerIconManager::TrainerIconObject* pCurrentIconObject  = &m_pIconObjectArray[ idx ];
      return  &( pCurrentIconObject->GetData() );
    }

//  ここから先は AppToolTrainerIconTextureクラスの定義
    AppToolTrainerIconTexture::AppToolTrainerIconTexture( gfl2::heap::HeapBase* pHeap )
    {
      _Clear();

      if( pHeap == NULL )
      {
        GFL_ASSERT( pHeap );
        return;
      }

      //  テクスチャー分のメモリをローカル確保(一番大きい主人公トレーナーを基準に)
      m_memSize = AppToolCharaTexPaneReplace::GetUseVramAllocatorSize() + sizeof( app::tool::LytTexData::BclimFooter );
      m_pTexData  = GflHeapAllocMemoryAlign( pHeap, m_memSize, 128 );
    }

    AppToolTrainerIconTexture::~AppToolTrainerIconTexture()
    {
      if( m_bCreateData == true )
      {
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
        nw::lyt::DeleteTexture( m_texInfo );
#endif
      }

      GflHeapSafeFreeMemory( m_pTexData );
    }

    //  画像ペインに作成したテクスチャーを設定
    bool AppToolTrainerIconTexture::ReplacePicPane( gfl2::lyt::LytPicture* pRqplacePane )
    {
      if( pRqplacePane == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      if( m_bCreateData == false )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &m_texInfo, pRqplacePane, 0, 0 );

      return  true;
    }

    //  データを転送
    void AppToolTrainerIconTexture::CopyData( void* pData, const u32 size )
    {
      if( m_bCreateData == true )
      {
        return;
      }

      if( pData == NULL )
      {
        GFL_ASSERT( 0 );
        return;
      }

      if( size <= 0 )
      {
        GFL_ASSERT( 0 );
        return;
      }

      if( m_memSize < size )
      {
        GFL_ASSERT( 0 );
        return;
      }

      //  バイナリデータをコピーする
      m_bCreateData = true;
      gfl2::std::MemCopy( pData, m_pTexData, size );

      //  コピーしたデータでテクスチャーをロード
      nw::lyt::LoadTexture( &m_texInfo, m_pTexData, size );
    }

    //  テクスチャーインフォからデータ生成
    void AppToolTrainerIconTexture::CreateTex( void* pTextureBuffer, gfl2::lyt::LytResTextureInfo& rTexInfo )
    {
      if( m_bCreateData == true )
      {
        return;
      }

      if( LytTexData::CreateTexDataFromTexInfo( &m_texInfo, m_pTexData, m_memSize, pTextureBuffer, rTexInfo ) == true )
      {
        m_bCreateData = true;
      }
    }

    void AppToolTrainerIconRendering::_SetIconObjectDataBasic( const u32 idx, const ICON_OBJECT_DATA& rObjectData, bool bVisibleTimerIcon, const bool bTimerIconFixPos )
    {
      if( m_bTerminate == true )
      {
        return;
      }

      if( m_bReady == false )
      {
        //  まだ準備ができていない
        GFL_ASSERT( 0 );
        return;
      }

      if( m_iconObjectArrayNum <= idx )
      {
        GFL_ASSERT( 0 );
        return;
      }

      if( trainer::TRTYPE_MAX <= rObjectData.trainerId )
      {
        //  id指定が不正
        GFL_ASSERT( 0 );
        return;
      }

      localPlayerIconManager::TrainerIconObject* pCurrentIconObject  = &m_pIconObjectArray[ idx ];
      pCurrentIconObject->SetEnableTimerIconFixPos( bTimerIconFixPos );
      pCurrentIconObject->SetData( rObjectData, bVisibleTimerIcon );
    }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
