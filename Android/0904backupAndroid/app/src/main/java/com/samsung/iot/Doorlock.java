package com.samsung.iot;
/**
 * Created by sora on 15. 8. 20.
 */
public class Doorlock {

    private String mKey;
    private String mName;

    Doorlock() {
        mKey = "";
        mName = "";
    }

    String getName() {
        return mName;
    }
    void setName(String name) {
        mName = name;
    }

    String getKey() {
        return mKey;
    }
    Doorlock setKey(String key) {
        mKey = key;
        return this;
    }
}