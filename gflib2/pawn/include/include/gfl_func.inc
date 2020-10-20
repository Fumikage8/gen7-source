native _Suspend(frame);

// フレーム数を指定してサスペンドする　0以下ならずっとサスペンドする
stock Suspend(frame=1)
{
	_Suspend(frame);
	sleep(0);
}
