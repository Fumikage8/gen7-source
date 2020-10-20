/**
* @brief  フィールドカメラ、フォーカス機能
* @file   FieldCameraAreaFocus.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"




// field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldRo/include/Camera/FieldCameraTypes.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h"

#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectTrainerFocus.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"

// debug
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#if PM_DEBUG
#include <gfx/include/gfl2_DrawUtil.h>
#include "Field/FieldRo/include/Debug/FieldDebugCollisionRenderManager.h"
#include <niji_conv_header/message/debug/msg_debug_menu.h>
#if defined(GF_PLATFORM_WIN32)
#include <system/include/InputDevice/gfl2_Pad.h>
#endif // #if defined(GF_PLATFORM_WIN32)
#endif // #if PM_DEBUG

#include <Field/FieldRo/include/Collision/BaseShape.h>
#include <Field/FieldRo/include/Collision/DynamicActor.h>

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

static const u32 SEARCH_RESOULT_ID_ERROR = 0xFFFF;


u32 FOCUS_STOP_BIT_EVENT = 0x01;

// 調整後、const化
s32 FOCUS_TRANS_IN_FRAME( 18.0f );
s32 FOCUS_TRANS_OUT_FRAME( 30.0f );

gfl2::math::Easing::EaseFunc FOCUS_TRANS_IN_EASING( gfl2::math::Easing::OUT_CUBIC );
gfl2::math::Easing::EaseFunc FOCUS_TRANS_OUT_EASING( gfl2::math::Easing::OUT_CUBIC );

f32 FOCUS_POINT_WEIGHT[4] =
{
  1.0f,
  1.0f,
  0.1f,
  0.1f
};
f32 FOCUS_POINT_PLAYER_TOP_OFFSET( 200.0f );
f32 FOCUS_POINT_TARGET_TOP_OFFSET( 200.0f );
s32 FOCUS_USE_EFFECT( 1 );

f32 FOCUS_HIT_RANGE( 600.0f );
f32 FOCUS_HIT_OFFSET( 100.0f );

struct FocusMode
{
  enum Value
  {
    Point = 0,      // 点を画面に納める
    Speare,         // 球を画面に納める　(奥行きで半径が伸びた際に無駄に引いてしまう)
  };
};

// 自機状態別設定
struct PlayerFocusSetting
{
  f32 focusSpeed;
  f32 focusLength;

  f32 focusSpeedWalk;
  f32 focusLengthWalk;

  f32 focusSpeedRun;
  f32 focusLengthRun;
};


struct FOCUS_SETTING
{
  s32 enable;
  s32 fovUse;
  f32 fovValue;
  f32 bgmVolume;
  s32 bgmInFrame;
  s32 bgmOutFrame;
  s32 inSEPlayFrame;
  s32 outSEPlayFrame;
  f32 frameInWidth;
  f32 frameInHeight;
  s32 keepFrame;
  f32 firstKeepValue;

  PlayerFocusSetting playerFocusSetting;
  PlayerFocusSetting rideFocusSetting[Field::RIDE_POKEMON_NUM];

  // 下記の設定は、自機のアクターから情報を取得できなかった際に使用される、製品では使用されないはず
  f32 playerEyeHeight;      // 高さボリューム、
  f32 playerEyeWidth;       // 横ボリュームの半分
  f32 playerEyeOffsetY;     // Y軸オフセット 

  f32 playerEyeWidthScale;  // 横ボリューム = 自機HITサイズ * playerEyeWidthScale
  f32 playerEyeKeepScale;
  f32 effectBaseSpeed;

  f32 maxDistaceScale;

#if PM_DEBUG
  s32 focusDummyTest;
  gfl2::math::Vector3 focusDummyOffset;
#endif
};

FOCUS_SETTING focusSetting =
{
  1,                // enable
  1,                // fovUse;
  0.95f,             // fovValue;
  0.35f,             // bgmVolume
  30,     // bgmInFrame
  25,     // bgmOutFrame
  3,      // inSEPlayFrame;
  3,      // outSEPlayFrame;
  360.0f,
  200.0f,
  15,     // keepFrame
  0.9f,     // firstKeepValue

  { 1.0f, 350.0f, 1.0f, 350.0f, 1.5f, 550.0f }, // playerFocusSetting
  { 
    { 1.0f, 350.0f, 1.5f, 670.0f, 1.5f, 750.0f }, // rideFocusSetting[ RIDE_KENTAROSU ] 
    { 1.0f, 350.0f, 1.5f, 690.0f, 1.5f, 850.0f }, // rideFocusSetting[ RIDE_SAMEHADA ] 
    { 1.0f, 350.0f, 1.5f, 670.0f, 1.0f, 290.0f }, // rideFocusSetting[ RIDE_MUURANDO ] 
    { 1.0f, 350.0f, 1.0f, 390.0f, 1.5f, 590.0f }, // rideFocusSetting[ RIDE_LAPLACE ] 
    { 1.0f, 350.0f, 1.3f, 470.0f, 1.3f, 470.0f }, // rideFocusSetting[ RIDE_KAIRIKY ] 
    { 1.0f, 350.0f, 1.0f, 350.0f, 1.5f, 550.0f }, // rideFocusSetting[ RIDE_RIZAADON ]  ※dummy
    { 1.0f, 350.0f, 1.3f, 470.0f, 1.5f, 690.0f }, // rideFocusSetting[ RIDE_ROBA2 ] 
  },

  1000.0f,  // playerEyeHeight
  41.0f,    // playerEyeWidth
  -400.0f,  // playerEyeOffsetY
  1.1f,     // playerEyeWidthScale
  1.2f,     // playerEyeKeepScale
  1.5f,     // effectBaseSpeed
  1.8f,     // maxDistaceScale;
#if PM_DEBUG
  0,
  gfl2::math::Vector3( 300.0f, 0.0f, 50.0f ),
#endif
};

inline f32 ValueControl(f32 src, f32 target, f32 speed )
{
  if( target > src )
  {
    src += speed;
    return ( src > target) ? target : src;
  }
  else
  {
    src -= speed;
    return ( src < target) ? target : src;
  }
}

// 分離軸に投影された軸成分から投影線分長を算出
f32 LenSegOnSeparateAxis( gfl2::math::Vector3 *Sep, gfl2::math::Vector3 *e1, gfl2::math::Vector3 *e2, gfl2::math::Vector3 *e3 = 0 )
{
  // 3つの内積の絶対値の和で投影線分長を計算
  // 分離軸Sepは標準化されていること
  f32 r1 = gfl2::math::FAbs( Sep->Dot( *e1 ) );
  f32 r2 = gfl2::math::FAbs( Sep->Dot( *e2 ) );
  f32 r3 = e3 ? gfl2::math::FAbs( Sep->Dot( *e3 ) ) : 0;
  return r1 + r2 + r3;
}

// OBB vs OBB
b32 TestOBBvsOBB( BaseActor* pActor1, BaseActor* pActor2 )
{
  const BoxShape* pBoxShape1 = reinterpret_cast<BoxShape*>( pActor1->GetShape() );
  const BoxShape* pBoxShape2 = reinterpret_cast<BoxShape*>( pActor2->GetShape() );

  // 各方向ベクトルの確保
  // （N***:標準化方向ベクトル）
  gfl2::math::Vector3 NAe1 = pBoxShape1->m_u0, Ae1 = NAe1 * pBoxShape1->m_e0;
  gfl2::math::Vector3 NAe2 = pBoxShape1->m_u1, Ae2 = NAe2 * pBoxShape1->m_e1;
  gfl2::math::Vector3 NAe3 = pBoxShape1->m_u0.Cross( pBoxShape1->m_u1 ), Ae3 = NAe3 * ( pBoxShape1->m_height * 0.5f);
  gfl2::math::Vector3 NBe1 = pBoxShape2->m_u0, Be1 = NBe1 * pBoxShape2->m_e0;
  gfl2::math::Vector3 NBe2 = pBoxShape2->m_u1, Be2 = NBe2 * pBoxShape2->m_e1;
  gfl2::math::Vector3 NBe3 = pBoxShape2->m_u0.Cross( pBoxShape2->m_u1 ), Be3 = NBe3 * ( pBoxShape2->m_height * 0.5f);

  gfl2::math::Vector3 centerPos1 = pBoxShape1->m_pos + pActor1->GetPosition();
  centerPos1.y += pBoxShape1->m_height * 0.5f;
  gfl2::math::Vector3 centerPos2 = pBoxShape2->m_pos + pActor2->GetPosition();
  centerPos2.y += pBoxShape2->m_height * 0.5f;

  gfl2::math::Vector3 Interval = ( centerPos1 ) - ( centerPos2 );

  // 分離軸 : Ae1
  f32 rA = Ae1.Length();
  f32 rB = LenSegOnSeparateAxis( &NAe1, &Be1, &Be2, &Be3 );
  f32 L = gfl2::math::FAbs( Interval.Dot( NAe1 ));
  if( L > rA + rB )
    return false; // 衝突していない

  // 分離軸 : Ae2
  rA = Ae2.Length();
  rB = LenSegOnSeparateAxis( &NAe2, &Be1, &Be2, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( NAe2 ));
  if( L > rA + rB )
    return false;

  // 分離軸 : Ae3
  rA = Ae3.Length();
  rB = LenSegOnSeparateAxis( &NAe3, &Be1, &Be2, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( NAe3 ));
  if( L > rA + rB )
    return false;

  // 分離軸 : Be1
  rA = LenSegOnSeparateAxis( &NBe1, &Ae1, &Ae2, &Ae3 );
  rB = Be1.Length();
  L = gfl2::math::FAbs( Interval.Dot( NBe1 ));
  if( L > rA + rB )
    return false;

  // 分離軸 : Be2
  rA = LenSegOnSeparateAxis( &NBe2, &Ae1, &Ae2, &Ae3 );
  rB = Be2.Length();
  L = gfl2::math::FAbs( Interval.Dot( NBe2 ));
  if( L > rA + rB )
    return false;

  // 分離軸 : Be3
  rA = LenSegOnSeparateAxis( &NBe3, &Ae1, &Ae2, &Ae3 );
  rB = Be3.Length();
  L = gfl2::math::FAbs( Interval.Dot( NBe3 ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C11
  gfl2::math::Vector3 Cross;
  Cross = NAe1.Cross( NBe1 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae2, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be2, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C12
  Cross = NAe1.Cross( NBe2 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae2, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C13
  Cross = NAe1.Cross( NBe3 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae2, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be2 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C21
  Cross = NAe2.Cross( NBe1 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be2, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C22
  Cross = NAe2.Cross( NBe2 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C23
  Cross = NAe2.Cross( NBe3 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae3 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be2 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C31
  Cross = NAe3.Cross( NBe1 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae2 );
  rB = LenSegOnSeparateAxis( &Cross, &Be2, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C32
  Cross = NAe3.Cross( NBe2 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae2 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be3 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離軸 : C33
  Cross = NAe3.Cross( NBe3 );
  rA = LenSegOnSeparateAxis( &Cross, &Ae1, &Ae2 );
  rB = LenSegOnSeparateAxis( &Cross, &Be1, &Be2 );
  L = gfl2::math::FAbs( Interval.Dot( Cross ));
  if( L > rA + rB )
    return false;

  // 分離平面が存在しないので「衝突している」
  return true;
}



/**
* @brief 線分との当たり判定
* @param [out] pOutIntersection 交点の座標(当たっていなければ何も入れない)
* @param [in] st,ed 線分の始点と終点
* @retval true 当たっている
* @retval false 当たっていない
*/
bool IntersectsRay( const gfl2::math::Vector3& st, const gfl2::math::Vector3& ed, const gfl2::math::Vector3* pTri )
{
  // 法線
  gfl2::math::Vector3 normal;
  {
    gfl2::math::Vector3 v0 = pTri[1] - pTri[0];
    gfl2::math::Vector3 v1 = pTri[2] - pTri[0];
    normal = v0.Cross(v1).Normalize();
  }

  gfl2::math::Vector3 v1 = st - pTri[0];
  gfl2::math::Vector3 v2 = ed - pTri[0];

  // 線分と平面が当たっているか
  if( v1.Dot( normal) * v2.Dot( normal) > 0.0f)
  {
    return false;
  }

  // 交点を計算
  f32 d1 = gfl2::math::FAbs( normal.Dot( v1)) / normal.Length();
  f32 d2 = gfl2::math::FAbs( normal.Dot( v2)) / normal.Length();
  if( d1 + d2 == 0.0f )
  {
    return false;
  }
  f32 rate = d1 / (d1 + d2);
  gfl2::math::Vector3 intersection = pTri[0] + (v1 * (1.0f - rate) + v2 * rate);

  // 交点がポリゴン内に含まれているか
  gfl2::math::Vector3 cross1,cross2,cross3;

  v1 = pTri[1] - pTri[0];
  v2 = intersection - pTri[0];
  cross1 = v1.Cross( v2);

  v1 = pTri[2] - pTri[1];
  v2 = intersection - pTri[1];
  cross2 = v1.Cross( v2);

  v1 = pTri[0] - pTri[2];
  v2 = intersection - pTri[2];
  cross3 = v1.Cross( v2);

  if( cross1.Dot( cross2) < 0.0f || cross1.Dot( cross3) < 0.0f)
  {
    return false;
  }

  // 当たってる
  // *pOutIntersection = intersection;
  return true;
}

