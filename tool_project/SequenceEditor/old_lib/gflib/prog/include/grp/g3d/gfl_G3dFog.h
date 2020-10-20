#ifndef __GFL_G3DFOG_H__
#define __GFL_G3DFOG_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFog.h
 *	@brief  3Dフォグ
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[参照テーブルを使用したフォグの説明]

参照テーブルの値は次のように使われる。

横軸 : 0がフォグの開始デプスの場所に相当する
       1がフォグの終了デプスの場所に相当する
       (NW4C中間ファイルでは範囲が0<= <=1でなく0<= <=255になっている)

縦軸 : 0のときフォグカラーが全く混じらない
       1のとき完全にフォグカラーになる

例 :

1 |       /
  |  ----
  | /
  +--------
0         1

開始デプスの場所では、フォグカラーが全く混じらない
開始デプスと終了デプスの中間の場所では、フォグカラーが半分混じっている
終了デプスの場所では、完全にフォグカラーになる*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dResource.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dフォグクラス
//=====================================
class Fog : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Fog);


public:
  //-------------------------------------
  ///	フォグテーブル構造体
  //=====================================
  static const u32 FOG_TABLE_VALUE_NUM = 128;  // フォグテーブルの要素数
  struct FogTable
  {
    f32 value[FOG_TABLE_VALUE_NUM];
        // index : 0がフォグの開始デプスの場所に相当する
        //         FOG_TABLE_VALUE_NUM-1がフォグの終了デプスの場所に相当する
        // value[index] : 0のときフォグカラーが全く混じらない
        //                1のとき完全にフォグカラーになる
  };
  static void InitializeFogTableForDefault(
      FogTable*    fog_table
  );
  static void SetupFogTableUsingLookupTable(  // この関数から戻ってきたら呼び出し元はlookup_table_resourceを削除してもよい
      FogTable*    fog_table,                 // 呼び出し元でfog_tableのメモリは確保しておくこと
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );


public:
  // 定数
  enum
  {
    LOOKUP_TABLE_INDEX_NONE    // 未使用でインデックスが設定されていない場合の値
  };
  // 関数
  enum Function
  {
    FUNCTION_LINEAR             = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_LINEAR,           // 線形関数
    FUNCTION_EXPONENT           = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT,         // 指数関数
    FUNCTION_EXPONENT_SQUARE    = nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT_SQUARE   // 二乗の指数関数
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Fog(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Fog();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         参照テーブルを使用したフォグの生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     lookup_table_resource                   参照テーブルのリソース
   *  @param[in]     lookup_table_set_index_in_resource      参照テーブルセットのリソース内におけるインデックス
   *  @param[in]     lookup_table_index_in_lookup_table_set  参照テーブルの参照テーブルセット内におけるインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void CreateFogUsingLookupTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource,
      Resource*                lookup_table_resource,
      s32                      lookup_table_set_index_in_resource,
      s32                      lookup_table_index_in_lookup_table_set
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         フォグテーブルを使用したフォグの生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     fog_table          フォグテーブル(内容をコピーしたものを保持するので、この関数から戻ってきたら、呼び出し元はfog_tableを削除してもよい)
   */
  //-----------------------------------------------------------------------------
  virtual void CreateFogUsingFogTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource,
      const FogTable&          fog_table
  );
  virtual void CreateFogUsingFogTable(
      gfl::heap::NwAllocator*  allocator,
      gfl::heap::NwAllocator*  device_allocator,
      Resource*                resource,
      s32                      index_in_resource
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         カラー
   *                 u8のときは0が黒&透明、255が白&不透明
   *                 f32のときは0.0fが黒&透明、1.0fが白&不透明
   */
  //-----------------------------------------------------------------------------
  void SetColor(f32 r, f32 g, f32 b);
  void SetColor(const gfl::grp::ColorF32& c);  // aはどう使われるか不明
  void SetColor(u8 r, u8 g, u8 b);
  void SetColor(const gfl::grp::ColorU8& c);   // aはどう使われるか不明
  gfl::grp::ColorF32 GetColor(void) const;     // aはどう使われるか不明
  void GetColor(gfl::grp::ColorF32* c) const;  // aはどう使われるか不明
  void GetColor(gfl::grp::ColorU8*  c) const;  // aはどう使われるか不明
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         説明
   */
  //-----------------------------------------------------------------------------
  void     SetFunction(Function function);
  Function GetFunction(void) const;
  void SetDensity(f32 value);
  f32  GetDensity(void) const;
  void SetMinDepth(f32 min);
  f32  GetMinDepth(void) const;
  void SetMaxDepth(f32 max);
  f32  GetMaxDepth(void) const;
  void SetDepth(f32 min, f32 max);
  void SetIntensity(const f32 intensity);  // m_typeがTYPE_CREATE_FOG_TABLEかTYPE_USE_LOOKUP_TABLE_RESOURCEのときのみ有効。  // 0.0fで全くフォグが出ない、1.0fでテーブルの通りにフォグが出る(0.0f<= <=1.0f)
  f32  GetIntensity(void) const;  // m_typeがTYPE_CREATE_FOG_TABLEかTYPE_USE_LOOKUP_TABLE_RESOURCEのときのみ有効。                         
  void SetFogTable(
      const FogTable&          fog_table  // フォグテーブル(内容をコピーしたものを保持するので、この関数から戻ってきたら、呼び出し元はfog_tableを削除してもよい)
  );  // m_typeがTYPE_CREATE_FOG_TABLEのときのみ有効。


  //-----------------------------------------------------------------------------
  /**
   *  @brief         有効フラグ
   */
  //-----------------------------------------------------------------------------
  void SetEnable(bool enable);
  b32 IsEnable(void) const;
  // @note フォグを無効にするときにnw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_NONEを使おうかと思ったが、
  //       これを使用するとNW4CのアサートNW_ASSERTMSG(false, "Unknown FogUpdater type.");に引っ掛かってしまう。
  //       だから、独自の方法で無効にするようにした(正確にはフォグが有効だが掛からないようにしている)。


//protected:
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         説明
   */
  //-----------------------------------------------------------------------------
  nw::gfx::Fog*          GetNwFog(void) const            {  return nw::ut::DynamicCast<nw::gfx::Fog*>( GetNwSceneNode() );  }
  nw::gfx::ResFog        GetNwResFog(void) const         {  return GetNwFog()->GetResFog();  }
  nw::gfx::ResFogUpdater GetNwResFogUpdater(void) const  {  return GetNwResFog().GetFogUpdater();  }
  void                   Update(Camera* camera);

  void                   UpdateUsingLookupTable(Camera* camera);
  void                   UpdateUsingFogTable(Camera* camera);
  void                   updateUsingFogTable(Camera* camera, const FogTable& fog_table );


private:
  enum Type
  {
    TYPE_NONE,                      // まだフォグを生成していない
    TYPE_NORMAL,                    // 普通のNW4Cの機能だけを使ったフォグ
    TYPE_CREATE_FOG_TABLE,          // フォグテーブルFogTableをつくり使っているフォグ
    TYPE_USE_LOOKUP_TABLE_RESOURCE  // 参照テーブルのリソースを使ったフォグ
  };
  static const f32 DISABLE_MAX_DEPTH = 100000000.0f;
  static const f32 DISABLE_MIN_DEPTH = DISABLE_MAX_DEPTH - 0.01f;

private:
  b32  m_enable_flag;
  f32  m_enable_min_depth;
  f32  m_enable_max_depth;

  Type       m_type;
  FogTable*  m_fog_table;

  Resource*  m_lookup_table_resource;
  s32        m_lookup_table_set_index_in_resource;
  s32        m_lookup_table_index_in_lookup_table_set;

  f32        m_intensity;    // m_typeがTYPE_CREATE_FOG_TABLEかTYPE_USE_LOOKUP_TABLE_RESOURCEのときのみ有効。  // 0.0fで全くフォグが出ない、1.0fでテーブルの通りにフォグが出る(0.0f<= <=1.0f)
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DFOG_H__
