package com.samsung.iot;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by sora on 15. 8. 16.
 */
public class HueGroup {

    public static final String     TAG = "Hue Group";
    public boolean hf1, hf2, hf3;
    List<String> hueGroupList = new ArrayList<String>();

    private static HueGroup hueGroup;

    private HueGroup() {
        Log.e(TAG, "new Hue Group()");
        hf1=hf2=hf3=false;
    }

    public static HueGroup getInstance() {
        if(hueGroup == null)
            hueGroup = new HueGroup();
        return hueGroup;
    }

    HueGroup getGroup() {
        return this;
    }

    HueGroup setGroup(String hNum,boolean state) {

        if(state) {
            hueGroupList.add(hNum);
            if(hNum.contains("1")){
                hf1 = true;
            }else if(hNum.contains("2")){
                hf2 = true;
            }else if(hNum.contains("3")){
                hf3 = true;
            }
        }
        else {
            if(hueGroupList.contains(hNum)) {
                hueGroupList.remove(hNum);
                if(hNum.contains("1")){
                    hf1 = false;
                }else if(hNum.contains("2")){
                    hf2 = false;
                }else if(hNum.contains("3")){
                    hf3 = false;
                }
            }
        }

        return this;
    }

    void allDeleteGroupList() {
        if(!hueGroupList.isEmpty()) {
            hf1 = hf2 =hf3 = false;
            hueGroupList.clear();
        }
    }



}