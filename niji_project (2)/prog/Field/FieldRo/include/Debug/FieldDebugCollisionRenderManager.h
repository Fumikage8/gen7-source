//======================================================================
/**
 * @file FieldDebugCollisionRenderManager.h
 * @date 2015/07/10 16:01:56
 * @author ikawa_hiroki
 * @brief フィールド関連コリジョン可視化管理マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELDDEBUGCOLLISIONRENDERMANAGER_H_INCLUDED__
#define __FIELDDEBUGCOLLISIONRENDERMANAGER_H_INCLUDED__
#pragma once

#if PM_DEBUG

#include <Collision/include/gfl2_CollisionModel.h>
#include <macro/include/gfl2_Macros.h>
#include "Field/FieldRo/include/Collision/BaseActor.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"

#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
  * @class DebugCollisionRenderManager
  * @brief フィールド関連コリジョン可視化管理マネージャー
  */
class DebugCollisionRenderManager : public gfl2::base::Singleton< DebugCollisionRenderManager >
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( DebugCollisionRenderManager );

public:
  /**
    * @enum Value
    * 各種値の設定
    */
  enum Value
  {
    MAX_RENDER_ACTOR = 128,
    MAX_RENDER_MODEL = 128,
    MAX_RENDER_PRIM = 128,
    MAX_RENDER_ENV_SOUND = 128,
  };

  /**
    * @enum FieldDebugCollisionType
    * 地形コリジョンの種類
    */
  enum FieldDebugCollisionType
  {
    COLLISION_TYPE_FLOOR = 0,   // 床面
    COLLISION_TYPE_WALL,    // 壁面
    COLLISION_TYPE_PLACEMENT,   // PlacementData生成物
    COLLISION_TYPE_CAMERA,    // カメラエリア
    COLLISION_TYPE_ENV_SOUND,   // 環境音
    NUM_COLLISION_TYPE,
  };

  /**
    * @enum CollisionPrimitiveType
    * コリジョンプリミティブの種類
    * 形状を直に指定する際に使用の予定
    * Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager に準拠
    * （円は高さ0の円柱で対処）
    */
  enum CollisionPrimitiveType
  {
    COLLISION_PRIMITIVE_CYLINDER = 0,   // 
    NUM_COLLISION_PRIMITIVE_TYPE,
  };
  /**
    * @struct RenderCollisionActorInfo
    * @brief  アクターとレンダリング情報のセット
    */
  struct RenderCollisionActorInfo
  {
    BaseActor*  pActor;
    gfl2::math::Vector4   color;
    gfl2::math::Vector3   pos;
    
    // カラーテーブル、　使われ方は形状毎に違う
    const gfl2::math::Vector4* pColorTable;
    u32                        colorTableSize;

    RenderCollisionActorInfo():
      pActor( NULL ),
      color( 0, 0, 0 ),
      pos( 0.0f, 0.0f, 0.0f ),
      pColorTable( NULL ),
      colorTableSize( 0 )
    {
    };
    RenderCollisionActorInfo( BaseActor* p_act, gfl2::math::Vector4 col, gfl2::math::Vector3 position, const gfl2::math::Vector4* pColTable, u32 colTableSize ):
      pActor( p_act ),
      color( col ),
      pos( position ),
      pColorTable( pColTable ),
      colorTableSize( colTableSize )
    {
    };

  };

  /**
    * @struct RenderCollisionModelInfo
    * @brief  モデルとレンダリング情報のセット
    */
  struct RenderCollisionModelInfo
  {
    gfl2::collision::CollisionModel *pModel;
    gfl2::math::Vector4   color;
    b32                   isHightClamp;
    s32                   hitAttr;

    // アトリビュート番号別カラー、sizeオーバー、エラー時はcolorを使用する
    const gfl2::math::Vector4* pAttrColorTable;
    u32                        attrColorTableSize;


    RenderCollisionModelInfo():
      pModel( NULL ),
      color( 0, 0, 0 ),
      isHightClamp( false ),
      hitAttr( -1 ),
      pAttrColorTable( NULL ),
      attrColorTableSize( 0 )
    {
    };
    RenderCollisionModelInfo( gfl2::collision::CollisionModel* p_model, gfl2::math::Vector4 col ):
      pModel( p_model ),
      color( col ),
      isHightClamp( false ),
      hitAttr( -1 ),
      pAttrColorTable( NULL ),
      attrColorTableSize( 0 )
    {
    };

  };

  /**
    * @struct RenderCollisionPrimitiveInfo
    * @brief  プリミティブレンダリング情報のセット
    */
  struct RenderCollisionPrimitiveInfo
  {
    u32 type;
    gfl2::math::Vector3 position;
    BaseShape*  pShape;
    gfl2::math::Vector4   color;
    b32 isDepthTestEnable;

    RenderCollisionPrimitiveInfo():
      type( COLLISION_PRIMITIVE_CYLINDER ),
      pShape( NULL ),
      color( 0, 0, 0 ),
      isDepthTestEnable( false )
    {
    };

  };

  /**
    * @struct RenderEnvSoundCollisionInfo
    * @brief  環境音コリジョンレンダリング情報のセット
    */
  struct RenderEnvSoundCollisionInfo
  {
    const void*     cpData;
    gfl2::math::Vector4   color;

    RenderEnvSoundCollisionInfo():
      cpData( NULL ),
      color( 0, 0, 0 )
    {
    };

  };

  /**
    * @brief コンストラクタ
    * @return なし
    * @detail 外部からのアクセス禁止
    */
  DebugCollisionRenderManager( void );

  /**
    * @brief デストラクタ
    * @return なし
    * @detail
    */
  virtual ~DebugCollisionRenderManager( void ){};

  /**
  * @brief コリジョンアクターを登録
  * @return なし
  * @detail 
  */
  void RegistRenderCollisionActor( BaseActor* pActor, gfl2::math::Vector4 color, gfl2::math::Vector3 positon = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ), const gfl2::math::Vector4* pColTable = NULL, u32 colTableSize = 0);

  /**
  * @brief コリジョンアクターを取得
  * @return RenderCollisionActorInfoのポインタ
  * @detail 
  */
  RenderCollisionActorInfo* GetRenderCollisionActorInfo( u32 index ){ return &m_RenderCollisionActorInfo[ index ]; };

  /**
  * @brief コリジョンアクター登録数を取得
  * @return コリジョンアクター登録数
  * @detail 
  */
  u32 GetRenderCollisionActorInfoNum( void ){ return m_nActor; };

  /**
  * @brief コリジョンアクターを登録解除
  * @return なし
  * @detail 
  */
  void ClearRenderCollisionActorInfo();

  /**
  * @brief コリジョンモデルを登録
  * @return なし
  * @detail 
  */
  void RegistRenderCollisionModel( gfl2::collision::CollisionModel* pModel, gfl2::math::Vector4 color, b32 isHightClamp = false, s32 hitAttr = -1 );

  /**
  * @brief コリジョンモデルを取得
  * @return RenderCollisionModelのポインタ
  * @detail 
  */
  RenderCollisionModelInfo* GetRenderCollisionModelInfo( u32 index ){ return &m_RenderCollisionModelInfo[ index ]; };

  /**
  * @brief コリジョンモデル登録数を取得
  * @return コリジョンモデル登録数
  * @detail 
  */
  u32 GetRenderCollisionModelInfoNum( void ){ return m_nModel; };

  /**
  * @brief コリジョンモデルを登録解除
  * @return なし
  * @detail
  */
  void ClearRenderCollisionModelInfo();

  /**
  * @brief コリジョンプリミティブを登録
  * @return なし
  * @detail 
  */
  void RegistRenderCollisionCylinder( gfl2::math::Vector3 position, f32 radius, f32 height, gfl2::math::Vector4 color, b32 isDepthTestEnable );

  /**
  * @brief コリジョンプリミティブを取得
  * @return RenderCollisionPrimitiveInfoのポインタ
  * @detail 
  */
  RenderCollisionPrimitiveInfo* GetRenderCollisionPrimitiveInfo( u32 index ){ return &m_RenderCollisionPrimInfo[ index ]; };

  /**
  * @brief コリジョンプリミティブ登録数を取得
  * @return コリジョンプリミティブ登録数
  * @detail 
  */
  u32 GetRenderCollisionPrimitiveInfoNum( void ){ return m_nPrim; };

  /**
  * @brief コリジョンプリミティブを登録解除
  * @return なし
  * @detail 
  */
  void ClearRenderCollisionPrimitiveInfo();

  /**
  * @brief 環境音コリジョンデータを登録
  * @return なし
  * @detail
  */
  void RegistRenderEnvSoundCollision( const void* cpData, gfl2::math::Vector4 color );

  /**
  * @brief 環境音コリジョンデータを取得
  * @return RenderEnvSoundCollisionInfoのポインタ
  * @detail 
  */
  RenderEnvSoundCollisionInfo* GetRenderEnvSoundCollisionInfo( u32 index ){ return &m_RenderEnvSoundCollisionInfo[ index ]; };

  /**
  * @brief 環境音コリジョンデータ登録数を取得
  * @return 環境音コリジョンデータ登録数
  * @detail 
  */
  u32 GetRenderEnvSoundCollisionInfoNum( void ){ return m_nEnvSound; };

  /**
  * @brief 環境音コリジョンデータを登録解除
  * @return なし
  * @detail 
  */
  void ClearRenderEnvSoundCollisionInfo();


  f32 GetPlayerPosY( void ) const { return m_PlyerPosY; }
  void SetPlayerPosY( f32 posY ) { m_PlyerPosY = posY; }


