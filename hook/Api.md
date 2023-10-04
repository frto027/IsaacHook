------

```
IsaacHook.setHook(uint32 address, callback)
```

The `address` is the game Instruction address offset, relative to isaac-ng.exe module base address.
the `callback` is function like `function(reg) xxxxxx end`
you can read and modify the following variables inside callback function:
- reg.eax
- reg.ebx
- reg.ecx
- reg.edx
- reg.esi
- reg.edi
- reg.esp
- reg.ebp
- reg.eflags

The function uses Patcher library to rewrite the instructions at `address`, and calls `callback` before the instruction. don't hook two instructions less than 5 bytes apart, I don't know what will happen.

------
```
uint32      = IsaacHook.toInt32(float)
float       = IsaacHook.toFloat32(uint32)
```
------
```
uint32, uint32 = IsaacHook.toInt64(double)
double         = IsaacHook.toFloat64(uint32,uint32)
```
------
```
uint32 = IsaacHook.getFuncAddr(api_function)
```
e.g.
local funcAddr = IsaacHook.getFuncAddr(EntityPlayer.__class.EvaluateItems)

The `api_function` MUST be the original game function object.  NOT compatable with mods that hijack them.

------
```
uint32 = IsaacHook.getObjectAddr(object)
```
e.g.
local player_addr = IsaacHook.getObjectAddr(Isaac.GetPlayer(0))

The `object` MUST be the original game object.  NOT compatable with mods that hijack them.

------
```
uint32 = IsaacHook.readInt32(uint32 addr)
IsaacHook.writeInt32(uint32 addr,uint32 value)
```

This is memory read/write function.