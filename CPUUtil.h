#pragma once
#include <vector>
class CPUUtil
{
public:
	CPUUtil();
	double getCPUUsageForProcess(HANDLE ProcessHandle);
	double getTotalCPUUsage();
	std::vector<double> getCPUUsageForProcesses(std::vector<HANDLE> ProcessHandles);
};

