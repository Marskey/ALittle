-- ALittle Generate Lua And Do Not Edit This Line!
module("ALittleIDE", package.seeall)

local ___pairs = pairs
local ___ipairs = ipairs
local ___all_struct = ALittle.GetAllStruct()

ALittle.RegStruct(-1479093282, "ALittle.UIEvent", {
name = "ALittle.UIEvent", ns_name = "ALittle", rl_name = "UIEvent", hash_code = -1479093282,
name_list = {"target"},
type_list = {"ALittle.DisplayObject"},
option_map = {}
})
ALittle.RegStruct(-1347278145, "ALittle.UIButtonEvent", {
name = "ALittle.UIButtonEvent", ns_name = "ALittle", rl_name = "UIButtonEvent", hash_code = -1347278145,
name_list = {"target","abs_x","abs_y","rel_x","rel_y","count","is_drag"},
type_list = {"ALittle.DisplayObject","double","double","double","double","int","bool"},
option_map = {}
})
ALittle.RegStruct(-641444818, "ALittle.UIRButtonDownEvent", {
name = "ALittle.UIRButtonDownEvent", ns_name = "ALittle", rl_name = "UIRButtonDownEvent", hash_code = -641444818,
name_list = {"target","abs_x","abs_y","rel_x","rel_y","count","is_drag"},
type_list = {"ALittle.DisplayObject","double","double","double","double","int","bool"},
option_map = {}
})

assert(ALittle.DisplayLayout, " extends class:ALittle.DisplayLayout is nil")
IDEUIProjectList = Lua.Class(ALittle.DisplayLayout, "ALittleIDE.IDEUIProjectList")

function IDEUIProjectList:OpenLastProject()
	if g_IDEProject.project ~= nil then
		return
	end
	local project_name = g_IDEConfig:GetConfig("last_project", "")
	if project_name == "" then
		return
	end
	self:OpenProjectImpl(project_name)
end

function IDEUIProjectList:HandleProjectSearchClick(event)
	self._project_scroll_screen:RemoveAllChild()
	local key = self._project_search_key.text
	local project_map = g_IDEConfig:GetConfig("project_map", {})
	for k, v in ___pairs(project_map) do
		if key == "" or ALittle.String_Find(v, key) ~= nil then
			local item = g_Control:CreateControl("ide_common_item_button")
			item.text = v
			item.drag_trans_target = self._project_scroll_screen
			item:AddEventListener(___all_struct[-641444818], self, self.HandleProjectItemRightClick)
			self._project_scroll_screen:AddChild(item)
		end
	end
end

function IDEUIProjectList:ShowNewProject()
	if self._project_new_dialog == nil then
		self._project_new_dialog = g_Control:CreateControl("ide_new_project_dialog", self)
		A_LayerManager:AddToModal(self._project_new_dialog)
	end
	self._project_new_name.text = ""
	self._project_new_dialog.visible = true
	A_UISystem.focus = self._project_new_name.show_input
end

function IDEUIProjectList:HandleNewProjectCancel(event)
	self._project_new_dialog.visible = false
end

function IDEUIProjectList:HandleNewProjectConfirm(event)
	local name = self._project_new_name.text
	if name == "" then
		g_AUITool:ShowNotice("错误", "请输入项目名")
		return
	end
	if IDEUIUtility_CheckName(name) == false then
		g_AUITool:ShowNotice("错误", "项目名不合法:" .. name)
		return
	end
	if ALittle.File_GetFileAttr(ALittle.File_BaseFilePath() .. name) ~= nil then
		g_AUITool:ShowNotice("错误", "项目已存在:" .. name)
		return
	end
	local size_list = ALittle.String_Split(self._view_size_dropdown.text, "x")
	local window_width = ALittle.Math_ToInt(size_list[1])
	if window_width == nil then
		window_width = 800
	end
	local window_height = ALittle.Math_ToInt(size_list[2])
	if window_height == nil then
		window_height = 600
	end
	local font_path = self._default_font_path.text
	local font_size = ALittle.Math_ToInt(self._default_font_size.text)
	if font_size == nil then
		font_size = 25
	end
	g_IDEProject:NewProject(name, ALittle.Math_Floor(window_width), ALittle.Math_Floor(window_height), font_path, ALittle.Math_Floor(font_size))
	if g_IDEProject.project == nil then
		g_IDECenter:OpenProject(name)
	end
	self._project_new_dialog.visible = false
