//======================================================================
/**
 * @file FieldEncountEffect.cpp
 * @date 2015/09/11 12:37:38
 * @author saita_kazuki
 * @brief 戦闘導入演出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/Encount/FieldEncountEffect.h"

#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

// arc 動的ヘッダを使用
#define DEMO_SEQ_SCRIPT_GAIX "arc_index/demo_seq_script.gaix"
#include GFL_DYNAMIC_HEADER( DEMO_SEQ_SCRIPT_GAIX )

// gflib
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNodeFactory.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_TextureResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <gfx/include/gfl2_GFGL.h>

// system
#include "System/include/HeapDefine.h"
#include "System/include/nijiAllocator.h"

// gamesys
#include "GameSys/include/GameManager.h"

// app
#include "AppLib/include/Fade/DemoFade.h"

// demo
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/FieldRenderingPipeLineManager.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )

//------------------------------------------------------------------------------
// 静的メンバ変数
//------------------------------------------------------------------------------

// 種類ごとのデモ開始前処理を行う関数
// @caution EffectTypeと同じ並びにすること
const EncountEffect::InitializeFunc EncountEffect::INITIALIZE_FUNC[ EncountEffect::EFFECT_KIND_MAX ] =
{
  &EncountEffect::initializeFunc_Default,
};

// 種類ごとの初期化処理を行う関数
// @caution EffectTypeと同じ並びにすること
const EncountEffect::StartPreFunc EncountEffect::START_PRE_FUNC[ EncountEffect::EFFECT_KIND_MAX ] =
{
  &EncountEffect::startPreFunc_Default,
};

//------------------------------------------------------------------------------
// PUBLIC
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */
EncountEffect::EncountEffect()
  : m_pHeap( NULL)
  , m_pDemoFade( NULL)
  , m_pAllocator( NULL)
  , m_pTextureResourceNode( NULL)
  , m_isCapture( false)
  , m_isInitialized( false)
  , m_isStartFlash( false)
  , m_seq( 0 )
  , m_flashSync( DEFAULT_FLASH_TOTAL_SYNC )
  , m_flashNum( 1 )
  , m_flashInterval( -2 )
  , m_flashIntervalCnt( 0 )
{
  gfl2::std::MemClear( &m_desc, sizeof( m_desc));
}

/**
 * @brief デストラクタ
 */
EncountEffect::~EncountEffect()
{
}

/**
 * @brief 初期化
 * @param pParent 親ヒープ
 * @param desc 初期化情報
 */
void EncountEffect::Initialize( gfl2::heap::HeapBase* pParent, const Description& desc)
{
  if( m_isInitialized)
  {
    GFL_ASSERT_MSG( 0, "Alrady initialized.");
    this->Terminate();
  }

  m_desc = desc;
  if( m_desc.effectKind >= EFFECT_KIND_MAX )
  {
    GFL_ASSERT_MSG( 0, "EncountEffect kind is invalid.");
    m_desc.effectKind = EFFECT_KIND_DEFAULT;
  }

  this->createHeap( pParent);
  this->createDemoFade();
  this->initialize();

  m_isInitialized = true;
}

/**
 * @brief 破棄
 */
void EncountEffect::Terminate()
{
  this->terminate();
  this->deleteDemoFade();
  this->deleteHeap();

  m_isInitialized = false;
}

/**
 * @brief 再生前準備
 * @note キャプチャー処理の後で呼ぶ
 */
void EncountEffect::Setup()
{
  // デモごとの開始前処理
  (this->*START_PRE_FUNC[ m_desc.effectKind ])();

  // NMCat[351] フェード中でなければ、フェード下画面をフェードアウトさせる
  // 上画面のタイミングと合わせる ＋ 通常の戦闘導入演出、外部再生の戦闘導入演出のどちらも対応させるためここに書く
  this->requestFadeOutLower();
}

/**
 * @brief 開始
 * @param isRequestFlash trueでフラッシュを開始
 * @param totalSync 合計時間
 * @param num 回数
 * @param interval 間隔
 */
void EncountEffect::Start( bool isRequestFlash, u32 totalSync, u32 flashNum, s32 flashInterval )
{
  GFL_ASSERT( m_pDemoFade);

  // デモ開始
  u32 demoID = this->getDemoID( m_desc.effectKind);
  m_pDemoFade->StartDemo( demoID);

  if( isRequestFlash )
  {
    this->StartFlash( totalSync, flashNum, flashInterval );
  }
}

/**
 * @brief シーケンス再生が開始しているか
 * @param isRequestFlash trueでフラッシュ更新も待つ
 * @retval true 開始している
 * @retval false まだ開始していない
 */
