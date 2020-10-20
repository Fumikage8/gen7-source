//#if PM_DEBUG

//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @author ito_isao
 *
 */
//====================================================================== 
#ifndef __MOVIE_ISCENE_H__
#define __MOVIE_ISCENE_H__

#pragma once

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"

namespace movie{

template <class T>
class IScene
{
public:
	IScene() : m_owner(NULL), m_parent(NULL)
	{
	}

	virtual ~IScene()
	{
	}

	virtual void Enter(int no) = 0;
	virtual int Update(int no) = 0;
	virtual bool Exit() = 0;
	virtual bool DrawTop(void) = 0;
	virtual bool DrawBottom(void) = 0;
  virtual void DrawEnd(void) = 0;

	void SetOwner(T* owner){m_owner = owner;}
	void SetParent(IScene* parent){m_parent = parent;}

protected:
	T* m_owner;
	IScene* m_parent;
};

template <class T>
class IChildScene : public IScene<T>
{
public:
	IChildScene()
	{
	}

	virtual ~IChildScene()
	{
	}
	
	virtual void Enter(int no) = 0;
	virtual int Update(int no) = 0;
	virtual bool Exit() = 0;
  virtual void DrawEnd(void) = 0;
};

template <class T>
class ParentScene : public IScene<T>
{
public:
	ParentScene() : m_current(NULL),
							m_next(NULL)
	{
	}

	virtual ~ParentScene()
	{
		if(m_next != NULL)
		{
			delete m_next;
			m_next = NULL;
		}
		if(m_current != NULL)
		{
			delete m_current;
			m_current = NULL;
		}
	}
  

	virtual void Enter(int movieno){}
	virtual int Update(int no)
	{
		if(m_next != NULL)
		{
      bool next = false;
			if(m_current != NULL)
			{
				if( m_current->Exit() )
        {
				  delete m_current;
          next = true;
        }
			}
      else
      {
        next = true;
      }
      if( next )
      {
        m_current = m_next;
        m_current->Enter(no);
        m_next = NULL;
        return 0;
      }
      else 
      {
        return 0;
      }
		}

		if(m_current)
		{
			return m_current->Update(no);
		}
		return 0;
	}

	virtual bool DrawTop(void)
	{
		if(m_current)
		{
			return m_current->DrawTop();
		}
    return false;
	}
	virtual bool DrawBottom(void)
	{
		if(m_current)
		{
			return m_current->DrawBottom();
		}
    return false;
	}
  virtual void DrawEnd(void)
  {
      if(m_current){m_current->DrawEnd();}
  }

	virtual bool Exit()
	{
		if(m_current)
		{
			return m_current->Exit();
		}
    return true;
	}

	void SetNext(IScene<T>* next)
	{
		m_next = next;
		m_next->SetParent(this);
		m_next->SetOwner(IScene<T>::m_owner);
	}

private:
	IScene<T>* m_current;
	IScene<T>* m_next;
};
	
}

#endif //__MOVIE_ISCENE_H__

