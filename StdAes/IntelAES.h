#pragma once

#include <Windows.h>
#include <intrin.h>

BOOL CheckAESNI();

void AES_128_Key_Expansion(PDWORD32 userkey, PDWORD32 key);
void AES_192_Key_Expansion(PDWORD32 userkey, PDWORD32 key);
void AES_256_Key_Expansion(PDWORD32 userkey, PDWORD32 key);

void AESNI_crypt(LPBYTE in, PDWORD32 key, int number_of_rounds);
void AESNI_invcrypt(LPBYTE in, PDWORD32 key, int number_of_rounds);