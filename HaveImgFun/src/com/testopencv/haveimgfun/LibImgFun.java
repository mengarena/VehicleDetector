package com.testopencv.haveimgfun;

public class LibImgFun {
	static  {
		System.loadLibrary("opencv_java");
		System.loadLibrary("nonfree");		
		System.loadLibrary("ImgFun");
	}

//	public static native int[] ImgFun(int[] buf, int w, int h);

	public static native double ImgFun(int[] buf, int w, int h);
	public static native double VehicleDetection();
	public static native int[] ConvertToGray(int[] buf, int w, int h);
//	public static native void ConvertToGray(int[] buf, int w, int h, int[] graybuf);

}
