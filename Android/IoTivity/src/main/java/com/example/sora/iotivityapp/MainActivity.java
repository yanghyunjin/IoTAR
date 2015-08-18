package com.example.sora.iotivityapp;


import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.base.ErrorCode;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;


import com.philips.lighting.hue.listener.PHGroupListener;
import com.philips.lighting.hue.sdk.PHAccessPoint;
import com.philips.lighting.hue.sdk.PHBridgeSearchManager;
import com.philips.lighting.hue.sdk.PHHueSDK;
import com.philips.lighting.hue.sdk.PHSDKListener;
import com.philips.lighting.hue.sdk.upnp.PHBridgeSearchManagerImpl;
import com.philips.lighting.model.PHBridge;
import com.philips.lighting.model.PHBridgeResource;
import com.philips.lighting.model.PHGroup;
import com.philips.lighting.model.PHHueError;
import com.philips.lighting.model.PHHueParsingError;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.TextView;



public class MainActivity extends Activity implements OcPlatform.OnResourceFoundListener, View.OnClickListener {

    private PHHueSDK               phHueSDK;
    public static final String     TAG = "Hue Plugin";
    private AccessPointListAdapter adapter;
    private Button get1, get2, get3, put1, put2, put3, put, cb, group;
    private TextView tget1, tget2, tget3;
    private EditText p1, p2, p3, b1, b2 ,b3, c1, c2, c3;
    private boolean flag = false; // resultVeiwflag
    private boolean groupFlag = false; //groupView Flag
    private CheckBox r1, r2, r3;

    static int                     OBSERVE_TYPE_TO_USE = ObserveType.OBSERVE.getValue();

    public static Light            myLight;
    public static HueGroup         hueGroup;
    public static OcResource       curResource;
    public static OcResourceHandle resourceHandle;
    static int                     oc                  = 0;
    static Activity                mActivity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        get1 = (Button) findViewById(R.id.get1);
        get2 = (Button) findViewById(R.id.get2);
        get3 = (Button) findViewById(R.id.get3);
        put1 = (Button) findViewById(R.id.put1);
        put2 = (Button) findViewById(R.id.put2);
        put3 = (Button) findViewById(R.id.put3);
        put=(Button) findViewById(R.id.allput);
        cb=(Button) findViewById(R.id.connectBtn);
        group =(Button)findViewById(R.id.group);
        get1.setOnClickListener(this);
        get2.setOnClickListener(this);
        get3.setOnClickListener(this);
        put1.setOnClickListener(this);
        put2.setOnClickListener(this);
        put3.setOnClickListener(this);
        put.setOnClickListener(this);
        cb.setOnClickListener(this);
        group.setOnClickListener(this);
        tget1 = (TextView) findViewById(R.id.tget1);
        tget2 = (TextView) findViewById(R.id.tget2);
        tget3 = (TextView) findViewById(R.id.tget3);
        p1 = (EditText) findViewById(R.id.power1);
        p2 = (EditText) findViewById(R.id.power2);
        p3 = (EditText) findViewById(R.id.power3);
        b1 = (EditText) findViewById(R.id.bri1);
        b2 = (EditText) findViewById(R.id.bri2);
        b3 = (EditText) findViewById(R.id.bri3);
        c1 = (EditText) findViewById(R.id.color1);
        c2 = (EditText) findViewById(R.id.color2);
        c3 = (EditText) findViewById(R.id.color3);
        r1 = (CheckBox) findViewById(R.id.light1);
        r2 = (CheckBox) findViewById(R.id.light2);
        r3 = (CheckBox) findViewById(R.id.light3);


        hueGroup = HueGroup.getInstance();
        setVisibleRadioButton();
        mActivity = this;
        myLight = new Light();

        OcRepresentation rp = new OcRepresentation();
        rp.setValueBool("Iotivityapp", false);
        boolean result = rp.getValueBool("Iotivityapp");

        // Gets an instance of the Hue SDK.
        phHueSDK = PHHueSDK.getInstance();

        // Set the Device Name (name of your app). This will be stored in your
        // bridge whitelist entry.
        phHueSDK.setAppName("QuickStartApp");
        phHueSDK.setDeviceName(android.os.Build.MODEL);

        // Register the PHSDKListener to receive callbacks from the bridge.
        phHueSDK.getNotificationManager().registerSDKListener(listener);

        adapter = new AccessPointListAdapter(this,
                phHueSDK.getAccessPointsFound());

        // Try to automatically connect to the last known bridge. For first time
        // use this will be empty so a bridge search is automatically started.

        // Automatically try to connect to the last connected IP Address. For
        // multiple bridge support a different implementation is required.

