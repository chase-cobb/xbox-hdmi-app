#ifndef XBOXHDMI_H
#define XBOXHDMI_H

class VersionCode;

class XboxHDMI
{
public:

static XboxHDMI* GetInstance();

void CacheInfo();

// is HW detected
bool HardwareIsPresent();

// firmware version
VersionCode* GetFirmwareVersion();

// kernel patch version
VersionCode* GetKernelPatchVersion();

// get bootloader mode
// get compile time
// get programming status
// get program full
// get programming error

// set bootloader mode

// write Page CRC
// write Page Data

private:
static XboxHDMI* m_instance;

// Any members that can be cached and should never change
// while the app is running.
bool m_isHardwareDetected;
VersionCode* m_firmwareVersion;
VersionCode* m_kernelPatchVersion;

XboxHDMI();
XboxHDMI(const XboxHDMI& copy);
XboxHDMI operator=(const XboxHDMI& copy);

~XboxHDMI();

};

#endif