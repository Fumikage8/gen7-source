#if PM_DEBUG

#include <Debug/StartMenu/include/d_kanamaru.h>

#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_kanamaru_masanori.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <GameSys/include/GameProcManager.h>

#include <System/include/ArcIdGetter.h>


namespace debug {
  namespace startmenu {

    // 前方宣言
    class KanamaruDynamicHeaderTestProc;

    const u32 KanamaruTestLauncher::ItemList[KanamaruTestLauncher::LIST_MAX] = {
      MS_LABEL_D_KANAMARU_MENU_000,
    };

    KanamaruTestLauncher::KanamaruTestLauncher()
    {}

    KanamaruTestLauncher::~KanamaruTestLauncher()
    {}

    gfl2::fs::ArcFile::ARCDATID KanamaruTestLauncher::GetArcDataID()
    {
      return GARC_debug_message_d_kanamaru_masanori_DAT;
    }

    const u32* KanamaruTestLauncher::GetListTable()
    {
      return KanamaruTestLauncher::ItemList;
    }

    u32 KanamaruTestLauncher::GetListTableSize()
    {
      return KanamaruTestLauncher::LIST_MAX;
    }

    launcher::ListData::RequestID KanamaruTestLauncher::SelectListener( u32 pos, launcher::LauncherProc* launcher)
    {
      // キャンセル
      if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
      {
        launcher->CreateListData<debug::startmenu::StartLauncher>();
        return debug::launcher::ListData::REQID_RESET;
      }

      switch (KanamaruTestLauncher::ItemList[pos])
      {
      case MS_LABEL_D_KANAMARU_MENU_000:
        NIJI_PROC_CALL<GameSys::GameProcManager, KanamaruDynamicHeaderTestProc>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager());
        return debug::launcher::ListData::REQID_CALL;
      }

      return debug::launcher::ListData::REQID_NONE;
    }
  } // startmenu
} // debug



// ----------------------------------------------------------------------
// 以下、動的ヘッダシステムの利用サンプル
// ----------------------------------------------------------------------

// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

#define NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH "niji_conv_header/field/FieldActionEventTypes.h"
#define NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH2 "arc_index/debug_message.gaix"
#define NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH3 "arc_def_index/arc_def.h"

// 動的ヘッダシステムで利用するヘッダは以下のマクロを通してincludeしてください
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH)
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH2)
#include GFL_DYNAMIC_HEADER(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH3)

namespace debug {
  namespace startmenu {

    //==============================================================================
    /**
     @class     KanamaruDynamicHeaderTestProc
     @brief     動的ヘッダシステム利用サンプル
     */
    //==============================================================================
    class KanamaruDynamicHeaderTestProc : public GameSys::GameProc
    {
      GFL_FORBID_COPY_AND_ASSIGN(KanamaruDynamicHeaderTestProc);
    public:
      KanamaruDynamicHeaderTestProc(){}
      virtual ~KanamaruDynamicHeaderTestProc(){}

      virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager)
      {
        // ヘッダファイルのロード
        // ファイルパスと同時にテーブルのキャパシティ(最大定義数)も指定する必要がある
        GFL_LOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH, 32);
        GFL_LOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH2, 32);        
        GFL_LOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH3, 2048);

        GFL_PRINT("===========================================\n");
        GFL_PRINT("DynamicHeaderTest Init\n");
        GFL_PRINT("===========================================\n");
        return gfl2::proc::RES_FINISH;
      }

      virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager)
      {
        // ファイルのアンロード
        GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH);
        GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH2);    
        GFL_UNLOAD_HEADER_HASH(NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH3);

        GFL_PRINT("===========================================\n");
        GFL_PRINT("DynamicHeaderTest End\n");
        GFL_PRINT("===========================================\n");
        return gfl2::proc::RES_FINISH;
      }

      virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager )
      {
        gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
        gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

        if (pButton->IsTrigger(gfl2::ui::BUTTON_B))
        {
          return gfl2::proc::RES_FINISH;
        }

        if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
        {
          // 値の取得
          s32 value = GFL_VALUE_DEFINE_IN_HEADER(ACTIONEVENT_STEPJUMP);
          // ロード済みのファイルパスを指定するとアクセスが高速化される
          // s32 value = GFL_VALUE_DEFINE_IN_HEADER(ACTIONEVENT_STEPJUMP, NIJI_DEBUG_KANAMARU_SAMPLE_HEADER_FILEPATH);

          s32 value3 = NIJI_ARCID_DEFINE_IN_HEADER(ARCID_GTS);

          GFL_PRINT("ACTIONEVENT_STEPJUMP = %d\n", value);
          GFL_PRINT("ARCID_GTS = %d\n", value3);
        }

        if (pButton->IsTrigger(gfl2::ui::BUTTON_X))
        {
          // 現在ロードされているヘッダの内容を出力する
          GFL_DYNAMIC_HEADER_DUMP();
        }

        return gfl2::proc::RES_CONTINUE;
      }

    };
  } // startmenu
} // debug

#endif // PM_DEBUG