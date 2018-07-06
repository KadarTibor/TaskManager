#include "stdafx.h"
#include "CPUUtil.h"
#include "windows.h"
#include <vector>
#include "pdh.h"


//shlwapi.lib
//Kernel32.lib

CPUUtil::CPUUtil()
{
}

ULONGLONG SubtractTimes(const FILETIME one, const FILETIME two)
{
	LARGE_INTEGER a, b;
	a.LowPart = one.dwLowDateTime;
	a.HighPart = one.dwHighDateTime;

	b.LowPart = two.dwLowDateTime;
	b.HighPart = two.dwHighDateTime;

	return a.QuadPart - b.QuadPart;
}

double CPUUtil::getTotalCPUUsage() {
	ULARGE_INTEGER prevCPUIdle, prevCPUKernel, prevCPUUser;
	ULARGE_INTEGER currentCPUIdle, currentCPUKernel, currentCPUUser;

	FILETIME ftime, fsys, fuser;
	
	//read the initial CPU times idle kernel and user mode times
	GetSystemTimes(&ftime, &fsys, &fuser);
	//copy them into ULARGE_INTEGERS -> later can do calculations on them
	memcpy(&prevCPUIdle, &ftime, sizeof(FILETIME));
	memcpy(&prevCPUKernel, &fsys, sizeof(FILETIME));
	memcpy(&prevCPUUser, &fuser, sizeof(FILETIME));

	Sleep(1000);
	
	//read the cpu times again
	GetSystemTimes(&ftime, &fsys, &fuser);
	memcpy(&currentCPUIdle, &ftime, sizeof(FILETIME));
	memcpy(&currentCPUKernel, &fsys, sizeof(FILETIME));
	memcpy(&currentCPUUser, &fuser, sizeof(FILETIME));

	double idleTime =  currentCPUIdle.QuadPart - prevCPUIdle.QuadPart;
	double kernelTime = currentCPUKernel.QuadPart -  prevCPUKernel.QuadPart;
	double userTime = currentCPUUser.QuadPart -  prevCPUUser.QuadPart;

	double percent = (100 * (kernelTime + userTime - idleTime) / (kernelTime + userTime));

	return percent;
}

double CPUUtil::getCPUUsageForProcess(HANDLE ProcessHandle) {


	//static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	//static int numProcessors;
	//
	//SYSTEM_INFO sysInfo;
	//FILETIME ftime, fsys, fuser;

	//GetSystemInfo(&sysInfo);
	//numProcessors = sysInfo.dwNumberOfProcessors;

	//GetSystemTimeAsFileTime(&ftime);
	//memcpy(&lastCPU, &ftime, sizeof(FILETIME));

	//GetProcessTimes(ProcessHandle, &ftime, &ftime, &fsys, &fuser);
	//memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
	//memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
	//
	////let the application sleep for 2 seconds
	//Sleep(1000);
	//
	//ULARGE_INTEGER now, sys, user;
	//double percent;

	//GetSystemTimeAsFileTime(&ftime);
	//memcpy(&now, &ftime, sizeof(FILETIME));

	//GetProcessTimes(ProcessHandle, &ftime, &ftime, &fsys, &fuser);
	//memcpy(&sys, &fsys, sizeof(FILETIME));
	//memcpy(&user, &fuser, sizeof(FILETIME));
	//percent = (sys.QuadPart - lastSysCPU.QuadPart) +
	//	(user.QuadPart - lastUserCPU.QuadPart);
	//percent /= (now.QuadPart - lastCPU.QuadPart);
	//percent /= numProcessors;
	//lastCPU = now;
	//lastUserCPU = user;
	//lastSysCPU = sys;

	//return percent * 100;
	//initialize the needed vectors
	ULARGE_INTEGER prevCPUIdle, prevCPUKernel, prevCPUUser;
	ULARGE_INTEGER prevProcCreation, prevProcExit, prevProcKernel, prevProcUser;
	FILETIME ftime, fsys, fuser;
	FILETIME ProcCreationftime, ProcExitftime, ProcfKernel, ProcfUser;

	static int numProcessors;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	numProcessors = sysInfo.dwNumberOfProcessors;

	ULARGE_INTEGER currentProcCreation, currentProcExit, currentProcKernel, currentProcUser;
	ULARGE_INTEGER currentCPUIdle, currentCPUKernel, currentCPUUser;
	double percent;

	
		//read the initial CPU times idle kernel and user mode times
		GetSystemTimes(&ftime , &fsys , &fuser );
		//copy them into ULARGE_INTEGERS -> later can do calculations on them
		memcpy(&prevCPUIdle , &ftime , sizeof(FILETIME));
		memcpy(&prevCPUKernel , &fsys , sizeof(FILETIME));
		memcpy(&prevCPUUser , &fuser , sizeof(FILETIME));

		//read the initial Proc times creation, exit, kernel and user times
		GetProcessTimes(ProcessHandle , &ProcCreationftime , &ProcExitftime , &ProcfKernel , &ProcfUser );
		//copy them into ULARGE_INTEGERS -> later can do calculations on them
		memcpy(&prevProcCreation, &ProcCreationftime , sizeof(FILETIME));
		memcpy(&prevProcExit, &ProcExitftime , sizeof(FILETIME));
		memcpy(&prevProcKernel, &ProcfKernel , sizeof(FILETIME));
		memcpy(&prevProcUser, &ProcfUser, sizeof(FILETIME));
	

	//let the application sleep
	Sleep(1000);

	
		//read the cpu times again
		GetSystemTimes(&ftime , &fsys , &fuser );
		memcpy(&currentCPUIdle , &ftime , sizeof(FILETIME));
		memcpy(&currentCPUKernel , &fsys, sizeof(FILETIME));
		memcpy(&currentCPUUser , &fuser, sizeof(FILETIME));

		//read the Proc times again
		GetProcessTimes(ProcessHandle , &ProcCreationftime , &ProcExitftime , &ProcfKernel , &ProcfUser );
		memcpy(&currentProcCreation , &ProcCreationftime , sizeof(FILETIME));
		memcpy(&currentProcExit , &ProcExitftime , sizeof(FILETIME));
		memcpy(&currentProcKernel , &ProcfKernel , sizeof(FILETIME));
		memcpy(&currentProcUser , &ProcfUser , sizeof(FILETIME));

		percent  = (currentProcUser .QuadPart - prevProcUser .QuadPart) +
			(currentProcKernel .QuadPart - prevProcKernel .QuadPart);

		percent  /= ((currentCPUUser .QuadPart - prevCPUUser .QuadPart) +
			(currentCPUKernel .QuadPart - prevCPUKernel .QuadPart));

		percent  /= numProcessors;

		percent  *= 100;
	
	return percent;
}

