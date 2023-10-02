# isaac-hook

此mod通过hook技术获取/修改游戏内部逻辑，可用于实现超出mod接口能力的逻辑，游戏需要以`--luadebug`方式启动。

# 如何构建

使用VS2022打开解决方案直接构建。游戏是32位程序，所以请构建x86版本。


# 接口说明

该mod新增全局变量`IsaacHook`。

## 钩子函数

```
IsaacHook.setHook(address:uint32, callback:function(reg)->void)
```
address是相对于isaac-ng.exe偏移的内存地址。在游戏执行到此位置时，会调用callback函数。

以下属性均为32位无符号整数，可读取或修改。
```lua
reg.eax
reg.ebx
reg.ecx
reg.edx
reg.esi
reg.edi
reg.esp
reg.ebp
reg.eflags
```

## 类型转换函数

主要用于快速处理浮点数据。

```
int = IsaacHook.toInt32(float)
float = IsaacHook.toFloat32(int)
--[[
	e.g.
	Speed = IsaacHook.toInt32(reg.eax)
	Speed = Speed * 0.5
	reg.eax = IsaacHook.toFloag32(Speed)
]]

int,int = IsaacHook.toInt64(double)
double = IsaacHook.toFloat64(int,int)
--[[
	e.g.
	Speed = IsaacHook.toInt32(reg.eax, reg.ebx)
	Speed = Speed * 0.5
	reg.eax, reg.ebx = IsaacHook.toFloag32(Speed)
]]
```

## 内存辅助函数

```
IsaacHook.getFuncAddr(api_function)
e.g.
	local GetPoolForRoomFunctionAddr = IsaacHook.getFuncAddr(Game():GetItemPool().GetPoolForRoom), 
```

`getFuncAddr` 用于获取游戏库函数的地址（相对于isaac-ng.exe偏移的内存地址）。返回值为32位无符号整数

```
uint32 IsaacHook.readInt32(addr)
IsaacHook.writeInt32(addr,value)
```

用于内存读写，`addr`和`value`均为32位无符号整数。

# c库引用说明

- 使用lua对接游戏lua系统
- 使用Patcher完成hook操作