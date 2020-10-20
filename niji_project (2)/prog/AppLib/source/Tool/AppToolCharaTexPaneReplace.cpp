//======================================================================
/**
 * @file AppToolCharaTexPaneReplace.cpp
 * @date 2015/08/06 16:13:13
 * @author uchida_yuto
 * @brief キャラテクスチャを画像ペインに貼り付ける
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/AppToolCharaTexPaneReplace.h"

//  nijiのインクルード
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Util/app_util_RenderTargetTexture.h"
#include "System/include/nijiAllocator.h"

// garc
#include <arc_def_index/arc_def.h>
#include <System/include/DressUp.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  @brief  使用するVRAMサイズを取得
  u32  AppToolCharaTexPaneReplace::GetUseVramAllocatorSize()
  {
    gfl2::gfx::Format   colorFormat = _GetTexturePixelColorFormat();
    gfl2::math::Vector2 textureSize = _GetTextureSize();

    u32 size  = (u32)(textureSize.x * textureSize.y);

    if( colorFormat == gfl2::gfx::Format::A8R8G8B8 )
    {
      size  *= 4;
    }

    return  size;
  }

  //  @brief  クラスが使用するシステムヒープサイズ
  u32 AppToolCharaTexPaneReplace::GetUseSystemHeapSize()
  {
    return  SYS_HEAP_SIZE;
  }

  //  @brief  クラスが使用するデバイスヒープサイズ
  u32 AppToolCharaTexPaneReplace::GetUseDeviceHeapSize()
  {
    return  DEV_HEAP_SIZE + CharaSimpleModel::USE_HEAP_SIZE;
  }

  //  @brief  モデルで使うメモリサイズ
  u32 AppToolCharaTexPaneReplace::GetUseModelHeapSize()
  {
    return  ( CharaSimpleModel::ONE_MODEL_BIN_HEAP_SIZE + CharaSimpleModel::ONE_MODEL_DRESS_BIN_HEAP_SIZE );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      コンストラクタ
  * @param[in]  pRenderingManager:  内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
  * @param[in]  pHeap:              内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
  * @param[in]  pFileManager:        モデル読みこみのファイル管理
  * @param[in]  pVramAllocator:      レンダーテクスチャのVRAMアロケータ
  */
  //--------------------------------------------------------------------------------------------
  AppToolCharaTexPaneReplace::AppToolCharaTexPaneReplace( 
  app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap,
  gfl2::fs::AsyncFileManager* pFileManager, System::nijiAllocator* pVramAllocator )
  {
    _Clear();

    GFL_ASSERT( pFileManager );

    //  モデル生成準備
    {
      m_pCharaModelFactory  = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::CharaModelFactory();
      m_pModelFactoryHeap = GFL_CREATE_LOCAL_HEAP(  pHeap->GetDeviceHeap(), CharaSimpleModel::ONE_MODEL_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

      m_bCreateCharaModelFactor   = true;
      m_pCharaModelFactory->Initialize( pFileManager, m_pModelFactoryHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL );

      m_pDressUpModelResourceManager = GFL_NEW( pHeap->GetDeviceHeap() ) poke_3d::model::DressUpModelResourceManager();
      m_pModelResourceManagerHeap = GFL_CREATE_LOCAL_HEAP(  pHeap->GetDeviceHeap(), CharaSimpleModel::ONE_MODEL_DRESS_BIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );

      m_bCreateDressUpModelResourceManager = true;
      m_pDressUpModelResourceManager->Initialize( pFileManager, m_pModelResourceManagerHeap, System::DressUp::GetDressUpArcIdListBattle() );
    }

    _Constructor( pRenderinManager, pHeap, pVramAllocator );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      コンストラクタ
  * @param[in]  pRenderingManager:   内部でオフスクリーンを使うのに必要( 内部で参照しているのでアドレス先を破棄してはいけない )
  * @param[in]  pHeap:               内部で使用するヒープ( 内部で参照しているのでアドレス先を破棄してはいけない )
  * @param[in]  pCharaModelFactory:  モデル制御に使う管理クラス
  * @param[in]  pVramAllocator:      レンダーテクスチャのVRAMアロケータ(レンダーテクスチャーに必要なメモリを作成 GetUseVramAllocatorSize() からサイズ取得可能 )
  * @note
  *   poke_3d::model::CharaModelFactoryのインスタンスで外部で作成して設定する
  *   なぜこれを用意したのかはメモリ節約が目的
  *   このクラスの処理を複数で平行で処理する場合は poke_3d::model::CharaModelFactory インスタンスは数分必要だが
  *   平行でなく単体のみの処理の時は poke_3d::model::CharaModelFactory インスタンスは一つだけ用意してい使いまわせばメモリ節約する場合は
  *   この関数を用意
  */
  //--------------------------------------------------------------------------------------------
  AppToolCharaTexPaneReplace::AppToolCharaTexPaneReplace( 
    app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, 
    poke_3d::model::CharaModelFactory* pCharaModelFactory,
    poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager, System::nijiAllocator* pVramAllocator )
  {
    _Clear();

    GFL_ASSERT( pCharaModelFactory );
    GFL_ASSERT( pDressUpModelResourceManager );

    m_pCharaModelFactory  = pCharaModelFactory;
    m_bCreateCharaModelFactor = false;

    m_pDressUpModelResourceManager = pDressUpModelResourceManager;
    m_bCreateDressUpModelResourceManager = false;

    _Constructor( pRenderinManager, pHeap, pVramAllocator );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      デストラクタ
  */
  //--------------------------------------------------------------------------------------------
  AppToolCharaTexPaneReplace::~AppToolCharaTexPaneReplace()
  {
    if( m_bCreateCharaModelFactor == true )
    {
      GFL_SAFE_DELETE( m_pCharaModelFactory );
    }

    GFL_SAFE_DELETE( m_pRenderTargetTexture );
    GFL_SAFE_DELETE( m_replaceDataBuffer.pCharaModel );
    GFL_SAFE_DELETE( m_pDrawEnvNodeCamera );
    GFL_SAFE_DELETE( m_pCamera );

    if( m_bCreateCharaModelFactor == true )
    {
      GFL_DELETE_HEAP( m_pModelFactoryHeap );
    }

    GFL_SAFE_DELETE( m_pHeap );
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      クラス使用終了
  *             クラスを破棄する時必ずこれを実行
  */
  //--------------------------------------------------------------------------------------------
  void AppToolCharaTexPaneReplace::Terminate()
  {
    m_terminateSeq  = 0;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      クラス使用終了可能か
  * @return     実行中 false / 実行終了(クラスの破棄可能に) true
  * @note
  *             必ず Terminate() を呼んでから実行
  */
  //--------------------------------------------------------------------------------------------
  bool AppToolCharaTexPaneReplace::IsDelete()
  {
    enum
    {
      TERMINATE_SEQ_INIT_DATA_LIST  = 0,
      TERMINATE_SEQ_UPDATE_DATA_LIST,
      TERMINATE_SEQ_CHAR_MODEL_FACTORY_END,
      TERMINATE_SEQ_END
    };

    //  後始末が終わっているか
    if( m_terminateSeq == TERMINATE_SEQ_INIT_DATA_LIST )
    {
      //  オフスクリーンをはずす
      if( m_pRenderinManager->GetOffScreenRenderTargetTexture() == m_pRenderTargetTexture )
      {
        m_pRenderinManager->UnsetOffScreenRenderTargetTexture( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
      }

      //  各貼り付けのモデルの破棄
      m_replaceDataBuffer.pCharaModel->Terminate();

      ++m_terminateSeq;
    }
    else
    //  貼り付けリスト破棄更新
    if( m_terminateSeq == TERMINATE_SEQ_UPDATE_DATA_LIST )
    {
      if( m_replaceDataBuffer.pCharaModel->IsDelete() == true )
      {
        ++m_terminateSeq;
      }
    }
    else
    if( m_terminateSeq == TERMINATE_SEQ_CHAR_MODEL_FACTORY_END )
    {
      if( m_bCreateCharaModelFactor == true )
      {
        m_pCharaModelFactory->Finalize();
      }

      ++m_terminateSeq;
    }
    
    if( m_terminateSeq == TERMINATE_SEQ_END )
    {
      return  true;
    }

    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      キャラテクスチャ画像貼り付け依頼
  * @param[in]  rRequestData:  貼り付け時の依頼データ
  * @return     依頼成功 true / 依頼失敗 false
  */
  //--------------------------------------------------------------------------------------------
  bool AppToolCharaTexPaneReplace::RequestReplace( const REQUEST_REPLACE_DATA& rRequestData )
  {
    GFL_ASSERT( rRequestData.pReplacePane != NULL );

    m_pInReplacePane  = NULL;
    m_pReplaceData    = NULL;

    //  オフスクリーン設定をしている場合は初期化
    if( m_pRenderinManager->GetOffScreenRenderTargetTexture() == m_pRenderTargetTexture )
    {
      m_pRenderinManager->UnsetOffScreenRenderTargetTexture( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
    }

    //  カメラのポインタが同じな時はこれは設定したカメラ内容が消える
    if( m_pRenderinManager->GetCamera( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ) == m_pCamera )
    {
      m_pRenderinManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
    }

    //  モデルロード開始
    m_replaceDataBuffer.pCharaModel->StartLoad( rRequestData.charaId, rRequestData.dressUpParam );
    m_replaceDataBuffer.pCharaModel->SetVisible( true );
    m_replaceDataBuffer.pCharaModel->SetMotionAnime( rRequestData.animeId, rRequestData.animeFrameNum, false );
    m_replaceDataBuffer.pCharaModel->SetPos( rRequestData.modelPos );
    m_replaceDataBuffer.pCharaModel->SetRot( rRequestData.modelRot );
    m_replaceDataBuffer.pCharaModel->SetEye( rRequestData.eyeIndex );

    m_replaceDataBuffer.requestData    = rRequestData;
    m_replaceDataBuffer.bReplaceStart  = true;

    //  カメラ設定
    {
      gfl2::math::Vector3 targetPos = rRequestData.camPos;
      targetPos.z += m_camTargetLength;

      m_pCamera->SetPosition( rRequestData.camPos );
      m_pCamera->SetTargetPosition( targetPos );

      m_pCamera->SetFovy( rRequestData.camFovy );
      //  アスペクト比は縦横同じ比率にしないとテクスチャーが間延びする
      m_pCamera->SetAspectRatio( 1.0f );
    }

    return  true;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      キャラテクスチャを貼り付けているか
  * @param[in]  pReplacePane: 指定ペインにテクスチャが貼り付けているか
  * @return     貼り付けている true / 貼り付けていない false
  */
  //--------------------------------------------------------------------------------------------
  bool AppToolCharaTexPaneReplace::IsReplacePane( const gfl2::lyt::LytPicture* pReplacePane )
  {
    if( pReplacePane == NULL )
    {
      return  false;
    }

    if( pReplacePane == m_pInReplacePane )
    {
      //  テクスチャを貼り付けている
      return  true;
    }

    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      貼り付け処理が動いているか
  */
  //--------------------------------------------------------------------------------------------
  bool AppToolCharaTexPaneReplace::IsReplaceRun()
  {
    if( m_replaceDataBuffer.pCharaModel->GetState() != CharaSimpleModel::STATE_TYPE_IDLE )
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
  void AppToolCharaTexPaneReplace::Update()
  {
    m_replaceDataBuffer.pCharaModel->Update();

    //  モデルロードが終わって貼り付ける状態になったので貼り付けるリストに登録
    if( m_replaceDataBuffer.pCharaModel->GetState() == CharaSimpleModel::STATE_TYPE_UPDATE )
    {
      if( m_replaceDataBuffer.bReplaceStart == true )
      {
        if( m_pReplaceData == NULL )
        {
          m_replaceDataBuffer.bReplaceStart = false;
          m_pReplaceData  = &m_replaceDataBuffer;

          m_replaceSeq  = 0;
        }
      }
    }

    //  ペインにキャラテクスチャを貼り付ける
    //  この貼り付けるは同フレーム内で一斉にすべてのペインに貼り付けることはできない
    //  一つ一つのモデルをオフスクリーンに転送しているから
    //  同時にやるのなら貼り付ける数分のオフスクリーンが必要だが、メモリの消費がやばい
    if( m_pReplaceData != NULL )
    {
      if( m_bTerminate == false )
      {
        enum
        {
          REPLACE_SEQ_STANBY  = 0,
          REPLACE_SEQ_SET,
          REPLACE_SEQ_CLEANUP,
          REPLACE_SEQ_CLEANUP_WAIT
        };

        //  貼り付ける準備
        if( m_replaceSeq == REPLACE_SEQ_STANBY )
        {
          GFL_ASSERT( m_replaceDataBuffer.pCharaModel->GetState() == CharaSimpleModel::STATE_TYPE_UPDATE );

          //  ターゲットが設定しない状況になるまで待つ
          if( ( m_pRenderinManager->GetOffScreenRenderTargetTexture() == NULL ) && ( m_pRenderinManager->GetCamera( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ) == NULL ) )
          {
            m_pRenderinManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_pCamera );
            m_pRenderinManager->SetOffScreenRenderTargetTexture( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_pRenderTargetTexture );
            ++m_replaceSeq;
          }
        }
        else
        if( m_replaceSeq == REPLACE_SEQ_SET )
        //  貼り付ける
        {
          GFL_ASSERT( m_replaceDataBuffer.pCharaModel->GetState() == CharaSimpleModel::STATE_TYPE_UPDATE );

          GFL_ASSERT( m_pRenderinManager->GetOffScreenRenderTargetTexture() == m_pRenderTargetTexture );

          if( m_bReplaceTex == true )
          {
            //  マテリアルのテクスチャ０に貼り付ける
            nw::lyt::Material* mat = m_pReplaceData->requestData.pReplacePane->GetMaterial();

            m_pRenderTargetTexture->GetColorTextureInfo( &m_texInfo );
            mat->SetTexMap( 0, &m_texInfo );
          }

          ++m_replaceSeq;
        }
        else
        if( m_replaceSeq == REPLACE_SEQ_CLEANUP )
        //  貼り付け終わった
        {
          GFL_ASSERT( m_replaceDataBuffer.pCharaModel->GetState() == CharaSimpleModel::STATE_TYPE_UPDATE );

          if( m_pRenderinManager->GetOffScreenRenderTargetTexture() == m_pRenderTargetTexture )
          {
            m_pRenderinManager->UnsetOffScreenRenderTargetTexture( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
          }

          //  カメラのポインタが同じな時はこれは設定したカメラ内容が消える
          if( m_pRenderinManager->GetCamera( app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN ) == m_pCamera )
          {
            m_pRenderinManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
          }

          //  モデルにはようはないので開放させる
          m_pReplaceData->pCharaModel->RequestFree();

          ++m_replaceSeq;
        }
        else
        if( m_replaceSeq == REPLACE_SEQ_CLEANUP_WAIT )
        {
          if( m_replaceDataBuffer.pCharaModel->GetState() == CharaSimpleModel::STATE_TYPE_IDLE )
          {
            //  モデルなどすべての処理が終わった段階

            //  開放して初めて貼り付け終わったとして記録する
            m_pInReplacePane  = m_pReplaceData->requestData.pReplacePane;

            //  参照を終了
            m_pReplaceData  = NULL;

            ++m_replaceSeq;
          }
        }
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief      テクスチャーバッファ取得
  */
  //--------------------------------------------------------------------------------------------
  void* AppToolCharaTexPaneReplace::GetTextureBuffer()
  {
    return  m_pRenderTargetTexture->GetTextureBuffer();
  }

  //  @brief クラスのコンストラクタで必ず呼び出す
  void AppToolCharaTexPaneReplace::_Constructor( app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, System::nijiAllocator* pVramAllocator )
  {
    GFL_ASSERT( pRenderinManager );
    GFL_ASSERT( pHeap );
    GFL_ASSERT( pVramAllocator );

    static const gfl2::gfx::Color CLEAR_COLOR( 72.0f/255.0f, 216.0f/255.0f, 237.0f/255.0f, 1.0f );

    //  専用Heap確保
    {
      m_pHeap = GFL_NEW( pHeap->GetDeviceHeap() ) app::util::Heap();
      m_pHeap->LocalHeapCreate( pHeap->GetSystemHeap(), pHeap->GetDeviceHeap(), GetUseSystemHeapSize(), GetUseDeviceHeapSize() );
    }

    m_pRenderinManager      = pRenderinManager;
    m_pVramAllocator        = pVramAllocator;
    m_pRenderTargetTexture  = GFL_NEW( m_pHeap->GetDeviceHeap() ) app::util::RenderTargetTexture();

    //  描画ターゲット作成
    {
      app::util::RenderTargetTexture::Description renderTargetTextureDesc;

      gfl2::math::Vector2 textureSize = _GetTextureSize();

      renderTargetTextureDesc.width  = (u32)textureSize.x;
      renderTargetTextureDesc.height = (u32)textureSize.y;
      renderTargetTextureDesc.colorFormat = _GetTexturePixelColorFormat();

      m_pRenderTargetTexture->Create( m_pVramAllocator, m_pHeap->GetDevGLHeapAllocator(), renderTargetTextureDesc );

      m_pRenderTargetTexture->SetClearColor( CLEAR_COLOR );
    }

    //  カメラ作成
    {
      app::util::AppRenderingManager::CreateDefaultCamera( m_pHeap, &m_pDrawEnvNodeCamera, &m_pCamera, app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN );
      //  カメラの位置と注視点との距離を取得
      m_camTargetLength = m_pCamera->GetTargetPosition().z - m_pCamera->GetPosition().z;
    }

    //  貼り付けモデル作成
    {
      m_replaceDataBuffer.pCharaModel = GFL_NEW( m_pHeap->GetDeviceHeap() ) CharaSimpleModel();

      CharaSimpleModel::SETUP_PARAM               setupParam;
      {
        setupParam.pUseHeap               = m_pHeap->GetDeviceHeap();
        setupParam.aRenderPlaceType[ 0 ]  = app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN;
        setupParam.pRenderingManager      = m_pRenderinManager;
      }

      CharaSimpleModel::SETUP_CREATE_MODEL_PARAM  setupModelParam;
      {
        setupModelParam.pCharaModelFactory            = m_pCharaModelFactory;
        setupModelParam.pDressUpModelResourceManager  = m_pDressUpModelResourceManager;
      }

      m_replaceDataBuffer.pCharaModel->Setup( setupParam, setupModelParam );
    }
  }

  //  @brief  テクスチャのピクセルカラーフォーマット
  gfl2::gfx::Format AppToolCharaTexPaneReplace::_GetTexturePixelColorFormat()
  {
    return  gfl2::gfx::Format::A8R8G8B8;
  }

  //  @brief  テクスチャのサイズ
  gfl2::math::Vector2  AppToolCharaTexPaneReplace::_GetTextureSize()
  {
    static const gfl2::math::Vector2  size( 128, 128 );

    return  size;
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
