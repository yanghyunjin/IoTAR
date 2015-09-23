using UnityEngine;
using System.Collections;
using UnityEngine.UI;
public class MemoryManager : MonoBehaviour {

	GameObject panel;
	Button backButton;

	public static bool Speaker_memory; 
	public static bool Hue_memory;

	GameObject Hue1_canvas;
	GameObject Hue2_canvas;
	GameObject Hue3_canvas;
	GameObject Speaker_canvas;
	GameObject Doorlock_canvas;
	private float getTime=0.0f;
	private float timeLeft=2.0f;

	GameObject canvas;
	GameObject preButton;

	Vector3 location;

	void Awake () {

		Speaker_memory = false;
		Hue_memory = false;
		Debug.Log("Sphere : memory ");
		panel = GameObject.Find ("Panel");

		backButton = GameObject.Find ("BackButton").GetComponent<Button> ();   //panel 에 버튼 컴포넌트 추가해야 동작함.
		Hue1_canvas = GameObject.Find ("Hue1_Canvas");
		Hue2_canvas = GameObject.Find ("Hue2_Canvas");
		Hue3_canvas = GameObject.Find ("Hue3_Canvas");
		Speaker_canvas = GameObject.Find ("Speaker_Canvas");
		Doorlock_canvas = GameObject.Find ("Doorlock_Canvas");

		canvas = GameObject.Find ("SongListCanvas");
		preButton = GameObject.Find ("PreButton");

		backButton.onClick.AddListener (delegate {
			Speaker_memory = false;
			Hue_memory = false;
			Application.LoadLevel("Speaker");
		});  // 원래 씬으로 이동 });

		panel.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite [NewBehaviourScript.memoryPick]; //선택한 이미지를 가져오는 ..


		//---------------------------------------------------------------------
		//Hue1_canvas.SetActive (false);
		//Hue2_canvas.SetActive (false); // 다 false로 하고 이 아래를 통해 트루로 .
		//Hue3_canvas.SetActive (false); // 다 false로 하고 이 아래를 통해 트루로 .
		//Speaker_canvas.SetActive (false);
		Doorlock_canvas.SetActive (false);

		//print (Hue1_canvas.transform.position);



		//Vector3 loc = new Vector3 (0, 50, 400);
		//Vector3 loc1 = new Vector3 (600, 50, 400);
		//Vector3 loc2 = new Vector3 (-200, 50, 400);
		//Hue3_canvas.transform.position = loc;
		//Hue1_canvas.transform.position = loc1;
		//Hue2_canvas.transform.position = loc2;
		//Hue3_canvas.SetActive (true);
		//Hue1_canvas.SetActive (true);
		//Hue2_canvas.SetActive (true);
		//---------------------------------------------------------------------
		for (int i=0; i<NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].len; i++) {   //트루로 하면서 위치까지 설정해서 출력하기 .
		

			location = setLocation(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].position[i]);

			if(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].whatObject[i] == 1){   // hue_1 일 때,

				Hue1_canvas.transform.position = location;
				Hue1_canvas.SetActive (true);
				Hue_memory = true;
			}else if(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].whatObject[i] == 2){ // hue_2 일 때,
				Hue2_canvas.transform.position = location;
				Hue2_canvas.SetActive (true);
				Hue_memory = true;
			}else if(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].whatObject[i] == 3){ //hue_3 일 때,
				
				Hue3_canvas.transform.position = location;
				Hue3_canvas.SetActive (true);
				Hue_memory = true;
			}else if(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].whatObject[i] == 4){ // speaker 일 때,
				Speaker_canvas.transform.position = location;
				Speaker_canvas.SetActive (true);
				Speaker_memory = true;
			}else if(NewBehaviourScript.memoryclass[NewBehaviourScript.memoryPick].whatObject[i] == 5){ // doorlock일 때,
				Doorlock_canvas.transform.position = location;
				Doorlock_canvas.SetActive (true);
			}
		}
		//---------------------------------------------------------------------

		getTime = Time.time;
	}

	Vector3 setLocation(Vector3 vec){

		Vector3 temp = new Vector3 (0, 50, 400);

		if (vec.x < -300) {
			temp.x = -270;
		} else if (vec.x < -100) {
			temp.x = -150;
		}


		if (vec.x > 400) {
			temp.x = 300;
		} else if (vec.x > 200) {
			temp.x = 150;
		}


		if (vec.z > 550) {
			temp.z = 470;
		} else if (vec.z > 480) {
			temp.z = 430;
		}


		if (vec.z < 200) {
			temp.z = 250;
		} else if (vec.z < 300) {
			temp.z = 350;
		}




		   return temp;

	}

	void Update () {
		/*
		if (Speaker_memory) {
			if (Time.time > getTime) {
				Debug.Log ("Time");
				getTime = Time.time + timeLeft;
				preButton.SetActive(true);
				canvas.SetActive(true);
				Debug.Log("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
				AndroidPluginManager.GetInstance ().CallAndroid_Speaker_Get ();
			}
		}
		*/
	}
		
	
	}




