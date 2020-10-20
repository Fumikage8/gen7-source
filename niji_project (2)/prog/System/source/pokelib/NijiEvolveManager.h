//=============================================================================
/**
 * @brief kujira用ポケモン進化マネージャ
 * @file XYEvolveManager.h
 * @author obata
 * @date 2011.11.21
 */
//=============================================================================
#ifndef __NIJIEVOLVEMANAGER_H__
#define __NIJIEVOLVEMANAGER_H__
#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(PmlUse)

  class NijiEvolveManager : public pml::pokepara::EvolveManager
  {
    public:
      //---------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       */
      NijiEvolveManager( void );

      /**
       * @brief デストラクタ
       */
      virtual ~NijiEvolveManager();



    protected:
      //---------------------------------------------------------------------
      // ゲーム状況のチェック
      //---------------------------------------------------------------------
      /**
       * @brief 指定された道具が「かわらずのいし」かどうかを調べる
       * @param item  チェック対象の道具
       * @retval true   item は「かわらずのいし」
       * @retval false  item は「かわらずのいし」でない
       */
      virtual bool CheckItem_KAWARAZUNOISHI( u32 item ) const;

  };


GFL_NAMESPACE_END(PmlUse)
GFL_NAMESPACE_END(System)


#endif  // __NIJIEVOLVEMANAGER_H__
