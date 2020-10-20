#if defined(GF_PLATFORM_CTR)

//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author ito_isao
 *
 */
//====================================================================== 

#ifndef __MOVIE_MOBICLIP_SCENE_H__
#define __MOVIE_MOBICLIP_SCENE_H__

#pragma once

#include "IScene.h"
#include "Mobiclipper.h"
#include "MovieProc.h"
//#include <assets/app/apparel_shop/kisekae_res.h>
#include "AppLib/include/Util/app_util_2d.h"

namespace movie{


class MobiclipScene : public IChildScene<MovieProcBase>
{
public:
	MobiclipScene();
	virtual ~MobiclipScene();

	virtual void Enter(int movieno);
	virtual int Update(int no);
	virtual bool Exit();

	virtual bool DrawTop(void);
	virtual bool DrawBottom(void);
  virtual void DrawEnd(void);

  void SetLoopFlag(bool b){m_loop = b;}

private:
  movie::Mobiclipper s_mobiclip;
  double s_before;
  double s_time;
  int m_decide;

  app::util::G2DUtil* m_layout;
  app::util::Heap m_heap;
  int mMovieNo;
  
  bool m_initialized;
  bool m_loop;
  bool m_canSkip;
  bool m_titleMode;
  u32 m_decidedSE;
  int m_seq;
  u32 m_hold;
};

}

#endif //__MOVIE_MOBICLIP_SCENE_H__

#endif //defined(GF_PLATFORM_CTR)