b32 HitCheckLay( const gfl2::math::Vector3& st, const gfl2::math::Vector3& ed, StaticActor* pStaticActor )
{
  {
    gfl2::math::Vector3 parentPos;
    parentPos = pStaticActor->GetPosition();

    // シェイプを更新
    BoxShape* pBoxShape = reinterpret_cast<BoxShape*>( pStaticActor->GetShape());

    // 
    gfl2::math::Vector3 axisZ( pBoxShape->m_u0.Cross( pBoxShape->m_u1 ) );
    gfl2::math::Vector3 center( parentPos + pBoxShape->m_pos );

    gfl2::math::Vector3 triTable[] =
    {
      center + ( pBoxShape->m_u1 * pBoxShape->m_e1 ) + ( axisZ * ( pBoxShape->m_height * -1) ), // 0
      center + ( pBoxShape->m_u1 * pBoxShape->m_e1 ) + ( axisZ * ( pBoxShape->m_height) ), // 1
      center + ( pBoxShape->m_u1 * ( pBoxShape->m_e1 * -1 ) ) + ( axisZ * ( pBoxShape->m_height * -1) ), // 2
      center + ( pBoxShape->m_u1 * pBoxShape->m_e1 ) + ( axisZ * ( pBoxShape->m_height) ), // 1
      center + ( pBoxShape->m_u1 * ( pBoxShape->m_e1 * -1 ) ) + ( axisZ * ( pBoxShape->m_height ) ), // 3
      center + ( pBoxShape->m_u1 * ( pBoxShape->m_e1 * -1 ) ) + ( axisZ * ( pBoxShape->m_height * -1) ) // 2
    };

    if( IntersectsRay( st, ed, &triTable[ 0 ] ) || IntersectsRay( st, ed, &triTable[ 3 ] )  )
    {
      return true;
    }
  }

  return false;
}