bool EncountEffect::IsStart( bool isRequestFlash )
{
  if( m_pDemoFade == NULL)
  {
    return true;
  }

  if( isRequestFlash )
  {
    if( this->UpdateFlash() == false )
    {
      return false;
    }
  }

  u32 now =  static_cast<u32>( m_pDemoFade->GetState());
  if( now < static_cast<u32>( AppLib::Fade::DemoFade::STATE_running))
  {
    return false;
  }

  return m_pDemoFade->GetSequence()->SeqComFunc_IsReadyToPlaySequenceFile();
}

/**
 * @brief フィールドキャプチャーが必要か
 * @retval true 必要
 * @retval false 不要
 */
bool EncountEffect::IsNeedFieldCapture() const
{
  return m_isCapture;
}

/**
 * @brief フィールドキャプチャーリクエスト
 */
void EncountEffect::RequestFieldCapture()
{
  GFL_ASSERT_MSG( m_isCapture, "Don't need capture field.\n");
  gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);
  GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetFieldmap()->GetFieldRenderingPipeLineManager()->RequestTransferColorBuffer( pTexture);
}

/**
 * @brief シーケンスプレイヤーの初期化完了後にシーケンス再生を待機させるモードの設定
 * @param trueにするとシーケンスプレイヤー初期化完了後にシーケンスが再生されなくなる
 * @note デフォルトはfalseです。設定する場合はInitialize後に呼んでください
 * @note trueに設定した場合、明示的にfalseを設定しないとシーケンスが再生されません
 */
void EncountEffect::SetSequenceStartupWaitMode( bool isEnable)
{
  GFL_ASSERT_MSG( m_pDemoFade, "DemoFade is NULL.\n");
  if( m_pDemoFade)
  {
    m_pDemoFade->SetSequenceStartupWaitMode( isEnable);
  }
}

/**
 * @brief 初期化されているか
 * @retval true 初期化されている
 * @retval false 未初期化
 */
bool EncountEffect::IsInitialized() const
{
  return m_isInitialized;
}

/**
  * @brief 終了待ち
  * @retval true 終了
  * @retval false 未初期化
  */
bool EncountEffect::IsFinish() const
{
  if( m_pDemoFade == NULL)
  {
    return true;
  }
  if( m_pDemoFade->IsEndStatus() == true)
  {
    return true;
  }
  //通信エラー時など同期が解けない場合があるので。
  m_pDemoFade->GetSequence()->SystemSync();
  return false;
}

/**
 * @brief 上画面フラッシュ開始
 * @param totalSync 合計時間
 * @param num 回数
 * @param interval 間隔
 */
void EncountEffect::StartFlash( u32 totalSync, u32 num, s32 interval )
{
  this->initializeFlashParameter( totalSync, num, interval );

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  const gfl2::math::Vector4 start_col = this->getFlashColor( 0.0f );
  const gfl2::math::Vector4 end_col = this->getFlashColor( 255.0f );
  pFadeManager->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col, m_flashSync );

  m_isStartFlash = true;
}

/**
 * @brief 上画面フラッシュ更新
 * @retval true 終了した
 * @retval false 実行中
 */
bool EncountEffect::UpdateFlash()
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  enum Seq
  {
    SEQ_WAIT_OUT,
    SEQ_WAIT_IN,
    SEQ_WAIT_IN_FINAL,
    SEQ_END,
  };

  // StartFlashが呼ばれないでこの関数が呼ばれた場合はアサートを出して終了シーケンスに移行
  if( m_isStartFlash == false )
  {
    GFL_ASSERT( 0 );
    m_isStartFlash = true;
    m_seq = SEQ_END;
  }

  switch( m_seq )
  {
  case SEQ_WAIT_OUT:
    // フェードアウトは必ず待つ
    if( pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER ) )
    {
      pFadeManager->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, m_flashSync );
      m_flashIntervalCnt = 0;
      m_seq++;
    }
    break;

  case SEQ_WAIT_IN:
    {
      // フェードインを指定フレーム待つ
      s32 waitSync = m_flashSync + m_flashInterval;
      if( m_flashIntervalCnt < waitSync )
      {
        m_flashIntervalCnt++;
        break;
      }

      // フラッシュ回数が残っていればフェードアウトリクエスト
      m_flashNum--;
      if( m_flashNum > 0 )
      {
        // 間隔がプラスならこの値を使う
        u32 sync = m_flashSync;

        // 間隔がマイナスならフェード時間を調整する
        if( m_flashInterval < 0 )
        {
          sync = waitSync;
        }

        // 開始色はフェードの現在の色を使いたいため再設定しない
        const gfl2::math::Vector4 end_col = this->getFlashColor( 255.0f );
        pFadeManager->RequestOut( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, NULL, &end_col, sync );

        m_seq = SEQ_WAIT_OUT;
        break;
      }

      // フラッシュ回数が残っていないのでフェードを待つシーケンスに移行
      m_seq++;
    }
    break;

  case SEQ_WAIT_IN_FINAL:
    if( pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER ) == false )
    {
      break;
    }
    m_seq++;
    // break;

  case SEQ_END:
    return true;
    // break;

  default:
    GFL_ASSERT( 0 );
    return true;
    // break;
  }

