#include "script.h"

#include <Windows.h>
#include <filesystem>
#include <winternal.h>
#include <macro.h>

gsf::script::script(std::string_view filepath_)
	: filepath(filepath_)
{
	DEBUG_COUT("\nConstructed gsf::script object # " << this->filepath << " # " << filepath_);
}

bool gsf::script::load()
{
	this->logs.clear();

	if (!this->script_file_exists())
		return false;

	// We use a std::unique_ptr in the stack so incase of failure we can just immediately return and automatically destroy the lua state
	// and this->lua_state will remain invalid (we use the bool operator to validate a script instance which internally just checks if a lua state exists)
	std::unique_ptr<sol::state> temp_lua_state = std::make_unique<sol::state>();
	if (!temp_lua_state)
		return false;

	temp_lua_state->open_libraries(sol::lib::string, sol::lib::math);

	auto load_res = temp_lua_state->script_file(this->filepath);

	if (!load_res.valid())
	{
		sol::error err_load_res = load_res;
		this->internal_log_error(err_load_res.what());
		return false;
	}

	sol::function callback_onload;
	{
		auto get_onload_res = temp_lua_state->operator[]("on_load");
		if (get_onload_res.valid())
		{
			callback_onload = get_onload_res;
		}
		else
		{
			this->internal_log_error("Callback \"on_load\" not found.");
			return false;
		}
	}

	if (!this->setup_script_api(temp_lua_state))
	{
		this->internal_log_error("Failed to load API in lua state");
		return false;
	}

	// upon success we can move the unique_ptr from this scope to the class
	this->lua_state = std::move(temp_lua_state);

	DEBUG_COUT("\nLoaded lua: " << this->filepath);
	this->logs.push_back("Successfuly loaded.");

	// Run the script
	callback_onload();

	++gsf::script::count_loaded_scripts;
	return true;
}

bool gsf::script::unload()
{
	if (!this->lua_state.operator bool())
		return false;

	this->logs.clear();

	DEBUG_COUT("\nUnloaded script: " << this->filepath);
	this->lua_state.reset();
	--gsf::script::count_loaded_scripts;
	return true;
}

bool gsf::script::script_file_exists()
{
	return std::filesystem::exists(this->filepath);
}

gsf::script::operator bool() const
{
	return this->lua_state.operator bool();
}

const std::vector<std::string> &gsf::script::get_logs()
{
	return this->logs;
}

const std::string_view gsf::script::get_filepath()
{
	return this->filepath;
}

void gsf::script::internal_log_error(std::string_view msg)
{
	DEBUG_COUT("\nLua failure from: " << this->filepath << "\nReason: " << msg);
	this->logs.emplace_back(msg.data());
}

bool gsf::script::setup_script_api(std::unique_ptr<sol::state> &state)
{
	// gsf namespace
	auto namespace_gsf = state->operator[]("gsf").get_or_create<sol::table>();
	namespace_gsf.set_function("log", &gsf::script::_api_gsf_log, this);

	// winternal namespace
	auto namespace_winternal = state->operator[]("winternal").get_or_create<sol::table>();
	namespace_winternal.set_function("find_module", &gsf::script::_api_winternal_find_module, this);

	return true;
}

void gsf::script::_api_gsf_log(std::string txt)
{
	DEBUG_COUT("\n[SCRIPT] " << txt);
	this->logs.push_back(txt);
}

sol::table gsf::script::_api_winternal_find_module(std::wstring mod_name)
{
	std::uintptr_t base = 0, size = 0;

	auto ldr = utils::ldr_data_table_entry_find(mod_name.c_str());

	if (ldr)
	{
		base = reinterpret_cast<std::uintptr_t>(ldr->dll_base);
		size = ldr->size_of_image;
	}

	return this->lua_state->create_table_with("base_address", base, "size", size);
}
