#pragma once
class RamUtil
{
public:
	RamUtil();
	DWORDLONG getAllPhysMemory();
	DWORDLONG getPhysMemoryCurrentlyUsed();
	SIZE_T getRAMUsedByProcess(HANDLE Process);
};

