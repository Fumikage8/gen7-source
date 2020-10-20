#ifndef __GFL_G3DOBJECT_H__
#define __GFL_G3DOBJECT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dObject.h
 *	@brief  3Dオブジェクト
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	オブジェクトのID
//=====================================
enum
{
  OBJECT_ID_NOT_MANAGED       = -1  // ObjectManagerに管理されていないことを示すIDの値。
  // 0以上はObjectManagerに管理されており、オブジェクト自身のIDとなる。
};

//-------------------------------------
///	このオブジェクトをつくったリソースのID
//=====================================
enum
{
  OBJECT_RESOURCE_ID_NOT_USE    = -1,  // リソースを使っていないことを示すIDの値。
  OBJECT_RESOURCE_ID_PARTICLE   = -2   // パーティクルのためリソース内の複数のデータからオブジェクトをつくったことを示すIDの値。
  // 0以上はリソースを使っており、そのリソースのIDとなる。
};

//-------------------------------------
///	リソース内におけるこのオブジェクトのインデックス
//=====================================
enum
{
  OBJECT_INDEX_IN_RESOURCE_NOT_USE    = -1  // リソースを使っていないことを示すインデックスの値。
  // 0以上はリソースを使っており、そのリソース内でのインデックスとなる。
};

//-------------------------------------
///	ユーザが任意で付けられるID
//=====================================
enum
{
  OBJECT_USER_ID_NOT_USE    = -1,  // ユーザが任意で付けられるIDを付けていないことを示すIDの値。
  OBJECT_USER_ID_ANYTHING   = -2   // ユーザが任意で付けられるIDは何でもいい。ユーザが任意で付けられるIDを指定せずに検索する場合に用いるIDの値。
  // 0以上はユーザが任意で付けられるIDを付けており、このオブジェクトにユーザが付けたIDとなる。
};

//-------------------------------------
///	オブジェクトの所有者
//=====================================
typedef enum
{
  OBJECT_OWNER_SCENE,  // Sceneが管理しているオブジェクト。Sceneが解放も行う。
  OBJECT_OWNER_USER    // ユーザが管理しているオブジェクト。ユーザが解放も行う。
}
ObjectOwner;

//-------------------------------------
///	オブジェクトの所有者のID
//=====================================
enum
{
  OBJECT_OWNER_ID_USER    = -1  // ユーザが管理しているオブジェクトの場合(OBJECT_OWNER_USER)のIDの値。
  // 0以上はユーザ以外が管理しており、その管理しているもののIDとなる。(例：Sceneが管理している場合(OBJECT_OWNER_SCENE)ならSceneのID)
};

