# [EAI80](https://github.com/SoCXin/EAI80)

[![sites](http://182.61.61.133/link/resources/SoC.png)](http://www.SoC.Xin)

#### [Vendor](https://github.com/SoCXin/Vendor)：[Gree Edgeless](https://github.com/SoCXin/Gree)
#### [Core](https://github.com/SoCXin/Cortex)：[Cortex M4](https://github.com/SoCXin/CM4)
#### [Level](https://github.com/SoCXin/Level)：2 x 200MHz x 1.25DMIPS/MHz (500DMIPS) , CNN-NPU @ 300MHz 300GOPS

## [简介](https://github.com/SoCXin/EAI80/wiki)

[EAI80](https://github.com/SoCXin/EAI80)是一个多核微控制器，实现了双核Cortex-M4核。所有核心都可以访问完整的内存映射。ARM Cortex-M4用作主处理器。另一个ARM Cortex-M4内核可以作为协处理器来协助ARM Cortex-M4并执行复杂的数学计算。

支持主流CNN模式，如Resnet-18, Resnet-34, Vgg16, GoogleNet, Lenet等，卷积与内核大小从1到7，频道/特征数高达512，最大/平均池功能与内核。

* CPU Dual-Cortex M4F@200MHz 500DMIPS
* AI-NPU：CNN-NPU @300 MHz 300GOPS,144MAC/cycle, EER up to 1TOPS/W, for image recognition scenario.
* 2D Graph :Dual-Camera Max
* Up to 384KB of SRAM, 256KB for CNN-NPU (Share with CPU) ,Up to 8MB of SIP-SDRAM

* 3x I2C interfaces
* 8x UARTs with full-duplex data exchange,
* 2x SPIs, full-duplex synchronous and single-wire bidirectional mode and 4- to 16-bit word frames
* 1x CAN 2.0B interfaces
* 1x USB 2.0 full-speed device/host/OTG controller with off-chip PHY

#### 关键特性

* CNN-NPU @300 MHz 300GOPS
* Dual Cortex M4F@200MHz 500DMIPS


### [收录资源](https://github.com/SoCXin/EAI80)

* [文档](docs/)
* [资源](src/)

### [选型建议](https://github.com/SoCXin)

[EAI80](https://github.com/SoCXin/EAI80)是一款新的边缘计算体系MCU，集成NPU，可应用于语音控制，计算机视觉-物体和生物(脸，身体，姿势)的检测和识别，vSLAM，边缘计算等。

后续缺乏相应支持，生态不够完善，个人开发较难。
###  [www.SoC.xin(芯)](http://www.SoC.Xin)

