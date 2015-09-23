using UnityEngine;
using System.Collections;

public class ChangeColor : MonoBehaviour {
	[Tooltip("What Color Picker Code Will Affect The Color.")]
	public Palette ColorPickerToUse;
	public DrawLine drawLine;

	private bool isCamera;
	private bool isLight;
	// Use this for initialization
	void Start () {
		if (gameObject.GetComponent<Camera> ()) {
						isCamera = true;
				} else {
			isCamera = false;		
		}

		if (gameObject.GetComponent<Light> ()) {
						isLight = true;

				} else {
			isLight = false;		
		}
	}
	
	// Update is called once per frame
	void Update () {
		if (ColorPickerToUse) {
			if(gameObject.name.Substring(0,13)=="Switch_Sphere"){
				if (DrawLine.Hue_State[int.Parse(gameObject.name.Substring(13))-1] == false){
					gameObject.GetComponent<Renderer>().material.color=Color.gray;
					drawLine.ColorPicker[int.Parse(gameObject.name.Substring(13))-1].SetActiveRecursively (false);
					drawLine.BrightnessPicker[int.Parse(gameObject.name.Substring(13))-1].SetActiveRecursively (false);

				}
				else {
					drawLine.ColorPicker[int.Parse(gameObject.name.Substring(13))-1].SetActiveRecursively (true);
					drawLine.BrightnessPicker[int.Parse(gameObject.name.Substring(13))-1].SetActiveRecursively (true);
					if (!isCamera && !isLight && gameObject.GetComponent<Renderer> ().material) {
						gameObject.GetComponent<Renderer> ().material.color = ColorPickerToUse.value[int.Parse(gameObject.name.Substring(13))-1];
					}
					if (isCamera) {
						gameObject.GetComponent<Camera> ().backgroundColor = ColorPickerToUse.value[int.Parse(gameObject.name.Substring(13))-1];
					}

					if (isLight) {
						gameObject.GetComponent<Light> ().color = ColorPickerToUse.value[int.Parse(gameObject.name.Substring(13))-1];
					}
				}
			}
		} else {
 			Debug.LogWarning ("No 'ColorPickerToUse' was found.");
			return;
		}
	}
}
