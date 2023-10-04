#include "pch.h"
#include "ilua.h"
#include <stdio.h>
#include <signal.h>
#include "../Patcher/Patcher.h"
#include <set>


using namespace Patcher;

#define VERSION 0.1

int c_module_reference_count = 0;

extern "C" {
    __declspec(dllexport) int open(lua_State* L);
}

Patcher::PatchContext* patchCtx = nullptr;
std::set<uint32_t> hooked_addrs;
uint32_t pageSizeMask = 0;
uint32_t IsaacNgBaseAddress;

void set_hook_error(lua_State* L, const char* error) {

}

void hook_init() {
    patchCtx = new Patcher::PatchContext();
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    pageSizeMask = ~(sysInfo.dwPageSize - 1);
    IsaacNgBaseAddress = (uint32_t)GetModuleHandleA(NULL);
}

int close_hook(lua_State* L) {
    delete patchCtx;
    patchCtx = nullptr;
    hooked_addrs.clear();

    return 0;
}


/* function set_hook(config: table) -> string */
int set_hook(lua_State* L) {
    static lua_State* LastLuaState = nullptr;
    LastLuaState = L;

    if (!lua_istable(L, 1)) {
        lua_pushstring(L, "invalid argument");
        return 1;
    }
    uint32_t addr = 0;
    lua_getfield(L, 1, "address");
    if (lua_isinteger(L, -1))
        addr = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if (addr == 0) {
        lua_pushstring(L, "invalid argument(address)");
        return 1;
    }

    if (hooked_addrs.count(addr)) {
        // don't hook the same address twice.
        lua_pushnil(L);
        return 1;
    }
    hooked_addrs.insert(addr);

    patchCtx->LowLevelHook((void *)(addr + IsaacNgBaseAddress),
        [](
            AddrArg<unsigned int> addr,
            Eax<unsigned int> eax,
            Ecx<unsigned int> ecx,
            Edx<unsigned int> edx,
            Ebx<unsigned int> ebx,
            Esi<unsigned int> esi,
            Edi<unsigned int> edi,
            Ebp<unsigned int> ebp,
            Esp<unsigned int> esp,
            Eflags<unsigned int> eflags) {
                lua_State* L = LastLuaState;
                lua_getglobal(L, "IsaacHookCallback");
                if (!lua_isfunction(L, -1)) {
                    lua_pop(L, 1);
                    return;
                }

                lua_pushinteger(L, (unsigned int)addr - IsaacNgBaseAddress);

                lua_newtable(L);
                lua_pushinteger(L, eax);
                lua_setfield(L, -2, "eax");
                lua_pushinteger(L, ecx);
                lua_setfield(L, -2, "ecx");
                lua_pushinteger(L, edx);
                lua_setfield(L, -2, "edx");
                lua_pushinteger(L, ebx);
                lua_setfield(L, -2, "ebx");
                lua_pushinteger(L, esi);
                lua_setfield(L, -2, "esi");
                lua_pushinteger(L, edi);
                lua_setfield(L, -2, "edi");
                lua_pushinteger(L, ebp);
                lua_setfield(L, -2, "ebp");
                lua_pushinteger(L, esp);
                lua_setfield(L, -2, "esp");
                lua_pushinteger(L, eflags);
                lua_setfield(L, -2, "eflags");

                lua_pcall(L, 2, 1, 0);
                if (!lua_isnil(L, -1)) {
                    lua_getfield(L, -1, "eax");
                    if (lua_isinteger(L, -1)) eax = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "ecx");
                    if (lua_isinteger(L, -1)) ecx = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "edx");
                    if (lua_isinteger(L, -1)) edx = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "ebx");
                    if (lua_isinteger(L, -1)) ebx = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "esi");
                    if (lua_isinteger(L, -1)) esi = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "edi");
                    if (lua_isinteger(L, -1)) edi = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "ebp");
                    if (lua_isinteger(L, -1)) ebp = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "esp");
                    if (lua_isinteger(L, -1)) esp = lua_tointeger(L, -1);
                    lua_pop(L, 1);

                    lua_getfield(L, -1, "eflags");
                    if (lua_isinteger(L, -1)) eflags = lua_tointeger(L, -1);
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
                return;
        }, LowLevelHookOpt::NoCustomReturnAddr);
    lua_pushnil(L);
    return 1;
}

