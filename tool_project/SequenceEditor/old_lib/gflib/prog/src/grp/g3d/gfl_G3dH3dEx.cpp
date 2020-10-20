//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEx.cpp
 *	@brief  H3D追加
 *	@author	Koji Kawada
 *	@date		2012.08.24
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
#include <bugfix_gflib.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dUtil.h>
#include <grp/g3d/gfl_G3dH3dCommandUtil.h>
#include <grp/g3d/gfl_G3dH3dEx.h>


// テスト用の定義
#define GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE (GFL_DEBUG&&(0))  // 1のとき全マテリアルを変更したのと同じ状態になるようフラグをたてる


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dコマンドリスナー
//=====================================
class H3dCommandListenerReadShader : public H3dCommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandListenerReadShader);
public:
  H3dCommandListenerReadShader(u8 location, gfl::math::VEC4* vec4)
    : m_location(location),
      m_vec4(vec4),
      m_state(STATE_SEARCH_ADDR_LOC)
  {}
  virtual ~H3dCommandListenerReadShader() {}
public:
  virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access)
  {
    // countの0から順番にこの関数が呼ばれるので、このやり方でよい。
    if( m_state == STATE_SEARCH_ADDR_LOC )
    {
      if( addr == PICA_REG_VS_FLOAT_ADDR )  // 連続データのときアドレスは増えていく。最初のアドレスに引っ掛かったらm_stateを変えて以降アドレスは見ない。
      {
        if( count == 0 )
        {
          if( ( 0x80000000 | m_location ) == command )
          {
            m_state = STATE_READ_DATA;
          }
        }
      }
    }
    else
    {
      switch(count)
      {
      case 1:
        m_vec4->w = nn::math::U32AsF32(command);
        break;
      case 2:
        m_vec4->z = nn::math::U32AsF32(command);
        break;
      case 3:
        m_vec4->y = nn::math::U32AsF32(command);
        break;
      case 4:
        m_vec4->x = nn::math::U32AsF32(command);
        break;
      }
    }

    if ( m_state == STATE_READ_DATA )
    {
      if( count == count_max -1 )  // count_max=5じゃないときがあるかもしれないので、m_stateのifelse文の外でチェックしておく。
      {
        m_state = STATE_SEARCH_ADDR_LOC;
        return true;
      }
    }
    return false;
  }

private:
  enum State
  {
    STATE_SEARCH_ADDR_LOC,  // 対象アドレス、ロケーション探し中
    STATE_READ_DATA         // データ読み中
  };

private:
  u8               m_location;
  gfl::math::VEC4* m_vec4;
  b32              m_state;
};




//-------------------------------------
///	H3Dコマンドリスナー
//=====================================
class H3dCommandListenerReadMaterial : public H3dCommandListener
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dCommandListenerReadMaterial);

private:
  // ★★★★この並び順を使ってif分岐しているので、並び順を変更しないように。★★★★
  enum
  {
    //
    ADDR_TEXTURE_COMBINER_COMBINE_0,
    ADDR_TEXTURE_COMBINER_COMBINE_1,
    ADDR_TEXTURE_COMBINER_COMBINE_2,
    ADDR_TEXTURE_COMBINER_COMBINE_3,
    ADDR_TEXTURE_COMBINER_COMBINE_4,
    ADDR_TEXTURE_COMBINER_COMBINE_5,
    //
    ADDR_TEXTURE_COMBINER_SCALE_0,
    ADDR_TEXTURE_COMBINER_SCALE_1,
    ADDR_TEXTURE_COMBINER_SCALE_2,
    ADDR_TEXTURE_COMBINER_SCALE_3,
    ADDR_TEXTURE_COMBINER_SCALE_4,
    ADDR_TEXTURE_COMBINER_SCALE_5,
    //
    ADDR_TEXTURE_COMBINER_OPERAND_0,
    ADDR_TEXTURE_COMBINER_OPERAND_1,
    ADDR_TEXTURE_COMBINER_OPERAND_2,
    ADDR_TEXTURE_COMBINER_OPERAND_3,
    ADDR_TEXTURE_COMBINER_OPERAND_4,
    ADDR_TEXTURE_COMBINER_OPERAND_5,
    //
    ADDR_TEXTURE_COMBINER_SOURCE_0,
    ADDR_TEXTURE_COMBINER_SOURCE_1,
    ADDR_TEXTURE_COMBINER_SOURCE_2,
    ADDR_TEXTURE_COMBINER_SOURCE_3,
    ADDR_TEXTURE_COMBINER_SOURCE_4,
    ADDR_TEXTURE_COMBINER_SOURCE_5,
    //
    ADDR_FRAGMENT_SHADER_BUFFER_INPUT,
    //
    ADDR_STENCIL_OPERATION_TEST, 
    ADDR_STENCIL_OPERATION_OP,
    //
    ADDR_ALPHA_TEST,
    //
    ADDR_DEPTH_TEST,
    //
    ADDR_BLEND_MODE,
    ADDR_BLEND_FUNCTION,
    ADDR_BLEND_LOGIC_OPERATION,
    //
    ADDR_COLOR_BUFFER_READ,
    ADDR_COLOR_BUFFER_WRITE,
    ADDR_DEPTH_BUFFER_READ,
    ADDR_DEPTH_BUFFER_WRITE,
    /*
    //出て来ないからコメントアウト
    ADDR_TEXTURE_SAMPLER_TYPE,
    ADDR_TEXTURE_SAMPLER_WRAP_FILTER_0,
    ADDR_TEXTURE_SAMPLER_WRAP_FILTER_1,
    ADDR_TEXTURE_SAMPLER_WRAP_FILTER_2,
    ADDR_TEXTURE_SAMPLER_LOD_0,
    ADDR_TEXTURE_SAMPLER_LOD_1,
    ADDR_TEXTURE_SAMPLER_LOD_2,
    */
    // 
    ADDR_RASTERIZATION_CULLING_MODE, 
    //
    ADDR_MAX
  };
  static const u32 ADDR_LIST[ADDR_MAX];

private:
  H3dMaterialExState* m_mate_ex_state;

private:
  void setTextureCombinerCombine(s32 step, bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if(mask&0x0000FFFF)
    {
      m_mate_ex_state->InitializeTextureCombinerCombineRgb(step, static_cast<H3dTextureCombinerExConst::Combine>((command&0x0000FFFF)>>0));
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
    if(mask&0xFFFF0000)
    {
      m_mate_ex_state->InitializeTextureCombinerCombineAlpha(step, static_cast<H3dTextureCombinerExConst::Combine>((command&0xFFFF0000)>>16));
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }

  void setTextureCombinerScale(s32 step, bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if(mask&0x0000FFFF)
    {
      m_mate_ex_state->InitializeTextureCombinerScaleRgb(step, static_cast<H3dTextureCombinerExConst::Scale>((command&0x0000FFFF)>>0));
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
    if(mask&0xFFFF0000)
    {
      m_mate_ex_state->InitializeTextureCombinerScaleAlpha(step, static_cast<H3dTextureCombinerExConst::Scale>((command&0xFFFF0000)>>16));
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }

  void setTextureCombinerOperand(s32 step, bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    for(s32 no=0; no<3; ++no)
    {
      if( mask & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*no) ) )
      {
        m_mate_ex_state->InitializeTextureCombinerOperandRgb(step, no,
            static_cast<H3dTextureCombinerExConst::OperandRgb>( ( command & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*no) ) ) >> (PICA_TEX_ENV_OPE_RGB1_SHIFT*no) )
        );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
      s32 ano = no + 3;
      if( mask & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*ano) ) )
      {
        m_mate_ex_state->InitializeTextureCombinerOperandAlpha(step, no,
            static_cast<H3dTextureCombinerExConst::OperandAlpha>( ( command & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*ano) ) ) >> (PICA_TEX_ENV_OPE_RGB1_SHIFT*ano) )
        );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }

  void setTextureCombinerSource(s32 step, bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    for(s32 no=0; no<3; ++no)
    {
      if( mask & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*no) ) )
      {
        m_mate_ex_state->InitializeTextureCombinerSourceRgb(step, no,
            static_cast<H3dTextureCombinerExConst::Source>( ( command & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*no) ) ) >> (PICA_TEX_ENV_SRC_RGB1_SHIFT*no) )
        );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
      s32 ano = no + 4;
      if( mask & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*ano) ) )
      {
        m_mate_ex_state->InitializeTextureCombinerSourceAlpha(step, no,
            static_cast<H3dTextureCombinerExConst::Source>( ( command & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*ano) ) ) >> (PICA_TEX_ENV_SRC_RGB1_SHIFT*ano) )
        );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }

  void setFragmentShaderBufferInput(bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    for(s32 i=0; i<4; ++i)
    {
      if( mask & (1<<(8+i)) )
      {
        m_mate_ex_state->InitializeFragmentShaderBufferInputRgb( i+1, static_cast<H3dFragmentShaderExConst::BufferInput>( ( command & (1<<(8+i)) ) >> (8+i) ) );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
      s32 ai = i+4;
      {
        m_mate_ex_state->InitializeFragmentShaderBufferInputAlpha( i+1, static_cast<H3dFragmentShaderExConst::BufferInput>( ( command & (1<<(8+ai)) ) >> (8+ai) ) );
#if GFL_DEBUG
        debug_flag = true;
#endif
      }
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    // FragmentShaderBufferInputではない他のコマンドのこともあり得るので、マスク値を確認するためのアサートは入れられない。
#endif
  }

  void setStencilOperationTest(bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if( mask & 0xFFFFFFFF )
    {
      m_mate_ex_state->InitializeStencilOperationTest( (command&1), static_cast<H3dStencilOperationExConst::TestFunc>((command&0xF0)>>4), ((command&0xFF00)>>8), ((command&0xFF0000)>>16), ((command&0xFF000000)>>24) );
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }
  
  void setStencilOperationOp(bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if( mask & 0xFFFF )
    {
      m_mate_ex_state->InitializeStencilOperationOp( static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF)>>0), static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF0)>>4), static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF00)>>8) );
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
    GFL_ASSERT(mask==0xFFFFFFFF);  //@check
