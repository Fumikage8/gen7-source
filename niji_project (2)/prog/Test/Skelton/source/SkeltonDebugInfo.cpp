// デバッグ用情報
#if defined(GF_PLATFORM_WIN32)
#include "Test/Skelton/include/SkeltonDebugInfo.h"
#include <FontRenderer/include/gfl2_FontRenderer.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>

#include "Test/Skelton/include/Field/MapJump/SkeltonJumpCollisionManager.h"
//#include "field/camera/SkeltonCameraControlCollision.h"
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)
  
SkeltonDebugInfo::DebugMode SkeltonDebugInfo::m_DebugInfoDebugMode = SkeltonDebugInfo::DEBUG_MODE_PLAYER;    ///< デバッグモード
u32 SkeltonDebugInfo::m_DebugInfoFPS = 0;                   ///< FPS
b32 SkeltonDebugInfo::m_DebugInfoIsFPS15( false );
f32 SkeltonDebugInfo::m_DebugInfoPlayerDefaultWalkSpeed = 2.7f;    ///< 自機歩き速度(秒速) 
f32 SkeltonDebugInfo::m_DebugInfoPlayerDefaultRunSpeed = 5.8f;     ///< 時機走り速度(秒速)
f32 SkeltonDebugInfo::m_DebugInfoPlayerWalkSpeed = SkeltonDebugInfo::m_DebugInfoPlayerDefaultWalkSpeed;    ///< 自機歩き速度(秒速) 
f32 SkeltonDebugInfo::m_DebugInfoPlayerRunSpeed = SkeltonDebugInfo::m_DebugInfoPlayerDefaultRunSpeed;     ///< 時機走り速度(秒速)
f32 SkeltonDebugInfo::m_DebugInfoPlayerMotionStepFrame = 1.0f;
gfl2::math::Vector4 SkeltonDebugInfo::m_DebugInfoPlayerPos( 0.0f, 0.0f, 0.0f );
f32 SkeltonDebugInfo::m_DebugInfoPlayerDir( 0.0f );
b32 SkeltonDebugInfo::m_DebugInfoCameraStopFlag = false;    ///< trueのとき、カメラ位置
f32 SkeltonDebugInfo::m_AngleOfView = 46.25f;               ///< 画角
f32 SkeltonDebugInfo::m_CameraSpeed( 1.0f );                ///< カメラ速度
f32 SkeltonDebugInfo::m_MapScale( 1.0f );
gfl2::math::Vector4 SkeltonDebugInfo::m_CameraTargetDefaultOffset( 0.0f, 0.0f, 0.0f );
gfl2::math::Vector4 SkeltonDebugInfo::m_CameraTargetOffset( 0.0f, 0.0f, 0.0f );
gfl2::math::Vector4 SkeltonDebugInfo::m_CameraTargetDrawPos( 0.0f, 0.0f, 0.0f );
gfl2::math::Vector4 SkeltonDebugInfo::m_CameraTargetDrawColor( 0.0f, 0.0f, 0.0f, 1.0f );
b32 SkeltonDebugInfo::m_CameraTargetDraw( false );
gfl2::gfx::ImageData* SkeltonDebugInfo::m_pShadowImageData( NULL );
b32 SkeltonDebugInfo::m_NaviMapDrawFlag( false );
b32 SkeltonDebugInfo::m_ShadowDrawFlag( true );
f32 SkeltonDebugInfo::m_DebugInfoStickLevel( 0.0f );
u32 SkeltonDebugInfo::m_DebugInfoPlayerMove = PlayerControl::PLAYER_MOVE_MAX;
f32 SkeltonDebugInfo::m_EdgeScale( 1.0f );
u32 SkeltonDebugInfo::m_DeleteCount( 0 );

// 自機関連
gfl2::math::Vector4 SkeltonDebugInfo::m_PlayerStartPosition( 460.0f, 0.0f, 3800.0f );
f32 SkeltonDebugInfo::m_PlayerStartAngle( 0.0f );

// 地形関連
u32 SkeltonDebugInfo::m_InfoMapVisibleMode( 0 );
c16 SkeltonDebugInfo::m_MapRootPath[1024] = L"R:\\home\\momiji\\exec\\data_win32\\skelton\\map\\island_01";

// カメラ関連
u32 SkeltonDebugInfo::m_InfoCameraManualMode( 0 );
c16 SkeltonDebugInfo::m_InfoCameraCollisionMeshName[ 512 ] = L"--";

