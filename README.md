![logo](./doc-readme/logo.png)
# 啥啥都队

## 队伍介绍

我们队伍名称为啥啥都队，来自中国科学技术大学，基本情况如下：

| 赛题     | proj333——基于Qemu的龙架构平台高性能插桩工具 |
| -------- | ------------------------------------------- |
| 小组成员 | 高林萍、穆奕博、陈曦                        |
| 指导老师 | 徐伟、卢建良                                |

## 文档

* **[初赛项目文档](target/loongarch/instrument/doc/code_structure.md)**
* **[代码结构](target/loongarch/instrument/doc/code_structure.md)**
* **[插桩工具教程](target/loongarch/instrument/doc/eazy_pintool.mddoc/run_test.md)**
* **[框架编译和使用](target/loongarch/instrument/doc/how_to_build.md)**


## 项目简介

### 背景介绍

随着软件行业的飞速发展，软件的规模和复杂性持续增长，这使得监控程序在运行过程中的行为变的越来越有挑战性。动态二进制插桩技术可以在不影响程序运行结果的前提下，在程序运行时插入分析代码，从而实现对程序的监控和分析。其在代码调试，安全性能检测，性能分析和代码分析等诸多场景有广泛应用。

二进制插桩技术发展至今，已经拥有了诸多各具特色的插桩框架，然而这些框架只支持主流的指令集架构，对于国产自主研发的指令集架构缺少高效的插桩框架。

为了解决这一问题，我们团队基于Qemu模拟器，设计并实现了一个支持龙芯指令集的高效二进制插桩框架原型。该框架原型能够在指令、基本块、函数和镜像等多个粒度上进行插桩，实现了对程序执行过程的全面监控。团队设计实现了与Pin高度兼容的大部分通用API。

### 整体框架

![框架图](./doc-readme/frame.png)

本系统的整体架构如上图所示，其主要由虚拟机、代码缓存与插桩接口构成，虚拟机中包含调度器和即时编译器。用户编写的插桩工具通过插桩接口，向插桩框架注册插桩函数。插桩框架先将应用程序加载到内存中完成初始化，随后虚拟机中的调度器负责协调各个部件来运行应用程序。 虚拟机采用即时编译（JIT）的方式运行来应用程序，在截获应用程序的执行流后，对应用程序的将要执行的二进制代码进行解析，得到指令序列并进行插桩，最后生成插桩后的二进制代码，并交给调度器进行执行。插桩后的二进制代码会被保存到代码缓存中，以避免重复的插桩工作。插桩框架会拦截应用程序的系统调用，并在插桩框架的控制下模拟执行。

![框架运行时序图](./doc-readme/sequence.png)

框架运行时序图如上图所示。

### 成果概述

**为框架设计并实现了指令级（INS）、基本块级（BBL）、轨迹级（TRACE）、函数级（RTN）以及镜像级（IMG）五个基本插桩粒度，同时添加了各粒度通用API**

#### INS级

INS级API的是以一条汇编指令为插桩对象设计实现的API。在框架中，一条原始的指令在经过代码解析模块，翻译模块和插桩模块的处理后，会生成与原始指令功能相同的一条或多条指令，每条指令由数据结构Ins描述，这些Ins共同组成了一个INS块。INS块是整个框架插桩操作的最小单位，也是可插桩的最细粒度。

#### BBL级

BBL级API的是以一个基本块为插桩对象设计实现的API。BBL是一个单入口单出口的指令序列，如果一个基本块的出口指向另一个基本块的中间，那么将会产生一个新的基本块。以下三种情况可以作为基本块的入口：

1. 进入程序的第一条指令。

2. 一个基本块结束后，紧跟着基本块的下一条指令。

3. 跳转指令的目标地址处的指令。

条件跳转指令，无条件跳转指令，系统调用等控制执行流的指令可作为基本块的出口。基本块是程序能够连续执行的最小指令序列。

#### TRACE级

TRACE通常是一个单入口多出口的指令序列，入口是上一个TRACE解析完此时PC中的地址，出口有两种情况：1.无条件跳转指令。2.TRACE中第三个基本块最后的条件跳转指令。一个TRACE中的基本块数不大于3。如果有一条跳转指令的目标地址在TRACE中间，那么将会在原有的BBL基础上产生新的BBL，同时也产生新的TRACE。

#### RTN级

RTN级API是以函数为插桩对象设计实现的API，应用程序运行时需要用到大量的符号，在动态链接时产生的相关符号信息即是所需收集的函数，可以通过符号解析进行RTN的获取。函数级API可以获取RTN的各种信息，允许在特定RTN的入口和出口处进行插桩分析，每个RTN有一个入口和多个出口。