//-------------------------------------
///	オブジェクトの種類
//=====================================
typedef enum
{
  // 順番を並び替える可能性がありますので、
  // OBJECT_TYPE_MODEL<=type&&type<=OBJECT_TYPE_FOG
  // という使い方はしないで下さい。
  OBJECT_TYPE_UNKNOWN,                    //  0
  
  OBJECT_TYPE_RESOURCE,                   //  1

  OBJECT_TYPE_RES_ANIM,                   //  2

  OBJECT_TYPE_ANIMATION,                  //  3
  OBJECT_TYPE_UNIT_ANIMATION,             //  4

  OBJECT_TYPE_TEXTURE,                    //  5

  OBJECT_TYPE_PARTICLE,                   //  6

  OBJECT_TYPE_SCENE_ENVIRONMENT_SETTING,  //  7

  OBJECT_TYPE_MODEL,                      //  8
  OBJECT_TYPE_CAMERA,                     //  9
  OBJECT_TYPE_STEREO_CAMERA,              // 10
  OBJECT_TYPE_LIGHT,                      // 11
  OBJECT_TYPE_FOG,                        // 12

  OBJECT_TYPE_SCENE,                      // 13

  OBJECT_TYPE_H3D_MODEL,                  // 14

  OBJECT_TYPE_MAX
}
ObjectType;


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dオブジェクトクラス
//=====================================
class Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Object);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  Object(void)
    : m_id(OBJECT_ID_NOT_MANAGED),
      m_resource_id(OBJECT_RESOURCE_ID_NOT_USE),
      m_index_in_resource(OBJECT_INDEX_IN_RESOURCE_NOT_USE),
      m_user_id(OBJECT_USER_ID_NOT_USE),
      m_owner(OBJECT_OWNER_USER),
      m_owner_id(OBJECT_OWNER_ID_USER),
      m_type(OBJECT_TYPE_UNKNOWN)
  {}
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~Object() {}

  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void) {}

  //-----------------------------------------------------------------------------
  /**
   *  @brief         IDを得る
   *
   *  @retval        s32   ID(0以上)
   */
  //-----------------------------------------------------------------------------
  s32 GetId(void) const {  return m_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このオブジェクトをつくったリソースのIDを得る
   *
   *  @retval        s32   このオブジェクトをつくったリソースのID(0以上)
   */
  //-----------------------------------------------------------------------------
  s32 GetResourceId(void) const {  return m_resource_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソース内におけるこのオブジェクトのインデックスを得る
   *
   *  @retval        s32   リソース内におけるこのオブジェクトのインデックス(0以上)
   */
  //-----------------------------------------------------------------------------
  s32 GetIndexInResource(void) const {  return m_index_in_resource;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソース内におけるこのオブジェクトのインデックスを得る
   *  @brief         ユーザが任意で付けられるIDを得る
   *
   *  @retval        s32   ユーザが任意で付けられるID(0以上)
   */
  //-----------------------------------------------------------------------------
  s32 GetUserId(void) const {  return m_user_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このオブジェクトをつくったリソースのIDを得る
   *                 リソース内におけるこのオブジェクトのインデックスを得る
   *
   *  @param[out]    a_resource_id         このオブジェクトをつくったリソースのID(0以上)を格納する
   *  @param[out]    a_index_in_resource   リソース内におけるこのオブジェクトのインデックス(0以上)を格納する
   */
  //-----------------------------------------------------------------------------
  void GetResourceIdAndIndexInResource(s32& a_resource_id, s32& a_index_in_resource ) const
  {
    a_resource_id       = GetResourceId();
    a_index_in_resource = GetIndexInResource();
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         所有者を得る
   *
   *  @retval        ObjectOwner   オブジェクトの所有者
   */
  //-----------------------------------------------------------------------------
  ObjectOwner GetOwner(void) const {  return m_owner;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         所有者のIDを得る
   *
   *  @retval        s32   所有者のID(0以上)
   */
  //-----------------------------------------------------------------------------
  s32 GetOwnerId(void) const {  return m_owner_id;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         所有者を得る
   *                 所有者のIDを得る
   *
   *  @param[out]    a_owner      オブジェクトの所有者を格納する
   *  @param[out]    a_owner_id   所有者のID(0以上)を格納する
   */
  //-----------------------------------------------------------------------------
  void GetOwnerAndOwnerId(ObjectOwner& a_owner, s32& a_owner_id) const
  {
    a_owner     = GetOwner();
    a_owner_id  = GetOwnerId();
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         種類を得る
   *
   *  @retval        ObjectType   オブジェクトの種類
   */
  //-----------------------------------------------------------------------------
  ObjectType GetObjectType(void) const {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ユーザが任意で付けられるIDを設定する
   *                 ユーザが管理しているオブジェクトでない場合は、ユーザが任意で付けられるIDを設定できない。
   *
   *  @param[in]     a_user_id  ユーザが任意で付けられるID(0以上しか設定させたくないのでu16にした)
   * 
   *  @retval        bool   ユーザが任意で付けられるIDを設定できたらtrueを返す
   */
  //-----------------------------------------------------------------------------
  bool SetUserId(u16 a_user_id)
  {
    if( m_owner == OBJECT_OWNER_USER )
    {
      m_user_id = a_user_id;
      return true;
    }
    return false;
  }

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このオブジェクトをつくったリソースのIDを設定する
   *                 リソース内におけるこのオブジェクトのインデックスを設定する
   *
   *  @param[in]     a_resource_id         このオブジェクトをつくったリソースのID
   *  @param[in]     a_index_in_resource   リソース内におけるこのオブジェクトのインデックス
   */
  //-----------------------------------------------------------------------------
  void SetResourceIdAndIndexInResource(s32 a_resource_id, s32 a_index_in_resource)
  {
    m_resource_id       = a_resource_id;
    m_index_in_resource = a_index_in_resource;
  }

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトにIDを設定する
   *                 class Objectのフレンド関数
   *
   *  @param[in]     object    オブジェクト 
   *  @param[in]     a_id      ID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetId(Object* object, s32 a_id);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         オブジェクトに種類を設定する
   *                 class Objectのフレンド関数
   *
   *  @param[in]     object    オブジェクト 
   *  @param[in]     a_type    オブジェクトの種類
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetType(Object* object, ObjectType a_type);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         所有者を設定する
   *                 所有者のIDを設定する
   *                 class Objectのフレンド関数
   *
   *  @param[in]     object       オブジェクト 
   *  @param[in]     a_owner      オブジェクトの所有者
   *  @param[in]     a_owner_id   所有者のID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetOwnerAndOwnerId(Object* object, ObjectOwner a_owner, s32 a_owner_id);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ユーザが任意で付けられるIDを設定する(強制的)
   *                 ユーザが管理していないオブジェクトでも、ユーザが任意で付けられるIDを設定できる。
   *
   *  @param[in]     object     オブジェクト 
   *  @param[in]     a_user_id  ユーザが任意で付けられるID
   */
  //-----------------------------------------------------------------------------
  friend void Object_SetUserIdForcedly(Object* object, s32 a_user_id);

private:
  s32           m_id;                 // OBJECT_ID                 // ID(0以上)
  s32           m_resource_id;        // OBJECT_RESOURCE_ID        // このオブジェクトをつくったリソースのID(0以上)
  s32           m_index_in_resource;  // OBJECT_INDEX_IN_RESOURCE  // リソース内におけるこのオブジェクトのインデックス(0以上)
  s32           m_user_id;            // OBJECT_USER_ID            // ユーザが任意で付けられるID(0以上)
  ObjectOwner   m_owner;              // OBJECT_OWNER
  s32           m_owner_id;           // OBJECT_OWNER_ID           //	所有者のID(0以上)
  ObjectType    m_type;               // OBJECT_TYPE
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DOBJECT_H__