#endif
  }

  void setAlphaTest(bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if( mask & 0x0000FFFF )
    {
      m_mate_ex_state->InitializeAlphaTest( (command&1), static_cast<H3dAlphaTestExConst::TestFunc>((command&0xF0)>>4), ((command&0xFF00)>>8) );
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
#endif
  }

  void setDepthTest(bit32 command, u32 mask)
  {
    if( mask & 0x0000FFFF )
    {
      m_mate_ex_state->InitializeDepthTest( (command&1), ((command&0x1000)!=0)?1:0, static_cast<H3dDepthTestExConst::TestFunc>((command&0x70)>>4) );
      
      // カラーマスクは全部1想定でつくっているのでチェックしておく。
      GFL_ASSERT(command&0x100);  // red_mask  //@check カラーマスクが1でない場合、絵がおかしくなるだけ。
      GFL_ASSERT(command&0x200);  // green_mask  //@check
      GFL_ASSERT(command&0x400);  // blue_mask  //@check
      GFL_ASSERT(command&0x800);  // alpha_mask  //@check
    }
  }
  
  void setBlendMode(bit32 command, u32 mask)
  {
    if( mask & 0x0000FF00 )
    {
      if((command&0x100))
      {
        // ブレンドが有効
        m_mate_ex_state->InitializeBlendMode(H3dBlendExConst::MODE_SEPARATE_BLEND);
      }
      else
      {
        // 論理演算が有効
        m_mate_ex_state->InitializeBlendMode(H3dBlendExConst::MODE_LOGIC);
      }
    }
  }
  void setBlendFunction(bit32 command, u32 mask)
  {
    if( mask & 0xFFFFFFFF )
    {
      m_mate_ex_state->InitializeBlendFunction(
          static_cast<H3dBlendExConst::Equation>((command&0x7)>>0),
          static_cast<H3dBlendExConst::Factor>((command&0xF0000)>>16),
          static_cast<H3dBlendExConst::Factor>((command&0xF00000)>>20),
          static_cast<H3dBlendExConst::Equation>((command&0x700)>>8),
          static_cast<H3dBlendExConst::Factor>((command&0xF000000)>>24),
          static_cast<H3dBlendExConst::Factor>((command&0xF0000000)>>28)
      );
    }
  }
  void setBlendLogicOperation(bit32 command, u32 mask)
  {
    if( mask & 0x0000000F )
    {
      m_mate_ex_state->InitializeBlendLogicOperation(static_cast<H3dBlendExConst::LogicOperation>(command&0xF));
    }
  }
  
  void setColorBufferRead(bit32 command, u32 mask)
  {
    if( mask )
    {
      b8 fColorRead = ( command & 0xF ) ? true : false;
      
      m_mate_ex_state->InitializeColorBufferRead( fColorRead );
    }
  }
  
  void setColorBufferWrite(bit32 command, u32 mask)
  {
    if( mask )
    {
      b8 fColorWrite = ( command & 0xF ) ? true : false;
      
      m_mate_ex_state->InitializeColorBufferWrite( fColorWrite );
    }
  }
  
  void setDepthBufferRead(bit32 command, u32 mask)
  {
    if( mask )
    {
      b8 fDepthRead = ( command & (1<<1) ) ? true : false;
      b8 fStencilRead = ( command & (1<<0) ) ? true : false;
      
      m_mate_ex_state->InitializeDepthBufferRead( fDepthRead, fStencilRead );
    }
  }
  
  void setDepthBufferWrite(bit32 command, u32 mask)
  {
    if( mask )
    {
      b8 fDepthWrite = ( command & (1<<1) ) ? true : false;
      b8 fStencilWrite = ( command & (1<<0) ) ? true : false;
      
      m_mate_ex_state->InitializeDepthBufferWrite( fDepthWrite, fStencilWrite );
    }
  }

  void setTextureSamplerType(bit32 command, u32 mask)
  {
    if( mask & 0x000000FF )
    {
      // 0
      m_mate_ex_state->InitializeTextureSamplerTypeEnable( 0, static_cast<H3dTextureSamplerExConst::SamplerTypeEnable>(command&0x1) );
      
      // 1 
      m_mate_ex_state->InitializeTextureSamplerTypeEnable( 1, static_cast<H3dTextureSamplerExConst::SamplerTypeEnable>((command&0x2)>>1) );
      if(command&0x2)
      {
        m_mate_ex_state->InitializeTextureSamplerType( 1, H3dTextureSamplerExConst::SAMPLER_TYPE_TEXTURE_2D );
      }
      else
      {
        m_mate_ex_state->InitializeTextureSamplerType( 1, H3dTextureSamplerExConst::SAMPLER_TYPE_FALSE );
      }

      // 2
      m_mate_ex_state->InitializeTextureSamplerTypeEnable( 2, static_cast<H3dTextureSamplerExConst::SamplerTypeEnable>((command&0x4)>>2) );
      if(command&0x4)
      {
        m_mate_ex_state->InitializeTextureSamplerType( 2, H3dTextureSamplerExConst::SAMPLER_TYPE_TEXTURE_2D );
      }
      else
      {
        m_mate_ex_state->InitializeTextureSamplerType( 2, H3dTextureSamplerExConst::SAMPLER_TYPE_FALSE );
      }
    }
  }
  
  void setTextureSamplerWrapFilter(s32 no, bit32 command, u32 mask)
  {
    // CTR_SDK/include/nn/gx/CTR/gx_MacroTexture.h
    // enum PicaDataTexture0SamplerType
    // のコメントに書かれているアドレス0x080は間違いだと思う。
    
    // 正しくは以下を見る限り0x083だろう。
    // CTR
    // DMPGL2.0システムAPI仕様書
    // 5.8.20.2 テクスチャのサンプラータイプの設定
    // 表5-28 テクスチャサンプラータイプの設定レジスタ

    // 以上を踏まえてここでは、0x083のときにH3dTextureSamplerExConst::SamplerTypeの値を設定しています。

    // 0のSamplerType
    if( no == 0 )
    {
      if( mask & 0xFF000000 )
      {
        m_mate_ex_state->InitializeTextureSamplerType( 0, static_cast<H3dTextureSamplerExConst::SamplerType>((command&0x70000000)>>28) );
      }
    }

    // Wrap
    if( mask & 0x0000FF00 )
    {
      m_mate_ex_state->InitializeTextureSamplerWrapU( no, static_cast<H3dTextureSamplerExConst::Wrap>((command&0x7000)>>12) );
      m_mate_ex_state->InitializeTextureSamplerWrapV( no, static_cast<H3dTextureSamplerExConst::Wrap>((command&0x700)>>8) );
    }
    if( mask & 0x000000FF )
    {
      m_mate_ex_state->InitializeTextureSamplerMagFilter( no, static_cast<H3dTextureSamplerExConst::MagFilter>((command&0x2)>>1) );
      m_mate_ex_state->InitializeTextureSamplerMinFilterDown( no, static_cast<H3dTextureSamplerExConst::MinFilterDown>((command&0x4)>>2) );
    }
    if( mask & 0xFF000000 )
    {
      m_mate_ex_state->InitializeTextureSamplerMinFilterUp( no, static_cast<H3dTextureSamplerExConst::MinFilterUp>((command&0x1000000)>>24) );
    }
  }

  void setTextureSamplerLod(s32 no, bit32 command, u32 mask)
  {
    // @note 使っていないので未実装
  }

  void setRasterizationCullingMode(bit32 command, u32 mask)
  {
#if GFL_DEBUG
    b32 debug_flag = false;
#endif
    if( mask & 0x000000FF )
    {
      m_mate_ex_state->InitializeRasterizationCullingMode( static_cast<H3dRasterizationExConst::CullingMode>(command&0x3) );
#if GFL_DEBUG
      debug_flag = true;
#endif
    }
#if GFL_DEBUG
    GFL_ASSERT(debug_flag);  //@check
#endif
  }


public:
  H3dCommandListenerReadMaterial(H3dMaterialExState* mate_ex_state)
    : m_mate_ex_state(mate_ex_state)
  {
#if GFL_DEBUG
    // 同じアドレスがないか確認する
    for(s32 i=0; i<ADDR_MAX; ++i)
    {
      for(s32 j=i+1; j<ADDR_MAX; ++j)
      {
        GFL_ASSERT( ADDR_LIST[i] != ADDR_LIST[j] );  //@check
      }
    }
#endif
  }
  virtual ~H3dCommandListenerReadMaterial() {}

public:
  virtual bool OnReadData(u32 index, bit32 command, u32 addr, u32 mask, u32 count, u32 count_max, Access access)
  {
    // 同じアドレスが何回も出ることがあるかもしれないので、全コマンドに対して全アドレスをチェックするしかないか？(済んだアドレスから比較対象から消せたら少し初期化時間が短くなるかも)
    for(u32 i=0; i<ADDR_MAX; ++i)
    {
      if( addr == ADDR_LIST[i] )
      {
        // ★★★★この並び順にenumがつくってあるので、並び順を変更しないように。★★★★
        if( i <= ADDR_TEXTURE_COMBINER_COMBINE_5 )
        {
          setTextureCombinerCombine(i-ADDR_TEXTURE_COMBINER_COMBINE_0, command, mask);
        }
        else if( i <= ADDR_TEXTURE_COMBINER_SCALE_5 )
        {
          setTextureCombinerScale(i-ADDR_TEXTURE_COMBINER_SCALE_0, command, mask);
        }
        else if( i <= ADDR_TEXTURE_COMBINER_OPERAND_5 )
        {
          setTextureCombinerOperand(i-ADDR_TEXTURE_COMBINER_OPERAND_0, command, mask);
        }
        else if( i <= ADDR_TEXTURE_COMBINER_SOURCE_5 )
        {
          setTextureCombinerSource(i-ADDR_TEXTURE_COMBINER_SOURCE_0, command, mask);
        }
        else if( i <= ADDR_FRAGMENT_SHADER_BUFFER_INPUT )
        {
          setFragmentShaderBufferInput(command, mask);
        }
        else if( i <= ADDR_STENCIL_OPERATION_TEST )
        {
          setStencilOperationTest(command, mask);
        }
        else if( i <= ADDR_STENCIL_OPERATION_OP )
        {
          setStencilOperationOp(command, mask);
        }
        else if( i <= ADDR_ALPHA_TEST )
        {
          setAlphaTest(command, mask);
        }
        else if( i <= ADDR_DEPTH_TEST )
        {
          setDepthTest(command, mask);
        }
        else if( i <= ADDR_BLEND_MODE )
        {
          setBlendMode(command, mask);
        }
        else if( i <= ADDR_BLEND_FUNCTION )
        {
          setBlendFunction(command, mask);
        }
        else if( i <= ADDR_BLEND_LOGIC_OPERATION )
        {
          setBlendLogicOperation(command, mask);
        }
        else if( i <= ADDR_COLOR_BUFFER_READ )
        {
          setColorBufferRead(command, mask);
        }
        else if( i <= ADDR_COLOR_BUFFER_WRITE )
        {
          setColorBufferWrite(command, mask);
        }
        else if( i <= ADDR_DEPTH_BUFFER_READ )
        {
          setDepthBufferRead(command, mask);
        }
        else if( i <= ADDR_DEPTH_BUFFER_WRITE )
        {
          setDepthBufferWrite(command, mask);
        }
        /*
        //出て来ないからコメントアウト
        else if( i <= ADDR_TEXTURE_SAMPLER_TYPE )
        {
          setTextureSamplerType(command, mask);
        }
        else if( i <= ADDR_TEXTURE_SAMPLER_WRAP_FILTER_2 )
        {
          setTextureSamplerWrapFilter(i-ADDR_TEXTURE_SAMPLER_WRAP_FILTER_0, command, mask);
        }
        else if( i <= ADDR_TEXTURE_SAMPLER_LOD_2 )
        {
          setTextureSamplerLod(i-ADDR_TEXTURE_SAMPLER_LOD_0, command, mask);
        }
        */
        else //if( i <= ADDR_RASTERIZATION_CULLING_MODE )
        {
          setRasterizationCullingMode(command, mask);
        }
      }
    }

    return false;
  }
};

const u32 H3dCommandListenerReadMaterial::ADDR_LIST[ADDR_MAX] =
{
  // 同じアドレスがないようにしておこう
  //
  PICA_REG_TEX_ENV0_COMBINE,
  PICA_REG_TEX_ENV1_COMBINE,
  PICA_REG_TEX_ENV2_COMBINE,
  PICA_REG_TEX_ENV3_COMBINE,
  PICA_REG_TEX_ENV4_COMBINE,
  PICA_REG_TEX_ENV5_COMBINE,
  //
  PICA_REG_TEX_ENV0_SCALE,
  PICA_REG_TEX_ENV1_SCALE,
  PICA_REG_TEX_ENV2_SCALE,
  PICA_REG_TEX_ENV3_SCALE,
  PICA_REG_TEX_ENV4_SCALE,
  PICA_REG_TEX_ENV5_SCALE,
  //
  PICA_REG_TEX_ENV0_OPERAND,
  PICA_REG_TEX_ENV1_OPERAND,
  PICA_REG_TEX_ENV2_OPERAND,
  PICA_REG_TEX_ENV3_OPERAND,
  PICA_REG_TEX_ENV4_OPERAND,
  PICA_REG_TEX_ENV5_OPERAND,
  //
  PICA_REG_TEX_ENV0,
  PICA_REG_TEX_ENV1,
  PICA_REG_TEX_ENV2,
  PICA_REG_TEX_ENV3,
  PICA_REG_TEX_ENV4,
  PICA_REG_TEX_ENV5,
  //
  PICA_REG_TEX_ENV_BUF_INPUT,
  // 
  PICA_REG_STENCIL_TEST,
  PICA_REG_STENCIL_OP,
  //
  PICA_REG_FRAGOP_ALPHA_TEST,
  //
  PICA_REG_DEPTH_COLOR_MASK,
  //
  PICA_REG_COLOR_OPERATION,
  PICA_REG_BLEND_FUNC,
  PICA_REG_LOGIC_OP,
  //
  PICA_REG_COLOR_BUFFER_READ,
  PICA_REG_COLOR_BUFFER_WRITE,
  PICA_REG_DEPTH_STENCIL_BUFFER_READ,
  PICA_REG_DEPTH_STENCIL_BUFFER_WRITE,
  /*
  //出て来ないからコメントアウト
  PICA_REG_TEXTURE_FUNC,
  PICA_REG_TEXTURE0_WRAP_FILTER,
  PICA_REG_TEXTURE1_WRAP_FILTER,
  PICA_REG_TEXTURE2_WRAP_FILTER,
  PICA_REG_TEXTURE0_LOD,
  PICA_REG_TEXTURE1_LOD,
  PICA_REG_TEXTURE2_LOD,
  */
  //
  PICA_REG_CULL_FACE,
  //
};



//-------------------------------------
///	H3Dシェーダ追加ステートクラス
//=====================================
H3dShaderExState::H3dShaderExState(void)
{
  constructorThis();
}

H3dShaderExState::~H3dShaderExState()
{
  Destroy();
  constructorThis();
}