// 同じターゲットの結果か取得
// 両方無効 or 両方有効でidが同じときはtrueを返す
b32 IsSameTarget( const Focus::SearchResult& r1, const Focus::SearchResult& r2 )
{
  if( !r1.isHit && !r2.isHit )
  {
    return true;
  }

  if( r1.isHit && r2.isHit && r1.id == r2.id )
  {
    return true;
  }

  return false;
}

#if PM_DEBUG

void AddFocusSettingItem( gfl2::heap::HeapBase *pHeap, gfl2::debug::DebugWinGroup* pGroup, gfl2::str::MsgData* pMsgData, PlayerFocusSetting& rFocusSetting, u32 titleMsgDatId )
{
  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, titleMsgDatId, NULL, NULL );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_31, &rFocusSetting.focusLength, 0.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_32, &rFocusSetting.focusLengthWalk, 0.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_33, &rFocusSetting.focusLengthRun, 0.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_34, &rFocusSetting.focusSpeed, 0.0f, 10000.0f, 0.01f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_35, &rFocusSetting.focusSpeedWalk, 0.0f, 10000.0f, 0.01f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_debug_area_focus_36, &rFocusSetting.focusSpeedRun, 0.0f, 10000.0f, 0.01f );
}


#endif



/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
Focus::Focus( const InitializeDescription& rInitDesc ) :
  m_EventStopBit( 0 )
  ,m_BgmStopBit( 0 )
  ,m_IsReady( false )
  ,m_KeepFrame( 0 )
  ,m_State( State::STOP )
  ,m_HitCheckLength( 0.0f )
  ,m_InterestPosition( gfl2::math::Vector3::GetZero() )
  ,m_pPlacementDataManager( NULL )
  ,m_pPlayerEyeDynamicActor( NULL )
#if PM_DEBUG
  ,m_pDebugWinGroup( NULL )
#endif
{
  m_pTarget = rInitDesc.m_pTarget;
  m_pPlacementDataManager = rInitDesc.m_pPlacementDataManager;

  m_FocusValueController.Set( 0.0f );

  ClearParam( &m_OffsetParam );
  ClearParam( &m_FixParam );

  // 
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  m_pPlayerEyeDynamicActor = pGameManager->GetFieldResident()->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_TRAINER_EYE )->CreateDynamicActor( 
    pFieldMap->GetDeviceHeapAllocator(),
    pFieldMap->GetHeap()
    );

  m_pPlayerEyeDynamicActor->CreateBox(
    gfl2::math::Vector3::GetZero(),
    gfl2::math::Vector3( 1.0f, 0.0f, 0.0f ),
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ),
    30,
    30,
    30
    );


  m_pKeepPlayerEyeDynamicActor = pGameManager->GetFieldResident()->GetCollisionScene( Field::FieldResident::COLLISION_SCENE_TRAINER_EYE )->CreateDynamicActor( 
    pFieldMap->GetDeviceHeapAllocator(),
    pFieldMap->GetHeap()
    );

  m_pKeepPlayerEyeDynamicActor->CreateBox(
    gfl2::math::Vector3::GetZero(),
    gfl2::math::Vector3( 1.0f, 0.0f, 0.0f ),
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ),
    30,
    30,
    30
    );

  m_NowSearchResult.isHit = false;

  // ゲームデータの設定を復帰
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    Field::Camera::SaveData saveData;
    pGameManager->GetGameData()->GetFieldCameraData( &saveData );
    m_EventStopBit = saveData.focusStopBit;
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
Focus::~Focus( void )
{
  GFL_SAFE_DELETE( m_pKeepPlayerEyeDynamicActor );
  GFL_SAFE_DELETE( m_pPlayerEyeDynamicActor );

#if PM_DEBUG
  UnResistDebugMenu();
#endif // #if PM_DEBUG
}

/**
* @brief 更新処理
*
* @return 無し
*/
void Focus::Update( poke_3d::model::BaseCamera* pCamera, f32 minDistance )
{
  // 初回更新
  if( m_IsReady == false )
  {
    m_IsReady = true;
    SetParam( pCamera, &m_FixParam );
  }

  // 自機の視線を更新
  UpdatePlayerEyeActor();

  //
  // フォーカスするターゲットを更新
  //

  UpdateKeepFrame();    // キープ状態、事前更新

  SearchResult newFocusResult;
  newFocusResult.isHit = false;

  if( focusSetting.enable != 0 && !(m_EventStopBit & EventStopBit::FOCUS ) )
  {
    b32 keepFlag( false );

#if PM_DEBUG
    if( Field::Debug::DebugTypes::s_FocusCameraCheck.camera_check_on )
    {
      SearchFocusTrainerForDebugCameraCheck( &newFocusResult, &keepFlag, m_NowSearchResult, m_KeepFrame > 0 || ( m_State == State::MOVE_IN && m_FocusValueController.GetValue() < focusSetting.firstKeepValue ) );
    }
    else
#endif
    {
      SearchFocusTrainerEx( &newFocusResult, &keepFlag, m_NowSearchResult, m_KeepFrame > 0 || ( m_State == State::MOVE_IN && m_FocusValueController.GetValue() < focusSetting.firstKeepValue ) );
    }

    if( keepFlag )
    {
      m_KeepFrame = ( m_KeepFrame > 0 ) ? m_KeepFrame - 1 : 0; // keepによるHITの場合はKeep時間を減らす
    }
    else
    {
      m_KeepFrame = focusSetting.keepFrame;
    }
  }
  else
  {
    m_KeepFrame = 0;
  }

  //
  // ターゲット座標更新
  //

  switch( m_State )
  {
    // 未フォーカス
  case State::STOP:
    {
      SetParam( pCamera, &m_FixParam );

      // フォーカス開始設定
      if( newFocusResult.isHit )
      {
        f32 changeSpeed = GetFocusSpeed();

        ClearParam( &m_OffsetParam );
        m_State = State::MOVE_IN;

        m_FocusValueController.Move(
          0.0f,
          1.0f,
          FOCUS_TRANS_IN_FRAME / changeSpeed,
          FOCUS_TRANS_IN_EASING
          );
      }

      // 結果を更新
      m_NowSearchResult = newFocusResult;

      break;
    }

    // フォーカス中
  case State::MOVE_IN:
    {
      // フォーカス対象が切り変わった
      if( !IsSameTarget( m_NowSearchResult, newFocusResult ) )
      {
        f32 changeSpeed = GetFocusSpeed();

        // 前のターゲットで一度更新
        GetFixParam( pCamera, m_NowSearchResult, m_OffsetParam, &m_FixParam, minDistance );

        if( newFocusResult.isHit )
        {
          // 違うターゲットに遷移する設定
          m_FocusValueController.Move(
            0.0f,
            1.0f,
            FOCUS_TRANS_IN_FRAME / changeSpeed,
            FOCUS_TRANS_IN_EASING
            );

          // 相対座標で補間するため、offset値を記憶
          m_OffsetParam.m_CameraPos = m_FixParam.m_CameraPos - pCamera->GetPosition();
          m_OffsetParam.m_CameraTargetPos = m_FixParam.m_CameraTargetPos - pCamera->GetTargetPosition();
          m_OffsetParam.m_FovY = m_FixParam.m_FovY - pCamera->GetFovy();
        }
        else
        {
          // 解除設定
          m_FocusValueController.Move(
            0.0f,
            1.0f,
            FOCUS_TRANS_OUT_FRAME / changeSpeed,
            FOCUS_TRANS_OUT_EASING
            );

          // 相対座標で補間するため、offset値を記憶
          m_OffsetParam.m_CameraPos = m_FixParam.m_CameraPos - pCamera->GetPosition();
          m_OffsetParam.m_CameraTargetPos = m_FixParam.m_CameraTargetPos - pCamera->GetTargetPosition();
          m_OffsetParam.m_FovY = m_FixParam.m_FovY - pCamera->GetFovy();
          m_State = State::MOVE_OUT;
        }

        // 結果を更新
        m_NowSearchResult = newFocusResult;
      }
      else
      {
        // 結果を更新
        m_NowSearchResult = newFocusResult;

        // @note オフセットには前回のターゲットをロストした際のズレが入っている
        GetFixParam( pCamera, m_NowSearchResult, m_OffsetParam, &m_FixParam, minDistance );
      }

      break;
    }

    // フォーカス解除中
  case State::MOVE_OUT:
    {
      // まず座標を更新
      FocusCameraParam targetParam;
      FocusCameraParam srcParam;

      // フォーカス状態->最新のカメラへ遷移
      SetParam( pCamera, &targetParam );
      AddParam( targetParam, m_OffsetParam, &srcParam );    // フォーカス状態の座標をオフセットから再現
      LerpParam( srcParam, targetParam, &m_FixParam, m_FocusValueController.GetValue() );

      // フォーカス開始設定
      if( newFocusResult.isHit )
      {
        f32 changeSpeed = GetFocusSpeed();

        m_FocusValueController.Move(
          0.0f,
          1.0f,
          FOCUS_TRANS_IN_FRAME / changeSpeed,
          FOCUS_TRANS_IN_EASING
          );

        // offset値を記憶
        m_OffsetParam.m_CameraPos = m_FixParam.m_CameraPos - pCamera->GetPosition();
        m_OffsetParam.m_CameraTargetPos = m_FixParam.m_CameraTargetPos - pCamera->GetTargetPosition();
        m_OffsetParam.m_FovY = m_FixParam.m_FovY - pCamera->GetFovy();
        m_State = State::MOVE_IN;

      }
      // 終了
      else
      {
        if( m_FocusValueController.IsEnd() )
        {
          m_State = State::STOP;
        }
      }

      // 結果を更新
      m_NowSearchResult = newFocusResult;
      break;
    }
  }

  // 自機の注目座標を更新
  if( m_NowSearchResult.isHit )
  {
    m_InterestPosition = m_NowSearchResult.interestPosition;
  }

  // 各種状態更新
  UpdateEffect();
  UpdateBGM();
  UpdateSE();

  //
  // 補間係数更新
  //
  m_FocusValueController.Update();

  // 更新結果をゲームデータに保存
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    Field::Camera::SaveData saveData;
    pGameManager->GetGameData()->GetFieldCameraData( &saveData );

    saveData.focusStopBit = m_EventStopBit;

    pGameManager->GetGameData()->SetFieldCameraData( saveData );
  }

}

