#pragma once

#include <Windows.h>
#include "IntelAES.h"

//#include "emmintrin.h"
//////////////////////////////////////
// 电子密码本模式
// 用相同的密钥分别对明文分组加密
//////////////////////////////////////
#define STDAES_MODE_ECB    0

//////////////////////////////////////
// 密码分组链接模式
// 加密算法的输入是上一个密文分组和下一个明文分组的异或
//////////////////////////////////////
#define STDAES_MODE_CBC    1

//////////////////////////////////////
// 密码反馈模式
// 一次处理J位，上一个分组密文作为产生一个伪随机数
// 输出的加密算法的输入，该输出与明文异或，作为下一个分组的输入
//////////////////////////////////////
#define STDAES_MODE_CFB   2

//////////////////////////////////////
// 输出反馈模式
// 与CFB基本相同，只是加密算法的输入是上一次AES的输出
//////////////////////////////////////
#define STDAES_MODE_OFB   3

//////////////////////////////////////
// 记数模式
// 每个明文分组是与加密计数器的异或，对每个后续的组，计数器是累加的
//////////////////////////////////////
#define STDAES_MODE_CTR   4

#define STDAES_PADDING_PKCS5     0
#define STDAES_PADDING_ISO10126  1


////////////////////////////////////////////////////////////////////////////
//  模式/填充             16字节加密后数据长度          不满16字节加密后长度
// CBC / NoPadding               16                          <不支持>
// CBC / PKCS5Padding            32                          16
// CBC / ISO10126Padding         32                          16
// CFB / NoPadding               16                          原始数据长度
// CFB / PKCS5Padding            32                          16
// CFB / ISO10126Padding         32                          16
// ECB / NoPadding               16                          <不支持>
// ECB / PKCS5Padding            32                          16
// ECB / ISO10126Padding         32                          16
// OFB / NoPadding               16                          原始数据长度
// OFB / PKCS5Padding            32                          16
// OFB / ISO10126Padding         32                          16
// PCBC / NoPadding              16                          <不支持>
// PCBC / PKCS5Padding           32                          16
// PCBC / ISO10126Padding        32                          16
/////////////////////////////////////////////////////////////////////////////

#define STDAES_KEY_128BIT 16
#define STDAES_KEY_192BIT 24
#define STDAES_KEY_256BIT 32

class StdAES {
public:
	StdAES(INT keyMode, INT AESMode, INT padding);
	~StdAES();

private:
	INT mKeyLength;
	INT mKeyColumn;
	INT mRounds;
	INT mMode;
	INT mPadding;
	INT mExpansionTableLength;
	/*__declspec(align(16)) */PDWORD32 mExpansionTable;

public:
	void setKey(LPBYTE key);

	INT getBufferLen(INT dataLen);
	void padding(LPBYTE buffer, INT buffLen, LPBYTE data, INT dataLen);
	void deletePadding(LPBYTE buffer, LPINT buffLen);

	////////////////////////////////////////////////
	// 缓冲区大小必须使用getBufferLen函数获得
	// 不合法的缓冲区大小将造成缓冲区溢出！
	////////////////////////////////////////////////
	void encrypt(LPBYTE buffer, INT buffLen);
	void encrypt(LPBYTE buffer, INT buffLen, LPBYTE iv);

	////////////////////////////////////////////////
	// 该函数不能确保解密后的数据是正确的，
	// 比如输入的数据是非AES算法加密的，或其他不合法数据
	// 请确保传入数据的合法性
	////////////////////////////////////////////////
	void decrypt(LPBYTE data, INT dataLen, LPBYTE iv);


private:
	//StdAES();
	//void encrypt(LPBYTE buffer, INT buffLen, LPBYTE iv);
	void keyExpansion(PDWORD32 key);

	void subWord(LPBYTE c);

	void addRoundKey(PDWORD32 data, UINT round);

	void subBytes_ShiftRows(LPBYTE data);
	void invSubBytes_ShiftRows(LPBYTE data);

	void subBytes_ShiftRows_MixColumns(LPBYTE data);
	void invSubBytes_ShiftRows_MixColumns(LPBYTE data);

	void cipher(LPBYTE data);
	void invCipher(LPBYTE data);
};

BOOL Check_CPU_support_AES();
