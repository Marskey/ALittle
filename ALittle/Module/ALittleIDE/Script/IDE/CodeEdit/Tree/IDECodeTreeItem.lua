-- ALittle Generate Lua And Do Not Edit This Line!
module("ALittleIDE", package.seeall)

local ___rawset = rawset
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
ALittle.RegStruct(-449066808, "ALittle.UIClickEvent", {
name = "ALittle.UIClickEvent", ns_name = "ALittle", rl_name = "UIClickEvent", hash_code = -449066808,
name_list = {"target","is_drag"},
type_list = {"ALittle.DisplayObject","bool"},
option_map = {}
})

assert(ALittleIDE.IDECodeTreeLogic, " extends class:ALittleIDE.IDECodeTreeLogic is nil")
IDECodeTreeItem = Lua.Class(ALittleIDE.IDECodeTreeLogic, "ALittleIDE.IDECodeTreeItem")

function IDECodeTreeItem:Ctor(ctrl_sys, user_info)
	___rawset(self, "_item", ctrl_sys:CreateControl("ide_code_tree_item", self))
	self:AddChild(self._item)
	self.width = self._item.width
	self.height = self._item.height
	self._item_button.selected = false
	self._item_button.group = user_info.group
	self._item_button:AddEventListener(___all_struct[-449066808], self, self.HandleClick)
	self._item_button:AddEventListener(___all_struct[-641444818], self, self.HandleRButtonDown)
	self._item_button._user_data = self
	self._item_title.text = self._user_info.name
end

function IDECodeTreeItem.__getter:is_tree()
	return false
end

function IDECodeTreeItem.__getter:fold()
	return false
end

function IDECodeTreeItem.__setter:fold(value)
end

function IDECodeTreeItem:HandleClick(event)
	g_IDECenter.center.content_edit:StartEditCodeBySelect(self._user_info.name, self._user_info)
end

function IDECodeTreeItem:SearchFile(name, list)
	if list == nil then
		list = {}
	end
	if ALittle.String_Find(self._user_info.name, name) ~= nil then
		ALittle.List_Push(list, self)
	end
	return list
end

function IDECodeTreeItem:HandleRButtonDown(event)
	local menu = AUIPlugin.AUIRightMenu()
	menu:AddItem("删除", Lua.Bind(self.HandleDeleteFile, self))
	menu:Show()
end

function IDECodeTreeItem:HandleDeleteFile()
	local file_name = ALittle.File_GetFileNameByPath(self._user_info.path)
	local result = g_AUITool:DeleteNotice("删除", "确定要删除" .. file_name .. "吗?")
	if result ~= "YES" then
		return
	end
	ALittle.File_DeleteFile(self._user_info.path)
	self:RemoveFromParent()
end
IDECodeTreeItem.HandleDeleteFile = Lua.CoWrap(IDECodeTreeItem.HandleDeleteFile)