/**
* @brief カメラ反映処理
*
* @return 無し
*/
void Focus::Execute( poke_3d::model::BaseCamera* pCamera ) const
{
  if( m_State == State::STOP )
  {
    return;
  }

  pCamera->SetupCameraLookAt( 
    m_FixParam.m_CameraPos,
    m_FixParam.m_CameraTargetPos,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )
    );

  if( focusSetting.fovUse )
  {
    pCamera->SetFovy( m_FixParam.m_FovY );
  }
}

/**
* @brief 注目すべきか取得する
*
* @return true 有効 false 無効
*/
b32 Focus::IsInterest( void ) const
{
  if( m_EventStopBit & EventStopBit::INTERES )
  {
    return false;
  }

  return ( m_NowSearchResult.isHit );
}

/**
* @brief 注目座標を取得する
*
* @param  pOut 座標格納先
*
* @return true 有効 false 無効( IsInterest関数と連動 )
*/
b32 Focus::GetInterestPosition( gfl2::math::Vector3* pOut ) const
{
  if( !this->IsInterest() )
  {
    return false;
  }

  *pOut = m_InterestPosition;
  return true;
}

/**
* @brief サポート挙動が有効か取得する
*
* @return true 有効 false 無効
*/
b32 Focus::IsValid( void ) const
{
  return ( m_State != State::STOP );
}

/**
* @brief イベントによるフォーカス中止フラグを設定
* @note  1フレームで複数回切り替わる可能性がある
*
* @return true 有効 false 無効
*/
void Focus::ClearEventStopBit( void )
{
  m_EventStopBit = 0;
}

/**
* @brief イベントによるフォーカス中止フラグを設定
* @note  1フレームで複数回切り替わる可能性がある
*
* @return true 有効 false 無効
*/
void Focus::EventStop( u32 bit )
{
  m_EventStopBit = m_EventStopBit | bit;
}

void Focus::Reset( void )
{
  // 今の所とくに呼ぶ事がない
}

void Focus::ResetForRideEvent(void)
{
  m_State = State::STOP;
  m_FocusValueController.Set( 0.0f );
  m_NowSearchResult.isHit = false;
  m_KeepFrame = 0;
}

#if PM_DEBUG
void Focus::ResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ取得」→「FieldDebugMenuから直接グループを取得」に変更。
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::debug::DebugWinGroup* pGroup = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup();
  if( pGroup )
  {
    gfl2::str::MsgData* pMsgData = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetMessageData();

    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::debug::DebugWinGroup* pGroupWork = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_debug_area_focus_00, pGroup, 39 + 48 );

    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_01, &FOCUS_TRANS_IN_FRAME, 0, 30 * 60, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_02, &FOCUS_TRANS_OUT_FRAME, 0, 30 * 60, 1 );

    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"FOCUS_TRANS_IN_EASING", &FOCUS_TRANS_IN_EASING );
    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGroupWork, pHeap, L"FOCUS_TRANS_OUT_EASING", &FOCUS_TRANS_OUT_EASING );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_05, &FOCUS_POINT_PLAYER_TOP_OFFSET, 0.0f, 1000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_06, &FOCUS_POINT_TARGET_TOP_OFFSET, 0.0f, 1000.0f, 1.0f );

    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_07, &focusSetting.enable, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_08, &FOCUS_POINT_WEIGHT[0], 0.0f, 100.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_09, &FOCUS_POINT_WEIGHT[1], 0.0f, 100.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_10, &FOCUS_POINT_WEIGHT[2], 0.0f, 100.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_11, &FOCUS_POINT_WEIGHT[3], 0.0f, 100.0f, 0.01f );


    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_12, &FOCUS_USE_EFFECT, 0, 1, 1 );


    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_14, &focusSetting.frameInWidth, 1.0f, 400.0f * 10.0f, 0.1f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_15, &focusSetting.frameInHeight, 1.0f, 240.0f * 10.0f, 0.1f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_16, &focusSetting.keepFrame, 0, 120 * 30, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_16, &focusSetting.firstKeepValue, 0, 120 * 30, 0.1f );

    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_17, &focusSetting.fovUse, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_18, &focusSetting.fovValue, 0.01f, 1.5f, 0.1f );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_19, &focusSetting.bgmVolume, 0.01f, 2.0f, 0.01f );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_20, &focusSetting.bgmInFrame, 1, 30 * 20, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_21, &focusSetting.bgmOutFrame, 1, 30 * 20, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_22, &focusSetting.inSEPlayFrame, 0, 50, 1 );
    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_23, &focusSetting.outSEPlayFrame, 0, 50, 1 );

    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.playerFocusSetting, msg_dmenu_debug_area_focus_24 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_KENTAROSU ], msg_dmenu_debug_area_focus_25 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_SAMEHADA ], msg_dmenu_debug_area_focus_26 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_MUURANDO ], msg_dmenu_debug_area_focus_27 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_LAPLACE ], msg_dmenu_debug_area_focus_28 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_KAIRIKY ], msg_dmenu_debug_area_focus_29 );
    AddFocusSettingItem( pHeap, pGroupWork, pMsgData, focusSetting.rideFocusSetting[ RIDE_ROBA2 ], msg_dmenu_debug_area_focus_30 );

    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_37, &focusSetting.maxDistaceScale, 0.1f, 10000.0f, 0.01f );

    gfl2::debug::DebugWin_AddItemValueInt( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_38, &focusSetting.focusDummyTest, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_39, &focusSetting.focusDummyOffset.x, -3000.0f, 3000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_40, &focusSetting.focusDummyOffset.y, -3000.0f, 3000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_41, &focusSetting.focusDummyOffset.z, -3000.0f, 3000.0f, 1.0f );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroupWork, pHeap, pMsgData, msg_dmenu_debug_area_focus_41, &focusSetting.effectBaseSpeed, 0.0f, 5.0f, 0.1f );



    // 記憶
    m_pDebugWinGroup = pGroupWork;
  }
}