#### IMG级

IMG级API是以二进制可执行文件为插桩对象设计实现的API，每加载一个可执行文件就创建一个IMG，每个IMG中包含多个RTN，镜像级API可以获取IMG的各种信息，允许在程序运行过程中对程序进行插桩和分析。

## API实现情况

#### INS级

✅ VOID INS_InsertCall(INS INS, IPOINT action, AFUNPTR funptr, ...)
✅ VOID INS_InsertPredicatedCall (INS ins, IPOINT ipoint, AFUNPTR funptr,...)
✅ VOID INS_InsertIfCall (INS ins, IPOINT action, AFUNPTR funptr,...)
✅ VOID INS_InsertThenCall (INS ins, IPOINT action, AFUNPTR funptr,...)
✅ VOID INS_InsertIfPredicatedCall (INS ins, IPOINT action, AFUNPTR funptr,...)
✅ VOID INS_InsertThenPredicatedCall (INS ins, IPOINT action, AFUNPTR funptr,...)

✅ INS INS_Next(INS x)
✅ INS INS_Prev(INS x)
✅ BOOL INS_Valid(INS x)
✅ ADDRINT INS_Address (INS ins)
✅ USIZE INS_Size (INS ins)
✅ enum PREDICATE INS_GetPredicate (INS ins)
✅ BOOL INS_IsMemoryRead (INS ins)
✅ BOOL INS_IsMemoryWrite (INS ins)
✅ BOOL INS_HasFallThrough (INS ins)
✅ const char *INS_Mnemonic (INS ins)
✅ BOOL INS_IsBranch (INS ins)
- [x] BOOL INS_IsDirectBranch (INS ins)
- [x] BOOL INS_IsDirectCall (INS ins)
- [x] BOOL INS_IsDirectControlFlow (INS ins)
- [x] BOOL INS_IsCall (INS ins)
- [x] BOOL INS_IsControlFlow (INS ins)
- [x] BOOL INS_IsInterrupt (INS ins)
- [x] BOOL INS_IsRet (INS ins)
- [x] BOOL INS_IsPrefetch (INS ins)
- [x] BOOL INS_IsMov (const INS ins)
- [x] BOOL INS_IsAtomicUpdate (const INS ins)
- [x] BOOL INS_IsIndirectControlFlow (INS ins)
- [x] OPCODE INS_Opcode (INS ins)
- [x] const char *INS_Disassemble (INS ins)
- [x] UINT32 INS_OperandCount (INS ins)
- [x] UINT32 INS_MemoryOperandCount (INS ins)
- [x] USIZE INS_MemoryOperandSize (INS ins, UINT32 memoryOp)
- [x] BOOL INS_MemoryOperandIsRead (INS ins, UINT32 memopIdx)
- [x] BOOL INS_MemoryOperandIsWritten (INS ins, UINT32 memopIdx)
- [x] BOOL INS_OperandIsReg (INS ins, UINT32 n)
- [x] REG INS_OperandReg (INS ins, UINT32 n)
- [x] BOOL INS_OperandIsImmediate (INS ins, UINT32 n)
- [x] UINT64 INS_OperandImmediate (INS ins, UINT32 n)
- [x] BOOL INS_OperandRead (INS ins, UINT32 n)
- [x] BOOL INS_OperandWritten (INS ins, UINT32 n)
- [x] BOOL INS_OperandReadOnly (INS ins, UINT32 n)
- [x] BOOL INS_OperandWrittenOnly (INS ins, UINT32 n)
- [x] BOOL INS_OperandReadAndWritten (INS ins, UINT32 n)
- [x] BOOL INS_IsSyscall (INS ins)
- [x] INS INS_Invalid (void)
- [x] ADDRINT INS_DirectControlFlowTargetAddress (INS ins)
- [x] ADDRINT INS_NextAddress (INS ins)
- [x] UINT32 INS_EffectiveAddressWidth(INS ins)
- [x] BOOL INS_IsValidForIpointAfter(INS ins)
- [x] BOOL INS_IsValidForIpointTakenBranch (INS ins)
- [x] BOOL INS_IsProcedureCall (INS ins)
- [x] BOOL INS_IsSub(const INS ins)
- [x] BOOL INS_IsOriginal(INS ins)
- [x] BOOL INS_OperandIsMemory (INS ins, UINT32 n)
- [x] UINT32 INS_MemoryOperandIndexToOperandIndex (INS ins, UINT32 memopIdx)
- [x] BOOL INS_OperandIsImplicit (INS ins, UINT32 n)
- [x] BOOL INS_RegIsImplicit (INS ins, REG reg)
- [x] BOOL INS_IsAddedForFunctionReplacement (INS ins)
- [x] BOOL INS_ChangeReg (const INS ins, const REG old_reg, const REG new_reg, const BOOL as_read)