#if PM_DEBUG
  static u32 update_count = 0;
  SAITA_PRINT( "EncountEffect : UpdateFlash : Frame = %d : Seq = %d\n", update_count, m_seq );
  update_count++;
#endif // PM_DEBUG

  return false;
}

//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------

/**
 * @brief 初期化
 */
void EncountEffect::initialize()
{
  m_pAllocator = GFL_NEW( m_pHeap) System::nijiAllocator( m_pHeap);

  // デモごとの初期化
  (this->*INITIALIZE_FUNC[ m_desc.effectKind ])();

  // 動的ヘッダロード
  GFL_LOAD_HEADER_HASH( DEMO_SEQ_SCRIPT_GAIX, 256 );
}

/**
 * @brief 終了
 */
void EncountEffect::terminate()
{
  // 動的ヘッダアンロード
  GFL_UNLOAD_HEADER_HASH( DEMO_SEQ_SCRIPT_GAIX );

  GFL_SAFE_DELETE( m_pTextureResourceNode);
  GFL_SAFE_DELETE( m_pAllocator);

  gfl2::std::MemClear( &m_desc, sizeof( m_desc));

  m_isCapture = false;
}

/**
 * @brief ヒープ作成
 * @param pParent 親ヒープ
 */
void EncountEffect::createHeap( gfl2::heap::HeapBase* pParent)
{
  const u32 HEAP_SIZE_SELF_LOCAL  = 0x19000;

  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(
    pParent,
    HEAP_SIZE_SELF_LOCAL + HEAP_SIZE_DEMO_SYSTEM + HEAP_SIZE_DEMO_DEVICE,
    gfl2::heap::HEAP_TYPE_EXP,
    false,
    "EncountEffect"
    );
}

/**
 * @brief ヒープ削除
 */
void EncountEffect::deleteHeap()
{
  if( m_pHeap)
  {
    GFL_DELETE_HEAP( m_pHeap);
    m_pHeap = NULL;
  }
}

/**
 * @brief デモフェード生成
 */
void EncountEffect::createDemoFade()
{
  m_pDemoFade = GFL_NEW( m_pHeap ) AppLib::Fade::DemoFade();
  m_pDemoFade->Initialize( m_pHeap , HEAP_SIZE_DEMO_SYSTEM, HEAP_SIZE_DEMO_DEVICE);
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->AddFade( gfl2::Fade::DISP_CUSTOM_UPPER, m_pDemoFade);
}

/**
 * @brief デモフェード削除
 */
void EncountEffect::deleteDemoFade()
{
  if( m_pDemoFade == NULL)
  {
    return;
  }
  if( m_pDemoFade->IsEndStatus() == false)
  {
    return;
  }

  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->RemoveFade( gfl2::Fade::DISP_CUSTOM_UPPER);
  GFL_SAFE_DELETE( m_pDemoFade);
}

/**
 * @brief キャプチャテクスチャリソースの生成
 */
void EncountEffect::createCaptureTextureResourceNode()
{
  GFL_ASSERT_MSG( m_isCapture, "Don't need create texture resource.\n");

  // キャプチャテクスチャを取得
  gfl2::gfx::Texture* pTexture = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager)->GetColorBufferTexture( gfl2::Fade::FadeManager::COLOR_BUFFER_UPPER);

  // テクスチャ生成ファクトリー取得
  const c8* pFactoryName = gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory::GetFactoryTypeName();
  gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pFactory =
    gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(
    gfl2::renderingengine::scenegraph::resource::IResourceData::Type( pFactoryName)
    );
  gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory* pTextureResourceNodeFactory = 
    reinterpret_cast<gfl2::renderingengine::scenegraph::resource::TextureResourceNodeFactory*>(pFactory);

  // テクスチャリソースノード作成
  m_pTextureResourceNode = pTextureResourceNodeFactory->CreateResourceNode( m_pAllocator, "into_battle_screen00.tga", pTexture, false);
}

/**
 * @brief デモID取得
 */
