package com.example.invo;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.cert.CertificateEncodingException;
import java.security.cert.X509Certificate;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

import android.content.Context;
import android.security.KeyChain;
import android.security.KeyChainException;

public class ServiceCtrl {
	
	static{
		System.loadLibrary("MyInvocation");
		System.loadLibrary("filecert_ex");
		System.loadLibrary("fakeVPN");
	}
	
	private Context mCtx;
	private String mAlias;
	private IshowMsg mIshowMsg;
	
	public native void start();
	
	public native boolean startFakeVPN();
	
	public native void initJavaObject();
	
	public ServiceCtrl(Context ctx,String alias,IshowMsg showMsg)
	{
		this.mCtx = ctx;
		this.mAlias = alias;
		this.mIshowMsg = showMsg;
	}
	
    public void callback(String str)
    {
    	this.mIshowMsg.showMsg(str);
    }
	
	public byte[] LoadX509Certificate()
	{
		X509Certificate X509cert;
		byte[] certByte = null;
		try {
			X509cert = KeyChain.getCertificateChain(mCtx, mAlias)[0];
			certByte = X509cert.getEncoded();
		} catch (KeyChainException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (CertificateEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return certByte;
	}
	
	public byte[] X509PrivateEncrypt(byte[] plainText)
	{
		byte[] cipherText = null;
		try {
			PrivateKey privateKey = KeyChain.getPrivateKey(mCtx, mAlias);
			Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding",
					"AndroidOpenSSL");
			cipher.init(Cipher.ENCRYPT_MODE, privateKey);
			cipherText = cipher.doFinal(plainText);
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (KeyChainException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvalidKeyException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchPaddingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalBlockSizeException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (BadPaddingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return cipherText;
	}
	
	public byte[] X509PrivateDecrypt(byte[] cipherData)
	{
		byte[] plainText = null;
		try {
			X509Certificate X509cert = KeyChain.getCertificateChain(mCtx,
					mAlias)[0];
			PublicKey publicKey = X509cert.getPublicKey();
			Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding",
					"AndroidOpenSSL");
			cipher.init(Cipher.DECRYPT_MODE, publicKey);
			plainText = cipher.doFinal(cipherData);
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		} catch (NoSuchPaddingException e) {
			e.printStackTrace();
		} catch (InvalidKeyException e) {
			e.printStackTrace();
		} catch (IllegalBlockSizeException e) {
			e.printStackTrace();
		} catch (BadPaddingException e) {
			e.printStackTrace();
		} catch (KeyChainException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (NoSuchProviderException e) {
			e.printStackTrace();
		}
		return plainText;
	}  

}