int to_float32(lua_State* L) {
    if (lua_isinteger(L, 1)) {
        union {
            uint32_t i;
            float   f;
        }n;
        n.i = lua_tointeger(L, 1);
        lua_pushnumber(L, n.f);
    }
    else {
        lua_pushnumber(L, NAN);
    }
    return 1;
}
int to_int32(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        union {
            uint32_t i;
            float   f;
        }n;
        n.f = lua_tonumber(L, 1);
        lua_pushinteger(L, n.i);
    }
    else {
        lua_pushinteger(L, 0);
    }
    return 1;
}

int to_float64(lua_State* L) {
    if (lua_isinteger(L, 1) && lua_isinteger(L,2)) {
        union {
            struct { uint32_t iH, iL; }is;
            double   f;
        }n;
        n.is.iH = lua_tointeger(L, 1);
        n.is.iL = lua_tointeger(L, 2);
        lua_pushnumber(L, n.f);
    }
    else {
        lua_pushnumber(L, NAN);
    }
    return 1;
}

int to_int64(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        union {
            struct { uint32_t iH, iL; }is;
            double   f;
        }n;
        n.f = lua_tonumber(L, 1);
        lua_pushinteger(L, n.is.iH);
        lua_pushinteger(L, n.is.iL);
    }
    else {
        lua_pushinteger(L, 0);
        lua_pushinteger(L, 0);
    }
    return 2;
}

int read_int32(lua_State* L) {
    if (!lua_isinteger(L, 1)) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t* ptr = (uint32_t*)lua_tointeger(L, 1);

    MEMORY_BASIC_INFORMATION Buffer;
    SIZE_T sz = VirtualQuery((void *)(pageSizeMask & (uint32_t)ptr), &Buffer, sizeof(Buffer));
    if (sz == 0) {
        lua_pushnil(L);
        set_hook_error(L, "can't check VirtualQuery of memory address.");
        return 1;
    }

    if (Buffer.State != MEM_COMMIT) {
        lua_pushnil(L);
        set_hook_error(L, "memory state is not MEM_COMMIT.");
        return 1;
    }

    if (0 == (Buffer.AllocationProtect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE))) {
        lua_pushnil(L);
        set_hook_error(L, "the memory can't be read.");
        return 1;
    }

    lua_pushinteger(L, *ptr);
    return 1;
}

int write_int32(lua_State* L) {
    if (!lua_isinteger(L, 1) || !lua_isinteger(L, 1)) {
        lua_pushboolean(L, false);
        return 1;
    }

    uint32_t* ptr = (uint32_t*)lua_tointeger(L, 1);
    uint32_t value = lua_tointeger(L, 2);

    MEMORY_BASIC_INFORMATION Buffer;
    SIZE_T sz = VirtualQuery((void*)(pageSizeMask & (uint32_t)ptr), &Buffer, sizeof(Buffer));
    if (sz == 0) {
        lua_pushboolean(L, false);
        set_hook_error(L, "can't check VirtualQuery of memory address.");
        return 1;
    }

    if (Buffer.State != MEM_COMMIT) {
        lua_pushboolean(L, false);
        set_hook_error(L, "memory state is not MEM_COMMIT.");
        return 1;
    }

    if (0 == (Buffer.AllocationProtect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_READWRITE | PAGE_WRITECOPY))) {
        lua_pushboolean(L, false);
        set_hook_error(L, "the memory can't be read.");
        return 1;
    }

    *ptr = value;
    lua_pushboolean(L, true);
    return 1;
}

int get_func_addr(lua_State* L) {
    if (lua_iscfunction(L, 1)) {
        const char * name = lua_getupvalue(L, 1, 1);
        if (name) {
            uint32_t* addr = (uint32_t *)lua_touserdata(L, -1);
            lua_pop(L, 1);
            lua_pushinteger(L, (*addr) - IsaacNgBaseAddress);
        }
        else {
            lua_pushnil(L);
        }
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

int get_object_addr(lua_State* L) {
    if (lua_isuserdata(L, 1)) {
        uint32_t * data = (uint32_t *)lua_touserdata(L, 1);
        lua_pushinteger(L, data[1]);
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

struct {
    const char* name;
    lua_CFunction func;
} hook_lib[] = {
    {"setHook", set_hook},
    {"toInt32", to_int32},
    {"toInt64", to_int64},
    {"toFloat32", to_float32},
    {"toFloat64", to_float64},
    {"readInt32", read_int32},
    {"writeInt32", write_int32},
    {"getFuncAddr", get_func_addr},
    {"getObjectAddr", get_object_addr},
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

    //do some init works
    hook_init();

    char buff[1024];
    sprintf_s(buff, sizeof(buff), "IsaacHook v%.1f made by @frto027 started.(isaac-ng module at: %p)\n", VERSION, (void *)IsaacNgBaseAddress);

    Isaac::ConsoleOutput(buff);

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
