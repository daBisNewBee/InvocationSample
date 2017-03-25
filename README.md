# 2017-03-25更新
- 在JNI中反向调用android.app.ActivityThread中的currentApplication，可以获取Context对象。免去提前由Java初始化对象至native中的过程。
- 参考：[Android安全系列之：如何在native层保存关键信息](http://www.jianshu.com/p/2576d064baf1)

# InvocationSample
描述了C调用Java的基本用法，包括Java发起的和C发起的调用。其中，解决了C中创建Java对象时，无法获取Android中Conetxt衍生对象（Activity、Service）导致无法创建Java对象的问题。

# Design
![InvocationSample](https://github.com/daBisNewBee/InvocationSample/blob/master/pic/InvocationSample.png)

# Usage
- 点击“Java invoke”，测试Java发起的C反向调用Java情况：

 > This String is from JNI!，表示从Native空间传来并显示在Java空间的字符串
 
- 点击“Choose KeyStore and Initial Java Object”，查看已经安装到系统的证书信息，选择其中一个后并“允许”，APP将使用选择的证书信息来初始化Java Object：

 > choose cert which alias is:alias_name，表示已经选择alias_name证书来做下面的操作

- 点击“C invoke”，测试C发起的C反向调用Java情况：

 >Invoke C success! 表示C发起的C反向调用Java成功；
 Invoke C failed! 表示C发起的C反向调用Java失败；
 
- 后台查看日志：
  ```
C:\Users\>adb logcat -s inv
--------- beginning of /dev/log/main
--------- beginning of /dev/log/system
D/inv     (14839): MyInvocation#JNI_OnLoad# Get JavaVM success
D/inv     (14839): MyInvocation#initJavaObject success
D/inv     (14839): fakeVPN#initialMethods success!
D/inv     (14839): filecert_ex#LoadCert# cert length:1086
D/inv     (14839): fakeVPN#LoadCert,size:1086
D/inv     (14839): filecert_ex#PrivateEncrypt# cipher length: 128
D/inv     (14839): filecert_ex#PrivateDecrypt# PlainText length: 64
D/inv     (14839): fakeVPN#PriDecrypt success:1111111111111111111111111111111111111111111111111111111111111111
  ```
 
- 这里，使用证书非对称加解密（私钥加密、公钥解密）操作作为业务。“android.security.KeyChain”作为Android API提供了系统级别的证书信息（公钥）、私钥句柄的接口。
上述日志表示了C使用Java中KeyChain方法，校验解密后的数据与原文一致，表示C反向调用Java成功。

- UI显示如下： 
![operation](https://github.com/daBisNewBee/InvocationSample/blob/master/pic/operation.png)
