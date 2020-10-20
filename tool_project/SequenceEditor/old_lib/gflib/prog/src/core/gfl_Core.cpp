// ファイルをまとめてコンパイル時間を大幅短縮

#include <gflib.h>

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Debug.cpp")
#endif
#include "gfl_Debug.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_MathBase.cpp")
#endif
#include "gfl_MathBase.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Vector.cpp")
#endif
#include "gfl_Vector.cpp"

#if GFL_USE_AOS

#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixAos44.cpp")
#    endif
#    include "gfl_MatrixAos44.cpp"
#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixAos43.cpp")
#    endif
#    include "gfl_MatrixAos43.cpp"

#endif

#if GFL_USE_SOA

#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixSoaNative44.cpp")
#    endif
#    include "gfl_MatrixSoaNative44.cpp"

#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixSoa44.cpp")
#    endif
#    include "gfl_MatrixSoa44.cpp"

#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixSoaNative34.cpp")
#    endif
#    include "gfl_MatrixSoaNative34.cpp"

#    ifdef GFL_COMPILER_VC
#        pragma message ("gfl_MatrixSoa34.cpp")
#    endif
#    include "gfl_MatrixSoa34.cpp"

#endif

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Quaternion.cpp")
#endif
#include "gfl_Quaternion.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Time.cpp")
#endif
#include "gfl_Time.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_HashList.cpp")
#endif
#include "gfl_HashList.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Camera.cpp")
#endif
#include "gfl_Camera.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_CoreFile.cpp")
#endif
#include "gfl_CoreFile.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_SmartPointer.cpp")
#endif
#include "gfl_SmartPointer.cpp"

#ifdef GFL_COMPILER_VC
#    pragma message ("gfl_Utility.cpp")
#endif
#include "gfl_Utility.cpp"
