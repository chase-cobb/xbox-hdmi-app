#ifndef XBOXHDMI_H
#define XBOXHDMI_H

class VersionCode;

// TODO : UpdateStatus struct to contain current state of update
//        and the current progress in %

// TODO : Create pure virtual interface to allow apps to handle
//        the firmware update process via callbacks

class XboxHDMI
{
public:

// TODO : create a state tracking system for firmware updates

static XboxHDMI* GetInstance();

/**
 * Performs caching of any data that doesn't change
 * frequently.
 *
 * @note This should be run at initialization of the
 *       program, and after updating the firmware.
 */
void CacheInfo();

/**
 * Checks to see if XboxHDMI hardware is present
 *
 * @return True if the XboxHDMI hardware is detected.
 * @note Make sure to call CacheInfo() before this function.
 */
bool HardwareIsPresent();


/**
 * Checks to see if XboxHDMI hardware is present
 *
 * @return True if the XboxHDMI hardware is detected.
 * @note Make sure to call CacheInfo() before this function.
 */
VersionCode* GetFirmwareVersion();

// kernel patch version
VersionCode* GetKernelPatchVersion();

// get bootloader mode
bool GetBootloaderMode(ULONG* bootMode);

// get compile time
// get programming status
// get program full
// get programming error

// set bootloader mode
bool SetBootloaderMode(ULONG dataValue, bool writeWordValue = false);

// write Page CRC
// write Page Data


bool CheckForFirmwareUpdate();


private:
static XboxHDMI* m_instance;

// Any members that can be cached and should never change
// while the app is running.
bool m_isHardwareDetected;
VersionCode* m_firmwareVersion;
VersionCode* m_kernelPatchVersion;

bool m_chachedDataIsDirty;

XboxHDMI();
XboxHDMI(const XboxHDMI& copy);
XboxHDMI operator=(const XboxHDMI& copy);

~XboxHDMI();

};

#endif