#include "CDSChain.h"

#include "CBulanci.h"
#include "CDSEasyMemStream.h"
#include "CDSGZipStream.h"
#include "CDSSimpleException.h"
#include "_Globals.h"
#include "_externs.h"

#include <windows.h>

namespace {

/* Layout matches Ghidra CDSChain_full @ CGame+0x31 (164 B). See CDSChain.md */
constexpr uint kOffConfigSig = 0x04;
constexpr uint kOffKeyBindings = 0x0d;
constexpr uint kOffStrConfigTail = 0x45;
constexpr uint kOffPanBinding = 0x49;
constexpr uint kOffReservedAudio0 = 0x4d;
constexpr uint kOffReservedAudio1 = 0x51;
constexpr uint kOffRegistryCommit = 0x55;
constexpr uint kOffOptionBlock = 0x56;
constexpr uint kOffIdsChained = 0x68;
constexpr uint kOffField78 = 0x78;
constexpr uint kOffProfileList = 0x79;
constexpr uint kOffProfileUser = 0x31;

constexpr uchar kConfigSignature = 0xb4;

using RegistryLoadChainFn = void(__thiscall*)(void* idsChainedThis, CDSGZipStream* gzipReadFace);

void InvokeRegistryLoadVfn(uchar* chain, CDSGZipStream* gzipReadFace) {
	auto* vt = *reinterpret_cast<void***>(chain + kOffIdsChained);
	auto fn = reinterpret_cast<RegistryLoadChainFn>(vt[4]);
	fn(reinterpret_cast<void*>(chain + kOffIdsChained), gzipReadFace);
}

void LoadConfigFromRegistryBody(uchar* chain) {
	auto* bul = reinterpret_cast<CBulanci*>(chain);

	short pathHolder[64];
	reinterpret_cast<CBulanci*>(pathHolder)->CDsStringAssignFromLiteral(
	    reinterpret_cast<short*>(&PTR_u_Software_SleepTeam_Bulanci_0047f66c_004ae000));

	alignas(void*) uchar regKey[16];
	CDSEasyMemStream mem;
	constexpr size_t kGzipObjectSize = 0x48;
	alignas(8) uchar gzipStorage[kGzipObjectSize];
	auto* gzip = reinterpret_cast<CDSGZipStream*>(gzipStorage);
	auto* gzipRead = reinterpret_cast<CDSGZipStream*>(gzipStorage + 0xc);

	_Globals::CDSRegKey_ctor(regKey, HKEY_LOCAL_MACHINE, reinterpret_cast<int>(pathHolder),
	                         reinterpret_cast<uchar*>(0xf003f));

	mem.CDSEasyMemStream_ctor(reinterpret_cast<uchar*>(0x1000), 0x800);
	bul->RegQueryBinaryStream(reinterpret_cast<LPCWSTR>(&PTR_u_Config_004ae5e8),
	                          reinterpret_cast<int*>(&mem));

	reinterpret_cast<CBulanci*>(gzip)->CDSGZipStream_Ctor(reinterpret_cast<int*>(&mem), 0);

	gzipRead->ReadBytes(chain + kOffConfigSig, 1);
	if (chain[kOffConfigSig] != kConfigSignature) {
		CDSSimpleException::CDSSimpleException_Throw(9, 0xb);
	}

	uchar profileCount = 0;
	gzipRead->ReadBytes(&profileCount, 1);
	while (profileCount != 0) {
		profileCount--;
		auto* rec = reinterpret_cast<uchar*>(_Globals::OperatorNewWithBadAlloc(6));
		gzipRead->ReadBytes(rec, 6);
		_Globals::CIntListInsertSortedOrAppend(
		    reinterpret_cast<int>(chain + kOffProfileList),
		    rec,
		    nullptr,
		    1);
	}

	for (uint i = 0; i < 6; ++i) {
		uchar* slot = chain + kOffKeyBindings + i * 6;
		gzipRead->ReadBytes(slot + 0, 1);
		gzipRead->ReadBytes(slot + 1, 1);
		bul->FUN_0042e2f0(reinterpret_cast<int*>(slot + 2), reinterpret_cast<int*>(gzipRead));
	}

	gzipRead->ReadBytes(chain + kOffProfileUser, 4);
	gzipRead->ReadBytes(chain + kOffOptionBlock, 0x0e);
	gzipRead->ReadBytes(chain + kOffPanBinding, 4);
	gzipRead->ReadBytes(chain + kOffReservedAudio0, 4);
	gzipRead->ReadBytes(chain + kOffReservedAudio1, 4);
	bul->FUN_0042e2f0(reinterpret_cast<int*>(chain + kOffStrConfigTail),
	                  reinterpret_cast<int*>(gzipRead));

	chain[kOffField78] = 1;
	InvokeRegistryLoadVfn(chain, gzipRead);

	chain[kOffConfigSig] = kConfigSignature;
	chain[kOffRegistryCommit] = 1;

	gzip->CDSGZipStream_dtor(reinterpret_cast<uint*>(gzip));
	mem.CDSEasyMemStream_dtor(reinterpret_cast<uint*>(&mem));

	if (*reinterpret_cast<int*>(regKey) != 0) {
		_Globals::CDSRegKey_CloseKey(reinterpret_cast<uint*>(regKey));
	}

	_Globals::CDSAudio_SetPanPreview(*reinterpret_cast<int*>(chain + kOffPanBinding));
}

} // namespace

