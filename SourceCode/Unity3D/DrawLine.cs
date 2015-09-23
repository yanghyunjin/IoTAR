using UnityEngine;
using System.Collections;

public class DrawLine : MonoBehaviour {


	public GameObject[] ColorPicker;
	public GameObject[] BrightnessPicker;
	public GameObject[] Switch;

	public static LineRenderer lineRenderer1;
	public static bool group1_flag = false;
	public Transform origin1;
	public Transform destination1;
	public static string origin1_name;
	public static string destination1_name;


	public static LineRenderer lineRenderer2;
	public static bool group2_flag = false;
	public Transform origin2;
	public Transform destination2;
	public static string origin2_name;
	public static string destination2_name;

	public static bool[] hueInGroup = {false,false,false};
	public static bool[] Hue_State = {false,false,false};

	public static Transform origin3;
	public static bool group3_flag = false;


	private float counter;
	private float dist;
	public static bool dragging=false;
	private bool link_flag=false;

	public float lineDrawSpeed = 50f;

	public Transform drawline_origin_thumb;
	public Vector3 offset;

	public static bool aniStart1=false;
	public static bool aniStart2=false;

	public void hueStateSend(int num){
		if (Hue_State[num]) {
			AndroidPluginManager.GetInstance().CallAndroid_Hue_State((num+1)+"/true");
		} else {
			AndroidPluginManager.GetInstance().CallAndroid_Hue_State((num+1)+"/false");
		}
	}
	public void hueStateChange (int num){
		print ("hueStateChange : " + num);
		if (Hue_State[num] == false) {
			Hue_State[num] = true;
		} else {
			Hue_State[num] = false;
		}
	}

	private void addColliderToLine(LineRenderer lineRenderer, Vector3 startPos,Vector3 endPos,int i)
	{
		BoxCollider col;
		if (!GameObject.Find ("Collider" + i)) {
			col = new GameObject("Collider"+i).AddComponent<BoxCollider> ();
		}
		else {
			Destroy(GameObject.Find ("Collider" + i).GetComponent<BoxCollider>());
			col = GameObject.Find ("Collider" + i).AddComponent<BoxCollider> ();

		}
		col.transform.parent = lineRenderer.transform; // Collider is added as child object of line
		float lineLength = Vector3.Distance (startPos, endPos); // length of line
		col.size = new Vector3 (lineLength, 80f, 80f); // size of collider is set where X is length of line, Y is width of line, Z will be set as per requirement
		Vector3 midPoint = (startPos + endPos)/2;
		col.transform.position = midPoint; // setting position of collider object
		// Following lines calculate the angle between startPos and endPos
		float angle = (Mathf.Abs (startPos.y - endPos.y) / Mathf.Abs (startPos.x - endPos.x));
		if((startPos.y<endPos.y && startPos.x>endPos.x) || (endPos.y<startPos.y && endPos.x>startPos.x))
		{
			angle*=-1;
		}
		angle = Mathf.Rad2Deg * Mathf.Atan (angle);
		col.transform.Rotate (0, 0, angle);
	}

	void Start () {
		for (int i=0; i<3; i++) {
			ColorPicker[i] = GameObject.Find ("Palette"+(i+1));
			BrightnessPicker[i] = GameObject.Find ("Bright"+(i+1));
		}
		lineRenderer1=GameObject.Find("LineRenderer1").GetComponent<LineRenderer>();
		lineRenderer1.SetWidth (25f, 25f);
		lineRenderer2=GameObject.Find("LineRenderer2").GetComponent<LineRenderer>();
		lineRenderer2.SetWidth (25f, 25f);
	}
	