#### BBL级

- [x] VOID BBL_InsertCall(BBL bbl, IPOINT action, AFUNPTR funptr, ...)
- [x] VOID BBL_InsertIfCall (BBL bbl, IPOINT action, AFUNPTR funptr,...)
- [x] VOID BBL_InsertThenCall (BBL bbl, IPOINT action, AFUNPTR funptr,...)
- [x] UINT32 BBL_NumIns(BBL bbl)
- [x] INS BBL_InsHead(BBL x)
- [x] INS BBL_InsTail(BBL x)
- [x] BBL BBL_Next(BBL x)
- [x] BBL BBL_Prev(BBL x)
- [x] BOOL BBL_Valid(BBL x)
- [x] BOOL BBL_Original(BBL bbl)
- [x] ADDRINT BBL_Address(BBL x)
- [x] USIZE BBL_Size(BBL x)
- [x] BOOL BBL_HasFallThrough(BBL x)

#### TRACE级

- [x] VOID TRACE_AddInstrumentFunction(TRACE_INSTRUMENT_CALLBACK fun, VOID *val)
- [x] VOID TRACE_InsertCall(TRACE trace, IPOINT action, AFUNPTR funptr, ...)
- [x] VOID TRACE_InsertIfCall (TRACE trace, IPOINT action, AFUNPTR funptr,...)
- [x] VOID TRACE_InsertThenCall (TRACE trace, IPOINT action, AFUNPTR funptr,...)
- [x] BBL TRACE_BblHead(TRACE trace)
- [x] BBL TRACE_BblTail(TRACE trace)
- [x] UINT32 TRACE_NumBbl(TRACE trace)
- [x] UINT32 TRACE_NumIns(TRACE trace)
- [x] ADDRINT TRACE_Address(TRACE trace)
- [x] USIZE TRACE_Size(TRACE trace)
- [x] RTN TRACE_Rtn(TRACE trace)
- [x] BOOL TRACE_HasFallThrough (TRACE trace)

#### RTN级
- [x] VOID RTN_InsertCall(RTN rtn, IPOINT action, AFUNPTR funptr, ...)
- [x] IMG RTN_Img(RTN rtn)
- [x] RTN RTN_Next(RTN rtn)
- [x] RTN RTN_Prev(RTN rtn)
- [x] RTN RTN_Invaild(void
- [x] BOOL RTN_Valid(RTN rtn)
- [x] const char * RTN_Name(RTN rtn)
- [x] UINT32 RTN_Id(RTN rtn)
- [x] USIZE RTN_Size(RTN rtn)
- [x] const CHAR *RTN_FindNameByAddress(ADDRINT address)
- [x] RTN RTN_FindByAddress(ADDRINT address)
- [x] RTN RTN_FindByName(IMG img, const CHAR *name)
- [x] VOID RTN_Open(RTN rtn)
- [x] VOID RTN_Close(RTN rtn)
- [x] UINT32 RTN_NumIns(RTN rtn)
- [x] ADDRINT RTN_Address(RTN rtn)

#### IMG级

- [x] VOID IMG_AddInstrumentFunction(IMAGECALLBACK fun, VOID *val)
- [x] IMG IMG_Next(IMG img)
- [x] IMG IMG_Prev(IMG img)
- [x] IMG IMG_Invalid(void)
- [x] BOOL IMG_Valid(IMG img)
- [x] RTN IMG_RtnHead(IMG img)
- [x] RTN IMG_RtnTail(IMG img)
- [x] const char * IMG_Name(IMG img)
- [x] UINT32 IMG_Id(IMG img)
- [x] IMG IMG_FindImgById(UINT32)


## 代码仓库

本项目是基于QEMU设计实现的插桩框架，项目主仓库以及项目主要插桩文件的地址如下，供参考：

| 仓库/主文件         | 仓库/主文件地址                                                     |
| ------------- | ------------------------------------------------------------ |
| 本项目主仓库  | https://gitlab.eduxiji.net/T202410358992583/project2210132-232208 |
| 本项目主文件  | https://gitlab.eduxiji.net/T202410358992583/project2210132-232208/-/tree/main/target/loongarch |


## 参考资料

* [qemu-instrument](https://github.com/foxsen/qemu-instrument/tree/instru)
* [Loongarch Docs](https://github.com/loongson/LoongArch-Documentation)龙芯相关手册



