#ifndef SAMPLE_FRAMEWORK_IAPPLICATION_H_INCLUDED
#define SAMPLE_FRAMEWORK_IAPPLICATION_H_INCLUDED

#include <stdio.h>

namespace Sample { namespace Framework {

class IApplication
{
public:
	
	virtual bool OnInit( void ) = 0;
	virtual bool OnUpdate( void ) = 0;
	virtual bool OnDraw( void ) = 0;
	virtual bool OnTerm( void ) = 0;

	IApplication(){}
	virtual ~IApplication(){}
};

}}

#endif
