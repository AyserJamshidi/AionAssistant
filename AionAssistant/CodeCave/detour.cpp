#include "detour.h"
#include <stdlib.h>

static const int minLen = 14;

void* DetourFunction(void* pSource, void* pDestination, int dwLen) {
	/* If length of bytes we are stealing is < 14 bytes we SHOULD NOT modify anything.
	 *
	 * x64 ASM requires at least 14 bytes to detour as that is the bare minimum for
	 * the QWORD pointer we are using.
	 *
	 * Continuing with < 14 bytes will corrupt the stack and crash Aion.
	 */
	if (dwLen < minLen)
		return nullptr;

	BYTE stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     // ptr that we input
	};

	// Gets address for detour code
	void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD dwOld = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	// trampoline, skips 6 bytes then places pDestination addr
	memcpy(stub + 6, &pDestination, 8);
	//memcpy(stub + 6, pSource, sizeof(pSource));

	// Copy ASM to destination
	memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);

	// JMP placement.  First var is location to place, second var is bytes to place, third var is size.
	// stub var seems to have some corruption at this point?
	memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

	// orig
	memcpy(stub + 6, &pTrampoline, 8);

	// Replaces pSource with JMP to detour
	memcpy(pSource, stub, sizeof(stub));

	// Apply NOPs if we didn't use enough bytes yet
	for (int i = minLen; i < dwLen; i++)
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;

	VirtualProtect(pSource, dwLen, dwOld, &dwOld);

	return (void*)((DWORD_PTR)pSource + dwLen);
}

bool DetourRemove(void* pSource, int dwLen) {
	/* This check is similar to DetourFunction(..), for more in-depth explanation, check that function.
	 * 
	 * The only difference here is we check to ensure that we are removing bytes 0xFF and 0x25, otherwise
	 * pSource is NOT a valid detour (not a jmp qword).
	 */
	if (dwLen < minLen || *(WORD*)(pSource) != 0x25FF)
		return false;

	void* pTrampoline = *(void**)((uintptr_t)pSource + 6);

	DWORD oldProtect;

	// If memory is changed to R/W then we continue.
	if (VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &oldProtect)) {
		// Place pTrampoline code (detoured code) to pSource;
		memcpy(pSource, pTrampoline, dwLen);
		return VirtualProtect(pSource, dwLen, oldProtect, &oldProtect);
	}

	return false;
}