private:
  RenderCollisionActorInfo    m_RenderCollisionActorInfo[MAX_RENDER_ACTOR];    // アクターとレンダリング情報のセット
  RenderCollisionActorInfo*   m_pActor;    // 空き領域先頭保持用ポインタ
  u32   m_nActor;       // 登録アクター数

  RenderCollisionModelInfo    m_RenderCollisionModelInfo[MAX_RENDER_MODEL];
  RenderCollisionModelInfo*   m_pModel;
  u32   m_nModel;

  RenderCollisionPrimitiveInfo    m_RenderCollisionPrimInfo[MAX_RENDER_PRIM];
  RenderCollisionPrimitiveInfo*   m_pPrim;
  u32   m_nPrim;

  RenderEnvSoundCollisionInfo     m_RenderEnvSoundCollisionInfo[MAX_RENDER_ENV_SOUND];
  RenderEnvSoundCollisionInfo*    m_pEnvSound;
  u32   m_nEnvSound;


  f32   m_PlyerPosY;
};

GFL_NAMESPACE_END( Debug )
GFL_NAMESPACE_END( Field )

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( Field::Debug::DebugCollisionRenderManager );
#endif  //#if PM_DEBUG

#endif // __FIELDDEBUGCOLLISIONRENDERMANAGER_H_INCLUDED__
