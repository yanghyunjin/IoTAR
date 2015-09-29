package com.samsung.iot;

import com.philips.lighting.hue.sdk.PHAccessPoint;
import com.philips.lighting.hue.sdk.PHBridgeSearchManager;
import com.philips.lighting.hue.sdk.PHHueSDK;
import com.philips.lighting.hue.sdk.PHSDKListener;
import com.philips.lighting.model.PHBridge;
import com.philips.lighting.model.PHHueError;
import com.philips.lighting.model.PHHueParsingError;
import com.qualcomm.QCARUnityPlayer.DebugLog;
import com.unity3d.player.*;
import android.app.NativeActivity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLES20;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;


import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.io.File;
import java.io.FileOutputStream;
import java.nio.IntBuffer;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
public class UnityPlayerNativeActivity extends NativeActivity implements OcPlatform.OnResourceFoundListener
{
	private static final String TAG = "UnityPlayerActivity: ";
	private PHHueSDK phHueSDK;
	private AccessPointListAdapter adapter;
	private boolean flag = false; // resultVeiwflag
	private boolean groupFlag = false; //groupView Flag
	private boolean unityStart=false;

	public static Light            myLight;
	public static HueGroup         hueGroup;
	protected UnityPlayer mUnityPlayer; // don't change the name of this variable; referenced from native code
	private OcResource curResource;
	public static OcResourceHandle resourceHandle;

	int volume, state, track, songNum, volumeNum;
	String lyrics1, lyrics2, titles, singers;
	String getResult;
	private Doorlock doorlock;

	public HueConnectThread hueConnectThread;

	public void initOICStack() {
		//create platform config
		Log.e(TAG, "initOICStack Start!");
		PlatformConfig cfg = new PlatformConfig(
				this,
				ServiceType.IN_PROC,
				ModeType.CLIENT_SERVER,
				"0.0.0.0", // bind to all available interfaces
				0,
				QualityOfService.LOW);
		OcPlatform.Configure(cfg);

	}

	public void findServer(){
		try {
			/**
			 * find all resources
			 */
			OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
					OcConnectivityType.CT_DEFAULT, this);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}

	}


	synchronized public void onResourceFound(OcResource ocResource) {
		/**
		 *  this may potentially be called by multiple threads at the same time
		 */
		synchronized (this) {
			String resourceUri;
			String hostAddress;
			resourceUri = ocResource.getUri();
			hostAddress = ocResource.getHost();

			Log.v(TAG, "resourceUri :" +resourceUri);
// get the resource types
			if (resourceUri.equals("/iotar/server")) {
				Log.v(TAG, "/iotar/hue gogo");
				curResource = ocResource;
				doGetLightRepresentation();
			}
		}
	}

	private void doGetDoorRepresentation() {
		// eventhandler for get()
		OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
			@Override
			public void onGetCompleted(List<OcHeaderOption> headerOptionList,
									   OcRepresentation ocRepresentation) {
				Log.e(TAG, "GET resource was successful ");
				try {
					String key = ocRepresentation.getValue("doorlock_key");
					doorlock.setKey(key);
					UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetDoorlock", doorlock.getKey());
				} catch (OcException e) {
					Log.e(TAG, e.getMessage());
				}
				Log.e(TAG, "onGetCompleted : " + doorlock.getKey());
			}

			@Override
			public void onGetFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
					//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};


		if(curResource == null){

			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}


		try {
			curResource.get(new HashMap<String, String>(), onGetListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
	}

	private void doPutDoorRepresentation(int order) {
		OcResource.OnPutListener onPutListener = new OcResource.OnPutListener() {
			@Override
			public void onPutCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
				Log.e(TAG, "PUT resource was successful ");
				try {
					doorlock.setKey(ocRepresentation.getValue("doorlock_key").toString());
					UnityPlayer.UnitySendMessage("A ndroidPluginManager", "SetDoorlock", doorlock.getKey());
				} catch(OcException e){
					Log.e(TAG, e.getMessage());
				}
			}

			@Override
			public void onPutFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
					//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};


		if(curResource == null){

			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}



		try {
			OcRepresentation rep = new OcRepresentation();
			if(order==40) {
				rep.setValue("order", 40);
				rep.setValue("doorlock_key", doorlock.getKey());
			} else
				rep.setValue("order", 41);
			curResource.put(rep, new HashMap<String, String>(), onPutListener);
		} catch(Exception e) {
			Log.e(TAG, e.getMessage());
		}

	}


	public void CallAndroid_Doorlock_Get() {
		Log.v("UnityPlayerActivity", "CallAndroid_Doorlock_Get() Start");
		doGetDoorRepresentation();
	}

	public void CallAndroid_Doorlock_Set_Put(String newCode) {
		Log.v("UnityPlayerActivity", "CallAndroid_Doorlock_Set_Put() Start");
		doorlock.setKey(newCode);
		doPutDoorRepresentation(40);
	}

	public void CallAndroid_Doorlock_Open_Put() {
		Log.v("UnityPlayerActivity", "CallAndroid_Doorlock_Open_Put() Start");
		doPutDoorRepresentation(41);
	}



	private void doGetSpeakerRepresentation() {
		// eventhandler for get()
		OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
			@Override
			public void onGetCompleted(List<OcHeaderOption> headerOptionList,
									   OcRepresentation ocRepresentation) {
				Log.e(TAG, "GET resource was successful ");
				try {
					volume = ocRepresentation.getValue("speaker_volume");
					volume /= 2;
					state = ocRepresentation.getValue("speaker_state");
					if(ocRepresentation.hasAttribute("speaker_lyrics1"))
						lyrics1 = ocRepresentation.getValue("speaker_lyrics1");
					else lyrics1 = " ";
					if(ocRepresentation.hasAttribute("speaker_lyrics2"))
						lyrics2 = ocRepresentation.getValue("speaker_lyrics2");
					else lyrics2 = " ";
					track = ocRepresentation.getValue("speaker_track");
					titles = ocRepresentation.getValue("speaker_titles");
					singers = ocRepresentation.getValue("speaker_singers");
					getResult = volume + "@" + state + "@"   + lyrics1 + "\n"  + lyrics2 + "@" + track + "@" + titles + "@" + singers;
					Log.e(TAG, "getResult : " + getResult);

					UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetGet", getResult);
				} catch (OcException e) {
					Log.e(TAG, e.getMessage());
				}
				Log.e(TAG, "onGetCompleted");
			}

			@Override
			public void onGetFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
					//do something based on errorCode
				}
				Log.e(TAG, "onGetFailed : "   + throwable.toString());
			}
		};
		if(curResource == null){

			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}
		try {
			curResource.get(new HashMap<String, String>(), onGetListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
	}


	private void doPutSpeakerRepresentation(int order) {
		OcResource.OnPutListener onPutListener = new OcResource.OnPutListener() {
			@Override
			public void onPutCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
				Log.e(TAG, "PUT resource was successful ");
			}

			@Override
			public void onPutFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
					//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};

		if(curResource == null){

			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}

		try {
			OcRepresentation rep = new OcRepresentation();

			if(order==30) {
				rep.setValue("order", 30);
				rep.setValue("temp", songNum);
			} else if(order==31){
				rep.setValue("order", 31);
				rep.setValue("temp", volumeNum);
			} else {
				rep.setValue("order", 37);
			}
			curResource.put(rep, new HashMap<String, String>(), onPutListener);
		} catch(Exception e) {
			Log.e(TAG, e.getMessage());
		}

	}

	public void CallAndroid_Hue_Get() {
		Log.v("UnityPlayerActivity", "CallAndroid_Hue_Get() Start");
		doGetLightRepresentation();
	}


	public void CallAndroid_Speaker_Get() {
		Log.v("UnityPlayerActivity", "CallAndroid_Speaker_Get() Start");
		doGetSpeakerRepresentation();
	}

	public void CallAndroid_Speaker_Put(int s) {
		Log.v("UnityPlayerActivity", "CallAndroid_Speaker_Put() Start");
		songNum = s;
		doPutSpeakerRepresentation(30);
		// UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetLog", String.valueOf(songNum));
	}

	public void CallAndroid_Speaker_Play_Put() {
		Log.v("UnityPlayerActivity", "CallAndroid_Speaker_Play_Put() Start");
		doPutSpeakerRepresentation(37);
		// UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetLog", String.valueOf(songNum));
	}

	public void CallAndroid_Speaker_Volume_Put(int v) {
		Log.v("UnityPlayerActivity", "CallAndroid_Speaker_Volume_Put() Start");
		volumeNum = v*2;
		doPutSpeakerRepresentation(31);
		// UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetLog", String.valueOf(songNum));
	}

	// Setup activity layout
	@Override protected void onCreate (Bundle savedInstanceState)
	{
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		super.onCreate(savedInstanceState);

		getWindow().takeSurface(null);
		getWindow().setFormat(PixelFormat.RGBX_8888); // <--- This makes xperia play happy
		OcRepresentation rep = new OcRepresentation();
		rep.setValueBool("test", false);
		boolean result = rep.getValueBool("test");

		initOICStack();


		doorlock = new Doorlock();
		hueGroup = HueGroup.getInstance();
		myLight = new Light();

		// Gets an instance of the Hue SDK.
		phHueSDK = PHHueSDK.getInstance();

// Set the Device Name (name of your app). This will be stored in your
// bridge whitelist entry.
		phHueSDK.setAppName("QuickStartApp");
		phHueSDK.setDeviceName(android.os.Build.MODEL);

// Register the PHSDKListener to receive callbacks from the bridge.
		phHueSDK.getNotificationManager().registerSDKListener(listener);

		adapter = new AccessPointListAdapter(this,
				phHueSDK.getAccessPointsFound());


		hueConnectThread = new HueConnectThread();
		hueConnectThread.start();

		unityStart=true;


		mUnityPlayer = new UnityPlayer(this);
		if (mUnityPlayer.getSettings().getBoolean("hide_status_bar", true))
		{
			getWindow ().setFlags (WindowManager.LayoutParams.FLAG_FULLSCREEN,
					WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}

		setContentView(mUnityPlayer);
		mUnityPlayer.requestFocus();

		Log.e(TAG, "startstart");

	}


	class HueConnectThread extends Thread{

		public void run(){

			Log.w(TAG, "Searching for Bridges.");


			doBridgeSearch();
			resultView();


		}



	}

	public void requestLinkHue(){

		if(myLight.getFirstaccess()) {
			hueConnectThread.start();
		}

		findServer();

	}


	// Quit Unity
	@Override protected void onDestroy ()
	{
		mUnityPlayer.quit();
		super.onDestroy();
	}

	// Pause Unity
	@Override protected void onPause()
	{
		super.onPause();
		mUnityPlayer.pause();
	}

	// Resume Unity
	@Override protected void onResume()
	{
		super.onResume();
		mUnityPlayer.resume();
	}

	// This ensures the layout will be correct.
	@Override public void onConfigurationChanged(Configuration newConfig)
	{
		super.onConfigurationChanged(newConfig);
		mUnityPlayer.configurationChanged(newConfig);
	}

	// Notify Unity of the focus change.
	@Override public void onWindowFocusChanged(boolean hasFocus)
	{
		super.onWindowFocusChanged(hasFocus);
		mUnityPlayer.windowFocusChanged(hasFocus);
	}

	// For some reason the multiple keyevent type is not supported by the ndk.
	// Force event injection by overriding dispatchKeyEvent().
	@Override public boolean dispatchKeyEvent(KeyEvent event)
	{
		if (event.getAction() == KeyEvent.ACTION_MULTIPLE)
			return mUnityPlayer.injectEvent(event);
		return super.dispatchKeyEvent(event);
	}

	// Pass any events not handled by (unfocused) views straight to UnityPlayer
	@Override public boolean onKeyUp(int keyCode, KeyEvent event)     { return mUnityPlayer.injectEvent(event); }
	@Override public boolean onKeyDown(int keyCode, KeyEvent event)   { return mUnityPlayer.injectEvent(event); }
	@Override public boolean onTouchEvent(MotionEvent event)          { return mUnityPlayer.injectEvent(event); }
	/*API12*/ public boolean onGenericMotionEvent(MotionEvent event)  { return mUnityPlayer.injectEvent(event); }

	public void CallAndroid_Hue_Color(String strMsg)
	{
		String strSendMsg = strMsg + " Hello Unity3D";
		Log.v("IoTAR", strMsg);
		String[] Msg = new String[4];
		Msg = strMsg.split("/");

		int target;
		target= Integer.parseInt(Msg[0]);
		double red,green,blue;

		red = Float.parseFloat(Msg[1]);
		green = Float.parseFloat(Msg[2]);
		blue = Float.parseFloat(Msg[3]);

		red = (red > 0.04045f) ? Math.pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
		green = (green > 0.04045f) ? Math.pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
		blue = (blue > 0.04045f) ? Math.pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

		double X = red * 0.664511f + green * 0.154324f + blue * 0.162028f;
		double Y = red * 0.283881f + green * 0.668433f + blue * 0.047685f;
		double Z = red * 0.000088f + green * 0.072310f + blue * 0.986039f;

		double x = X / (X + Y + Z);
		double y = Y / (X + Y + Z);

		if(hueGroup.hueGroupList.contains("hue"+String.valueOf(target))) {
			for(int i=0;i<hueGroup.hueGroupList.size();i++) {
				String str = hueGroup.hueGroupList.get(i);
				int index = Integer.parseInt(str.substring(3));

				setLight(index,myLight.getPower(index),myLight.getBri(index),x,y);
			}
		} else {
			setLight(target,myLight.getPower(target),myLight.getBri(target),x,y);
		}

		doPutLightRepresentation(1);

		UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetLog", strSendMsg);
	}
	public void CallAndroid_Hue_State(String strMsg)
	{

		Log.v("IoTAR", strMsg);
		String[] Msg = new String[2];
		Msg = strMsg.split("/");
		int target;
		target=Integer.parseInt(Msg[0]);

		if(hueGroup.hueGroupList.contains("hue"+String.valueOf(target))) {
			for(int i=0;i<hueGroup.hueGroupList.size();i++) {
				String str = hueGroup.hueGroupList.get(i);
				int index = Integer.parseInt(str.substring(3));
				Log.v(TAG, "for index : "+String.valueOf(index));
				setLight(index, Msg[1], myLight.getBri(index), myLight.getColor_x(index), myLight.getColor_y(index));
			}
		}
		else {
			setLight(target, Msg[1], myLight.getBri(target), myLight.getColor_x(target), myLight.getColor_y(target));
		}
		doPutLightRepresentation(1);

	}
	public void CallAndroid_Hue_Group(String strMsg)
	{
		Log.v("IoTAR", strMsg);
		String[] Msg = new String[2];
		Msg = strMsg.split("/");
		int target;
		boolean state;
		target=Integer.parseInt(Msg[0]);
		state = Boolean.parseBoolean(Msg[1]);
		hueGroup.setGroup("hue"+String.valueOf(target), state);
		Log.v(TAG, "Size : " + String.valueOf(hueGroup.hueGroupList.size()));
		for(int i=0;i<hueGroup.hueGroupList.size();i++) {
			Log.v(TAG, String.valueOf(i)+" : "+hueGroup.hueGroupList.get(i));

		}

		doPutLightRepresentation(1);

	}
	public void CallAndroid_Hue_Bri(String strMsg)
	{

		Log.v("IoTAR", strMsg);
		String[] Msg = new String[2];
		Msg = strMsg.split("/");
		int target;
		target=Integer.parseInt(Msg[0]);
		if(hueGroup.hueGroupList.contains("hue"+String.valueOf(target))) {
			for(int i=0;i<hueGroup.hueGroupList.size();i++) {
				String str = hueGroup.hueGroupList.get(i);
				int index = Integer.parseInt(str.substring(3));
				Log.v(TAG, "for index : "+String.valueOf(index));
				setLight(index, myLight.getPower(index), Integer.parseInt(Msg[1]), myLight.getColor_x(index), myLight.getColor_y(index));
			}
		}
		else {
			setLight(target, myLight.getPower(target), Integer.parseInt(Msg[1]), myLight.getColor_x(target), myLight.getColor_y(target));
		}

		doPutLightRepresentation(1);

	}


	private PHSDKListener listener;

	{
		listener = new PHSDKListener() {

			@Override
			public void onAccessPointsFound(
					List<PHAccessPoint> accessPoint) {
				Log.w(TAG, "Access Points Found. "
						+ accessPoint.size());
				if (accessPoint != null
						&& accessPoint.size() > 0) {
					phHueSDK.getAccessPointsFound()
							.clear();
					phHueSDK.getAccessPointsFound()
							.addAll(accessPoint);
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							adapter.updateData(phHueSDK
									.getAccessPointsFound());
						}
					});
					PHAccessPoint accessPoint2 = (PHAccessPoint) adapter
							.getItem(0);
					accessPoint2.setUsername("VgB67nPMqdHJOPXh");

					Log.e(TAG, accessPoint2.getIpAddress() + " : " + accessPoint2.getUsername());

					myLight.setIp(accessPoint2.getIpAddress());
					myLight.setUsername(accessPoint2.getUsername());

					PHBridge connectedBridge = PHHueSDK.getInstance().getSelectedBridge();


					if (connectedBridge != null) {
						String connectedIP = connectedBridge
								.getResourceCache()
								.getBridgeConfiguration()
								.getIpAddress();
						Log.w(TAG, "Connected IP: "
								+ connectedIP);


						if (connectedIP != null) {
							Log.w(TAG,
									"Connected IP != null");
							phHueSDK.disableHeartbeat(connectedBridge);
							phHueSDK.disconnect(connectedBridge);
						}
					}
					phHueSDK.connect(accessPoint2);
				}
			}


			@Override
			public void onCacheUpdated(List<Integer> arg0, PHBridge bridge) {
				Log.w(TAG, "On CacheUpdated.");
			}

			@Override
			public void onBridgeConnected(PHBridge b, String s) {
				Log.w(TAG,
						"Connected to the Bridge. " + s);
				phHueSDK.setSelectedBridge(b);
				phHueSDK.enableHeartbeat(b,
						PHHueSDK.HB_INTERVAL);
				phHueSDK.getLastHeartbeat()
						.put(b.getResourceCache()
										.getBridgeConfiguration()
										.getIpAddress(),
								System.currentTimeMillis());
				findServer();



			}


			@Override
			public void onAuthenticationRequired(
					PHAccessPoint accessPoint) {
				Log.w(TAG,
						"Authentication Required.");
				phHueSDK.startPushlinkAuthentication(accessPoint);
				Log.w(TAG, "Access Point IP addr: "
						+ accessPoint.getIpAddress());
				Log.w(TAG,
						"Access Point MAC addr: "
								+ accessPoint
								.getMacAddress());
				Log.w(TAG, "Access Point key: "
						+ accessPoint.getUsername());
			}

			@Override
			public void onConnectionResumed(
					PHBridge bridge) {
// Log.v(TAG, "onConnectionResumed "
// +
// bridge.getResourceCache().getBridgeConfiguration().getIpAddress());
				phHueSDK.getLastHeartbeat()
						.put(bridge
										.getResourceCache()
										.getBridgeConfiguration()
										.getIpAddress(),
								System.currentTimeMillis());
				for (int i = 0; i < phHueSDK
						.getDisconnectedAccessPoint()
						.size(); i++) {
					if (phHueSDK
							.getDisconnectedAccessPoint()
							.get(i)
							.getIpAddress()
							.equals(bridge
									.getResourceCache()
									.getBridgeConfiguration()
									.getIpAddress())) {
						phHueSDK.getDisconnectedAccessPoint()
								.remove(i);
					}
				}
			}

			@Override
			public void onConnectionLost(
					PHAccessPoint accessPoint) {
				Log.v(TAG, "onConnectionLost : "
						+ accessPoint.getIpAddress());
				if (!phHueSDK
						.getDisconnectedAccessPoint()
						.contains(accessPoint)) {
					phHueSDK.getDisconnectedAccessPoint()
							.add(accessPoint);
				}
			}

			@Override
			public void onError(int code,
								final String message) {
				Log.e(TAG, "on Error Called : "
						+ code + ":" + message);

				if (code == PHHueError.NO_CONNECTION) {
					Log.w(TAG, "On No Connection");
				} else if (code == PHHueError.AUTHENTICATION_FAILED
						|| code == 1158) {

				} else if (code == PHHueError.BRIDGE_NOT_RESPONDING) {
					Log.w(TAG,
							"Bridge Not Responding . . . ");
				}
			}

			@Override
			public void onParsingErrors(
					List<PHHueParsingError> parsingErrorsList) {
				for (PHHueParsingError parsingError : parsingErrorsList) {
					Log.e(TAG,
							"ParsingError : "
									+ parsingError
									.getMessage());
				}
			}
		};
	}

	private void doPostLightRepresentation() {
		OcResource.OnPostListener onPostListener = new OcResource.OnPostListener() {
			@Override
			public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
				Log.e(TAG, "Post resource was successful ");
				doGetLightRepresentation();
			}

			@Override
			public void onPostFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};
		Log.d(TAG, "MyLight Post START");
		OcRepresentation rep = new OcRepresentation();

		try {
			rep.setValue("hf1", hueGroup.hf1);
			rep.setValue("hf2", hueGroup.hf2);
			rep.setValue("hf3", hueGroup.hf3);

		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
		try {
			Log.d(TAG, "before calling post");
			curResource.post(rep, new HashMap<String, String>(), onPostListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
		Log.d(TAG, "end of put call");
	}

	private void doDeletetLightRepresentation() {
		OcResource.OnDeleteListener onDeleteListener = new OcResource.OnDeleteListener() {

			@Override
			public void onDeleteCompleted(List<OcHeaderOption> list) {
				Log.e(TAG, "Delete resource was successful ");
			}

			@Override
			public void onDeleteFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};
		Log.d(TAG, "MyLight Delete START");
		try {
			Log.d(TAG, "before calling delete");
			curResource.deleteResource(onDeleteListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
		Log.d(TAG, "end of put call");
	}

	private void doPutLightRepresentation(int num) {
// eventhandler for put()
		OcResource.OnPutListener onPutListener = new OcResource.OnPutListener() {
			@Override
			public void onPutCompleted(List<OcHeaderOption> ocHeaderOptions,
									   OcRepresentation ocRepresentation) {
				Log.e(TAG, "PUT resource was successful");
				try {
					String strSetHue = "";
					for(int i=1; i<=3; ++i) {
						int bri;
						double x,y;
						String power;

						if(myLight.getFirstaccess()) {
							Log.e(TAG, "group end ");
							myLight.setFirstaccess(false);
						}


						bri = Integer.parseInt(ocRepresentation.getValue("hue" + i + "_brightness").toString());
						x = Double.parseDouble(ocRepresentation.getValue("hue" + i + "_color_x").toString());
						y = Double.parseDouble(ocRepresentation.getValue("hue" + i + "_color_y").toString());
						power = ocRepresentation.getValue("hue"+i+"_power").toString();
						myLight.setBri(bri, i);
						myLight.setColor(x, y, i);
						myLight.setPower(power, i);
						strSetHue += i+"/"+myLight.getPower(i)+"/"+myLight.getColor_x(i)+"/"+myLight.getColor_y(i)+"/"+myLight.getBri(i)+"/";
						Log.v(TAG,"receive"+strSetHue);
					}
					UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetHue", strSetHue);
				} catch (OcException e) {
					Log.e(TAG, e.getMessage());
				}
				flag= true;
				Log.e(TAG, "onPutCompleted\n<Brightness> Light 1 - " + myLight.getBri(1) +
						" Light 2 - " + myLight.getBri(2) + " Light 3 - " + myLight.getBri(3) +
						"\n<Power> Light 1 - " + myLight.getPower(1) + " Light 2 - " + myLight.getPower(2) +
						" Light 3 - " + myLight.getPower(3) + "\n<Color> Light 1 - " + myLight.getColor_x(1) +
						" Light 2 - " + myLight.getColor_x(2) + " Light 3 - " + myLight.getColor_x(3));
				if(unityStart) {
					String strSetHue = "";
					for (int i = 1; i <= 3; ++i) {
						strSetHue += String.valueOf(i) + "/" + myLight.getPower(i) + "/" + myLight.getColor_x(i) + "/" + myLight.getColor_y(i) + "/" + myLight.getBri(i) + "/";
					}
					Log.e(TAG,strSetHue);
					UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetHue", strSetHue);
				}

			}

			@Override
			public void onPutFailed(Throwable throwable) {

				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};
		Log.d(TAG, "MyLight Put START");



		if(curResource == null){
			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}

		if(myLight.getIp() == null){

			return;
		}

		OcRepresentation rep = new OcRepresentation();

		try {

			if(myLight.getFirstaccess() ) {
				Log.e(TAG, "FirstAccess() ip :" + myLight.getIp() + ", username : " + myLight.getUsername());
				rep.setValue("order", 11);
				rep.setValue("hue_ip", myLight.getIp());
				rep.setValue("hue_username", myLight.getUsername());
			} else {
				rep.setValue("order", 10);
				for(int i=1; i<=3; ++i) {
					rep.setValue("hue"+i+"_brightness", myLight.getBri(i));
					rep.setValue("hue"+i+"_color_x", myLight.getColor_x(i));
					rep.setValue("hue"+i+"_color_y", myLight.getColor_y(i));
					rep.setValue("hue"+i+"_power", myLight.getPower(i));
				}
			}

			rep.setValue("firstaccess", myLight.getFirstaccess());

			rep.setValue("hue1_group", hueGroup.hf1 );
			rep.setValue("hue2_group", hueGroup.hf2 );
			rep.setValue("hue3_group", hueGroup.hf3 );

			Log.v(TAG, "HUE gropu bool test : h1 = " + hueGroup.hf1 + "   h2 = " + hueGroup.hf2 + "    h3 = " + hueGroup.hf3 );

		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
		try {
			Log.d(TAG, "before calling put");
			curResource.put(rep, new HashMap<String, String>(), onPutListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
		Log.d(TAG, "end of put call");
	}

	void setLight(int n, String p, int b, double x, double y) {
		myLight.setPower(p, n);
		myLight.setBri(b, n);
		myLight.setColor(x, y, n);
	}

	private void doGetLightRepresentation() {
// eventhandler for get()
		OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
			@Override
			public void onGetCompleted(List<OcHeaderOption> headerOptionList,
									   OcRepresentation ocRepresentation) {
				Log.e(TAG, "GET resource was successful ");

				try {
					int bri;
					double x,y;
					String power;


					for(int i=1; i<=3; ++i) {
						power = ocRepresentation.getValue("hue" + i + "_power");
						Log.e(TAG, "power :    " + power);
						myLight.setPower(power, i);
					}


					UnityPlayer.UnitySendMessage("AndroidPluginManager", "SetHue", myLight.getPower(1)+ "/"+myLight.getPower(2)+"/"+myLight.getPower(3));

					flag= true;
					Log.e(TAG, "firstAcess???????????" + myLight.getFirstaccess());
					Log.e(TAG, "onGetCompleted\n<Brightness> Light 1 - " + myLight.getBri(1) +
							" Light 2 - " + myLight.getBri(2) + " Light 3 - " + myLight.getBri(3) +
							"\n<Power> Light 1 - " + myLight.getPower(1) + " Light 2 - " + myLight.getPower(2) +
							" Light 3 - " + myLight.getPower(3) + "\n<Color> Light 1 - " + myLight.getColor_x(1) +
							" Light 2 - " + myLight.getColor_x(2) + " Light 3 - "  + myLight.getColor_x(3));

				} catch (OcException e) {
					Log.e(TAG, e.getMessage());
				}

			}

			@Override
			public void onGetFailed(Throwable throwable) {
				if (throwable instanceof OcException) {
					OcException ocEx = (OcException) throwable;
					ErrorCode errCode = ocEx.getErrorCode();
//do something based on errorCode
				}
				Log.e(TAG, throwable.toString());
			}
		};

		if(curResource == null){

			try {
				OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
						OcConnectivityType.CT_DEFAULT, this);
			} catch (OcException e) {
				e.printStackTrace();
			}

		}

		if(myLight.getIp() == null){

			return;
		}

		try {
			if(myLight.getFirstaccess())
				doPutLightRepresentation(0);
			curResource.get(new HashMap<String, String>(), onGetListener);
		} catch (OcException e) {
			Log.e(TAG, e.getMessage());
		}
	}

	void resultView() {
		while (true) {
			try {
				Thread.sleep(100);
				if (flag) {
					Log.v(TAG,"¿¬°á ¼º°ø");
					flag = false;
					break;
				}
			} catch (Exception e) {

			}

		}
	}
	public void saveScreenShot( String filename) {

		WindowManager wm = (WindowManager) getSystemService(Context.WINDOW_SERVICE);
		Display dsp = wm.getDefaultDisplay();
		int mViewWidth = dsp.getWidth();
		int mViewHeight = dsp.getHeight();

		Bitmap bmp = grabPixels(0, 0, mViewWidth, mViewHeight);
		try {
			//String path = getApplicationContext().getFilesDir().getPath() + "/" + filename;
			String path = Environment.getExternalStorageDirectory() + "/" + filename;

			DebugLog.LOGD("Sphere : " + path);

			File file = new File(path);
			file.createNewFile();

			FileOutputStream fos = new FileOutputStream(file);
			bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos);

			fos.flush();

			fos.close();

		} catch (Exception e) {
			DebugLog.LOGD(e.getStackTrace().toString());
		}
	}

	public Bitmap grabPixels(int x, int y, int w, int h) {
		int b[] = new int[w * (y + h)];
		int bt[] = new int[w * h];
		IntBuffer ib = IntBuffer.wrap(b);
		ib.position(0);

		GLES20.glReadPixels(x, 0, w, y + h,
				GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, ib);

		for (int i = 0, k = 0; i < h; i++, k++) {
			for (int j = 0; j < w; j++) {
				int pix = b[i * w + j];
				int pb = (pix >> 16) & 0xff;
				int pr = (pix << 16) & 0x00ff0000;
				int pix1 = (pix & 0xff00ff00) | pr | pb;
				bt[(h - k - 1) * w + j] = pix1;
			}
		}

		Bitmap sb = Bitmap.createBitmap(bt, w, h, Bitmap.Config.RGB_565);
		return sb;
	}


	public void doBridgeSearch() {

		PHBridgeSearchManager sm = (PHBridgeSearchManager) phHueSDK
				.getSDKService(PHHueSDK.SEARCH_BRIDGE);
// Start the UPNP Searching of local bridges.
		sm.search(true, true, true);
		OcRepresentation rep = new OcRepresentation();
		rep.setValueBool("test", false);
		boolean result = rep.getValueBool("test");
	}
}
