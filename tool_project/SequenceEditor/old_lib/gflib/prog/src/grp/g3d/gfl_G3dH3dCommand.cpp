//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dCommand.cpp
 *	@brief  H3Dコマンド
 *	@author	Koji Kawada
 *	@date		2012.04.09
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



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dCommand.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dコマンドクラス
//=====================================

H3dCommand::H3dCommand(void)
  : m_dcs(NULL),
    m_ss(NULL),
    m_begin_addr(NULL),
    m_end_addr(NULL)
{
  gfl::std::MemClear(m_dcs_buff, DCS_BUFF_BYTE_SIZE);
  gfl::std::MemClear(m_ss_buff, SS_BUFF_BYTE_SIZE);
}
H3dCommand::~H3dCommand()
{
  if( (m_dcs) && (m_ss) )
  {
    GFL_ASSERT_MSG(0, "EndDrawが呼ばれていません。\n");
  }
}

void H3dCommand::BegineDraw(void)
{
  if( (!m_dcs) && (!m_ss) )
  {
    flushCommandBuffer();

    bit32* command;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));

    AddressType addr = command;
    m_dcs = GFL_NEW_PLACEMENT(m_dcs_buff) nn::gr::CTR::CommandBufferJumpHelper(addr);  // placement new
        // h3d_Draw.hとh3d_DrawImpl.hのソースを見る限り、アドレス0を渡して初期化しても問題なさそうだ。
        // その場合は使う前にResetで適切なアドレスを渡さなければならないが。

    m_ss = GFL_NEW_PLACEMENT(m_ss_buff) nw::h3d::ShaderState;

    m_begin_addr = addr;
    m_end_addr   = m_begin_addr;  // 全く何も描画しないこともあり得るので、m_begin_addrで初期化しておく。
  }
  else
  {
    GFL_ASSERT_MSG(0, "EndDrawが呼ばれていません。\n");
  }
}

void H3dCommand::EndDraw(void)
{
  if( (m_dcs) && (m_ss) )
  {
    u32 size = m_dcs->GetSize();
    if( size )
    {
      nngxMoveCommandbufferPointer(size);

      u32 first_size = m_dcs->GetSizeToFirstJump();
      if( first_size )
      {
        nngxFlush3DCommandPartially(first_size); // 区切りコマンドを追加（最初のキックコマンドまでのサイズを指定）
      }

      bit32* command;
      nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));

      AddressType addr = command;
      m_dcs->FinalizeJump(addr);  // 最後のコマンドのサイズを書き込むために Reset を行います。

      m_end_addr = addr;
    }

    //updateCommandBuffer();

    m_dcs->~CommandBufferJumpHelper();  // 明示的にデストラクタを呼び出す  // placement deleteは呼ばなくてもいいかな。
    m_ss->~ShaderState();
    m_dcs = NULL;
    m_ss = NULL;
    m_begin_addr = NULL;
    m_end_addr = NULL;
  }
  else
  {
    GFL_ASSERT_MSG(0, "BeginDrawが呼ばれていません。\n");
  }
}

void H3dCommand::flushCommandBuffer(void)
{
  // 他の描画システムのコマンドを作成していた場合、
  // nngxFlush3DCommandNoCacheFlush() を実行して区切りを入れる必要があります。
  // または、nngxFlush3DCommandPartially() の引数のコマンドサイズに
  // 事前のコマンドサイズを加算するようにしてください。
  nngxFlush3DCommandNoCacheFlush();
}

/*
void H3dCommand::updateCommandBuffer(void)
{
  size_t size = (m_end_addr - m_begin_addr) * sizeof(AddressType);  // 引き算後の値はAddressType型の個数なので、sizeofしたのを掛けてバイトサイズにしてやる。
  if( size )
  {
    // 描画コマンドリクエストの生成に nngxFlush3DCommandPartially 関数を使っているのだが、
    // nngxFlush3DCommandPartially 関数はコマンドバッファに対してキャッシュフラッシュを行っていないので、
    // ここで行う。

    // @note コマンドバッファ全体に対してキャッシュフラッシュをしたほうが効率がよい。
    //       それよりも、デバイスメモリ全体に対してキャッシュフラッシュをしたほうが効率がよい。
    //       が、それらをやるには、グラフィックスシステムとかヒープシステムとかをも巻き込むことになるだろうから、今はやっていない。
    //       で、ここで今使ったコマンドバッファにだけキャッシュフラッシュを行うことにしている。
    //       これで、h3d::MaterialDataの領域のキャッシュフラッシュもできているはずだ。
    //       →
    nngxUpdateBuffer(m_begin_addr, size);
  }
}
*/


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

