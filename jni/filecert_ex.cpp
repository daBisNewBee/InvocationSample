
#include "MyInvocation.h"
#include <stdlib.h>

long g_nLastError=0;

using namespace std;

extern "C"
{
	int LoadCert(unsigned char * bufCert, int * pRetLen);
	int PrivateEncrypt(int flen, const unsigned char *from, unsigned char *to, int nKeyLength, int padding);
	int PrivateDecrypt(int flen, const unsigned char *from, unsigned char *to, int nKeyLength, int padding);
	void GetTFName(char* szName);
}

//====================================================================================
JNIEnv* getJNIEnv(int* needsDetach)
 {
	JNIEnv* env = NULL;
	jint result = -1;
	if (g_JavaVM == NULL) {
		LOGE("filecert_ex#getJNIEnv# g_JavaVM is NULL");
		return NULL;
	}

	if (g_JavaVM->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		int status = g_JavaVM->AttachCurrentThread(&env, 0);
		if (status < 0) {
			LOGE("filecert_ex#getJNIEnv# failed to attach current thread");
			return NULL;
		}
		*needsDetach = 1;
	}
	return env;
}

// 0 on success,1 on failed
int LoadCert(unsigned char * bufCert, int * pRetLen)
{
	int nRet = 1, needsDetach, certLen;
	jbyte* pTmp;
	jbyteArray jba;
	JNIEnv* env;

	if (NULL == g_obj || NULL == g_mid_LoadX509Cert)
	{
		LOGE("filecert_ex#LoadCert# You need initialize Java Object,g_obj/g_mid_LoadX509Cert is NULL");
		goto ERR;
	}

	env = getJNIEnv(&needsDetach);
	if (NULL == env)
	{
		LOGE("filecert_ex#LoadCert# getJNIEnv failed!");
		goto ERR;
	}

	// Call the method on the object
	jba = (jbyteArray)env->CallObjectMethod(g_obj,g_mid_LoadX509Cert);
	if (NULL == jba)
	{
		LOGE("filecert_ex#LoadCert# CallObjectMethod failed!");
		goto ERR;
	}

	certLen = env->GetArrayLength(jba);
	pTmp = env->GetByteArrayElements(jba,0);
	memcpy(bufCert, pTmp, certLen);
	*pRetLen = certLen;
	env->ReleaseByteArrayElements(jba, pTmp, 0);
	nRet = 0;

	LOGD("filecert_ex#LoadCert# cert length:%u", certLen);
ERR:
	return nRet;
}

// return nKeyLength on success,0 on failed
int PrivateEncrypt(int flen, const unsigned char *from, unsigned char *to, int nKeyLength, int padding)
{
	int nRet = 1, needsDetach, signLen;
	jbyte* pTmp;
	jbyteArray jba_cipherText,jba_plainText;
	JNIEnv* env;

	if (NULL == g_obj || NULL == g_mid_X509PrivateEncrypt)
	{
		LOGE("filecert_ex#PrivateEncrypt# You need initialize Java Object,g_obj/g_mid_X509PrivateEncrypt is NULL");
		goto ERR;
	}

	env = getJNIEnv(&needsDetach);
	if (NULL == env)
	{
		LOGE("filecert_ex#PrivateEncrypt# getJNIEnv failed!");
		goto ERR;
	}

	jba_plainText = env->NewByteArray(flen);
	env->SetByteArrayRegion(jba_plainText,0,flen,(jbyte*)from);

	jba_cipherText = (jbyteArray)env->CallObjectMethod(g_obj,g_mid_X509PrivateEncrypt,jba_plainText);
	if (NULL == jba_cipherText)
	{
		LOGE("filecert_ex#PrivateEncrypt# CallObjectMethod failed!");
		goto ERR;
	}

	signLen = env->GetArrayLength(jba_cipherText);
	pTmp = env->GetByteArrayElements(jba_cipherText,0);
	memcpy(to, pTmp, signLen);
	env->ReleaseByteArrayElements(jba_cipherText, pTmp, 0);
	nRet = 0;

	LOGD("filecert_ex#PrivateEncrypt# cipher length: %u", signLen);
ERR:
	return nRet == 0 ? nKeyLength : 0;
}

// return nKeyLength on success,0 on failed
int PrivateDecrypt(int flen, const unsigned char *from, unsigned char *to, int nKeyLength, int padding)
{
	int nRet = 1, needsDetach, dataLen;
	jbyte* pTmp;
	jbyteArray jba_cipherText,jba_plainText;
	JNIEnv* env;

	if (NULL == g_obj || NULL == g_mid_X509PrivateDecrypt)
	{
		LOGE("filecert_ex#PrivateDecrypt# You need initialize Java Object,g_obj/g_mid_X509PrivateDecrypt is NULL");
		goto ERR;
	}

	env = getJNIEnv(&needsDetach);
	if (NULL == env)
	{
		LOGE("filecert_ex#PrivateDecrypt# getJNIEnv failed!");
		goto ERR;
	}

	jba_cipherText = env->NewByteArray(flen);
	env->SetByteArrayRegion(jba_cipherText,0,flen,(jbyte*)from);

	jba_plainText = (jbyteArray)env->CallObjectMethod(g_obj,g_mid_X509PrivateDecrypt,jba_cipherText);
	if (NULL == jba_plainText)
	{
		LOGE("filecert_ex#PrivateDecrypt# CallObjectMethod failed!");
		goto ERR;
	}

	dataLen = env->GetArrayLength(jba_plainText);
	pTmp = env->GetByteArrayElements(jba_plainText,0);
	memcpy(to, pTmp, dataLen);
	env->ReleaseByteArrayElements(jba_plainText, pTmp, 0);
	nRet = 0;

	LOGD("filecert_ex#PrivateDecrypt# PlainText length: %u", dataLen);
ERR:
	return nRet == 0 ? nKeyLength : 0;
}

void GetTFName(char* szName)
{
	strcpy(szName, "F-model-ex");
}
