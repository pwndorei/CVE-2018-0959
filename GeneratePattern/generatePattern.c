#include<Windows.h>
#include<stdio.h>

#define VRAM_SIZE 0x800000
#define FILE_NAME L"pattern.dat"
#define PATTERN_SIZE 0x100000

int main() {
	PVOID *mapView = NULL;
	__int64 data = 0x4141414141414141;
	DWORD patternSize = PATTERN_SIZE;
	DWORD i = 0;

	HANDLE hFile, hFileMap;
	hFile = CreateFileW(FILE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	hFileMap = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, 0, 0x800000, NULL);//8MB
	mapView = MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
	if (mapView == NULL) {
		printf("MapViewOfFile Failed(%d)\n", GetLastError());
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return -1;
	}

	for (i = 0; i < VRAM_SIZE; i+=patternSize) {
		if (data == 0x4848484848484848) break;//H, last 1MB
		for (DWORD j = 0; j < patternSize / 0x8; j++) {
			((__int64*)(mapView + i/8))[j] = data;
		}
		data += 0x0101010101010101;
	}

	patternSize /= 0x8;

	for (; i < VRAM_SIZE; i += patternSize) {//128KB per pattern
		if (data == 0x4f4f4f4f4f4f4f4f) break;//), Last 128KB
		printf("%p\n", mapView + i);
		for (DWORD j = 0; j < patternSize / 0x8; j++) {
			((__int64*)(mapView + i/8))[j] = data;
		}
		data += 0x0101010101010101;
	}

	patternSize /= 0x8;

	for (; i < VRAM_SIZE; i += patternSize) {//16KB per pattern
		printf("%p\n", mapView + i);
		for (DWORD j = 0; j < patternSize / 0x8; j++) {
			((__int64*)(mapView + i/8))[j] = data;
		}
		data += 0x0101010101010101;
	}

	UnmapViewOfFile(mapView);
	CloseHandle(hFileMap);
	CloseHandle(hFile);
}
