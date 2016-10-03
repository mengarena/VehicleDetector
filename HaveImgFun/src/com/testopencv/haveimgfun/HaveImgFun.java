package com.testopencv.haveimgfun; 
 
import android.app.Activity;  
import android.graphics.Bitmap;   
import android.graphics.Bitmap.Config;   
import android.graphics.drawable.BitmapDrawable;   
import android.os.Bundle;   
import android.widget.Button;  
import android.view.View;   
import android.widget.ImageView;
import android.content.Context;
import android.os.PowerManager;


public class HaveImgFun extends Activity{ 
/** Called when the activity is first created. */  
ImageView imgView;  
Button btnNDK, btnRestore, btnGray; 
protected PowerManager.WakeLock mWakeLock;

@Override  
public void onCreate(Bundle savedInstanceState) {  
    super.onCreate(savedInstanceState);   
     setContentView(R.layout.main);  
 
    this.setTitle("Use NDK Grayscale");   
    btnRestore=(Button)this.findViewById(R.id.btnRestore);   
    btnRestore.setOnClickListener(new ClickEvent()); 
    
    btnNDK=(Button)this.findViewById(R.id.btnNDK);   
    btnNDK.setOnClickListener(new ClickEvent());   

    btnGray=(Button)this.findViewById(R.id.btnGray);   
    btnGray.setOnClickListener(new ClickEvent());   

    imgView=(ImageView)this.findViewById(R.id.ImageView01);  
        Bitmap img=((BitmapDrawable) getResources().getDrawable(R.drawable.sc)).getBitmap();  
    imgView.setImageBitmap(img); 
    
    PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
    this.mWakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, "My Tag");
    this.mWakeLock.acquire();
} 

@Override
public void onDestroy() {
    this.mWakeLock.release();
    super.onDestroy();
}
 
class ClickEvent implements View.OnClickListener{  
    public void onClick(View v){ 
        if(v == btnNDK){
        	HaveImgFun.this.setTitle("Running");
            long current=System.currentTimeMillis();   
            Bitmap img1=((BitmapDrawable) getResources().getDrawable(R.drawable.sc)).getBitmap();   
            int w=img1.getWidth(),h=img1.getHeight();   
            int[] pix = new int[w * h];
            img1.getPixels(pix, 0, w, 0, 0, w, h);   
//            int[] resultInt=LibImgFun.ImgFun(pix, w, h);
          double resultInt=LibImgFun.ImgFun(pix, w, h);
//            Bitmap resultImg=Bitmap.createBitmap(w, h, Config.RGB_565);   
//            resultImg.setPixels(resultInt, 0, w, 0, 0,w, h);   
            long performance=System.currentTimeMillis()-current;   
//            imgView.setImageBitmap(resultImg);   
//            HaveImgFun.this.setTitle("w:"+String.valueOf(img1.getWidth())+",h:"+String.valueOf(img1.getHeight())+"NDK takes"+String.valueOf(performance)+" ms"); 
            HaveImgFun.this.setTitle("M:"+String.valueOf(resultInt)+",NDK:"+String.valueOf(performance)+" ms"); 

            System.out.println("Mark.......!\n");
        } else if(v == btnRestore){  
        	HaveImgFun.this.setTitle("Running");
                //Bitmap img2=((BitmapDrawable) getResources().getDrawable(R.drawable.sc)).getBitmap();  
                //imgView.setImageBitmap(img2);  
                //HaveImgFun.this.setTitle("Use OpenCV Process");
                //System.out.println("Restore...........!\n");
        		double dTime = LibImgFun.VehicleDetection();
                HaveImgFun.this.setTitle("Time:" + String.valueOf(dTime) +" ms");
        } else if(v == btnGray) {
        	HaveImgFun.this.setTitle("Running");
            long current=System.currentTimeMillis();   
            Bitmap img2=((BitmapDrawable) getResources().getDrawable(R.drawable.sc)).getBitmap();   
            int w=img2.getWidth(),h=img2.getHeight();   
            int[] pix = new int[w * h];
           // int[] resultInt = new int[w * h];
            img2.getPixels(pix, 0, w, 0, 0, w, h);   
            int[] resultInt = LibImgFun.ConvertToGray(pix, w, h);
           
            Bitmap resultImg=Bitmap.createBitmap(w, h, Config.ARGB_8888);   
            resultImg.setPixels(resultInt, 0, w, 0, 0, w, h); 
            long performance=System.currentTimeMillis()-current;   
            imgView.setImageBitmap(resultImg);   
            HaveImgFun.this.setTitle("w:"+String.valueOf(img2.getWidth())+",h:"+String.valueOf(img2.getHeight())+"NDK takes"+String.valueOf(performance)+" ms"); 
        	System.out.println("Gray.......!\n");
        }
     }
   } 
}