end

function IDEUIProjectList:OpenProjectImpl(name)
	if name == "" then
		g_AUITool:ShowNotice("错误", "请输入项目名")
		return
	end
	if ALittle.File_GetFileAttr(ALittle.File_BaseFilePath() .. "Module/" .. name) == nil then
		g_AUITool:ShowNotice("错误", "项目不存在:" .. name)
		return
	end
	g_IDECenter:CloseProject()
	g_IDECenter:OpenProject(name)
end

function IDEUIProjectList:ShowOpenProject()
	if self._project_open_dialog == nil then
		self._project_open_dialog = g_Control:CreateControl("ide_open_project_dialog", self)
		A_LayerManager:AddToModal(self._project_open_dialog)
	end
	self._project_open_name.text = ""
	self._project_open_dialog.visible = true
	local module_list = {}
	local file_map = ALittle.File_GetFileNameListByDir(g_ModuleBasePath .. "..")
	for name, info in ___pairs(file_map) do
		if info.mode == "directory" then
			ALittle.List_Push(module_list, name)
		end
	end
	self._project_dropdown.data_list = module_list
	self._project_dropdown.text = ""
	A_UISystem.focus = self._project_open_name.show_input
end

function IDEUIProjectList:HandleOpenProjectSelect(event)
	self._project_open_name.text = event.target.text
	event.target.text = ""
end

function IDEUIProjectList:HandleOpenProjectCancel(event)
	self._project_open_dialog.visible = false
end

function IDEUIProjectList:HandleOpenProjectConfirm(event)
	self._project_open_dialog.visible = false
	local name = self._project_open_name.text
	if not g_IDETabManager:IsSaveAll() then
		local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
		if result == "YES" then
			g_IDETabManager:SaveAllTab()
		end
	end
	self:OpenProjectImpl(name)
end
IDEUIProjectList.HandleOpenProjectConfirm = Lua.CoWrap(IDEUIProjectList.HandleOpenProjectConfirm)

function IDEUIProjectList:ShowSettingProject(event)
	if g_IDEProject.project == nil then
		g_AUITool:ShowNotice("错误", "当前没有打开的项目")
		return
	end
	if self._project_setting_dialog == nil then
		self._project_setting_dialog = g_Control:CreateControl("ide_setting_project_dialog", self)
		A_LayerManager:AddToModal(self._project_setting_dialog)
	end
	self._project_setting_dialog.visible = true
	self._default_show_width.text = g_IDEProject.project.config:GetConfig("default_show_width", 800)
	self._default_show_height.text = g_IDEProject.project.config:GetConfig("default_show_height", 600)
	self._default_font_path.text = g_IDEProject.project.config:GetConfig("default_font_path", "")
	self._default_font_size.text = g_IDEProject.project.config:GetConfig("default_font_size", 15)
end

function IDEUIProjectList:HandleSettingProjectCancel(event)
	self._project_setting_dialog.visible = false
end

function IDEUIProjectList:HandleSettingProjectConfirm(event)
	local default_show_width = ALittle.Math_ToInt(self._default_show_width.text)
	if default_show_width == nil or default_show_width <= 0 then
		g_AUITool:ShowNotice("错误", "默认宽度不合法")
		return
	end
	local default_show_height = ALittle.Math_ToInt(self._default_show_height.text)
	if default_show_height == nil or default_show_height <= 0 then
		g_AUITool:ShowNotice("错误", "默认高度不合法")
		return
	end
	local default_font_path = self._default_font_path.text
	local default_font_size = ALittle.Math_ToInt(self._default_font_size.text)
	if default_font_size == nil or default_font_size <= 0 then
		g_AUITool:ShowNotice("错误", "默认字体大小不合法")
		return
	end
	g_IDEProject.project.config:SetConfig("default_show_width", default_show_width)
	g_IDEProject.project.config:SetConfig("default_show_height", default_show_height)
	g_IDEProject.project.config:SetConfig("default_font_path", default_font_path)
	g_IDEProject.project.config:SetConfig("default_font_size", default_font_size)
	g_IDETabManager:SetTabChildWH(default_show_width, default_show_height)
	self._project_setting_dialog.visible = false