std::vector<double> CPUUtil::getCPUUsageForProcesses(std::vector<HANDLE> ProcessHandles) {
	
		//initialize the needed vectors
		int processCount = ProcessHandles.size();
		static std::vector<ULARGE_INTEGER> prevCPUIdle(processCount), prevCPUKernel(processCount), prevCPUUser(processCount);
		static std::vector<ULARGE_INTEGER> prevProcCreation(processCount), prevProcExit(processCount), prevProcKernel(processCount), prevProcUser(processCount);
		std::vector<FILETIME> ftime(processCount), fsys(processCount), fuser(processCount);
		std::vector<FILETIME> ProcCreationftime(processCount), ProcExitftime(processCount), ProcfKernel(processCount), ProcfUser(processCount);

		static int numProcessors;
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		numProcessors = sysInfo.dwNumberOfProcessors;

		std::vector<ULARGE_INTEGER> currentProcCreation(processCount), currentProcExit(processCount), currentProcKernel(processCount), currentProcUser(processCount);
		std::vector<ULARGE_INTEGER> currentCPUIdle(processCount), currentCPUKernel(processCount), currentCPUUser(processCount);
		std::vector<double> percent(processCount);

		for (int i = 0; i < ProcessHandles.size(); i++) {
			//read the initial CPU times idle kernel and user mode times
			if (0 != GetSystemTimes(&ftime.at(i), &fsys.at(i), &fuser.at(i))) {
				//copy them into ULARGE_INTEGERS -> later can do calculations on them
				memcpy(&prevCPUIdle.at(i), &ftime.at(i), sizeof(FILETIME));
				memcpy(&prevCPUKernel.at(i), &fsys.at(i), sizeof(FILETIME));
				memcpy(&prevCPUUser.at(i), &fuser.at(i), sizeof(FILETIME));
			}
			

			//read the initial Proc times creation, exit, kernel and user times
			if (0 != GetProcessTimes(ProcessHandles.at(i), &ProcCreationftime.at(i), &ProcExitftime.at(i), &ProcfKernel.at(i), &ProcfUser.at(i))) {
				//copy them into ULARGE_INTEGERS -> later can do calculations on them
				memcpy(&prevProcCreation.at(i), &ProcCreationftime.at(i), sizeof(FILETIME));
				memcpy(&prevProcExit.at(i), &ProcExitftime.at(i), sizeof(FILETIME));
				memcpy(&prevProcKernel.at(i), &ProcfKernel.at(i), sizeof(FILETIME));
				memcpy(&prevProcUser.at(i), &ProcfUser.at(i), sizeof(FILETIME));
			}
			
		}

		//let the application sleep
		Sleep(1000);

		for (int i = 0; i < ProcessHandles.size(); i++) {
			//read the cpu times again
			if (0 != GetSystemTimes(&ftime.at(i), &fsys.at(i), &fuser.at(i))) {
				memcpy(&currentCPUIdle.at(i), &ftime.at(i), sizeof(FILETIME));
				memcpy(&currentCPUKernel.at(i), &fsys.at(i), sizeof(FILETIME));
				memcpy(&currentCPUUser.at(i), &fuser.at(i), sizeof(FILETIME));
			}
			

			//read the Proc times again
			if (0 != GetProcessTimes(ProcessHandles.at(i), &ProcCreationftime.at(i), &ProcExitftime.at(i), &ProcfKernel.at(i), &ProcfUser.at(i))) {
				memcpy(&currentProcCreation.at(i), &ProcCreationftime.at(i), sizeof(FILETIME));
				memcpy(&currentProcExit.at(i), &ProcExitftime.at(i), sizeof(FILETIME));
				memcpy(&currentProcKernel.at(i), &ProcfKernel.at(i), sizeof(FILETIME));
				memcpy(&currentProcUser.at(i), &ProcfUser.at(i), sizeof(FILETIME));

				percent.at(i) = (currentProcUser.at(i).QuadPart - prevProcUser.at(i).QuadPart) +
					(currentProcKernel.at(i).QuadPart - prevProcKernel.at(i).QuadPart);

				percent.at(i) /= ((currentCPUUser.at(i).QuadPart - prevCPUUser.at(i).QuadPart) +
					((currentCPUKernel.at(i).QuadPart - currentCPUIdle.at(i).QuadPart) - (prevCPUKernel.at(i).QuadPart - prevCPUIdle.at(i).QuadPart)));
				percent.at(i) /= numProcessors;
				percent.at(i) *= 100;
			}
			
		}
		return percent;
}
