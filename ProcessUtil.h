#pragma once
#include <vector>
class ProcessUtil
{
public:
	ProcessUtil();
	std::vector<HANDLE> getAllProcesses();
	void killProcess(HANDLE process);
};

