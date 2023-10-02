IsaacHook.setHook(
	IsaacHook.getFuncAddr(Game():GetItemPool().GetPoolForRoom), 
	function(r) 
		IsaacHook.writeInt32(r.esp+4, RoomType.ROOM_BOSS) 
	end
	)