#include "debug.h"


void detect_memory_leaks(bool on_off)
{
	int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	if (!on_off)
		flags &= ~_CRTDBG_LEAK_CHECK_DF;
	else {
		flags |= _CRTDBG_LEAK_CHECK_DF;
		//下面这两个是指警告信息的输出模式，下面是指输出到控制台。
		//没有设置则信息是输出到VS的自带的输出框里
		//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		//_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	}
	_CrtSetDbgFlag(flags);
}