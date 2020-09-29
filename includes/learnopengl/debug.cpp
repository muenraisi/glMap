#include "debug.h"


void detect_memory_leaks(bool on_off)
{
	int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	if (!on_off)
		flags &= ~_CRTDBG_LEAK_CHECK_DF;
	else {
		flags |= _CRTDBG_LEAK_CHECK_DF;
		//������������ָ������Ϣ�����ģʽ��������ָ���������̨��
		//û����������Ϣ�������VS���Դ����������
		//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		//_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	}
	_CrtSetDbgFlag(flags);
}