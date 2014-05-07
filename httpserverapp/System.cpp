
#include "System.h"

namespace System
{
#ifdef WIN32
	struct EnumData
	{
		native_processid_type process_id;
		HWND hWnd;
	};

	BOOL WINAPI EnumProc(HWND hWnd, LPARAM lParam)
	{
		EnumData &ed = *reinterpret_cast<EnumData *>(lParam);

		native_processid_type process_id = 0;

		GetWindowThreadProcessId(hWnd, &process_id);

		if (process_id == ed.process_id)
		{
			ed.hWnd = hWnd;

			return false;
		}

		return true;
	}
#endif

	bool sendSignal(native_processid_type pid, int signal)
	{
	#ifdef WIN32
		EnumData ed = {pid, nullptr};

		if (0 == EnumWindows(EnumProc, reinterpret_cast<LPARAM>(&ed) ) )
		{
			return false;
		}

		return 0 != PostMessage(ed.hWnd, signal, 0, 0);
	#elif POSIX
		return 0 == kill(pid, signal);
	#else
		#error "Undefine platform"
	#endif
	}
};