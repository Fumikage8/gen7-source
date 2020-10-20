
//kawa20150428
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
// コマンドバッファマネージャ
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■

//
//
// コマンドバッファ
//
//
template<size_t BufferSizeMax>
inline CTRCommandBuffer<BufferSizeMax> *CTRCommandBuffer<BufferSizeMax>::Create()
{
  return new(BUFFER_ALIGN) CTRCommandBuffer<BufferSizeMax>();
}

template<size_t BufferSizeMax>
inline CTRCommandBuffer<BufferSizeMax>::CTRCommandBuffer() :
    m_CurrentPtr(m_Buffer)
{
  //STATIC_ASSERTにしたいナリィ
  GFL_ASSERT(BufferSizeMax % BUFFER_ALIGN == 0);  // 16の倍数になっていない。  // 開発中に気付いてもらうためのASSERT
}
template<size_t BufferSizeMax>
inline CTRCommandBuffer<BufferSizeMax>::~CTRCommandBuffer()
{
}

template<size_t BufferSizeMax>
inline size_t CTRCommandBuffer<BufferSizeMax>::GetSize(void) const
{
  // m_Bufferに作成したコマンドのサイズ
  return this->calcSize(m_CurrentPtr);
}
template<size_t BufferSizeMax>
inline const bit32* CTRCommandBuffer<BufferSizeMax>::GetStartPtr(void) const
{
  return m_Buffer;
}
template<size_t BufferSizeMax>
inline bit32* CTRCommandBuffer<BufferSizeMax>::GetCurrentPtr(void) const
{
  return m_CurrentPtr;
}
template<size_t BufferSizeMax>
inline void CTRCommandBuffer<BufferSizeMax>::SetCurrentPtr(bit32* address)
{
  if( this->calcSize(address) <= BufferSizeMax )
  {
    m_CurrentPtr = address;
  }
  else
  {
    GFL_ASSERT(0);  // コマンドバッファからあふれた。  // 開発中に気付いてもらうためのASSERT
  }
}
template<size_t BufferSizeMax>
inline void CTRCommandBuffer<BufferSizeMax>::ResetCurrentPtr(void)
{
  m_CurrentPtr = m_Buffer;
}
template<size_t BufferSizeMax>
inline void CTRCommandBuffer<BufferSizeMax>::AddSubroutineCommand(void)
{
  bit32* command = this->GetCurrentPtr();
  const bit32* head    = this->GetStartPtr();

  // 最終的なバッファサイズが 16 の倍数になるように調整する。
  // 後に追加するキックコマンドが 8 バイトであることを考慮。
  command = nn::gr::AddDummyDataForCommandBuffer( command, (command - head) * sizeof(bit32) + 8 );
  // キックコマンドの追加
  command = nn::gr::MakeChannelKickCommand( command, nn::gr::CTR::CMD_BUF_CH1 );

  this->SetCurrentPtr(command);
}

template<size_t BufferSizeMax>
inline size_t CTRCommandBuffer<BufferSizeMax>::calcSize(bit32* address) const
{
  return ( (address - m_Buffer) * sizeof(bit32) );
}

//
//
// コマンドバッファマネージャ
//
//

////////////////////////////////////////////////////////////////////////////////
// 参考にしたもの
//
// コマンドバッファの参考
// nn::gr::CTR::CommandBufferJumpHelper
// nn::gr::CTR::AddDummyDataForCommandBuffer
//
// ダブルバッファの参考
// nw::h3d::res::MaterialData
// nw::h3d::MaterialState
////////////////////////////////////////////////////////////////////////////////
template<size_t BufferSizeMax>
inline CTRCommandBufferManager<BufferSizeMax> *CTRCommandBufferManager<BufferSizeMax>::Create()
{
  return new(BUFFER_ALIGN) CTRCommandBufferManager<BufferSizeMax>();
}

