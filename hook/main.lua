local function GetCurrentModPath()
	if debug and REPENTANCE then
		return string.sub(debug.getinfo(GetCurrentModPath).source,2) .. "/../"
	end
end

local mod_path = GetCurrentModPath()

local addr_hooks = {}

local function post_init_hook_lib()
	local original_hook = IsaacHook.setHook
	IsaacHook.setHook = function(addr, callback)
		-- local addr, callback = args["address"], args["callback"]
		if addr == nil then return "invalid argument" end
		if callback == nil then return "invalid argument" end
		
		if addr_hooks[addr] == nil then
			local hook_result = original_hook({address=addr})
			if hook_result ~= nil then
				print("IsaacHook Error: " .. hook_result)
				return hook_result
			end
			addr_hooks[addr] = {callback}
		else
			-- add callback to addr_hooks
			table.insert(addr_hooks[addr], callback)
		end
	end

	IsaacHookCallback = function(addr, regs)
		for k,v in pairs(addr_hooks[addr] or {}) do
			v(regs)
		end
		return regs
	end
end

if mod_path then
	local hook_init = package.loadlib(GetCurrentModPath() .. "lib/isaac-hook.bin","open")
	if hook_init then
		hook_init()
		post_init_hook_lib()

		require("hook")
	end
else
	Isaac.ConsoleOutput("Error: IsaacHook need REPENTANCE and start with --luadebug.\n")
end