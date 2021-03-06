﻿#include "AES.h"


/////////////////////////////////////////////////////////
// sBox置换表
// 用特定算法生成的一张置换16x16表格，加密时用
/////////////////////////////////////////////////////////
static const BYTE sBox[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
	/*1*/0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
	/*2*/0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
	/*3*/0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
	/*4*/0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
	/*5*/0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
	/*6*/0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
	/*7*/0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
	/*8*/0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
	/*9*/0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
	/*A*/0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
	/*B*/0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
	/*C*/0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
	/*D*/0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
	/*E*/0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
	/*F*/0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/////////////////////////////////////////////////////////
// 逆sBox置换表
// 解密时用
/////////////////////////////////////////////////////////
static const BYTE invSBox[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	/*1*/0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	/*2*/0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	/*3*/0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	/*4*/0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	/*5*/0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	/*6*/0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	/*7*/0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	/*8*/0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	/*9*/0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	/*A*/0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	/*B*/0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	/*C*/0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	/*D*/0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	/*E*/0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	/*F*/0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

//////////////////////////////////////////////////////////
// GF(2^8)置换表
// 为了提高计算速度，将GF域内的所有值预先计算出来。
// 用的时候直接查表得到结果，而不需要计算。
// 算法请见具体函数
//////////////////////////////////////////////////////////
static const BYTE gf02[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0xC6, 0xF8, 0xEE, 0xF6, 0xFF, 0xD6, 0xDE, 0x91, 0x60, 0x02, 0xCE, 0x56, 0xE7, 0xB5, 0x4D, 0xEC,
	/*1*/0x8F, 0x1F, 0x89, 0xFA, 0xEF, 0xB2, 0x8E, 0xFB, 0x41, 0xB3, 0x5F, 0x45, 0x23, 0x53, 0xE4, 0x9B,
	/*2*/0x75, 0xE1, 0x3D, 0x4C, 0x6C, 0x7E, 0xF5, 0x83, 0x68, 0x51, 0xD1, 0xF9, 0xE2, 0xAB, 0x62, 0x2A,
	/*3*/0x08, 0x95, 0x46, 0x9D, 0x30, 0x37, 0x0A, 0x2F, 0x0E, 0x24, 0x1B, 0xDF, 0xCD, 0x4E, 0x7F, 0xEA,
	/*4*/0x12, 0x1D, 0x58, 0x34, 0x36, 0xDC, 0xB4, 0x5B, 0xA4, 0x76, 0xB7, 0x7D, 0x52, 0xDD, 0x5E, 0x13,
	/*5*/0xA6, 0xB9, 0x00, 0xC1, 0x40, 0xE3, 0x79, 0xB6, 0xD4, 0x8D, 0x67, 0x72, 0x94, 0x98, 0xB0, 0x85,
	/*6*/0xBB, 0xC5, 0x4F, 0xED, 0x86, 0x9A, 0x66, 0x11, 0x8A, 0xE9, 0x04, 0xFE, 0xA0, 0x78, 0x25, 0x4B,
	/*7*/0xA2, 0x5D, 0x80, 0x05, 0x3F, 0x21, 0x70, 0xF1, 0x63, 0x77, 0xAF, 0x42, 0x20, 0xE5, 0xFD, 0xBF,
	/*8*/0x81, 0x18, 0x26, 0xC3, 0xBE, 0x35, 0x88, 0x2E, 0x93, 0x55, 0xFC, 0x7A, 0xC8, 0xBA, 0x32, 0xE6,
	/*9*/0xC0, 0x19, 0x9E, 0xA3, 0x44, 0x54, 0x3B, 0x0B, 0x8C, 0xC7, 0x6B, 0x28, 0xA7, 0xBC, 0x16, 0xAD,
	/*A*/0xDB, 0x64, 0x74, 0x14, 0x92, 0x0C, 0x48, 0xB8, 0x9F, 0xBD, 0x43, 0xC4, 0x39, 0x31, 0xD3, 0xF2,
	/*B*/0xD5, 0x8B, 0x6E, 0xDA, 0x01, 0xB1, 0x9C, 0x49, 0xD8, 0xAC, 0xF3, 0xCF, 0xCA, 0xF4, 0x47, 0x10,
	/*C*/0x6F, 0xF0, 0x4A, 0x5C, 0x38, 0x57, 0x73, 0x97, 0xCB, 0xA1, 0xE8, 0x3E, 0x96, 0x61, 0x0D, 0x0F,
	/*D*/0xE0, 0x7C, 0x71, 0xCC, 0x90, 0x06, 0xF7, 0x1C, 0xC2, 0x6A, 0xAE, 0x69, 0x17, 0x99, 0x3A, 0x27,
	/*E*/0xD9, 0xEB, 0x2B, 0x22, 0xD2, 0xA9, 0x07, 0x33, 0x2D, 0x3C, 0x15, 0xC9, 0x87, 0xAA, 0x50, 0xA5,
	/*F*/0x03, 0x59, 0x09, 0x1A, 0x65, 0xD7, 0x84, 0xD0, 0x82, 0x29, 0x5A, 0x1E, 0x7B, 0xA8, 0x6D, 0x2C
};

static const BYTE gf03[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0xA5, 0x84, 0x99, 0x8D, 0x0D, 0xBD, 0xB1, 0x54, 0x50, 0x03, 0xA9, 0x7D, 0x19, 0x62, 0xE6, 0x9A,
	/*1*/0x45, 0x9D, 0x40, 0x87, 0x15, 0xEB, 0xC9, 0x0B, 0xEC, 0x67, 0xFD, 0xEA, 0xBF, 0xF7, 0x96, 0x5B,
	/*2*/0xC2, 0x1C, 0xAE, 0x6A, 0x5A, 0x41, 0x02, 0x4F, 0x5C, 0xF4, 0x34, 0x08, 0x93, 0x73, 0x53, 0x3F,
	/*3*/0x0C, 0x52, 0x65, 0x5E, 0x28, 0xA1, 0x0F, 0xB5, 0x09, 0x36, 0x9B, 0x3D, 0x26, 0x69, 0xCD, 0x9F,
	/*4*/0x1B, 0x9E, 0x74, 0x2E, 0x2D, 0xB2, 0xEE, 0xFB, 0xF6, 0x4D, 0x61, 0xCE, 0x7B, 0x3E, 0x71, 0x97,
	/*5*/0xF5, 0x68, 0x00, 0x2C, 0x60, 0x1F, 0xC8, 0xED, 0xBE, 0x46, 0xD9, 0x4B, 0xDE, 0xD4, 0xE8, 0x4A,
	/*6*/0x6B, 0x2A, 0xE5, 0x16, 0xC5, 0xD7, 0x55, 0x94, 0xCF, 0x10, 0x06, 0x81, 0xF0, 0x44, 0xBA, 0xE3,
	/*7*/0xF3, 0xFE, 0xC0, 0x8A, 0xAD, 0xBC, 0x48, 0x04, 0xDF, 0xC1, 0x75, 0x63, 0x30, 0x1A, 0x0E, 0x6D,
	/*8*/0x4C, 0x14, 0x35, 0x2F, 0xE1, 0xA2, 0xCC, 0x39, 0x57, 0xF2, 0x82, 0x47, 0xAC, 0xE7, 0x2B, 0x95,
	/*9*/0xA0, 0x98, 0xD1, 0x7F, 0x66, 0x7E, 0xAB, 0x83, 0xCA, 0x29, 0xD3, 0x3C, 0x79, 0xE2, 0x1D, 0x76,
	/*A*/0x3B, 0x56, 0x4E, 0x1E, 0xDB, 0x0A, 0x6C, 0xE4, 0x5D, 0x6E, 0xEF, 0xA6, 0xA8, 0xA4, 0x37, 0x8B,
	/*B*/0x32, 0x43, 0x59, 0xB7, 0x8C, 0x64, 0xD2, 0xE0, 0xB4, 0xFA, 0x07, 0x25, 0xAF, 0x8E, 0xE9, 0x18,
	/*C*/0xD5, 0x88, 0x6F, 0x72, 0x24, 0xF1, 0xC7, 0x51, 0x23, 0x7C, 0x9C, 0x21, 0xDD, 0xDC, 0x86, 0x85,
	/*D*/0x90, 0x42, 0xC4, 0xAA, 0xD8, 0x05, 0x01, 0x12, 0xA3, 0x5F, 0xF9, 0xD0, 0x91, 0x58, 0x27, 0xB9,
	/*E*/0x38, 0x13, 0xB3, 0x33, 0xBB, 0x70, 0x89, 0xA7, 0xB6, 0x22, 0x92, 0x20, 0x49, 0xFF, 0x78, 0x7A,
	/*F*/0x8F, 0xF8, 0x80, 0x17, 0xDA, 0x31, 0xC6, 0xB8, 0xC3, 0xB0, 0x77, 0x11, 0xCB, 0xFC, 0xD6, 0x3A
};

static const BYTE gf09[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77,
	/*1*/0x90, 0x99, 0x82, 0x8B, 0xB4, 0xBD, 0xA6, 0xAF, 0xD8, 0xD1, 0xCA, 0xC3, 0xFC, 0xF5, 0xEE, 0xE7,
	/*2*/0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04, 0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C,
	/*3*/0xAB, 0xA2, 0xB9, 0xB0, 0x8F, 0x86, 0x9D, 0x94, 0xE3, 0xEA, 0xF1, 0xF8, 0xC7, 0xCE, 0xD5, 0xDC,
	/*4*/0x76, 0x7F, 0x64, 0x6D, 0x52, 0x5B, 0x40, 0x49, 0x3E, 0x37, 0x2C, 0x25, 0x1A, 0x13, 0x08, 0x01,
	/*5*/0xE6, 0xEF, 0xF4, 0xFD, 0xC2, 0xCB, 0xD0, 0xD9, 0xAE, 0xA7, 0xBC, 0xB5, 0x8A, 0x83, 0x98, 0x91,
	/*6*/0x4D, 0x44, 0x5F, 0x56, 0x69, 0x60, 0x7B, 0x72, 0x05, 0x0C, 0x17, 0x1E, 0x21, 0x28, 0x33, 0x3A,
	/*7*/0xDD, 0xD4, 0xCF, 0xC6, 0xF9, 0xF0, 0xEB, 0xE2, 0x95, 0x9C, 0x87, 0x8E, 0xB1, 0xB8, 0xA3, 0xAA,
	/*8*/0xEC, 0xE5, 0xFE, 0xF7, 0xC8, 0xC1, 0xDA, 0xD3, 0xA4, 0xAD, 0xB6, 0xBF, 0x80, 0x89, 0x92, 0x9B,
	/*9*/0x7C, 0x75, 0x6E, 0x67, 0x58, 0x51, 0x4A, 0x43, 0x34, 0x3D, 0x26, 0x2F, 0x10, 0x19, 0x02, 0x0B,
	/*A*/0xD7, 0xDE, 0xC5, 0xCC, 0xF3, 0xFA, 0xE1, 0xE8, 0x9F, 0x96, 0x8D, 0x84, 0xBB, 0xB2, 0xA9, 0xA0,
	/*B*/0x47, 0x4E, 0x55, 0x5C, 0x63, 0x6A, 0x71, 0x78, 0x0F, 0x06, 0x1D, 0x14, 0x2B, 0x22, 0x39, 0x30,
	/*C*/0x9A, 0x93, 0x88, 0x81, 0xBE, 0xB7, 0xAC, 0xA5, 0xD2, 0xDB, 0xC0, 0xC9, 0xF6, 0xFF, 0xE4, 0xED,
	/*D*/0x0A, 0x03, 0x18, 0x11, 0x2E, 0x27, 0x3C, 0x35, 0x42, 0x4B, 0x50, 0x59, 0x66, 0x6F, 0x74, 0x7D,
	/*E*/0xA1, 0xA8, 0xB3, 0xBA, 0x85, 0x8C, 0x97, 0x9E, 0xE9, 0xE0, 0xFB, 0xF2, 0xCD, 0xC4, 0xDF, 0xD6,
	/*F*/0x31, 0x38, 0x23, 0x2A, 0x15, 0x1C, 0x07, 0x0E, 0x79, 0x70, 0x6B, 0x62, 0x5D, 0x54, 0x4F, 0x46
};

static const BYTE gf0b[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x00, 0x0B, 0x16, 0x1D, 0x2C, 0x27, 0x3A, 0x31, 0x58, 0x53, 0x4E, 0x45, 0x74, 0x7F, 0x62, 0x69,
	/*1*/0xB0, 0xBB, 0xA6, 0xAD, 0x9C, 0x97, 0x8A, 0x81, 0xE8, 0xE3, 0xFE, 0xF5, 0xC4, 0xCF, 0xD2, 0xD9,
	/*2*/0x7B, 0x70, 0x6D, 0x66, 0x57, 0x5C, 0x41, 0x4A, 0x23, 0x28, 0x35, 0x3E, 0x0F, 0x04, 0x19, 0x12,
	/*3*/0xCB, 0xC0, 0xDD, 0xD6, 0xE7, 0xEC, 0xF1, 0xFA, 0x93, 0x98, 0x85, 0x8E, 0xBF, 0xB4, 0xA9, 0xA2,
	/*4*/0xF6, 0xFD, 0xE0, 0xEB, 0xDA, 0xD1, 0xCC, 0xC7, 0xAE, 0xA5, 0xB8, 0xB3, 0x82, 0x89, 0x94, 0x9F,
	/*5*/0x46, 0x4D, 0x50, 0x5B, 0x6A, 0x61, 0x7C, 0x77, 0x1E, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2F,
	/*6*/0x8D, 0x86, 0x9B, 0x90, 0xA1, 0xAA, 0xB7, 0xBC, 0xD5, 0xDE, 0xC3, 0xC8, 0xF9, 0xF2, 0xEF, 0xE4,
	/*7*/0x3D, 0x36, 0x2B, 0x20, 0x11, 0x1A, 0x07, 0x0C, 0x65, 0x6E, 0x73, 0x78, 0x49, 0x42, 0x5F, 0x54,
	/*8*/0xF7, 0xFC, 0xE1, 0xEA, 0xDB, 0xD0, 0xCD, 0xC6, 0xAF, 0xA4, 0xB9, 0xB2, 0x83, 0x88, 0x95, 0x9E,
	/*9*/0x47, 0x4C, 0x51, 0x5A, 0x6B, 0x60, 0x7D, 0x76, 0x1F, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2E,
	/*A*/0x8C, 0x87, 0x9A, 0x91, 0xA0, 0xAB, 0xB6, 0xBD, 0xD4, 0xDF, 0xC2, 0xC9, 0xF8, 0xF3, 0xEE, 0xE5,
	/*B*/0x3C, 0x37, 0x2A, 0x21, 0x10, 0x1B, 0x06, 0x0D, 0x64, 0x6F, 0x72, 0x79, 0x48, 0x43, 0x5E, 0x55,
	/*C*/0x01, 0x0A, 0x17, 0x1C, 0x2D, 0x26, 0x3B, 0x30, 0x59, 0x52, 0x4F, 0x44, 0x75, 0x7E, 0x63, 0x68,
	/*D*/0xB1, 0xBA, 0xA7, 0xAC, 0x9D, 0x96, 0x8B, 0x80, 0xE9, 0xE2, 0xFF, 0xF4, 0xC5, 0xCE, 0xD3, 0xD8,
	/*E*/0x7A, 0x71, 0x6C, 0x67, 0x56, 0x5D, 0x40, 0x4B, 0x22, 0x29, 0x34, 0x3F, 0x0E, 0x05, 0x18, 0x13,
	/*F*/0xCA, 0xC1, 0xDC, 0xD7, 0xE6, 0xED, 0xF0, 0xFB, 0x92, 0x99, 0x84, 0x8F, 0xBE, 0xB5, 0xA8, 0xA3
};

static const BYTE gf0d[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x00, 0x0D, 0x1A, 0x17, 0x34, 0x39, 0x2E, 0x23, 0x68, 0x65, 0x72, 0x7F, 0x5C, 0x51, 0x46, 0x4B,
	/*1*/0xD0, 0xDD, 0xCA, 0xC7, 0xE4, 0xE9, 0xFE, 0xF3, 0xB8, 0xB5, 0xA2, 0xAF, 0x8C, 0x81, 0x96, 0x9B,
	/*2*/0xBB, 0xB6, 0xA1, 0xAC, 0x8F, 0x82, 0x95, 0x98, 0xD3, 0xDE, 0xC9, 0xC4, 0xE7, 0xEA, 0xFD, 0xF0,
	/*3*/0x6B, 0x66, 0x71, 0x7C, 0x5F, 0x52, 0x45, 0x48, 0x03, 0x0E, 0x19, 0x14, 0x37, 0x3A, 0x2D, 0x20,
	/*4*/0x6D, 0x60, 0x77, 0x7A, 0x59, 0x54, 0x43, 0x4E, 0x05, 0x08, 0x1F, 0x12, 0x31, 0x3C, 0x2B, 0x26,
	/*5*/0xBD, 0xB0, 0xA7, 0xAA, 0x89, 0x84, 0x93, 0x9E, 0xD5, 0xD8, 0xCF, 0xC2, 0xE1, 0xEC, 0xFB, 0xF6,
	/*6*/0xD6, 0xDB, 0xCC, 0xC1, 0xE2, 0xEF, 0xF8, 0xF5, 0xBE, 0xB3, 0xA4, 0xA9, 0x8A, 0x87, 0x90, 0x9D,
	/*7*/0x06, 0x0B, 0x1C, 0x11, 0x32, 0x3F, 0x28, 0x25, 0x6E, 0x63, 0x74, 0x79, 0x5A, 0x57, 0x40, 0x4D,
	/*8*/0xDA, 0xD7, 0xC0, 0xCD, 0xEE, 0xE3, 0xF4, 0xF9, 0xB2, 0xBF, 0xA8, 0xA5, 0x86, 0x8B, 0x9C, 0x91,
	/*9*/0x0A, 0x07, 0x10, 0x1D, 0x3E, 0x33, 0x24, 0x29, 0x62, 0x6F, 0x78, 0x75, 0x56, 0x5B, 0x4C, 0x41,
	/*A*/0x61, 0x6C, 0x7B, 0x76, 0x55, 0x58, 0x4F, 0x42, 0x09, 0x04, 0x13, 0x1E, 0x3D, 0x30, 0x27, 0x2A,
	/*B*/0xB1, 0xBC, 0xAB, 0xA6, 0x85, 0x88, 0x9F, 0x92, 0xD9, 0xD4, 0xC3, 0xCE, 0xED, 0xE0, 0xF7, 0xFA,
	/*C*/0xB7, 0xBA, 0xAD, 0xA0, 0x83, 0x8E, 0x99, 0x94, 0xDF, 0xD2, 0xC5, 0xC8, 0xEB, 0xE6, 0xF1, 0xFC,
	/*D*/0x67, 0x6A, 0x7D, 0x70, 0x53, 0x5E, 0x49, 0x44, 0x0F, 0x02, 0x15, 0x18, 0x3B, 0x36, 0x21, 0x2C,
	/*E*/0x0C, 0x01, 0x16, 0x1B, 0x38, 0x35, 0x22, 0x2F, 0x64, 0x69, 0x7E, 0x73, 0x50, 0x5D, 0x4A, 0x47,
	/*F*/0xDC, 0xD1, 0xC6, 0xCB, 0xE8, 0xE5, 0xF2, 0xFF, 0xB4, 0xB9, 0xAE, 0xA3, 0x80, 0x8D, 0x9A, 0x97
};

static const BYTE gf0e[] = {
	/*     0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
	/*0*/0x00, 0x0E, 0x1C, 0x12, 0x38, 0x36, 0x24, 0x2A, 0x70, 0x7E, 0x6C, 0x62, 0x48, 0x46, 0x54, 0x5A,
	/*1*/0xE0, 0xEE, 0xFC, 0xF2, 0xD8, 0xD6, 0xC4, 0xCA, 0x90, 0x9E, 0x8C, 0x82, 0xA8, 0xA6, 0xB4, 0xBA,
	/*2*/0xDB, 0xD5, 0xC7, 0xC9, 0xE3, 0xED, 0xFF, 0xF1, 0xAB, 0xA5, 0xB7, 0xB9, 0x93, 0x9D, 0x8F, 0x81,
	/*3*/0x3B, 0x35, 0x27, 0x29, 0x03, 0x0D, 0x1F, 0x11, 0x4B, 0x45, 0x57, 0x59, 0x73, 0x7D, 0x6F, 0x61,
	/*4*/0xAD, 0xA3, 0xB1, 0xBF, 0x95, 0x9B, 0x89, 0x87, 0xDD, 0xD3, 0xC1, 0xCF, 0xE5, 0xEB, 0xF9, 0xF7,
	/*5*/0x4D, 0x43, 0x51, 0x5F, 0x75, 0x7B, 0x69, 0x67, 0x3D, 0x33, 0x21, 0x2F, 0x05, 0x0B, 0x19, 0x17,
	/*6*/0x76, 0x78, 0x6A, 0x64, 0x4E, 0x40, 0x52, 0x5C, 0x06, 0x08, 0x1A, 0x14, 0x3E, 0x30, 0x22, 0x2C,
	/*7*/0x96, 0x98, 0x8A, 0x84, 0xAE, 0xA0, 0xB2, 0xBC, 0xE6, 0xE8, 0xFA, 0xF4, 0xDE, 0xD0, 0xC2, 0xCC,
	/*8*/0x41, 0x4F, 0x5D, 0x53, 0x79, 0x77, 0x65, 0x6B, 0x31, 0x3F, 0x2D, 0x23, 0x09, 0x07, 0x15, 0x1B,
	/*9*/0xA1, 0xAF, 0xBD, 0xB3, 0x99, 0x97, 0x85, 0x8B, 0xD1, 0xDF, 0xCD, 0xC3, 0xE9, 0xE7, 0xF5, 0xFB,
	/*A*/0x9A, 0x94, 0x86, 0x88, 0xA2, 0xAC, 0xBE, 0xB0, 0xEA, 0xE4, 0xF6, 0xF8, 0xD2, 0xDC, 0xCE, 0xC0,
	/*B*/0x7A, 0x74, 0x66, 0x68, 0x42, 0x4C, 0x5E, 0x50, 0x0A, 0x04, 0x16, 0x18, 0x32, 0x3C, 0x2E, 0x20,
	/*C*/0xEC, 0xE2, 0xF0, 0xFE, 0xD4, 0xDA, 0xC8, 0xC6, 0x9C, 0x92, 0x80, 0x8E, 0xA4, 0xAA, 0xB8, 0xB6,
	/*D*/0x0C, 0x02, 0x10, 0x1E, 0x34, 0x3A, 0x28, 0x26, 0x7C, 0x72, 0x60, 0x6E, 0x44, 0x4A, 0x58, 0x56,
	/*E*/0x37, 0x39, 0x2B, 0x25, 0x0F, 0x01, 0x13, 0x1D, 0x47, 0x49, 0x5B, 0x55, 0x7F, 0x71, 0x63, 0x6D,
	/*F*/0xD7, 0xD9, 0xCB, 0xC5, 0xEF, 0xE1, 0xF3, 0xFD, 0xA7, 0xA9, 0xBB, 0xB5, 0x9F, 0x91, 0x83, 0x8D
};

//////////////////////////////////////////////////////////
// 轮常量
// 在扩展密钥中使用的常量
//////////////////////////////////////////////////////////
static const INT Rcon[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x6C, 0xD8, 0xAB, 0x4D
};

// AES的块大小固定为16字节，不允许改变！
#define BLOCK_LENGTH 16

BOOL AESNI = FALSE;

StdAES::StdAES(INT keyMode, INT AESMode, INT padding) :mExpansionTable(NULL) {
	AESNI = CheckAESNI();
	mKeyLength = keyMode;
	mKeyColumn = mKeyLength / 4;
	mRounds = mKeyColumn + 6;
	mExpansionTableLength = (mRounds + 1) * mKeyColumn;

	if(AESNI) {
		mExpansionTable = (/*__declspec(align(16)) */PDWORD32) _mm_malloc(sizeof(PDWORD32)*mExpansionTableLength, 16);
	} else {
		mExpansionTable = new DWORD32[mExpansionTableLength];
	}

	mMode = AESMode;
	mPadding = padding;
}

StdAES::~StdAES() {
	if(AESNI) {
		_mm_free(mExpansionTable);
	} else {
		delete[] mExpansionTable;
	}
}

////////////////////////////////////////////////
// 设置AES加解密的密钥
////////////////////////////////////////////////
void StdAES::setKey(LPBYTE key) {
	if(AESNI) {
		if(mKeyLength == STDAES_KEY_128BIT) {
			AES_128_Key_Expansion((PDWORD32) key, mExpansionTable);
		} else if(mKeyLength == STDAES_KEY_192BIT) {
			AES_192_Key_Expansion((PDWORD32) key, mExpansionTable);
		} else {
			AES_256_Key_Expansion((PDWORD32) key, mExpansionTable);
		}
	} else {
		keyExpansion((PDWORD32) key);
	}
}

///////////////////////////////////////////////
// 密钥扩展
// 扩展后的密钥表长度为(密钥长度) + (密钥长度) * 轮数
// 例如输入的密钥是16字节，则扩展后的密钥表为16+16*10=176字节
///////////////////////////////////////////////
void StdAES::keyExpansion(PDWORD32 key) {
	// 将原始密钥拷贝到密钥扩展表中
	memcpy(mExpansionTable, key, mKeyLength);
	for(INT i = mKeyColumn; i < mExpansionTableLength; i++) {
		mExpansionTable[i] = mExpansionTable[i - 1];
		if(i  % mKeyColumn == 0) { // 每组密钥的第一个WORD
			mExpansionTable[i] = ((mExpansionTable[i]) << 24) | (mExpansionTable[i] >> 8);
			subWord((LPBYTE) &mExpansionTable[i]);
			mExpansionTable[i] ^= Rcon[i / mKeyColumn - 1];
		} else if(mKeyLength == STDAES_KEY_256BIT && (i % 4 == 0)) {
			subWord((LPBYTE) &mExpansionTable[i]);
		}
		mExpansionTable[i] ^= mExpansionTable[i - mKeyColumn];
	}
}

/////////////////////////////////////////////////
// 取得带填充的数据长度，用于设置缓冲区大小
/////////////////////////////////////////////////
INT StdAES::getBufferLen(INT dataLen) {
	switch(mPadding) {
		case STDAES_PADDING_PKCS5:
		case STDAES_PADDING_ISO10126:
			return (((INT) (dataLen / BLOCK_LENGTH) + 1) * BLOCK_LENGTH);
		default:
			return dataLen;
	}
}

inline void blockXor(PDWORD32 data, PDWORD32 _xor) {
	for(INT i = 0; i < BLOCK_LENGTH / sizeof(DWORD32); i++) {
		data[i] ^= _xor[i];
	}

	//_mm_storeu_si128((__m128i*)data, _mm_xor_si128(*((__m128i*)data), *((__m128i*)_xor)));
}

void StdAES::padding(LPBYTE buffer, INT buffLen, LPBYTE data, INT dataLen) {
	memcpy(buffer, data, dataLen);
	switch(mPadding) {
		case STDAES_PADDING_PKCS5: {
			UINT paddingLen = buffLen - dataLen;
			memset(&buffer[dataLen], paddingLen, paddingLen);
			break;
		}
		case STDAES_PADDING_ISO10126:
			buffer[buffLen - 1] = buffLen - dataLen;
			break;
		default:
			break;
	}
}

void StdAES::deletePadding(LPBYTE buffer, LPINT buffLen) {
	switch(mPadding) {
		case STDAES_PADDING_PKCS5:
		case STDAES_PADDING_ISO10126:
			*buffLen = *buffLen - buffer[*buffLen - 1];
			break;
		default:
			break;
	}
}

void StdAES::encrypt(LPBYTE buffer, INT buffLen) {
	encrypt(buffer, buffLen, nullptr);
}


void StdAES::encrypt(LPBYTE buffer, INT buffLen, LPBYTE IV) {
	BYTE myiv[16];
	switch(mMode) {
		case STDAES_MODE_ECB:
			for(INT i = 0; i < buffLen; i += BLOCK_LENGTH) {
				cipher(&buffer[i]);
			}
			break;
		case STDAES_MODE_CBC:
			memcpy(myiv, IV, BLOCK_LENGTH);
			for(INT i = 0; i < buffLen; i += BLOCK_LENGTH) {
				blockXor((PDWORD32) &buffer[i], (PDWORD32) myiv);
				cipher(&buffer[i]);
				// 加密后的数据作为下一次的IV
				memcpy(myiv, &buffer[i], BLOCK_LENGTH);
			}
			break;
		case STDAES_MODE_CFB: {
			memcpy(myiv, IV, BLOCK_LENGTH);
			for(INT i = 0; i < buffLen; i += BLOCK_LENGTH) {
				cipher(myiv);
				blockXor((PDWORD32) &buffer[i], (PDWORD32) myiv);
				// 加密后的数据作为下一次的IV
				memcpy(myiv, &buffer[i], BLOCK_LENGTH);
			}
			break;
		}
		case STDAES_MODE_OFB: {
			memcpy(myiv, IV, BLOCK_LENGTH);
			for(INT i = 0; i < buffLen; i += BLOCK_LENGTH) {
				cipher(myiv);
				blockXor((PDWORD32) &buffer[i], (PDWORD32) myiv);
			}
			break;
		}
		default:
			break;
	}
}

/////////////////////////////////////////////////////
// AES解密函数
// 解密一段数据
/////////////////////////////////////////////////////
void StdAES::decrypt(LPBYTE data, INT dataLen, LPBYTE iv) {
	BYTE bac[BLOCK_LENGTH];
	switch(mMode) {
		case STDAES_MODE_ECB:
			for(INT i = 0; i < dataLen; i += BLOCK_LENGTH) {
				invCipher(&data[i]);
			}
			break;
		case STDAES_MODE_CBC: {
			for(INT i = 0; i < dataLen; i += BLOCK_LENGTH) {
				memcpy(bac, &data[i], BLOCK_LENGTH);
				invCipher(&data[i]);
				blockXor((PDWORD32) &data[i], (PDWORD32) iv);
				memcpy(iv, bac, BLOCK_LENGTH);
			}
			break;
		}
		case STDAES_MODE_CFB: {
			for(INT i = 0; i < dataLen; i += BLOCK_LENGTH) {
				memcpy(bac, &data[i], BLOCK_LENGTH);
				cipher(iv);
				blockXor((PDWORD32) &data[i], (PDWORD32) iv);
				memcpy(iv, bac, BLOCK_LENGTH);
			}
			break;
		}
		case STDAES_MODE_OFB:
			encrypt(data, dataLen, iv);
			break;
		default:
			break;
	}
}

/////////////////////////////////////////////////////
// AES数据块加密函数
// 加密一个数据块
/////////////////////////////////////////////////////
void StdAES::cipher(LPBYTE data) {
	if(AESNI) {
		AESNI_crypt(data, mExpansionTable, mRounds);
	} else {
		addRoundKey((PDWORD32) data, 0);

		for(INT i = 1; i < mRounds; i++) {
			subBytes_ShiftRows_MixColumns(data);
			addRoundKey((PDWORD32) data, i);
		}

		subBytes_ShiftRows(data);
		addRoundKey((PDWORD32) data, mRounds);
	}
}

/////////////////////////////////////////////////////
// AES数据块解密函数
// 解密一个数据块
/////////////////////////////////////////////////////
void StdAES::invCipher(LPBYTE data) {
	if(AESNI) {
		AESNI_invcrypt(data, mExpansionTable, mRounds);
	} else {
		addRoundKey((PDWORD32) data, mRounds);
		invSubBytes_ShiftRows(data);

		for(INT i = mRounds - 1; i > 0; i--) {
			addRoundKey((PDWORD32) data, i);
			invSubBytes_ShiftRows_MixColumns(data);
		}

		addRoundKey((PDWORD32) data, 0);
	}
}

//////////////////////////////////////////////////////////
// 与轮密钥按位异或
//////////////////////////////////////////////////////////
void StdAES::addRoundKey(PDWORD32 data, UINT round) {
	data[0] ^= mExpansionTable[round * 4];
	data[1] ^= mExpansionTable[round * 4 + 1];
	data[2] ^= mExpansionTable[round * 4 + 2];
	data[3] ^= mExpansionTable[round * 4 + 3];
}

//////////////////////////////////////////////////////////
// 该函数是subBytes、shiftRows和mixColumns这三个函数的整合及优化
//////////////////////////////////////////////////////////
void StdAES::subBytes_ShiftRows_MixColumns(LPBYTE data) {
	BYTE tmp[BLOCK_LENGTH] =
	{
		(BYTE) (gf02[data[0]] ^ gf03[data[5]] ^ sBox[data[10]] ^ sBox[data[15]]),
		(BYTE) (sBox[data[0]] ^ gf02[data[5]] ^ gf03[data[10]] ^ sBox[data[15]]),
		(BYTE) (sBox[data[0]] ^ sBox[data[5]] ^ gf02[data[10]] ^ gf03[data[15]]),
		(BYTE) (gf03[data[0]] ^ sBox[data[5]] ^ sBox[data[10]] ^ gf02[data[15]]),

		(BYTE) (gf02[data[4]] ^ gf03[data[9]] ^ sBox[data[14]] ^ sBox[data[3]]),
		(BYTE) (sBox[data[4]] ^ gf02[data[9]] ^ gf03[data[14]] ^ sBox[data[3]]),
		(BYTE) (sBox[data[4]] ^ sBox[data[9]] ^ gf02[data[14]] ^ gf03[data[3]]),
		(BYTE) (gf03[data[4]] ^ sBox[data[9]] ^ sBox[data[14]] ^ gf02[data[3]]),

		(BYTE) (gf02[data[8]] ^ gf03[data[13]] ^ sBox[data[2]] ^ sBox[data[7]]),
		(BYTE) (sBox[data[8]] ^ gf02[data[13]] ^ gf03[data[2]] ^ sBox[data[7]]),
		(BYTE) (sBox[data[8]] ^ sBox[data[13]] ^ gf02[data[2]] ^ gf03[data[7]]),
		(BYTE) (gf03[data[8]] ^ sBox[data[13]] ^ sBox[data[2]] ^ gf02[data[7]]),

		(BYTE) (gf02[data[12]] ^ gf03[data[1]] ^ sBox[data[6]] ^ sBox[data[11]]),
		(BYTE) (sBox[data[12]] ^ gf02[data[1]] ^ gf03[data[6]] ^ sBox[data[11]]),
		(BYTE) (sBox[data[12]] ^ sBox[data[1]] ^ gf02[data[6]] ^ gf03[data[11]]),
		(BYTE) (gf03[data[12]] ^ sBox[data[1]] ^ sBox[data[6]] ^ gf02[data[11]]),
	};
	memcpy(data, tmp, BLOCK_LENGTH);
}

//////////////////////////////////////////////////////////
// 该函数是subBytes、invShiftRows和invMixColumns这三个函数的整合及优化
//////////////////////////////////////////////////////////
void StdAES::invSubBytes_ShiftRows_MixColumns(LPBYTE data) {
	BYTE tmp[BLOCK_LENGTH] =
	{
		invSBox[gf0e[data[0]] ^ gf0b[data[1]] ^ gf0d[data[2]] ^ gf09[data[3]]],
		invSBox[gf09[data[12]] ^ gf0e[data[13]] ^ gf0b[data[14]] ^ gf0d[data[15]]],
		invSBox[gf0d[data[8]] ^ gf09[data[9]] ^ gf0e[data[10]] ^ gf0b[data[11]]],
		invSBox[gf0b[data[4]] ^ gf0d[data[5]] ^ gf09[data[6]] ^ gf0e[data[7]]],

		invSBox[gf0e[data[4]] ^ gf0b[data[5]] ^ gf0d[data[6]] ^ gf09[data[7]]],
		invSBox[gf09[data[0]] ^ gf0e[data[1]] ^ gf0b[data[2]] ^ gf0d[data[3]]],
		invSBox[gf0d[data[12]] ^ gf09[data[13]] ^ gf0e[data[14]] ^ gf0b[data[15]]],
		invSBox[gf0b[data[8]] ^ gf0d[data[9]] ^ gf09[data[10]] ^ gf0e[data[11]]],

		invSBox[gf0e[data[8]] ^ gf0b[data[9]] ^ gf0d[data[10]] ^ gf09[data[11]]],
		invSBox[gf09[data[4]] ^ gf0e[data[5]] ^ gf0b[data[6]] ^ gf0d[data[7]]],
		invSBox[gf0d[data[0]] ^ gf09[data[1]] ^ gf0e[data[2]] ^ gf0b[data[3]]],
		invSBox[gf0b[data[12]] ^ gf0d[data[13]] ^ gf09[data[14]] ^ gf0e[data[15]]],

		invSBox[gf0e[data[12]] ^ gf0b[data[13]] ^ gf0d[data[14]] ^ gf09[data[15]]],
		invSBox[gf09[data[8]] ^ gf0e[data[9]] ^ gf0b[data[10]] ^ gf0d[data[11]]],
		invSBox[gf0d[data[4]] ^ gf09[data[5]] ^ gf0e[data[6]] ^ gf0b[data[7]]],
		invSBox[gf0b[data[0]] ^ gf0d[data[1]] ^ gf09[data[2]] ^ gf0e[data[3]]],
	};
	memcpy(data, tmp, BLOCK_LENGTH);
}

void StdAES::subBytes_ShiftRows(LPBYTE data) {
	BYTE tmp[BLOCK_LENGTH] =
	{
		sBox[data[0]], sBox[data[5]], sBox[data[10]], sBox[data[15]],
		sBox[data[4]], sBox[data[9]], sBox[data[14]], sBox[data[3]],
		sBox[data[8]], sBox[data[13]], sBox[data[2]], sBox[data[7]],
		sBox[data[12]], sBox[data[1]], sBox[data[6]], sBox[data[11]],
	};
	memcpy(data, tmp, BLOCK_LENGTH);
}

void StdAES::invSubBytes_ShiftRows(LPBYTE data) {
	BYTE tmp[BLOCK_LENGTH] =
	{
		invSBox[data[0]], invSBox[data[13]], invSBox[data[10]], invSBox[data[7]],
		invSBox[data[4]], invSBox[data[1]], invSBox[data[14]], invSBox[data[11]],
		invSBox[data[8]], invSBox[data[5]], invSBox[data[2]], invSBox[data[15]],
		invSBox[data[12]], invSBox[data[9]], invSBox[data[6]], invSBox[data[3]],
	};
	memcpy(data, tmp, BLOCK_LENGTH);
}

////////////////////////////////////////////////////////
// 用sBox中的数据替换数据
////////////////////////////////////////////////////////
inline void StdAES::subWord(LPBYTE c) {
	*c++ = sBox[*c];
	*c++ = sBox[*c];
	*c++ = sBox[*c];
	*c = sBox[*c];
}


//void StdAES::subBytes(LPBYTE data) {
//	for (INT c = 0; c < 4; c++) {
//		subWord(data);
//		data += 4;
//	}
//}
//
//void StdAES::shiftRows(LPBYTE data) {
//	BYTE tmp[BLOCK_LENGTH] =
//	{
//		data[0], data[5], data[10], data[15],
//		data[4], data[9], data[14], data[3],
//		data[8], data[13], data[2], data[7],
//		data[12], data[1], data[6], data[11],
//	};
//	memcpy(data, tmp, BLOCK_LENGTH);
//}
//
//void StdAES::invShiftRows(LPBYTE data) {
//	BYTE tmp[BLOCK_LENGTH] =
//	{
//		data[0], data[13], data[10], data[7],
//		data[4], data[1], data[14], data[11],
//		data[8], data[5], data[2], data[15],
//		data[12], data[9], data[6], data[3],
//	};
//	memcpy(data, tmp, BLOCK_LENGTH);
//}

////////////////////////////////////////////////////////
// 1, 2, 3, 4  →  2, 3, 4, 1
////////////////////////////////////////////////////////
//void StdAES::rotWord(PDWORD32 dest, PDWORD32 src) {
//	*dest = ((*src & 0x000000FF) << 24) | (*src >> 8);
//	//dest[0] = src[1];
//	//dest[1] = src[2];
//	//dest[2] = src[3];
//	//dest[3] = src[0];
//}

//static BYTE gfmultby02(BYTE value) {
//	if (value & 0x80) {
//		return (value << 1) ^ 0x1B;
//	} else {
//		return value << 1;
//	}
//}
//
//static BYTE gfmultby03(BYTE value) {
//	return gfmultby02(value) ^ value;
//}

//void gf_9() {
//	LPBYTE tmp = new BYTE[256];
//	for (INT i = 0; i < 256; i++) {
//		tmp[i] = (BYTE) pow((double) 2, (double) i);
//	}
//	_TCHAR* out = new _TCHAR[3 * 256];
//	bytesToHex(256, tmp, out);
//	OutputDebugString(out);
//	//_tprintf(_T("%s\r\n"), out);
//	delete[] out;
//}