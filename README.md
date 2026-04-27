# STM32-PLC-Emulator ( FX Series Compatible)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: STM32F10X](https://img.shields.io/badge/Platform-STM32F10X-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f1-series.html)

[中文版](#chinese-version) | [English](#english-version)

---

<a name="chinese-version"></a>
## 中文介绍

### 🚀 项目简介
本项目是一个基于 **STM32F10X** 系列单片机开发的 PLC 模拟系统。它深度兼容 ** FX 系列协议**，允许用户直接使用梯形图编程软件进行梯形图编写、下载及在线调试。

### 🛠️ 硬件及资源要求
为保证系统稳定运行及梯形图逻辑的流畅执行，硬件需满足以下最低要求：
* **RAM**: > **20 KB** (用于存放运行时的软元件状态、堆栈及动态内存)
* **Flash**: > **30 KB** (用于存放核心逻辑引擎及梯形图程序代码)
* **MCU 推荐**: STM32F103C8T6 或更高规格。

### 📂 文件夹结构说明
* **`/fx1n`**: 存放 PLC 核心功能实现文件（包括协议解析、逻辑执行引擎等）。
* **`/src`**: 存放单片机初始化程序，包含 `main.c`、`stm32f10x` 标准库链接等基础代码。
* **`/objects`**: 编译产物目录。内含生成的 `.hex` 文件，可直接用于烧录测试。

### ⚠️ 注意事项 
* **代码注释**: 当前源码中的注释主要以**中文**编写。
* **乱码问题**: 由于编码格式问题，英文环境下可能存在乱码或显示不全。英文版本及编码兼容性正在进一步完善中，敬请期待后续更新。
* **移植建議**：如果換成大容量晶片（如 F103VET6，一頁是 2KB），擦除地址偏移量必須重寫，否則會擦除掉不該擦除的區域。
* **首次使用**：烧录程序后，请先执行一次plc擦除.

 ### 可用原件
 X:48 Y:48 M:256  S:256 T:32  C:128  M8xxx:24  D:128






---

<a name="english-version"></a>
## English Version

### 🚀 Overview
This project is an  PLC emulation system developed on the **STM32F10X** platform, fully compatible with the ** FX Series protocol**.

### 🛠️ Hardware & Resource Requirements
To ensure stable execution of ladder logic, the following minimum specifications are required:
* **RAM**: > **20 KB**
* **Flash**: > **30 KB**
* **Recommended MCU**: STM32F103C8T6 or higher.

### 📂 Folder Structure
* **`/fx1n`**: Core PLC functionality files (Protocol parsing, logic engine, etc.).
* **`/src`**: MCU initialization source code, including `main.c` and `stm32f10x` firmware library files.
* **`/objects`**: Compilation output. Includes the `.hex` file for direct firmware flashing.

### ⚠️ Important Notes 
* **Comments**: The source code currently contains **Chinese comments**.
* **Known Issues**: The English localized version is still under development. You may encounter encoding issues (mojibake) in certain environments. Full internationalization (i18n) and encoding fixes are planned for future updates.
* **Porting Recommendation:** If switching to a high-capacity chip (such as F103VET6, where one page is 2KB), the erase address offset must be rewritten; otherwise, areas that should not be erased will be erased.
* **First-time to use:** After writing in  the hex program, please perform a PLC erase  first.

 ### Components available
 X:48 Y:48 M:256  S:256 T:32  C:128  M8xxx:24  D:128