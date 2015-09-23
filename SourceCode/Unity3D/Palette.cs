using UnityEngine;
using System.Collections;

public class Palette : MonoBehaviour {

	public Color[] value = {Color.white,Color.white,Color.white};
	public DrawLine drawLine;

	public GameObject palette1;
	public Renderer[] paletteItem1;
	public GameObject bright1;
	public Renderer[] brightItem1;
	public bool palette_dragging1=false;
	public bool bright_dragging1=false;

	public GameObject palette2;
	public Renderer[] paletteItem2;
	public GameObject bright2;
	public Renderer[] brightItem2;
	public bool palette_dragging2=false;
	public bool bright_dragging2=false;

	public GameObject palette3;
	public Renderer[] paletteItem3;
	public GameObject bright3;
	public Renderer[] brightItem3;
	public bool palette_dragging3=false;
	public bool bright_dragging3=false;

	public int[] brightArray = {36,72,108,144,180,216,252};
	public string[] ColorArray = {"#FF0000","#FF1A1A","#FF3535","#FF5050","#FF6B6B","#FF8686","#FFA1A1",
							"#FF6600","#FF761A","#FF8635","#FF9650","#FFA66B","#FFB686","#FFC6A1",
							"#FFFF00","#FFFF1A","#FFFF35","#FFFF50","#FFFF6B","#FFFF86","#FFFFA1",
							"#00FF00","#1AFF1A","#35FF35","#50FF50","#6BFF6B","#86FF86","#A1FFA1",
							"#0000FF","#1A1AFF","#3535FF","#5050FF","#6B6BFF","#8686FF","#A1A1FF",
							"#6600FF","#761AFF","#8635FF","#9650FF","#A66BFF","#B686FF","#C6A1FF",
							"#FF00FF","#FF1AFF","#FF35FF","#FF50FF","#FF6BFF","#FF86FF","#FFA1FF"};


