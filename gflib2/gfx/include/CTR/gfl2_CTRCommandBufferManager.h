#if !defined(GFLIB2_GFX_CTR_CTRCOMMANDBUFFERMANAGER_H_INCLUDED)
#define GFLIB2_GFX_CTR_CTRCOMMANDBUFFERMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_CTRCommandBufferManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.28
 @brief   コマンドバッファマネージャ

 @note    複数のコマンドバッファを管理するCTRCommandBufferManagerクラスと
          コマンドバッファを扱うCTRCommandBufferクラスが記述されている。
          あらかじめコマンドをつくっておきそこにジャンプする、
          繋いだコマンドを更新する(ダブルバッファで実現)、というのが主な使途。
 */
//==============================================================================


#if defined(GF_PLATFORM_CTR)


#include <gfx/include/gfl2_GLMemory.h>


namespace gfl2 { namespace gfx { namespace ctr {
  
//! @brief  コマンドバッファ
template<size_t BufferSizeMax>
class CTRCommandBuffer : public GLMemory
{
private:
  static const u32 BUFFER_ALIGN = 16;  //!< コマンドバッファのアラインメント

private:
  //! @brief  コンストラクタ
  CTRCommandBuffer();

public:
  static CTRCommandBuffer *Create();
  //! @brief  デストラクタ
  ~CTRCommandBuffer();

  //! @brief  「作成したコマンドのサイズ」を取得する(確保したメモリのサイズではない)
  size_t GetSize(void) const;
  //! @brief  コマンドバッファの先頭アドレスを取得する
  const bit32* GetStartPtr(void) const;
  //! @brief  「次のコマンドの書き込み先アドレス」を取得する
  bit32* GetCurrentPtr(void) const;
  //! @brief  「次のコマンドの書き込み先アドレス」を設定する
  void SetCurrentPtr(bit32* address);
  //! @brief  「次のコマンドの書き込み先アドレス」を先頭に戻す
  void ResetCurrentPtr(void);
  //! @brief  サブルーチンとして機能するようなコマンドを書き込む
  void AddSubroutineCommand(void);

private:
  //! @brief  「次のコマンドの書き込み先アドレス」を渡し、そのときの「作成したコマンドのサイズ」を取得する(確保したメモリのサイズではない)
  size_t calcSize(bit32* address) const;

private:
  bit32* m_CurrentPtr;     //!< 次のコマンドの書き込み先アドレス
  bit32  m_Buffer[BufferSizeMax / sizeof(bit32)] __attribute__((aligned(16)));         //!< コマンドバッファ
} __attribute__((aligned(16)));

//kawa20150428
//! @brief  コマンドバッファマネージャ
template<size_t BufferSizeMax>
class CTRCommandBufferManager : public GLMemory
{
  //! @note  使い方
  //!
  //! (1) GetCurrentPtrOfBackBufferでバックバッファのアドレスを取得し、
  //!     バックバッファにコマンドを積んで、
  //!     SetCurrentPtrOfBackBufferでバックバッファのアドレスを進めておく。
  //!     最後にAddSubroutineCommandToBuckBufferで、サブルーチン化する。
  //! (2) SwapCommandBufferでコマンドバッファを入れ替える。
  //!     毎フレーム呼ぶのでもいいし、バックバッファにコマンドを積んだときだけ呼ぶのでもいい。
  //!     スワップ後このフレームではバックバッファにコマンドを積んではならない。
  //!     なぜならスワップで新たにバックバッファになったバッファをまだ描画に使用しているかもしれないから。
  //!     スワップ後「主となるコマンドバッファ」のスワップも完了したらバックバッファにコマンドを積んでもよい。
  //! (3) IsFrontBufferUsedでフロントバッファが使用されているか確認する。
  //!     使用されている場合は、
  //!     コマンドバッファジャンプヘルパーにGetStartPtrOfFrontBufferとGetSizeOfFrontBufferを渡し、
  //!     「主となるコマンドバッファ」にフロントバッファを繋ぐ。
  //!     バックバッファにコマンドを積んでいないフレームでも、スワップを行っていないフレームでも、
  //!     IsFrontBufferUsedがfalseでないときはフロントバッファを繋がなければならない。
  //!
  //! コマンドを積むときの流れを簡潔に書くと
  //! (1) バックバッファにコマンドを積む
  //! (2) スワップしてバックバッファをフロントバッファにする
  //! (3) フロントバッファにジャンプするようにする
  //! となり、
  //! 既に積んであるときの流れを簡潔に書くと
  //! (1) スワップしても何も起きない(バックバッファにコマンドを積んでいないから)
  //! (2) フロントバッファにジャンプするようにする
  //! となる。

private:
  static const u32 BUFFER_NUM =  2;  //!< コマンドバッファの数

  //! @brief  ビットフラグ
  enum Flag
  {
    FLAG_FRONT_BUFFER_USED    =  1 <<  0,  //!< フロントバッファが使用されているか(フロントバッファにコマンドが積まれているか)
    FLAG_BACK_BUFFER_UPDATED  =  1 <<  1,  //!< バックバッファが更新されているか(バックバッファにコマンドが積まれているか)
  };

  static const u32 BUFFER_ALIGN = 16;  //!< コマンドバッファのアラインメント

  //! @brief  コンストラクタ
  CTRCommandBufferManager();

public:
  static CTRCommandBufferManager *Create();

  //! @brief  デストラクタ
  virtual ~CTRCommandBufferManager();

  //! @brief  コマンドバッファを未使用にする
  void ResetCommandBuffer(void);
  //! @brief  コマンドバッファを入れ替える
  void SwapCommandBuffer(void);

  //! @brief  フロントバッファが使用されているか
  b32 IsFrontBufferUsed(void) const;
  //! @brief  フロントバッファにある「作成したコマンドのサイズ」を取得する(確保したメモリのサイズではない)
  //! @note  IsFrontBufferUsedがfalseのときは0を返す
  size_t GetSizeOfFrontBuffer(void) const;
  //! @brief  フロントバッファの先頭アドレスを取得する
  //! @note  IsFrontBufferUsedがfalseのときはNULLを返す
  const bit32* GetStartPtrOfFrontBuffer(void) const;

  //! @brief  バックバッファの「次のコマンドの書き込み先アドレス」を取得する
  bit32* GetCurrentPtrOfBackBuffer(void) const;
  //! @brief  バックバッファの「次のコマンドの書き込み先アドレス」を設定する
  void SetCurrentPtrOfBackBuffer(bit32* address);
  //! @brief  バックバッファの「次のコマンドの書き込み先アドレス」を先頭に戻す
  void ResetCurrentPtrOfBackBuffer(void);
  //! @brief  バックバッファがサブルーチンとして機能するようなコマンドを書き込む
  void AddSubroutineCommandToBuckBuffer(void);

private:
  //! @brief  バッファのインデックスを進める
  void increaseBufferIndex(void);

private:
  u32                     m_FrontBufferIndex;           //!< フロントバッファのインデックス(描画に使用しているバッファ)
  u32                     m_BackBufferIndex;            //!< バックバッファのインデックス(コマンドを積むバッファ)
  CTRCommandBuffer<BufferSizeMax>       m_CommandBuffer[BUFFER_NUM];  //!< コマンドバッファ
  u32                     m_Flags;                      //!< ビットフラグ
};

#include "gfl2_CTRCommandBufferManager.inl"

}}}


#endif  // GF_PLATFORM_CTR


#endif  // GFLIB2_GFX_CTR_CTRCOMMANDBUFFERMANAGER_H_INCLUDED