c16 SkeltonDebugInfo::m_LogBuffer[ 1024 ] = L"LOG";
u32 SkeltonDebugInfo::m_LogDrawCounter = 0;
u32 SkeltonDebugInfo::m_LogLine = 1;

b32 SkeltonDebugInfo::m_MapjumpFadeFlag = false;
f32 SkeltonDebugInfo::m_FadeAlpha = 0.0f;
b32 SkeltonDebugInfo::m_MapjumpCollisoinDrawFlag = false;
CollisionDataManager* SkeltonDebugInfo::m_pMapJumpCollisionDataManager;

gfl2::math::Vector4	SkeltonDebugInfo::m_luminanceWeight;
gfl2::math::Vector4	SkeltonDebugInfo::m_luminanceMulColor;
f32					SkeltonDebugInfo::m_luminanceThreshold;
f32					SkeltonDebugInfo::m_luminanceScale;

// カメラ操作用コリジョン関連
// SkeltonCameraControlCollision* SkeltonDebugInfo::m_pCameraControlCollision;

#if 0
static c16* CAMERA_CONTROL_COLLISON_TYPE_NAME[SkeltonCameraControlCollision::CollisionType::MAX] =
{
  L"交点で止める",
  L"高さを合わせる",
};

#endif
static c16* PLAYER_SETTIG_LABEL_NAME_TABLE[ PlayerControl::PLAYER_MOVE_MAX + 1 ] = {
  L"低速歩き",
  L"歩き", 
  L"小走り", 
  L"走り", 
  L"デバッグ走り",
  L"その他"
};
static c16* MAP_VISIBLE_MODE_NAME_TABLE[] = {
  L"地形",
  L"高さコリジョン", 
  L"カメラコリジョン",
  L"マップジャンプコリジョンモデル",
  L"地形＋マップジャンプコリジョンAABB (XML)",
  L"地形＋カメラ制御コリジョン 交点判定用",
  L"地形＋カメラ制御コリジョン 高さ判定用",
};

SkeltonDebugInfo::PlayerMoveSetting SkeltonDebugInfo::m_PlayerMoveSettingTable[ PlayerControl::PLAYER_MOVE_MAX ] =
{
  // PLAYER_MOVE_WALK_SLOW
  {
    L"低速歩き",
    0.9f,
    1.0f,
    1.8f,
    true,
    chara::MOTION_ID_WALK,
    0.4f
  },

  // PLAYER_MOVE_WALK
  {
    L"歩き", 
    1.6f,
    1.0f,
    1.8f,
    true,
    chara::MOTION_ID_WALK,
    0.8f
  },

  // PLAYER_MOVE_WALK_FAST
  {
    L"早歩き", 
    3.5f,
    1.0f,
    2.7f,
    true,
    chara::MOTION_ID_WALK_FAST,
    1.0f
  },

  // PLAYER_MOVE_RUN
  {
    L"走り", 
    5.8f,
    1.0f,
    5.8f,
    true,
    chara::MOTION_ID_RUN,
    1.0f
  },

  // PLAYER_MOVE_DEBUG_RUN
  {
    L"デバッグ走り", 
    30.0f,
    1.0f,
    5.8f,
    true,
    chara::MOTION_ID_RUN,
    1.0f
  },

};

s32 SkeltonDebugInfo::m_PlayerMoveByDush = PlayerControl::PLAYER_MOVE_WALK_SLOW;

///< 更新実行
void SkeltonDebugInfo::OnUpdate(void)
{
}

