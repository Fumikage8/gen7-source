#if !defined(__ROM_PROTECT_H__)
#define __ROM_PROTECT_H__
#pragma once

#if PM_DEBUG

#if defined(GF_PLATFORM_CTR)

//=============================================================================
/**
 * @file   DebugRomProtect.h
 * @brief  デバッグ用 プロテクト解除操作検証クラス
 * @author Masanori Kanamaru
 * @date   2015.06.08
 */
//=============================================================================


#include <macro/include/gfl2_Macros.h>
#include <ui/include/gfl2_UI.h>


namespace System{
  class RomProtect{
    GFL_FORBID_COPY_AND_ASSIGN(RomProtect);
  private:
    static const u32 DOWN_SECOND = 2;  // 押し込み時間
    static const u32 MISS_COUNT = 20;  // 許容間違い回数
    static const u32 CORRECT_KEY = (gfl2::ui::BUTTON_X|gfl2::ui::BUTTON_B); // ボタン組み合わせ
    static const char* FILE_PATH;

    /**
     * @brief ファイルが存在することの検証
     * @param pHeap FS用一時ヒープ
     */
    bool ExistTheFile(gfl2::heap::HeapBase* pHeap);

  public:
    RomProtect(){}
    ~RomProtect(){};

    /**
     * @brief キー入力の検証
     * @param heap  FS用一時ヒープ
     */
    void KeyCheckLoop(gfl2::heap::HeapBase* pHeap);
  };
} // System

#endif // GF_PLATFORM_CTR

#endif // PM_DEBUG
#endif // __ROM_PROTECT_H__