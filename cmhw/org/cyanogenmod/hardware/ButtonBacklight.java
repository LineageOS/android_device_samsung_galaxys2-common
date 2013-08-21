/*
 * Copyright (C) 2013 The CyanogenMod Project
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

import org.cyanogenmod.hardware.util.FileUtils;

import java.io.File;

/*
 * Button backlight brightness adjustment
 *
 * Exports methods to get the valid value boundaries, the
 * default and current backlight brightness, and a method to set
 * the backlight brightness.
 *
 * Values exported by min/max can be the direct values required
 * by the hardware, or a local (to ButtonBacklightBrightness) abstraction
 * that's internally converted to something else prior to actual use. The
 * Settings user interface will normalize these into a 0-100 (percentage)
 * scale before showing them to the user, but all values passed to/from
 * the client (Settings) are in this class' scale.
 */

public class ButtonBacklight {

    private static String FILE_TOUCHKEY_DISABLE = "/sys/class/sec/sec_touchkey/force_disable";
    private static String FILE_TOUCHKEY_BRIGHTNESS = "/sys/class/sec/sec_touchkey/brightness";

    /*
     * All HAF classes should export this boolean.
     * Real implementations must, of course, return true
     */

    public static boolean isSupported() {
        File f = new File(FILE_TOUCHKEY_BRIGHTNESS);

        if(f.exists()) {
            return true;
        } else {
            return false;
        }
    }

    /*
     * Set the button backlight brightness to given integer input. That'll
     * be a value between the boundaries set by get(Max|Min)Intensity
     * (see below), and it's meant to be locally interpreted/used.
     */

    public static boolean setBrightness(int brightness)  {
        boolean ret = false;

        if (brightness >= 1) {
            FileUtils.writeLine(FILE_TOUCHKEY_DISABLE, "0");
            ret = FileUtils.writeLine(FILE_TOUCHKEY_BRIGHTNESS, "1");
        } else {
            ret = FileUtils.writeLine(FILE_TOUCHKEY_BRIGHTNESS, "2");
            FileUtils.writeLine(FILE_TOUCHKEY_DISABLE, "1");
        }

        return ret;
    }

    /*
     * What's the maximum integer value we take for setBrightness()?
     */

    public static int getMaxBrightness()  {
        return 1;
    }

    /*
     * What's the minimum integer value we take for setBrightness()?
     */

    public static int getMinBrightness()  {
        return 0;
    }

    /*
     * What's the current brightness value?
     */

    public static int getCurBrightness() {
        if (Integer.parseInt(FileUtils.readOneLine(FILE_TOUCHKEY_BRIGHTNESS)) == 1) {
            return 1;
        } else {
            return 0;
        }
    }

    /*
     * What's the shipping brightness value?
     */

    public static int getDefaultBrightness()  {
        return 1;
    }
}
