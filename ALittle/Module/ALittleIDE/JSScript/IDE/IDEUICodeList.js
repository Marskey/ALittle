{
if (typeof ALittleIDE === "undefined") window.ALittleIDE = {};
let ___all_struct = ALittle->GetAllStruct();

ALittle.RegStruct(1715346212, "ALittle.Event", {
name : "ALittle.Event", ns_name : "ALittle", rl_name : "Event", hash_code : 1715346212,
name_list : ["target"],
type_list : ["ALittle.EventDispatcher"],
option_map : {}
})
ALittle.RegStruct(1450277461, "ALittleIDE.IDECodeModuleInfo", {
name : "ALittleIDE.IDECodeModuleInfo", ns_name : "ALittleIDE", rl_name : "IDECodeModuleInfo", hash_code : 1450277461,
name_list : ["module_name","root_path"],
type_list : ["string","string"],
option_map : {}
})
ALittle.RegStruct(-889634858, "ALittleIDE.IDECodeFileSearchInfo", {
name : "ALittleIDE.IDECodeFileSearchInfo", ns_name : "ALittleIDE", rl_name : "IDECodeFileSearchInfo", hash_code : -889634858,
name_list : ["list","count","index","name"],
type_list : ["List<ALittleIDE.IDECodeTreeLogic>","int","int","string"],
option_map : {}
})

if (ALittle.DisplayLayout === undefined) throw new Error(" extends class:ALittle.DisplayLayout is undefined");
ALittleIDE.IDEUICodeList = JavaScript.Class(ALittle.DisplayLayout, {
	Ctor : function(ctrl_sys) {
		this._group = new Map();
	},
	TCtor : function() {
		ALittleIDE.g_IDEProject.AddEventListener(___all_struct.get(-975432877), this, this.HandleProjectOpen);
		ALittleIDE.g_IDEProject.AddEventListener(___all_struct.get(-332308624), this, this.HandleProjectClose);
	},
	HandleProjectClose : function(event) {
		this._code_scroll_screen.RemoveAllChild();
	},
	HandleProjectOpen : function(event) {
		let module_map = ALittleIDE.g_IDEProject.project.config.GetConfig("code_module", {});
		delete module_map["Core"];
		delete module_map["Std"];
		delete module_map["CEngine"];
		delete module_map[event.name];
		let info = {};
		info.module_name = event.name;
		info.name = "src";
		info.path = ALittle.File_BaseFilePath() + "Module/" + event.name + "/src";
		info.module_path = ALittle.File_BaseFilePath() + "Module/" + event.name + "/";
		info.group = this._group;
		info.root = true;
		info.project = ALittleIDE.g_IDEProject.project.code;
		this._code_scroll_screen.AddChild(ALittle.NewObject(ALittleIDE.IDECodeTree, ALittleIDE.g_Control, info));
		let std_list = ["Core", "Std", "CEngine"];
		let ___OBJECT_1 = std_list;
		for (let index = 1; index <= ___OBJECT_1.length; ++index) {
			let name = ___OBJECT_1[index - 1];
			if (name === undefined) break;
			info = {};
			info.module_name = name;
			info.name = "src";
			info.path = ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary/" + name + "/src";
			info.module_path = ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary/" + name + "/";
			info.group = this._group;
			info.root = true;
			info.project = ALittleIDE.g_IDEProject.project.code;
			let tree = ALittle.NewObject(ALittleIDE.IDECodeTree, ALittleIDE.g_Control, info);
			this._code_scroll_screen.AddChild(tree);
		}
		let ___OBJECT_2 = module_map;
		for (let index in ___OBJECT_2) {
			let module = ___OBJECT_2[index];
			if (module === undefined) continue;
			info = {};
			info.module_name = module.module_name;
			info.name = ALittle.File_GetFileNameByPath(module.root_path);
			info.path = module.root_path;
			info.module_path = ALittle.File_GetFilePathByPath(module.root_path) + "/";
			info.group = this._group;
			info.project = ALittleIDE.g_IDEProject.project.code;
			info.root = true;
			let tree = ALittle.NewObject(ALittleIDE.IDECodeTree, ALittleIDE.g_Control, info);
			this._code_scroll_screen.AddChild(tree);
		}
	},
	AddLibrary : function(name) {
		let file_map = ALittle.File_GetFileNameListByDir(ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary");
		let attr = file_map[name];
		if (attr === undefined || attr.mode !== "directory") {
			return false;
		}
		let module_map = ALittleIDE.g_IDEProject.project.config.GetConfig("code_module", {});
		let module_info = {};
		module_info.module_name = name;
		module_info.root_path = ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary/" + name + "/src";
		module_map[name] = module_info;
		ALittleIDE.g_IDEProject.project.config.SetConfig("code_module", module_map);
		let info = {};
		info.module_name = name;
		info.name = "src";
		info.path = ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary/" + name + "/src";
		info.module_path = ALittle.File_BaseFilePath() + "Module/ALittleIDE/Other/GameLibrary/" + name + "/";
		info.group = this._group;
		info.root = true;
		info.project = ALittleIDE.g_IDEProject.project.code;
		let tree = ALittle.NewObject(ALittleIDE.IDECodeTree, ALittleIDE.g_Control, info);
		this._code_scroll_screen.AddChild(tree);
		return true;
	},
	OpenByFullPath : function(full_path, line_start, char_start, line_end, char_end) {
		let ___OBJECT_3 = this._code_scroll_screen.childs;
		for (let index = 1; index <= ___OBJECT_3.length; ++index) {
			let child = ___OBJECT_3[index - 1];
			if (child === undefined) break;
			let item = child.FindFile(full_path);
			if (item !== undefined) {
				let code_child = ALittleIDE.g_IDECenter.center.content_edit.StartEditCodeBySelect(item.user_info.name, item.user_info);
				code_child.JumpFocus(line_start, char_start, line_end, char_end);
				return;
			}
		}
		if (ALittle.File_GetFileAttr(full_path) === undefined) {
			return;
		}
		let user_info = {};
		user_info.name = ALittle.File_GetFileNameByPath(full_path);
		user_info.path = full_path;
		ALittleIDE.g_IDECenter.center.content_edit.StartEditCodeBySelect(user_info.name, user_info);
	},
	AddModule : function(name) {
		let ___OBJECT_4 = this._code_scroll_screen.childs;
		for (let index = 1; index <= ___OBJECT_4.length; ++index) {
			let tree = ___OBJECT_4[index - 1];
			if (tree === undefined) break;
			if (tree.user_info.module_name === name) {
				return;
			}
		}
		let module_map = ALittleIDE.g_IDEProject.project.config.GetConfig("code_module", {});
		let module_info = {};
		module_info.module_name = name;
		module_info.root_path = ALittle.File_BaseFilePath() + "Module/" + name + "/src";
		module_map[name] = module_info;
		ALittleIDE.g_IDEProject.project.config.SetConfig("code_module", module_map);
		let info = {};
		info.module_name = name;
		info.name = ALittle.File_GetFileNameByPath(module_info.root_path);
		info.path = module_info.root_path;
		info.module_path = ALittle.File_BaseFilePath() + "Module/" + name + "/";
		info.group = this._group;
		info.root = true;
		info.project = ALittleIDE.g_IDEProject.project.code;
		let tree = ALittle.NewObject(ALittleIDE.IDECodeTree, ALittleIDE.g_Control, info);
		this._code_scroll_screen.AddChild(tree);
	},
	ShowTreeItemFocus : function(target) {
		target.ShowSelect();
		if (target !== this._code_scroll_screen) {
			let parent = target.logic_parent;
			while (parent !== undefined && parent !== this._code_scroll_screen) {
				parent.fold = true;
				parent = parent.logic_parent;
			}
		}
		this._code_scroll_screen.RejustScrollBar();
		let [x, y] = target.LocalToGlobal(this._code_scroll_screen.container);
		let target_x = (this._code_scroll_screen.view_width - target.width / 2) / 2 - x;
		let target_y = (this._code_scroll_screen.view_height - target.height) / 2 - y;
		if (this._tree_loop_x !== undefined) {
			this._tree_loop_x.Stop();
			this._tree_loop_x = undefined;
		}
		if (this._tree_loop_y !== undefined) {
			this._tree_loop_y.Stop();
			this._tree_loop_y = undefined;
		}
		this._tree_loop_x = ALittle.NewObject(ALittle.LoopLinear, this._code_scroll_screen, "container_x", target_x, 300, 0);
		this._tree_loop_x.Start();
		this._tree_loop_y = ALittle.NewObject(ALittle.LoopLinear, this._code_scroll_screen, "container_y", target_y, 300, 0);
		this._tree_loop_y.Start();
	},
	HandleCodeSearchClick : function(event) {
		if (this._search_info === undefined || this._search_info.name !== this._code_search_key.text) {
			this._search_info = {};
			this._search_info.name = this._code_search_key.text;
			this._search_info.index = 0;
			this._search_info.list = [];
			let ___OBJECT_5 = this._code_scroll_screen.childs;
			for (let index = 1; index <= ___OBJECT_5.length; ++index) {
				let child = ___OBJECT_5[index - 1];
				if (child === undefined) break;
				child.SearchFile(this._search_info.name, this._search_info.list);
			}
			this._search_info.count = ALittle.List_MaxN(this._search_info.list);
		}
		if (this._search_info.count <= 0) {
			return;
		}
		this._search_info.index = this._search_info.index + (1);
		if (this._search_info.index > this._search_info.count) {
			this._search_info.index = 1;
		}
		let item = this._search_info.list[this._search_info.index - 1];
		this.ShowTreeItemFocus(item);
	},
}, "ALittleIDE.IDEUICodeList");

}