	// Use this for initialization
	void Start () {
		palette1 = GameObject.Find ("Palette1");
		paletteItem1 = palette1.GetComponentsInChildren<Renderer> ();
		bright1 = GameObject.Find ("Bright1");
		brightItem1 = bright1.GetComponentsInChildren<Renderer> ();
		palette2 = GameObject.Find ("Palette2");
		paletteItem2 = palette2.GetComponentsInChildren<Renderer> ();
		bright2 = GameObject.Find ("Bright2");
		brightItem2 = bright2.GetComponentsInChildren<Renderer> ();
		palette3 = GameObject.Find ("Palette3");
		paletteItem3 = palette3.GetComponentsInChildren<Renderer> ();
		bright3 = GameObject.Find ("Bright3");
		brightItem3 = bright3.GetComponentsInChildren<Renderer> ();
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetMouseButtonDown (0)) {
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				if(hitInfo.transform.gameObject.name.Substring(0, 5) == "1Cube") {
					Debug.Log ("Palette : It's down!");
					palette_dragging1 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor(paletteItem1,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),0);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),0);
				}
				else if(hitInfo.transform.gameObject.name.Substring(0, 6) == "1BCube") {
					Debug.Log ("Bright : It's down!");
					bright_dragging1 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright(brightItem1,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),0);

				}
				else if(hitInfo.transform.gameObject.name.Substring(0, 5) == "2Cube") {
					Debug.Log ("Palette : It's down!");
					palette_dragging2 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor(paletteItem2,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),1);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),1);

				}
				else if(hitInfo.transform.gameObject.name.Substring(0, 6) == "2BCube") {
					Debug.Log ("Bright : It's down!");
					bright_dragging2 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright(brightItem2,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),1);
				}
				else if(hitInfo.transform.gameObject.name.Substring(0, 5) == "3Cube") {
					Debug.Log ("Palette : It's down!");
					palette_dragging3 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor(paletteItem3,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),2);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),2);
				}
				else if(hitInfo.transform.gameObject.name.Substring(0, 6) == "3BCube") {
					Debug.Log ("Bright : It's down!");
					bright_dragging3 = true;
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright(brightItem3,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),2);
				}
			}
		}
		if (palette_dragging1 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 5) == "1Cube")
				{
					SetColor(paletteItem1,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),0);
				}
			}
		}
		if (palette_dragging1 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "1Cube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor (paletteItem1,int.Parse (hitInfo.transform.gameObject.name.Substring (5)),0);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),0);
				}
			}
			palette_dragging1 = false;
		}
		if (bright_dragging1 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 6) == "1BCube")
				{
					SetBright(brightItem1,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
				}
			}
		}
		if (bright_dragging1 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "1BCube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright (brightItem1,int.Parse (hitInfo.transform.gameObject.name.Substring (6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),0);
				}
			}
			bright_dragging1 = false;
		}

		if (palette_dragging2 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 5) == "2Cube")
				{
					SetColor(paletteItem2,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),1);
				}
			}
		}
		if (palette_dragging2 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "2Cube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor (paletteItem2,int.Parse (hitInfo.transform.gameObject.name.Substring (5)),1);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),1);
				}
			}
			palette_dragging2 = false;
		}
		if (bright_dragging2 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 6) == "2BCube")
				{
					SetBright(brightItem2,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
				}
			}
		}
		if (bright_dragging2 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "2BCube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright (brightItem2,int.Parse (hitInfo.transform.gameObject.name.Substring (6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),1);
	
				}
			}
			bright_dragging2 = false;
		}

		if (palette_dragging3 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 5) == "3Cube")
				{
					SetColor(paletteItem3,int.Parse(hitInfo.transform.gameObject.name.Substring(5)),2);
				}
			}
		}
		if (palette_dragging3 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "3Cube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetColor (paletteItem3,int.Parse (hitInfo.transform.gameObject.name.Substring (5)),2);
					SendColor(int.Parse(hitInfo.transform.gameObject.name.Substring(5)),2);
				}
			}
			palette_dragging3 = false;
		}
		if (bright_dragging3 && Input.touchCount > 0 && Input.GetTouch (0).phase == TouchPhase.Moved) {
			Debug.Log ("Touch ing~~~");
			RaycastHit hitInfo = new RaycastHit();
			bool hit = Physics.Raycast(Camera.main.ScreenPointToRay(Input.mousePosition), out hitInfo);
			if (hit) 
			{
				Debug.Log("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring(0, 6) == "3BCube")
				{
					SetBright(brightItem3,int.Parse(hitInfo.transform.gameObject.name.Substring(6)));
				}
			}
		}
		if (bright_dragging3 && Input.GetTouch (0).phase == TouchPhase.Ended) {
			Debug.Log ("Touch end~~~");
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				Debug.Log ("Hit " + hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name.Substring (0, 5) == "3BCube") {
					//android.CallAndroid_Speaker_Volume_Put(int.Parse(hitInfo.transform.gameObject.name.Substring(3)));
					SetBright (brightItem3,int.Parse (hitInfo.transform.gameObject.name.Substring (6)));
					SendBright(int.Parse (hitInfo.transform.gameObject.name.Substring (6)),2);
				}
			}
			bright_dragging3 = false;
		}
	}

	public void SetColor(Renderer[] paletteItem,int colorIndex,int num) {
		for (int i=0; i<49; i++) {
			((Renderer)paletteItem.GetValue (i)).material.color = new Color(((Renderer)paletteItem.GetValue (i)).material.color.r,((Renderer)paletteItem.GetValue (i)).material.color.g,((Renderer)paletteItem.GetValue (i)).material.color.b,.5f);
		}
		((Renderer)paletteItem.GetValue (colorIndex-1)).material.color = new Color(((Renderer)paletteItem.GetValue (colorIndex-1)).material.color.r,((Renderer)paletteItem.GetValue (colorIndex-1)).material.color.g,((Renderer)paletteItem.GetValue (colorIndex-1)).material.color.b,1f);
		Color.TryParseHexString (ColorArray [colorIndex - 1], out value[num]);
	}

	public void SendColor(int colorIndex,int num){
		if(DrawLine.hueInGroup[num]){
			for(int i=0;i<3;i++){
				if(DrawLine.hueInGroup[i]){
					Color.TryParseHexString (ColorArray [colorIndex - 1], out value[i]);
				}
			}
		}
		AndroidPluginManager.GetInstance().CallAndroid_Hue_Color((num+1)+"/"+value[num].r+"/"+value[num].g+"/"+value[num].b);
	}

	public void SetBright(Renderer[] brightItem,int brightIndex) {
		for (int i=0; i<7; i++) {
			((Renderer)brightItem.GetValue (i)).material.color = new Color(((Renderer)brightItem.GetValue (i)).material.color.r,((Renderer)brightItem.GetValue (i)).material.color.g,((Renderer)brightItem.GetValue (i)).material.color.b,.5f);
		}
		((Renderer)brightItem.GetValue (brightIndex-1)).material.color = new Color(((Renderer)brightItem.GetValue (brightIndex-1)).material.color.r,((Renderer)brightItem.GetValue (brightIndex-1)).material.color.g,((Renderer)brightItem.GetValue (brightIndex-1)).material.color.b,1f);
	}

	public void SendBright(int brightItem,int num){
		AndroidPluginManager.GetInstance().CallAndroid_Hue_Bri((num+1)+"/"+brightArray[brightItem-1]);
	}
}
