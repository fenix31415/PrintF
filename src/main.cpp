#include "PrintF.h"

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

template class params_array<RE::BSFixedString>;
template class params_string<RE::BSFixedString>;

RE::BSFixedString PrintFStr(RE::StaticFunctionTag*, const RE::BSFixedString format, const RE::BSFixedString params)
{
	return Fprints(format.c_str(), params);
}

RE::BSFixedString PrintFArr(RE::StaticFunctionTag*, const RE::BSFixedString format, const std::vector<RE::BSFixedString> params)
{
	return Fprinta(format.c_str(), params);
}

RE::BSFixedString PrintF9(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4
	, const RE::BSFixedString a5, const RE::BSFixedString a6, const RE::BSFixedString a7, const RE::BSFixedString a8
	, const RE::BSFixedString a9) {
	return Fprintv(format.c_str(), a1, a2, a3, a4, a5, a6, a7, a8, a9);
}
RE::BSFixedString PrintF8(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4
	, const RE::BSFixedString a5, const RE::BSFixedString a6, const RE::BSFixedString a7, const RE::BSFixedString a8) {
	return Fprintv(format.c_str(), a1, a2, a3, a4, a5, a6, a7, a8);
}
RE::BSFixedString PrintF7(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4
	, const RE::BSFixedString a5, const RE::BSFixedString a6, const RE::BSFixedString a7) {
	return Fprintv(format.c_str(), a1, a2, a3, a4, a5, a6, a7);
}
RE::BSFixedString PrintF6(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4
	, const RE::BSFixedString a5, const RE::BSFixedString a6) {
	return Fprintv(format.c_str(), a1, a2, a3, a4, a5, a6);
}
RE::BSFixedString PrintF5(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4
	, const RE::BSFixedString a5) {
	return Fprintv(format.c_str(), a1, a2, a3, a4, a5);
}
RE::BSFixedString PrintF4(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3, const RE::BSFixedString a4) {
	return Fprintv(format.c_str(), a1, a2, a3, a4);
}
RE::BSFixedString PrintF3(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2, const RE::BSFixedString a3) {
	return Fprintv(format.c_str(), a1, a2, a3);
}
RE::BSFixedString PrintF2(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1, const RE::BSFixedString a2) {
	return Fprintv(format.c_str(), a1, a2);
}
RE::BSFixedString PrintF1(RE::StaticFunctionTag*, const RE::BSFixedString format
	, const RE::BSFixedString a1) {
	return Fprintv(format.c_str(), a1);
}
RE::BSFixedString PrintF0(RE::StaticFunctionTag*, const RE::BSFixedString format) {
	return Fprintv(format.c_str());
}

bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	a_vm->RegisterFunction("PrintF0", "f314PF_PrintF", PrintF0);
	a_vm->RegisterFunction("PrintF1", "f314PF_PrintF", PrintF1);
	a_vm->RegisterFunction("PrintF2", "f314PF_PrintF", PrintF2);
	a_vm->RegisterFunction("PrintF3", "f314PF_PrintF", PrintF3);
	a_vm->RegisterFunction("PrintF4", "f314PF_PrintF", PrintF4);
	a_vm->RegisterFunction("PrintF5", "f314PF_PrintF", PrintF5);
	a_vm->RegisterFunction("PrintF6", "f314PF_PrintF", PrintF6);
	a_vm->RegisterFunction("PrintF7", "f314PF_PrintF", PrintF7);
	a_vm->RegisterFunction("PrintF8", "f314PF_PrintF", PrintF8);
	a_vm->RegisterFunction("PrintF9", "f314PF_PrintF", PrintF9);

	a_vm->RegisterFunction("PrintFArr", "f314PF_PrintF", PrintFArr);
	a_vm->RegisterFunction("PrintFStr", "f314PF_PrintF", PrintFStr);
	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	auto papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus->Register(RegisterFuncs)) {
		return false;
	}
	return true;
}
