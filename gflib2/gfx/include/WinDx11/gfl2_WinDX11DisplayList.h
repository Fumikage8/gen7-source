#ifndef GFLIB2_GRX_WINOPENGL_DISPLAYLIST_H_INCLUDED
#define GFLIB2_GRX_WINOPENGL_DISPLAYLIST_H_INCLUDED

#include <gfx/include/gfl2_DisplayList.h>
#include <gfx/include/WinDX11/gfl2_WinDX11Types.h>

// Windows版のディスプレイリストはラムダ式でごまかします
#include <functional>

namespace gfl2 { namespace gfx { namespace windx11 {

class WinDX11DisplayList : public DisplayList
{
public:

  // コンストラクタ
	WinDX11DisplayList();

  // デストラクタ
	virtual ~WinDX11DisplayList();

  // ディスプレイリスト関数設定
  void SetFunc(const std::function<void()>& func)
  {
    m_Func = func;
  }

  // ディスプレイリスト呼び出し
  void Call()
  {
    m_Func();
  }

private:

  std::function<void()> m_Func;

};

}}}

#endif
