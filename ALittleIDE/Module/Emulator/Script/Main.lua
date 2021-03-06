-- ALittle Generate Lua And Do Not Edit This Line!
do
if _G.Emulator == nil then _G.Emulator = {} end
local ___pairs = pairs
local ___ipairs = ipairs


function Emulator.__Browser_Setup(layer_group, control, module_base_path, script_base_path)
	local window_width, window_height, flag, scale = ALittle.System_CalcLandscape(1200, 600, 0x00000020)
	ALittle.System_CreateView("Emulator", window_width, window_height, flag, scale)
	ALittle.System_SetViewIcon(module_base_path .. "Other/ic_launcher.png")
	A_ModuleSystem:LoadModule(module_base_path, "Emulator")
end
Emulator.__Browser_Setup = Lua.CoWrap(Emulator.__Browser_Setup)

function Emulator.__Browser_AddModule(module_name, layer_group, module_info)
end

function Emulator.__Browser_Shutdown()
end

Emulator.g_Control = nil
Emulator.g_LayerGroup = nil
Emulator.g_ModuleBasePath = nil
Emulator.g_AUIPluinControl = nil
Emulator.g_ADeeplearningControl = nil
Emulator.g_VersionManager = nil
function Emulator.__Module_Setup(layer_group, control, module_base_path, script_base_path, debug)
	Emulator.g_Control = control
	Emulator.g_LayerGroup = layer_group
	Emulator.g_ModuleBasePath = module_base_path
	Emulator.g_AUIPluinControl = A_ModuleSystem:LoadPlugin("AUIPlugin")
	if ALittle.System_GetPlatform() == "Windows" then
		package.cpath = package.cpath .. ";./" .. module_base_path .. "Other/?.dll"
		require("memory")
		require("protobuf")
		require("socket")
	end
	Require(script_base_path, "GCenter")
	Require(script_base_path, "GClient")
	Require(script_base_path, "GRobot")
	Require(script_base_path, "GBlueprint")
	Require(script_base_path, "RobotStep")
	Require(script_base_path, "IDETree/IDETreeLogic")
	Require(script_base_path, "IDETree/IDETree")
	Require(script_base_path, "IDETree/IDETreeValue")
	Require(script_base_path, "IDETree/IDETreeEnum")
	Require(script_base_path, "IDETree/IDETreeBool")
	Require(script_base_path, "IDETree/IDETreeRepeatedValue")
	Require(script_base_path, "IDETree/IDETreeRepeatedEnum")
	Require(script_base_path, "IDETree/IDETreeRepeatedBool")
	Require(script_base_path, "IDETree/IDETreeRepeatedMessage")
	Require(script_base_path, "IDETree/IDETreeBoolMapBool")
	Require(script_base_path, "IDETree/IDETreeBoolMapEnum")
	Require(script_base_path, "IDETree/IDETreeBoolMapValue")
	Require(script_base_path, "IDETree/IDETreeBoolMapMessage")
	Require(script_base_path, "IDETree/IDETreeEnumMapBool")
	Require(script_base_path, "IDETree/IDETreeEnumMapEnum")
	Require(script_base_path, "IDETree/IDETreeEnumMapValue")
	Require(script_base_path, "IDETree/IDETreeEnumMapMessage")
	Require(script_base_path, "IDETree/IDETreeValueMapBool")
	Require(script_base_path, "IDETree/IDETreeValueMapEnum")
	Require(script_base_path, "IDETree/IDETreeValueMapValue")
	Require(script_base_path, "IDETree/IDETreeValueMapMessage")
	Require(script_base_path, "IDETree/IDETreeMessage")
	Require(script_base_path, "IDETree/IDETreeMap")
	Require(script_base_path, "IDETree/IDETreeRepeated")
	Require(script_base_path, "GUtility")
	Require(script_base_path, "PluginSocket")
	g_GCenter:Setup()
	Emulator.g_VersionManager = AUIPlugin.AUIVersionManager("139.159.176.119", 1100, "alittle", "Emulator")
	if A_ModuleSystem:GetDebugInfo() ~= "debug" then
		Emulator.g_VersionManager:CheckVersionUpdate()
	end
end
Emulator.__Module_Setup = Lua.CoWrap(Emulator.__Module_Setup)

function Emulator.__Module_Shutdown()
	Emulator.g_VersionManager:Shutdown()
	g_GCenter:Shutdown()
end

function Emulator.__Module_GetInfo(control, module_base_path, script_base_path)
	local info = {}
	info.title = "Emulator"
	info.icon = nil
	info.width_type = 4
	info.width_value = 0
	info.height_type = 4
	info.height_value = 0
	return info
end

end