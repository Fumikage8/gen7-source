#ifndef __GFL_G3DH3DRESOURCE_H__
#define __GFL_G3DH3DRESOURCE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResource.h
 *	@brief  H3Dリソース
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*@note 注意事項
一度Setupしたバッファは中身が書き換わっていると思う。
デタッチ後バッファを再利用したい場合は注意すること。
バッファが書き換わっていることを確認しているわけではないが。



説明
このH3dResourceでは
nw::h3d::res::ResourceBinary
を扱います。

使い方
(1)   AttachBuffer
(2)   Map
(3)   Setup(Mapを呼んだ場合はメモリを使わない)
(3.5) VRAMへのコピーコマンドの完了待ち
(4)   DeleteSomeSection


(1)   AttachBuffer
(2)   Setup(Mapを呼んでいない場合はメモリを使う)
(3)   DeleteSomeSection


(1)   AttachBufferAndSetup(Mapを呼んでいない場合はメモリを使う)
(2)   DeleteSomeSection
*/

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>
#include <bugfix_gflib.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dリソースクラス
//=====================================
class H3dResource
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dResource);


  friend class H3dModel;
  friend class H3dResSkeletalAnim;
  friend class H3dResMaterialAnim;
  friend class H3dResVisibilityAnim;
  friend class H3dResTexture;
  friend class H3dDynamicTexture;
  friend class H3dResShader;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 定数
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  enum
  {
    INDEX_NONE    = -1  // インデックスが見付からないとき
  };

  //-------------------------------------
  ///	共通リソースのビット
  //=====================================
  enum
  {
    COMMON_RESOURCE_NONE                     = 0,
    COMMON_RESOURCE_H3D_DEFAULT_SHADER_BIT   = 1 << 0
  };

  //-------------------------------------
  ///	Setupの結果  ビットが1になっているところに問題あり
  //=====================================
  enum Result
  {
    RESULT_OK                             = 0,

    RESULT_BUFFER_TOO_SHORT               = 1 << 0,             // バッファが小さすぎます。 

    RESULT_RESOURCE_NOT_FOUND_TEXTURE     = 1 << 3,  
    RESULT_RESOURCE_NOT_FOUND_SHADER      = 1 << 4,  
    RESULT_RESOURCE_NOT_FOUND_LUT         = 1 << 5,

    //RESULT_ANIM_SOME_ELEMENT_NOT_BOUND    = 1 << 14,  // いくつかのアニメーション要素がバインドできませんでした。 
    //RESULT_ANIM_ANY_ELEMENT_NOT_BOUND     = 1 << 15,   // アニメーション要素が 1 つもバインドできませんでした。 
    
    //RESULT_ANIM_SOME_TEXTURE_NOT_BOUND    = 1 << 14,  // いくつかのテクスチャがバインドできませんでした。 
    //RESULT_ANIM_ANY_TEXTURE_NOT_BOUND     = 1 << 15
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 構造体
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  メモリ配置
  struct MemoryLocation
  {
    // MemoryAreaについて
    // gfl::grp::MEMORY_AREA_FCRAMは何もしない、どこに置いてあっても何もしない。
    // gfl::grp::MEMORY_AREA_VRAMA, gfl::grp::MEMORY_AREA_VRAMBはVRAMにコピーする、VRAMにおいてあってもコピーする。
    MemoryArea command_section;  // コマンドセクションの配置場所
    MemoryArea raw_section;      // RAWセクション(テクスチャ、頂点インデックスストリーム、頂点ストリーム)の配置場所
    
    MemoryLocation(void)
      : command_section(gfl::grp::MEMORY_AREA_FCRAM),
        raw_section(gfl::grp::MEMORY_AREA_FCRAM)
    {
      STATIC_ASSERT(gfl::grp::MEMORY_AREA_VRAMA==NN_GX_MEM_VRAMA);  // この値をこのまま使っている箇所があるので、
      STATIC_ASSERT(gfl::grp::MEMORY_AREA_VRAMB==NN_GX_MEM_VRAMB);  // コンパイル時に変わっていないかチェックする。
    }
    b32 IsCommandSectionVram(void) const
    {
      return (    command_section == gfl::grp::MEMORY_AREA_VRAMA
               || command_section == gfl::grp::MEMORY_AREA_VRAMB );
    }
    b32 IsRawSectionVram(void) const
    {
      return (    raw_section == gfl::grp::MEMORY_AREA_VRAMA
               || raw_section == gfl::grp::MEMORY_AREA_VRAMB );
    }
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dResource(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dResource();


  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ロード
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         グラフィックスリソースファイルのロード(同期読み込み)
   *
   *                 この関数が終了したら
   *                   AttachBuffer
   *                   SetOwnBufferFlag(true)
   *                 しているのと同じ状態になります。
   *                 SetOwnBufferFlag(true)と同じ状態なので、確保したバッファはデストラクタで破棄されます。
   *
   *  @param[in]  device_memory     最終的に残るバッファを確保するデバイスメモリ
   *  @param[in]  heap_memory_temp  解凍などに使用する一時的なメモリ
   *  @param[in]  path              パス付きファイル名(例"rom:/field/FieldChar.bcsdr")
   *  @param[in]  arc_file          アーカイブファイル
   *  @param[in]  arc_id            アークID
   *  @param[in]  arc_data_id       アーカイブファイル内のデータID
   *  @param[in]  is_compress       圧縮データの場合はtrue
   *
   *  @retval  正常にロードできたらtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, const char* path, b32 is_compress = false);
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, gfl::fs::ArcFile* arc_file, u32 arc_data_id, b32 is_compress = false);
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, u32 arc_id, u32 arc_data_id, b32 is_compress = false);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // アタッチ / デタッチ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         バッファのアタッチ
   *
   *  @param[in]     buff  デバイスメモリか...に配置すること。  @note VRAMでもOKだろうが試していない(MapでVRAMにコピーすることを言っているのではなく
   *                                                                  最初からVRAMに置いている場合のことをこの@noteでは言っています。)
   */
  //-----------------------------------------------------------------------------
  b32 AttachBuffer(void* buff);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         バッファのデアタッチ
   */
  //-----------------------------------------------------------------------------
  void DetachBuffer( b32 fUnInit = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アタッチしたバッファのポインタを得る
   *
   *  @retval        バッファがアタッチされていないときはNULLを返す。
   *                 【注意】Setupが完全に完了しているか否かを返すわけではない。
   */
  //-----------------------------------------------------------------------------
  const void* GetBuffer(void) const {  return m_buff;  }


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アタッチしたバッファの管理をこのインスタンスで行うか否かのフラグを得る
   *
   *  @retval        trueのときアタッチしているバッファの管理をこのインスタンスで行う。
   */
  //-----------------------------------------------------------------------------
  b32 GetOwnBufferFlag(void) const
  { 
    return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_OWN_BUFF) != 0 );
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アタッチしたバッファの管理をこのインスタンスで行うか否かのフラグを設定する
   *
   *  @param[in]     flag    trueのときアタッチしているバッファの管理をこのインスタンスで行う。
   *                         管理を行う場合はDetachBuffer(デストラクタからも呼ばれる)時にバッファをGflHeapFreeMemoryで解放する。
   */
  //-----------------------------------------------------------------------------
  void SetOwnBufferFlag(b32 flag)
  {
    SetBitFlag(&m_bit_flag, BIT_FLAG_OWN_BUFF, flag);
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // アドレスマップ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソースのアドレス割り当て
   *                 Setupした後ではリソースのアドレス割り当てはできないので、Setup前に呼んで下さい。
   *
   *  @param[in]     mem_loc      メモリ配置
   *                              VRAMへのコピーが指定されていた場合は、VRAMへのコピーコマンドをカレントコマンドリストに積みます。
   *                              そのコマンドが実行されるまでDelete○○Sectionしないようにして下さい。
   *                              DeleteRelocationTableSectionはVRAMへのコピーと無関係なので、Setup後ならいつでも呼び出し可能です。
   *
   *  @param[in]     vram_copy_heap_allocator     VRAMへのコピーが指定されていた場合は、コピーの際に必要な一時的なメモリをここから確保する。
   *                                              NULLが指定されている場合は、heap_allocatorを使う。
   *                                              ここから確保したメモリはDeleteCommandSectionもしくはDeleteSomeSectionを呼んだ際に
   *                                              解放される(呼ばなければ、このリソースを削除するときに解放される)。
   */
  //-----------------------------------------------------------------------------
  void Map(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const MemoryLocation* mem_loc=NULL,
      gfl::heap::NwAllocator* vram_copy_heap_allocator=NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         リロケーションテーブルセクションの分だけメモリを解放する
   *
   *                 Setup完了後ならいつでも呼び出し可能です。
   *
   *  @param[in]     allocator  AttachBufferで渡したbuffを確保しているアロケータ
   */
  //-----------------------------------------------------------------------------
  void DeleteRelocationTableSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         RAWセクションの分だけメモリを解放する
   *
   *                 RAWセクション(テクスチャ、頂点インデックスストリーム、頂点ストリーム)をVRAMにコピーしている場合だけ解放できます。
   *                 Setup完了かつVRAMへのコピーコマンド完了後ならいつでも呼び出し可能です。
   *                 【注意】VRAMへのコピーコマンド完了後かどうかはチェックしていないので呼び出し元で注意して下さい。
   *                
   *                 内部挙動
   *                 (1) DeleteRelocationTableSectionを呼び出します。
   *                 (2) RAWセクションを解放できる場合は解放します。
   *
   *  @param[in]     allocator  AttachBufferで渡したbuffを確保しているアロケータ
   */
  //-----------------------------------------------------------------------------
  void DeleteRawSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コマンドセクションの分だけメモリを解放する
   *
   *                 コマンドセクションとRAWセクションをVRAMにコピーしている場合だけ解放できます。
   *                 Setup完了かつVRAMへのコピーコマンド完了後ならいつでも呼び出し可能です。
   *                 【注意】VRAMへのコピーコマンド完了後かどうかはチェックしていないので呼び出し元で注意して下さい。
   *
   *                 内部挙動
   *                 (1) DeleteRawSectionを呼び出します。（DeleteRawSectionの内部挙動が全て走ります）
   *                 (2) コマンドセクションを解放できる場合は解放します。
   *
   *  @param[in]     allocator  AttachBufferで渡したbuffを確保しているアロケータ
   */
  //-----------------------------------------------------------------------------
  void DeleteCommandSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         解放できるところのメモリを解放する
   *
   *                 コマンドセクションとRAWセクションをVRAMにコピーしている場合だけ解放できます。
   *                 Setup完了かつVRAMへのコピーコマンド完了後ならいつでも呼び出し可能です。
   *                 【注意】VRAMへのコピーコマンド完了後かどうかはチェックしていないので呼び出し元で注意して下さい。
   *
   *                 内部挙動
   *                 (1) DeleteCommandSectionを呼び出します。（DeleteCommandSectionの内部挙動が全て走ります）
   *
   *  @param[in]     allocator  AttachBufferで渡したbuffを確保しているアロケータ
   */
  //-----------------------------------------------------------------------------
  void DeleteSomeSection( gfl::heap::NwAllocator* allocator );


  //----------------------------------------------------------------------------
  /**
   *	@brief  Ｒａｗデータの先頭アドレスを取得
   */
  //-----------------------------------------------------------------------------
  uptr GetSrcRawAddress( void ) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // セットアップ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         セットアップ
   */
  //-----------------------------------------------------------------------------
  b32 Setup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, H3dResource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  // @brief  nameのテクスチャを欲しがっているところにtextureを割り当ててセットアップする
  // @param[in]  name     モデルが欲しがっているテクスチャ名。textureの名前ではない。
  // @param[in]  texture  テクスチャ。名前はnameでなくてもいい。
  //b32 Setup(const char* name, H3dResTexture* texture);
  //b32 Setup(const char* name, H3dDynamicTexture* texture);
 
  // @brief   gfl::grp::g3d::Systemが保持しているテクスチャを使って、まだテクスチャに関してセットアップが完了していないところを完了させる。
  //          使う場合は、必ず通常のSetupより後に使うようにして下さい。
  // @retval  セットアップが完了したらtrueを返す。
  //          テクスチャに関してのセットアップがこれで完了したとしても、他のセットアップがまだ完了していなかったらfalseを返す。
  b32 SetupUsingSystemTexture(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         セットアップが完全に完了していたらtureを返す
   */
  //-----------------------------------------------------------------------------
  b32 IsSetup(void) const { return (m_bit_flag & BIT_FLAG_IS_SETUP); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         直前のセットアップの結果を得る
   *
   *  @retval        bit32   直前のセットアップの結果(Resultのビットで判断して下さい)
   */
  //-----------------------------------------------------------------------------
  bit32 GetResult(void) const { return m_result_desc; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         リソース内の全ての参照オブジェクトを巡回して、未解決の参照オブジェクトの名前を出力する
   *                 GFL_DEBUGが0のときは何もしません。
   */
  //-----------------------------------------------------------------------------
  void PrintUnresolvedReference(void) const;

private:
  // @brief リソース内の全ての参照解決がなされていたらtrueを返す
  b32 isAllReferenceResolved(void) const;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 便利関数  アタッチ & セットアップ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         AttachBufferしてからSetupする
   *                 AttachBufferとSetupのコメントをよくみること
   */
  //-----------------------------------------------------------------------------
  b32 AttachBufferAndSetup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buff, H3dResource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 取得
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  s32 GetModelContentIndex(const char* name) const;
  s32 GetSkeletalAnimContentIndex(const char* name) const;
  s32 GetMaterialAnimContentIndex(const char* name) const;
  s32 GetVisibilityAnimContentIndex(const char* name) const;
  s32 GetTextureContentIndex(const char* name) const;
  
#ifdef BUGFIX_GFBTS1721_20130801
  s32 GetMaterialCount( s32 index_in_resource ) const;
#endif

  // NintendoWare/CTR/documents/API/nw/h3d/res/ShaderContent/Overview.html
  // nw::h3d::res::ShaderContent 構造体
  // 参照用の名前は"[デスクリプションID]@[シェーダ名]"になります。
  // 例: インデックス0の名前 0@DefaultShader
  //     インデックス1の名前 1@DefaultShader
  //     インデックス2の名前 0@UserShader
  //     インデックス3の名前 1@UserShader
  s32 GetShaderContentIndex(const char* name) const;
  const char* GetShaderContentName(s32 index) const;

  u32 GetModelContentCount(void) const;
  u32 GetSkeletalAnimContentCount(void) const;
  u32 GetMaterialAnimContentCount(void) const;
  u32 GetVisibilityAnimContentCount(void) const;
  u32 GetTextureContentCount(void) const;
  u32 GetShaderContentCount(void) const;

  // H3dResAnim::GetAnimElementNumと同じ値が得られる
  // index_in_resource番目のSkeletalAnimContentに含まれているアニメーション要素数を得る
  u32 GetSkeletalAnimElementNum(s32 index_in_resource) const;
  // index_in_resource番目のMaterialAnimContentに含まれているアニメーション要素数を得る
  u32 GetMaterialAnimElementNum(s32 index_in_resource) const;
  // index_in_resource番目のVisibilityAnimContentに含まれているアニメーション要素数を得る
  u32 GetVisibilityAnimElementNum(s32 index_in_resource) const;



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendには公開している非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  nw::h3d::res::ResourceBinary* GetNwResourceBinary(void) const { return m_resbin; }
  
  nw::h3d::res::ModelContent* GetNwModelContent(const char* name) const;
  nw::h3d::res::ModelContent* GetNwModelContent(const s32 index) const;
  nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(const char* name) const;
  nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(const s32 index) const;
  nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(const char* name) const;
  nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(const s32 index) const;
  nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(const char* name) const;
  nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(const s32 index) const;
  nw::h3d::res::TextureContent* GetNwTextureContent(const char* name) const;
  nw::h3d::res::TextureContent* GetNwTextureContent(const s32 index) const;
  nw::h3d::res::ShaderContent* GetNwShaderContent(const char* name) const;
  nw::h3d::res::ShaderContent* GetNwShaderContent(const s32 index) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendにも公開していない非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  void init(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void linkOwn(void);
  void linkOther(H3dResource* other_resource);

private:
  enum
  {
    BIT_FLAG_NONE        =  0,
    BIT_FLAG_IS_INIT     =  1 <<  0,  // セットアップの初期化まで完了していたら1
    BIT_FLAG_IS_LINK_OWN =  1 <<  1,  // セットアップの自己リンクまで完了していたら1(これが1になるときは必ずBIT_FLAG_IS_INITが1になっている)
    BIT_FLAG_IS_SETUP    =  1 <<  2,  // セットアップが完全に完了していたら1(これが1になるときは必ずBIT_FLAG_IS_LINK_OWNが1になっている)
    BIT_FLAG_OWN_BUFF    =  1 <<  3   // m_buffの管理をこのインスタンスで行う場合は1
                                      // 管理を行う場合はDetachBuffer(デストラクタからも呼ばれる)時にバッファをGflHeapFreeMemoryで解放する。
  };

private:
  MemoryLocation                           m_mem_loc;
  nw::h3d::res::ResourceBinary::AddressMap m_address_map;
  u32                                      m_srcRawAddress;
  void*                         m_buff;
  nw::h3d::res::ResourceBinary* m_resbin;
  bit32                         m_bit_flag;

  bit32 m_result_desc;  // Setupの結果を入れておく
};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DRESOURCE_H__