// !FUNC 0x004011f0 BEGIN
/* 4011F0-4011F6 00006 */
uchar* CDSChain::CDSChain_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b7cd8);
}
// !FUNC 0x004011f0 END

// !FUNC 0x00401200 BEGIN
/* 401200-401206 00006 */
uchar* CDSChain::CDSChain_Referenced_GetClassTable() {
    return reinterpret_cast<uchar*>(&DAT_004b7c98);
}
// !FUNC 0x00401200 END

// !FUNC 0x00401210 BEGIN
/* 401210-40122E 0001E */
void* CDSChain::CDSChain_Referenced_DtorScalar(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401210 END

// !FUNC 0x00401260 BEGIN
/* 401260-401268 00008 */
uchar CDSChain::CDSChain_OnChainEvent(uchar param_1) { STUB_BODY(); return 0; }
// !FUNC 0x00401260 END

// !FUNC 0x0040a440 BEGIN
/* 40A440-40A62E — CDSChain_LoadConfigFromRegistry(CDSChain_full *this).
   Spec: ghidra_analysis/engine/struct_recovery/CDSChain_LoadConfigFromRegistry.md */
void CDSChain::CDSChain_LoadConfigFromRegistry() {
	auto* chain = reinterpret_cast<uchar*>(this);
	try {
		LoadConfigFromRegistryBody(chain);
	} catch (...) {
		/* @0x0040a62f — original: virtual release on stack CDSGZipStream* @ [EBP+0x48] */
		CBulanci::CBulanci_InstallFactoryDefaults(chain);
		chain[kOffField78] = 1;
		_Globals::CDSAudio_SetPanPreview(*reinterpret_cast<int*>(chain + kOffPanBinding));
	}
}
// !FUNC 0x0040a440 END

// !FUNC 0x0040a62f BEGIN
/* 40A62F-40A64C — MSVC SEH catch slice; logic folded into CDSChain_LoadConfigFromRegistry */
uchar* CDSChain::Catch_0040a62f() { STUB_BODY(); return 0; }
// !FUNC 0x0040a62f END

// !FUNC 0x0040a680 BEGIN
/* 40A680-40A724 000A4 */
void* CDSChain::CDSChain_ctor(void* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0040a680 END

// !FUNC 0x0041aa40 BEGIN
/* 41AA40-41AA48 00008 */
uchar CDSChain::CDSChain_ReleaseChild(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0041aa40 END

// !FUNC 0x0042ac90 BEGIN
/* 42AC90-42AC94 00004 */
int CDSChain::CDSChain_AdjustThisOffset() {
    return reinterpret_cast<int>(this) - 4;
}
// !FUNC 0x0042ac90 END

// !FUNC 0x0042fb70 BEGIN
/* 42FB70-42FC2F 000BF */
uchar CDSChain::CDSChain_Append(int param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042fb70 END

// !FUNC 0x0042fc30 BEGIN
/* 42FC30-42FC97 00067 */
uchar CDSChain::CDSChain_Remove(int* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042fc30 END

// !FUNC 0x0042fcd0 BEGIN
/* 42FCD0-42FD2B 0005B */
uchar CDSChain::CDSChain_dtor(uint* param_1) { STUB_BODY(); return 0; }
// !FUNC 0x0042fcd0 END

