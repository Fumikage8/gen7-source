#ifndef GFLIB2_GRX_WINOPENGL_DISPLAYLIST_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_DISPLAYLIST_H_INCLUDED

#include <gfx/include/gfl2_DisplayList.h>
#include <gfx/include/WinOpenGL/gfl2_WinOpenGLTypes.h>

// Windows版のディスプレイリストはラムダ式でごまかします
#include <functional>

namespace gfl2 { namespace gfx { namespace winOpenGL {

class WinOpenGLDisplayList : public DisplayList
{
public:

  // コンストラクタ
	WinOpenGLDisplayList();

  // デストラクタ
	virtual ~WinOpenGLDisplayList();

  // ディスプレイリスト関数設定
  void SetFunc(const ::std::function<void()>& func)
  {
    m_Func = func;
  }

  // ディスプレイリスト呼び出し
  void Call()
  {
    m_Func();
  }

private:

  ::std::function<void()> m_Func;

};

}}}

#endif
