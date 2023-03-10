#define no_init_all
#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <intrin.h>
#include <NTstatus.h>
#define WIN32_NO_STATUS
#include <Windows.h>
#include <Powrprof.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <comdef.h>
#include "TCHAR.h"
#include "pdh.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"pdh.lib")
#pragma comment(lib, "Powrprof.lib")

using Microsoft::WRL::ComPtr;

class Specs {

public:

	bool SSE3(void) { return f_1_ECX_[0]; }
	bool PCLMULQDQ(void) { return f_1_ECX_[1]; }
	bool MONITOR(void) { return f_1_ECX_[3]; }
	bool SSSE3(void) { return f_1_ECX_[9]; }
	bool FMA(void) { return f_1_ECX_[12]; }
	bool CMPXCHG16B(void) { return f_1_ECX_[13]; }
	bool SSE41(void) { return f_1_ECX_[19]; }
	bool SSE42(void) { return f_1_ECX_[20]; }
	bool MOVBE(void) { return f_1_ECX_[22]; }
	bool POPCNT(void) { return f_1_ECX_[23]; }
	bool AES(void) { return f_1_ECX_[25]; }
	bool XSAVE(void) { return f_1_ECX_[26]; }
	bool OSXSAVE(void) { return f_1_ECX_[27]; }
	bool AVX(void) { return f_1_ECX_[28]; }
	bool F16C(void) { return f_1_ECX_[29]; }
	bool RDRAND(void) { return f_1_ECX_[30]; }

	bool MSR(void) { return f_1_EDX_[5]; }
	bool CX8(void) { return f_1_EDX_[8]; }
	bool SEP(void) { return f_1_EDX_[11]; }
	bool CMOV(void) { return f_1_EDX_[15]; }
	bool CLFSH(void) { return f_1_EDX_[19]; }
	bool MMX(void) { return f_1_EDX_[23]; }
	bool FXSR(void) { return f_1_EDX_[24]; }
	bool SSE(void) { return f_1_EDX_[25]; }
	bool SSE2(void) { return f_1_EDX_[26]; }

	bool FSGSBASE(void) { return f_7_EBX_[0]; }
	bool BMI1(void) { return f_7_EBX_[3]; }
	bool HLE(void) { return isIntel_ && f_7_EBX_[4]; }
	bool AVX2(void) { return f_7_EBX_[5]; }
	bool BMI2(void) { return f_7_EBX_[8]; }
	bool ERMS(void) { return f_7_EBX_[9]; }
	bool INVPCID(void) { return f_7_EBX_[10]; }
	bool RTM(void) { return isIntel_ && f_7_EBX_[11]; }
	bool AVX512F(void) { return f_7_EBX_[16]; }
	bool RDSEED(void) { return f_7_EBX_[18]; }
	bool ADX(void) { return f_7_EBX_[19]; }
	bool AVX512PF(void) { return f_7_EBX_[26]; }
	bool AVX512ER(void) { return f_7_EBX_[27]; }
	bool AVX512CD(void) { return f_7_EBX_[28]; }
	bool SHA(void) { return f_7_EBX_[29]; }

	bool PREFETCHWT1(void) { return f_7_ECX_[0]; }

	bool LAHF(void) { return f_81_ECX_[0]; }
	bool LZCNT(void) { return isIntel_ && f_81_ECX_[5]; }
	bool ABM(void) { return isAMD_ && f_81_ECX_[5]; }
	bool SSE4a(void) { return isAMD_ && f_81_ECX_[6]; }
	bool XOP(void) { return isAMD_ && f_81_ECX_[11]; }
	bool TBM(void) { return isAMD_ && f_81_ECX_[21]; }

	bool SYSCALL(void) { return isIntel_ && f_81_EDX_[11]; }
	bool MMXEXT(void) { return isAMD_ && f_81_EDX_[22]; }
	bool RDTSCP(void) { return isIntel_ && f_81_EDX_[27]; }
	bool _3DNOWEXT(void) { return isAMD_ && f_81_EDX_[30]; }
	bool _3DNOW(void) { return isAMD_ && f_81_EDX_[31]; }