template<size_t BufferSizeMax>
CTRCommandBufferManager<BufferSizeMax>::CTRCommandBufferManager() :
    m_FrontBufferIndex(0),  // increaseBufferIndexで適切な値を設定するので0でいい
    m_BackBufferIndex(0),   // increaseBufferIndexで適切な値を設定するので0でいい
    m_Flags(0)
{
  GFL_ASSERT( this->GetInstanceAllocator() );//@note newして生成されることが前提

  this->increaseBufferIndex();
}
template<size_t BufferSizeMax>
CTRCommandBufferManager<BufferSizeMax>::~CTRCommandBufferManager()
{
}

template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::ResetCommandBuffer(void)
{
  // 未使用とするだけ。「バッファを0で埋め尽くす」とかはしない。
  // なぜなら「主となるコマンドバッファ」がスワップするまでは描画中かもしれないので。
  // 未使用にした後でコマンドを積み始めるかもしれないので、バッファのインデックスも変更しない。
  m_Flags = 0;
}
template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::SwapCommandBuffer(void)
{
  if(m_Flags & FLAG_BACK_BUFFER_UPDATED)
  {
    this->increaseBufferIndex();
    // 「次のコマンドの書き込み先アドレス」を先頭に戻すだけ。「バッファを0で埋め尽くす」とかはしない。
    // なぜなら「主となるコマンドバッファ」がスワップするまでは描画中かもしれないので。
    m_CommandBuffer[m_BackBufferIndex].ResetCurrentPtr();
    m_Flags |= FLAG_FRONT_BUFFER_USED;
    m_Flags &= (~FLAG_BACK_BUFFER_UPDATED);
  }
}

template<size_t BufferSizeMax>
b32 CTRCommandBufferManager<BufferSizeMax>::IsFrontBufferUsed(void) const
{
  return ( (m_Flags & FLAG_FRONT_BUFFER_USED) != 0 );
}
template<size_t BufferSizeMax>
size_t CTRCommandBufferManager<BufferSizeMax>::GetSizeOfFrontBuffer(void) const
{
  if(m_Flags & FLAG_FRONT_BUFFER_USED)
  {
    return m_CommandBuffer[m_FrontBufferIndex].GetSize();
  }
  else
  {
    return 0;
  }
}
template<size_t BufferSizeMax>
const bit32* CTRCommandBufferManager<BufferSizeMax>::GetStartPtrOfFrontBuffer(void) const
{
  if(m_Flags & FLAG_FRONT_BUFFER_USED)
  {
    return m_CommandBuffer[m_FrontBufferIndex].GetStartPtr();
  }
  else
  {
    return NULL;
  }
}

template<size_t BufferSizeMax>
bit32* CTRCommandBufferManager<BufferSizeMax>::GetCurrentPtrOfBackBuffer(void) const
{
  return m_CommandBuffer[m_BackBufferIndex].GetCurrentPtr();
}
template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::SetCurrentPtrOfBackBuffer(bit32* address)
{
  m_CommandBuffer[m_BackBufferIndex].SetCurrentPtr(address);
  m_Flags |= FLAG_BACK_BUFFER_UPDATED;
}
template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::ResetCurrentPtrOfBackBuffer(void)
{
  m_CommandBuffer[m_BackBufferIndex].ResetCurrentPtr();
  m_Flags &= (~FLAG_BACK_BUFFER_UPDATED);
}
template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::AddSubroutineCommandToBuckBuffer(void)
{
  m_CommandBuffer[m_BackBufferIndex].AddSubroutineCommand();
  m_Flags |= FLAG_BACK_BUFFER_UPDATED;
}

template<size_t BufferSizeMax>
void CTRCommandBufferManager<BufferSizeMax>::increaseBufferIndex(void)
{
  m_FrontBufferIndex = m_BackBufferIndex % BUFFER_NUM;
  m_BackBufferIndex  = (m_BackBufferIndex +1) % BUFFER_NUM;
}