void H3dShaderExState::Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const nw::h3d::res::MaterialContent* cont,
    const Description* desc
)
{
  m_shader_symbol_info.vs_float_list = NULL;
  
  {// 最適化の為、拡張パラメーターを持つ、DefaultShaderという名前のユーザー定義は無いものとする。
    const char*   shader_name = cont->shaderContent->name;
    //ISHIGURO_PRINT("shader_name = %s\n", shader_name);
    
    if ( ::std::strstr( shader_name, "DefaultShader" ) )
      return;    
  }
  
  // @note Floatにしか対応していない
  m_shader_symbol_info.vs_float_name_list = desc->shader_symbol_vs_float_name_list;
  m_shader_symbol_info.vs_float_list_num = desc->shader_symbol_vs_float_list_num;
  m_shader_symbol_info.f_static_vs_float_list = ( desc->p_shader_symbol_vs_float_param_static_buffer ) ? true : false;
  
  s32       staticBufferIndex = 0;

  if( m_shader_symbol_info.vs_float_list_num > 0 )
  {
    nn::gr::CTR::ShaderLite& shader = cont->shaderContent->binding->shader;
  
    m_shader_symbol_info.vs_float_list = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ShaderSymbolVSFloatParameter*[m_shader_symbol_info.vs_float_list_num];    
    ShaderSymbolVSFloatParameter* param;

#ifdef BUGFIX_GFBTS1721_20130801    
    if ( m_shader_symbol_info.f_static_vs_float_list )
      param = &(desc->p_shader_symbol_vs_float_param_static_buffer[staticBufferIndex++]);
    else
      param = GFL_NEW(heap_allocator->GetHeapBase()) ShaderSymbolVSFloatParameter;
#else
    if ( m_shader_symbol_info.f_static_vs_float_list )
      param = &(desc->p_shader_symbol_vs_float_param_static_buffer[staticBufferIndex]);
    else
      param = GFL_NEW(heap_allocator->GetHeapBase()) ShaderSymbolVSFloatParameter;
#endif    
    b32     fHit = false;
    s32     oTopNodeOffset = 0;
    
    for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
    {
      b32                 fTopNode = true;
      b32                 hasDefaultValue = false;
      gfl::math::VEC4     defaultValue(0.0f, 0.0f, 0.0f, 0.0f);
      s32                 fixIndex = -1;
      
      if ( desc->shader_symbol_vs_float_param_list )
      {
        fTopNode = desc->shader_symbol_vs_float_param_list[i].fTopNode;
        hasDefaultValue = desc->shader_symbol_vs_float_param_list[i].hasDefaultValue;
        defaultValue = desc->shader_symbol_vs_float_param_list[i].defaultValue;
        fixIndex = desc->shader_symbol_vs_float_param_list[i].fixIndex;
      }
      
      if ( fTopNode == true )
        oTopNodeOffset = 0;
      
      //前回ヒットせず、トップノードでなければ、連続してヒットしないのでスキップさせる。
      if ( fHit == true || fTopNode == true )
      {
        //nn::os::Tick fdi_start_tick = nn::os::Tick::GetSystemCurrent();
        if ( fixIndex == -1 )
          fHit = shader.SearchBindSymbol(&(param->bind_symbol), m_shader_symbol_info.vs_float_name_list[i]);
        else
          fHit = shader.SearchBindSymbol(&(param->bind_symbol), fixIndex);
        
        /*
        nn::os::Tick fdi_end_tick = nn::os::Tick::GetSystemCurrent();

        fdi_end_tick = fdi_end_tick - fdi_start_tick;
        TOMOYA_PRINT( "search bind symbol %s time %d\n", m_shader_symbol_info.vs_float_name_list[i], static_cast<s32>(fdi_end_tick.ToTimeSpan().GetMicroSeconds()) );
        */
      }
      
      if( fHit )
      {
        // 見つかった
        {
          // 残りの初期化
          param->bitflag = 0;
          param->value = defaultValue;
        }

        // 値を取得する
        if ( hasDefaultValue == false )
        {
          /*
          CTR_SDK/include/nn/gr/CTR/gr_Utility.h
            inline bit32* MakeUniformCommandVS( bit32* command, u8 location, const nn::math::VEC4& vec4 )
          CTR_SDK/include/nn/gr/CTR/gr_BindSymbol.h
                bit32* MakeUniformCommand( bit32* command, const nn::math::VEC4& vec4 ) const
          */

          H3dCommandListenerReadShader listener(param->bind_symbol.start, &(param->value));
          
          //はずれH3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(cont->shaderContent->shaderFullCmd.GetBeginAddress()),
          //    cont->shaderContent->shaderFullCmd.size(), &listener);
          //はずれH3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(cont->shaderContent->shaderCmd.GetBeginAddress()),
          //    cont->shaderContent->shaderCmd.size(), &listener);
          //はずれH3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(cont->shaderContent->shaderWithPrepareCmd.GetBeginAddress()),
          //    cont->shaderContent->shaderWithPrepareCmd.size(), &listener);

          //nn::os::Tick fdi_start_tick = nn::os::Tick::GetSystemCurrent();

          if(fTopNode){
            oTopNodeOffset = H3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(cont->paramCmd.GetBeginAddress()+oTopNodeOffset),
                cont->paramCmd.size()-oTopNodeOffset, &listener);
          }
          else{
            u32 end_index;
            end_index = H3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(cont->paramCmd.GetBeginAddress()+oTopNodeOffset),
                          cont->paramCmd.size()-oTopNodeOffset, &listener);
          }

          /*
          nn::os::Tick fdi_end_tick = nn::os::Tick::GetSystemCurrent();
          fdi_end_tick = fdi_end_tick - fdi_start_tick;
          TOMOYA_PRINT( "scan command %s time %d\n", m_shader_symbol_info.vs_float_name_list[i], static_cast<s32>(fdi_end_tick.ToTimeSpan().GetMicroSeconds()) );
          */

// Bool型には未対応。
// 対応しているのはVEC4のみ。

/*
シェーダパラメータの Vector* 型は
"nw::h3d::res::MaterialContent::paramCmd"
にコマンドとして格納されています。

シェーダパラメータの Bool 型は
"nw::h3d::res::SubMeshData::drawCmd"
にコマンドとして格納されています。

CTR のシェーダの仕様で、Bool のユニフォーム設定が
ひとつのコマンドにまとまっており、Bool 型のユニフォームの
使用目的の事情で、描画キック毎に送る必要があります。

そのため、Bool 型のユニフォームは Vector* 型とは
別のコマンドに格納されることになっています。
*/

        }

/*
CTR_SDK/sources/libraries/gr/CTR/
gr_Shader.cpp

            bool Shader::SearchBindSymbol( BindSymbol* symbol,
                                           const char* name ) const
ループを回す場合の参考になる記述
                for ( s32 i = 0; i < exe_info->bindSymbolCount; ++i )
*/

        m_shader_symbol_info.vs_float_list[i] = param;
#ifdef BUGFIX_GFBTS1721_20130801            
        if ( m_shader_symbol_info.f_static_vs_float_list )
          param = &(desc->p_shader_symbol_vs_float_param_static_buffer[staticBufferIndex++]);
        else
          param = GFL_NEW(heap_allocator->GetHeapBase()) ShaderSymbolVSFloatParameter;
#else        
        ++staticBufferIndex;
        
        // 次
        if ( m_shader_symbol_info.f_static_vs_float_list == false )
          param = GFL_NEW(heap_allocator->GetHeapBase()) ShaderSymbolVSFloatParameter;
#endif
      }
      else
      {
        // 見つからなかった
        m_shader_symbol_info.vs_float_list[i] = NULL;
      }
    }
    
    // 最後の1個は見つかっていない
    if ( m_shader_symbol_info.f_static_vs_float_list == false )
      GFL_DELETE param;
  }
}

void H3dShaderExState::Destroy(void)
{
  // @note Floatにしか対応していない
  if( m_shader_symbol_info.vs_float_list )
  {
    for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
    {
      if( m_shader_symbol_info.vs_float_list[i] )
      {
        if ( m_shader_symbol_info.f_static_vs_float_list == false )
          GFL_DELETE m_shader_symbol_info.vs_float_list[i];
        m_shader_symbol_info.vs_float_list[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shader_symbol_info.vs_float_list;
    m_shader_symbol_info.vs_float_list = NULL;
  }
  m_shader_symbol_info.vs_float_name_list = NULL;
  m_shader_symbol_info.vs_float_list_num = 0;
}

void H3dShaderExState::DumpShaderSymbolName(
    const nw::h3d::res::MaterialContent* cont
) const
{
  // @note Floatにしか対応していない
  nn::gr::CTR::ShaderLite& shader = cont->shaderContent->binding->shader;

  u32 vs_input_num   = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_VERTEX,   nn::gr::CTR::BindSymbol::SYMBOL_TYPE_INPUT  ) ;
  u32 vs_float_num   = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_VERTEX,   nn::gr::CTR::BindSymbol::SYMBOL_TYPE_FLOAT  ) ;
  u32 vs_integer_num = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_VERTEX,   nn::gr::CTR::BindSymbol::SYMBOL_TYPE_INTEGER) ;
  u32 vs_bool_num    = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_VERTEX,   nn::gr::CTR::BindSymbol::SYMBOL_TYPE_BOOL   ) ;
  u32 gs_input_num   = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_GEOMETRY, nn::gr::CTR::BindSymbol::SYMBOL_TYPE_INPUT  ) ;
  u32 gs_float_num   = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_GEOMETRY, nn::gr::CTR::BindSymbol::SYMBOL_TYPE_FLOAT  ) ;
  u32 gs_integer_num = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_GEOMETRY, nn::gr::CTR::BindSymbol::SYMBOL_TYPE_INTEGER) ;
  u32 gs_bool_num    = shader.SearchBindSymbolNum(nn::gr::CTR::BindSymbol::SHADER_TYPE_GEOMETRY, nn::gr::CTR::BindSymbol::SYMBOL_TYPE_BOOL   ) ;

  u32 total_num =        \
          vs_input_num   \
        + vs_float_num   \
        + vs_integer_num \
        + vs_bool_num    \
        + gs_input_num   \
        + gs_float_num   \
        + gs_integer_num \
        + gs_bool_num    ;

  GFL_PRINT("total_num      = %d\n", total_num      );
  GFL_PRINT("vs_input_num   = %d\n", vs_input_num   );
  GFL_PRINT("vs_float_num   = %d\n", vs_float_num   );
  GFL_PRINT("vs_integer_num = %d\n", vs_integer_num );
  GFL_PRINT("vs_bool_num    = %d\n", vs_bool_num    );
  GFL_PRINT("gs_input_num   = %d\n", gs_input_num   );
  GFL_PRINT("gs_float_num   = %d\n", gs_float_num   );
  GFL_PRINT("gs_integer_num = %d\n", gs_integer_num );
  GFL_PRINT("gs_bool_num    = %d\n", gs_bool_num    );

  for(u32 i=0; i<total_num; ++i)
  {
    nn::gr::CTR::BindSymbolVSFloat symbol;
    shader.SearchBindSymbol(&symbol, i);  // @note 何でもかんでもVSFloatで取得していいの？→デバッグ用だからまあここはこれでいいか。
    if( symbol.name ) GFL_PRINT("[%d] name = %s\n", i, symbol.name);
    else              GFL_PRINT("[%d] no name\n", i);
  }
}


void H3dShaderExState::SetShaderSymbolVSFloat(const char* name, const gfl::math::VEC4& value)
{
  for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
  {
    if( strcmp(m_shader_symbol_info.vs_float_name_list[i], name)==0 )
    {
      SetShaderSymbolVSFloat(i, value);
      break;  // 同じ名前があったとしても先に見つけたのが優先
    }
  }
}
void H3dShaderExState::SetShaderSymbolVSFloat(s32 index, const gfl::math::VEC4& value)
{
  if( m_shader_symbol_info.vs_float_list )
  {
    if( m_shader_symbol_info.vs_float_list[index] )
    {
      m_shader_symbol_info.vs_float_list[index]->bitflag = 1;
      m_shader_symbol_info.vs_float_list[index]->value = value;
    }
  }
}


void H3dShaderExState::GetShaderSymbolVSFloat(const char* name, gfl::math::VEC4* value) const
{
  for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
  {
    if( strcmp(m_shader_symbol_info.vs_float_name_list[i], name)==0 )
    {
      GetShaderSymbolVSFloat(i, value);
      break;  // 同じ名前があったとしても先に見つけたのが優先
    }
  }
}
void H3dShaderExState::GetShaderSymbolVSFloat(s32 index, gfl::math::VEC4* value) const
{
  if( m_shader_symbol_info.vs_float_list )
  {
    if( index < m_shader_symbol_info.vs_float_list_num )
    {
      if( m_shader_symbol_info.vs_float_list[index] )
      {
        //フラグ見たほうがいいか？m_shader_symbol_info.vs_float_list[index]->bitflag == 1
        *value = m_shader_symbol_info.vs_float_list[index]->value;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "H3dShaderExState::GetShaderSymbolVSFloat index over\n");
    }
  }
}

s32 H3dShaderExState::GetShaderSymbolIndex(const char* name) const
{
  for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
  {
    if( strcmp(m_shader_symbol_info.vs_float_name_list[i], name)==0 )
    {
      return i;
    }
  }
  return -1;
}

void H3dShaderExState::MakeShaderCommand(H3dCommand* command)
{
  nn::gr::CTR::CommandBufferJumpHelper  *dcs  =  command->GetNwDrawCommandStream();
  b32                         isDirty = false;
  
  // @note Floatにしか対応していない
  for(s32 i=0; i<m_shader_symbol_info.vs_float_list_num; ++i)
  {
    if( m_shader_symbol_info.vs_float_list[i] && m_shader_symbol_info.vs_float_list[i]->bitflag )
    {
      dcs->Put( m_shader_symbol_info.vs_float_list[i]->bind_symbol.MakeUniformCommand(dcs->GetCurrentPtr(), m_shader_symbol_info.vs_float_list[i]->value) );
    }
  }
}

void H3dShaderExState::constructorThis(void)
{
  m_shader_symbol_info.vs_input_name_list   = NULL;
  m_shader_symbol_info.vs_float_name_list   = NULL;
  m_shader_symbol_info.vs_integer_name_list = NULL;
  m_shader_symbol_info.vs_input_list_num   = 0;
  m_shader_symbol_info.vs_float_list_num   = 0;
  m_shader_symbol_info.vs_integer_list_num = 0;
  m_shader_symbol_info.vs_input_list   = NULL;
  m_shader_symbol_info.vs_float_list   = NULL;
  m_shader_symbol_info.vs_integer_list = NULL;

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
  ResetCommandCount();
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
}

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
H3dShaderExState::CommandCount H3dShaderExState::m_shader_command_max_set;
H3dShaderExState::CommandCount H3dShaderExState::m_command_curr_set;

void H3dShaderExState::ResetCommandCount(void)
{
  m_shader_command_max_set.shader_command = 0;
  m_command_curr_set.shader_command = 0;
}
void H3dShaderExState::StartOneFrameCommandCount(void)
{
  m_command_curr_set.shader_command = 0;
}
void H3dShaderExState::EndOneFrameCommandCount(void)
{
  b32 print_flag = false;
  
  if( m_command_curr_set.shader_command > m_shader_command_max_set.shader_command )
  {
    m_shader_command_max_set.shader_command = m_command_curr_set.shader_command;
    print_flag = true;
  }

  if(print_flag)
  {
    PrintCommandCount();
  }
}
void H3dShaderExState::PrintCommandCount(void)
{
  GFL_PRINT("[m_shader_command_max_set]  ");
  printCommandCount(m_shader_command_max_set);
  GFL_PRINT("[m_command_curr_set]  ");
  printCommandCount(m_command_curr_set);
}
void H3dShaderExState::printCommandCount(const CommandCount& command_count)
{
  GFL_PRINT("shader_command=%d\n",
      command_count.shader_command);
}
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT



//-------------------------------------
///	H3Dマテリアル追加ステートクラス
//=====================================

H3dMaterialExState::H3dMaterialExState(void)
{
  constructorThis();
}

H3dMaterialExState::~H3dMaterialExState()
{
  m_shader_ex_state.Destroy();
  constructorThis();
}



