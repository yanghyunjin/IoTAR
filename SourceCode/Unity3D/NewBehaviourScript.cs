using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.IO;
public class NewBehaviourScript : MonoBehaviour {

	public static int memoryNum;              // 지금까지 몇개 찍었는가... 
	public static int memoryPick;            //메모리 씬 중에 몇번을 클릭했는지 저장하는 함수.
	public static memoryClass[] memoryclass; // 사진 찍었을때 오브젝들의 위치와 누구 였는지. 
	public static Sprite[] memorySprite;      //이미지를 저장하는 곳, 매번 불러오면 너무 오래걸려서. 한번 시도.

	GameObject canvasTarget;        // 모든 오브젝트를 다 없애거나 나타나게 하려고. 
	GameObject memoryCanvas;
	//Button cameraButton;           // 사진찍는 버튼 .

	Button memorySceneButton0;        // 해당 이미지로 이동하는 버튼
	Button memorySceneButton1;
	Button memorySceneButton2;
	Button memorySceneButton3;
	Button memorySceneButton4;

	public static Image memorySceneImage0;       // 저장한 이미지를 저장하는 함수.
	public static Image memorySceneImage1;
	public static Image memorySceneImage2; 
	public static Image memorySceneImage3;
	public static Image memorySceneImage4;

	Vuforia.StateManager state;   // 무슨 오브젝이 화면에 출력 중인지 알기 위해 필요한 변수.

	GameObject Hue1;          // 각종 이미지 타겟들
	GameObject Hue3;          // 각종 이미지 타겟들
	GameObject Hue2;          // 각종 이미지 타겟들
	GameObject Speaker;          // 각종 이미지 타겟들
	GameObject Doorlock;          // 각종 이미지 타겟들



	GameObject longClickImage1;
	GameObject longClickImage2;
	GameObject longClickImage3;
	GameObject longClickImage4;
	GameObject longClickImage5;
	GameObject longClickImage6;


	Button linkButton;
	GameObject linkingImage1;
	GameObject linkingImage2;
	GameObject linkingImage3;
	GameObject linkingImage4;
	GameObject linkingImage5;
	GameObject linkingImage6;

	bool longClickDown;


	private float longClickTime;

	double linkCount;
	public static bool linkState = true;



	Touch touch;

