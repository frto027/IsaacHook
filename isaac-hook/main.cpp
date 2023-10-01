#include "pch.h"
#include "ilua.h"
#include <stdio.h>
#include <signal.h>

#define VERSION 0.1

int c_module_reference_count = 0;

extern "C" {
    __declspec(dllexport) int open(lua_State* L);
}

struct PatchRecord {
    unsigned int return_addr;
    unsigned int patch_type;
};

void hook_init() {
}

int close_hook(lua_State* L) {
    return 0;
}



/* function set_hook(config: table) -> hook_id */
int set_hook(lua_State* L) {
    if (!lua_istable(L, 1)) {
        lua_pushinteger(L, -1);
        return 1;
    }

    const char* addr = nullptr;


    lua_getfield(L, 1, "address");
    if (lua_isstring(L, -1))
        addr = lua_tostring(L, -1);
    lua_pop(L, 1);

    return 0;

}

/* function remove_hook(hook_id) -> boolean */
int remove_hook(lua_State* L) {
    lua_pushboolean(L, false);
    return 1;
}

/*
int receive(lua_State* L) {
    LuaState luastate(L);
    danmu_get_buffered([](DanmuItem* item) {
        lua_State* L = LuaState::getL();
        //the called function
        lua_pushvalue(L, 1);
        //text arg
        lua_pushstring(L, item->text);

        lua_call(L, 1, 0);
        });
    return 0;
}

int set_room_id(lua_State* L) {
    if (lua_isinteger(L, 1)) {
        danmu_set_roomid(lua_tointeger(L, 1));
    }
    return 0;
}

int set_reading_enabled(lua_State* L) {
    if (lua_isboolean(L, 1)) {
        danmu_set_reading_enabled(lua_toboolean(L, 1));
    }
    return 0;
}

int get_reading_enabled(lua_State* L) {
    lua_pushboolean(L, danmu_get_reading_enabled());
    return 1;
}

int get_popularity(lua_State* L) {
    lua_pushinteger(L, danmu_get_popularity());
    return 1;
}
*/
struct {
    const char* name;
    lua_CFunction func;
} hook_lib[] = {
    {"setHook", set_hook},
    {"removeHook", remove_hook},
    {NULL, NULL}
};



int open(lua_State * L) {
    if (c_module_reference_count) {
        // we don't need to reinit the danmuB object...
        return 0;
    }
    LuaState luastate(L);

    lua_createtable(L, 0, 10); // the danmuB object
    lua_createtable(L, 0, 1);  // the metatable of danmuB object
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, close_hook);
    lua_settable(L, -3);
    lua_setmetatable(L, -2);

    //now, the stack top is danmuB object;
#pragma warning(push)
#pragma warning(disable:6385)
    for (int i = 0; hook_lib[i].name != NULL; i++) {
        lua_pushstring(L, hook_lib[i].name);
        lua_pushcfunction(L, hook_lib[i].func);
        lua_settable(L, -3);
    }
#pragma warning(pop)

    lua_pushstring(L, "version");
    lua_pushnumber(L, VERSION);
    lua_settable(L, -3);

    c_module_reference_count++;
    lua_setglobal(L, "IsaacHook");

    char buff[1024];
    sprintf_s(buff, sizeof(buff), "IsaacHook v%.1f service made by @frto027 started.\n", VERSION);

    Isaac::ConsoleOutput(buff);

    //do some init works
    hook_init();

    return 0;
}

int close_danmuB(lua_State* L) {
    LuaState luastate(L);

    c_module_reference_count--;
    if (c_module_reference_count == 0) {
        Isaac::ConsoleOutput("danmuB service stoped.\n");
        //hook_cleanup();
    }
    else if (c_module_reference_count < 0) {
        Isaac::ConsoleOutput("something bad happened(reference count error).\n");
    }

    return 0;
}
