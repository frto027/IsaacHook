danmu = "test"

local MyMod = RegisterMod("test",1)

local color = KColor(1,0,0,0.5)

local json = require("json")

if hook then
	hook.setReadingEnabled(true)
end

MyMod:AddCallback(ModCallbacks.MC_POST_RENDER, function()
	if hook then

		EID.font:DrawStringScaledUTF8("人气值："..tostring(hook.getPopularity()),80,200,1,1,color)

		hook.receive(function(text)
			Isaac.DebugString(text)
			local dm = json.decode(text)
			if dm.cmd == "DANMU_MSG" then
				local user = dm.info[3][2]
				local text = dm.info[2]

				danmu = user .. ":" .. text
			end
		end)
		EID.font:DrawStringScaledUTF8(danmu,80,180,1,1,color)
	end

end)


