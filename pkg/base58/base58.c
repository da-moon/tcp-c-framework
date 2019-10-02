#include "base58.h"
static unsigned char INDEXES[128] = { -1 };
unsigned char *  Encode( unsigned char *in, int inLen, int *outLen )
{
   if(inLen == 0)
		return NULL;

	unsigned char *inCopy = malloc(inLen);
	memcpy(inCopy, in, inLen);

	//count leading zeros
	int z = -1;
	while(z < inLen && inCopy[++z] == 0x00)
		;

	int j = inLen * 2;
	int inLen_x2 = j;
	unsigned char *temp = malloc(inLen_x2);

	//skip leading zeros and encode from startAt
	int startAt = z;
	while(startAt < inLen){
		unsigned char mod = divmod58(inCopy, inLen, startAt);
		if(inCopy[startAt] == 0)
			++startAt;

		temp[--j] = dictionary[mod];
	}

	free(inCopy);

	while(j<inLen_x2 && temp[j] == '1')		j++;

	while(--z >= 0)
		temp[--j] = '1';


	*outLen = inLen_x2 - j;

	int len = inLen_x2 - j;

	unsigned char *out = malloc(len + 1);
	out[len] = 0;
	memcpy(out, temp + j, len);
	free(temp);

	return out;

}


unsigned char * Decode( unsigned char *in, int inLen)
{
  if(inLen == 0)
		return NULL;

	unsigned char *input58 = malloc(inLen);
	unsigned char *indexes = getIndexes();

	int i=0;
	for(; i<inLen; i++){
		input58[i] = indexes[in[i]];
	}

	//count leading zeros
	int z = -1;
	while(z<inLen && input58[++z] == 0x00)
		;

	unsigned char *temp = malloc(inLen);
	int j = inLen;

	int startAt = z;
	while(startAt < inLen){
		char mod = divmod256(input58, inLen, startAt);
		if(input58[startAt] == 0)
			++startAt;

		temp[--j] = mod;
	}

	free(input58);

	while(j<inLen && temp[j] == 0)		j++;

	int len = inLen - j + z;
	unsigned char *out = malloc(len + 1);
	out[len] = 0;
	memcpy(out, temp + j - z, len);
	free(temp);

	return out;

}

unsigned char * getIndexes(){
	int i;

	for (i = 0; i < 58; i++)
		INDEXES[(int)dictionary[i]] = i;

	return INDEXES;
}

unsigned char divmod58(unsigned char *in, int inLen, int i){
	int rem = 0;
	for(;i<inLen; i++){
		rem = rem * 256 + in[i];
		in[i] = rem / 58;
		rem = rem % 58;
	}
	return rem & 0xFF;
}
unsigned char divmod256(unsigned char *in, int inLen, int i){
	int rem = 0;
	for(;i<inLen; i++){
		rem = rem * 58 + in[i];
		in[i] = rem / 256;
		rem = rem % 256;
	}
	return rem & 0xFF;
}