	void Awake () {
		linkCount = 0;
//		cameraButton = GameObject.Find ("CameraButton").GetComponent<Button> ();           //카메라 버튼 
//		cameraButton.onClick.AddListener (delegate{StartCoroutine(CameraButtonFunction());});   //카메라 버튼 리스너 

		memorySceneButton0 = GameObject.Find ("ScreenshotPanel0").GetComponent<Button> ();
		memorySceneButton1 = GameObject.Find ("ScreenshotPanel1").GetComponent<Button> ();   //panel 에 버튼 컴포넌트 추가해야 동작함.
		memorySceneButton2 = GameObject.Find ("ScreenshotPanel2").GetComponent<Button> ();
		memorySceneButton3 = GameObject.Find ("ScreenshotPanel3").GetComponent<Button> ();
		memorySceneButton4 = GameObject.Find ("ScreenshotPanel4").GetComponent<Button> ();

		memorySceneImage0 = GameObject.Find ("ScreenshotPanel0").GetComponent<Image> ();
		memorySceneImage1 = GameObject.Find ("ScreenshotPanel1").GetComponent<Image> ();   //panel 에 이미지 컴포넌트 추가해야 세팅됨.
		memorySceneImage2 = GameObject.Find ("ScreenshotPanel2").GetComponent<Image> ();
		memorySceneImage3 = GameObject.Find ("ScreenshotPanel3").GetComponent<Image> ();
		memorySceneImage4 = GameObject.Find ("ScreenshotPanel4").GetComponent<Image> ();

		memorySceneButton0.onClick.AddListener (delegate { ChangeScene(0);});  //1, 2, 3 다 추가 해야 함 
		memorySceneButton1.onClick.AddListener (delegate { ChangeScene(1);});  //1, 2, 3 다 추가 해야 함 
		memorySceneButton2.onClick.AddListener (delegate { ChangeScene(2);});  //1, 2, 3 다 추가 해야 함 
		memorySceneButton3.onClick.AddListener (delegate { ChangeScene(3);});  //1, 2, 3 다 추가 해야 함 
		memorySceneButton4.onClick.AddListener (delegate { ChangeScene(4);});  //1, 2, 3 다 추가 해야 함 

		
		canvasTarget = GameObject.Find ("CanvasTarget");   //타겟들을 하나의 캔버스에 다 넣고 캔버스를 없애면 차일드도 다 사라지는듯 
		memoryCanvas = GameObject.Find ("MemoryCanvas");


		state = Vuforia.TrackerManager.Instance.GetStateManager ();  // whatObject 함수에서 필요함. 

		if (memoryclass == null) {                          // 초기 부팅시 할것들 .
			memoryclass = new memoryClass[5];
		}

		if (memorySprite == null) {
			memorySprite = new Sprite[5];
		}

		setMemoryImage (); // 화면 상단에 이미지 세팅하는 함수;   
	


		Hue1 = GameObject.Find ("ImageTarget1");  // 현재는 구지만 파레트나 mp3 도어락 같은거 해야함.
		Hue2 = GameObject.Find ("ImageTarget2");  // 현재는 구지만 파레트나 mp3 도어락 같은거 해야함.
		Hue3 = GameObject.Find ("ImageTarget3");  // 현재는 구지만 파레트나 mp3 도어락 같은거 해야함.
		Speaker = GameObject.Find ("ObjectTarget"); 
		Doorlock = GameObject.Find ("ImageTarget"); 


		longClickImage1 = GameObject.Find ("LongClickImage1");
		longClickImage2 = GameObject.Find ("LongClickImage2");
		longClickImage3 = GameObject.Find ("LongClickImage3");
		longClickImage4 = GameObject.Find ("LongClickImage4");
		longClickImage5 = GameObject.Find ("LongClickImage5");
		longClickImage6 = GameObject.Find ("LongClickImage6");

		
		linkingImage1 = GameObject.Find ("LinkingImage1");
		linkingImage2 = GameObject.Find ("LinkingImage2");
		linkingImage3 = GameObject.Find ("LinkingImage3");
		linkingImage4 = GameObject.Find ("LinkingImage4");
		linkingImage5 = GameObject.Find ("LinkingImage5");
		linkingImage6 = GameObject.Find ("LinkingImage6");

		linkButton = GameObject.Find ("LinkButton").GetComponent<Button> ();

		linkButton.onClick.AddListener (delegate { 
			LinkButtonFunction();
		});  


		setLongClickImage (-1);


	}
	// Update is called once per frame
	void Update () {


	//	linkButton.enabled = true;   //버튼 되살리기 
	//	linkButton.GetComponentInChildren<CanvasRenderer> ().SetAlpha (1);


		if (DrawLine.dragging) {

			longClickDown = false;
			setLongClickImage (-1);
		}

		if (Input.GetMouseButtonDown (0)) {
			print ("down click");
			longClickTime = Time.time;
			longClickDown = true;

			//writeImageInfo();
		} else if (Input.GetMouseButtonUp (0)) {

			
			longClickDown = false;
			setLongClickImage (-1);
		} else if(longClickDown ) {

			//print (Time.time - t0);
			setLongClickImage(Time.time - longClickTime);

		}
	
	}

	public IEnumerator CameraButtonFunction(){  //카메라버튼을 누르면 사진찍는 리스너 함수   IEnumerator
		Debug.Log ("Sphere : camera button click");
		if (WhatObject()) {  //화면에 오브젝트가 찍힌 경우에만 수행하겠다.
			canvasTarget.SetActive (false);
			memoryCanvas.SetActive(false);
			Debug.Log ("thisthisthisthis");
			yield return new WaitForSeconds(0.5f); //1초간 실행을 보류한다.
			string filename = "test" + memoryNum%5 + ".png";
			AndroidPluginManager.GetInstance ().ScreenShot (filename);

			SetImage (memoryNum%5);
			
			memoryNum++;

		}
		canvasTarget.SetActive (true);
		memoryCanvas.SetActive(true);
		writeImageInfo ();
	}

	public void LinkButtonFunction(){
		AndroidPluginManager.GetInstance ().RequestHueConnect ();
	}