void H3dMaterialExState::InitializeTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine)
{
  m_tex_comb_step[step].combine_rgb = combine;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_COMBINE_RGB_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerCombineRgb(s32 step, H3dTextureCombinerExConst::Combine combine)
{
  InitializeTextureCombinerCombineRgb(step, combine);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_COMBINE_RGB_MUTABLE;
}
H3dTextureCombinerExConst::Combine H3dMaterialExState::GetTextureCombinerCombineRgb(s32 step)
{
  return static_cast<H3dTextureCombinerExConst::Combine>(m_tex_comb_step[step].combine_rgb);
}

void H3dMaterialExState::InitializeTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine)
{
  m_tex_comb_step[step].combine_alpha = combine;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_COMBINE_ALPHA_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerCombineAlpha(s32 step, H3dTextureCombinerExConst::Combine combine)
{
  InitializeTextureCombinerCombineAlpha(step, combine);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_COMBINE_ALPHA_MUTABLE;
}
H3dTextureCombinerExConst::Combine H3dMaterialExState::GetTextureCombinerCombineAlpha(s32 step)
{
  return static_cast<H3dTextureCombinerExConst::Combine>(m_tex_comb_step[step].combine_alpha);
}

void H3dMaterialExState::InitializeTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale)
{
  m_tex_comb_step[step].scale_rgb = scale;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_SCALE_RGB_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerScaleRgb(s32 step, H3dTextureCombinerExConst::Scale scale)
{
  InitializeTextureCombinerScaleRgb(step, scale);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_SCALE_RGB_MUTABLE;
}
H3dTextureCombinerExConst::Scale H3dMaterialExState::GetTextureCombinerScaleRgb(s32 step)
{
  return static_cast<H3dTextureCombinerExConst::Scale>(m_tex_comb_step[step].scale_rgb);
}

void H3dMaterialExState::InitializeTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale)
{
  m_tex_comb_step[step].scale_alpha = scale;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_SCALE_ALPHA_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerScaleAlpha(s32 step, H3dTextureCombinerExConst::Scale scale)
{
  InitializeTextureCombinerScaleAlpha(step, scale);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::TEXTURE_COMBINER_SCALE_ALPHA_MUTABLE;
}
H3dTextureCombinerExConst::Scale H3dMaterialExState::GetTextureCombinerScaleAlpha(s32 step)
{
  return static_cast<H3dTextureCombinerExConst::Scale>(m_tex_comb_step[step].scale_alpha);
}

void H3dMaterialExState::InitializeTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope)
{
  m_tex_comb_step[step].source[no].operand_rgb = ope;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_OPERAND_RGB_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerOperandRgb(s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope)
{
  InitializeTextureCombinerOperandRgb(step, no, ope);
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_OPERAND_RGB_MUTABLE;
}
H3dTextureCombinerExConst::OperandRgb H3dMaterialExState::GetTextureCombinerOperandRgb(s32 step, s32 no)
{
  return static_cast<H3dTextureCombinerExConst::OperandRgb>(m_tex_comb_step[step].source[no].operand_rgb);
}

void H3dMaterialExState::InitializeTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope)
{
  m_tex_comb_step[step].source[no].operand_alpha = ope;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_OPERAND_ALPHA_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerOperandAlpha(s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope)
{
  InitializeTextureCombinerOperandAlpha(step, no, ope);
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_OPERAND_ALPHA_MUTABLE;
}
H3dTextureCombinerExConst::OperandAlpha H3dMaterialExState::GetTextureCombinerOperandAlpha(s32 step, s32 no)
{
  return static_cast<H3dTextureCombinerExConst::OperandAlpha>(m_tex_comb_step[step].source[no].operand_alpha);
}

void H3dMaterialExState::InitializeTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  m_tex_comb_step[step].source[no].source_rgb = source;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_SOURCE_RGB_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerSourceRgb(s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  InitializeTextureCombinerSourceRgb(step, no, source);
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_SOURCE_RGB_MUTABLE;
}
H3dTextureCombinerExConst::Source H3dMaterialExState::GetTextureCombinerSourceRgb(s32 step, s32 no)
{
  return static_cast<H3dTextureCombinerExConst::Source>(m_tex_comb_step[step].source[no].source_rgb);
}

void H3dMaterialExState::InitializeTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  m_tex_comb_step[step].source[no].source_alpha = source;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_SOURCE_ALPHA_MUTABLE;
#endif
}
void H3dMaterialExState::SetTextureCombinerSourceAlpha(s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  InitializeTextureCombinerSourceAlpha(step, no, source);
  m_tex_comb_step[step].source[no].bitflag |= TextureCombinerSource::TEXTURE_COMBINER_SOURCE_ALPHA_MUTABLE;
}
H3dTextureCombinerExConst::Source H3dMaterialExState::GetTextureCombinerSourceAlpha(s32 step, s32 no)
{
  return static_cast<H3dTextureCombinerExConst::Source>(m_tex_comb_step[step].source[no].source_alpha);
}

//SDK 5.0 対応
void H3dMaterialExState::SetMaterialTextureCombinerConstant(s32 step, s32 no, const gfl::grp::ColorU8& rColor)
{
  m_MaterialTextureCombinerConstantNo[step] = no;
  m_ConbinerColor[step] = rColor;
}

//SDK 5.0 対応
s32 H3dMaterialExState::GetMaterialTextureCombinerConstant(s32 step) const
{
  return m_MaterialTextureCombinerConstantNo[step];
}

//SDK 5.0 対応
gfl::grp::ColorU8 H3dMaterialExState::GetMaterialTextureCombinerConstantColor(s32 step) const
{
  return m_ConbinerColor[step];
}

void H3dMaterialExState::InitializeFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  m_tex_comb_step[step].buffer_input_rgb = input;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_RGB_MUTABLE;
#endif
}
void H3dMaterialExState::SetFragmentShaderBufferInputRgb(s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  InitializeFragmentShaderBufferInputRgb(step, input);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_RGB_MUTABLE;
}
H3dFragmentShaderExConst::BufferInput H3dMaterialExState::GetFragmentShaderBufferInputRgb(s32 step)
{
  return static_cast<H3dFragmentShaderExConst::BufferInput>(m_tex_comb_step[step].buffer_input_rgb);
}

void H3dMaterialExState::InitializeFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  m_tex_comb_step[step].buffer_input_alpha = input;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_ALPHA_MUTABLE;
#endif
}
void H3dMaterialExState::SetFragmentShaderBufferInputAlpha(s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  InitializeFragmentShaderBufferInputAlpha(step, input);
  m_tex_comb_step[step].bitflag |= TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_ALPHA_MUTABLE;
}
H3dFragmentShaderExConst::BufferInput H3dMaterialExState::GetFragmentShaderBufferInputAlpha(s32 step)
{
  return static_cast<H3dFragmentShaderExConst::BufferInput>(m_tex_comb_step[step].buffer_input_alpha);
}



void H3dMaterialExState::InitializeStencilOperationTest(u8 enable, H3dStencilOperationExConst::TestFunc func_func, u8 mask, u8 func_ref, u8 func_mask)
{
  m_stencil_op_info.enable    = enable;
  m_stencil_op_info.func_func = func_func;
  m_stencil_op_info.mask      = mask;
  m_stencil_op_info.func_ref  = func_ref;
  m_stencil_op_info.func_mask = func_mask;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
#endif
}
void H3dMaterialExState::InitializeStencilOperationOp(
    H3dStencilOperationExConst::StencilOp op_fail, H3dStencilOperationExConst::StencilOp op_zfail, H3dStencilOperationExConst::StencilOp op_zpass)
{
  m_stencil_op_info.op_fail    = op_fail;
  m_stencil_op_info.op_zfail   = op_zfail;
  m_stencil_op_info.op_zpass   = op_zpass;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_OP_MUTABLE;
#endif
}

b32 H3dMaterialExState::IsStencilOperationTestEnable(void) const
{
  return ( m_stencil_op_info.enable != 0 );
}
void H3dMaterialExState::SetStencilOperationTestEnable(b32 enable)
{
  if( enable ) m_stencil_op_info.enable = 1;
  else         m_stencil_op_info.enable = 0;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
}

H3dStencilOperationExConst::TestFunc H3dMaterialExState::GetStencilOperationTestFunc(void) const
{
  return static_cast<H3dStencilOperationExConst::TestFunc>(m_stencil_op_info.func_func);
}
void H3dMaterialExState::SetStencilOperationTestFunc(H3dStencilOperationExConst::TestFunc func_func)
{
  m_stencil_op_info.func_func = func_func;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
}

void H3dMaterialExState::SetStencilOperationTestFuncRef(u8 func_ref)
{
  m_stencil_op_info.func_ref = func_ref;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
}
u8 H3dMaterialExState::GetStencilOperationTestFuncRef(void) const
{
  return m_stencil_op_info.func_ref;
}

u8 H3dMaterialExState::GetStencilOperationTestMask(void) const  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
{
  return m_stencil_op_info.mask;
}
void H3dMaterialExState::SetStencilOperationTestMask(u8 mask)  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
{
  m_stencil_op_info.mask = mask;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
}
u8 H3dMaterialExState::GetStencilOperationTestFuncMask(void) const  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
{
  return m_stencil_op_info.func_mask;
}
void H3dMaterialExState::SetStencilOperationTestFuncMask(u8 func_mask)  // @note MaskかFuncMaskどちらかを使用禁止にしたい。どっちがオーサリングツールで設定できるほうか？
{
  m_stencil_op_info.func_mask = func_mask;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_TEST_MUTABLE;
}

H3dStencilOperationExConst::StencilOp H3dMaterialExState::GetStencilOperationOpFail(void) const
{
  return static_cast<H3dStencilOperationExConst::StencilOp>(m_stencil_op_info.op_fail);
}
void H3dMaterialExState::SetStencilOperationOpFail(H3dStencilOperationExConst::StencilOp op_fail)
{
  m_stencil_op_info.op_fail = op_fail;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_OP_MUTABLE;
}
H3dStencilOperationExConst::StencilOp H3dMaterialExState::GetStencilOperationOpZFail(void) const
{
  return static_cast<H3dStencilOperationExConst::StencilOp>(m_stencil_op_info.op_zfail);
}
void H3dMaterialExState::SetStencilOperationOpZFail(H3dStencilOperationExConst::StencilOp op_zfail)
{
  m_stencil_op_info.op_zfail = op_zfail;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_OP_MUTABLE;
}
H3dStencilOperationExConst::StencilOp H3dMaterialExState::GetStencilOperationOpZPass(void) const
{
  return static_cast<H3dStencilOperationExConst::StencilOp>(m_stencil_op_info.op_zpass);
}
void H3dMaterialExState::SetStencilOperationOpZPass(H3dStencilOperationExConst::StencilOp op_zpass)
{
  m_stencil_op_info.op_zpass = op_zpass;
  m_stencil_op_info.bitflag |= StencilOperationInfo::STENCIL_OP_MUTABLE;
}



void H3dMaterialExState::InitializeAlphaTest(u8 enable, H3dAlphaTestExConst::TestFunc func, u8 ref)
{
  m_alpha_test_info.enable = enable;
  m_alpha_test_info.func   = func;
  m_alpha_test_info.ref    = ref;
#if GFL_GRP_G3D_H3D_EX_DEBUG_MATERIAL_EX_STATE_ALL_MUTABLE
  m_alpha_test_info.bitflag |= AlphaTestInfo::ALPHA_TEST_MUTABLE;
#endif
}

b32 H3dMaterialExState::IsAlphaTestEnable(void) const
{
  return ( m_alpha_test_info.enable != 0 );
}
void H3dMaterialExState::SetAlphaTestEnable(b32 enable)  // trueのときアルファテスト有効
{
  if(enable) m_alpha_test_info.enable = 1;
  else       m_alpha_test_info.enable = 0;
  m_alpha_test_info.bitflag |= AlphaTestInfo::ALPHA_TEST_MUTABLE;
}
H3dAlphaTestExConst::TestFunc H3dMaterialExState::GetAlphaTestFunc(void) const
{
  return static_cast<H3dAlphaTestExConst::TestFunc>(m_alpha_test_info.func);
}
void H3dMaterialExState::SetAlphaTestFunc(H3dAlphaTestExConst::TestFunc func)
{
  m_alpha_test_info.func = func;
  m_alpha_test_info.bitflag |= AlphaTestInfo::ALPHA_TEST_MUTABLE;
}

void H3dMaterialExState::SetAlphaTestRef(u8 ref)
{
  m_alpha_test_info.ref = ref;
  m_alpha_test_info.bitflag |= AlphaTestInfo::ALPHA_TEST_MUTABLE;
}
u8 H3dMaterialExState::GetAlphaTestRef(void) const
{
  return m_alpha_test_info.ref;
}


void H3dMaterialExState::InitializeDepthTest(u8 enable, u8 mask_enable, H3dDepthTestExConst::TestFunc func)
{
  m_depth_color_mask_info.depth_test_enable = enable;
  m_depth_color_mask_info.depth_mask        = mask_enable;
  m_depth_color_mask_info.depth_test_func   = func;

  // 今のところ変更する予定はないので常に1にしておく
  m_depth_color_mask_info.red_mask   = 1;
  m_depth_color_mask_info.green_mask = 1;
  m_depth_color_mask_info.blue_mask  = 1;
  m_depth_color_mask_info.alpha_mask = 1;
}
b32 H3dMaterialExState::IsDepthTestEnable(void) const
{
  return ( m_depth_color_mask_info.depth_test_enable != 0 );
}
void H3dMaterialExState::SetDepthTestEnable(b32 enable)  // trueのときデプステスト有効
{
  if(enable) m_depth_color_mask_info.depth_test_enable = 1;
  else       m_depth_color_mask_info.depth_test_enable = 0;
  m_depth_color_mask_info.bitflag |= DepthColorMaskInfo::DEPTH_TEST_MUTABLE;
  
  m_buffer_read_info.fDepthRead = m_depth_color_mask_info.depth_test_enable;
}
b32 H3dMaterialExState::IsDepthTestMaskEnable(void) const
{
  return ( m_depth_color_mask_info.depth_mask != 0 );
}
void H3dMaterialExState::SetDepthTestMaskEnable(b32 enable)  // trueのときデプスマスク有効(trueのときデプスバッファを更新する)
{
  if(enable) m_depth_color_mask_info.depth_mask = 1;
  else       m_depth_color_mask_info.depth_mask = 0;
  m_depth_color_mask_info.bitflag |= DepthColorMaskInfo::DEPTH_COLOR_MASK_MUTABLE;
  
  m_buffer_read_info.fDepthWrite = (m_depth_color_mask_info.depth_mask) ? 1 : 0;
}
H3dDepthTestExConst::TestFunc H3dMaterialExState::GetDepthTestFunc(void) const
{
  return static_cast<H3dDepthTestExConst::TestFunc>(m_depth_color_mask_info.depth_test_func);
}
void H3dMaterialExState::SetDepthTestFunc(H3dDepthTestExConst::TestFunc func)
{
  m_depth_color_mask_info.depth_test_func = func;
  m_depth_color_mask_info.bitflag |= DepthColorMaskInfo::DEPTH_TEST_MUTABLE;
  m_buffer_read_info.fDepthRead = m_depth_color_mask_info.depth_test_enable;
}


void H3dMaterialExState::InitializeBlendMode(H3dBlendExConst::Mode mode)
{
  m_blend_info.blend_mode = mode;
}
void H3dMaterialExState::InitializeBlendFunction(
    H3dBlendExConst::Equation rgb_equation,
    H3dBlendExConst::Factor   rgb_src_function,
    H3dBlendExConst::Factor   rgb_dst_function,
    H3dBlendExConst::Equation alpha_equation,
    H3dBlendExConst::Factor   alpha_src_function,
    H3dBlendExConst::Factor   alpha_dst_function
)
{
  m_blend_info.blend_equation_rgb       = rgb_equation;
  m_blend_info.blend_equation_alpha     = alpha_equation;
  m_blend_info.blend_function_src_rgb   = rgb_src_function;
  m_blend_info.blend_function_src_alpha = alpha_src_function;
  m_blend_info.blend_function_dst_rgb   = rgb_dst_function;
  m_blend_info.blend_function_dst_alpha = alpha_dst_function;
}
void H3dMaterialExState::InitializeBlendLogicOperation(H3dBlendExConst::LogicOperation operation)
{
  m_blend_info.logic_operation = operation;
}

