//===-- HostInfoMacOSX.mm ---------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/Host/macosx/HostInfoMacOSX.h"
#include "lldb/Interpreter/Args.h"

#include "lldb/Utility/SafeMachO.h"

// C++ Includes
#include <string>

// C inclues
#include <sys/sysctl.h>
#include <sys/types.h>

// Objective C/C++ includes
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <objc/objc-auto.h>

using namespace lldb_private;

bool
HostInfoMacOSX::GetOSBuildString(std::string &s)
{
    int mib[2] = {CTL_KERN, KERN_OSVERSION};
    char cstr[PATH_MAX];
    size_t cstr_len = sizeof(cstr);
    if (::sysctl(mib, 2, cstr, &cstr_len, NULL, 0) == 0)
    {
        s.assign(cstr, cstr_len);
        return true;
    }

    s.clear();
    return false;
}

bool
HostInfoMacOSX::GetOSKernelDescription(std::string &s)
{
    int mib[2] = {CTL_KERN, KERN_VERSION};
    char cstr[PATH_MAX];
    size_t cstr_len = sizeof(cstr);
    if (::sysctl(mib, 2, cstr, &cstr_len, NULL, 0) == 0)
    {
        s.assign(cstr, cstr_len);
        return true;
    }
    s.clear();
    return false;
}

bool
HostInfoMacOSX::GetOSVersion(uint32_t &major, uint32_t &minor, uint32_t &update)
{
    static uint32_t g_major = 0;
    static uint32_t g_minor = 0;
    static uint32_t g_update = 0;

    if (g_major == 0)
    {
        @autoreleasepool
        {
            NSDictionary *version_info = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
            NSString *version_value = [version_info objectForKey:@"ProductVersion"];
            const char *version_str = [version_value UTF8String];
            if (version_str)
                Args::StringToVersion(version_str, g_major, g_minor, g_update);
        }
    }

    if (g_major != 0)
    {
        major = g_major;
        minor = g_minor;
        update = g_update;
        return true;
    }
    return false;
}

void
HostInfoMacOSX::ComputeHostArchitectureSupport(ArchSpec &arch_32, ArchSpec &arch_64)
{
    // All apple systems support 32 bit execution.
    uint32_t cputype, cpusubtype;
    uint32_t is_64_bit_capable = false;
    size_t len = sizeof(cputype);
    ArchSpec host_arch;
    // These will tell us about the kernel architecture, which even on a 64
    // bit machine can be 32 bit...
    if (::sysctlbyname("hw.cputype", &cputype, &len, NULL, 0) == 0)
    {
        len = sizeof(cpusubtype);
        if (::sysctlbyname("hw.cpusubtype", &cpusubtype, &len, NULL, 0) != 0)
            cpusubtype = CPU_TYPE_ANY;

        len = sizeof(is_64_bit_capable);
        ::sysctlbyname("hw.cpu64bit_capable", &is_64_bit_capable, &len, NULL, 0);

        if (is_64_bit_capable)
        {
            if (cputype & CPU_ARCH_ABI64)
            {
                // We have a 64 bit kernel on a 64 bit system
                arch_64.SetArchitecture(eArchTypeMachO, cputype, cpusubtype);
            }
            else
            {
                // We have a 64 bit kernel that is returning a 32 bit cputype, the
                // cpusubtype will be correct as if it were for a 64 bit architecture
                arch_64.SetArchitecture(eArchTypeMachO, cputype | CPU_ARCH_ABI64, cpusubtype);
            }

            // Now we need modify the cpusubtype for the 32 bit slices.
            uint32_t cpusubtype32 = cpusubtype;
#if defined(__i386__) || defined(__x86_64__)
            if (cpusubtype == CPU_SUBTYPE_486 || cpusubtype == CPU_SUBTYPE_X86_64_H)
                cpusubtype32 = CPU_SUBTYPE_I386_ALL;
#elif defined(__arm__) || defined(__arm64__) || defined(__aarch64__)
            if (cputype == CPU_TYPE_ARM || cputype == CPU_TYPE_ARM64)
                cpusubtype32 = CPU_SUBTYPE_ARM_V7S;
#endif
            arch_32.SetArchitecture(eArchTypeMachO, cputype & ~(CPU_ARCH_MASK), cpusubtype32);

            if (cputype == CPU_TYPE_ARM || cputype == CPU_TYPE_ARM64)
            {
                arch_32.GetTriple().setOS(llvm::Triple::IOS);
                arch_64.GetTriple().setOS(llvm::Triple::IOS);
            }
            else
            {
                arch_32.GetTriple().setOS(llvm::Triple::MacOSX);
                arch_64.GetTriple().setOS(llvm::Triple::MacOSX);
            }
        }
        else
        {
            // We have a 32 bit kernel on a 32 bit system
            arch_32.SetArchitecture(eArchTypeMachO, cputype, cpusubtype);
            arch_64.Clear();
        }
    }
}