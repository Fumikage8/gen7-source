#include <stdio.h>
#include <tchar.h>

#include <system/include/InputDevice/gfl2_Pad.h>
#include <system/include/Timer/gfl2_PerformanceCounter.h>

using namespace gfl2::system;

int _tmain(int argc, _TCHAR* argv[])
{
	b32						fBreak = false;
	InputDevice::Pad::Initialize();

	while( !fBreak )
	{
		InputDevice::Pad::Update();

		for( u32 padNo = 0; padNo < InputDevice::Pad::MAX_USE_PAD; ++padNo )
		{
			if ( !InputDevice::Pad::IsConnecting( padNo ) )
				break;

			if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::Start ) )
				 fBreak = true;

			if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::A ) )
			{
				printf_s( "Pad%d Push A\n", padNo );
			}
			else if( InputDevice::Pad::IsPress( padNo, InputDevice::Button::B ) )
			{
				printf_s( "Pad%d Push B\n", padNo );
			}

			{
				f32			power = InputDevice::Pad::GetAnalogTrigger( padNo );
				if ( power >= 0.51f || power <= 0.49f )
					printf_s( "Z Trigger = %f\n", power );
			}

			{
				const InputDevice::StickPos&				pos = InputDevice::Pad::GetStick( padNo, InputDevice::Stick::Left );
				if ( pos.x != 0.0f && pos.y != 0.0f )
					printf_s( "Pad%d Stick = %f, %f\n", padNo, pos.x, pos.y );
			}
		}

		u64				mill = Timer::PerformanceCounter::GetMilliSecond();
		printf_s( "time = 	%llu\n", mill  );

		::Sleep( 1 );
	}

	InputDevice::Pad::Terminate();

	return 0;
}

