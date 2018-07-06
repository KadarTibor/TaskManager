#include "stdafx.h"
#include "ProcessUtil.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
ProcessUtil::ProcessUtil()
{
}

std::vector<HANDLE> ProcessUtil::getAllProcesses(){
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		printf("ERROR\n Could not retrieve the current processes\n");
		exit(1);
	}


	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	std::vector<HANDLE> handles;
	
	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, aProcesses[i]);
		if (hProcess) {
			handles.push_back(hProcess);
		}
		
	}
	
	return handles;
}


void ProcessUtil::killProcess(HANDLE process) {

	//TerminateProcess second input parameter will be the exit code of the terminated process
	if (TerminateProcess(process, 1) == 0){
		printf("ERROR\nThe process could not be terminated\n");
	}
}
