-- ALittle Generate Lua And Do Not Edit This Line!
do
if _G.SuperMarioBros == nil then _G.SuperMarioBros = {} end
local ___rawset = rawset
local ___pairs = pairs
local ___ipairs = ipairs


SuperMarioBros.PlayerState = {
	PS_IDLE = 0,
	PS_WALK = 1,
	PS_JUMP = 2,
}

assert(ALittle.DisplayLayout, " extends class:ALittle.DisplayLayout is nil")
SuperMarioBros.BattlePlayer = Lua.Class(ALittle.DisplayLayout, "SuperMarioBros.BattlePlayer")

function SuperMarioBros.BattlePlayer:Ctor()
	___rawset(self, "_right", true)
	___rawset(self, "_level", 1)
	___rawset(self, "_speed", 0)
	___rawset(self, "_speed_rate", 0)
	___rawset(self, "_walk_frame_change", 0.0)
	___rawset(self, "_walk_frame", 1)
end

function SuperMarioBros.BattlePlayer:TCtor()
	self._level_1_sprite_right.visible = false
	self._level_1_sprite_left.visible = false
	self._level_2_sprite_right.visible = false
	self._level_2_sprite_left.visible = false
	self._level_3_sprite_right.visible = false
	self._level_3_sprite_left.visible = false
end

function SuperMarioBros.BattlePlayer:Init(row, col, level)
	self._state = SuperMarioBros.PlayerState.PS_IDLE
	self._speed = 0
	self._right = true
	self._level = level
	self._level = 1
	if self._level == 1 then
		self._level_1_sprite_right.visible = true
		self.height = self._level_1_sprite_right.height
		self._level_1_sprite_right.col_index = 1
		self._level_1_sprite_right.row_index = 1
		self.x = col * SuperMarioBros.TILE_WIDTH
		self.y = row * SuperMarioBros.TILE_HEIGHT - self._level_1_sprite_right.height
	elseif self._level == 2 then
		self._level_2_sprite_right.visible = true
		self.height = self._level_2_sprite_right.height
		self._level_2_sprite_right.col_index = 2
		self.x = col * SuperMarioBros.TILE_WIDTH
		self.y = row * SuperMarioBros.TILE_HEIGHT - self._level_2_sprite_right.height
	elseif self._level == 3 then
		self._level_3_sprite_right.visible = true
		self.height = self._level_3_sprite_right.height
		self._level_3_sprite_right.col_index = 2
		self.x = col * SuperMarioBros.TILE_WIDTH
		self.y = row * SuperMarioBros.TILE_HEIGHT - self._level_3_sprite_right.height
	end
end

