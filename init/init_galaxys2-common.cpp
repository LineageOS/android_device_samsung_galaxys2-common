#include <stdlib.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#include "vendor_init.h"

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;
using android::base::ReadFileToString;
using android::base::Trim;

#define MODEL_NAME_LEN 5
#define SERIAL_NUMBER_FILE "/sys/class/android_usb/android0/iSerial"

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const system_prop[], char const vendor_prop[], char const value[])
{
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

void vendor_load_properties()
{
    const std::string bootloader = GetProperty("ro.bootloader", "");
    const std::string platform = GetProperty("ro.board.platform", "");
    const std::string model = bootloader.substr(0, MODEL_NAME_LEN);

    char const *serial_number_file = SERIAL_NUMBER_FILE;
    std::string serial_number;

    if (platform != ANDROID_TARGET)
        return;

    if (ReadFileToString(serial_number_file, &serial_number)) {
        serial_number = Trim(serial_number);
        property_override("ro.serialno", serial_number.c_str());
    }

    if (model == "I9100") {
        /* GT-I9100 */
        property_override_dual("ro.build.fingerprint", "ro.vendor.build.fingerprint", "samsung/GT-I9100/GT-I9100:4.1.2/JZO54K/I9100XWMS2:user/release-keys");
        property_override_dual("ro.product.model", "ro.product.vendor.model", "GT-I9100");
        property_override_dual("ro.product.device", "ro.product.vendor.device", "GT-I9100");
   }

    const std::string device = android::base::GetProperty("ro.product.device", "");
    LOG(INFO) << "Found bootloader " << bootloader.c_str() << ". " << "Setting build properties for " << device.c_str() << ".\n";
}

