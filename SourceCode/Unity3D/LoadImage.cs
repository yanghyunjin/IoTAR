using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.IO;
public class LoadImage : MonoBehaviour {

	public static bool first = false;

	// Use this for initialization
	void Awake () {
	

	

	}
	
	// Update is called once per frame
	void Update () {
	
		if (first == false) {
		//	SetImage (0);
		//	SetImage (1);
		//	SetImage (2);
		//	SetImage (3);
		//	SetImage (4);
			Debug.Log ("file read : 하기 전 ");
			first=true;
			LoadImageInfo ();

			for(int i=0; i<NewBehaviourScript.memoryNum; i++){

				SetImage (i);
			}
		}

	}

	public void LoadImageInfo(){

		//string filepath = "file:///C:/Users/rhkdgss232/Pictures/memoryImage.txt";  
		string t = "";
		string line = "";
		string x, y, z;


		StreamReader sr = new StreamReader("/storage/emulated/0/memoryImage.txt");
		if (sr == null)
		{
			NewBehaviourScript.memoryNum = 0; // 한개도 없다. 또는 파일이 없다 .
		}
		else
		{
			line = sr.ReadLine();
		
			NewBehaviourScript.memoryNum = int.Parse(line);


			for(int i=0; i<NewBehaviourScript.memoryNum; i++){    //저장된 메모리 값만 큼 돈다!,

				NewBehaviourScript.memoryClass temp = new NewBehaviourScript.memoryClass();

				line = sr.ReadLine();

				temp.len = int.Parse (line); // i 번 이미지에 오브젝트가 몇개나 있는가?

				for(int j=0; j<temp.len; j++){
					line = sr.ReadLine();
					x = sr.ReadLine();
					y = sr.ReadLine();
					z = sr.ReadLine();
					temp.whatObject[j] = int.Parse (line);




					Vector3 loc = new Vector3 (float.Parse(x),float.Parse(y), float.Parse(z));
					temp.position[j] = loc;
					NewBehaviourScript.memoryclass[i] = temp;
					Debug.Log (i+" 번 이미지 갯수 : "+ NewBehaviourScript.memoryclass[i].len + " " + NewBehaviourScript.memoryclass[i].whatObject[j]
					           + "    좌표 : " + NewBehaviourScript.memoryclass[i].position[j]);
				}
			}
		


			Debug.Log ("총 몇개의 이미지 : " + NewBehaviourScript.memoryNum);

			sr.Close();
			// print("Loaded " + Application.dataPath + "/Resources/db/" + fileName);
		}


	}

	public void SetImage (int a) {  // 이미지를 불러와서 sprite에 저장하는 함수.
		string url = "file://" + "/storage/emulated/0" + "/test"+a+".png";  
		
	//	string url = "file:///C:/Users/rhkdgss232/Pictures/test"+a+".png";  
		
		Debug.Log("Sphere in uinty url : " + url);
		WWW imageURLWWW = new WWW(url);  
		if(imageURLWWW.texture != null) {
			Sprite sprite = new Sprite();     
			sprite = Sprite.Create(imageURLWWW.texture, new Rect(0, 0, imageURLWWW.texture.width, imageURLWWW.texture.height), Vector2.zero);  
			NewBehaviourScript.memorySprite[a] = sprite;
		}else{
			print ("not load image");
		}

		//-------------------------------------------------------------------------------------------
		
		switch(a){
		case 0 :
			NewBehaviourScript.memorySceneImage0.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite[0];
			break;
		case 1:
			NewBehaviourScript.memorySceneImage1.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite[1];
			break;
		case 2:
			NewBehaviourScript.memorySceneImage2.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite[2];
			break;
		case 3:
			NewBehaviourScript.memorySceneImage3.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite[3];
			break;
		case 4:
			NewBehaviourScript.memorySceneImage4.GetComponent<Image> ().sprite = NewBehaviourScript.memorySprite[4];
			break;
		}
		
		
	}
}
