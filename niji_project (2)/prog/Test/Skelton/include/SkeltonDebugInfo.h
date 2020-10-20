#if defined(GF_PLATFORM_WIN32)

#ifndef FIELD_DEBUG_H_INCLUDED
#define FIELD_DEBUG_H_INCLUDED

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <gfx/include/gfl2_ImageData.h>
#include "Test/Skelton/include/Field/SkeltonPlayerControl.h"
#include <util/include/gfl2_UtilTextRenderPath.h>

// デバッグ用情報
GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)


class CollisionDataManager;
namespace camera {
class SkeltonCameraControlCollision;
}

class SkeltonDebugInfo
{
public:

  enum DebugMode
  {
    DEBUG_MODE_PLAYER = 0,
    DEBUG_MODE_NONE,

    DEBUG_MODE_START = DEBUG_MODE_PLAYER,
    DEBUG_MODE_END = DEBUG_MODE_NONE,
  };

  struct PlayerMoveSetting
  {
    c16  name[256];
    f32  moveSpeed;                   // 移動速度
    f32  motionSpeed;                 // モーション再生速度
    f32  motionBaseMoveSpeed;         // モーションが想定している移動速度
    s32  motionSpeedAutomaticFlag;    // trueなら移動速度に合わせてモーション速度を自動変更させる

    s32  motionId;                    // 再生モーション
    f32  stickLevel;                  // 入力値がこの値以下なら発動対象
  };

  ///< 更新実行
  static void OnUpdate(void);

  ///< 描画実行
  static void OnDrawUtil(void);
  static void OnDrawUtil_NotDepthTest(void);
  static void OnDrawFont( gfl2::util::DrawUtilText* pDebugDrawUtilText );

  static DebugMode m_DebugInfoDebugMode;    ///< デバッグモード
  static u32 m_DebugInfoFPS;                ///< FPS
  static b32 m_DebugInfoIsFPS15;            ///< 15FPSならtrue(デバッグ表示制御)
  static f32 m_DebugInfoPlayerWalkSpeed;    ///< 自機歩き速度(秒速) 
  static f32 m_DebugInfoPlayerRunSpeed;     ///< 時機走り速度(秒速)
  static f32 m_DebugInfoPlayerDefaultWalkSpeed;    ///< 自機歩き速度(秒速) 
  static f32 m_DebugInfoPlayerDefaultRunSpeed;     ///< 時機走り速度(秒速)
  static f32 m_DebugInfoPlayerMotionStepFrame;  ///< 自機モーション再生速度(フレーム)
  static gfl2::math::Vector4 m_DebugInfoPlayerPos;
  static f32 m_DebugInfoPlayerDir;
  static b32 m_DebugInfoCameraStopFlag;         ///< trueのとき、カメラ位置
  static f32 m_AngleOfView;                     ///< 画角
  static f32 m_CameraSpeed;                     ///< カメラ速度
  static f32 m_MapScale;
  static gfl2::math::Vector4 m_CameraTargetDefaultOffset;
  static gfl2::math::Vector4 m_CameraTargetOffset;
  static gfl2::math::Vector4 m_CameraTargetDrawPos;
  static gfl2::math::Vector4 m_CameraTargetDrawColor;
  static b32 m_CameraTargetDraw;

  static gfl2::gfx::ImageData*	m_pShadowImageData;    
  static b32 m_NaviMapDrawFlag;                     ///< trueなら簡易マップを表示する
  static b32 m_ShadowDrawFlag;                      ///< trueなら影を表示する

  static PlayerMoveSetting m_PlayerMoveSettingTable[ PlayerControl::PLAYER_MOVE_MAX ];
  static s32 m_PlayerMoveByDush;  ///< ダッシュ時の移動タイプ
  static f32 m_EdgeScale;
  static u32 m_DeleteCount;

  // 自機関連
  static gfl2::math::Vector4 m_PlayerStartPosition;
  static f32 m_PlayerStartAngle;

  // 地形関連
  static u32 m_InfoMapVisibleMode;
  static c16 m_MapRootPath[1024];

  // カメラ関連
  static u32 m_InfoCameraManualMode;
  static c16 m_InfoCameraCollisionMeshName[512];

  static u32 m_DebugInfoPlayerMove;
  static f32 m_DebugInfoStickLevel;
  static c16 m_LogBuffer[ 1024 ];
  static u32 m_LogDrawCounter;
  static u32 m_LogLine;

  // マップジャンプ関連
  static b32 m_MapjumpFadeFlag;                     ///< trueならフェード表示
  static f32 m_FadeAlpha;                           ///< アルファ(外部操作)
  static b32 m_MapjumpCollisoinDrawFlag;            ///< xmlデータのマップジャンプコリジョン表示
  static CollisionDataManager* m_pMapJumpCollisionDataManager;

  // カメラ操作用コリジョン関連
  // static SkeltonCameraControlCollision* m_pCameraControlCollision;

  static void SetLog( u32 line, const c8* str, ... );
  static void SetCameraCollisionMeshName( const c8* pName );

	static gfl2::math::Vector4	m_luminanceWeight;
	static gfl2::math::Vector4	m_luminanceMulColor;
	static f32					m_luminanceThreshold;
	static f32					m_luminanceScale;
};


GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#define FIELD_LOG_PRINT( line, ... ) Test::Skelton::SkeltonDebugInfo::SetLog( line, __VA_ARGS__ )

#endif

#endif // GF_PLATFORM_WIN32