void H3dMaterialExState::InitializeColorBufferRead( b8 fColorRead )
{
  m_buffer_read_info.fColorRead = fColorRead;
}

void H3dMaterialExState::InitializeColorBufferWrite( b8 fColorWrite )
{
  m_buffer_read_info.fColorWrite = fColorWrite;
}

void H3dMaterialExState::InitializeDepthBufferRead( b8 fDepthRead, b8 fStencilRead )
{
  m_buffer_read_info.fDepthRead = fDepthRead;
  m_buffer_read_info.fStencilRead = fStencilRead;
}

void H3dMaterialExState::InitializeDepthBufferWrite( b8 fDepthWrite, b8 fStencilWrite )
{
  m_buffer_read_info.fDepthWrite = fDepthWrite;
  m_buffer_read_info.fStencilWrite = fStencilWrite;
}

void H3dMaterialExState::InitializeBlendAfterAllSet(void)
{
  if( m_blend_info.blend_mode != H3dBlendExConst::MODE_LOGIC )
  {
    if(
         m_blend_info.blend_equation_rgb       == H3dBlendExConst::EQUATION_FUNC_ADD
      && m_blend_info.blend_function_src_rgb   == H3dBlendExConst::FACTOR_ONE
      && m_blend_info.blend_function_dst_rgb   == H3dBlendExConst::FACTOR_ZERO
      && m_blend_info.blend_equation_alpha     == H3dBlendExConst::EQUATION_FUNC_ADD
      && m_blend_info.blend_function_src_alpha == H3dBlendExConst::FACTOR_ONE
      && m_blend_info.blend_function_dst_alpha == H3dBlendExConst::FACTOR_ZERO
    )
    {
      m_blend_info.blend_mode = H3dBlendExConst::MODE_NOT_USE;
    }
    else if(
         m_blend_info.blend_equation_rgb       == m_blend_info.blend_equation_alpha
      && m_blend_info.blend_function_src_rgb   == m_blend_info.blend_function_src_alpha
      && m_blend_info.blend_function_dst_rgb   == m_blend_info.blend_function_dst_alpha
    )
    {
      m_blend_info.blend_mode = H3dBlendExConst::MODE_BLEND;
    }
    else
    {
      m_blend_info.blend_mode = H3dBlendExConst::MODE_SEPARATE_BLEND;
    }
  }
}

// @attention 【注意】ブレンドモードによって設定できたりしなかったりするように制御しているので、まず最初にブレンドモードを変更して下さい。
void H3dMaterialExState::SetBlendMode(H3dBlendExConst::Mode mode)
{
  // NintendoWare/CTR/documents/API/nw/gfx/res/ResBlendOperation/Mode.html
  // nw::gfx::res::ResBlendOperation::Mode 列挙型
  // CTR の GPU にはブレンドモードと論理演算モードの 2 つしかなく、 MODE_NOT_USE が指定されているときは、 カラーとアルファそれぞれの入力要素に BLENDFACTOR_ONE が、出力要素に BLENDFACTOR_ZERO が、 ブレンド式に BLENDEQ_FUNC_ADD が指定された状態で、ブレンドモードが有効になっています。

  switch(mode)
  {
  case H3dBlendExConst::MODE_NOT_USE:
    {
      SetBlendEquationRgb(H3dBlendExConst::EQUATION_FUNC_ADD);
      SetBlendFunctionSourceRgb(H3dBlendExConst::FACTOR_ONE);
      SetBlendFunctionDestinationRgb(H3dBlendExConst::FACTOR_ZERO);
      SetBlendEquationAlpha(H3dBlendExConst::EQUATION_FUNC_ADD);
      SetBlendFunctionSourceAlpha(H3dBlendExConst::FACTOR_ONE);
      SetBlendFunctionDestinationAlpha(H3dBlendExConst::FACTOR_ZERO);
      break;
    }
  case H3dBlendExConst::MODE_BLEND:
    {
      SetBlendEquationAlpha(GetBlendEquationRgb());
      SetBlendFunctionSourceAlpha(GetBlendFunctionSourceRgb());
      SetBlendFunctionDestinationAlpha(GetBlendFunctionDestinationRgb());
      break;
    }
  case H3dBlendExConst::MODE_SEPARATE_BLEND:
    {
      break;
    }
  case H3dBlendExConst::MODE_LOGIC:
    {
      break;
    }
  }
  
  m_blend_info.blend_mode = mode;
  m_blend_info.bitflag |= BlendInfo::BLEND_MODE_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
}
H3dBlendExConst::Mode H3dMaterialExState::GetBlendMode(void) const
{
  return static_cast<H3dBlendExConst::Mode>(m_blend_info.blend_mode);
}

H3dBlendExConst::LogicOperation H3dMaterialExState::GetBlendLogicOperation(void) const
{
  return static_cast<H3dBlendExConst::LogicOperation>(m_blend_info.logic_operation);
}
void H3dMaterialExState::SetBlendLogicOperation(H3dBlendExConst::LogicOperation operation)
{
  m_blend_info.logic_operation = operation;
  m_blend_info.bitflag |= BlendInfo::BLEND_LOGIC_OPERATION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
}

H3dBlendExConst::Factor H3dMaterialExState::GetBlendFunctionSourceRgb(void) const
{
  return static_cast<H3dBlendExConst::Factor>(m_blend_info.blend_function_src_rgb);
}
void H3dMaterialExState::SetBlendFunctionSourceRgb(H3dBlendExConst::Factor factor)
{
  m_blend_info.blend_function_src_rgb = factor;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
  
  if( m_blend_info.blend_mode == H3dBlendExConst::MODE_BLEND )
  {
    SetBlendFunctionSourceAlpha(factor);
  }
}
H3dBlendExConst::Factor H3dMaterialExState::GetBlendFunctionSourceAlpha(void) const
{
  return static_cast<H3dBlendExConst::Factor>(m_blend_info.blend_function_src_alpha);
}
void H3dMaterialExState::SetBlendFunctionSourceAlpha(H3dBlendExConst::Factor factor)
{
  m_blend_info.blend_function_src_alpha = factor;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
}

H3dBlendExConst::Factor H3dMaterialExState::GetBlendFunctionDestinationRgb(void) const
{
  return static_cast<H3dBlendExConst::Factor>(m_blend_info.blend_function_dst_rgb);
}
void H3dMaterialExState::SetBlendFunctionDestinationRgb(H3dBlendExConst::Factor factor)
{
  m_blend_info.blend_function_dst_rgb = factor;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;

  if( m_blend_info.blend_mode == H3dBlendExConst::MODE_BLEND )
  {
    SetBlendFunctionDestinationAlpha(factor);
  }
}
H3dBlendExConst::Factor H3dMaterialExState::GetBlendFunctionDestinationAlpha(void) const
{
  return static_cast<H3dBlendExConst::Factor>(m_blend_info.blend_function_dst_rgb);
}
void H3dMaterialExState::SetBlendFunctionDestinationAlpha(H3dBlendExConst::Factor factor)
{
  m_blend_info.blend_function_dst_alpha = factor;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
}

H3dBlendExConst::Equation H3dMaterialExState::GetBlendEquationRgb(void) const
{
  return static_cast<H3dBlendExConst::Equation>(m_blend_info.blend_equation_rgb);
}
void H3dMaterialExState::SetBlendEquationRgb(H3dBlendExConst::Equation equation)
{
  m_blend_info.blend_equation_rgb = equation;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;

  if( m_blend_info.blend_mode == H3dBlendExConst::MODE_BLEND )
  {
    SetBlendEquationAlpha(equation);
  }
}
H3dBlendExConst::Equation H3dMaterialExState::GetBlendEquationAlpha(void) const
{
  return static_cast<H3dBlendExConst::Equation>(m_blend_info.blend_equation_alpha);
}
void H3dMaterialExState::SetBlendEquationAlpha(H3dBlendExConst::Equation equation)
{
  m_blend_info.blend_equation_alpha = equation;
  m_blend_info.bitflag |= BlendInfo::BLEND_FUNCTION_MUTABLE;
  this->m_buffer_read_info.fColorRead = true;
}

//------------------------ 緊急対応 ------------------------------------------------------
void H3dMaterialExState::InitializeTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
}
void H3dMaterialExState::SetTextureSamplerWrapU(s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
  m_tex_sampler_info[no].wrap_u = wrap;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::WRAP_MUTABLE;
}
H3dTextureSamplerExConst::Wrap  H3dMaterialExState::GetTextureSamplerWrapU(s32 no) const
{
  return static_cast<H3dTextureSamplerExConst::Wrap>(m_tex_sampler_info[no].wrap_u);
}

void H3dMaterialExState::InitializeTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
}
void H3dMaterialExState::SetTextureSamplerWrapV(s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
  m_tex_sampler_info[no].wrap_v = wrap;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::WRAP_MUTABLE;
}
H3dTextureSamplerExConst::Wrap  H3dMaterialExState::GetTextureSamplerWrapV(s32 no) const
{
  return static_cast<H3dTextureSamplerExConst::Wrap>(m_tex_sampler_info[no].wrap_v);
}

void H3dMaterialExState::SetTextureMipMapLodMinLevel( s32 no, s32 level )
{
  m_tex_sampler_info[no].min_lod_level = level;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::FILTER_MUTABLE;
}

void H3dMaterialExState::SetTextureMipMapLodBias( s32 no, s32 bias )
{
  m_tex_sampler_info[no].lod_bias = bias;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::FILTER_MUTABLE;
}


void H3dMaterialExState::InitializeTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable)
{
}
void H3dMaterialExState::SetTextureSamplerTypeEnable(s32 no, H3dTextureSamplerExConst::SamplerTypeEnable enable)
{
}
H3dTextureSamplerExConst::SamplerTypeEnable  H3dMaterialExState::GetTextureSamplerTypeEnable(s32 no) const
{
  // @note 使っていない関数なので、ソース整理時に消そう。
  return H3dTextureSamplerExConst::SAMPLER_TYPE_ENABLE_FALSE;
}


void H3dMaterialExState::InitializeTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type)
{
}
void H3dMaterialExState::SetTextureSamplerType(s32 no, H3dTextureSamplerExConst::SamplerType sampler_type)
{
  m_tex_sampler_info[no].sampler_type = sampler_type;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::SAMPLER_TYPE_MUTABLE;
}
H3dTextureSamplerExConst::SamplerType  H3dMaterialExState::GetTextureSamplerType(s32 no) const
{
  return static_cast<H3dTextureSamplerExConst::SamplerType>(m_tex_sampler_info[no].sampler_type);
}
//res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // テクスチャサンプラータイプを設定します。 テクスチャサンプラータイプが設定可能なのは０番目のテクスチャマッパーのみです。 １、２番目に設定すると正しく動作しません。

void H3dMaterialExState::InitializeTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter)
{
}
void H3dMaterialExState::SetTextureSamplerMagFilter(s32 no, H3dTextureSamplerExConst::MagFilter mag_filter)
{
  m_tex_sampler_info[no].mag_filter = mag_filter;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::FILTER_MUTABLE;
}
H3dTextureSamplerExConst::MagFilter H3dMaterialExState::GetTextureSamplerMagFilter(s32 no) const
{
  return static_cast<H3dTextureSamplerExConst::MagFilter>(m_tex_sampler_info[no].mag_filter);
}

void H3dMaterialExState::InitializeTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter)
{
}
void H3dMaterialExState::InitializeTextureSamplerMinFilterDown(s32 no, H3dTextureSamplerExConst::MinFilterDown min_filter_down)
{
}
void H3dMaterialExState::InitializeTextureSamplerMinFilterUp(s32 no, H3dTextureSamplerExConst::MinFilterUp min_filter_up)
{
}
void H3dMaterialExState::SetTextureSamplerMinFilter(s32 no, H3dTextureSamplerExConst::MinFilter min_filter)
{
  m_tex_sampler_info[no].min_filter = min_filter;
  m_tex_sampler_info[no].bitflag |= TextureSamplerInfo::FILTER_MUTABLE;
}
H3dTextureSamplerExConst::MinFilter H3dMaterialExState::GetTextureSamplerMinFilter(s32 no) const
{
  return static_cast<H3dTextureSamplerExConst::MinFilter>(m_tex_sampler_info[no].min_filter);
}


void H3dMaterialExState::setTextureSamplerMinFilterByUpDown(s32 no)
{
}

//----------------------------------------------------------------------------------------


void H3dMaterialExState::InitializeRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull)
{
  m_raster_info.culling_mode = cull;
}
void H3dMaterialExState::SetRasterizationCullingMode(H3dRasterizationExConst::CullingMode cull)
{
  InitializeRasterizationCullingMode(cull);
  m_raster_info.bitflag |= RasterizationInfo::CULLING_MODE_MUTABLE;
}
H3dRasterizationExConst::CullingMode H3dMaterialExState::GetRasterizationCullingMode(void)
{
  return static_cast<H3dRasterizationExConst::CullingMode>(m_raster_info.culling_mode);
}

void H3dMaterialExState::ResetDirtyBits()
{
  for( s32 step=0; step<H3dTextureCombinerExConst::STEP_NUM; ++step )
  {
    for( s32 src=0; src<H3dTextureCombinerExConst::SOURCE_NUM; ++src )
    {
      m_tex_comb_step[step].source[src].bitflag = 0;
    }
    m_tex_comb_step[step].bitflag = 0;
  }    

  m_stencil_op_info.bitflag = 0;
  m_alpha_test_info.bitflag = 0;
  m_raster_info.bitflag = 0;
  m_depth_color_mask_info.bitflag = 0;
  m_blend_info.bitflag = 0;
    
  for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
  {
    m_tex_sampler_info[i].bitflag = 0;
  }
}

