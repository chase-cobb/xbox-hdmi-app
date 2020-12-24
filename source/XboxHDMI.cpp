#include "XboxHDMI.h"
#include "VersionCode.h"
#include "common.h"
#include "i2c_map.h"
#include <xboxkrnl/xboxkrnl.h>

XboxHDMI::XboxHDMI()
{
    m_isHardwareDetected = false;
    m_firmwareVersion = nullptr;
    m_kernelPatchVersion = nullptr;
}

XboxHDMI::~XboxHDMI()
{
    // TODO : clean up any allocated memory!!

    if(m_firmwareVersion)
    {
        delete m_firmwareVersion;
    }
}

XboxHDMI::XboxHDMI(const XboxHDMI& copy)
{
    //Unused
}

XboxHDMI XboxHDMI::operator=(const XboxHDMI& copy)
{
    // Unused
    return *this;
}

XboxHDMI* XboxHDMI::GetInstance()
{
    if(m_instance == nullptr)
    {
        m_instance = new XboxHDMI();
    }
    return m_instance;
}

void XboxHDMI::CacheInfo()
{
    m_firmwareVersion = GetFirmwareVersion();
    m_kernelPatchVersion = GetKernelPatchVersion();

    m_isHardwareDetected = m_firmwareVersion->HasBeenCached();
}

bool XboxHDMI::HardwareIsPresent()
{
    return m_isHardwareDetected;
}

VersionCode* XboxHDMI::GetFirmwareVersion()
{
    // Attempt to recieve firmware version
    ULONG smbus_read;

    uint8_t major, minor, patch;

    if(HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 0, false, &smbus_read) == 0) {
        major = (uint8_t)smbus_read;

        HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 1, false, &smbus_read);
        minor = (uint8_t)smbus_read;

        HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 2, false, &smbus_read);
        patch = (uint8_t)smbus_read;

        m_firmwareVersion->SetVersion(major, minor, patch);

        if (m_firmwareVersion == nullptr)
        {
            m_firmwareVersion = new VersionCode();
        }

        // Firmware 1.0.0 will incorrectly report 0.0.0, so let's fix that.
        if(major == 0 && minor == 0 && patch == 0) {
        major = 1;
        }

        m_firmwareVersion->SetVersion(major, minor, patch);
    }

    return m_firmwareVersion;
}

VersionCode* XboxHDMI::GetKernelPatchVersion()
{
    // TODO : 

    return nullptr;
}

bool XboxHDMI::GetBootloaderMode(ULONG* bootMode)
{
    return (HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_BOOT_MODE, false, bootMode) == 0);
}