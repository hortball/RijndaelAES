# RijndaelAES

## 介绍 ##
自己实现的AES算法。算法已经过特别优化。

支持AES128、AES192和AES256。

支持填充方式：无填充、PKCS#5和ISO 10126

支持加解密模式：ECB、CBC、CFB和OFB

**用Visual Studio 2015创建的项目**
## English ##

AES algorithm implementation by myself.

Supports for AES128、AES192 and AES256.

Supported paddings: no padding、PKCS#5 and ISO 10126

The mode of encryption supported: ECB、CBC、CFB and OFB

**This project is created by Visual Studio 2015**
## Example code ##
    
	BYTE keyword[] = { 0x00,0x01,0x02,0x03,0x00,0x01,0x02,0x03,0x00,0x01,0x02,0x03,0x00,0x01,0x02,0x03 };

	BYTE data[] = { 0x00 };

	StdAES AES(STDAES_KEY_128BIT, STDAES_MODE_ECB, STDAES_PADDING_PKCS5);

	AES.setKey(keyword);

	int bufferLength = AES.getBufferLen(16);
	LPBYTE buffer = new BYTE[bufferLength];
	memcpy(buffer, data, 16);

	AES.encrypt(buffer, bufferLength, &bufferLength, nullptr);
