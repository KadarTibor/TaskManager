#include "stdafx.h"
#include "RamUtil.h"
#include "psapi.h"

MEMORYSTATUSEX memInfo1;

RamUtil::RamUtil()
{
	memInfo1.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo1);
}


DWORDLONG RamUtil::getAllPhysMemory() {
	DWORDLONG totalPhysMem = memInfo1.ullTotalPhys;
	return totalPhysMem/1024;
}

DWORDLONG RamUtil::getPhysMemoryCurrentlyUsed() {
	return (memInfo1.ullTotalPhys - memInfo1.ullAvailPhys)/1024;
}

SIZE_T RamUtil::getRAMUsedByProcess(HANDLE Process) {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(Process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.WorkingSetSize/1024;
}