void Focus::UnResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ削除」→「記憶したグループクラスを指定して削除」に変更。
  if( m_pDebugWinGroup )
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->GetFieldmap()->GetFieldDebugMenu() && pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup() )
    {
      pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup()->RemoveItem( m_pDebugWinGroup );
      m_pDebugWinGroup = NULL;
    }
    else
    {
      // @note 無いなら親ごと削除されている
      m_pDebugWinGroup = NULL;
    }
  }
}

/** 
  *  @brief  環境音コリジョンをデバッグ表示へ登録
  *  @param  color   表示色
  *  @param  onlyPlayFlag  再生中のコリジョンのみ表示
  *  @param  onlyActiveFlag true:アクティブなコリジョンのみ表示する false：全て表示する
  */
void Focus::DebugDrawCameraCheckInfo( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext )
{
  if( !m_NowSearchResult.isHit )
  {
    return;
  }

  static gfl2::gfx::DrawUtil::VertexFormat s_vertexFormat[3]; // ライン表示用のデータ

  for( u32 i=0; i<3; i++ )
  {
    s_vertexFormat[i].color = gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
  }

  // ライン描画のためにカメラ設定
  {
    // プロジェクション行列設定
    gfl2::gfx::DrawUtil::SetProjectionMatrix(rDrawContext.m_Projection);
    // ビュー行列設定
    gfl2::gfx::DrawUtil::SetViewMatrix(rDrawContext.m_View);
    // モデル行列設定
    gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  }

  // TRからプレイヤーへのベクトルを表示
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  gfl2::math::Vector3 offset( 0.0f, 5.0f, 0.0f );
  gfl2::math::Vector3 playerPos = pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate + offset;
  gfl2::math::Vector3 targetPos = m_NowSearchResult.position + offset;
  
  {
    // チェック範囲まで伸ばす
    gfl2::math::Vector3 vec = ( playerPos - targetPos ).Normalize() * ( Field::Debug::DebugTypes::s_FocusCameraCheck.distance + Field::Debug::DebugTypes::s_FocusCameraCheck.geta );
    playerPos = targetPos + vec;

    s_vertexFormat[0].pos = playerPos;  //視点
    s_vertexFormat[1].pos = targetPos;  //終点
    s_vertexFormat[2].pos = (playerPos + ((targetPos - playerPos) * 0.50f));
    s_vertexFormat[2].pos.y += 8.0f;  //若干膨らませる。
    gfl2::gfx::DrawUtil::DrawTriangles( s_vertexFormat, 3, false );
  }
}

#endif

/**
* @brief エフェクト状態を更新
*
* @return 無し
*/
void Focus::UpdateEffect( void )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  Field::Effect::EffectTrainerFocus* pEffect = pFieldMap->GetEffectManager()->GetTrainerFocus();

  if( pEffect == NULL )
  {
    return;
  }


  b32 onFlag( false );

  switch( m_State )
  {
  case State::MOVE_IN:
    {
      if( !(m_EventStopBit & EventStopBit::EFFECT ) )
      {
        onFlag = true;
      }
      break;
    }
  }

  f32 changeSpeed = GetFocusSpeed();
  if( onFlag )
  {
    if( !pEffect->IsIn() )
    {
      pFieldMap->GetEffectManager()->GetTrainerFocus()->StartIn( FOCUS_TRANS_IN_FRAME / changeSpeed / focusSetting.effectBaseSpeed );
    }
  }
  else
  {
    if( pEffect->IsIn() )
    {
      pFieldMap->GetEffectManager()->GetTrainerFocus()->StartOut( FOCUS_TRANS_OUT_FRAME / changeSpeed / focusSetting.effectBaseSpeed );
    }
  }
}

/**
* @brief BGM状態を更新
*
* @return 無し
*/
void Focus::UpdateBGM( void )
{
  b32 onFlag( false );

  switch( m_State )
  {
  case State::MOVE_IN:
    {
      if( !(m_EventStopBit & EventStopBit::BGM ) )
      {
        onFlag = true;
      }
      break;
    }
  }


  if( onFlag )
  {
    // BGM音量を下げる
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( !pGameManager->GetGameData()->GetFieldSound()->IsValidTrainerFocusMode() )
    {
      pGameManager->GetGameData()->GetFieldSound()->SetTrainerFocusMode( focusSetting.bgmVolume, static_cast<Sound::FadeFrame>( focusSetting.bgmInFrame ) );
    }
  }
  else
  {
    // BGM音量を戻す
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->GetGameData()->GetFieldSound()->IsValidTrainerFocusMode() )
    {
      pGameManager->GetGameData()->GetFieldSound()->ResetTrainerFocusMode( static_cast<Sound::FadeFrame>( focusSetting.bgmOutFrame ) );
    }
  }
}

/**
* @brief 効果音を更新
*
* @return 無し
*/
void Focus::UpdateSE( void )
{
  if( (m_EventStopBit & EventStopBit::SE ) )
  {
    return;
  }

  // @note 一瞬反応した際に毎回ならない様に、少し遅延して再生する
  switch( m_State )
  {
  case State::MOVE_IN:
    {
      if( m_FocusValueController.GetFrame() == focusSetting.inSEPlayFrame )
      {
        Sound::PlaySE( SEQ_SE_FLD_SHISEN_CAMERAZOOMIN );
      }
      break;
    }

  case State::MOVE_OUT:
    {
      if( m_FocusValueController.GetFrame() == focusSetting.outSEPlayFrame )
      {
        Sound::PlaySE( SEQ_SE_FLD_SHISEN_CAMERAZOOMOUT );
      }
      break;
    }
  }
}

/**
* @brief キープ状態更新
*
* @return 無し
*/
void Focus::UpdateKeepFrame( void )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // 自機が動いていたらキープ解除
  if( pFieldMap->GetPlayerCharacter()->IsMoveState() )
  {
    m_KeepFrame = 0;
  }
}

