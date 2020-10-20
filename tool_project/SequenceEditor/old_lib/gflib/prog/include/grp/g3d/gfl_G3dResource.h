#ifndef __GFL_G3DRESOURCE_H__
#define __GFL_G3DRESOURCE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResource.h
 *	@brief  3Dリソース
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObject.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	クラスの前方宣言
//=====================================
class ResAnim;


//-------------------------------------
///	3Dリソースクラス
//=====================================
class Resource : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Resource);

public:
  //---------------------------------------------------------------------------
  /**
   *           定数
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	リソース用の定数
  //=====================================
  enum
  {
    RESOURCE_INDEX_NOT_FOUND    = -1  // 指定した名前を持つリソースが存在しない場合のリソースのインデックス
  };

  //-------------------------------------
  ///	共通リソースのビット
  //=====================================
  enum
  {
    COMMON_RESOURCE_NONE                           = 0,
    COMMON_RESOURCE_DEFAULT_SHADER_BIT             = 1 <<  0,
    COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT    = 1 <<  1,
    COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT          = 1 <<  2,

    // Ex共通データを共通リソースとして用いる(Ex共通リソースと呼ぶ)
    EX_COMMON_RESOURCE_BIT_00  = 1 <<  3,
    EX_COMMON_RESOURCE_BIT_01  = 1 <<  4,
    EX_COMMON_RESOURCE_BIT_02  = 1 <<  5,
    EX_COMMON_RESOURCE_BIT_03  = 1 <<  6,
    EX_COMMON_RESOURCE_BIT_04  = 1 <<  7,
    EX_COMMON_RESOURCE_BIT_05  = 1 <<  8,
    EX_COMMON_RESOURCE_BIT_06  = 1 <<  9,
    EX_COMMON_RESOURCE_BIT_07  = 1 << 10,
  };

  //-------------------------------------
  ///	アニメーションリソースのタイプ
  //=====================================
  enum AnimationResourceType  // @note gfl::grp::g3d::AnimationTypeだけにしたいので、いずれ消す。
  {
    ANIMATION_RESOURCE_TYPE_NONE        = ANIMATION_TYPE_NONE,        // 何も設定されていないときのタイプ
    ANIMATION_RESOURCE_TYPE_SKELETAL    = ANIMATION_TYPE_SKELETAL,
    ANIMATION_RESOURCE_TYPE_MATERIAL    = ANIMATION_TYPE_MATERIAL,
    ANIMATION_RESOURCE_TYPE_VISIBILITY  = ANIMATION_TYPE_VISIBILITY,
    ANIMATION_RESOURCE_TYPE_CAMERA      = ANIMATION_TYPE_CAMERA,
    ANIMATION_RESOURCE_TYPE_LIGHT       = ANIMATION_TYPE_LIGHT,
    ANIMATION_RESOURCE_TYPE_ALL         = ANIMATION_TYPE_ALL         // アニメーションを解除するときのみ渡せるタイプ
  };

  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	リソースの設定内容
  //=====================================
  struct Description
  {
    u32 texture_location_flag;        // テクスチャのメモリ配置場所
    u32 index_stream_location_flag;   // 頂点インデックスストリームのメモリ配置場所
    u32 vertex_stream_location_flag;  // 頂点ストリームのメモリ配置場所

    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : texture_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        index_stream_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        vertex_stream_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP)
      //: texture_location_flag(gfl::grp::MEMORY_AREA_VRAMA | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
      //  index_stream_location_flag(gfl::grp::MEMORY_AREA_VRAMB | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
      //  vertex_stream_location_flag(gfl::grp::MEMORY_AREA_VRAMB | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP)
    {}
  };

  //-------------------------------------
  ///	アニメーションリソースの情報パック
  //=====================================
  struct AnimationResourcePack
  {
    Resource*             resource;
    AnimationResourceType animation_resource_type;
    s32                   index_in_resource;
  };

  //-------------------------------------
  ///	テクスチャリソースの情報パック
  //=====================================
  struct TextureResourcePack
  {
    Resource*             resource;
    s32                   index_in_resource;
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         メンバーアニメーションの要素数で最大の値を得る
   *
   *  @param[in]     array_element_num              animation_resource_pack_array配列の要素数 
   *  @param[in]     animation_resource_pack_array  AnimationResourcePackの配列の先頭へのポインタ
   * 
   *  @retval        s32  animation_resource_pack_arrayのメンバーアニメーションの要素数で最大の値
   *
   *  使用例
   *  gfl::grp::g3d::Resource::AnimationResourcePack  animation_resource_pack_array[]  =
   *  {
   *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
   *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
   *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
   *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
   *  };
   *  s32 max_member_anim_set_count = gfl::grp::g3d::Resource::GetMaxMemberAnimSetCount(
   *      sizeof(animation_resource_pack_array)/sizeof(animation_resource_pack_array[0]),
   *      animation_resource_pack_array
   *  );
   */
  //-----------------------------------------------------------------------------
  static s32 GetMaxMemberAnimSetCount(
      const u32                    array_element_num,
      const AnimationResourcePack* animation_resource_pack_array
  );


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     path         グラフィックスリソースファイルのパス名
   *                              (allocatorとpathの両方がNULLでないときはロードする。
   *                               GetBufferの戻り値がNULLのときはロード失敗)
   *  @param[in]     description  リソースの設定内容
   *                              (ロードしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
   *                              (ロードしたときのみ使用する。
   *                               セットアップできているか否かはIsSetupで確認)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   */
  //-----------------------------------------------------------------------------
  Resource(gfl::heap::NwAllocator* allocator = NULL, const char* path = NULL, const Description* description = NULL, bool do_setup = true, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 グラフィックスリソースファイルを読み込んだバッファのアタッチ
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
   *                              (GetBufferの戻り値がNULLのときはバッファのアタッチ失敗)
   *                              @note constが付いていますがNintendoWareでconst_castされ書き換えられます。
   *  @param[in]     description  リソースの設定内容
   *                              (バッファをアタッチしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
   *                              (バッファをアタッチしたときのみ使用する。
   *                               セットアップできているか否かはIsSetupで確認)
   *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
   *                              falseのときbufferの管理は呼び出し元で行って下さい。
   *                              (バッファをアタッチしたときのみ使用する)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   */
  //-----------------------------------------------------------------------------
  Resource(
      gfl::heap::NwAllocator* allocator,
      const void* buffer,
      const Description* description = NULL,
      bool do_setup = true,
      bool own_buffer = false,
      u32 common_resource_flag = COMMON_RESOURCE_NONE );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 アーカイブファイルからデータを読み込む
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     arc_file     アーカイブファイル
   *                              (GetBufferの戻り値がNULLのときはロード失敗)
   *  @param[in]     arc_data_id  アーカイブファイル内のデータID
   *  @param[in]     description  リソースの設定内容
   *                              (読み込んだときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     do_setup     ファイル中のコンテントのセットアップを行う場合はtrue
   *                              (読み込んだときのみ使用する。
   *                               セットアップできているか否かはIsSetupで確認)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   *  @param[in]     is_compress  圧縮データの場合はtrue
   */
  //-----------------------------------------------------------------------------
  Resource(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile*       arc_file,
      u32                     arc_data_id,
      const Description* description = NULL,
      bool do_setup = true,
      u32 common_resource_flag = COMMON_RESOURCE_NONE,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~Resource(); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルのロード
   *                 ロードだけでファイル中のコンテントのセットアップはしない。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     path         グラフィックスリソースファイルのパス名
   *  @param[in]     description  リソースの設定内容
   *                              (ロードしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *
   *  @retval        bool    falseのときはロード失敗。
   *                         (失敗の理由は
   *                          allocatorが不正か、pathが不正か、既にロードした他のデータが存在するか、
   *                          メモリ不足のどれか)
   */
  //-----------------------------------------------------------------------------
  bool Load(gfl::heap::NwAllocator* allocator, const char* path, const Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アーカイブファイルからデータを読み込む
   *                 読み込むだけでファイル中のコンテントのセットアップはしない。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     arc_file     アーカイブファイル
   *                              (GetBufferの戻り値がNULLのときはロード失敗)
   *  @param[in]     arc_data_id  アーカイブファイル内のデータID
   *  @param[in]     description  リソースの設定内容
   *                              (読み込んだときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     is_compress  圧縮データの場合はtrue
   * 
   *  @retval        bool    falseのときはロード失敗。
   *                         (失敗の理由は
   *                          allocatorが不正か、pathが不正か、既にロードした他のデータが存在するか、
   *                          メモリ不足のどれか)
   */
  //-----------------------------------------------------------------------------
  bool Load(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile* arc_file,
      u32 arc_data_id,
      const Description* description = NULL,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルのアンロード
   *
   *  @retval        bool    falseのときはアンロード失敗。
   *                         (失敗の理由はロードしたデータが存在しない)
   */
  //-----------------------------------------------------------------------------
  bool Unload(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルを読み込んだバッファのアタッチ
   *                 バッファのアタッチだけでファイル中のコンテントのセットアップはしない。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
   *                              @note constが付いていますがNintendoWareでconst_castされ書き換えられます。
   *  @param[in]     description  リソースの設定内容
   *                              (バッファをアタッチしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
   *                              falseのときbufferの管理は呼び出し元で行って下さい。
   *                              (バッファをアタッチしたときのみ使用する)
   *
   *  @retval        bool    falseのときはバッファのアタッチ失敗。
   *                         (失敗の理由は
   *                          allocatorが不正か、bufferが不正か、既にロードした他のデータが存在するか、
   *                          メモリ不足のどれか)
   */
  //-----------------------------------------------------------------------------
  bool AttachBuffer(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description = NULL, bool own_buffer = false);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルを読み込んだバッファのデタッチ
   *
   *  @retval        bool    falseのときはバッファのデタッチ失敗。
   *                         (失敗の理由はバッファをアタッチしたデータが存在しない)
   */
  //-----------------------------------------------------------------------------
  bool DetachBuffer(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ファイル中のコンテントのセットアップ
   *                 セットアップが完了するまで、other_resourceを変更して何度呼び出してもよい。
   *
   *  @param[in]     allocator         アロケータ
   *  @param[in]     other_resource    セットアップに必要な他のリソース
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   *
   *  @retval        bool   trueのときはセットアップ完了。
   *                        falseのときはセットアップがまだ完了していない。その理由はGetResultで確認。
   */
  //-----------------------------------------------------------------------------
  bool Setup(gfl::heap::NwAllocator* allocator, Resource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルのロード
   *                 ファイル中のコンテントのセットアップ
   *                 Loadの後にSetupを行う。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     path         グラフィックスリソースファイルのパス名
   *  @param[in]     description  リソースの設定内容
   *                              (ロードしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   *
   *  @retval        bool   trueのときはロードに成功し、セットアップが完了している。
   *                        falseのときはロードに失敗したかセットアップが完了していない。
   *                        GetBufferの戻り値がNULLのときはロード失敗、NULLでないときはセットアップが完了していない。
   */
  //-----------------------------------------------------------------------------
  bool LoadAndSetup(gfl::heap::NwAllocator* allocator, const char* path, const Description* description = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アーカイブファイルからデータを読み込む
   *                 ファイル中のコンテントのセットアップ
   *                 Loadの後にSetupを行う。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     arc_file     アーカイブファイル
   *                              (GetBufferの戻り値がNULLのときはロード失敗)
   *  @param[in]     arc_data_id  アーカイブファイル内のデータID
   *  @param[in]     description  リソースの設定内容
   *                              (読み込んだときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   *  @param[in]     is_compress  圧縮データの場合はtrue
   *
   *  @retval        bool   trueのときはロードに成功し、セットアップが完了している。
   *                        falseのときはロードに失敗したかセットアップが完了していない。
   *                        GetBufferの戻り値がNULLのときはロード失敗、NULLでないときはセットアップが完了していない。
   */
  //-----------------------------------------------------------------------------
  bool LoadAndSetup(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile* arc_file,
      u32 arc_data_id,
      const Description* description = NULL,
      u32 common_resource_flag = COMMON_RESOURCE_NONE,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルのアタッチ
   *                 ファイル中のコンテントのセットアップ
   *                 Attachの後にSetupを行う。
   *
   *  @param[in]     allocator    アロケータ
   *  @param[in]     buffer       グラフィックスリソースを読み込んだバッファのアドレス
   *                              @note constが付いていますがNintendoWareでconst_castされ書き換えられます。
   *  @param[in]     description  リソースの設定内容
   *                              (バッファをアタッチしたときのみ使用する。
   *                               NULLのときはデフォルト値。
   *                               呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
   *  @param[in]     own_buffer   tureのときbufferの管理はこのインスタンスが行うので呼び出し元はbufferを解放してはならない。
   *                              falseのときbufferの管理は呼び出し元で行って下さい。
   *                              (バッファをアタッチしたときのみ使用する)
   *  @param[in]     common_resource_flag  使用する共通リソースがある場合はビットを立てておく
   *
   *  @retval        bool   trueのときはバッファのアタッチに成功し、セットアップが完了している。
   *                        falseのときはバッファのアタッチに失敗したかセットアップが完了していない。
   *                        GetBufferの戻り値がNULLのときはバッファのアタッチ失敗、NULLでないときはセットアップが完了していない。
   */
  //-----------------------------------------------------------------------------
  bool AttachBufferAndSetup(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description = NULL, bool own_buffer = false, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         セットアップが完了しているか
   *
   *  @retval        bool   trueのときはセットアップ完了。
   *                        falseのときはセットアップがまだ完了していない。その理由はGetResultで確認。
   */
  //-----------------------------------------------------------------------------
  bool IsSetup(void) const {  return m_is_setup;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         直前のセットアップの結果を得る
   *
   *  @retval        int    直前のセットアップの結果(nw::gfx::Result::GetDescription)
   */
  //-----------------------------------------------------------------------------
  int  GetResult(void) const  {  return m_nw_result.GetDescription();  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソース内の全ての参照オブジェクトを巡回して、未解決の参照オブジェクトの名前を出力する
   *                 GFL_DEBUGが0のときは何もしません。
   */
  //-----------------------------------------------------------------------------
  void PrintUnresolvedReference(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         このリソースのメモリサイズを、ヘッダー部だけのメモリサイズにする
   *
   *                 実データ部をVRAMに配置しているのでデバイスメモリにはヘッダー部だけ残しておけばいい、
   *                 だから実データ部をデバイスメモリから削除しよう、というときに使用して下さい。
   *
   *                 Setupが完了した後に呼んで下さい。
   *                 このリソースに含まれる全てのデータをVRAMに配置している場合のみ、使用可能です。
   *                 1つでもFCRAMに配置している場合は使用しないで下さい。
   *
   *                 読み込みをこのリソースクラスで行っているとき(Load系の関数を利用したとき)は、
   *                 読み込みに使用したアロケータを渡して下さい。
   *                 そのアロケータから確保しているバッファのサイズをヘッダー部のサイズにまで縮小します。
   *
   *                 自分でバッファを確保しこのリソースクラスにアタッチしたとき(AttachBuffer系の関数を利用したとき)は、
   *                 そのバッファを確保したアロケータを渡して下さい。
   *                 そのアロケータから確保しているバッファのサイズをヘッダー部のサイズにまで縮小します。
   *
   *  @param[in]     allocator    アロケータ
   */
  //-----------------------------------------------------------------------------
  void DeleteImageBlockData( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ロードしたバッファへのポインタを得る
   *
   *  @retval        cosnt void*  ロードしたバッファへのポインタ(NULLのときはロードしていない)
   *                              @note constが付いていますがNintendoWareでconst_castされ書き換えられます。
   */
  //-----------------------------------------------------------------------------
  const void* GetBuffer(void) const {  return m_buffer;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         モデルリソースの要素数を取得する
   * 
   *  @retval        s32    モデルリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetModelsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         カメラリソースの要素数を取得する
   * 
   *  @retval        s32    カメラリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetCamerasCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ライトリソースの要素数を取得する
   * 
   *  @retval        s32    ライトリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetLightsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フォグリソースの要素数を取得する
   * 
   *  @retval        s32    フォグリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetFogsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン環境設定リソースの要素数を取得する
   * 
   *  @retval        s32    シーン環境設定リソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetSceneEnvironmentSettingsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         スケルタルアニメーションリソースの要素数を取得する
   *
   *  @retval        s32    スケルタルアニメーションリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetSkeletalAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         マテリアルアニメーションリソースの要素数を取得する
   *
   *  @retval        s32    マテリアルアニメーションリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetMaterialAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ビジビリティアニメーションリソースの要素数を取得する
   *
   *  @retval        s32    ビジビリティアニメーションリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetVisibilityAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         カメラアニメーションリソースの要素数を取得する
   *
   *  @retval        s32    カメラアニメーションリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetCameraAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ライトアニメーションリソースの要素数を取得する
   *
   *  @retval        s32    ライトアニメーションリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetLightAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャリソースの要素数を取得する
   * 
   *  @retval        s32    テクスチャリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetTexturesCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         シェーダーリソースの要素数を取得する
   * 
   *  @retval        s32    シェーダーリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetShadersCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         エミッターリソースの要素数を取得する
   * 
   *  @retval        s32    エミッターリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetEmittersCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ルックアップテーブルセットリソースの要素数を取得する
   * 
   *  @retval        s32    ルックアップテーブルセットリソースの要素数
   */
  //-----------------------------------------------------------------------------
  s32                 GetLutSetsCount() const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソースのインデックス番号を取得する
   * 
   *  @param[in]     key  リソースの名前
   * 
   *  @retval        s32    リソースのインデックス番号
   *                        指定した名前を持つリソースが存在しない場合、RESOURCE_INDEX_NOT_FOUNDを返す
   *
   */
  //-----------------------------------------------------------------------------
  s32 GetModelsIndex(const char* key) const;
  s32 GetCamerasIndex(const char* key) const;
  s32 GetLightsIndex(const char* key) const;
  s32 GetFogsIndex(const char* key) const;
  s32 GetSceneEnvironmentSettingsIndex(const char* key) const;
  s32 GetSkeletalAnimsIndex(const char* key) const;
  s32 GetMaterialAnimsIndex(const char* key) const;
  s32 GetVisibilityAnimsIndex(const char* key) const;
  s32 GetCameraAnimsIndex(const char* key) const;
  s32 GetLightAnimsIndex(const char* key) const;
  s32 GetTexturesIndex(const char* key) const;
  s32 GetShadersIndex(const char* key) const;
  s32 GetEmittersIndex(const char* key) const;
  s32 GetLutSetsIndex(const char* key) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         メンバーアニメーションの要素数を取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   * 
   *  @retval        s32    メンバーアニメーションの要素数
   */
  //-----------------------------------------------------------------------------
  s32 GetSkeletalMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetMaterialMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetVisibilityMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetCameraMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetLightMemberAnimSetCount(const s32 index_in_resource) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションリソースを取得する
   * 
   *  @param[in]     type               アニメーションリソースのタイプ
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[out]    res_anim           取得したアニメーションリソースの代入先
   */
  //-----------------------------------------------------------------------------
  void GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const;
  

#if GFL_DEBUG
  //-----------------------------------------------------------------------------
  /**
   *  @brief    アニメーションの内容確認
   */
  //-----------------------------------------------------------------------------
  void DEBUG_PrintAnimResult(const AnimationResourceType type, const s32 index_in_resource);
  s32  DEBUG_GetMemberAnimSetIndex(const AnimationResourceType type, const s32 index_in_resource, const char* key) const;
#endif


  // @note gfl::grp::g3d::Resourceクラスが肥大化するのを防ぐためにも、テクスチャの情報の取得はgfl::grp::g3d::Textureクラスに
  // 渡してから行ったほうがいいかもしれない
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャの高さ(縦の長さ)を取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @retval        s32                テクスチャの高さ(縦の長さ)
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureHeight(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャの幅(横の長さ)を取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @retval        s32                テクスチャの幅(横の長さ)
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureWidth(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャのフォーマットを取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @retval        s32                gfl::grp::g3d::Texture::Format
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureFormat(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャの1ピクセルあたりのビット長を取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @retval        u32                テクスチャの1ピクセルあたりのビット長
   */
  //-----------------------------------------------------------------------------
  u32 GetTextureBitsPerPixel(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャのイメージのアドレスを取得する
   * 
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @retval        void*              テクスチャのイメージのアドレス
   */
  //-----------------------------------------------------------------------------
  void* GetTextureImageAddress(const s32 index_in_resource) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソースの内容を出力する
   */
  //-----------------------------------------------------------------------------
  void Dump(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソースからシーンオブジェクトを生成する
   *
   *  @param[in]     allocator             アロケータ
   *  @param[in]     device_allocator      デバイスメモリのアロケータ
   *                                       (allocatorとdevice_allocatorは同じものでも構わない)
   *  @param[in]     resource              シーンオブジェクトのリソース
   *  @param[in]     is_animation_enabled  アニメーションが有効かどうかを指定
   *  @param[in]     buffer_option         バッファの種類
   *  @param[in]     max_anim_objects      AnimBindingが持てるAnimGroupごとのAnimObjectの最大数
   * 
   *  @retval        nw::gfx::SceneObject*     生成したシーンオブジェクト
   *                                           (NULLのときは生成失敗。失敗の理由は
   *                                            allocatorが不正か、device_allocatorが不正か、SceneBuilderのCreateObjectで失敗したかのどれか)
   */
  //-----------------------------------------------------------------------------
  static nw::gfx::SceneObject* CreateNwSceneObject(
      gfl::heap::NwAllocator*       allocator,
      gfl::heap::NwAllocator*       device_allocator,
      nw::gfx::ResSceneObject       resource,
      bool                          is_animation_enabled   /*= true*/,
      bit32                         buffer_option          /*= nw::gfx::Model::FLAG_BUFFER_NOT_USE*/,
      s32                           max_anim_objects       /*= 0*/
  );

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックス用のファイルを表すバイナリリソースクラスへのポインタを得る
   *
   *  @retval        nw::gfx::ResGraphicsFile*  グラフィックス用のファイルを表すバイナリリソースクラスへのポインタ(NULLのときはロードしていない)
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ResGraphicsFile* GetNwResGraphicsFile(void) const {  return m_nw_res_graphics_file;  }

private:
  nw::gfx::ResGraphicsFile*  m_nw_res_graphics_file;
  nw::gfx::Result            m_nw_result;  // 直前のnw::gfx::res::ResGraphicsFile::Setupの結果を覚えておく
  bool                       m_is_self_setup;  // 自分のセットアップをしていたらtrue(完全にセットアップが完了しているか否かはm_is_setupで確認して下さい)
  bool                       m_is_setup;   // セットアップが完了していたらtrue
  Description                m_description;
  const void*                m_buffer;  // @note constが付いていますがNintendoWareでconst_castされ書き換えられます。
  bool                       m_own_buffer;  // trueのときbufferの管理はこのインスタンスで行う
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRESOURCE_H__