// デバッグ3D表示
void SkeltonDebugInfo::OnDrawUtil(void)
{
  // カメラ注視点を表示
  if( m_CameraTargetDraw ) 
  {
    f32 cameraObjectSize = 5.0f;
    gfl2::math::Vector4 cameraObjectWork( cameraObjectSize * 0.5f, cameraObjectSize * 0.5f, cameraObjectSize * 0.5f );
    gfl2::gfx::DrawUtil::SetModelMatrix( gfl2::math::Matrix34::GetIdentity() );
    DrawUtil::DrawAABB(
      SkeltonDebugInfo::m_CameraTargetDrawPos - cameraObjectWork,
      SkeltonDebugInfo::m_CameraTargetDrawPos + cameraObjectWork,
      m_CameraTargetDrawColor
      );
  }

#if 0
  // マップジャンプコリジョンデータを表示
  if( m_MapjumpCollisoinDrawFlag)
  {
    const COLLISION_DATA* pData = m_pMapJumpCollisionDataManager->GetData();
    u32 dataNum = m_pMapJumpCollisionDataManager->GetDataNum();
    for( int i = 0; i < dataNum; ++i)
    {
      const f32 colorValue = 1.0f-(1.0f/dataNum*i);
      const gfl2::math::Vector4 color( colorValue, 0.0f, 0.0f);
      gfl2::gfx::DrawUtil::SetMaterialAlpha( 0.3f);
      gfl2::gfx::DrawUtil::SetModelMatrix( gfl2::math::Matrix34::GetIdentity() );
      DrawUtil::DrawAABB(
        pData[i].aabb.GetMin(),
        pData[i].aabb.GetMax(),
        color
        );
    }
  }
#endif

#if 1
  // 自機の足元に影を表示
  if( m_ShadowDrawFlag && m_pShadowImageData )
  {
    f32 shadowSize = 40.0f;

    //gfl2::gfx::DrawUtil::SetTexture( *m_pShadowImageData );

    // GFGL::BeginScene()〜EndScene()の間に呼ぶとアサートになる
#ifndef _DEBUG
    //gfl2::gfx::DrawUtil::SetDepthTestEnable( true );
#endif

    gfl2::math::Vector3 debugInfoPlayerPos( SkeltonDebugInfo::m_DebugInfoPlayerPos.GetX(), SkeltonDebugInfo::m_DebugInfoPlayerPos.GetY(), SkeltonDebugInfo::m_DebugInfoPlayerPos.GetZ());
    gfl2::gfx::DrawUtil::SetModelMatrix( gfl2::math::Matrix34::GetTranslation( debugInfoPlayerPos ) );

    gfl2::math::Vector4 color(1.0f, 1.0f, 1.0f);
    gfl2::gfx::DrawUtil::VertexFormat vertexFormat[] =
    {
      // 0
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( - shadowSize , 0.0f, - shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 0.0f, 0.0f, 0.0f)),
      // 1
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( - shadowSize, 0.0f, shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 0.0f, 1.0f, 0.0f)),
      // 2
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( shadowSize, 0.0f, - shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 1.0f, 0.0f, 0.0f)),

      // 2
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( shadowSize, 0.0f, - shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 1.0f, 0.0f, 0.0f)),
      // 1
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( - shadowSize, 0.0f, shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 0.0f, 1.0f, 0.0f)),
      //3
      gfl2::gfx::DrawUtil::VertexFormat(	gfl2::math::Vector4( shadowSize, 0.0f, shadowSize ),
      gfl2::math::Vector4(color.x, color.y, color.z),
      gfl2::math::Vector4( 1.0f, 1.0f, 0.0f)),
    };

    gfl2::gfx::DrawUtil::DrawTriangles(vertexFormat, 6, true);

#ifndef _DEBUG
    // gfl2::gfx::DrawUtil::SetDepthTestEnable( false );
#endif
  }
#endif


  // 擬似マップジャンプフェード表示
  if( m_MapjumpFadeFlag)
  {
    gfl2::gfx::DrawUtil::SetMaterialAlpha( m_FadeAlpha);
    gfl2::gfx::DrawUtil::FillScreen( gfl2::math::Vector4( 0.0f, 0.0f, 0.0f));
  }
}

// デバッグ3D表示(デプステストなし)
void SkeltonDebugInfo::OnDrawUtil_NotDepthTest(void)
{
}