void H3dMaterialExState::MakeMutableCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs)
{ 
  b32 f_buffer_input = false;
  for(s32 step=0; step<H3dTextureCombinerExConst::STEP_NUM; ++step)
  {
    b32 f_operand = false;
    b32 f_source = false;
    for(s32 src=0; src<H3dTextureCombinerExConst::SOURCE_NUM; ++src)
    {
      if(
           m_tex_comb_step[step].source[src].bitflag & TextureCombinerSource::TEXTURE_COMBINER_OPERAND_RGB_MUTABLE
        || m_tex_comb_step[step].source[src].bitflag & TextureCombinerSource::TEXTURE_COMBINER_OPERAND_ALPHA_MUTABLE
      )
      {
        f_operand = true;
      }

      if(
           m_tex_comb_step[step].source[src].bitflag & TextureCombinerSource::TEXTURE_COMBINER_SOURCE_RGB_MUTABLE
        || m_tex_comb_step[step].source[src].bitflag & TextureCombinerSource::TEXTURE_COMBINER_SOURCE_ALPHA_MUTABLE
      )
      {
        f_source = true;
      }
    }

    if( f_operand )
    {
      PicaDataTexEnvOperand ope_rgb[3] =
      {
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[0].operand_rgb),
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[1].operand_rgb),
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[2].operand_rgb),
      };
      PicaDataTexEnvOperand ope_alpha[3] =
      {
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[0].operand_alpha),
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[1].operand_alpha),
        static_cast<PicaDataTexEnvOperand>(m_tex_comb_step[step].source[2].operand_alpha),
      };
      dcs->Put( H3dCommandUtil::MakeTextureCombinerOperandCommand(dcs->GetCurrentPtr(), step, ope_rgb, ope_alpha) );
    }

    if( f_source )
    {
      PicaDataTexEnvSrc source_rgb[3] = 
      {
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[0].source_rgb),
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[1].source_rgb),
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[2].source_rgb),
      };
      PicaDataTexEnvSrc source_alpha[3] =
      {
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[0].source_alpha),
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[1].source_alpha),
        static_cast<PicaDataTexEnvSrc>(m_tex_comb_step[step].source[2].source_alpha),
      };
      dcs->Put( H3dCommandUtil::MakeTextureCombinerSourceCommand(dcs->GetCurrentPtr(), step, source_rgb, source_alpha) );
    }

    if( m_tex_comb_step[step].bitflag & TextureCombinerStep::TEXTURE_COMBINER_COMBINE_RGB_MUTABLE )
    {
      dcs->Put( H3dCommandUtil::MakeTextureCombinerCombineRgbCommand(dcs->GetCurrentPtr(), step, static_cast<PicaDataTexEnvCombine>(m_tex_comb_step[step].combine_rgb)) );
    }
    if( m_tex_comb_step[step].bitflag & TextureCombinerStep::TEXTURE_COMBINER_COMBINE_ALPHA_MUTABLE )
    {
      dcs->Put( H3dCommandUtil::MakeTextureCombinerCombineAlphaCommand(dcs->GetCurrentPtr(), step, static_cast<PicaDataTexEnvCombine>(m_tex_comb_step[step].combine_alpha)) );
    }
    if( m_tex_comb_step[step].bitflag & TextureCombinerStep::TEXTURE_COMBINER_SCALE_RGB_MUTABLE )
    {
      dcs->Put( H3dCommandUtil::MakeTextureCombinerScaleRgbCommand(dcs->GetCurrentPtr(), step, static_cast<PicaDataTexEnvScale>(m_tex_comb_step[step].scale_rgb)) );
    }
    if( m_tex_comb_step[step].bitflag & TextureCombinerStep::TEXTURE_COMBINER_SCALE_ALPHA_MUTABLE )
    {
      dcs->Put( H3dCommandUtil::MakeTextureCombinerScaleRgbCommand(dcs->GetCurrentPtr(), step, static_cast<PicaDataTexEnvScale>(m_tex_comb_step[step].scale_alpha)) );
    }

    if(
         m_tex_comb_step[step].bitflag & TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_RGB_MUTABLE
      || m_tex_comb_step[step].bitflag & TextureCombinerStep::FRAGMENT_SHADER_BUFFER_INPUT_ALPHA_MUTABLE
    )
    {
      f_buffer_input = true;
    }
  }
  
  if( f_buffer_input )
  {
    PicaDataTexEnvBufferInput input_rgb[4] =
    {
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[1].buffer_input_rgb),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[2].buffer_input_rgb),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[3].buffer_input_rgb),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[4].buffer_input_rgb),
    };
    PicaDataTexEnvBufferInput input_alpha[4] =
    {
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[1].buffer_input_alpha),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[2].buffer_input_alpha),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[3].buffer_input_alpha),
      static_cast<PicaDataTexEnvBufferInput>(m_tex_comb_step[4].buffer_input_alpha),
    };
    dcs->Put( H3dCommandUtil::MakeFragmentShaderBufferInputCommand(dcs->GetCurrentPtr(), input_rgb, input_alpha) );
  }

  if( m_stencil_op_info.bitflag & StencilOperationInfo::STENCIL_TEST_MUTABLE )
  {
    dcs->Put( H3dCommandUtil::MakeStencilOperationTestCommand(dcs->GetCurrentPtr(), m_stencil_op_info.enable, static_cast<PicaDataStencilTest>(m_stencil_op_info.func_func), m_stencil_op_info.mask, m_stencil_op_info.func_ref, m_stencil_op_info.func_mask) );
  }
  if( m_stencil_op_info.bitflag & StencilOperationInfo::STENCIL_OP_MUTABLE )
  {
    dcs->Put( H3dCommandUtil::MakeStencilOperationOpCommand(dcs->GetCurrentPtr(), static_cast<PicaDataStencilOp>(m_stencil_op_info.op_fail), static_cast<PicaDataStencilOp>(m_stencil_op_info.op_zfail), static_cast<PicaDataStencilOp>(m_stencil_op_info.op_zpass)) );
  }

  if( m_alpha_test_info.bitflag & AlphaTestInfo::ALPHA_TEST_MUTABLE )
  {
    dcs->Put( H3dCommandUtil::MakeAlphaTestCommand(dcs->GetCurrentPtr(), m_alpha_test_info.enable, static_cast<PicaDataAlphaTest>(m_alpha_test_info.func), m_alpha_test_info.ref) );
  }
  
  //ブレンド等のバッファアクセス関連
  this->makeMutableBufferBlensCommand(*dcs);

  if( m_raster_info.bitflag & RasterizationInfo::CULLING_MODE_MUTABLE )
  {
    dcs->Put( H3dCommandUtil::MakeRasterizationCullingModeCommand(dcs->GetCurrentPtr(), m_raster_info.culling_mode) );
  }
}
    
void H3dMaterialExState::makeMutableBufferBlensCommand( nn::gr::CTR::CommandBufferJumpHelper& dcs ) const
{
  b32               fChangeAccess = false;

  if( m_depth_color_mask_info.bitflag & DepthColorMaskInfo::DEPTH_TEST_MUTABLE )
  {
    fChangeAccess = true;
    dcs.Put( H3dCommandUtil::MakeDepthTestCommand(dcs.GetCurrentPtr(), m_depth_color_mask_info.depth_test_enable,
          static_cast<PicaDataDepthTest>(m_depth_color_mask_info.depth_test_func)) );
  }
  
  if( m_depth_color_mask_info.bitflag & DepthColorMaskInfo::DEPTH_COLOR_MASK_MUTABLE )
  {
    fChangeAccess = true;
    dcs.Put( H3dCommandUtil::MakeDepthColorMaskCommand(dcs.GetCurrentPtr(),
          1,  // 今のところ変更する予定はないので常に1にしておく
          1,  // 今のところ変更する予定はないので常に1にしておく
          1,  // 今のところ変更する予定はないので常に1にしておく
          1,  // 今のところ変更する予定はないので常に1にしておく
          m_depth_color_mask_info.depth_mask) );
  }

  if( m_blend_info.bitflag & BlendInfo::BLEND_MODE_MUTABLE )
  {
    fChangeAccess = true;
    dcs.Put( H3dCommandUtil::MakeBlendModeCommand(dcs.GetCurrentPtr(), (m_blend_info.blend_mode==H3dBlendExConst::MODE_LOGIC)?0:1) );
        // ブレンドモードの種類分の区別があるわけではない、LOGICかそうでないかの2つ区別だけしかない。
  }
  
  if( m_blend_info.bitflag & BlendInfo::BLEND_FUNCTION_MUTABLE )
  {
    fChangeAccess = true;
    dcs.Put( H3dCommandUtil::MakeBlendFunctionCommand(
          dcs.GetCurrentPtr(),
          static_cast<PicaDataBlendEquation>(m_blend_info.blend_equation_rgb),
          static_cast<PicaDataBlendFunc>(m_blend_info.blend_function_src_rgb),
          static_cast<PicaDataBlendFunc>(m_blend_info.blend_function_dst_rgb),
          static_cast<PicaDataBlendEquation>(m_blend_info.blend_equation_alpha),
          static_cast<PicaDataBlendFunc>(m_blend_info.blend_function_src_alpha),
          static_cast<PicaDataBlendFunc>(m_blend_info.blend_function_dst_alpha)
       )
    );
  }
  
  if( m_blend_info.bitflag & BlendInfo::BLEND_LOGIC_OPERATION_MUTABLE )
  {
    fChangeAccess = true;
    dcs.Put( H3dCommandUtil::MakeBlendLogicOperationCommand(dcs.GetCurrentPtr(), static_cast<PicaDataLogicOp>(m_blend_info.logic_operation)) );
  }
  
  if ( fChangeAccess )
  {
    u32 s_AddCommands[] =
    {
      // フレームバッファアクセス制御
      NW_FONT_CMD_SET_FBACCESS(
          m_buffer_read_info.fColorRead,
          m_buffer_read_info.fColorWrite,
          m_buffer_read_info.fDepthRead,
          m_buffer_read_info.fDepthWrite,
          m_buffer_read_info.fStencilRead,
          m_buffer_read_info.fStencilWrite )
    };
    
    bit32     *p_command = dcs.GetCurrentPtr();
    
    for( s32 i = 0; i < sizeof(s_AddCommands) / sizeof(u32); ++i )
    {
      *p_command = s_AddCommands[i];
      ++p_command;
    }
    dcs.Put( p_command );
  }
}

// SDK 5.0対応
void H3dMaterialExState::MakeCombinerColorCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState) const
{
  for( s32 step = 0; step < H3dTextureCombinerExConst::STEP_NUM; ++step )
  {
    using namespace nw;
    using namespace nw::h3d;

    s32     constantNo = this->GetMaterialTextureCombinerConstant( step );

    if ( constantNo == -1 )
      continue;

    static const u32      s_RegNos[] = { PICA_REG_TEX_ENV0, PICA_REG_TEX_ENV1, PICA_REG_TEX_ENV2, PICA_REG_TEX_ENV3, PICA_REG_TEX_ENV4, PICA_REG_TEX_ENV5 };

    gfl::grp::ColorU8 l_c;  // nw::ut::Colorはnw::ut::Color8

    //l_c = materialState->GetConstantColor(constantNo);
    l_c = this->GetMaterialTextureCombinerConstantColor( step );
    
    //コンバイナカラー地下指定
    dcs->Put( fnd::Util::MakeCombinerConstantCommand(dcs->GetCurrentPtr(), s_RegNos[step], l_c) );
  }
}

#if 0

void H3dMaterialExState::MakeTextureSamplerCommand(H3dCommand* command, const nw::h3d::MaterialState* materialState, const nw::h3d::res::TextureSamplerData* textureSamplers) const
{
  nn::gr::CTR::CommandBufferJumpHelper&  dcs  =  *(command->GetNwDrawCommandStream());
  
  b32 f_texture_sampler_type = false;
  for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
  {
    if( m_tex_sampler_info[i].bitflag & TextureSamplerInfo::SAMPLER_TYPE_MUTABLE )
    {
      f_texture_sampler_type = true;
      break;
    }
  }
  if( f_texture_sampler_type )
  {
    dcs.Put( H3dCommandUtil::MakeTextureSamplerTypeCommand( dcs.Get(),
          static_cast<PicaDataTexture0SamplerType>(m_tex_sampler_info[0].sampler_type),
          static_cast<PicaDataTexture1SamplerType>(m_tex_sampler_info[1].sampler_type),
          static_cast<PicaDataTexture2SamplerType>(m_tex_sampler_info[2].sampler_type),
          m_tex_sampler_info[0].min_filter_up )
    );
  }

#if 0
  for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
  {
    if( m_tex_sampler_info[i].bitflag & TextureSamplerInfo::WRAP_MUTABLE )
    {
      dcs.Put( H3dCommandUtil::MakeTextureSamplerWrapCommand( dcs.Get(), i,
          static_cast<PicaDataTextureWrap>(m_tex_sampler_info[i].wrap_u),
          static_cast<PicaDataTextureWrap>(m_tex_sampler_info[i].wrap_v) )
      );
    }
    if( m_tex_sampler_info[i].bitflag & TextureSamplerInfo::FILTER_MUTABLE )
    {
      dcs.Put( H3dCommandUtil::MakeTextureSamplerFilterCommand( dcs.Get(), i,
          static_cast<PicaDataTextureMagFilter>(m_tex_sampler_info[i].mag_filter),
          m_tex_sampler_info[i].min_filter_up,
          m_tex_sampler_info[i].min_filter_down )
      );
    }
  }
#else

  const nw::h3d::MaterialTextureFlag::enum_t etcFlags[H3dTextureSamplerExConst::TEXTURE_NUM] =
  {
    nw::h3d::MaterialTextureFlag::TEXTURE0_IS_ETC,
    nw::h3d::MaterialTextureFlag::TEXTURE1_IS_ETC,
    nw::h3d::MaterialTextureFlag::TEXTURE2_IS_ETC,
  };

  for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
  {
    if(    m_tex_sampler_info[i].bitflag & TextureSamplerInfo::WRAP_MUTABLE
        || m_tex_sampler_info[i].bitflag & TextureSamplerInfo::FILTER_MUTABLE )
    {
      dcs.Put( H3dCommandUtil::MakeTextureSamplerWrapFilterCommand(
              dcs.Get(), i,
              static_cast<PicaDataTextureMagFilter>(m_tex_sampler_info[i].mag_filter),
              static_cast<PicaDataTextureMinFilter>(m_tex_sampler_info[i].min_filter),
              materialState->TestTextureFlagsAnd(etcFlags[i]) ? PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP : PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE,
              static_cast<PicaDataTextureWrap>(m_tex_sampler_info[i].wrap_u),
              static_cast<PicaDataTextureWrap>(m_tex_sampler_info[i].wrap_v),
              m_tex_sampler_info[i].sampler_type
          )
      );
    }
  }

#endif
}

