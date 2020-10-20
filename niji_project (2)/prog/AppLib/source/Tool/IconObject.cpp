//======================================================================
/**
 * @file IconTex.cpp
 * @date 2015/10/14 12:30:03
 * @author uchida_yuto
 * @brief 2Dアイコンオブジェクト(テクスチャーを非同期でロードして張り付ける)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/IconObject.h"

//  gflib2のインクルード
#include <base/include/gfl2_Singleton.h>

//  nijiのインクルード
#include "AppLib/include/Tool/app_tool_LytTexReplaceSystem.h"
#include "GameSys/include/GameManager.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  必要なヒープサイズを取得
  //  正確に取得するにはテクスチャーのフォーマットも必要な気が
  u32 IconObject::GetUseHeapSize( const u32 width, const u32 height, const bool bCompFile, const gfl2::gfx::Format colorFormat )
  {
      //  テクスチャーサイズ
    return ( GetUseSystemHeapSize() + GetUseDeviceHeapSize( width, height, bCompFile, colorFormat ) );
  }

  u32 IconObject::GetUseSystemHeapSize()
  {
    return 2048;
  }

  u32 IconObject::GetUseDeviceHeapSize( const u32 width, const u32 height, const bool bCompFile, const gfl2::gfx::Format colorFormat )
  {
    u32 size  = GetUseTexHeapSize( width, height, colorFormat );

    //  圧縮していると展開時にサイズの2倍以上のバイト数を取る。
    if( bCompFile == true )
    {
      size *= 2;
    }
    size += 1024 * 2;

    return  size;
  }

  u32 IconObject::GetUseTexHeapSize( const u32 width, const u32 height, const gfl2::gfx::Format colorFormat )
  {
    u32 pixelSize = 4;
    if( colorFormat == gfl2::gfx::Format::A8R8G8B8 )
    {
      pixelSize = 4;
    }

    u32 orgTexSize = ( width * height * pixelSize );
    u32 newTexSize = 2;
    //  2のべき乗にしないとアウト
    {
      while( newTexSize < orgTexSize )
      {
        newTexSize *= 2;
      }
    }

    u32 texSize = newTexSize + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE;

#if defined(GF_PLATFORM_CTR)
#elif defined(GF_PLATFORM_WIN32)
    // @note PC版のサイズは適当です。4倍あれば足りるはず。
    texSize   = newTexSize * 4 + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE;  //!< テクスチャサイズ
#endif

    return  texSize;
  }

  IconObject::IconObject( const INIT_PARAM& rInitParam, gfl2::heap::HeapBase* pHeap )
  {
    _Clear();

    GFL_ASSERT( pHeap );
    GFL_ASSERT( rInitParam.arcId != gfl2::fs::ArcFile::ARCDATID_NULL );

    m_initParam = rInitParam;

    //  クラス内部のみでつかうヒープ確保
    {
      m_pLocalHeap        = GFL_CREATE_LOCAL_HEAP( pHeap, GetUseSystemHeapSize(), gfl2::heap::HEAP_TYPE_EXP, false );
      m_pLocalTexHeap     = GFL_CREATE_LOCAL_HEAP( pHeap, GetUseDeviceHeapSize( rInitParam.sizeWidth, rInitParam.sizeHeight, rInitParam.bCompFile, rInitParam.colorFormat ), gfl2::heap::HEAP_TYPE_EXP, false );
    }

    {
      gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      m_pLytTexData = GFL_NEW( m_pLocalHeap ) app::tool::LytTexData( m_pLocalTexHeap, GetUseTexHeapSize( rInitParam.sizeWidth, rInitParam.sizeHeight, rInitParam.colorFormat ), 128, man );
    }
  }

  IconObject::~IconObject()
  {
    GFL_SAFE_DELETE( m_pLytTexData );

    GFL_DELETE_HEAP( m_pLocalTexHeap );
    GFL_DELETE_HEAP( m_pLocalHeap );
  }

  //  初期化
  bool IconObject::InitFunc()
  {
    return  true;
  }

  //  終了処理
  bool IconObject::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNextSeq  = true;
      //  クラス破棄できるようになるまで繰り返す
      if( m_pLytTexData->IsModuleFree() == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //  テクスチャーをロードして貼り付け
  IconObject::ReplaceTexResult IconObject::ReplaceTex( gfl2::lyt::LytPicture* pPane, const gfl2::fs::ArcFile::ARCDATID dataId )
  {
    GFL_ASSERT( pPane );
    GFL_ASSERT( dataId != gfl2::fs::ArcFile::ARCDATID_NULL );

    m_pPicturePane  = pPane;

    if( dataId == m_loadedArcDataId )
    {
      //  同じデータをすでにロードしている
      app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &m_pLytTexData->GetTexInfo(), m_pPicturePane, 0, 0 );
      m_pPicturePane->SetVisible( true );

      return  REPLACE_TEX_RESULT_LOAD_DUBLICATION;
    }

    m_updateSeq       = UPDATE_SEQ_LOAD_START;
    m_loadedArcDataId = (u32)DATA_ID_NONE;
    m_arcDataId       = dataId;

    //  ロード中は非表示
    m_pPicturePane->SetVisible( false );

    return  REPLACE_TEX_RESULT_LOAD_IN;
  }

  //  テクスチャーの貼り付けが終了している
  bool IconObject::IsReplaceTex()
  {
    //  idleの場合はそもそも貼り付けすらしていないので終了とみなす
    return  ( ( m_updateSeq == UPDATE_SEQ_LOAD_END ) || ( m_updateSeq == UPDATA_SEQ_IDLE ) );
  }

  //  更新処理
  void IconObject::UpdateFunc()
  {
    if( m_updateSeq == UPDATA_SEQ_IDLE )
    {
      
    }

    if( m_updateSeq == UPDATE_SEQ_LOAD_START )
    {
      //  ロード開始！

      bool  bNextSeq  = false;
      if( m_pLytTexData->IsLoadRun() == false )
      {
        //  まだ一度もロードしていない
        bNextSeq  = true;
      }
      else
      if( m_pLytTexData->IsLoadTextureFinished() == true )
      {
        //  ロードをして終わった
        bNextSeq  = true;
      }
      else
      {
        //  ロード中なのでロードしてはいけない
      }

      if( bNextSeq == true )
      {
        m_pLytTexData->LoadTextureAsync( m_pLocalTexHeap, m_initParam.arcId, m_arcDataId, m_initParam.bCompFile );
        ++m_updateSeq;
      }
    }
    else
    if( m_updateSeq == UPDATE_SEQ_LOAD_RUN )
    {
      //  ロード中

      if( m_pLytTexData->LoadTexture() == true )
      {
        GFL_ASSERT( m_pPicturePane );

        if( m_bReplaceTex == true )
        {
          app::tool::LytTexReplaceSystem::ReplaceTextureStatic( &m_pLytTexData->GetTexInfo(), m_pPicturePane, 0, 0 );
          m_pPicturePane->SetVisible( true );
        }

        //  データをロードidを保管（再ロード用）
        m_loadedArcDataId = m_arcDataId;

        ++m_updateSeq;
      }
    }
    else
    {
      //  ロード終了
      m_updateSeq = UPDATA_SEQ_IDLE;
    }
  }

  //  テクスチャーデータ取得
  void* IconObject::GetTexData()
  {
    if( m_pLytTexData->LoadTexture() == false )
    {
      return  NULL;
    }

    return  m_pLytTexData->GetTexData();
  }

  //  テクスチャーデータサイズを取得
  u32 IconObject::GetTexSize()
  {
    if( m_pLytTexData->LoadTexture() == false )
    {
      return  0;
    }

    return  m_pLytTexData->GetTexSize();
  }

  //  テクスチャーデータ制御クラスを取得
  app::tool::LytTexData*  IconObject::GetTexDataCtrl()
  {
    if( m_pLytTexData == NULL )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  m_pLytTexData;
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