// デバッグ文字表示
void SkeltonDebugInfo::OnDrawFont( gfl2::util::DrawUtilText* pDebugDrawUtilText )
{
  pDebugDrawUtilText->SetTextScale( 0.75f, 0.75f );

  gfl2::math::Vector2 pos( 0.0f, 0.0f );
  f32 lineDistance = 18.0f;
#define DRAW_TEXT( ... ) pDebugDrawUtilText->DrawText( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;

  switch( m_DebugInfoDebugMode )
  {
  case DEBUG_MODE_PLAYER:
    {
      DRAW_TEXT( L"DebugMode : INFO" );
      DRAW_TEXT( L"自機 座標:%f,%f,%f 向き %f", m_DebugInfoPlayerPos.x, m_DebugInfoPlayerPos.y, m_DebugInfoPlayerPos.z, m_DebugInfoPlayerDir );
      DRAW_TEXT( L"表示モデル:%s", MAP_VISIBLE_MODE_NAME_TABLE[ m_InfoMapVisibleMode ] );
      DRAW_TEXT( L"カメラデータ名:%s", SkeltonDebugInfo::m_InfoCameraCollisionMeshName );
      DRAW_TEXT( L"デバッグ操作" );
      DRAW_TEXT( L"← → (パッド) : DebugMode変更" );
      DRAW_TEXT( L"U  : 設定ファイル再読み込み" );
      DRAW_TEXT( L"9  : 地形リソース再読み込み" );
      DRAW_TEXT( L"B  : ガッツポーズ再生" );
      DRAW_TEXT( L"L1 : デバッグ走り" );
      DRAW_TEXT( L"X  : 自機・カメラ情報ファイル出力" );
      DRAW_TEXT( L"Z  : 表示モデル切り替え" );
      break;
    }
  }

  if( m_LogDrawCounter > 0 )
  {
    m_LogDrawCounter -= 1;
    pDebugDrawUtilText->DrawText( 0.0f, 240.0f - 20.0f * m_LogLine, m_LogBuffer );
  }


#if 0
  gfl2::fontrenderer::FontRenderer::SetDefaultFontColor( gfl2::math::Vector4( 1.0f, 0.0f, 0.0f ) );

  u32 line = 0;
  f32 fontHeight = 20.0f;
  f32 fontWidth = 20.0f;
  
  // @note インデントが思った通りにならないので、表示を分けた
  f32 infoStart = 160.0f;
  
  switch( m_DebugInfoDebugMode )
  {
  case DEBUG_MODE_PLAYER:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::PLAYER" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line++;
      if( m_DebugInfoPlayerMove >= PlayerControl::PLAYER_MOVE_MAX )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"移動タイプ:%s", L"その他" );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"移動タイプ:%s", m_PlayerMoveSettingTable[ m_DebugInfoPlayerMove ].name );
      }
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"スティック入力量:%f", m_DebugInfoStickLevel );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"速度:%fms モーション再生速度:%fms", m_DebugInfoPlayerWalkSpeed, m_DebugInfoPlayerMotionStepFrame );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"自機 座標:%f,%f,%f 向き %f", m_DebugInfoPlayerPos.x, m_DebugInfoPlayerPos.y, m_DebugInfoPlayerPos.z, m_DebugInfoPlayerDir );
      line++;

      line = 7;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Bボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": ガッツポーズ再生" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"R1" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": デバッグ走り" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Z" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 影のON・OFF" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"X" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 自機情報をファイルに出力" );
      line++;
      
      break;
    }

  case DEBUG_MODE_CAMERA:
    {
      line = 0;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, 40.0f, 40.0f, L"DebugMode::CAMERA" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"画角:%f", m_AngleOfView );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"MeshName:%s", &m_InfoCameraCollisionMeshName[0] );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"カメラコリジョン判定タイプ:%s", CAMERA_CONTROL_COLLISON_TYPE_NAME[m_pCameraControlCollision->GetCollisionType()] );
      line++;
      
      line = 8;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"0" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ状態リセット" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"R2ボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 注視点変更モード" );
      line++;

      switch( m_InfoCameraManualMode )
      {
      case 0:
        {
          GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"右スティック" );
          GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ注視点移動(平面)" );
          line++;
          GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"↑ ↓(パッド)" );
          GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ注視点移動(高さ)" );
          line++;
          break;
        }
      case 1:
        {
          GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"右スティック" );
          GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ角度変更" );
          line++;
          GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"↑ ↓(パッド)" );
          GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ距離変更" );
          line++;
          break;
        }
      }

      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"X Yボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ画角変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"L1ボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 回転リセット" );
      line++;
#if 0
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Xボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ追従ON・OFF" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"0" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ設定を初期状態に戻す" );
      line++;