#else
// 緊急対応

void H3dMaterialExState::MakeTextureSamplerCommand(nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState* materialState) const
{
  bit32* command = dcs->GetCurrentPtr();
  b32 make_command_flag = false;

  const nw::h3d::MaterialTextureFlag::enum_t etcFlags[H3dTextureSamplerExConst::TEXTURE_NUM] =
  {
    nw::h3d::MaterialTextureFlag::TEXTURE0_IS_ETC,
    nw::h3d::MaterialTextureFlag::TEXTURE1_IS_ETC,
    nw::h3d::MaterialTextureFlag::TEXTURE2_IS_ETC,
  };
  const u32 wrapFilerAddr[H3dTextureSamplerExConst::TEXTURE_NUM] =
  {
    PICA_REG_TEXTURE0_WRAP_FILTER, 
    PICA_REG_TEXTURE1_WRAP_FILTER, 
    PICA_REG_TEXTURE2_WRAP_FILTER, 
  };
  const u32 lodAddr[H3dTextureSamplerExConst::TEXTURE_NUM] =
  {
    PICA_REG_TEXTURE0_LOD,
    PICA_REG_TEXTURE1_LOD,
    PICA_REG_TEXTURE2_LOD,
  };
  const s32 mipmapSize = 1;

  for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
  {
    if(
         m_tex_sampler_info[i].bitflag & TextureSamplerInfo::SAMPLER_TYPE_MUTABLE
      || m_tex_sampler_info[i].bitflag & TextureSamplerInfo::WRAP_MUTABLE
      || m_tex_sampler_info[i].bitflag & TextureSamplerInfo::FILTER_MUTABLE
    )
    {
      // 0x083, 0x093, 0x09b
      *command++ = PICA_CMD_DATA_TEXTURE_WRAP_FILTER(
          m_tex_sampler_info[i].mag_filter,
          m_tex_sampler_info[i].min_filter,
          materialState->TestTextureFlagsAnd(etcFlags[i]) ? PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP : PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE,
          m_tex_sampler_info[i].wrap_u,
          m_tex_sampler_info[i].wrap_v,
          0,
          m_tex_sampler_info[i].sampler_type);
      *command++ = PICA_CMD_HEADER_SINGLE(wrapFilerAddr[i]);
      make_command_flag = true;

      if ((m_tex_sampler_info[i].min_filter != PICA_DATA_TEXTURE_MIN_FILTER_LINEAR) &&
          (m_tex_sampler_info[i].min_filter != PICA_DATA_TEXTURE_MIN_FILTER_NEAREST))
      {
          // 0x084, 0x094, 0x09c
          NW_MINMAX_ASSERT(m_tex_sampler_info[i].lod_bias, -16.0f, 16.0f);
          *command++ = PICA_CMD_DATA_TEXTURE_LOD_LEVEL(
              nn::gr::Float32ToFix13Fraction8(m_tex_sampler_info[i].lod_bias),
              nw::ut::Max(m_tex_sampler_info[i].mipmapSize - 1, 0),
              m_tex_sampler_info[i].min_lod_level);
          *command++ = PICA_CMD_HEADER_SINGLE(lodAddr[i]);
          make_command_flag = true;
      }
    }
  }

  if( make_command_flag ) 
  {
    dcs->Put(command);
  }
}

#endif


void H3dMaterialExState::constructorThis(void)
{
  for(s32 step=0; step<H3dTextureCombinerExConst::STEP_NUM; ++step)
  {
    for(s32 src=0; src<H3dTextureCombinerExConst::SOURCE_NUM; ++src)
    {
      m_tex_comb_step[step].source[src].bitflag = 0;
          // m_tex_comb_step[step].source[src].bitflag = 0xFFFFFFFF;  こんな方法では確認できない。
          // 正しく初期値を設定できているか確認するとき全更新フラグをたてるとかすると、
          // もともとのリソースで値が設定されていないものの更新フラグまでたってしまうのでよくない。

      m_tex_comb_step[step].source[src].operand_rgb    = 0;
      m_tex_comb_step[step].source[src].operand_alpha  = 0;
      m_tex_comb_step[step].source[src].source_rgb     = 0;
      m_tex_comb_step[step].source[src].source_alpha   = 0;
    }

    m_tex_comb_step[step].bitflag = 0;
    
    m_tex_comb_step[step].combine_rgb         = 0;
    m_tex_comb_step[step].combine_alpha       = 0;
    m_tex_comb_step[step].scale_rgb           = 0;
    m_tex_comb_step[step].scale_alpha         = 0;
    m_tex_comb_step[step].buffer_input_rgb    = 0;
    m_tex_comb_step[step].buffer_input_alpha  = 0;
  }

  {
    m_stencil_op_info.bitflag   = 0;
    
    m_stencil_op_info.enable    = 0;
    m_stencil_op_info.func_func = 0;
    m_stencil_op_info.mask      = 0;
    m_stencil_op_info.func_ref  = 0;
    m_stencil_op_info.func_mask = 0;
    m_stencil_op_info.op_fail   = 0;
    m_stencil_op_info.op_zfail  = 0;
    m_stencil_op_info.op_zpass  = 0;
  }

  {
    m_alpha_test_info.bitflag = 0;
    
    m_alpha_test_info.enable  = 0;
    m_alpha_test_info.func    = 0;
    m_alpha_test_info.ref     = 0;
  }

  {
    m_depth_color_mask_info.bitflag           = 0;
    m_depth_color_mask_info.depth_test_enable = 0;
    m_depth_color_mask_info.depth_test_func   = 0;
    m_depth_color_mask_info.red_mask          = 0;
    m_depth_color_mask_info.green_mask        = 0;
    m_depth_color_mask_info.blue_mask         = 0;
    m_depth_color_mask_info.alpha_mask        = 0;
    m_depth_color_mask_info.depth_mask        = 0;
  }

  {
    m_blend_info.bitflag                  = 0;
    m_blend_info.blend_mode               = 0;
    m_blend_info.blend_equation_rgb       = 0;
    m_blend_info.blend_equation_alpha     = 0;
    m_blend_info.blend_function_src_rgb   = 0;
    m_blend_info.blend_function_src_alpha = 0;
    m_blend_info.blend_function_dst_rgb   = 0;
    m_blend_info.blend_function_dst_alpha = 0;
    m_blend_info.logic_operation          = 0;
  }

  {
    for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
    {
      m_tex_sampler_info[i].bitflag             = 0;
      m_tex_sampler_info[i].sampler_type_enable = 0;
      m_tex_sampler_info[i].sampler_type        = 0;
      m_tex_sampler_info[i].wrap_u              = 0;
      m_tex_sampler_info[i].wrap_v              = 0;
      m_tex_sampler_info[i].mag_filter          = 0;
      m_tex_sampler_info[i].min_filter          = 0;
      m_tex_sampler_info[i].min_filter_down     = 0;
      m_tex_sampler_info[i].min_filter_up       = 0;
      m_tex_sampler_info[i].mag_lod_level       = 0;
      m_tex_sampler_info[i].min_lod_level       = 0;
      m_tex_sampler_info[i].lod_bias            = 0.0f;    
    }
  }
  
  for( s32 i = 0; i < H3dTextureCombinerExConst::STEP_NUM; ++i )
    m_MaterialTextureCombinerConstantNo[i] = -1;

  {
    m_raster_info.bitflag = 0;
    
    m_raster_info.culling_mode = 0;
  }

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
  ResetCommandCount();
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
}

void H3dMaterialExState::debugPrint(void)
{
  for(s32 step=0; step<H3dTextureCombinerExConst::STEP_NUM; ++step)
  {
    for(s32 src=0; src<H3dTextureCombinerExConst::SOURCE_NUM; ++src)
    {
      ISHIGURO_PRINT("m_tex_comb_step %d, %d, %d, %d, %d\n",
      m_tex_comb_step[step].source[src].bitflag,
      m_tex_comb_step[step].source[src].operand_rgb,
      m_tex_comb_step[step].source[src].operand_alpha,
      m_tex_comb_step[step].source[src].source_rgb,
      m_tex_comb_step[step].source[src].source_alpha );
    }

    ISHIGURO_PRINT("m_tex_comb_step %d, %d, %d, %d, %d, %d, %d\n",
    m_tex_comb_step[step].bitflag,
    m_tex_comb_step[step].combine_rgb,
    m_tex_comb_step[step].combine_alpha,
    m_tex_comb_step[step].scale_rgb,
    m_tex_comb_step[step].scale_alpha,
    m_tex_comb_step[step].buffer_input_rgb,
    m_tex_comb_step[step].buffer_input_alpha );
  }

  {
    ISHIGURO_PRINT("m_stencil_op_info %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
    m_stencil_op_info.bitflag,
    m_stencil_op_info.enable,
    m_stencil_op_info.func_func,
    m_stencil_op_info.mask,
    m_stencil_op_info.func_ref,
    m_stencil_op_info.func_mask,
    m_stencil_op_info.op_fail,
    m_stencil_op_info.op_zfail,
    m_stencil_op_info.op_zpass );
  }

  {
    ISHIGURO_PRINT("m_alpha_test_info %d, %d, %d, %d\n",
    m_alpha_test_info.bitflag,
    m_alpha_test_info.enable,
    m_alpha_test_info.func,
    m_alpha_test_info.ref );
  }

  {
    ISHIGURO_PRINT("m_depth_color_mask_info %d, %d, %d, %d, %d, %d, %d, %d\n",
    m_depth_color_mask_info.bitflag,
    m_depth_color_mask_info.depth_test_enable,
    m_depth_color_mask_info.depth_test_func,
    m_depth_color_mask_info.red_mask,
    m_depth_color_mask_info.green_mask,
    m_depth_color_mask_info.blue_mask,
    m_depth_color_mask_info.alpha_mask,
    m_depth_color_mask_info.depth_mask );
  }

  {
    ISHIGURO_PRINT("m_blend_info %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
    m_blend_info.bitflag,
    m_blend_info.blend_mode,
    m_blend_info.blend_equation_rgb,
    m_blend_info.blend_equation_alpha,
    m_blend_info.blend_function_src_rgb,
    m_blend_info.blend_function_src_alpha,
    m_blend_info.blend_function_dst_rgb,
    m_blend_info.blend_function_dst_alpha,
    m_blend_info.logic_operation );
  }

  {
    for(s32 i=0; i<H3dTextureSamplerExConst::TEXTURE_NUM; ++i)
    {
      ISHIGURO_PRINT("m_tex_sampler_info %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f\n",
      m_tex_sampler_info[i].bitflag,
      m_tex_sampler_info[i].sampler_type_enable,
      m_tex_sampler_info[i].sampler_type,
      m_tex_sampler_info[i].wrap_u,
      m_tex_sampler_info[i].wrap_v,
      m_tex_sampler_info[i].mag_filter,
      m_tex_sampler_info[i].min_filter,
      m_tex_sampler_info[i].min_filter_down,
      m_tex_sampler_info[i].min_filter_up,
      m_tex_sampler_info[i].mag_lod_level,
      m_tex_sampler_info[i].min_lod_level,
      m_tex_sampler_info[i].lod_bias );
    }
  }
  
  for( s32 i = 0; i < H3dTextureCombinerExConst::STEP_NUM; ++i )
  {
    ISHIGURO_PRINT("%d\n", m_MaterialTextureCombinerConstantNo[i] );
  }

  {
    ISHIGURO_PRINT("m_raster_info %d, %d\n",
    m_raster_info.bitflag,
    m_raster_info.culling_mode );
  }
}

