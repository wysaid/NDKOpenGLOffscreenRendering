package org.wysaid.ndkopenglbackdraw;

import android.os.Bundle;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Bitmap;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;

public class MainActivity extends Activity {
	static
	{
		System.loadLibrary("NDKOpenGLBackDraw");
	}
	
	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		dispalyLayout = (LinearLayout) findViewById(R.id.displayLayout);
		displayView = (ImageView) findViewById(R.id.imageView1);
		button = (Button) findViewById(R.id.button1);
		button.setOnClickListener(clickListener);
		GLHelpFunctions.initEGL();
		bitmap = Bitmap.createBitmap(800, 800, Bitmap.Config.ARGB_8888);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	android.view.View.OnClickListener clickListener =
			new android.view.View.OnClickListener() {		
		@Override
		public void onClick(View view) {
			// TODO Auto-generated method stub
			GLHelpFunctions.enableEGL();
			GLHelpFunctions.getGLBackDrawImage(bitmap);
			displayView.setImageBitmap(bitmap);
		}
	};
	
	ImageView displayView;
	LinearLayout dispalyLayout;
	Button button;
	Bitmap bitmap;
}
