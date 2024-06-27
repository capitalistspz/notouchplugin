#include <wups.h>
#include <vpad/input.h>
#include <wups/config/WUPSConfigItemBoolean.h>

WUPS_PLUGIN_NAME("No Touch");
WUPS_PLUGIN_DESCRIPTION("Disables touch input from the Wii U GamePad");
WUPS_PLUGIN_VERSION("v1.0");
WUPS_PLUGIN_AUTHOR("capitalistspz");
WUPS_PLUGIN_LICENSE("MIT");

#define DISABLE_TOUCH_CONFIG_ID "disableTouch"

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("notouch");

bool disableTouch = false;

void touchToggled(ConfigItemBoolean *, bool newValue)
{
   WUPSStorageAPI::Store(DISABLE_TOUCH_CONFIG_ID, newValue);
   disableTouch = newValue;
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle) {
   WUPSConfigCategory root = WUPSConfigCategory(rootHandle);

   try {
      root.add(WUPSConfigItemBoolean::Create(DISABLE_TOUCH_CONFIG_ID, "Disable Touch",
                                             false, disableTouch,
                                             &touchToggled));
   } catch (std::exception &e) {
      return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
   }
   return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback()
{
   WUPSStorageAPI::SaveStorage();
}

INITIALIZE_PLUGIN() {
   WUPSConfigAPIOptionsV1 configOptions = {.name = "No Touch"};
   WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback);

   WUPSStorageAPI::GetOrStoreDefault(DISABLE_TOUCH_CONFIG_ID, disableTouch, false);
   WUPSStorageAPI::SaveStorage();
}

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffers, uint32_t count, VPADReadError *outError)
{
   const auto sampleCount =  real_VPADRead(chan, buffers, count, outError);
   for (auto i = 0; i < sampleCount; ++i)
   {
      buffers[i].tpNormal.touched = !disableTouch;
      buffers[i].tpFiltered1.touched = !disableTouch;
      buffers[i].tpFiltered2.touched = !disableTouch;
   }
   return sampleCount;
}

WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);