void H3dMaterialExState::Initialize(gfl::heap::NwAllocator* device_allocator, const nw::h3d::res::ModelContent* model, s32 materialIndex )
{
  const nw::h3d::res::MaterialContent* materialContent = model->materials[ materialIndex ].content;
/*
  // 全コマンドダンプしてみる
  GFL_PRINT("-------- MaterialContent paramCmd --------\n");
  if( cont->name ) GFL_PRINT("    Name=%s\n", cont->name);
  else             GFL_PRINT("    No name\n");
  //H3dCommandUtil::DumpCommandEasy(cont->paramCmd);
  //H3dCommandUtil::DumpCommandEasy(cont->shaderContent->shaderFullCmd);
*/
  
#if 1
  {
    b32     fSetUp = false;

    if (materialContent->metaData)
    {
      bit32       command = 0;
      
      int modIdx = materialContent->metaData->FindIndex("PrivateMod");

      if ( modIdx != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
      {
        const nw::h3d::res::MetaData& metaData = materialContent->metaData->At(modIdx);

        //ISHIGURO_PRINT("##### HasFragOp = %d, CullFace = %d\n", ModHelper::HasFragmentOpData(metaData), ModHelper::GetCullFace(metaData));
        static const H3dRasterizationExConst::CullingMode       cullTypes[] = {
          H3dRasterizationExConst::CULLINGMODE_FRONT,
          H3dRasterizationExConst::CULLINGMODE_BACK,
          H3dRasterizationExConst::CULLINGMODE_NONE
        };
        
        this->InitializeRasterizationCullingMode( cullTypes[ ModHelper::GetCullFace(metaData) ] );

        for (int step = 0; step < nw::h3d::Constants::COMBINER_QUANTITY; ++step)
        {
          bit32* combinerCmd = ModHelper::GetCombinerCmd(metaData, materialContent, step);
          if (combinerCmd)
          {
            command = ModHelper::GetCombinerCombine(combinerCmd);
            this->InitializeTextureCombinerCombineRgb(step, static_cast<H3dTextureCombinerExConst::Combine>((command&0x0000FFFF)>>0));
            this->InitializeTextureCombinerCombineAlpha(step, static_cast<H3dTextureCombinerExConst::Combine>((command&0xFFFF0000)>>16));
            
            command = ModHelper::GetCombinerScale(combinerCmd);
            this->InitializeTextureCombinerScaleRgb(step, static_cast<H3dTextureCombinerExConst::Scale>((command&0x0000FFFF)>>0));
            this->InitializeTextureCombinerScaleAlpha(step, static_cast<H3dTextureCombinerExConst::Scale>((command&0xFFFF0000)>>16));
            
            command = ModHelper::GetCombinerOperand(combinerCmd);
            for(s32 no=0; no<3; ++no)
            {
              {
                this->InitializeTextureCombinerOperandRgb(step, no,
                    static_cast<H3dTextureCombinerExConst::OperandRgb>( ( command & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*no) ) ) >> (PICA_TEX_ENV_OPE_RGB1_SHIFT*no) )
                );
              } 
              s32 ano = no + 3;
              {
                this->InitializeTextureCombinerOperandAlpha(step, no,
                    static_cast<H3dTextureCombinerExConst::OperandAlpha>( ( command & ( 0xF<<(PICA_TEX_ENV_OPE_RGB1_SHIFT*ano) ) ) >> (PICA_TEX_ENV_OPE_RGB1_SHIFT*ano) )
                );
              }
            }
            
            command = ModHelper::GetCombinerSource(combinerCmd);
            for(s32 no=0; no<3; ++no)
            {
              {
                this->InitializeTextureCombinerSourceRgb(step, no,
                    static_cast<H3dTextureCombinerExConst::Source>( ( command & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*no) ) ) >> (PICA_TEX_ENV_SRC_RGB1_SHIFT*no) )
                );
              }
              s32 ano = no + 4;
              {
                this->InitializeTextureCombinerSourceAlpha(step, no,
                    static_cast<H3dTextureCombinerExConst::Source>( ( command & ( 0xF<<(PICA_TEX_ENV_SRC_RGB1_SHIFT*ano) ) ) >> (PICA_TEX_ENV_SRC_RGB1_SHIFT*ano) )
                );
              }
            }

            /*ISHIGURO_PRINT("#### Combiner %d : Src = 0x%08x, Ope = 0x%08x, Cmb = 0x%08x, Cst = 0x%08x, Scl = 0x%08x\n",
              cidx,
              //ModHelper::GetCombinerSource(combinerCmd),
              //ModHelper::GetCombinerOperand(combinerCmd),
              //ModHelper::GetCombinerCombine(combinerCmd),
              ModHelper::GetCombinerConst(combinerCmd),
              //ModHelper::GetCombinerScale(combinerCmd));*/
          }
        }

        bit32* bufferCmd = ModHelper::GetCombinerBufferCmd(metaData, materialContent);
        if (bufferCmd)
        {
          command = ModHelper::GetCombinerBufferInput(bufferCmd);
          //command = ModHelper::GetCombinerBufferColor(bufferCmd);
          for(s32 i=0; i<4; ++i)
          {
            {
              this->InitializeFragmentShaderBufferInputRgb( i+1, static_cast<H3dFragmentShaderExConst::BufferInput>( ( command & (1<<(8+i)) ) >> (8+i) ) );
            }
            s32 ai = i+4;
            {
              this->InitializeFragmentShaderBufferInputAlpha( i+1, static_cast<H3dFragmentShaderExConst::BufferInput>( ( command & (1<<(8+ai)) ) >> (8+ai) ) );
            }
          }
          
          //ISHIGURO_PRINT("#### Combiner Buffer : Inp = 0x%08x, Col = 0x%08x\n", ModHelper::GetCombinerBufferInput(bufferCmd), ModHelper::GetCombinerBufferColor(bufferCmd));
        }

        bit32* stencilCmd = ModHelper::GetStencilCmd(metaData, materialContent);
        if (stencilCmd)
        {
          //ISHIGURO_PRINT("#### Stencil : Tst = 0x%08x", ModHelper::GetStencilTest(bufferCmd));
          command = ModHelper::GetStencilTest(stencilCmd);
          this->InitializeStencilOperationTest( (command&1), static_cast<H3dStencilOperationExConst::TestFunc>((command&0xF0)>>4), ((command&0xFF00)>>8), ((command&0xFF0000)>>16), ((command&0xFF000000)>>24) );
          
          if (nw::h3d::fnd::Optional<bit32> sop = ModHelper::GetStencilOp(stencilCmd))
          {
            command = sop.Get();
            this->InitializeStencilOperationOp( static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF)>>0), static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF0)>>4), static_cast<H3dStencilOperationExConst::StencilOp>((command&0xF00)>>8) );
          }
          //ISHIGURO_PRINT("\n");
        }

        bit32* alphaTest = ModHelper::GetAlphaTestCmd(metaData, materialContent);
        if (alphaTest)
        {
          command = ModHelper::GetAlphaTest(alphaTest);
          this->InitializeAlphaTest( (command&1), static_cast<H3dAlphaTestExConst::TestFunc>((command&0xF0)>>4), ((command&0xFF00)>>8) );
          //ISHIGURO_PRINT("#### Alpha : Tst = 0x%08x\n", ModHelper::GetAlphaTest(alphaTest));
        }

        bit32* depthColorMaskCmd = ModHelper::GetDepthColorMaskCmd(metaData, materialContent);
        if (depthColorMaskCmd)
        {
          command = ModHelper::GetDepthColorMask(depthColorMaskCmd);
          this->InitializeDepthTest( (command&1), ((command&0x1000)!=0)?1:0, static_cast<H3dDepthTestExConst::TestFunc>((command&0x70)>>4) );
          //ISHIGURO_PRINT("#### Depth Color Mask : Msk = 0x%08x\n", ModHelper::GetDepthColorMask(depthColorMaskCmd));
        }

        bit32* blendLogicOpCmd = ModHelper::GetBlendLogicOpCmd(metaData, materialContent);
        if (blendLogicOpCmd)
        {
          //ISHIGURO_PRINT("#### Blend Logic : Cop = 0x%08x, Bfn = 0x%08x", ModHelper::GetColorOp(blendLogicOpCmd), ModHelper::GetBlendFunc(blendLogicOpCmd));
          
          {
            command = ModHelper::GetColorOp(blendLogicOpCmd);
            if((command&0x100))
            {
              // ブレンドが有効
              this->InitializeBlendMode(H3dBlendExConst::MODE_SEPARATE_BLEND);
            }
            else
            {
              // 論理演算が有効
              this->InitializeBlendMode(H3dBlendExConst::MODE_LOGIC);
            }
          }
          
          command = ModHelper::GetBlendFunc(blendLogicOpCmd);          
          this->InitializeBlendFunction(
              static_cast<H3dBlendExConst::Equation>((command&0x7)>>0),
              static_cast<H3dBlendExConst::Factor>((command&0xF0000)>>16),
              static_cast<H3dBlendExConst::Factor>((command&0xF00000)>>20),
              static_cast<H3dBlendExConst::Equation>((command&0x700)>>8),
              static_cast<H3dBlendExConst::Factor>((command&0xF000000)>>24),
              static_cast<H3dBlendExConst::Factor>((command&0xF0000000)>>28)
          );

          if (nw::h3d::fnd::Optional<bit32> lop = ModHelper::GetLogicOp(blendLogicOpCmd))
          {
            command = lop.Get();
            this->InitializeBlendLogicOperation(static_cast<H3dBlendExConst::LogicOperation>(command&0xF));
            //ISHIGURO_PRINT(", Lop = 0x%08x", lop.Get());
          }
          
          /*if (nw::h3d::fnd::Optional<bit32> bco = ModHelper::GetBlendColor(blendLogicOpCmd))
          {
            ISHIGURO_PRINT(", Bco = 0x%08x", bco.Get());
          }*/
          //ISHIGURO_PRINT("\n");
        }
        
        {
          command = materialContent->GetColorBufferRead();
          b8 fColorRead = ( command & 0xF ) ? true : false;
          this->InitializeColorBufferRead( fColorRead );
        }
        
        {
          command = materialContent->GetColorBufferWrite();
          b8 fColorWrite = ( command & 0xF ) ? true : false;
          this->InitializeColorBufferWrite( fColorWrite );
        }
        
        {
          command = materialContent->GetDepthStencilBufferRead();
          b8 fDepthRead = ( command & (1<<1) ) ? true : false;
          b8 fStencilRead = ( command & (1<<0) ) ? true : false;
          this->InitializeDepthBufferRead( fDepthRead, fStencilRead );
        }
        
        {
          command = materialContent->GetDepthStencilBufferWrite();
          b8 fDepthWrite = ( command & (1<<1) ) ? true : false;
          b8 fStencilWrite = ( command & (1<<0) ) ? true : false;
          this->InitializeDepthBufferWrite( fDepthWrite, fStencilWrite );
        }
        
        fSetUp = true;
      }
    }
    
    if ( fSetUp == false )
    {//古いデータ
      H3dCommandListenerReadMaterial listener(this);
      H3dCommandUtil::ScanCommand(reinterpret_cast<const bit32*>(materialContent->paramCmd.GetBeginAddress()), materialContent->paramCmd.size(), &listener);
    }
  }
#endif
  
  // 全ての設定が終わった後に呼ぶ初期
  InitializeBlendAfterAllSet();
}

void H3dMaterialExState::InitializeTextureSampler(s32 no, const nw::h3d::res::TextureSamplerData* textureSampler, s32 mipmapSize )
{
#if 0
  InitializeTextureSamplerWrapU(no, static_cast<H3dTextureSamplerExConst::Wrap>(textureSampler->wrapS));
  InitializeTextureSamplerWrapV(no, static_cast<H3dTextureSamplerExConst::Wrap>(textureSampler->wrapT));

  if( no == 0 )  // @note 0と12でちゃんとsamplerTypeに入れている値の型が違ってそうか、テストデータをつくって確認したほうが安心できる。
                 //       テクスチャを3枚も使っていない場合の値も、テストデータをつくって確認したほうが安心できる。→コメントアウト内の使えないソースのことなので放置する。
  {
    if( textureSampler->samplerType == H3dTextureSamplerExConst::SAMPLER_TYPE_FALSE )
    {
      InitializeTextureSamplerTypeEnable(no, H3dTextureSamplerExConst::SAMPLER_TYPE_ENABLE_FALSE);
      InitializeTextureSamplerType(no, static_cast<H3dTextureSamplerExConst::SamplerType>(textureSampler->samplerType));
    }
    else
    {
      InitializeTextureSamplerTypeEnable(no, H3dTextureSamplerExConst::SAMPLER_TYPE_ENABLE_TEXTURE_2D);
      InitializeTextureSamplerType(no, static_cast<H3dTextureSamplerExConst::SamplerType>(textureSampler->samplerType));
    }
  }
  else  // no = 1 or no = 2
  {
    if( textureSampler->samplerType == H3dTextureSamplerExConst::SAMPLER_TYPE_ENABLE_FALSE )
    {
      InitializeTextureSamplerTypeEnable(no, static_cast<H3dTextureSamplerExConst::SamplerTypeEnable>(textureSampler->samplerType));
      InitializeTextureSamplerType(no, H3dTextureSamplerExConst::SAMPLER_TYPE_FALSE);
    }
    else
    {
      InitializeTextureSamplerTypeEnable(no, static_cast<H3dTextureSamplerExConst::SamplerTypeEnable>(textureSampler->samplerType));
      InitializeTextureSamplerType(no, H3dTextureSamplerExConst::SAMPLER_TYPE_TEXTURE_2D);
    }
  }

  InitializeTextureSamplerMagFilter(no, static_cast<H3dTextureSamplerExConst::MagFilter>(textureSampler->magFilter));
  InitializeTextureSamplerMinFilter(no, static_cast<H3dTextureSamplerExConst::MinFilter>(textureSampler->minFilter));
#else
  // 緊急対応
  m_tex_sampler_info[no].sampler_type    = textureSampler->samplerType;
  m_tex_sampler_info[no].wrap_u          = textureSampler->wrapS;
  m_tex_sampler_info[no].wrap_v          = textureSampler->wrapT;
  m_tex_sampler_info[no].mag_filter      = textureSampler->magFilter;
  m_tex_sampler_info[no].min_filter      = textureSampler->minFilter;
  m_tex_sampler_info[no].min_lod_level   = textureSampler->minLodLevel;
  m_tex_sampler_info[no].lod_bias        = textureSampler->lodBias;
  m_tex_sampler_info[no].mipmapSize      = mipmapSize;
#endif
}


#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
H3dMaterialExState::CommandCount H3dMaterialExState::m_mutable_command_max_set;
H3dMaterialExState::CommandCount H3dMaterialExState::m_tex_sampler_command_max_set;
H3dMaterialExState::CommandCount H3dMaterialExState::m_command_curr_set;

void H3dMaterialExState::ResetCommandCount(void)
{
  m_mutable_command_max_set.mutable_command = 0;
  m_mutable_command_max_set.tex_sampler_command = 0;

  m_tex_sampler_command_max_set.mutable_command = 0;
  m_tex_sampler_command_max_set.tex_sampler_command = 0;

  m_command_curr_set.mutable_command = 0;
  m_command_curr_set.tex_sampler_command = 0;
}
void H3dMaterialExState::StartOneFrameCommandCount(void)
{
  m_command_curr_set.mutable_command = 0;
  m_command_curr_set.tex_sampler_command = 0;
}
void H3dMaterialExState::EndOneFrameCommandCount(void)
{
  b32 print_flag = false;

  if( m_command_curr_set.mutable_command > m_mutable_command_max_set.mutable_command )
  {
    m_mutable_command_max_set.mutable_command     = m_command_curr_set.mutable_command;
    m_mutable_command_max_set.tex_sampler_command = m_command_curr_set.tex_sampler_command;
    print_flag = true;
  }

  if( m_command_curr_set.tex_sampler_command > m_tex_sampler_command_max_set.tex_sampler_command )
  {
    m_tex_sampler_command_max_set.mutable_command     = m_command_curr_set.mutable_command;
    m_tex_sampler_command_max_set.tex_sampler_command = m_command_curr_set.tex_sampler_command;
    print_flag = true;
  }

  if(print_flag)
  {
    PrintCommandCount();
  }
}
void H3dMaterialExState::PrintCommandCount(void)
{
  GFL_PRINT("[m_mutable_command_max_set]  ");
  printCommandCount(m_mutable_command_max_set);
  GFL_PRINT("[m_tex_sampler_command_max_set]  ");
  printCommandCount(m_tex_sampler_command_max_set);
  GFL_PRINT("[m_command_curr_set]  ");
  printCommandCount(m_command_curr_set);
}
void H3dMaterialExState::printCommandCount(const CommandCount& command_count)
{
  GFL_PRINT("mutable_command=%d, tex_sampler_command=%d\n",
      command_count.mutable_command, command_count.tex_sampler_command);
}
#endif  // #if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