	public void ChangeScene(int pick){  //화면 상단이미지를 클릭하면 그 이미지로 이동하는 함수. 

		if (pick < memoryNum) {  //선택한게 현재까지 저장한 num보다는 커야 한다. 작으면 비어있는 칸이므로.
			memoryPick = pick;
			Application.LoadLevel ("memory");  // 씬 이동
		}

	}

	void setMemoryImage(){      //화면 상단에 이미지를 적용하는 함수.
		if (memoryNum >= 5) {
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
			memorySceneImage1.GetComponent<Image> ().sprite = memorySprite[1];
			memorySceneImage2.GetComponent<Image> ().sprite = memorySprite[2];
			memorySceneImage3.GetComponent<Image> ().sprite = memorySprite[3];
			memorySceneImage4.GetComponent<Image> ().sprite = memorySprite[4];
		} else if (memoryNum >= 4) {
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
			memorySceneImage1.GetComponent<Image> ().sprite = memorySprite[1];
			memorySceneImage2.GetComponent<Image> ().sprite = memorySprite[2];
			memorySceneImage3.GetComponent<Image> ().sprite = memorySprite[3];
		}
		else if (memoryNum >= 3) {
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
			memorySceneImage1.GetComponent<Image> ().sprite = memorySprite[1];
			memorySceneImage2.GetComponent<Image> ().sprite = memorySprite[2];
		} else if (memoryNum >= 2) {
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
			memorySceneImage1.GetComponent<Image> ().sprite = memorySprite[1];
		} else if (memoryNum >= 1) {
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
		} else{
			print ("not image");
		}
	}


	public void SetImage (int a) {  // 이미지를 불러와서 sprite에 저장하는 함수.
		string url = "file://" + "/storage/emulated/0" + "/test"+a+".png";  



		Debug.Log("Sphere in uinty url : " + url);
		WWW imageURLWWW = new WWW(url);  
		if(imageURLWWW.texture != null) {
			Sprite sprite = new Sprite();     
			sprite = Sprite.Create(imageURLWWW.texture, new Rect(0, 0, imageURLWWW.texture.width, imageURLWWW.texture.height), Vector2.zero);  
			memorySprite[a] = sprite;
		}else{
			print ("not load image");
		}

		//-------------------------------------------------------------------------------------------
		
		switch(a){
		case 0 :
			memorySceneImage0.GetComponent<Image> ().sprite = memorySprite[0];
			break;
		case 1:
			memorySceneImage1.GetComponent<Image> ().sprite = memorySprite[1];
			break;
		case 2:
			memorySceneImage2.GetComponent<Image> ().sprite = memorySprite[2];
			break;
		case 3:
			memorySceneImage3.GetComponent<Image> ().sprite = memorySprite[3];
			break;
		case 4:
			memorySceneImage4.GetComponent<Image> ().sprite = memorySprite[4];
			break;
		}


	}

	bool WhatObject(){  //현재 어떤 물체가 화면에 출력되어있는지 아는 함수 
		System.Collections.Generic.IEnumerable<Vuforia.TrackableBehaviour> activeTrackables = state.GetActiveTrackableBehaviours ();
		bool have = false;
		memoryClass temp = new memoryClass();
		int present = 0;
		foreach(Vuforia.TrackableBehaviour tb in activeTrackables){
			if(tb.TrackableName == "hue_1"){
				Debug.Log ("hue_1 detection");  //여기에 memoryclass에 위치정보를 저장 고고 
				temp.position[present%3] = Hue1.transform.position;  //여길 변경 
				temp.whatObject[present%3] = 1;
				present++;
				have = true;
			}else if(tb.TrackableName == "hue_2"){
				Debug.Log ("hue_2 detection");
				temp.position[present%3] = Hue2.transform.position;  //여길 변경 
				temp.whatObject[present%3] = 2;
				present++;
				have = true;
			}else if(tb.TrackableName == "hue_3"){
				Debug.Log ("hue_3 detection");
				Debug.Log (Hue3.transform.position);  //여기
				temp.position[present%3] = Hue3.transform.position;  //여길 변경 
				temp.whatObject[present%3] = 3;
				present++;
				have = true;
			}else if(tb.TrackableName == "toggi"){
				Debug.Log ("speaker detection");
				temp.position[present%3] = Speaker.transform.position;  //여길 변경 
				temp.whatObject[present%3] = 4;
				present++;
				have = true;
			}else if(tb.TrackableName == "tracker"){
				Debug.Log ("doorlock detection");
				temp.position[present%3] = Doorlock.transform.position;  //여길 변경 
				temp.whatObject[present%3] = 5;
				present++;
				have = true;
			}
		}

		if (present >= 3) {
			temp.len = 3;
		} else {
			temp.len = present;
		}
		print (temp.len);
		memoryclass [memoryNum % 5] = temp;
		return have;
	}