	typedef struct _PROCESSOR_POWER_INFORMATION {
		ULONG  Number;
		ULONG  MaxMhz;
		ULONG  CurrentMhz;
		ULONG  MhzLimit;
		ULONG  MaxIdleState;
		ULONG  CurrentIdleState;
	} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;

	typedef BOOL(WINAPI* LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);

	DWORD CountSetBits(ULONG_PTR bitMask)
	{
		DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
		DWORD bitSetCount = 0;
		ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
		DWORD i;

		for (i = 0; i <= LSHIFT; ++i)
		{
			bitSetCount += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}

		return bitSetCount;
	}

	int getCores()
	{
		LPFN_GLPI glpi;
		BOOL done = FALSE;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
		DWORD returnLength = 0;
		DWORD byteOffset = 0;
		PCACHE_DESCRIPTOR Cache;

		glpi = (LPFN_GLPI)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (NULL == glpi)
		{
			_tprintf(TEXT("\nGetLogicalProcessorInformation is not supported.\n"));
			return (1);
		}

		while (!done)
		{
			DWORD rc = glpi(buffer, &returnLength);

			if (FALSE == rc)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						free(buffer);

					buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
						returnLength);

					if (NULL == buffer)
					{
						_tprintf(TEXT("\nError: Allocation failure\n"));
						return (2);
					}
				}
				else
				{
					_tprintf(TEXT("\nError %d\n"), GetLastError());
					return (3);
				}
			}
			else
			{
				done = TRUE;
			}
		}

		ptr = buffer;

		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationProcessorCore:
				processorCoreCount++;
				logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
				break;

			case RelationCache:
				Cache = &ptr->Cache;
				if (Cache->Level == 1)
				{
					processorL1CacheCount++;
				}
				else if (Cache->Level == 2)
				{
					processorL2CacheCount++;
				}
				else if (Cache->Level == 3)
				{
					processorL3CacheCount++;
				}
				break;

			default:
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		_tprintf(TEXT("Number of processor cores: %d\n"),
			processorCoreCount);
		_tprintf(TEXT("Number of logical processors: %d\n"),
			logicalProcessorCount);
		_tprintf(TEXT("Number of processor L1/L2/L3 caches: %d/%d/%d\n"),
			processorL1CacheCount,
			processorL2CacheCount,
			processorL3CacheCount);

		free(buffer);

		return 0;
	}

	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal;

	void init() {
		PdhOpenQuery(NULL, NULL, &cpuQuery);
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQuery);
	}

	double getCurrentValue() {

		PDH_FMT_COUNTERVALUE counterVal;
		PdhCollectQueryData(cpuQuery);
		PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	}

	void QueryKey(HKEY hKey)
	{
		DWORD    cValues;
		DWORD    retCode;
		TCHAR    pvData[16383];
		DWORD    cbData = sizeof(TCHAR) * 16383;
		TCHAR    targetValue[] = L"Identifier";

		retCode = RegQueryInfoKey(
			hKey,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			&cValues,
			NULL,
			NULL,
			NULL,
			NULL);

		if (cValues)
		{
			retCode = RegGetValue(hKey, NULL, targetValue, RRF_RT_REG_SZ, NULL, pvData, &cbData);
			if (retCode != ERROR_SUCCESS)
				return;
			wchar_t temp[128];
			char temp_char[128];
			//snprintf(temp, 1024, "%s: %s\r\n", targetValue, pvData);
			_snwprintf_s(temp, 1024, TEXT("%s: %s\r\n"), targetValue, pvData);
			size_t charsConverted = 0;
			wcstombs_s(&charsConverted, temp_char, temp, 128);
			strcat_s(storageDevices, temp_char);
		}
	}

	void getDrives() {

		std::stringstream ss;

		DWORD cchBuffer;
		WCHAR* driveStrings;
		WCHAR* temp;
		UINT driveType;
		LPCWSTR driveTypeString;
		ULARGE_INTEGER freeSpace;

		cchBuffer = GetLogicalDriveStrings(0, NULL);

		driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(TCHAR));
		temp = driveStrings;
		if (driveStrings == NULL)
			return;
 
		GetLogicalDriveStrings(cchBuffer, driveStrings);

		while (*driveStrings)
		{
			// Dump drive information
			driveType = GetDriveType(driveStrings);
			GetDiskFreeSpaceEx(driveStrings, &freeSpace, NULL, NULL);

			switch (driveType)
			{
			case DRIVE_FIXED:
				driveTypeString = L"Hard disk";
				break;

			case DRIVE_CDROM:
				driveTypeString = L"CD/DVD";
				break;

			case DRIVE_REMOVABLE:
				driveTypeString = L"Removable";
				break;

			case DRIVE_REMOTE:
				driveTypeString = L"Network";
				break;

			default:
				driveTypeString = L"Unknown";
				break;
			}

			//ss << driveStrings << " - " << driveTypeString << " - " << freeSpace.QuadPart / 1024 / 1024 / 1024 << std::endl;

			char temp[1024];

			snprintf(temp, 1024, "%S - %S - %I64u GB free\r\n", driveStrings, driveTypeString,
				freeSpace.QuadPart / 1024 / 1024 / 1024);

			strcat_s(drives, temp);
			//snprintf("%S - %S - %I64u GB free\n", driveStrings, driveTypeString,
				//freeSpace.QuadPart / 1024 / 1024 / 1024);

			// Move to next drive string
			// +1 is to move past the null at the end of the string.
			driveStrings += lstrlen(driveStrings) + 1;
		}

		//strcpy_s(drives, ss.str().c_str());

		free(temp);
	}

	int getAvailableMemory() {
		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		return status.ullAvailPhys / 1024 / 1024;
	}

	int getUsage() {
		return (int)getCurrentValue();
	}

	int getFrequency() {
		SYSTEM_INFO si = { 0 };
		::GetSystemInfo(&si);
		// allocate buffer to get info for each processor
		const int size = si.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
		LPBYTE pBuffer = new BYTE[size];

		NTSTATUS status = ::CallNtPowerInformation(ProcessorInformation, NULL, 0, pBuffer, size);
		if (STATUS_SUCCESS == status)
		{
			// list each processor frequency 
			PPROCESSOR_POWER_INFORMATION ppi = (PPROCESSOR_POWER_INFORMATION)pBuffer;
			return ppi->CurrentMhz;
			for (DWORD nIndex = 0; nIndex < si.dwNumberOfProcessors; nIndex++)
			{
				//std::cout << "Processor #" << ppi->Number << " frequency: "
					//<< ppi->CurrentMhz << " MHz" << std::endl;
				ppi++;
			}
		}
		else
		{
			return -1;
		}
		delete[]pBuffer;
		return -1;
	}

	static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
	{
		static unsigned long long _previousTotalTicks = 0;
		static unsigned long long _previousIdleTicks = 0;

		unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
		unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

		float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

		_previousTotalTicks = totalTicks;
		_previousIdleTicks = idleTicks;
		return ret;
	}

	static unsigned long long FileTimeToInt64(const FILETIME & ft) { return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); }

	int getUsage2()
	{
		FILETIME idleTime, kernelTime, userTime;
		return (1.0 * (GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f)*100);
	}

	int getNominalFrequency()
	{
		wchar_t Buffer[_MAX_PATH];
		DWORD BufSize = _MAX_PATH;
		DWORD dwMHz = _MAX_PATH;
		HKEY hKey;

		long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
		if (lError != ERROR_SUCCESS)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lError, 0, Buffer, _MAX_PATH, 0);
			wprintf(Buffer);
			return 0;
		}

		RegQueryValueEx(hKey, L"~MHz", NULL, NULL, (LPBYTE)&dwMHz, &BufSize);
		return (int)dwMHz;
	}

	void retrieveInstructions() {
		std::array<int, 4> cpui;

		__cpuid(cpui.data(), 0);
		nIds_ = cpui[0];

		for (int i = 0; i <= nIds_; ++i)
		{
			__cpuidex(cpui.data(), i, 0);
			data_.push_back(cpui);
		}

		if (vendor == "GenuineIntel")
		{
			isIntel_ = true;
		}
		else if (vendor == "AuthenticAMD")
		{
			isAMD_ = true;
		}

		if (nIds_ >= 1)
		{
			f_1_ECX_ = data_[1][2];
			f_1_EDX_ = data_[1][3];
		}

		if (nIds_ >= 7)
		{
			f_7_EBX_ = data_[7][1];
			f_7_ECX_ = data_[7][2];
		}

		__cpuid(cpui.data(), 0x80000000);
		nExIds_ = cpui[0];

		for (int i = 0x80000000; i <= nExIds_; ++i)
		{
			__cpuidex(cpui.data(), i, 0);
			extdata_.push_back(cpui);
		}

		if (nExIds_ >= 0x80000001)
		{
			f_81_ECX_ = extdata_[1][2];
			f_81_EDX_ = extdata_[1][3];
		}
		int i = 0;
		std::stringstream outstream;
		auto support_message = [&outstream, &i](std::string isa_feature, bool is_supported) {
			if (is_supported)
			{
				outstream << isa_feature + "/";
				i++;
				if (i % 10 == 0)
					outstream << std::endl;
			}
		};

		support_message("3DNOW", _3DNOW());
		support_message("3DNOWEXT", _3DNOWEXT());
		support_message("ABM", ABM());
		support_message("ADX", ADX());
		support_message("AES", AES());
		support_message("AVX", AVX());
		support_message("AVX2", AVX2());
		support_message("AVX512CD", AVX512CD());
		support_message("AVX512ER", AVX512ER());
		support_message("AVX512F", AVX512F());
		support_message("AVX512PF", AVX512PF());
		support_message("BMI1", BMI1());
		support_message("BMI2", BMI2());
		support_message("CLFSH", CLFSH());
		support_message("CMPXCHG16B", CMPXCHG16B());
		support_message("CX8", CX8());
		support_message("ERMS", ERMS());
		support_message("F16C", F16C());
		support_message("FMA", FMA());
		support_message("FSGSBASE", FSGSBASE());
		support_message("FXSR", FXSR());
		support_message("HLE", HLE());
		support_message("INVPCID", INVPCID());
		support_message("LAHF", LAHF());
		support_message("LZCNT", LZCNT());
		support_message("MMX", MMX());
		support_message("MMXEXT", MMXEXT());
		support_message("MONITOR", MONITOR());
		support_message("MOVBE", MOVBE());
		support_message("MSR", MSR());
		support_message("OSXSAVE", OSXSAVE());
		support_message("PCLMULQDQ", PCLMULQDQ());
		support_message("POPCNT", POPCNT());
		support_message("PREFETCHWT1", PREFETCHWT1());
		support_message("RDRAND", RDRAND());
		support_message("RDSEED", RDSEED());
		support_message("RDTSCP", RDTSCP());
		support_message("RTM", RTM());
		support_message("SEP", SEP());
		support_message("SHA", SHA());
		support_message("SSE", SSE());
		support_message("SSE2", SSE2());
		support_message("SSE3", SSE3());
		support_message("SSE4.1", SSE41());
		support_message("SSE4.2", SSE42());
		support_message("SSE4a", SSE4a());
		support_message("SSSE3", SSSE3());
		support_message("SYSCALL", SYSCALL());
		support_message("TBM", TBM());
		support_message("XOP", XOP());
		support_message("XSAVE", XSAVE());
		strcpy_s(instructions, outstream.str().c_str());

	}

	void retrieveInformation() {
		int CPUInfo[4];

		//Brand string
		__cpuid(CPUInfo, 0x80000000);
		int exIds = CPUInfo[0];
		for (int i = 0x80000000; i <= exIds; i++)
		{
			__cpuid(CPUInfo, i);
			if (i == 0x80000002)
				memcpy(brand, CPUInfo, 16);
			if (i == 0x80000003)
				memcpy(brand + 16, CPUInfo, 16);
			if (i == 0x80000004)
				memcpy(brand + 32, CPUInfo, 16);
		}

		//Vendor
		__cpuid(CPUInfo, 0);
		memset(vendor, 0, sizeof(vendor));
		*reinterpret_cast<int*>(vendor) = CPUInfo[1];
		*reinterpret_cast<int*>(vendor + 4) = CPUInfo[3];
		*reinterpret_cast<int*>(vendor + 8) = CPUInfo[2];

		getCores();

		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		//std::cout << "Number of Threads: " << systemInfo.dwNumberOfProcessors << std::endl;
		//std::cout << "Processor architecture: " << systemInfo.wProcessorArchitecture << std::endl;
		pageSize = systemInfo.dwPageSize;
		//std::cout << "Minimum application address: " << systemInfo.lpMinimumApplicationAddress << std::endl;
		//std::cout << "Maximum application address: " << systemInfo.lpMaximumApplicationAddress << std::endl;
		//std::cout << "Active processor mask: " << systemInfo.dwActiveProcessorMask << std::endl;
		//std::cout << "Processor type: " << systemInfo.dwProcessorType << std::endl;
		//std::cout << "Allocation granularity: " << systemInfo.dwAllocationGranularity << std::endl;
		//std::cout << "Processor level: " << systemInfo.wProcessorLevel << std::endl;
		//std::cout << "Processor revision: " << systemInfo.wProcessorRevision << std::endl;

		MEMORYSTATUSEX status;
		status.dwLength = sizeof(status);
		GlobalMemoryStatusEx(&status);
		//std::cout << "Total System Memory: " << (status.ullTotalPhys / 1024) / 1024 << "MB" << std::endl;
		memoryLength = status.dwLength;
		//std::cout << "Memory load: " << status.dwMemoryLoad << "%" << std::endl;
		totalMemory = status.ullTotalPhys / 1024 / 1024;
		availableMemory = status.ullAvailPhys / 1024 / 1024;
		//std::cout << "Total page file: " << status.ullTotalPageFile << "" << std::endl;
		//std::cout << "Available page file: " << status.ullAvailPageFile << "" << std::endl;
		//std::cout << "Total virtual: " << status.ullTotalVirtual / 1024 / 1024 << "MB" << std::endl;
		//std::cout << "Available virtual: " << status.ullAvailVirtual / 1024 / 1024 << "MB" << std::endl;
		//std::cout << "Available extended virtual: " << status.ullAvailExtendedVirtual << "" << std::endl;
		nominalFrequency = getNominalFrequency();

		ComPtr<IDXGIFactory1> dxgiFactory;
		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.ReleaseAndGetAddressOf()));

		ComPtr<IDXGIAdapter1> adapter;
		for (UINT adapterIndex = 0;
			SUCCEEDED(dxgiFactory->EnumAdapters1(
				adapterIndex,
				adapter.ReleaseAndGetAddressOf()));
			adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc = {};
			hr = adapter->GetDesc1(&desc);
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			_bstr_t b(desc.Description);
			char* c = b;
			strcat_s(gpuName, c);
			char nlcr[5] = "\r\n";
			strcat_s(gpuName, nlcr);
			//std::cout << gpuName << std::endl;
		}

		init();

		for (int i = 0; i < 64; i++)
		{
			std::wstringstream ss;
			ss << "HARDWARE\\DEVICEMAP\\\Scsi\\Scsi Port 0\\Scsi Bus ";
			ss << i;
			ss << "\\Target Id 0\\Logical Unit Id 0";
			HKEY hTestKey;
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, ss.str().c_str(), 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
				QueryKey(hTestKey);
			RegCloseKey(hTestKey);
		}
	}

	DWORD logicalProcessorCount = 0;//
	DWORD processorCoreCount = 0;/////
	DWORD processorL1CacheCount = 0;//
	DWORD processorL2CacheCount = 0;//
	DWORD processorL3CacheCount = 0;//
	char vendor[32];				//
	char brand[64];					//
	int totalMemory;				//
	int availableMemory;			//
	int memoryLength;				//
	int pageSize;					//
	double cpuUsage;
	int nominalFrequency;
	int frequency;
	char gpuName[256];

	char instructions[1024];
	char drives[1024];
	char storageDevices[1024];
	int nIds_;
	int nExIds_;
	bool isIntel_;
	bool isAMD_;
	std::bitset<32> f_1_ECX_;
	std::bitset<32> f_1_EDX_;
	std::bitset<32> f_7_EBX_;
	std::bitset<32> f_7_ECX_;
	std::bitset<32> f_81_ECX_;
	std::bitset<32> f_81_EDX_;
	std::vector<std::array<int, 4>> data_;
	std::vector<std::array<int, 4>> extdata_;
};