function SuperMarioBros.BattlePlayer:UpdateFrame(frame_time)
	if A_UISystem.sym_map[100] ~= nil then
		if self._state == SuperMarioBros.PlayerState.PS_JUMP then
		else
			if not self._right then
				self._speed_rate = 0
				self._speed = 0
				if self._level == 1 then
					self._level_1_sprite_right.visible = true
					self._level_1_sprite_left.visible = false
				elseif self._level == 2 then
					self._level_2_sprite_right.visible = true
					self._level_2_sprite_left.visible = false
				elseif self._level == 3 then
					self._level_3_sprite_right.visible = true
					self._level_3_sprite_left.visible = false
				end
			end
			self._right = true
			self._state = SuperMarioBros.PlayerState.PS_WALK
			self._speed_rate = self._speed_rate + (0.001)
			if self._speed_rate > SuperMarioBros.PLAYER_MAX_SPEED_RATE then
				self._speed_rate = SuperMarioBros.PLAYER_MAX_SPEED_RATE
			end
			self._speed = self._speed + (self._speed_rate * frame_time)
			if self._speed >= SuperMarioBros.PLAYER_MAX_WALK_SPEED then
				self._speed = SuperMarioBros.PLAYER_MAX_WALK_SPEED
			end
			self.x = self.x + (self._speed)
			self._walk_frame_change = self._walk_frame_change + (0.006 * frame_time * (1 + self._speed))
			if self._walk_frame_change >= 1 then
				self._walk_frame_change = 0.0
				self._walk_frame = self._walk_frame + (1)
				if self._level == 1 then
					if self._walk_frame == 2 then
						self._level_1_sprite_right.col_index = 5
					elseif self._walk_frame == 3 then
						self._level_1_sprite_right.col_index = 6
					else
						self._walk_frame = 1
						self._level_1_sprite_right.col_index = 4
					end
				elseif self._level == 2 then
					if self._walk_frame == 2 then
						self._level_2_sprite_right.col_index = 5
					elseif self._walk_frame == 3 then
						self._level_2_sprite_right.col_index = 6
					else
						self._walk_frame = 1
						self._level_2_sprite_right.col_index = 4
					end
				elseif self._level == 3 then
					if self._walk_frame == 2 then
						self._level_3_sprite_right.col_index = 5
					elseif self._walk_frame == 3 then
						self._level_3_sprite_right.col_index = 6
					else
						self._walk_frame = 1
						self._level_3_sprite_right.col_index = 4
					end
				end
			end
		end
		return
	end
	if A_UISystem.sym_map[97] ~= nil then
		if self._state == SuperMarioBros.PlayerState.PS_JUMP then
		else
			if self._right then
				self._speed_rate = 0
				self._speed = 0
				if self._level == 1 then
					self._level_1_sprite_right.visible = false
					self._level_1_sprite_left.visible = true
				elseif self._level == 2 then
					self._level_2_sprite_right.visible = false
					self._level_2_sprite_left.visible = true
				elseif self._level == 3 then
					self._level_3_sprite_right.visible = false
					self._level_3_sprite_left.visible = true
				end
			end
			self._right = false
			self._state = SuperMarioBros.PlayerState.PS_WALK
			self._speed_rate = self._speed_rate + (0.001)
			if self._speed_rate > SuperMarioBros.PLAYER_MAX_SPEED_RATE then
				self._speed_rate = SuperMarioBros.PLAYER_MAX_SPEED_RATE
			end
			self._speed = self._speed + (self._speed_rate * frame_time)
			if self._speed >= SuperMarioBros.PLAYER_MAX_WALK_SPEED then
				self._speed = SuperMarioBros.PLAYER_MAX_WALK_SPEED
			end
			self.x = self.x - (self._speed)
			self._walk_frame_change = self._walk_frame_change + (0.006 * frame_time * (1 + self._speed))
			if self._walk_frame_change >= 1 then
				self._walk_frame_change = 0.0
				self._walk_frame = self._walk_frame + (1)
				if self._level == 1 then
					if self._walk_frame == 2 then
						self._level_1_sprite_left.col_index = 4
					elseif self._walk_frame == 3 then
						self._level_1_sprite_left.col_index = 3
					else
						self._walk_frame = 1
						self._level_1_sprite_left.col_index = 2
					end
				elseif self._level == 2 then
					if self._walk_frame == 2 then
						self._level_2_sprite_left.col_index = 4
					elseif self._walk_frame == 3 then
						self._level_2_sprite_left.col_index = 3
					else
						self._walk_frame = 1
						self._level_2_sprite_left.col_index = 2
					end
				elseif self._level == 3 then
					if self._walk_frame == 2 then
						self._level_3_sprite_left.col_index = 4
					elseif self._walk_frame == 3 then
						self._level_3_sprite_left.col_index = 3
					else
						self._walk_frame = 1
						self._level_3_sprite_left.col_index = 2
					end
				end
			end
		end
		return
	end
	if self._state == SuperMarioBros.PlayerState.PS_WALK then
		self._speed = 0
		self._state = SuperMarioBros.PlayerState.PS_IDLE
		if self._level == 1 then
			if self._right then
				self._level_1_sprite_right.col_index = 1
			else
				self._level_1_sprite_left.col_index = 7
			end
		elseif self._level == 2 then
			if self._right then
				self._level_2_sprite_right.col_index = 2
			else
				self._level_2_sprite_left.col_index = 6
			end
		elseif self._level == 3 then
			if self._right then
				self._level_3_sprite_right.col_index = 2
			else
				self._level_3_sprite_left.col_index = 6
			end
		end
	end
end

end