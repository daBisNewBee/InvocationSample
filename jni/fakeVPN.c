#include "MyInvocation.h"
#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>

#define CERT_LEN	4096
#define SIGNIN_LEN	64
#define SIGNOUT_LEN	256

typedef int (*FP_LOADCERT)(BYTE*, int*);
typedef int (*FP_CRYPT)(int, const BYTE*, BYTE*, int, int);
typedef int (*FP_DECRYPT)(int, const BYTE*, BYTE*, int, int);
typedef int (*FP_GETTFNAME)(char*);

typedef struct tagRSAMETHOD
{
	FP_LOADCERT			pLoadCert;
	FP_CRYPT			pPriEncrypt;
	FP_DECRYPT			pPriDecrypt;
	FP_GETTFNAME		pGetTFName;
}RSAMETHOD,*LPRSAMETHOD;

typedef struct tagRSATFINFO
{
	void*		hLib;
	RSAMETHOD	method;
} RSATFINFO, *LPRSATFINFO;

RSATFINFO		m_rti_ex;

// return 1 on success;0 on failed
int initialMethods()
{
	int bRet = 0;
	char *soPath = "/data/data/com.example.invo/lib/libfilecert_ex.so";

	memset(&m_rti_ex, 0, sizeof(m_rti_ex));
	m_rti_ex.hLib = dlopen(soPath, RTLD_LAZY);

	if (NULL != m_rti_ex.hLib)
	{
		m_rti_ex.method.pLoadCert     = (FP_LOADCERT)dlsym(m_rti_ex.hLib, "LoadCert");
		m_rti_ex.method.pPriEncrypt   = (FP_CRYPT)dlsym(m_rti_ex.hLib, "PrivateEncrypt");
		m_rti_ex.method.pPriDecrypt   = (FP_DECRYPT)dlsym(m_rti_ex.hLib, "PrivateDecrypt");
		m_rti_ex.method.pGetTFName    = (FP_GETTFNAME)dlsym(m_rti_ex.hLib, "GetTFName");
	}
	else
	{
		LOGE("fakeVPN#dlopen falied:%s error:%d reason:%s",soPath,errno,strerror(errno));
		return bRet;
	}

	 bRet = m_rti_ex.hLib &&
			m_rti_ex.method.pLoadCert &&
			m_rti_ex.method.pPriEncrypt &&
			m_rti_ex.method.pPriDecrypt &&
			m_rti_ex.method.pGetTFName;

	 return bRet;
}

/*
 * Class:     com_example_invo_ServiceCtrl
 * Method:    startFakeVPN
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL Java_com_example_invo_ServiceCtrl_startFakeVPN
  (JNIEnv *env, jobject object)
{

	 // 1.初始化获取方法句柄
	if (initialMethods())
	{
		LOGD("fakeVPN#initialMethods success!");
	}
	else
	{
		LOGE("fakeVPN#initialMethods failed!");
		return false;
	}

	// 2. 读取证书
	 BYTE szCert[CERT_LEN] = {0};
	 int nLen = CERT_LEN;

	 if(m_rti_ex.method.pLoadCert(szCert, &nLen) != 0)
	 {
		 LOGE("fakeVPN#LoadCert failed!");
		 return false;
	 }

	 LOGD("fakeVPN#LoadCert,size:%d", nLen);

	 // 3. 私钥加密
	 BYTE szSignIn[SIGNIN_LEN] = {0};
	 BYTE szSignOut[SIGNOUT_LEN] = {0};
	 BYTE szSignDeOut[SIGNOUT_LEN] = {0};
	 int nKeyLength = 128, padding = 0;
	 memset(szSignIn, 0x31, SIGNIN_LEN);

	 if(!m_rti_ex.method.pPriEncrypt(SIGNIN_LEN, szSignIn, szSignOut, nKeyLength, padding))
	 {
		 LOGE("fakeVPN#PriEncrypt failed!");
		 return false;
	 }

	 //4. 公钥解密
	 if(!m_rti_ex.method.pPriDecrypt(nKeyLength,szSignOut, szSignDeOut, nKeyLength, padding))
	 {
		 LOGE("fakeVPN#PriDecrypt failed!");
		 return false;
	 }

	 if (memcmp(szSignIn,szSignDeOut,SIGNIN_LEN))
	 {
		 LOGE("fakeVPN#PriDecrypt failed!");
		 return false;
	 }

	 LOGD("fakeVPN#PriDecrypt success:%s",szSignDeOut);

	 return true;
}
