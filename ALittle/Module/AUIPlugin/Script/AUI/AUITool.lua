-- ALittle Generate Lua And Do Not Edit This Line!
module("AUIPlugin", package.seeall)

local ___pairs = pairs
local ___ipairs = ipairs


AUIToolOption = {
	NO = "NO",
	YES = "YES",
	CANCEL = "CANCEL",
}

AUITool = Lua.Class(nil, "AUIPlugin.AUITool")

function AUITool:Shutdown()
	if self._alert_dialog ~= nil then
		A_LayerManager:RemoveFromModal(self._alert_dialog)
		self._alert_dialog = nil
	end
	if self._notice_dialog ~= nil then
		A_LayerManager:RemoveFromModal(self._notice_dialog)
		self._notice_dialog = nil
	end
	if self._save_dialog ~= nil then
		A_LayerManager:RemoveFromModal(self._save_dialog)
		self._save_dialog = nil
	end
	if self._delete_dialog ~= nil then
		A_LayerManager:RemoveFromModal(self._delete_dialog)
		self._delete_dialog = nil
	end
	if self._rename_input ~= nil then
		A_LayerManager:RemoveFromModal(self._rename_input)
		self._rename_input = nil
	end
	if self._tip_help_dialog ~= nil then
		A_LayerManager:RemoveFromTip(self._tip_help_dialog)
		self._tip_help_dialog = nil
	end
	if self._selectdir_dialog ~= nil then
		A_LayerManager:RemoveFromModal(self._selectdir_dialog)
		self._selectdir_dialog = nil
	end
end

function AUITool:ShowAlertDialog(title, content)
	if self._alert_dialog == nil then
		self._alert_dialog = g_Control:CreateControl("ide_alert_dialog", self)
		A_LayerManager:AddToModal(self._alert_dialog)
	end
	self._alert_dialog.visible = true
	self._alert_dialog:MoveToTop()
	self._alert_dialog.title = title
	self._content.text = content
end

function AUITool:HideAlertDialog()
	self._alert_dialog.visible = false
end

function AUITool:ShowNotice(title, content)
	if self._notice_dialog == nil then
		self._notice_dialog = g_Control:CreateControl("ide_notice_dialog", self)
		A_LayerManager:AddToModal(self._notice_dialog)
		self._notice_min_size = self._notice_content.height
	end
	self._notice_dialog.visible = true
	self._notice_dialog:MoveToTop()
	self._notice_dialog.title = title
	self._notice_content.text = content
	local real_height = self._notice_content.real_height
	if real_height < self._notice_min_size then
		real_height = self._notice_min_size
	end
	local dialog_body_height = real_height + self._notice_content.y + self._notice_content.height_value
	self._notice_dialog.height = dialog_body_height + self._notice_dialog.head_size
end

function AUITool:HandleNoticeConfirmClick(event)
	self._notice_dialog.visible = false
end

function AUITool:SaveNotice(title, content)
	local ___COROUTINE = coroutine.running()
	if self._save_dialog == nil then
		self._save_dialog = g_Control:CreateControl("ide_save_dialog", self)
		A_LayerManager:AddToModal(self._save_dialog)
	end
	self._save_dialog.visible = true
	self._save_dialog:MoveToTop()
	self._save_dialog.title = title
	self._save_content.text = content
	self._save_dialog._user_data = ___COROUTINE
	return coroutine.yield()
end

function AUITool:HandleSaveConfirmClick(event)
	self._save_dialog.visible = false
	local thread = self._save_dialog._user_data
	if thread == nil then
		return
	end
	self._save_dialog._user_data = nil
	ALittle.Coroutine.Resume(thread, AUIToolOption.YES)
end

function AUITool:HandleNoSaveClick(event)
	self._save_dialog.visible = false
	local thread = self._save_dialog._user_data
	if thread == nil then
		return
	end
	self._save_dialog._user_data = nil
	ALittle.Coroutine.Resume(thread, AUIToolOption.NO)
end

function AUITool:HandleSaveCancelClick(event)
	self._save_dialog.visible = false
	local thread = self._save_dialog._user_data
	if thread == nil then
		return
	end
	self._save_dialog._user_data = nil
	ALittle.Coroutine.Resume(thread, AUIToolOption.CANCEL)
end

function AUITool:DeleteNotice(title, content)
	local ___COROUTINE = coroutine.running()
	if self._delete_dialog == nil then
		self._delete_dialog = g_Control:CreateControl("ide_delete_dialog", self)
		A_LayerManager:AddToModal(self._delete_dialog)
		self._delete_delta_height = self._delete_dialog.height - self._delete_content.height
	end
	self._delete_dialog.visible = true
	self._delete_dialog:MoveToTop()
	self._delete_dialog.title = title
	self._delete_content.text = content
	self._delete_dialog.height = self._delete_delta_height + self._delete_content.real_height
	self._delete_dialog._user_data = ___COROUTINE
	return coroutine.yield()
end

function AUITool:HandleDeleteConfirmClick(event)
	self._delete_dialog.visible = false
	local thread = self._delete_dialog._user_data
	if thread == nil then
		return
	end
	self._delete_dialog._user_data = nil
	ALittle.Coroutine.Resume(thread, AUIToolOption.YES)
end

function AUITool:HandleDeleteCancelClick(event)
	self._delete_dialog.visible = false
	local thread = self._delete_dialog._user_data
	if thread == nil then
		return
	end
	self._delete_dialog._user_data = nil
	ALittle.Coroutine.Resume(thread, AUIToolOption.CANCEL)
