# RijndaelAES
## 最近更新 2016年10月14日 ##
1. 现在已支持Intel AES-NI技术。AES-NI是Intel CPU的一个AES硬件加速技术
2. 如果你的CPU支持AES-NI技术，将应用该技术
3. 将填充和加/解密分离了出来，这是为了让加/解密的过程理解起来更易懂。请见示例代码
4. 重载了一个encrypt函数，ECB加密时可忽略IV参数。
5. 优化了加密过程，删除了一处多余的操作。现在IV参数将在encrypt函数中拷贝一份，而不是直接对IV参数操作（这样做会破坏外部的IV数据）

## 性能 ##
测试机器： Intel Core I7 5700HQ 、16G 1600MHZ内存

操作系统： Windows 10-64位

测试数据：500MB

# 32位程序 （未开启AES-NI技术） #


> 128位密钥 ECB模式 无填充 加密速率：513Mbps（64MB/s）



> 192位密钥 ECB模式 无填充 加密速率：425Mbps（53MB/s）



> 256位密钥 ECB模式 无填充 加密速率：363Mbps（45MB/s）
# 64位程序 （未开启AES-NI技术） #


> 128位密钥 ECB模式 无填充 加密速率：657Mbps（82MB/s）



> 192位密钥 ECB模式 无填充 加密速率：535Mbps（66MB/s）



> 256位密钥 ECB模式 无填充 加密速率：464Mbps（58MB/s）
# 32位程序 （开启AES-NI技术） #


> 128位密钥 ECB模式 无填充 加密速率：11594Mbps（1449MB/s）



> 192位密钥 ECB模式 无填充 加密速率：9708Mbps（1213MB/s）



> 256位密钥 ECB模式 无填充 加密速率：8492Mbps（1061MB/s）
# 64位程序 （开启AES-NI技术） #


> 128位密钥 ECB模式 无填充 加密速率：16393Mbps（2049MB/s）



> 192位密钥 ECB模式 无填充 加密速率：13513Mbps（1689MB/s）



> 256位密钥 ECB模式 无填充 加密速率：10526Mbps（1315MB/s）

## 介绍 ##
自己实现的AES算法。算法已经过特别优化。

支持AES128、AES192和AES256。

支持填充方式：无填充、PKCS#5和ISO 10126

支持加解密模式：ECB、CBC、CFB和OFB

**用Visual Studio 2015创建的项目**
## English ##

AES algorithm implementation by myself.

Supports 128bits、192bits and 256bits key length.

Supported paddings: no padding、PKCS#5 and ISO 10126

The mode of encryption supported: ECB、CBC、CFB and OFB

**This project is created by Visual Studio 2015**
## Example code ##
	BYTE keyword[] = { 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37 };
	BYTE iv[] = { 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36 };

	const int datalength = 1024;

	// 要加密的明文
	LPBYTE data = new BYTE[datalength];
	for(int i = 0; i < datalength; i++) {
		data[i] = i;
	}

	// 构造方法
	StdAES AES(STDAES_KEY_256BIT, STDAES_MODE_OFB, STDAES_PADDING_PKCS5);

	// 设置密钥
	AES.setKey(keyword);

	// 取明文填充后的长度
	int bufferLength = AES.getBufferLen(datalength);

	// 分配缓冲区
	LPBYTE buffer = new BYTE[bufferLength];

	// 填充明文
	AES.padding(buffer, bufferLength, data, datalength);

	// 加密数据
	AES.encrypt(buffer, bufferLength, iv);

	// 解密数据
	AES.decrypt(buffer, bufferLength, iv);

	// 去掉填充
	AES.deletePadding(buffer, &bufferLength);
