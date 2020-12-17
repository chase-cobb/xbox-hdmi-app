#ifndef VERSIONCODE_H
#define VERSIONCODE_H

#include <stdint.h>

class VersionCode
{
private:
    uint8_t m_major;
    uint8_t m_minor;
    uint8_t m_patch;
    bool m_cached;

    void SetMajor(uint8_t major);
    void SetMinor(uint8_t minor);
    void SetPatch(uint8_t patch);
    void SetAsCached();

public:
    VersionCode();

    //Accessors
    uint8_t GetMajor() {return m_major;}
    uint8_t GetMinor() {return m_minor;}
    uint8_t GetPatch() {return m_patch;}
    bool HasBeenCached() {return m_cached;}

    // Mutators
    void SetVersion(uint8_t major, uint8_t minor, uint8_t patch);

    char* GetVersionCodeAsCString();
};

#endif