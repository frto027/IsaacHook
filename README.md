# isaac-hook

This mod use hook tech to obtain/modify game internal logic, can do things beyond the lua api. The game should boot with `--luadebug` parameter.

[中文README](README.zh.md)

# How to build

In most cases, lua can do everything, so there's no need to build. If you want to build the hook library, follow the steps below.

Use VS2022 open `isaac-hook.sln`, build with `x86/Release`.

If you build with `X86/Debug`, it is okay. A copy file may failed, you can fix the path for your game in ProjectSetthings or just ignore it.

Rename the `isaac-hook.dll` to `isaac-hook.bin`, and copy to the `lib` folder inside the mod.

# API

Global variable `IsaacHook` is added.

Refer to: [API](hook/Api.md), [ConsoleCommands](hook/ConsoleCommands.md)

# HookExample

The following lua program hijacks the internal function, and changes every item pool to Boss pool. Try `spawn 5.100` and you will always get items from ROOM_BOSS.

```lua
IsaacHook.setHook(
	IsaacHook.getFuncAddr(Game():GetItemPool().GetPoolForRoom), 
	function(r) 
		IsaacHook.writeInt32(r.esp+4, RoomType.ROOM_BOSS) 
	end
	)
```

# C Library

- Lua(header file only)
- [Patcher](https://github.com/BradDorney/Patcher) The library has been modified
- [capstone](https://github.com/capstone-engine/capstone/tree/v5)