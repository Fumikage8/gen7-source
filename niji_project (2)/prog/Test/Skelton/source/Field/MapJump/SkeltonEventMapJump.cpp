#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
@file event_mapjump.cpp
@brief 擬似マップジャンプのイベント
@date 2015.01.23
@author saita_kazuki
*/
//*************************************************************************************************

#include "Test/Skelton/include/Field/MapJump/SkeltonEventMapJump.h"
#include "GameSys/include/GameManager.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"
#include <Fade/include/gfl2_FadeManager.h>

//#include "gamesystem/gamemanager.h"
//#include "gamesystem/gamedata.h"
//#include "gamesystem/gameeventmanager.h"
//#include "field/fieldmap.h"

//#include "field/movemodel_manager.h"
//#include "field/movemodel.h"
//#include "field/field_transform.h"

#include "Test/Skelton/include/Field/SkeltonFieldTransform.h"
#include <Collision/include/gfl2_CollisionModel.h>
#include "Test/Skelton/include/SkeltonDebugInfo.h"

//#include "field/FieldDebug.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

  //-------------------------------------------------------------------------------------------------
  // イベント起動
  //-------------------------------------------------------------------------------------------------
  void SkeltonEventMapJump::CallMapJumpEvent( GameSys::GameManager * gmgr , const JUMP_DATA* pJumpData, SkeltonModel* pPlayer, const SkeltonFieldTransform& transform, gfl2::collision::CollisionModel* pCollisionModel )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();

  SkeltonEventMapJump* pEvent = GFL_NEW( pGameEventManager->GetHeap()) SkeltonEventMapJump( pGameEventManager->GetHeap());
  pGameEventManager->CallEvent( pEvent);

  // 向きは未設定なら今のそのままの値を使う
  f32 angle = pJumpData->angle;
  if( angle == JumpDataManager::JUMP_DATA_ANGLE_NONE)
  {
    angle = transform.GetAngle();
  }

  const Setting setting =
  {
    pJumpData->pos,
    pJumpData->fadeFrame,
    angle,
    pCollisionModel,
    pPlayer
  };

  pEvent->Setup( setting);
}

//-------------------------------------------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------------------------------------------
SkeltonEventMapJump::SkeltonEventMapJump(gfl2::heap::HeapBase * pHeap ) :
  GameSys::GameEvent(pHeap),
  m_seq( 0),
  m_setting(),
  pPlayer( NULL )
{
}

//-------------------------------------------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------------------------------------------
SkeltonEventMapJump::~SkeltonEventMapJump()
{
}

//-------------------------------------------------------------------------------------------------
// 起動チェック
//-------------------------------------------------------------------------------------------------
bool SkeltonEventMapJump::BootChk( GameSys::GameManager * gmgr )
{
  return true;
}

//-------------------------------------------------------------------------------------------------
// 初期化処理
//-------------------------------------------------------------------------------------------------
void SkeltonEventMapJump::InitFunc( GameSys::GameManager * gmgr )
{
  m_seq = 0;
}

