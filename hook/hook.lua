local IsaacHookProxy = RegisterMod("IsaacHook", 1)

local CMDS = {}
local hook_enabled = false

IsaacHookProxy:AddCallback(ModCallbacks.MC_EXECUTE_CMD, function(_, cmd, args)
    if cmd == 'hook' and CMDS[args] then
        CMDS[args]()
    end
end)

local Toggles = {}
local function RegisterToggle(name, init, callback)
    Toggles[name] = {
        state = false,
        finit = init,
        fcallback = callback,
    }
    CMDS[name] = function() 
        local Toggle = Toggles[name]
        Toggle.state = not Toggle.state
        if Toggle.state and not hook_enabled then
            hook_enabled = true
            print("hook has been enabled")
        end
        print("hook " .. name .. ": " .. (Toggle.state and "on" or "off"))
        if Toggle.finit then
            Toggle.finit()
            Toggle.finit = nil
        end
    end
end

CMDS[''] = function()
    hook_enabled = not hook_enabled
    if hook_enabled then
        print("hook has been enabled")
    else
        print("hook has been disabled")
    end
end

local f = Font()
f:Load("font/terminus.fnt")
local scale = 0.5
local x, y = 60, 50
local function D(str)
    f:DrawStringScaled(str,x,y, scale, scale,KColor(1,1,1,1),0,true)
    y = y + 14 * scale
end

IsaacHookProxy:AddCallback(ModCallbacks.MC_POST_RENDER, function()
    if not hook_enabled then return end
    x,y = 70, 70
    D("IsaacHook mod. For the BOI v1.7.9b")
    for _,v in pairs(Toggles) do
        if v.state then
            v.fcallback()
        end
    end
end)

IsaacHook.RegisterToggle = RegisterToggle
IsaacHook.DisplayLine = D
-------------------------------------------------------------------------

RegisterToggle('damo', nil, function()
    local offset = 0x30
    local roomEntities = Isaac.GetRoomEntities() -- table
    local has_content = false
    for i = 1, #roomEntities do
        local entity = roomEntities[i]
        if entity.Type == 3 and entity.Variant == 202 then
            local addr = IsaacHook.getObjectAddr(entity)
            local is_active = IsaacHook.readInt32(addr + offset) == 1
            D("Damo is " .. (is_active and "DANGER" or "SAFE"))
            has_content = true
        end
    end
    if not has_content then
        D("Damo not found.")
    end
end)

RegisterToggle('d8', nil, function()
    local offset = 0x185c
    for i = 0, Game():GetNumPlayers() - 1 do
        local player = Isaac.GetPlayer(i)
        D("D8 arg for player " .. tostring(i) .. " :")
        local output = ""

        local arg1 = IsaacHook.readInt32(IsaacHook.getObjectAddr(player) + offset)
        local arg2 = IsaacHook.readInt32(IsaacHook.getObjectAddr(player) + offset + 4)
        local arg3 = IsaacHook.readInt32(IsaacHook.getObjectAddr(player) + offset + 8)
        local arg4 = IsaacHook.readInt32(IsaacHook.getObjectAddr(player) + offset + 0xc)

        arg1, arg2, arg3, arg4 = IsaacHook.toFloat32(arg1), IsaacHook.toFloat32(arg2), IsaacHook.toFloat32(arg3), IsaacHook.toFloat32(arg4)
        D(tostring(arg1) .. " " ..tostring(arg2) .. " " ..tostring(arg3) .. " " ..tostring(arg4))
    end
end)

RegisterToggle('redstew', nil, function()
    local offset = 0x1D8C
    for i = 0, Game():GetNumPlayers() - 1 do
      local player = Isaac.GetPlayer(i)
      local RedStewCount = IsaacHook.readInt32(IsaacHook.getObjectAddr(player) + offset)
      D("RedStew for player " .. tostring(i) .. " = " .. tostring(RedStewCount))
    end  
end)