	// Update is called once per frame
	void Update () {
		print ("hueInGroup : " + hueInGroup [0] + " , " + hueInGroup [1] + " , " + hueInGroup [2]);
		print ("aniStart1 : " + aniStart1+" aniStart2 : " + aniStart2);
		if (aniStart1) {
			origin1=GameObject.Find(origin1_name).transform;
			destination1=GameObject.Find(destination1_name).transform;
			print("origin1 : "+origin1+"  destination1 : "+destination1);
			lineRenderer1.SetPosition (0, origin1.position);
			lineRenderer1.SetPosition (1, destination1.position);
			addColliderToLine(lineRenderer1,origin1.position,destination1.position,1);
		}
		if (aniStart2) {
			origin2=GameObject.Find(origin2_name).transform;
			destination2=GameObject.Find(destination2_name).transform;
			print("origin2 : "+origin2+"  destination2 : "+destination2);
			lineRenderer2.SetPosition (0, origin2.position);
			lineRenderer2.SetPosition (1, destination2.position);
			addColliderToLine(lineRenderer2,origin2.position,destination2.position,2);
		} 
		if (Input.GetMouseButtonDown (0)) {
			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				print (hitInfo.transform.gameObject.name);
				if (hitInfo.transform.gameObject.name == "Collider1") {
					Vector3 reset=new Vector3(0,0,0);
					lineRenderer1.SetPosition(0,reset);
					lineRenderer1.SetPosition(1,reset);
					group1_flag=false;
					aniStart1=false;

					lineRenderer1.gameObject.SetActiveRecursively (false);

					if(origin1.gameObject.name.Substring(0,13)=="Switch_Sphere"){
						hueInGroup[int.Parse(origin1.gameObject.name.Substring(13))-1]=false;
						AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin1.gameObject.name.Substring(13))+"/false");
					}

					if(destination1.gameObject.name.Substring(0,13)=="Switch_Sphere"){
						hueInGroup[int.Parse(destination1.gameObject.name.Substring(13))-1]=false;
						AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination1.gameObject.name.Substring(13))+"/false");
					}

					
					if(aniStart2){
						if(origin2.gameObject.name.Substring(0,13)=="Switch_Sphere"){
							hueInGroup[int.Parse(origin2.gameObject.name.Substring(13))-1]=true;
							AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin2.gameObject.name.Substring(13))+"/true");
						}
									                     
						if(destination2.gameObject.name.Substring(0,13)=="Switch_Sphere"){
							hueInGroup[int.Parse(destination2.gameObject.name.Substring(13))-1]=true;
							AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination2.gameObject.name.Substring(13))+"/true");
						}
					}
				}
				else if (hitInfo.transform.gameObject.name == "Collider2") {
					Vector3 reset=new Vector3(0,0,0);
					lineRenderer2.SetPosition(0,reset);
					lineRenderer2.SetPosition(1,reset);
					group2_flag=false;
					aniStart2=false;

					lineRenderer2.gameObject.SetActiveRecursively (false);

					if(origin2.gameObject.name.Substring(0,13)=="Switch_Sphere"){
						hueInGroup[int.Parse(origin2.gameObject.name.Substring(13))-1]=false;
						AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin2.gameObject.name.Substring(13))+"/false");

					}
					
					if(destination2.gameObject.name.Substring(0,13)=="Switch_Sphere"){
						hueInGroup[int.Parse(destination2.gameObject.name.Substring(13))-1]=false;
						AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination2.gameObject.name.Substring(13))+"/false");
					}
					
					
					if(aniStart1){
						if(origin1.gameObject.name.Substring(0,13)=="Switch_Sphere"){
							hueInGroup[int.Parse(origin1.gameObject.name.Substring(13))-1]=true;
							AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin1.gameObject.name.Substring(13))+"/true");
						}
						
						if(destination1.gameObject.name.Substring(0,13)=="Switch_Sphere"){
							hueInGroup[int.Parse(destination1.gameObject.name.Substring(13))-1]=true;
							AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination1.gameObject.name.Substring(13))+"/true");
						}
					}
				}
				else if (hitInfo.transform.gameObject.name.Substring(0,13) == "Switch_Sphere") {
					dragging = true;
					if(!aniStart1){
						origin1 = hitInfo.transform;
						origin1_name=hitInfo.transform.gameObject.name;
						group1_flag=true;
					}
					else if(!aniStart2){
						origin2 = hitInfo.transform;
						origin2_name=hitInfo.transform.gameObject.name;
						group2_flag=true;
					}
					else{
						origin3=hitInfo.transform;
						group3_flag=true;
					}
				}

			}
		}
		if (Input.GetMouseButtonUp (0)) {

			RaycastHit hitInfo = new RaycastHit ();
			bool hit = Physics.Raycast (Camera.main.ScreenPointToRay (Input.mousePosition), out hitInfo);
			if (hit) {
				print ("dragging target : " + hitInfo.transform.gameObject.name);
				if(dragging){
					if (hitInfo.transform.gameObject.name.Substring(0,13) == "Switch_Sphere"){
						if (group1_flag) {
							if (origin1.gameObject.name != hitInfo.transform.gameObject.name) {
								lineRenderer1.gameObject.SetActiveRecursively (true);
								destination1 = hitInfo.transform;
								destination1_name=hitInfo.transform.gameObject.name;
								bool origin_State=false;
								bool destination_State=false;

								if(origin1.gameObject.name.Substring(0, 13) == "Switch_Sphere") {

									hueInGroup[int.Parse(origin1.gameObject.name.Substring(13))-1]=true;
									AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin1.gameObject.name.Substring(13))+"/true");
									origin_State = Hue_State[int.Parse(origin1.gameObject.name.Substring(13))-1];
								}

								if(destination1.gameObject.name.Substring(0, 13) == "Switch_Sphere"){
									hueInGroup[int.Parse(destination1.gameObject.name.Substring(13))-1]=true;
									AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination1.gameObject.name.Substring(13))+"/true");
									destination_State = Hue_State[int.Parse(destination1.gameObject.name.Substring(13))-1];
									if(origin_State!=destination_State){
										hueStateChange(int.Parse(destination1.gameObject.name.Substring(13))-1);
										hueStateSend(int.Parse(destination1.gameObject.name.Substring(13))-1);

										if(hueInGroup[int.Parse(origin1.gameObject.name.Substring(13))-1]){
											for(int i=0;i<3;i++){
												if(hueInGroup[i]){
													Hue_State[i]=Hue_State[int.Parse(origin1.gameObject.name.Substring(13))-1];	
												}
											}
										}
									}
								}

								aniStart1 = true;
								group1_flag=false;

								print ("Linked origin : " + origin1.gameObject.name);
								print ("Linked destination : " + destination1.gameObject.name);
							}
							else {
								dragging=false;
								if (hitInfo.transform.gameObject.name.Substring(0, 13) == "Switch_Sphere") {
									if (hitInfo.transform.gameObject.name.Substring(0, 13) == "Switch_Sphere") {
										hueStateChange(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);
										hueStateSend(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);

										if(hueInGroup[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1]){
											for(int i=0;i<3;i++){
												if(hueInGroup[i]){
													Hue_State[i]=Hue_State[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1];	
												}
											}
										}
									}
								}
							}
						}
						else if(group2_flag){
							if (origin2.gameObject.name != hitInfo.transform.gameObject.name) {

								lineRenderer2.gameObject.SetActiveRecursively (true);
								destination2 = hitInfo.transform;
								destination2_name=hitInfo.transform.gameObject.name;
								bool origin_State=false;
								bool destination_State=false;

								if(origin2.gameObject.name.Substring(0, 13) == "Switch_Sphere") {
									
									hueInGroup[int.Parse(origin2.gameObject.name.Substring(13))-1]=true;
									AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(origin2.gameObject.name.Substring(13))+"/true");
									origin_State = Hue_State[int.Parse(origin2.gameObject.name.Substring(13))-1];
								}
								
								if(destination2.gameObject.name.Substring(0, 13) == "Switch_Sphere"){
									hueInGroup[int.Parse(destination2.gameObject.name.Substring(13))-1]=true;
									AndroidPluginManager.GetInstance().CallAndroid_Hue_Group(int.Parse(destination2.gameObject.name.Substring(13))+"/true");
									destination_State = Hue_State[int.Parse(destination2.gameObject.name.Substring(13))-1];
									if(origin_State!=destination_State){
										hueStateChange(int.Parse(destination2.gameObject.name.Substring(13))-1);
										hueStateSend(int.Parse(destination2.gameObject.name.Substring(13))-1);

										if(hueInGroup[int.Parse(origin2.gameObject.name.Substring(13))-1]){
											for(int i=0;i<3;i++){
												if(hueInGroup[i]){
													Hue_State[i]=Hue_State[int.Parse(origin2.gameObject.name.Substring(13))-1];	
												}
											}
										}
									}
								}

								aniStart2 = true;
								group2_flag = false;
								print ("Linked origin : " + origin2.gameObject.name);
								print ("Linked destination : " + destination2.gameObject.name);

							}
							else {
								dragging=false;
								if (hitInfo.transform.gameObject.name.Substring(0, 13) == "Switch_Sphere") {
									hueStateChange(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);
									hueStateSend(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);

									if(hueInGroup[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1]){
										for(int i=0;i<3;i++){
											if(hueInGroup[i]){
												Hue_State[i]=Hue_State[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1];	
											}
										}
									}
								}
							}
						}
						else if(group3_flag){
							dragging=false;
							if (origin3.gameObject.name == hitInfo.transform.gameObject.name) {
								if (hitInfo.transform.gameObject.name.Substring(0, 13) == "Switch_Sphere") {
									hueStateChange(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);
									hueStateSend(int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1);

									if(hueInGroup[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1]){
										for(int i=0;i<3;i++){
											if(hueInGroup[i]){
												Hue_State[i]=Hue_State[int.Parse(hitInfo.transform.gameObject.name.Substring(13))-1];	
											}
										}
									}
								}
							}
						}
					}
				}
			}
			dragging=false;
			Debug.Log ("owijfuwiojfeiowjfwioejfiowejfio0wjfiojwefiowjeio0fjwe");
		}
	}
}
