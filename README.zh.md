# isaac-hook

此mod通过hook技术获取/修改游戏内部逻辑，可用于实现超出mod接口能力的逻辑，游戏需要以`--luadebug`方式启动。

# 如何构建

使用VS2022打开解决方案直接构建。游戏是32位程序，所以请构建x86版本。

如果构建Debug版本可能会有一个文件拷贝错误，可以忽略。或者在项目设置的生成后事件里面修复一下游戏安装路径。

把`isaac-hook.dll`重命名为`isaac-hook.bin`，然后拷贝到mod的lib目录下即可。

# 控制台说明

mod增加若干方便测试的控制台指令。这些指令在版本更新后可能会失效。

- `hook` 总开关
- `hook d8` d8的内部参数显示开关
- `hook redstew` 红豆汤的内部数值显示开关

更多指令参考[这里](hook/ConsoleCommands.md)

# 接口说明

该mod新增全局变量`IsaacHook`。

这些函数本身的功能（大概率）不依赖特定的游戏版本，但在不同的游戏版本上，它们的参数（hook地址）会有所不同。

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

- 使用Lua头文件对接游戏lua系统
- 使用[Patcher](https://github.com/BradDorney/Patcher)完成hook操作，附加依赖[capstone](https://github.com/capstone-engine/capstone/tree/v5)