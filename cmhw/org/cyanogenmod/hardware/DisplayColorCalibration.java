/*
 * Copyright (C) 2014, 2016 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.cyanogenmod.hardware;

import android.os.IBinder;
import android.os.Parcel;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.util.Slog;

import org.cyanogenmod.internal.util.FileUtils;

public class DisplayColorCalibration {

    private static final String TAG = "DisplayColorCalibration";

    private static final String RED_FILE = "/sys/class/mdnie/mdnie/r_adj";
    private static final String GREEN_FILE = "/sys/class/mdnie/mdnie/g_adj";
    private static final String BLUE_FILE = "/sys/class/mdnie/mdnie/b_adj";

    private static final int MIN = 1;
    private static final int MAX = 255;

    public static boolean isSupported() {
        // Always true
        return true;
    }

    public static int getMaxValue()  {
        return MAX;
    }

    public static int getMinValue()  {
        return MIN;
    }

    public static int getDefValue() {
        return getMaxValue();
    }

    public static String getCurColors()  {
        return String.format("%s %s %s",
            FileUtils.readOneLine(RED_FILE),
            FileUtils.readOneLine(GREEN_FILE),
            FileUtils.readOneLine(BLUE_FILE));
    }

    public static boolean setColors(String colors) {
        String[] array = colors.split(" ");
        return FileUtils.writeLine(RED_FILE, array[0]) &&
            FileUtils.writeLine(GREEN_FILE, array[1]) &&
            FileUtils.writeLine(BLUE_FILE, array[2]);
    }
}