        Log.w(TAG, "Searching for Bridges.");

    }

    private void setCheckRadioButton(){
        if (hueGroup.hf1)
            r1.setChecked(true);
        else
            r1.setChecked(false);
        if (hueGroup.hf2)
            r2.setChecked(true);
        else
            r2.setChecked(false);
        if (hueGroup.hf3)
            r3.setChecked(true);
        else
            r3.setChecked(false);
    }

    private void setVisibleRadioButton(){
        if(groupFlag) {
            r1.setVisibility(View.VISIBLE);
            r2.setVisibility(View.VISIBLE);
            r3.setVisibility(View.VISIBLE);
            setCheckRadioButton();
        } else {
            r1.setVisibility(View.INVISIBLE);
            r2.setVisibility(View.INVISIBLE);
            r3.setVisibility(View.INVISIBLE);
        }
    }


    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.connectBtn:
                doBridgeSearch();
                resultView();
                break;
            case R.id.allput:
                doPutLightRepresentation(0);
                resultView();
                break;
            case R.id.put1:
                doPutLightRepresentation(1);
                resultView();
                break;
            case R.id.put2:
                doPutLightRepresentation(2);
                resultView();
                break;
            case R.id.put3:
                doPutLightRepresentation(3);
                resultView();
                break;
            case R.id.get1:
                break;
            case R.id.get2:
                break;
            case R.id.get3:
                break;
            case R.id.group:
                if(!groupFlag) {
                    groupFlag = true;
                    setVisibleRadioButton();
                } else {
                    doDeletetLightRepresentation();
                    hueGroup.allDeleteGroupList();
                    if(r1.isChecked()) hueGroup.setGroup("1");
                    if(r2.isChecked()) hueGroup.setGroup("2");
                    if(r3.isChecked()) hueGroup.setGroup("3");
                    doPostLightRepresentation();
                    groupFlag = false;
                    setVisibleRadioButton();
                }
                break;

        }

    }

    void resultView() {
        while (true) {
            try {
                Thread.sleep(100);
                if (flag) {
                    tget1.setText("p:" + myLight.getPower(1) + ", b:" + String.valueOf(myLight.getBri(1)) + ", c:" + String.valueOf(myLight.getColor(1)));
                    tget2.setText("p:" + myLight.getPower(2) + ", b:" + String.valueOf(myLight.getBri(2)) + ", c:" + String.valueOf(myLight.getColor(2)));
                    tget3.setText("p:" + myLight.getPower(3) + ", b:" + String.valueOf(myLight.getBri(3)) + ", c:" + String.valueOf(myLight.getColor(3)));
                    flag = false;
                    break;
                }
            } catch (Exception e) {

            }

        }
    }


    public void doBridgeSearch() {

        PHBridgeSearchManager sm = (PHBridgeSearchManager) phHueSDK
                .getSDKService(PHHueSDK.SEARCH_BRIDGE);
        // Start the UPNP Searching of local bridges.
        sm.search(true, true, true);
        OcRepresentation rep = new OcRepresentation();
        rep.setValueBool("test", false);
        boolean result = rep.getValueBool("test");
    }

    private PHSDKListener listener = new PHSDKListener() {

        @Override
        public void onAccessPointsFound(
                List<PHAccessPoint> accessPoint) {
            Log.w(TAG, "Access Points Found. "
                    + accessPoint.size());
            if (accessPoint != null
                    && accessPoint.size() > 0) {
                phHueSDK.getAccessPointsFound()
                        .clear();
                phHueSDK.getAccessPointsFound()
                        .addAll(accessPoint);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        adapter.updateData(phHueSDK
                                .getAccessPointsFound());
                    }
                });
                PHAccessPoint accessPoint2 = (PHAccessPoint) adapter
                        .getItem(0);
                accessPoint2.setUsername("VgB67nPMqdHJOPXh");

                Log.e(TAG, accessPoint2.getIpAddress() + " : " + accessPoint2.getUsername());

                myLight.setIp(accessPoint2.getIpAddress());
                myLight.setUsername(accessPoint2.getUsername());

                PHBridge connectedBridge = PHHueSDK.getInstance().getSelectedBridge();


                if (connectedBridge != null) {
                    String connectedIP = connectedBridge
                            .getResourceCache()
                            .getBridgeConfiguration()
                            .getIpAddress();
                    Log.w(TAG, "Connected IP: "
                            + connectedIP);


                    if (connectedIP != null) { // We
                        // are
                        // already
                        // connected
                        // here:-
                        Log.w(TAG,
                                "Connected IP != null");
                        phHueSDK.disableHeartbeat(connectedBridge);
                        phHueSDK.disconnect(connectedBridge);
                    }
}
                phHueSDK.connect(accessPoint2);
}
            }


            @Override
            public void onCacheUpdated (List < Integer > arg0, PHBridge bridge){
                Log.w(TAG, "On CacheUpdated.");
            }

            @Override
            public void onBridgeConnected(PHBridge b, String s) {
            Log.w(TAG,
                    "Connected to the Bridge. " + s);
            phHueSDK.setSelectedBridge(b);
            phHueSDK.enableHeartbeat(b,
                    PHHueSDK.HB_INTERVAL);
            phHueSDK.getLastHeartbeat()
                    .put(b.getResourceCache()
                                    .getBridgeConfiguration()
                                    .getIpAddress(),
                            System.currentTimeMillis());
            initOICStack();
        }


        @Override
        public void onAuthenticationRequired(
                PHAccessPoint accessPoint) {
            Log.w(TAG,
                    "Authentication Required.");
            phHueSDK.startPushlinkAuthentication(accessPoint);
            Log.w(TAG, "Access Point IP addr: "
                    + accessPoint.getIpAddress());
            Log.w(TAG,
                    "Access Point MAC addr: "
                            + accessPoint
                            .getMacAddress());
            Log.w(TAG, "Access Point key: "
                    + accessPoint.getUsername());
        }

        @Override
        public void onConnectionResumed(
                PHBridge bridge) {
            // Log.v(TAG, "onConnectionResumed "
            // +
            // bridge.getResourceCache().getBridgeConfiguration().getIpAddress());
            phHueSDK.getLastHeartbeat()
                    .put(bridge
                                    .getResourceCache()
                                    .getBridgeConfiguration()
                                    .getIpAddress(),
                            System.currentTimeMillis());
            for (int i = 0; i < phHueSDK
                    .getDisconnectedAccessPoint()
                    .size(); i++) {
                if (phHueSDK
                        .getDisconnectedAccessPoint()
                        .get(i)
                        .getIpAddress()
                        .equals(bridge
                                .getResourceCache()
                                .getBridgeConfiguration()
                                .getIpAddress())) {
                    phHueSDK.getDisconnectedAccessPoint()
                            .remove(i);
                }
            }
        }

        @Override
        public void onConnectionLost(
                PHAccessPoint accessPoint) {
            Log.v(TAG, "onConnectionLost : "
                    + accessPoint.getIpAddress());
            if (!phHueSDK
                    .getDisconnectedAccessPoint()
                    .contains(accessPoint)) {
                phHueSDK.getDisconnectedAccessPoint()
                        .add(accessPoint);
            }
        }

        @Override
        public void onError(int code,
                            final String message) {
            Log.e(TAG, "on Error Called : "
                    + code + ":" + message);

            if (code == PHHueError.NO_CONNECTION) {
                Log.w(TAG, "On No Connection");
            } else if (code == PHHueError.AUTHENTICATION_FAILED
                    || code == 1158) {

            } else if (code == PHHueError.BRIDGE_NOT_RESPONDING) {
                Log.w(TAG,
                        "Bridge Not Responding . . . ");
            }
        }

        @Override
        public void onParsingErrors(
                List<PHHueParsingError> parsingErrorsList) {
            for (PHHueParsingError parsingError : parsingErrorsList) {
                Log.e(TAG,
                        "ParsingError : "
                                + parsingError
                                .getMessage());
            }
        }
    };

    private void initOICStack() {
        //create platform config
        PlatformConfig cfg = new PlatformConfig(
                this,
                ServiceType.IN_PROC,
                ModeType.CLIENT,
                "0.0.0.0", // bind to all available interfaces
                0,
                QualityOfService.LOW);
        OcPlatform.Configure(cfg);
        try {
            /**
             * find all resources
             */
            Log.e(TAG, "findResource");
            OcPlatform.findResource("", OcPlatform.WELL_KNOWN_QUERY,
                    OcConnectivityType.CT_DEFAULT, this);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }

    synchronized public void onResourceFound(OcResource ocResource) {
        /**
         *  this may potentially be called by multiple threads at the same time
         */
        synchronized (this) {
            String resourceUri;
            String hostAddress;
            resourceUri = ocResource.getUri();
            hostAddress = ocResource.getHost();
            Log.v(TAG, "resourceUri :" +resourceUri);
            // get the resource types
            if (resourceUri.equals("/iotar/hue")) {
                Log.v(TAG, "/iotar/hue gogo");
                curResource = ocResource;
                doGetLightRepresentation();
            }
        }

    }


    private void doPostLightRepresentation() {
        OcResource.OnPostListener onPostListener = new OcResource.OnPostListener() {
            @Override
            public void onPostCompleted(List<OcHeaderOption> list, OcRepresentation ocRepresentation) {
                Log.e(TAG, "Post resource was successful ");
                doGetLightRepresentation();
            }

            @Override
            public void onPostFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };
        Log.d(TAG, "MyLight Post START");
        OcRepresentation rep = new OcRepresentation();

        try {
            rep.setValue("hf1", hueGroup.hf1);
            rep.setValue("hf2", hueGroup.hf2);
            rep.setValue("hf3", hueGroup.hf3);

        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        try {
            Log.d(TAG, "before calling post");
            curResource.post(rep, new HashMap<String, String>(), onPostListener);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "end of put call");
    }

    private void doDeletetLightRepresentation() {
        OcResource.OnDeleteListener onDeleteListener = new OcResource.OnDeleteListener() {

            @Override
            public void onDeleteCompleted(List<OcHeaderOption> list) {
                Log.e(TAG, "Delete resource was successful ");
            }

            @Override
            public void onDeleteFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };
        Log.d(TAG, "MyLight Delete START");
        try {
            Log.d(TAG, "before calling delete");
            curResource.deleteResource(onDeleteListener);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "end of put call");
    }

    private void doPutLightRepresentation(int num) {
        // eventhandler for put()
        OcResource.OnPutListener onPutListener = new OcResource.OnPutListener() {
            @Override
            public void onPutCompleted(List<OcHeaderOption> ocHeaderOptions,
                                       OcRepresentation ocRepresentation) {
                Log.e(TAG, "PUT resource was successful");
                try {
                    for(int i=1; i<=3; ++i) {
                        int bri, color;
                        String power;
                        bri = Integer.parseInt(ocRepresentation.getValue("hue" + i + "_brightness").toString());
                        color = Integer.parseInt(ocRepresentation.getValue("hue"+i+"_color").toString());
                        power = ocRepresentation.getValue("hue"+i+"_power").toString();
                        myLight.setBri(bri, i);
                        myLight.setColor(color, i);
                        myLight.setPower(power, i);
                    }
                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }
                flag= true;
                Log.e(TAG, "onPutCompleted\n<Brightness> Light 1 - " + myLight.getBri(1) +
                        " Light 2 - " + myLight.getBri(2) + " Light 3 - " + myLight.getBri(3) +
                        "\n<Power> Light 1 - " + myLight.getPower(1) + " Light 2 - " + myLight.getPower(2) +
                        " Light 3 - " + myLight.getPower(3) + "\n<Color> Light 1 - " + myLight.getColor(1) +
                        " Light 2 - " + myLight.getColor(2) + " Light 3 - " + myLight.getColor(3));
            }

            @Override
            public void onPutFailed(Throwable throwable) {

                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };
        Log.d(TAG, "MyLight Put START");

        OcRepresentation rep = new OcRepresentation();

        try {

            if(myLight.getFirstaccess()) {
                Log.e(TAG, "FirstAccess() ip :" + myLight.getIp() + ", username : " + myLight.getUsername());

                rep.setValue("hue_ip", myLight.getIp());
                rep.setValue("hue_username", myLight.getUsername());
            } else {
                switch(num) {
                    case 0:
                        setLight(1, p1.getText().toString(), Integer.parseInt(b1.getText().toString()), Integer.parseInt(c1.getText().toString()));
                        setLight(2, p2.getText().toString(), Integer.parseInt(b2.getText().toString()), Integer.parseInt(c2.getText().toString()));
                        setLight(3, p3.getText().toString(), Integer.parseInt(b3.getText().toString()), Integer.parseInt(c3.getText().toString()));
                        break;
                    case 1:
                        setLight(1, p1.getText().toString(), Integer.parseInt(b1.getText().toString()), Integer.parseInt(c1.getText().toString()));
                        if(hueGroup.hf1) {
                            if(hueGroup.hf2) {
                                setLight(2, p1.getText().toString(), Integer.parseInt(b1.getText().toString()), Integer.parseInt(c1.getText().toString()));
                            }
                            if(hueGroup.hf3) {
                                setLight(3, p1.getText().toString(), Integer.parseInt(b1.getText().toString()), Integer.parseInt(c1.getText().toString()));
                            }
                        }
                        break;
                    case 2:
                        Log.e(TAG, p2.getText().toString());
                        setLight(2, p2.getText().toString(), Integer.parseInt(b2.getText().toString()), Integer.parseInt(c2.getText().toString()));
                        if(hueGroup.hf2) {
                            if(hueGroup.hf1) {
                                setLight(1, p2.getText().toString(), Integer.parseInt(b2.getText().toString()), Integer.parseInt(c2.getText().toString()));
                            }
                            if(hueGroup.hf3) {
                                setLight(3, p2.getText().toString(), Integer.parseInt(b2.getText().toString()), Integer.parseInt(c2.getText().toString()));
                            }
                        }
                        break;
                    case 3:
                        setLight(3, p3.getText().toString(), Integer.parseInt(b3.getText().toString()), Integer.parseInt(c3.getText().toString()));
                        if(hueGroup.hf3) {
                            if(hueGroup.hf1) {
                                setLight(1, p3.getText().toString(), Integer.parseInt(b3.getText().toString()), Integer.parseInt(c3.getText().toString()));
                            }
                            if(hueGroup.hf2) {
                                setLight(2, p3.getText().toString(), Integer.parseInt(b3.getText().toString()), Integer.parseInt(c3.getText().toString()));
                            }
                        }
                        break;
                }
                for(int i=1; i<=3; ++i) {
                    rep.setValue("hue"+i+"_brightness", myLight.getBri(i));
                    rep.setValue("hue"+i+"_color", myLight.getColor(i));
                    rep.setValue("hue"+i+"_power", myLight.getPower(i));
                }
            }
            rep.setValue("order", 10);
            rep.setValue("firstaccess", myLight.getFirstaccess());

        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        try {
            Log.d(TAG, "before calling put");
            curResource.put(rep, new HashMap<String, String>(), onPutListener);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
        Log.d(TAG, "end of put call");
    }

    void setLight(int n, String p, int b, int c) {
        myLight.setPower(p, n);
        myLight.setBri(b, n);
        myLight.setColor(c, n);
    }

    private void doGetLightRepresentation() {
        // eventhandler for get()
        OcResource.OnGetListener onGetListener = new OcResource.OnGetListener() {
            @Override
            public void onGetCompleted(List<OcHeaderOption> headerOptionList,
                                       OcRepresentation ocRepresentation) {
                Log.e(TAG, "GET resource was successful ");

                try {
                    int bri, color;
                    String power;
                    if(myLight.getFirstaccess()) {
                        int hueGroupMemberSize = Integer.parseInt(ocRepresentation.getValue("hue_group_size").toString());
                        Log.e(TAG, "groupsize : " + hueGroupMemberSize);
                        if(hueGroupMemberSize!=0 ) {
                            for(int i=1; i<=hueGroupMemberSize; ++i) {
                                hueGroup.setGroup(ocRepresentation.getValue("hue"+i+"_group").toString());
                                Log.e(TAG, hueGroup.getGroup().hueGroupList.get(i - 1).toString());
                            }
                        }
                        Log.e(TAG, "group end ");
                        myLight.setFirstaccess(false);
                    }

                    for(int i=1; i<=3; ++i) {
                        bri = Integer.parseInt(ocRepresentation.getValue("hue" + i + "_brightness").toString());
                        color = Integer.parseInt(ocRepresentation.getValue("hue"+i+"_color").toString());
                        power = ocRepresentation.getValue("hue"+i+"_power").toString();
                        myLight.setBri(bri, i);
                        myLight.setColor(color, i);
                        myLight.setPower(power, i);
                    }
                    flag= true;
                    Log.e(TAG, "firstAcess----------------------" + myLight.getFirstaccess());
                    Log.e(TAG, "onGetCompleted\n<Brightness> Light 1 - " + myLight.getBri(1) +
                            " Light 2 - " + myLight.getBri(2) + " Light 3 - " + myLight.getBri(3) +
                            "\n<Power> Light 1 - " + myLight.getPower(1) + " Light 2 - " + myLight.getPower(2) +
                            " Light 3 - " + myLight.getPower(3) + "\n<Color> Light 1 - " + myLight.getColor(1) +
                            " Light 2 - " + myLight.getColor(2) + " Light 3 - "  + myLight.getColor(3));

                } catch (OcException e) {
                    Log.e(TAG, e.getMessage());
                }

            }

            @Override
            public void onGetFailed(Throwable throwable) {
                if (throwable instanceof OcException) {
                    OcException ocEx = (OcException) throwable;
                    ErrorCode errCode = ocEx.getErrorCode();
                    //do something based on errorCode
                }
                Log.e(TAG, throwable.toString());
            }
        };

        try {
            if(myLight.getFirstaccess())
                doPutLightRepresentation(0);
            curResource.get(new HashMap<String, String>(), onGetListener);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
        }
    }


}