#endif
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Z" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ設定をログに出力" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"X" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラ情報をファイルに出力" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"1" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": カメラコリジョン判定タイプを変更" );
      line++;
      break;
    }

  case DEBUG_MODE_POKEMON:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::POKEMON" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      
      line = 7;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Xボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":ポケモン配置・種族変更(パターンA)" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"L1" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":ポケモン配置・種族変更(パターンB)" );
      line++;
      break;
    }

  case DEBUG_MODE_BLOOM:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::BLOOM" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line = 3;
	   GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"輝度 = r*weightR + g*weightG + b*weightB" );

      line = 5;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"B + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: weightRの増減 %f",m_luminanceWeight.x );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"A + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: weightGの増減 %f",m_luminanceWeight.y );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"G + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: weightBの増減 %f",m_luminanceWeight.z );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"RB + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: ブルーム対象とする輝度 %f",m_luminanceThreshold );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"RT + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: 抽出した輝度にたいする係数 %f",m_luminanceMulColor.x );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"LT + ↑or↓" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: 光の広がり %f",m_luminanceScale );
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"START" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: 設定の保存");
	  line++;
	  GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U(キーボード)" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L"　　　　　　　: 設定ファイル再読み込み" );

	  line++;

	  break;
    }

  case DEBUG_MODE_MAP:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::MAP" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"地形モデル拡大率:%f", m_MapScale );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"表示モデル:%s", MAP_VISIBLE_MODE_NAME_TABLE[ m_InfoMapVisibleMode ] );

      line = 7;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"L2 R2" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":地形モデル拡大率を変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"0" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":地形モデル拡大率リセット" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"9" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":地形モデル再読込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Z" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L":地形コリジョン表示ON・OFF" );
      line++;
      line++;
#if 0
      // 1行表示
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"ジャンプ" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"0:初期位置(中央エリア) 1:看板(中央エリア) 2:空港(南端) 3:東端 4:看板(北東エリア) 5:北端 6:看板(北西エリア) 7:巨大ビル前(南西エリア)" );
      line++;
#endif
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"ジャンプ" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"1:初期位置(看板)" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"2:NPC前(中央エリア)" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"3:港(南エリア)" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"4:砂浜(東南エリア)" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"5:北端" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"6:西端" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"7:ビル前(南西エリア)" );
      line++;
      break;
    }

  case DEBUG_MODE_NAVI_MAP:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::NAVI_MAP" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line++;
      line = 7;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"Bボタン" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 地図表示ON・OFF" );
      line++;

      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"L2 R2" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 地図の表示範囲を変更" );
      line++;

      break;
    }

  case DEBUG_MODE_ONLY_FPS:
    {
      GFL_FONT_PRINT( 0.0f, 0.0f, 40.0f, 40.0f, L"DebugMode::ONLY_FPS" );
      line = 2;
      if( m_DebugInfoIsFPS15 )
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d(15FPSモード)", m_DebugInfoFPS );
      }
      else
      {
        GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"FPS:%d", m_DebugInfoFPS );
      }
      line = 7;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"デバッグ操作" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"← → (キーボード/パッド): DebugMode変更" );
      line++;
      GFL_FONT_PRINT( 0.0f, line * fontHeight, fontWidth, fontHeight, L"U" );
      GFL_FONT_PRINT( infoStart, line * fontHeight, fontWidth, fontHeight, L": 設定ファイル再読み込み" );
      line++;
      break;
    }

  case DEBUG_MODE_NONE:
    {
      // 何も描画しない
      break;
    }

  }


  if( m_LogDrawCounter > 0 )
  {
    m_LogDrawCounter -= 1;
    GFL_FONT_PRINT( 0.0f, 720.0f - 25.0f * m_LogLine, 25.0f, 25.0f, m_LogBuffer );
  }
#endif
}



void SkeltonDebugInfo::SetLog( u32 line, const c8* str, ... )
{
#if 1
  c8 buffer[1024];

  va_list	argp;
  va_start(argp, str);
  s32 size = vsprintf_s( buffer, 1024, str, argp );
  va_end(argp);

  if( size < 0 )
  {
    GFL_ASSERT( 0 );
    m_LogDrawCounter = 0;
    return;
  }

  m_LogLine = line;
  m_LogDrawCounter = 90;

  // @todo
  for( u32 i = 0; i < 1024; i++ )
  {
    m_LogBuffer[i] = '\0';
  }

  wcscpy_s( m_LogBuffer, 1024, gfl2::renderingengine::scenegraph::resource::ParserUtil::ShitJIStoUTF16( buffer ).c_str() );

#endif
}

void SkeltonDebugInfo::SetCameraCollisionMeshName( const c8* pName )
{
  // gfl2::renderingengine::scenegraph::resource::ParserUtil::ShitJIStoUTF16( pName ).copy( m_InfoCameraCollisionMeshName, 512 );
}


GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)
#endif // GF_PLATFORM_WIN32

