package com.example.invo;

import android.app.Activity;
import android.os.Bundle;
import android.security.KeyChain;
import android.security.KeyChainAliasCallback;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener,IshowMsg{
	
	private TextView txtLog = null;
	private ScrollView sclView = null;
	private String mAlias = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		findViewById(R.id.btnInvokeFromJava).setOnClickListener(this);
		findViewById(R.id.btnInvokeFromC).setOnClickListener(this);
		findViewById(R.id.btnChooseCert).setOnClickListener(this);
		
		txtLog = (TextView)findViewById(R.id.txtLog);
		sclView = (ScrollView)findViewById(R.id.sclView);
		
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId())
		{
			case R.id.btnInvokeFromJava:
					ServiceCtrl mServiceCtrl = new ServiceCtrl(MainActivity.this,mAlias,MainActivity.this);
					mServiceCtrl.start();
				break;
			case R.id.btnChooseCert:
				KeyChain.choosePrivateKeyAlias(MainActivity.this,
						new KeyChainAliasCallback() {
							public void alias(String alias) {
								// Credential alias selected. Remember the alias
								// selection for future use.
								mAlias = new String(alias);
								AppendLog("choose cert which alias is:"+alias);
								ServiceCtrl mServiceCtrl = new ServiceCtrl(MainActivity.this,mAlias,MainActivity.this);
								mServiceCtrl.initJavaObject();
							}
						}, new String[] { "RSA" }, // List of acceptable key
													// types. null for any
						null, // issuer, null for any
						MainActivity.this.getPackageName(), // host name of server requesting
												// the cert, null if unavailable
						-1, // port of server requesting the cert, -1 if
							// unavailable
						"lwb2");// 打开对话框时默认选中的值，若存在的话
				
				break;
			case R.id.btnInvokeFromC:
				new Thread(){
					public void run()
					{
						ServiceCtrl mmServiceCtrl = new ServiceCtrl(MainActivity.this,mAlias,MainActivity.this);
						if(mmServiceCtrl.startFakeVPN())
							AppendLog("Invoke C success!");
						else
							AppendLog("Invoke C failed!");
							
					}
				}.start();
				break;
			default:
				break;
		}
	}
	
	// can be called from child-thread
	void AppendLog(String strLog)
	{
		final String msg = new String(strLog);
		this.runOnUiThread(new Runnable() {
			public void run() {
				txtLog.append(msg + "\n");
				sclView.post(new Runnable() {
					@Override
					public void run() {
						sclView.fullScroll(ScrollView.FOCUS_DOWN);
					}
				});
			}
		});
	}

	@Override
	public void showMsg(String msg) {
		// TODO Auto-generated method stub
		AppendLog(msg);
	}
}
