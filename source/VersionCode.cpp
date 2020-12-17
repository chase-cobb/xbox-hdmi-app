#include "VersionCode.h"

VersionCode::VersionCode()
{
    SetMajor(0);
    SetMinor(0);
    SetPatch(0);
    m_cached = false;
}

void VersionCode::SetMajor(uint8_t major)
{
    m_major = major;
}

void VersionCode::SetMinor(uint8_t minor)
{
    m_minor = minor;
}

void VersionCode::SetPatch(uint8_t patch)
{
    m_patch = patch;
}

void VersionCode::SetAsCached()
{
    m_cached = true;
}

void VersionCode::SetVersion(uint8_t major, uint8_t minor, uint8_t patch)
{
    SetMajor(major);
    SetMinor(minor);
    SetPatch(patch);
    SetAsCached();
}

char* GetVersionCodeAsCString()
{
    // TODO : CC

    return nullptr;
}