#include "stdafx.h"
#include "VirtualMemoryUtil.h"
#include "windows.h"
#include "psapi.h"

MEMORYSTATUSEX memInfo;

VirtualMemoryUtil::VirtualMemoryUtil()
{
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
}

DWORDLONG VirtualMemoryUtil::getAllVirtualMemory() {
	DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
	return totalVirtualMem/1024;
}

DWORDLONG VirtualMemoryUtil::getVirtualMemoryCurrentlyUsed() {
	return (memInfo.ullTotalPageFile - memInfo.ullAvailPageFile)/1024;
}

SIZE_T VirtualMemoryUtil::getVMUsedByProcess(HANDLE Process) {
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(Process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return pmc.PrivateUsage/1024;
}
