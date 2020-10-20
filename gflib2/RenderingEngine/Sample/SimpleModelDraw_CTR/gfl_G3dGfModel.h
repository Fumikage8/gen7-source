//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GfModel.h
 *	@brief  シェーダーモデルクラス
 *	@author	Masayuki Onoue
 *	@date		2012.06.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __GfModel_H__ )
#define __GfModel_H__

#include <gfx/include/gfl2_GFGL.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )	\
	class CLASSNAME {											\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )		\
		};                                  \
	};

namespace gfl {
namespace grp {
namespace g3d {

struct ChunkHeader{
  ChunkHeader( u64 id, u32 size ){ m_ID = id;m_Size = size; m_Padding = 0xFFFFFFFF; }
  u64				m_ID;
  u32				m_Size;
  u32				m_Padding;
};

struct FaceHeader
{
  u32       hash;
  c8        name[64];
  u32       padding[3];//96

  u32       vertexSize;
  u32       indexSize;
  u32       vertexBufferSize;
  u32       indexBufferSize;//16
};
  
 //---------------------------------------------------------------------------
// #2 モデル描画を行うクラスの実装 : MyModel
//---------------------------------------------------------------------------
class MyModel
{
public:
    MyModel(){}

    ~MyModel(){}

    gfl2::math::Matrix& GetWorldMtx() { return m_WorldMtx; }
    const gfl2::math::Matrix& GetWorldMtx() const { return m_WorldMtx; }

    void DrawMesh();
    
    void SetTexture( s32 no, void* pData );
    void SetVertexShader( void* pData );
    void SetGfModel( void* pData );
    
    f32 m_AnimFrame;

		gfl2::math::Matrix m_WorldMtx;
    gfl2::math::Matrix m_NormalMtx;

    b32                   m_fH3d;
    void                  *m_StateMemory;
    void                  *m_StateDeviceMemory;
    void                  *m_pTextureData;
    void                  *m_pVertexShaderData;
    void                  *m_pGfModelData;
    
    u32                   m_VertexShaderSize;
    u32                   m_TextureCommandTop[3][6];
    
    u32                   m_MaterialCommandSize;
    u32                   *m_pMaterilCommandTop;
    
    u32                   m_MeshCommandNum;
    u32                   m_MeshCommandSize[16];
    u32                   *m_pMeshCommandTop[16];
    
    FaceHeader            *m_pFaceHeader[16];
    u32                   *m_pVertexBuffer[16];
    u32                   *m_pIndexBuffer[16];
};


class GfModel
{
public:

	GfModel();
	virtual ~GfModel();
  
  void SetTexture( s32 no, void* pData ){ m_pMyModelList->SetTexture( no, pData ); }
  void SetVertexShader( void* pData ){ m_pMyModelList->SetVertexShader( pData ); }
  void SetGfModel( void* pData ){ m_pMyModelList->SetGfModel( pData ); }
  
  void H3dEnable( b32 f ){ m_fH3d = f; m_pMyModelList->m_fH3d = m_fH3d; }
  b32 IsH3dEnable(){ return m_fH3d; }

	void Draw(){ m_pMyModelList->DrawMesh(); }
  
  void                  *m_pData;
  s32                   m_ModelCnt;
  MyModel               *m_pMyModelList;
  b32                   m_fH3d;
};

}}}

#endif		// __GfModel_H__