//-------------------------------------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------------------------------------
GameSys::GMEVENT_RESULT SkeltonEventMapJump::MainFunc( GameSys::GameManager * gmgr )
{
  enum MainSeq
  {
    START_FADE_OUT,   // フェードアウト開始
    WAIT_FADE_OUT,    // フェードアウト終了待ち
    PLAYER_UPDATE,    // プレイヤー情報更新
    WAIT_FADE_IN,     // フェードイン終了待ち
  };

  if ( !pPlayer )
  {
    return GameSys::GMEVENT_RES_FINISH;
  }

  switch( m_seq)
  {
  case START_FADE_OUT:
    {
      // SkeltonDebugInfo::m_MapjumpFadeFlag = true;
      // SkeltonDebugInfo::m_FadeAlpha = 0.0f;

      // フェードアウト開始
      gfl2::Fade::FadeManager* pFade = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

      gfl2::math::Vector4 start_col( 0.0f, 0.0f, 0.0f, 0.0f );
      gfl2::math::Vector4 end_col( 0.0f, 0.0f, 0.0f, 255.0f );

      pFade->RequestOut(
        gfl2::Fade::DISP_DOUBLE,
        gfl2::Fade::FADE_TYPE_ALPHA,
        &start_col,
        &end_col,
        m_setting.fadeFrame
        );

      m_seq++;
      break;
    }

  case WAIT_FADE_OUT:
    {

      // フェードアウト待ち
      gfl2::Fade::FadeManager* pFade = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

      if( pFade->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        this->AddSeqNo();
        /* FALL THROUGH */
      }
      else
      {
        break;
      }
    }

  case PLAYER_UPDATE:
    {
      gfl2::math::Vector4 newPos( m_setting.jumpPos);
      newPos *= SkeltonDebugInfo::m_MapScale;
      newPos.y = this->GetHeight( newPos);
      pPlayer->GetTransform()->SetTranslation( newPos);
      pPlayer->GetTransform()->SetAngle( m_setting.angle);

      // フェードイン開始
      gfl2::Fade::FadeManager* pFade = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFade->RequestIn(
        gfl2::Fade::DISP_DOUBLE,
        gfl2::Fade::FADE_TYPE_ALPHA,
        m_setting.fadeFrame
        );

      m_seq++;
    }
    break;

  case WAIT_FADE_IN:
    {
      // フェード待ち
      gfl2::Fade::FadeManager* pFade = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFade->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
      {
        this->AddSeqNo();
        /* FALL THROUGH */
      }
      else
      {
        break;
      }
    }

  default:
    {
      GFL_PRINT("MapJump Event End !!\n");
      return GameSys::GMEVENT_RES_FINISH;
    }
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//-------------------------------------------------------------------------------------------------
// 終了処理
//-------------------------------------------------------------------------------------------------
void SkeltonEventMapJump::EndFunc( GameSys::GameManager * gmgr )
{
}

//-------------------------------------------------------------------------------------------------
// セットアップ
//-------------------------------------------------------------------------------------------------
void SkeltonEventMapJump::Setup( const Setting& setting)
{
  std::memcpy( &m_setting, &setting, sizeof(Setting));
  pPlayer = setting.pPlayer;
}


//-------------------------------------------------------------------------------------------------
// 高さ取得
//-------------------------------------------------------------------------------------------------
f32 SkeltonEventMapJump::GetHeight( const gfl2::math::Vector4& pos)
{
  static const u32 MAX_HIT_COUNT = 10;
  // コリジョンライブラリ、コールバックインターフェイス
  class MapCallbackable : public gfl2::collision::ICollisionModelCallbackable
  {
  public:
    u32 m_HitCount;
    f32 m_HitHeightTable[ MAX_HIT_COUNT ];

    MapCallbackable() : m_HitCount( 0 )
    {
      for( u32 i = 0; i < MAX_HIT_COUNT; i++ )
      {
        m_HitHeightTable[ i ] = 0.0f;
      }
    }

    virtual ~MapCallbackable()
    {
    }

    // 当たり判定コールバック関数
    virtual bool CollisionModelHeightCallback( const gfl2::renderingengine::scenegraph::resource::CollisionTriangle* pTriangle, f32 height )
    {
      // 高さを記憶
      m_HitHeightTable[ m_HitCount ] = height;
      m_HitCount++;

      if( m_HitCount >= MAX_HIT_COUNT )
      {
        // 衝突検出終了
        return false;
      }
      else
      {
        // 衝突検出続行
        return true;
      }
    }

    // 指定の高さに一番近い高さを取得する、ヒットしていない場合は指定の高さがそのまま返される
    f32 GetNearHight( f32 height )
    {
      if( m_HitCount == 0 )
      {
        return height;
      }

      height += 10.0f;

      f32 resultHeight( m_HitHeightTable[0] );
      f32 resultDiff( gfl2::math::Abs( resultHeight - height ) );
      for( u32 i = 1; i < m_HitCount; i++ )
      {
        f32 diff( gfl2::math::Abs( m_HitHeightTable[i] - height ));
        if( diff < resultDiff )
        {
          resultHeight = m_HitHeightTable[i];
          resultDiff = diff;
        }
      }

      return resultHeight;
    }

    // 再取得可能な状態にする
    void Clear( void )
    {
      m_HitCount = 0;
    }
  };

  gfl2::math::Vector4 newPos( pos);
  MapCallbackable mapCallbackabl;
  m_setting.pCollisionModel->GetHeight( newPos, NULL, &mapCallbackabl );
  f32 newHight = mapCallbackabl.GetNearHight( newPos.y );
  return newHight;
}

GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