u32 EncountEffect::getDemoID( EffectKind kind)
{
  // demo_seq_script.garcはファイル名ソートをしているため、新規追加シーケンスがあるとデモIDがずれる可能性がある
  // ずれても動作するように動的ヘッダの仕組みを使って戦闘導入演出で使用するデモIDを取得します

  switch( kind)
  {
  case EFFECT_KIND_DEFAULT:
    return GFL_VALUE_DEFINE_IN_HEADER_FAST( GARC_demo_seq_script_sys_fadeinbattle_default_BSEQ , DEMO_SEQ_SCRIPT_GAIX );

  default:
    GFL_ASSERT( 0);
    break;
  }

  return GFL_VALUE_DEFINE_IN_HEADER_FAST( GARC_demo_seq_script_sys_fadeinbattle_default_BSEQ , DEMO_SEQ_SCRIPT_GAIX );
}

/**
 * @brief 下画面フェードリクエスト
 */
void EncountEffect::requestFadeOutLower()
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

  const gfl2::Fade::DISP disp = gfl2::Fade::DISP_LOWER;

  // isEnd
  // isEndStatus
  // の状態は下記のようになる(LayoutFade参照)
  //
  // Wait OutReq  →→→  OutFin  →→→  InReq  →→→  InFin Wait
  // true         false           true           false         true
  // true         false           false          false         true
  const bool isEnd        = pFadeManager->IsEnd( disp );
  const bool isEndStatus  = pFadeManager->IsEndStatus( disp );

  // フェードアウト中、フェードイン中であるならアサートを出して何もしない
  if( isEnd == false && isEndStatus == false )
  {
    GFL_ASSERT( 0 );
    return;
  }

  // フェードアウト可能な状態のときだけフェードアウトリクエストを出す
  if( isEnd && isEndStatus )
  {
    const gfl2::math::Vector4 start_col( 0, 0, 0, 0.0f );
    const gfl2::math::Vector4 end_col( 0, 0, 0, 255.0f );
    pFadeManager->RequestOut( disp, gfl2::Fade::FADE_TYPE_ALPHA, &start_col, &end_col );
  }
}

/**
 * @brief フラッシュパラメーター初期化
 * @param totalSync 合計時間
 * @param num 回数
 * @param interval 間隔
 */
void EncountEffect::initializeFlashParameter( u32 totalSync, u32 num, s32 interval )
{
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_SYNC_ENABLE ) == 1 )
  {
    totalSync = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_SYNC );
    num = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_NUM );
    if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_INTERVAL_REVERSE ) == 1 )
    {
      interval = -FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_INTERVAL );
    }
    else
    {
      interval = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_INTERVAL );
    }
  }
#endif // PM_DEBUG

  if( totalSync < 2 )
  {
    GFL_ASSERT( 0 );
    totalSync = 2;
  }

  s32 half = totalSync / 2;
  if( (interval < 0) &&
      (gfl2::math::Abs( interval ) > half)
    )
  {
    GFL_ASSERT( 0 );
    interval = 0;
  }

  m_seq               = 0;
  m_flashSync         = half;
  m_flashInterval     = interval;
  m_flashNum          = num;
  m_flashIntervalCnt  = 0;

  m_isStartFlash      = false;
}

/**
 * @brief フラッシュの色を取得
 * @param alpha アルファ値
 */
gfl2::math::Vector4 EncountEffect::getFlashColor( f32 alpha ) const
{
  f32 color = 205.0f;
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_SYNC_ENABLE ) == 1 )
  {
    color = static_cast<f32>( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENC_EFF_FLASH_COLOR ) );
  }
#endif // PM_DEBUG

  if( (alpha < 0.0f) || (alpha > 255.0f) )
  {
    GFL_ASSERT( 0 );
    alpha = gfl2::math::Clamp( alpha, 0.0f, 255.0f );
  }

  return gfl2::math::Vector4( color, color, color, alpha );
}

//------------------------------------------------------------------------------
// 各エフェクトごとの初期化処理
//------------------------------------------------------------------------------

/**
 * @brief 初期化処理 (EFFECT_KIND_DEFAULT)
 */
void EncountEffect::initializeFunc_Default()
{
  m_isCapture = true;
}

//------------------------------------------------------------------------------
// 各エフェクトごとのデモ開始前処理
//------------------------------------------------------------------------------

/**
 * @brief デモ開始前処理 (EFFECT_KIND_DEFAULT)
 */
void EncountEffect::startPreFunc_Default()
{
  this->createCaptureTextureResourceNode();
  m_pDemoFade->SetReplaceTexture( 0, m_pTextureResourceNode);
}

GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )
