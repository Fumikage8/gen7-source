#if PM_DEBUG

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugModelInstanceInfo.h
 *	@brief  モデルインスタンスの情報を取得するクラス
 *	@author	tomoya takahashi
 *	@date		2015.03.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_DEBUG_MODEL_INSTANCE_INFO_H__ )
#define __FIELD_DEBUG_MODEL_INSTANCE_INFO_H__
#pragma once


#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
GFL_NAMESPACE_BEGIN( Test );
GFL_NAMESPACE_BEGIN( FieldTest );


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class DebugModelInstanceInfo
{

public:
  DebugModelInstanceInfo( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* p_node ) : m_pModelInstance(p_node) {}
  ~DebugModelInstanceInfo(){}

  // ポリゴン数の取得
  inline u32 GetPolygonNum()
  {
    u32 poly_num = 0;
    u32 mesh_num = m_pModelInstance->GetMehsNum();
    for( int i=0; i<mesh_num; ++i )
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* mesh;
      mesh = m_pModelInstance->GetMeshLinkData(i);

      u32 face_num = mesh->GetFaceNum();
      for( int j=0; j<face_num; ++j )
      {
        const gfl2::renderingengine::scenegraph::resource::MeshResourceNode::FaceData* face;
        face = mesh->GetFaceData(j);

        poly_num += face->m_IndexNum / 3;
      }
    }

    return poly_num;
  }

  // メッシュ数の取得
  // (MeshResourceNode的に言うとFaceの数）
  inline u32 GetMeshNum()
  {
    u32 face_total_num = 0;
    u32 mesh_num = m_pModelInstance->GetMehsNum();
    for( int i=0; i<mesh_num; ++i )
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode::MeshLinkData* mesh;
      mesh = m_pModelInstance->GetMeshLinkData(i);

      face_total_num += mesh->GetFaceNum();
    }
    return face_total_num;
  }

  inline u32 GetMaterialNum()
  {
    return m_pModelInstance->GetMaterialNum();
  }

  // ボーン数の取得
  inline u32 GetBoneNum()
  {
    u32 count = 0;
    for( u32 i = 0; i < m_pModelInstance->GetJointNum(); i++ )
    {
      if( m_pModelInstance->GetJointInstanceNode(i)->IsNeedRendering() )
      {
        count++;
      }
    }

    return count;
  }

  // ジョイント数の取得
  inline u32 GetJointNum()
  {
    return m_pModelInstance->GetJointNum();
  }

  // テクスチャ数の取得
  inline u32 GetTextureNum()
  {
    u32 size;
    GetTextureInfoList( &size );
    return size;
  }

  // それぞれのテクスチャの情報を取得
  inline const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureInfo( int index )
  {
    u32 size;
    const gfl2::renderingengine::scenegraph::resource::TextureResourceNode**  infoArray = GetTextureInfoList( &size );

    if( index < size ){
      return infoArray[index];
    }
    GFL_ASSERT( 0 );
    return NULL;
  }

  // モデル情報の出力
  inline void Dump( const c8* name )
  {
#if 0
    GFL_RELEASE_PRINT( "==========================================\n" );
    GFL_RELEASE_PRINT( "ModelInstance %s\n", name );
    GFL_RELEASE_PRINT( "\tPolygonNum  %d\n",  GetPolygonNum() );
    GFL_RELEASE_PRINT( "\tMeshNum     %d\n",  GetMeshNum() );
    GFL_RELEASE_PRINT( "\tBoneNum     %d\n",  GetBoneNum() );
    u32 texture_num = GetTextureNum();
    GFL_RELEASE_PRINT( "\tTextureNum  %d\n", texture_num );
    for( int i=0; i<texture_num; ++i )
    {
      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* info = GetTextureInfo(i);
      gfl2::gfx::Texture::TextureDesc desc = info->GetTexture()->GetTextureDesc();
      GFL_RELEASE_PRINT( "\tTexture  %s\n",  info->GetName() );
      GFL_RELEASE_PRINT( "\t\tformat      %d(%s)\n", desc.format, GetFormatName(desc.format) );
      GFL_RELEASE_PRINT( "\t\twidth       %d\n",  desc.width );
      GFL_RELEASE_PRINT( "\t\theight      %d\n",  desc.height );
      GFL_RELEASE_PRINT( "\t\tmipLevels   %d\n",  desc.mipLevels );
    }
    GFL_RELEASE_PRINT( "==========================================\n" );
#endif
    u32 texture_num = GetTextureNum();
    
    GFL_RELEASE_PRINT( "%s,%d,%d,%d,%d,%d,%d", name, GetPolygonNum(), GetMaterialNum(), GetMeshNum(), GetJointNum(), GetBoneNum(), texture_num );
    for( int i=0; i<texture_num; ++i )
    {
      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* info = GetTextureInfo(i);
      gfl2::gfx::Texture::TextureDesc desc = info->GetTexture()->GetTextureDesc();
      GFL_RELEASE_PRINT( ",%s,",  info->GetName() );
      GFL_RELEASE_PRINT( "%s,", GetFormatName(desc.format) );
      GFL_RELEASE_PRINT( "%d,",  desc.width );
      GFL_RELEASE_PRINT( "%d,",  desc.height );
      GFL_RELEASE_PRINT( "%d",  desc.mipLevels );
    }
    GFL_RELEASE_PRINT( "\n");
  }

