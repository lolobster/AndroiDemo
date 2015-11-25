/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package fi.lobster;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;

/**
 * This class loads the Java Native Interface (JNI)
 * library, 'libAndroidLobster.so', and provides access to the
 * exposed C functions.
 * The library is packaged and installed with the application.
 * See the C file, /jni/AndroidLobster.c file for the
 * implementations of the native methods. 
 * 
 * For more information on JNI, see: http://java.sun.com/docs/books/jni/
 */

public class AndroidLobster extends Activity
{

private TextView tv;
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// Enter code here
		 tv = new TextView(this);
        tv.setText( "Starting..." );
        setContentView(tv);
		
		Thread updateThread = new Thread(new Runnable() {
			public void run() {
				while (true) {
					tv.post(new Runnable() {
						public void run() {
							tv.setText( headerText() );
						}
					});
					try { Thread.sleep(1000); } catch (Exception e) { }
					tv.post(new Runnable() {
						public void run() {
							tv.setText("");
						}
					});
					try { Thread.sleep(1000); } catch (Exception e) { }
				}
			}
		});
		updateThread.start();
	}

	/**
	 * An example native method.  See the library function,
	 * <code>Java_fi_lobster_AndroidLobster_androidlobsterNative</code>
	 * for the implementation.
	 */
	public native String headerText();
	public native void lobsterSquare();

	/* This is the static constructor used to load the
	 * 'AndroidLobster' library when the class is
	 * loaded.
	 */
	static {
		System.loadLibrary("AndroidLobster");
	}
}