end

function AUITool:ShowRename(text, x, y, width)
	local ___COROUTINE = coroutine.running()
	if self._rename_input == nil then
		self._rename_input = g_Control:CreateControl("ide_rename_image_input", self)
		A_LayerManager:AddToModal(self._rename_input)
	end
	self._rename_input.visible = true
	A_UISystem.focus = self._rename_input.show_input
	self._rename_input.x = x
	self._rename_input.y = y
	self._rename_input.width = width
	self._rename_input.text = text
	if text ~= "" then
		self._rename_input:SelectAll()
	end
	self._rename_input._user_data = ___COROUTINE
	return coroutine.yield()
end

function AUITool:HandleRenameConfirm(event)
	self._rename_input.visible = false
	local thread = self._rename_input._user_data
	if thread == nil then
		return
	end
	self._rename_input._user_data = nil
	local text = self._rename_input.text
	ALittle.Coroutine.Resume(thread, text)
end

function AUITool:HandleRenameCancel(event)
	if event.sym ~= 27 then
		return
	end
	local thread = self._rename_input._user_data
	if thread == nil then
		return
	end
	self._rename_input._user_data = nil
	self._rename_input.visible = false
	ALittle.Coroutine.Resume(thread, nil)
end

function AUITool:ShowSelectDir(title, dir)
	local ___COROUTINE = coroutine.running()
	if self._selectdir_dialog == nil then
		self._selectdir_dialog = g_Control:CreateControl("ide_common_selectdir_dialog", self)
		A_LayerManager:AddToModal(self._selectdir_dialog)
	end
	self._selectdir_dialog.visible = true
	self._selectdir_input:DelayFocus()
	if dir ~= nil then
		self._selectdir_input.text = dir
	end
	self._selectdir_dialog._user_data = ___COROUTINE
	return coroutine.yield()
end

function AUITool:HandleSelectDirConfirm(event)
	self._selectdir_dialog.visible = false
	local thread = self._selectdir_dialog._user_data
	if thread == nil then
		return
	end
	self._selectdir_dialog._user_data = nil
	local text = self._selectdir_input.text
	ALittle.Coroutine.Resume(thread, text)
end

function AUITool:HandleSelectDirCancel(event)
	local thread = self._selectdir_dialog._user_data
	if thread == nil then
		return
	end
	self._selectdir_dialog._user_data = nil
	self._selectdir_dialog.visible = false
	ALittle.Coroutine.Resume(thread, nil)
end

function AUITool:HandleSelectDirClick(event)
	if event.path ~= nil then
		self._selectdir_input.text = event.path
	end
end

function AUITool:ShowTipHelp(content, show_time)
	if self._tip_help_dialog == nil then
		self._tip_help_dialog = g_Control:CreateControl("ide_tip_help", self)
		A_LayerManager:AddToTip(self._tip_help_dialog)
	end
	if self._tip_help_loop ~= nil then
		if A_LoopSystem:HasUpdater(self._tip_help_loop) then
			self._tip_help_loop:SetCompleted()
			A_LoopSystem:RemoveUpdater(self._tip_help_loop)
		end
		self._tip_help_loop = nil
	end
	self._tip_help_dialog.visible = true
	self._tip_help_dialog.x = A_UISystem.view_width
	self._tip_help_dialog.y = 0
	self._tip_help_content.text = content
	local target_x = A_UISystem.view_width - self._tip_help_dialog.width
	if show_time == nil then
		show_time = 10000
	end
	self._tip_help_loop = ALittle.LoopList()
	local move_in = ALittle.LoopLinear(self._tip_help_dialog, "x", target_x, 1000, 0)
	self._tip_help_loop:AddUpdater(move_in)
	local move_out = ALittle.LoopLinear(self._tip_help_dialog, "x", A_UISystem.view_width, 1000, show_time)
	self._tip_help_loop:AddUpdater(move_out)
	local set_value = ALittle.LoopAttribute(self._tip_help_dialog, "visible", false, 1)
	self._tip_help_loop:AddUpdater(set_value)
	A_LoopSystem:AddUpdater(self._tip_help_loop)
end

function AUITool:HandleTipHelpCloseClick(event)
	if self._tip_help_loop ~= nil then
		if A_LoopSystem:HasUpdater(self._tip_help_loop) then
			self._tip_help_loop:SetCompleted()
			A_LoopSystem:RemoveUpdater(self._tip_help_loop)
		end
		self._tip_help_loop = nil
	end
	self._tip_help_dialog.visible = false
end

function AUITool:ShowTip(content)
	if self._tip_dialog == nil then
		self._tip_dialog = g_Control:CreateControl("ide_tool_tip", self)
		A_LayerManager:AddToTip(self._tip_dialog)
	end
	self._tip_dialog.visible = true
	self._tip_text.text = content
	self._tip_dialog.width = self._tip_text.width + 10
	self._tip_dialog.height = self._tip_text.height + 10
end

function AUITool:GetTipSize()
	return self._tip_dialog.width, self._tip_dialog.height
end

function AUITool:MoveTip(x, y)
	if self._tip_dialog == nil then
		return
	end
	self._tip_dialog.x = x
	self._tip_dialog.y = y
end

function AUITool:HideTip()
	if self._tip_dialog == nil then
		return
	end
	self._tip_dialog.visible = false
end

_G.g_AUITool = AUITool()