	public class memoryClass
	{
		public int len;
		public Vector3[] position;
		public int[] whatObject;  // 1 = hue1, 2 = hue2; 3 = hue3; 4 = speaker, 5 = doorlock;

		public memoryClass(){
			position = new Vector3[3];
			whatObject = new int[3];
		}

	}

	public void setLongClickImage(double a){

		Vector3 image1 = new Vector3 (-129.6f, 0f);
		Vector3 image2 = new Vector3 (-74.5f, 101.7f);
		Vector3 image3 = new Vector3 (62.2f, 100.3f);
		Vector3 image4 = new Vector3 (117.5f, -6.7f);
		Vector3 image5 = new Vector3 (60.1f, -109.9f);
		Vector3 image6 = new Vector3 (-75.5f, -106.5f);

		longClickImage1.transform.position = Input.mousePosition + image1;
		longClickImage2.transform.position = Input.mousePosition + image2;
		longClickImage3.transform.position = Input.mousePosition + image3;
		longClickImage4.transform.position = Input.mousePosition + image4;
		longClickImage5.transform.position = Input.mousePosition + image5;
		longClickImage6.transform.position = Input.mousePosition + image6;

		if (a < 0.5) {
			longClickImage1.SetActive (false);
			longClickImage2.SetActive (false);
			longClickImage3.SetActive (false);
			longClickImage4.SetActive (false);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
		}
		else if (a <= 1) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (false);
			longClickImage3.SetActive (false);
			longClickImage4.SetActive (false);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
		} else if (a <= 1.5) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (true);
			longClickImage3.SetActive (false);
			longClickImage4.SetActive (false);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
		} else if (a <= 2) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (true);
			longClickImage3.SetActive (true);
			longClickImage4.SetActive (false);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
		} else if (a <= 2.5) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (true);
			longClickImage3.SetActive (true);
			longClickImage4.SetActive (true);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
		} else if (a <= 3) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (true);
			longClickImage3.SetActive (true);
			longClickImage4.SetActive (true);
			longClickImage5.SetActive (true);
			longClickImage6.SetActive (false);
		} else if (a <= 3.5) {
			longClickImage1.SetActive (true);
			longClickImage2.SetActive (true);
			longClickImage3.SetActive (true);
			longClickImage4.SetActive (true);
			longClickImage5.SetActive (true);
			longClickImage6.SetActive (true);
		} else {
			longClickImage1.SetActive (false);
			longClickImage2.SetActive (false);
			longClickImage3.SetActive (false);
			longClickImage4.SetActive (false);
			longClickImage5.SetActive (false);
			longClickImage6.SetActive (false);
			StartCoroutine(CameraButtonFunction());
			longClickDown = false;
		}

	}

	void writeImageInfo(){

		Debug.Log ("write start");

		//StreamWriter sr = new StreamWriter("C:/Users/rhkdgss232/Pictures/memoryImage.txt");
		//string url = "file://" + "/storage/emulated/0" + "/test"+a+".png";  
		StreamWriter sr = new StreamWriter("/storage/emulated/0/memoryImage.txt");
		int temp = memoryNum;
		if (temp > 5) {
			temp = 5;
		}
		sr.WriteLine (temp);
			
			
		for(int i=0; i<temp; i++){    //저장된 메모리 값만 큼 돈다!,
				
				
			sr.WriteLine(memoryclass[i].len);
		
				
			for(int j=0; j<memoryclass[i].len; j++){

				sr.WriteLine(memoryclass[i].whatObject[j]);
				sr.WriteLine(memoryclass[i].position[j].x);
				sr.WriteLine(memoryclass[i].position[j].y);
				sr.WriteLine(memoryclass[i].position[j].z);
					
					
				}
			}
			
			
			
			sr.Close();
		
		
		
		Debug.Log ("write end");



	}


}