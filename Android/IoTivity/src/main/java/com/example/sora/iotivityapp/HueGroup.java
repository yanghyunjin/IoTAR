package com.example.sora.iotivityapp;

import android.content.Context;
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

    HueGroup setGroup(String hNum) {
        hueGroupList.add(hNum);
        if(hNum.equals("\"1\"") || hNum.equals("1")) {
            Log.e(TAG, "hf1 => true");
            hf1=true;
        }
        if(hNum.equals("\"2\"") || hNum.equals("2")) {
            Log.e(TAG, "hf2 => true");
            hf2=true;
        }
        if(hNum.equals("\"3\"") || hNum.equals("3")) {
            Log.e(TAG, "hf3 => true");
            hf3=true;
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