/**
* @brief フォーカス速度を取得
*
* @return フォーカス速度
*/
f32 Focus::GetFocusSpeed( void ) const
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // 参照データ取得
  const PlayerFocusSetting* pFocusSetting( NULL );
  if( pFieldMap->GetPlayerCharacter()->IsRideMoveModel() )
  {
    // ライド中
    MoveModel::FieldMoveModel* pModel = pFieldMap->GetPlayerCharacter()->GetOnMoveModel();
    RIDE_POKEMON_ID id = EventPokemonRideTool::GetCharactorId_to_RIDE_POKEMON_ID( pModel->GetCharacterId() );
    pFocusSetting = &focusSetting.rideFocusSetting[ id ];
  }
  else
  {
    // 人
    pFocusSetting = &focusSetting.playerFocusSetting;
  }

  // 
  if( pFieldMap->GetPlayerCharacter()->IsMoveState() )
  {
    // 走っている
    if( pFieldMap->GetPlayerCharacter()->IsRunLoopState() )
    {
      return pFocusSetting->focusSpeedRun;
    }
    else
    {
      return pFocusSetting->focusSpeedWalk;
    }
  }
  else
  {
    return pFocusSetting->focusSpeed;
  }

}

/**
* @brief 自機の視線アクターを更新
*
* @return 無し
*/
void Focus::UpdatePlayerEyeActor( void )
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  //
  // 判定距離算出
  //

  const PlayerFocusSetting* pFocusSetting( NULL );
  f32 playerEyeLengh = 0.0f;

  // 参照データ取得
  if( pFieldMap->GetPlayerCharacter()->IsRideMoveModel() )
  {
    // ライド中
    MoveModel::FieldMoveModel* pModel = pFieldMap->GetPlayerCharacter()->GetOnMoveModel();
    RIDE_POKEMON_ID id = EventPokemonRideTool::GetCharactorId_to_RIDE_POKEMON_ID( pModel->GetCharacterId() );
    pFocusSetting = &focusSetting.rideFocusSetting[ id ];
  }
  else
  {
    // 人
    pFocusSetting = &focusSetting.playerFocusSetting;
  }

  // 距離設定
  if( pFieldMap->GetPlayerCharacter()->IsMoveState() )
  {
    // 走っている
    if( pFieldMap->GetPlayerCharacter()->IsRunLoopState() )
    {
      playerEyeLengh = pFocusSetting->focusLengthRun;
    }
    else
    {
      playerEyeLengh = pFocusSetting->focusLengthWalk;
    }
  }
  else
  {
    // とまっている
    playerEyeLengh = pFocusSetting->focusLength;
  }

  //
  // アクター座標更新
  //
  gfl2::math::Vector3 actorPosition( pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate );
  m_pPlayerEyeDynamicActor->SetPosition( actorPosition );
  m_pKeepPlayerEyeDynamicActor->SetPosition( actorPosition );

  //
  // 形状更新
  //
  f32 heightVolume;
  f32 widthVolume;
  f32 ofssetY;
  DynamicActor* pDynamicActor = pFieldMap->GetPlayerCharacter()->GetDynamicActor( MoveModel::COLLISION_TYPE_TRAINER_EYE );
  if( pDynamicActor == NULL )
  {
    // @note RideOn/Off時にインスタンスが消える、この場合は座標のみ更新し、形状を更新しない
    return;
  }

  // 自機の視線HIT形状のサイズを参照する
  if( pDynamicActor->GetShapeType() == BaseActor::SHAPE_CYLINDER )
  {
    // 自機の視線HIT形状(円柱)を進行方向に伸ばしたBOXを算出する
    CylinderShape* pShape = static_cast<CylinderShape*>(pDynamicActor->GetShape());
    heightVolume = pShape->m_height;
    widthVolume = pShape->m_r * focusSetting.playerEyeWidthScale;
    ofssetY = pShape->m_pos.y;
  }
  else
  {
    // 形状が想定外の場合のケア
    GFL_ASSERT( 0 );   // 開発中に気づくためのアサート、視線の形状が変わった
    heightVolume = focusSetting.playerEyeHeight;
    widthVolume = focusSetting.playerEyeWidth * focusSetting.playerEyeWidthScale;
    ofssetY = focusSetting.playerEyeOffsetY;
  }

  gfl2::math::Vector3 offset( 0.0f, ofssetY, playerEyeLengh * 0.5f );   
  gfl2::math::Vector3 u0( 1.0f, 0.0f, 0.0f );
  gfl2::math::Vector3 u1( 0.0f, 0.0f, 1.0f );

  offset = pFieldMap->GetPlayerQuaternion().Transform( offset );
  u0 = pFieldMap->GetPlayerQuaternion().Transform( u0 );
  u1 = pFieldMap->GetPlayerQuaternion().Transform( u1 );

  // シェイプを更新
  BoxShape* pBoxShape = reinterpret_cast<BoxShape*>( m_pPlayerEyeDynamicActor->GetShape());
  pBoxShape->m_pos    = offset;
  pBoxShape->m_e0     = widthVolume;
  pBoxShape->m_e1     = playerEyeLengh * 0.5f;
  pBoxShape->m_u0     = u0;
  pBoxShape->m_u1     = u1;
  pBoxShape->m_height = heightVolume;

  // シェイプを更新(今ヒットしているトレーナーに当たりやすくするために拡大した物)
  if( m_NowSearchResult.isHit )
  {
    offset = gfl2::math::Vector3( 0.0f, 75.0f - heightVolume * 0.5f, m_NowSearchResult.checkLengh * 0.5f );   
    offset = pFieldMap->GetPlayerQuaternion().Transform( offset );

    BoxShape* pBoxShape = reinterpret_cast<BoxShape*>( m_pKeepPlayerEyeDynamicActor->GetShape());
    pBoxShape->m_pos    = offset;

    // @fix Mcat[1390]:フォーカスが激しく切り替わる、対処。キープ状態の横幅にケア用の定数値が使われており、ライド時に横幅が小さくなっていた。最新の主人公の形状から取得した値を使うように修正
    pBoxShape->m_e0     = widthVolume * focusSetting.playerEyeKeepScale;
    pBoxShape->m_e1     = m_NowSearchResult.checkLengh * 0.5f * focusSetting.playerEyeKeepScale;
    pBoxShape->m_u0     = u0;
    pBoxShape->m_u1     = u1;
    pBoxShape->m_height = heightVolume;
  }

  m_HitCheckLength = playerEyeLengh;
}

