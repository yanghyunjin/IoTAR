using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class AndroidPluginManager : MonoBehaviour
{
	static AndroidPluginManager _instance;
	public DrawLine drawline;
	private AndroidJavaObject curActivity;
	public PlaySpeaker speaker;
	public NewBehaviourScript memoryCS;
	public PinCodeControl pin;
	
	public string strLog = "Westwood Forever Unity3D Android Plugin Sample";
	
	public static AndroidPluginManager GetInstance()
	{
		if( _instance == null )
		{
			_instance = new GameObject("AndroidPluginManager").AddComponent<AndroidPluginManager>();    
		}
		
		return _instance;
	}
	
	void Awake()
	{
		Debug.Log ("Awake()");
		AndroidJavaClass jc = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
		curActivity = jc.GetStatic<AndroidJavaObject>("currentActivity");
	}
	
	
	public void CallAndroid_Hue_State(string strMsg)
	{
		//자바 호출
		curActivity.Call("CallAndroid_Hue_State", strMsg);
	}
	public void CallAndroid_Hue_Group(string strMsg)
	{
		//자바 호출
		curActivity.Call("CallAndroid_Hue_Group", strMsg);
	}
	public void CallAndroid_Hue_Color(string strMsg)
	{
		//		자바 호출
		curActivity.Call("CallAndroid_Hue_Color", strMsg);
	}
	public void CallAndroid_Hue_Bri(string strMsg)
	{
		//자바 호출
		curActivity.Call("CallAndroid_Hue_Bri", strMsg);
	}
	
	public void SetHue(string strLog)
	{
		//		strSetHue += i+"/"+power+"/"+x+"/"+y+"/"+bri+"/";
		string[] strMsg = strLog.Split (new char[] {'/'});

		/*
		for (int i=0; i<3; i++) {
			if(drawline.Hue_State[i]!=bool.Parse(strMsg[i*5+1])){
				drawline.hueStateChange(i);
			}
		}*/
		DrawLine.Hue_State [0] = bool.Parse(strMsg [0]);
		DrawLine.Hue_State [1] = bool.Parse (strMsg [1]);
		DrawLine.Hue_State [2] =  bool.Parse(strMsg [2]);

		
		Debug.Log ("pppppppppppppppppppppppppppppppppppppppp");
		Debug.Log (strLog); //           true/false/true
	}
	public void CallAndroid_Hue_Get() {
		Debug.Log ("CallAndroid_Hue_Get() gogo");
		curActivity.Call("CallAndroid_Hue_Get");
	}
	
	public void CallAndroid_Speaker_Get() {
		Debug.Log ("CallAndroid_Speaker_Get() gogo");
		curActivity.Call("CallAndroid_Speaker_Get");
	}
	
	
	public void CallAndroid_Speaker_Put(int songNum) {
		Debug.Log ("CallAndroid_Speaker_Put() gogo" + songNum);
		curActivity.Call("CallAndroid_Speaker_Put", songNum);
	}
	
	public void CallAndroid_Speaker_Play_Put() {
		Debug.Log ("CallAndroid_Speaker_Play_Put() gogo");
		curActivity.Call("CallAndroid_Speaker_Play_Put");
	}
	
	public void CallAndroid_Speaker_Volume_Put(int volumeNum) {
		Debug.Log ("CallAndroid_Speaker_Volume_Put() gogo");
		curActivity.Call("CallAndroid_Speaker_Volume_Put", volumeNum);
	}
	
	
	protected void SetGet(string str)
	{
		Debug.Log ("SetGet Start" + str);
		speaker = FindObjectOfType<PlaySpeaker> ();
		string[] result = str.Split ('@');




		speaker.SetVolume (int.Parse (result.GetValue(0).ToString()));
		if (result.GetValue(1).ToString() == "1") { // 재생 중이므로 일시 정지 만들어줌
			speaker.SetPlaying (true);
		} else {
			speaker.SetPlaying (false);
		}


		speaker.SetList (result.GetValue (4).ToString ().Split('/'), result.GetValue (5).ToString ().Split ('/'));
		speaker.SetPlayText (result.GetValue(2).ToString(), (int.Parse (result.GetValue(3).ToString())));


		//speaker.SetPlaying (true);
		//speaker.SetNewCode (strLog);
	}
	
	public void CallAndroid_Doorlock_Get() {
		Debug.Log ("CallAndroid_Doorlock_Get() gogo");
		curActivity.Call("CallAndroid_Doorlock_Get");
	}
	
	public void CallAndroid_Doorlock_Open_Put() {
		Debug.Log ("CallAndroid_Doorlock_Open_Put() gogo");
		curActivity.Call("CallAndroid_Doorlock_Open_Put");
	}
	
	public void CallAndroid_Doorlock_Set_Put(string newCode) {
		Debug.Log ("CallAndroid_Doorlock_Set_Put() gogo" + newCode);
		curActivity.Call("CallAndroid_Doorlock_Set_Put", newCode);
	}

	protected void SetLog(string strLog)
	{
		Debug.Log ("SetLog gogo" + strLog);
	}
	
	protected void SetDoorlock(string strLog)
	{
		Debug.Log ("SetDoorlock gogo" + strLog);
		this.strLog = strLog;
		pin = FindObjectOfType<PinCodeControl> ();
		pin.SetNewCode (strLog);
	}
	public void ScreenShot(string strMsg){
		
		curActivity.Call ("saveScreenShot", strMsg);
	}
	public void RequestHueConnect(){
		Debug.Log ("Request Link Hue gogo" + strLog);
		curActivity.Call ("requestLinkHue");
	}


}