private:
  // テクスチャーフォーマット名を取得
  const c8 * GetFormatName( int format )
  {
    static const c8* sc_Name[] = 
    {
      "None",
      //  surface		 [surface]	   [texture]
      //! 各色に 5 ビットを使用する 16 ビットのピクセル形式。
      "X1R5G5B5",	   //  X1R5G5B5		X1R5G5B5
      //! R 5ビット、G 6ビット、B 5ビットをのピクセル形式。
      "R5G6B5",		 //  R5G6B5		  R5G6B5
      //! 各色に 8 ビットを使用する 32 ビットの RGB ピクセル形式。
      "X8R8G8B8",	   //  X8R8G8B8		X8R8G8B8
      //! アルファ付きの、チャネルごとに 8 ビットを使用する 32 ビットの ARGB ピクセル形式。 
      "A8R8G8B8",	   //  A8R8G8B8		A8R8G8B8		(PS3:displayable)
      //! ８ビットのBのみ。
      "B8",			 //  B8			  B8
      //! ８ビットのGとBのみ。
      "G8B8",		   //  G8B8			G8B8
      //! アルファ付きの、チャネルごとに 16 ビットを使用する 64　ビットのHalfFloatのARGBピクセル形式
      "A16B16G16R16F",  //  A16B16G16R16F   A16B16G16R16F   (PS3:displayable)
      //! アルファ付きの、チャネルごとに 32 ビットを使用する 128　ビットのFloatのARGBピクセル形式
      "A32B32G32R32F",  //  A32B32G32R32F   A32B32G32R32F
      //! 32ビットのFloat
      "R32F",		   //  R32F			R32F
      //! 各色に 8 ビットを使用する 32 ビットの BGR ピクセル形式。
      "X8B8G8R8",	   //  X8B8G8R8		(X8R8G8B8 + PS3:remap)
      //! アルファ付きの各色 8 ビットを使用する 32 ビットの ABGR ピクセル形式。
      "A8B8G8R8",	   //  A8B8G8R8		(A8R8G8B8 + PS3:remap)

      //  depth stencil
      //! 16 ビットの Z バッファ
      "D16",			//  D16			 D16
      //! 16 ビットFloatの Z バッファ
      "D16F",		   //  D16F			D16F
      //! 深度チャネルに 24 ビットを使用する 32 ビットの z バッファ。
      "D24X8",		  //  D24S8		   D24S8		   (PS3:Ｚステンシルバッファが３２ビットの場合ステンシルバッファを切れない)
      //! 深度チャネルに 24 ビット、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
      "D24S8",		  //  D24S8		   D24S8
      //! 深度チャネルに 24 ビットFloat、ステンシル チャネルに 8 ビットを使用する 32 ビットの Z バッファ。 
      "D24S8F",		 //  D24S8F		  D24S8F

      //  texture
      //! R 6ビット、G 5ビット、B 5ビットをのピクセル形式。
      "R6G5B5",		 //  (none)		  R6G5B5
      //! G, Rに16ビットFloatを使用する32ビットのピクセル形式。
      "G16R16F",		//  (none)		  G16R16F
      //! G, Rに16ビットを使用する32ビットのピクセル形式。
      "G16R16",		 //  (none)		  G16R16
      //! Rに16ビットを使用するピクセル形式。
      "R16",			//  (none)		  R16
      //! Aに1ビット、RGBに５ビットを使用する32ビットのピクセル形式。
      "A1R5G5B5",	   //  (none)		  A1R5G5B5
      //! アルファ付きの各色 4 ビットを使用する 16 ビットの ARGB ピクセル形式。
      "A4R4G4B4",	   //  (none)		  A4R4G4B4
      //! Aに1ビット、RGBに５ビットを使用する16ビットのRGBAピクセル形式。
      "R5G5B5A1",	   //  (none)		  R5G5B5A1
      //! DXT1圧縮フォーマット
      "DXT1",		   //  (none)		  DXT1
      //! DXT3圧縮フォーマット
      "DXT3",		   //  (none)		  DXT3
      //! DXT5圧縮フォーマット
      "DXT5",		   //  (none)		  DXT5

      //! V8U8フォーマット(バンプマップに使用)
      "V8U8",		   //  (none)		  V8U8

      "X2R10G10B10",
      "A2R10G10B10",
      "X8R8G8B8_LE",
      "A8R8G8B8_LE",
      "X2R10G10B10_LE",
      "A2R10G10B10_LE",

      // CTR用に追加
      "R8G8B8",
      "L8A8",
      "H8L8",
      "L8",
      "A8",
      "L4A4",
      "L4",
      "A4",
      "ETC1",
      "ETC1_A4",
      "SHADOW",
      "GAS",
      "REF",
    };

    if( format < GFL_NELEMS(sc_Name) ){
      return sc_Name[format];
    }
    return sc_Name[0];
  }

  // 重複なしテクスチャノードリストを取得
  const gfl2::renderingengine::scenegraph::resource::TextureResourceNode** GetTextureInfoList( u32 * p_size )
  {
    const int TEXTURE_MAX = 32;
    static const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texhash_list[ TEXTURE_MAX ] = {NULL,};
    int tex_num = 0;
    u32 material_num = m_pModelInstance->GetMaterialNum();

    for( int i=0; i<material_num; ++i )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* material;
      material = m_pModelInstance->GetMaterialInstanceNode(i);

      int texture_num = material->GetTextureNum();

      for( int j=0; j<texture_num; ++j )
      {

        const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texinfo;
        texinfo = material->GetTextureResourceNode(j);
      

        // 重複チェック
        bool is_exist = false;
        for( int k=0; k<tex_num; ++k )
        {
          if( texhash_list[k] == texinfo )
          {
            is_exist = true;
            break;
          }
        }
        // 重複が無ければカウント
        if(!is_exist) 
        {
          texhash_list[tex_num] = texinfo;
          tex_num ++;
        }
      }
    }

    *p_size = tex_num;

    return texhash_list;
  }


  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstance;
};

/**
 *  @class モデルインスタンスの情報を表示するビジター
 */
class DebugModelInstanceInfoVisitor
{
public:
  DebugModelInstanceInfoVisitor(){}
  virtual ~DebugModelInstanceInfoVisitor(){}

  void Visit( gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pNode )
  {
    DebugModelInstanceInfo info(pNode);
    info.Dump( pNode->GetNodeName() );
  }

};

// シーンをトラバースして、情報を表示する。
static void DebugTraverseModelInstanceInfoDraw(void)
{
  gfl2::renderingengine::scenegraph::Traverser::Visitor<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode, Test::FieldTest::DebugModelInstanceInfoVisitor> visitor;
  gfl2::renderingengine::scenegraph::SceneGraphManager::Traverse<gfl2::renderingengine::scenegraph::instance::ModelInstanceNode, Test::FieldTest::DebugModelInstanceInfoVisitor>( &visitor );
}

GFL_NAMESPACE_END( FieldTest );
GFL_NAMESPACE_END( Test );


#endif	// __FIELD_DEBUG_MODEL_INSTANCE_INFO_H__

#endif