/**
* @brief フォーカス対象トレーナーを取得
*
* @param  pOut            今回の結果
* @param  pResultLostKeep keepFlagによるHITならtrue
* @param  rPrevResult     前回の結果
* @param  keepFlag        true = 前回のターゲットが存在した場合、Keepする
*
* @return 対象
*/
void Focus::SearchFocusTrainerEx( SearchResult* pOut, b32* pResultLostKeep, const SearchResult& rPrevResult, b32 keepFlag )
{
  pOut->isHit = false;
  *pResultLostKeep = false;

  if( m_pPlacementDataManager == NULL )
  {
    return;
  }


  const FieldTrainerActor* pResultTrainerActor( NULL );
  f32 hitLengthSq( 0.0f );
  f32 hitCheckLength( 0.0f );
  b32 keepHitFlag( false );

  // 配置されているトレーナーとコリジョン判定 @note 最大で16程度のスフィア判定なのでコリジョンの最適化はしていない
  for( u32 i = 0; i < PlacementDataManager::TRAINER_ACTOR_MAX; i++ )
  {
    const FieldTrainerActor* pWorkTrainerActor(  
      reinterpret_cast<const Field::FieldTrainerActor*>( m_pPlacementDataManager->GetTrainerActor( i ) ));

    if( pWorkTrainerActor == NULL )
    {
      continue;
    }

    // バニッシュフラグなどで消されている
    if( pWorkTrainerActor->IsRegist() == false )
    {
      continue;
    }

    // 対戦済みはスルー
    if( pWorkTrainerActor->IsBattleFinished() )
    {
      continue;
    }

    Field::MoveModel::FieldMoveModel* pCharaModel = pWorkTrainerActor->GetMoveModel();
    if( pCharaModel )
    {
      gfl2::math::Vector3 offset = pCharaModel->GetPositionPrevUpdateEnd() - m_pPlayerEyeDynamicActor->GetPosition();
      f32 workLengthSq = offset.LengthSq();

      // ヒット済みなら近い方を優先
      if( pResultTrainerActor && workLengthSq > hitLengthSq )
      {
        continue;
      }

      b32 prevHit = false;
      if( rPrevResult.isHit && rPrevResult.id == pWorkTrainerActor->GetData()->scriptID )
      {
        prevHit = true;
      }

      // @note カリング中は視線のShapeが正常更新されていないので、非アクティブになっている。HITしていない扱いにする
      // @note 旧EYEコリジョンは常に非アクティブになっているので、新コリジョンを参照する
      b32 isActive = pCharaModel->GetStaticActor( MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE_NEW )->IsActive();

      // 前回あたっているトレーナーの場合は、太らせた視線とコリジョンを行う + ロスト後しばらくキープする処理を実行する
      if( prevHit )
      {
        // 自機の視線とTRの視線
        if( !isActive || !TestOBBvsOBB( m_pKeepPlayerEyeDynamicActor, pCharaModel->GetStaticActor( MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE ) ))
        {
          if( keepFlag )
          {
            // キープ期間であればHIT扱いにする
            keepHitFlag = true;
          }
          else
          {
            continue;
          }
        }

        hitCheckLength = rPrevResult.checkLengh; 
      }
      else
      {
        // 自機の視線とTRの視線
        if( !isActive || !TestOBBvsOBB( m_pPlayerEyeDynamicActor, pCharaModel->GetStaticActor( MoveModel::COLLISION_STATIC_TYPE_TRAINER_EYE )))
        {
          continue;
        }

        hitCheckLength = m_HitCheckLength;
        keepHitFlag = false;
      }

      hitLengthSq = workLengthSq;
      pResultTrainerActor = pWorkTrainerActor;
    }

  } // for PlacementDataManager::TRAINER_ACTOR_MAX

  if( pResultTrainerActor )
  {
    pOut->isHit = true;
    pOut->position = pResultTrainerActor->GetMoveModel()->GetPositionPrevUpdateEnd();
    pOut->id = pResultTrainerActor->GetData()->scriptID;
    pOut->checkLengh = hitCheckLength;
    GetInterestPositionByActor( pResultTrainerActor, &pOut->interestPosition );
    *pResultLostKeep = keepHitFlag;
  }
}

#if PM_DEBUG
void Focus::SearchFocusTrainerForDebugCameraCheck( SearchResult* pOut, b32* pResultLostKeep, const SearchResult& rPrevResult, b32 keepFlag )
{
  pOut->isHit = false;
  *pResultLostKeep = false;

  if( m_pPlacementDataManager == NULL )
  {
    return;
  }

  const FieldTrainerActor* pResultTrainerActor( NULL );
  f32 hitLengthSq( 0.0f );
  f32 hitCheckLength( 0.0f );
  b32 keepHitFlag( false );

  // 配置されているトレーナーとコリジョン判定 @note 最大で16程度のスフィア判定なのでコリジョンの最適化はしていない
  for( u32 i = 0; i < PlacementDataManager::TRAINER_ACTOR_MAX; i++ )
  {
    const FieldTrainerActor* pWorkTrainerActor(  
      reinterpret_cast<const Field::FieldTrainerActor*>( m_pPlacementDataManager->GetTrainerActor( i ) ));

    if( pWorkTrainerActor == NULL )
    {
      continue;
    }

    // バニッシュフラグなどで消されている
    if( pWorkTrainerActor->IsRegist() == false )
    {
      continue;
    }

    // 対戦済みはスルー
    if( pWorkTrainerActor->IsBattleFinished() )
    {
      continue;
    }

    Field::MoveModel::FieldMoveModel* pCharaModel = pWorkTrainerActor->GetMoveModel();
    if( pCharaModel )
    {
      gfl2::math::Vector3 offset = pCharaModel->GetPositionPrevUpdateEnd() - m_pPlayerEyeDynamicActor->GetPosition();
      f32 workLengthSq = offset.LengthSq();

      // ヒット済みなら近い方を優先
      if( pResultTrainerActor && workLengthSq > hitLengthSq )
      {
        continue;
      }

      if( offset.Length() <= Field::Debug::DebugTypes::s_FocusCameraCheck.distance + Field::Debug::DebugTypes::s_FocusCameraCheck.geta )
      {
        // 指定範囲内にいる場合は強制的にHITにする
      }
      else
      {
        continue;
      }

      hitLengthSq = workLengthSq;
      pResultTrainerActor = pWorkTrainerActor;
    }

  } // for PlacementDataManager::TRAINER_ACTOR_MAX

  if( pResultTrainerActor )
  {
    pOut->isHit = true;
    pOut->position = pResultTrainerActor->GetMoveModel()->GetPositionPrevUpdateEnd();
    pOut->id = pResultTrainerActor->GetData()->scriptID;
    pOut->checkLengh = hitCheckLength;
    GetInterestPositionByActor( pResultTrainerActor, &pOut->interestPosition );
    *pResultLostKeep = keepHitFlag;
  }
}

#endif


/**
* @brief 注目座標を所得
*
* @param pTarget 対象
* @param pOut    座標
*
* @return 成否
*/
b32 Focus::GetInterestPositionByActor( const FieldTrainerActor* pTarget, gfl2::math::Vector3* pOut ) const
{
  const Field::MoveModel::FieldMoveModel* pMoveModel = pTarget->GetMoveModel();
  if( pMoveModel == NULL )
  {
    return false;
  }

  const poke_3d::model::BaseModel* pModel = pMoveModel->GetCharaDrawInstance();
  s32 jointIndex = pModel->GetJointIndex( "Loc_Eye" );
  if( jointIndex == -1 )
  {
    return false;
  }

  *pOut = pModel->GetJointWorldMatrix( jointIndex ).GetElemPosition();
  return true;
}


