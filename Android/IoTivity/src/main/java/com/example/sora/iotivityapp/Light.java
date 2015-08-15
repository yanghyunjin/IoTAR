package com.example.sora.iotivityapp;


class Light {
    public String[] m_power = new String[5];
    public int[]    m_brightness = new int[5];
    public int[]    m_color = new int[5];
    public String m_name, m_ip, m_username;
    public boolean m_firstaccess;

    public Light() {
        for (int i = 1; i <= 3; ++i) {
            m_power[i] = "false";
            m_brightness[i] = 0;
            m_color[i] = 0;
        }
        m_name = "iotar's hue";
        m_ip = null;
        m_username = null;
        m_firstaccess = true;
    }

    ///// power /////
    Light setPower(String power, int i) {
        m_power[i] = power;
        return this;
    }

    String getPower(int i) { return m_power[i];}

    ///// brightness /////
    Light setBri(int bri, int i) {
        m_brightness[i] = bri;
        return this;
    }

    int getBri(int i) {
        return m_brightness[i];
    }

    ///// color /////
    Light setColor(int color, int i) {
        m_color[i] = color;
        return this;
    }

    int getColor(int i) {
        return m_color[i];
    }

    ///// name /////
    Light setName(String name) {
        m_name = name;
        return this;
    }

    String getName() {
        return m_name;
    }

    ///// ip /////
    Light setIp(String ip) {
        m_ip = ip;
        return this;
    }

    String getIp() {
        return m_ip;
    }

    ///// username /////
    Light setUsername(String username) {
        m_username = username;
        return this;
    }

    String getUsername() {
        return m_username;
    }

    ///// firstaccess /////
    Light setFirstaccess(boolean firstaccess) {
        m_firstaccess = firstaccess;
        return this;
    }

    boolean getFirstaccess() {
        return m_firstaccess;
    }
}