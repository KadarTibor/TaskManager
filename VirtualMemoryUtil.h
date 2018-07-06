#pragma once
class VirtualMemoryUtil
{
public:
	VirtualMemoryUtil();
	DWORDLONG getAllVirtualMemory();
	DWORDLONG getVirtualMemoryCurrentlyUsed();
	SIZE_T getVMUsedByProcess(HANDLE Process);
};