end

function IDEUIProjectList:HandleProjectItemRightClick(event)
	local menu = AUIPlugin.AUIRightMenu()
	menu:AddItem("打开", Lua.Bind(self.HandleRightProjectOpen, self, event.target))
	menu:AddItem("关闭", Lua.Bind(self.HandleRightProjectClose, self, event.target))
	menu:AddItem("刷新", Lua.Bind(self.HandleRightProjectRefresh, self, event.target))
	menu:AddItem("移除", Lua.Bind(self.HandleRightProjectRemove, self, event.target))
end

function IDEUIProjectList:HandleRightProjectOpen(target)
	local project_name = target.text
	if not g_IDETabManager:IsSaveAll() then
		local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
		if result == "YES" then
			g_IDETabManager:SaveAllTab()
		end
	end
	self:OpenProjectImpl(project_name)
end
IDEUIProjectList.HandleRightProjectOpen = Lua.CoWrap(IDEUIProjectList.HandleRightProjectOpen)

function IDEUIProjectList:HandleRightProjectRemove(target)
	local project_name = target.text
	if g_IDEProject.project ~= nil and g_IDEProject.project.name == project_name then
		g_AUITool:ShowNotice("提示", "当前项目正在编辑，请先关闭")
		return
	end
	self._project_scroll_screen:RemoveChild(target)
	g_IDEProject:RemoveProject(project_name)
end

function IDEUIProjectList:HandleRightProjectClose(target)
	local project_name = target.text
	if g_IDEProject.project == nil or g_IDEProject.project.name ~= project_name then
		return
	end
	if not g_IDETabManager:IsSaveAll() then
		local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
		if result == "YES" then
			g_IDETabManager:SaveAllTab()
		end
	end
	g_IDECenter:CloseProject()
end
IDEUIProjectList.HandleRightProjectClose = Lua.CoWrap(IDEUIProjectList.HandleRightProjectClose)

function IDEUIProjectList:RefreshProject()
	if not g_IDETabManager:IsSaveAll() then
		local result = g_AUITool:SaveNotice("提示", "是否保存当前项目?")
		if result == "YES" then
			g_IDETabManager:SaveAllTab()
		end
	end
	g_IDECenter:RefreshProject()
end
IDEUIProjectList.RefreshProject = Lua.CoWrap(IDEUIProjectList.RefreshProject)

function IDEUIProjectList:HandleRightProjectRefresh(target)
	local project_name = target.text
	if g_IDEProject.project == nil or g_IDEProject.project.name ~= project_name then
		return
	end
	self:RefreshProject()
end

function IDEUIProjectList:ShowExportProject()
	if g_IDEProject.project == nil then
		g_AUITool:ShowNotice("错误", "当前没有打开的项目")
		return
	end
	if self._export_dialog == nil then
		self._export_dialog = g_Control:CreateControl("ide_export_dialog", self)
		g_DialogLayer:AddChild(self._export_dialog)
		self._main_export_tab:DisableAllCloseButton()
		self._main_export_tab.tab_index = 1
		self._export_windows_official.config_key = "export_windows_official"
		self._export_android_official.config_key = "export_android_official"
		self._export_ios_official.config_key = "export_ios_official"
	end
	self._export_dialog.visible = true
	self._export_windows_official:LoadConfigImpl()
	self._export_android_official:LoadConfigImpl()
	self._export_ios_official:LoadConfigImpl()
end

function IDEUIProjectList:RunProject()
	if g_IDEProject.project == nil then
		g_AUITool:ShowNotice("提示", "当前没有打开的项目")
		return
	end
	os.execute("start ALittleClient.exe " .. g_IDEProject.project.name .. " debug")
end