/**
* @brief カメラフレームIN処理、ターゲットを納めるカメラ位置を算出
*
* @param rOffsetVector  カメラ向き(TargetToPositon)
* @param fovY           画角
* @param pTarget        カメラターゲット座標格納先
* @param pPosition      カメラ座標格納先
* @param minDistace     最小カメラ距離
*
* @return 無し
*/
void Focus::GetPosEx( const SearchResult& rResult, const gfl2::math::Vector3& rOffsetVector, f32 fovY, gfl2::math::Vector3* pTarget, gfl2::math::Vector3* pPosition, f32 minDistace, f32 maxDistance ) const
{
  gfl2::math::Vector3 dir( rOffsetVector.Normalize() );

  struct PosData
  {
    gfl2::math::Vector3 pos;
    f32                 weight;
  };

  PosData posDataTable[4];

  // 自機の足元座標
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  gfl2::math::Vector3 playerPosition( pFieldMap->GetPlayerPosition() );

  // ターゲットの足元座標
  gfl2::math::Vector3 focusTargetPosition = rResult.position;

  //
  // 画面に納めたい点の中心(カメラターゲット座標)を求める
  //
  posDataTable[0].pos = playerPosition;
  posDataTable[0].weight = FOCUS_POINT_WEIGHT[0];
  posDataTable[1].pos = playerPosition + gfl2::math::Vector3( 0.0f, FOCUS_POINT_PLAYER_TOP_OFFSET, 0.0f );
  posDataTable[1].weight = FOCUS_POINT_WEIGHT[1];
  posDataTable[2].pos = focusTargetPosition;
  posDataTable[2].weight = FOCUS_POINT_WEIGHT[2];
  posDataTable[3].pos = focusTargetPosition + gfl2::math::Vector3( 0.0f, FOCUS_POINT_TARGET_TOP_OFFSET, 0.0f );
  posDataTable[3].weight = FOCUS_POINT_WEIGHT[3];

  // カメラターゲット座標を算出
  gfl2::math::Vector3 center = gfl2::math::Vector3::GetZero();
  f32 totalWeight = 0.0f;
  for( u32 i = 0; i < GFL_NELEMS( posDataTable ); i++ )
  {
    center += posDataTable[i].pos * posDataTable[i].weight;
    totalWeight += posDataTable[i].weight;
  }
  center = center / totalWeight;


  //
  // 以下はカメラ座標系で行う
  //

  gfl2::math::Matrix34 viewMat = gfl2::math::Matrix34::GetLookAt(
    center + rOffsetVector,
    center,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f )
    );
  gfl2::math::Vector3 centerForViewSpace( viewMat.TransformCoord( center ) );
  gfl2::math::Vector3 dirOfViewSpace( 0.0f, 0.0f, 1.0f );

  fovY *= 0.5f;
  f32 aspect( 400.0f / 240.0f );
  //f32 theta = (aspect >= 1.0f) ? fovY : fovY * aspect;

  f32 thetaY = fovY * ( focusSetting.frameInHeight / 240.0f );
  f32 thetaX = fovY * aspect * ( focusSetting.frameInWidth / 400.0f );


  // 全ての点を画面に捉える距離を算出
  f32 distance( 0.0f );
  for( u32 i = 0; i < GFL_NELEMS( posDataTable ); i++ )
  {
    gfl2::math::Vector3 posOfViewSpace = viewMat.TransformCoord( posDataTable[i].pos );


    // 三角関数が使えるように、直交する位置を取得
    gfl2::math::Vector3 centerToPos( posOfViewSpace - centerForViewSpace );
    gfl2::math::Vector3 posWork;
    posWork = dirOfViewSpace * centerToPos.Dot( dirOfViewSpace );
    posWork = centerForViewSpace + posWork;

    // tanから距離を算出
    // X
    {
      f32 r = gfl2::math::Abs( posOfViewSpace.x - posWork.x );
      f32 distanceWork = r / tan( thetaX );
      distanceWork += centerToPos.Dot( dirOfViewSpace );

      // 最大距離を記憶
      distance = ( distanceWork > distance ) ? distanceWork : distance;
    }

    // Y
    {
      f32 r = gfl2::math::Abs( posOfViewSpace.y - posWork.y );
      f32 distanceWork = r / tan( thetaY );
      distanceWork += centerToPos.Dot( dirOfViewSpace );

      // 最大距離を記憶
      distance = ( distanceWork > distance ) ? distanceWork : distance;
    }

  }

  // 距離を指定範囲に収める
  distance = gfl2::math::Clamp( distance, minDistace, maxDistance );


  // 結果を格納
  *pTarget = center;
  *pPosition = center + dir * distance;
}

// サーチ結果からカメラ位置を算出
void Focus::GetFixParam( const poke_3d::model::BaseCamera* pCamera, const SearchResult& rResult,  const FocusCameraParam& rOffset, FocusCameraParam* pParam, f32 minDistace ) const
{

  // 点を画面に納める
  gfl2::math::Vector3 offset( pCamera->GetPosition() - pCamera->GetTargetPosition() ); // 元々の向きを維持

  // 引く距離の制限
  f32 maxDistance = minDistace * focusSetting.maxDistaceScale;

  // ターゲットが収まるカメラ位置を算出
  f32 targetFovY = pCamera->GetFovy() * focusSetting.fovValue;
  gfl2::math::Vector3 cameraPostion;
  gfl2::math::Vector3 cameraTargetPostion;
  GetPosEx( m_NowSearchResult, offset, targetFovY, &cameraTargetPostion, &cameraPostion, minDistace, maxDistance );

  // 最新の座標->フォーカスターゲットを収める座標に遷移
  pParam->m_CameraTargetPos = pCamera->GetTargetPosition() + rOffset.m_CameraTargetPos;
  pParam->m_CameraPos = pCamera->GetPosition() + rOffset.m_CameraPos;
  f32 srcFovY = pCamera->GetFovy() + rOffset.m_FovY;

  pParam->m_CameraTargetPos.Lerp( cameraTargetPostion, m_FocusValueController.GetValue() );
  pParam->m_CameraPos.Lerp( cameraPostion, m_FocusValueController.GetValue() );
  pParam->m_FovY = srcFovY + ( targetFovY - srcFovY ) * m_FocusValueController.GetValue();
}

// FocusCameraParamの線形補間
void Focus::LerpParam( const FocusCameraParam& rSrc, const FocusCameraParam& rTarget, FocusCameraParam* pOut, f32 t ) const
{
  pOut->m_CameraPos = rSrc.m_CameraPos;
  pOut->m_CameraPos.Lerp( rTarget.m_CameraPos, t );

  pOut->m_CameraTargetPos = rSrc.m_CameraTargetPos;
  pOut->m_CameraTargetPos.Lerp( rTarget.m_CameraTargetPos, t );

  pOut->m_FovY = rSrc.m_FovY + ( rTarget.m_FovY - rSrc.m_FovY ) * t;
}

// 現在のカメラデータをFocusCameraParamに設定
void Focus::SetParam( const poke_3d::model::BaseCamera* pCamera, FocusCameraParam* pOut ) const
{
  pOut->m_CameraPos = pCamera->GetPosition();
  pOut->m_CameraTargetPos = pCamera->GetTargetPosition();
  pOut->m_FovY = pCamera->GetFovy();
}

// FocusCameraParamを0クリア
void Focus::ClearParam( FocusCameraParam* pOut ) const
{
  pOut->m_CameraPos = gfl2::math::Vector3::GetZero();
  pOut->m_CameraTargetPos = gfl2::math::Vector3::GetZero();
  pOut->m_FovY = 0.0f;
}

// FocusCameraParamを足した結果を取得
void Focus::AddParam( const FocusCameraParam& rSrc1, const FocusCameraParam& rSrc2, FocusCameraParam* pOut ) const
{
  pOut->m_CameraPos = rSrc1.m_CameraPos + rSrc2.m_CameraPos;
  pOut->m_CameraTargetPos = rSrc1.m_CameraTargetPos + rSrc2.m_CameraTargetPos;
  pOut->m_FovY = rSrc1.m_FovY + rSrc2.m_FovY;
}


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );