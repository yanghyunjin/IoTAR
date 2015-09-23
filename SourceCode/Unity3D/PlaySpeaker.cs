using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class PlaySpeaker : MonoBehaviour {
	
	
	/// <summary>
	/// 마우스가 다운된 오브젝트
	/// </summary>

	public AndroidPluginManager android;
	private bool playFlag = false, pauseFlag = false, preFlag = false, nextFlag = false,dragging = false;
	private GameObject playButton, pauseButton, preButton, nextButton, lyrics, song, volume, canvas;
	TextMesh lyricsTextMesh, songTextMesh;
	private Renderer[] volumeItem;
	private Button[] songItem;
	private int volumeNum = 0;
	private int track;
	private Vuforia.StateManager repeatGet;
	private float getTime=0.0f;
	private float timeLeft=2.0f;

	//hexToColor("0DCCBDAB") (13,204, 189, 189); 볼륨 click
	// hexToColor ("011D1B25"); (1, 29, 27, 27); 볼륨 unclick
	
	public static string[] sarr, aarr;
	public static bool List;


	void Start() {
		Application.LoadLevelAdditive ("IoTAR1");
		Debug.Log ("PlaySpeaker Start()");
		repeatGet = Vuforia.TrackerManager.Instance.GetStateManager (); 
		android = AndroidPluginManager.GetInstance ();
		// android.CallAndroid_Speaker_Get ();
		playButton = GameObject.Find ("PlayButton");
		pauseButton = GameObject.Find ("PauseButton");
		preButton = GameObject.Find ("PreButton");
		nextButton = GameObject.Find ("NextButton");
		song = GameObject.Find ("Song");
		lyrics = GameObject.Find ("Lyrics");
		volume = GameObject.Find ("Volume");
		canvas = GameObject.Find ("SongListCanvas");
		songItem = GameObject.Find ("GridOfList").GetComponentsInChildren<Button> ();
		List = false;
		for (int i=0; i<20; ++i) {
			AddListener((Button)songItem.GetValue (i), ((Button)songItem.GetValue (i)).name.Substring(4));
		}

		songTextMesh = (TextMesh)song.GetComponent (typeof(TextMesh));
		lyricsTextMesh = (TextMesh)lyrics.GetComponent (typeof(TextMesh));
		
		playButton.SetActive (true);
		pauseButton.SetActive (false);
		volumeItem = volume.GetComponentsInChildren<Renderer> (); // volume item (100개)
		volume.SetActive (false);
		android.CallAndroid_Speaker_Get();
		getTime = Time.time;
	}

	void AddListener(Button b, string v) {
		Debug.Log ("AddListener" + v);
		b.onClick.AddListener (delegate {
			android.CallAndroid_Speaker_Put (int.Parse(v));
		});
	}
	
	// Use this for initialization
	void Awake () {
		Debug.Log("PlaySpeaker Awake()");
	}

	// Update is called once per frame
	void Update () {
		if (Time.time > getTime) {
			Debug.Log ("Time");
			getTime = Time.time + timeLeft;
			WhatObject();
		}
	// When Mouse Button Down...
		if (Input.GetMouseButtonDown (0)) {
			Debug.Log ("Mouse is down");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name == "PlayButton") {
					Debug.Log ("PlayButton : It's down!");
					playFlag = true;
				} else if (hitInfo.transform.gameObject.name == "PauseButton") {
					Debug.Log ("PauseButton : It's down!"); 
					pauseFlag = true;
				} else if (hitInfo.transform.gameObject.name == "PreButton") {
					Debug.Log ("PreButton : It's down!");
					preFlag = true;
				} else if (hitInfo.transform.gameObject.name == "NextButton") {
					Debug.Log ("NextButton : It's down!");
					nextFlag = true;
				} else if(hitInfo.transform.gameObject.name.Substring(0, 3) == "vol") {
					Debug.Log ("Volume : It's down!");
					dragging = true;
					android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetVolume(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
				} 
			}
		}
		
		// When Mouse Button Up...
		if (Input.GetMouseButtonUp (0)) {
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name == "PlayButton" && playFlag)
				{
					Debug.Log ("PlayButton : It's up!");
					android.CallAndroid_Speaker_Play_Put();
					SetPlaying(true);
				} else if (hitInfo.transform.gameObject.name == "PauseButton" && pauseFlag) {
					Debug.Log ("PauseButton : It's up!");
					android.CallAndroid_Speaker_Play_Put();
					SetPlaying(false);
				} else if (hitInfo.transform.gameObject.name == "PreButton" && preFlag) {
					Debug.Log ("PreButton : It's up!");
					//SetLyricsText(changeText);
					if(track==1)
						android.CallAndroid_Speaker_Put(sarr.Length);
					else android.CallAndroid_Speaker_Put(track-1);
					preFlag = false;
				} else if (hitInfo.transform.gameObject.name == "NextButton" && nextFlag) {
					Debug.Log ("NextButton : It's up!");
					//SetLyricsText(changeText2);
					if(track==sarr.Length)
						android.CallAndroid_Speaker_Put(1);
					else android.CallAndroid_Speaker_Put(track+1);
					nextFlag = false;
				} else if (hitInfo.transform.gameObject.name.Substring(0, 4) == "Song") {
					Debug.Log ("SongButton : It's up!");
					android.CallAndroid_Speaker_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
				}
			}
		}

		//If a touch is detected
		if (dragging && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 3) == "vol")
				{
					SetVolume(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
				}
			}
		}

		if(dragging && Input.GetTouch (0).phase == TouchPhase.Ended){
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 3) == "vol")
				{
					SetVolume(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
				}
			}
			dragging = false;
		}
			
	}
	
	public void SetVolume(int volumeIndex) {
		if (volumeNum > volumeIndex) {
			for(int i=volumeIndex; i<volumeNum; ++i)
				((Renderer)volumeItem.GetValue(i)).material.color = hexToColor("94A8A8AE");
		}
		for (int i=0; i<volumeIndex; ++i)
			((Renderer)volumeItem.GetValue (i)).material.color = hexToColor ("32A8FDA3");
		volumeNum = volumeIndex;

		Debug.Log ("asdf: 나ㅓ 볼륨 와썽요~~~~~" );
	}
	
	public void SetPlayText(string text, int songIndex) {
		lyricsTextMesh.text = text;

		Debug.Log ("asdf: 666666666666666666666666666" );
		songTextMesh.text = sarr[songIndex-1].ToString();

		Debug.Log ("asdf: 7777777777777777777777777777" );
		track = songIndex;

		Debug.Log ("asdf: 888888888888888888888888888" );

		Debug.Log ("asdf: " + lyricsTextMesh.text);
		Debug.Log ("asdf: " + lyrics.activeSelf);
	}

	public void SetPlaying(bool play) {
		playButton.SetActive (!play); 
		pauseButton.SetActive (play);
		volume.SetActive (play);
		song.SetActive(play);
		lyrics.SetActive (play);
		playFlag = !play; 
		pauseFlag = play;
		Debug.Log ("asdf: 나ㅓ 플레잉 와썽요~~~~~" );
	}

	public void SetList(string[] songArr, string[] artistArr) {

		Debug.Log ("asdf: 000000000000000000000000" );
			sarr = songArr;
			aarr = artistArr;
			

			for (int i=0; i<songArr.Length; i++) {
			Debug.Log ("asdf: 111111111111111111111111111111111" );
				((Button)songItem.GetValue (i)).GetComponentInChildren<Text> ().text = songArr [i] + "\n" + artistArr [i];

			}
			for (int i=songArr.Length; i<20; i++) {
			Debug.Log ("asdf: 22222222222222222222222222222222" );
				((Button)songItem.GetValue (i)).gameObject.SetActive (false);

			}

		
	}


	void WhatObject() {
		System.Collections.Generic.IEnumerable<Vuforia.TrackableBehaviour> activeTrackables = repeatGet.GetActiveTrackableBehaviours ();
		bool markVisible = false;
		bool Hue_detect = false;
		foreach(Vuforia.TrackableBehaviour tb in activeTrackables) {
			Debug.Log (tb.TrackableName);
			if(tb.TrackableName == "toggi") {
				Debug.Log ("toggi detection");
				markVisible = true;
			}else if(tb.TrackableName == "hue_1"){
				Debug.Log ("hue_1 detection");  //여기에 memoryclass에 위치정보를 저장 고고 
				Hue_detect = true;
			}else if(tb.TrackableName == "hue_2"){
				Debug.Log ("hue_2 detection");
				Hue_detect = true;
			}else if(tb.TrackableName == "hue_3"){
				Debug.Log ("hue_3 detection");
				Hue_detect = true;

			}
		}
		Debug.Log (markVisible);

		if (Hue_detect || MemoryManager.Hue_memory) {
			Debug.Log("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
			android.CallAndroid_Hue_Get();
		}

		if (markVisible || MemoryManager.Speaker_memory) {
			canvas.SetActive (true);
			android.CallAndroid_Speaker_Get();
			Debug.Log("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
		} else {
			canvas.SetActive (false);
		
		}
	}

	
	public static Color hexToColor(string hex)
	{
		hex = hex.Replace ("0x", "");//in case the string is formatted 0xFFFFFF
		hex = hex.Replace ("#", "");//in case the string is formatted #FFFFFF
		byte a = 255;//assume fully visible unless specified in hex
		byte r = byte.Parse(hex.Substring(0,2), System.Globalization.NumberStyles.HexNumber);
		byte g = byte.Parse(hex.Substring(2,2), System.Globalization.NumberStyles.HexNumber);
		byte b = byte.Parse(hex.Substring(4,2), System.Globalization.NumberStyles.HexNumber);
		//Only use alpha if the string has enough characters
		if(hex.Length == 8){
			a = byte.Parse(hex.Substring(4,2), System.Globalization.NumberStyles.HexNumber);
		}
		Debug.Log (r + ", " + g + ", " + b + ", " + a);
		return new Color32(r,g,b,a);
	}

}
