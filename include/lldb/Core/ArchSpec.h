//===-- ArchSpec.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ArchSpec_h_
#define liblldb_ArchSpec_h_

#if defined(__cplusplus)

#include "lldb/lldb-forward.h"
#include "lldb/Core/ConstString.h"
#include "llvm/ADT/Triple.h"

namespace lldb_private {

struct CoreDefinition;    

//----------------------------------------------------------------------
/// @class ArchSpec ArchSpec.h "lldb/Core/ArchSpec.h"
/// @brief An architecture specification class.
///
/// A class designed to be created from a cpu type and subtype, a
/// string representation, or an llvm::Triple.  Keeping all of the
/// conversions of strings to architecture enumeration values confined
/// to this class allows new architecture support to be added easily.
//----------------------------------------------------------------------
class ArchSpec
{
public:
    enum MIPSSubType
    {
        eMIPSSubType_unknown,
        eMIPSSubType_mips32,
        eMIPSSubType_mips32r2,
        eMIPSSubType_mips32r6,
        eMIPSSubType_mips32el,
        eMIPSSubType_mips32r2el,
        eMIPSSubType_mips32r6el,
        eMIPSSubType_mips64,
        eMIPSSubType_mips64r2,
        eMIPSSubType_mips64r6,
        eMIPSSubType_mips64el,
        eMIPSSubType_mips64r2el,
        eMIPSSubType_mips64r6el,
    };
    
    enum Core
    {
        eCore_arm_generic,
        eCore_arm_armv4,
        eCore_arm_armv4t,
        eCore_arm_armv5,
        eCore_arm_armv5e,
        eCore_arm_armv5t,
        eCore_arm_armv6,
        eCore_arm_armv6m,
        eCore_arm_armv7,
        eCore_arm_armv7f,
        eCore_arm_armv7s,
        eCore_arm_armv7k,
        eCore_arm_armv7m,
        eCore_arm_armv7em,
        eCore_arm_xscale,  

        eCore_thumb,
        eCore_thumbv4t,
        eCore_thumbv5,
        eCore_thumbv5e,
        eCore_thumbv6,
        eCore_thumbv6m,
        eCore_thumbv7,
        eCore_thumbv7s,
        eCore_thumbv7k,
        eCore_thumbv7f,
        eCore_thumbv7m,
        eCore_thumbv7em,
        eCore_arm_arm64,
        eCore_arm_armv8,
        eCore_arm_aarch64,

        eCore_mips32,
        eCore_mips32r2,
        eCore_mips32r3,
        eCore_mips32r5,
        eCore_mips32r6,
        eCore_mips32el,
        eCore_mips32r2el,
        eCore_mips32r3el,
        eCore_mips32r5el,
        eCore_mips32r6el,
        eCore_mips64,
        eCore_mips64r2,
        eCore_mips64r3,
        eCore_mips64r5,
        eCore_mips64r6,
        eCore_mips64el,
        eCore_mips64r2el,
        eCore_mips64r3el,
        eCore_mips64r5el,
        eCore_mips64r6el,

        eCore_ppc_generic,
        eCore_ppc_ppc601,
        eCore_ppc_ppc602,
        eCore_ppc_ppc603,
        eCore_ppc_ppc603e,
        eCore_ppc_ppc603ev,
        eCore_ppc_ppc604,
        eCore_ppc_ppc604e,
        eCore_ppc_ppc620,
        eCore_ppc_ppc750,
        eCore_ppc_ppc7400,
        eCore_ppc_ppc7450,
        eCore_ppc_ppc970,
        
        eCore_ppc64_generic,
        eCore_ppc64_ppc970_64,
        
        eCore_sparc_generic,
        
        eCore_sparc9_generic,
        
        eCore_x86_32_i386,
        eCore_x86_32_i486,
        eCore_x86_32_i486sx,
        eCore_x86_32_i686,
        
        eCore_x86_64_x86_64,
        eCore_x86_64_x86_64h, // Haswell enabled x86_64
        eCore_hexagon_generic,
        eCore_hexagon_hexagonv4,
        eCore_hexagon_hexagonv5,

        eCore_uknownMach32,
        eCore_uknownMach64,

        eCore_kalimba3,
        eCore_kalimba4,
        eCore_kalimba5,

        kNumCores,

        kCore_invalid,
        // The following constants are used for wildcard matching only
        kCore_any,
        kCore_arm_any,
        kCore_ppc_any,
        kCore_ppc64_any,
        kCore_x86_32_any,
        kCore_x86_64_any,
        kCore_hexagon_any,

        kCore_arm_first     = eCore_arm_generic,
        kCore_arm_last      = eCore_arm_xscale,

        kCore_thumb_first   = eCore_thumb,
        kCore_thumb_last    = eCore_thumbv7em,

        kCore_ppc_first     = eCore_ppc_generic,
        kCore_ppc_last      = eCore_ppc_ppc970,

        kCore_ppc64_first   = eCore_ppc64_generic,
        kCore_ppc64_last    = eCore_ppc64_ppc970_64,

        kCore_x86_32_first  = eCore_x86_32_i386,
        kCore_x86_32_last   = eCore_x86_32_i686,

        kCore_x86_64_first  = eCore_x86_64_x86_64,
        kCore_x86_64_last   = eCore_x86_64_x86_64h,

        kCore_hexagon_first  = eCore_hexagon_generic,
        kCore_hexagon_last   = eCore_hexagon_hexagonv5,

        kCore_kalimba_first = eCore_kalimba3,
        kCore_kalimba_last = eCore_kalimba5
    };

    typedef void (* StopInfoOverrideCallbackType)(lldb_private::Thread &thread);

    //------------------------------------------------------------------
    /// Default constructor.
    ///
    /// Default constructor that initializes the object with invalid
    /// cpu type and subtype values.
    //------------------------------------------------------------------
    ArchSpec ();

    //------------------------------------------------------------------
    /// Constructor over triple.
    ///
    /// Constructs an ArchSpec with properties consistent with the given
    /// Triple.
    //------------------------------------------------------------------
    explicit 
    ArchSpec (const llvm::Triple &triple);
    explicit 
    ArchSpec (const char *triple_cstr);
    explicit 
    ArchSpec (const char *triple_cstr, Platform *platform);
    //------------------------------------------------------------------
    /// Constructor over architecture name.
    ///
    /// Constructs an ArchSpec with properties consistent with the given
    /// object type and architecture name.
    //------------------------------------------------------------------
    explicit 
    ArchSpec (ArchitectureType arch_type,
              uint32_t cpu_type,
              uint32_t cpu_subtype);

    //------------------------------------------------------------------
    /// Destructor.
    //------------------------------------------------------------------
    ~ArchSpec ();

    //------------------------------------------------------------------
    /// Assignment operator.
    ///
    /// @param[in] rhs another ArchSpec object to copy.
    ///
    /// @return A const reference to this object.
    //------------------------------------------------------------------
    const ArchSpec&
    operator= (const ArchSpec& rhs);

    static size_t
    AutoComplete (const char *name, 
                  StringList &matches);

    //------------------------------------------------------------------
    /// Returns a static string representing the current architecture.
    ///
    /// @return A static string correcponding to the current
    ///         architecture.
    //------------------------------------------------------------------
    const char *
    GetArchitectureName () const;

    //------------------------------------------------------------------
    /// Clears the object state.
    ///
    /// Clears the object state back to a default invalid state.
    //------------------------------------------------------------------
    void
    Clear ();

    //------------------------------------------------------------------
    /// Returns the size in bytes of an address of the current
    /// architecture.
    ///
    /// @return The byte size of an address of the current architecture.
    //------------------------------------------------------------------
    uint32_t
    GetAddressByteSize () const;

    //------------------------------------------------------------------
    /// Returns a machine family for the current architecture.
    ///
    /// @return An LLVM arch type.
    //------------------------------------------------------------------
    llvm::Triple::ArchType
    GetMachine () const;

    //------------------------------------------------------------------
    /// Returns the distribution id of the architecture.
    ///
    /// This will be something like "ubuntu", "fedora", etc. on Linux.
    ///
    /// @return A ConstString ref containing the distribution id,
    ///         potentially empty.
    //------------------------------------------------------------------
    const ConstString&
    GetDistributionId () const;

    //------------------------------------------------------------------
    /// Set the distribution id of the architecture.
    ///
    /// This will be something like "ubuntu", "fedora", etc. on Linux.
    /// This should be the same value returned by
    /// HostInfo::GetDistributionId ().
    ///------------------------------------------------------------------
    void
    SetDistributionId (const char* distribution_id);

    //------------------------------------------------------------------
    /// Tests if this ArchSpec is valid.
    ///
    /// @return True if the current architecture is valid, false
    ///         otherwise.
    //------------------------------------------------------------------
    bool
    IsValid () const
    {
        return m_core >= eCore_arm_generic && m_core < kNumCores;
    }

    bool
    TripleVendorWasSpecified() const
    {
        return !m_triple.getVendorName().empty();
    }

    bool
    TripleOSWasSpecified() const
    {
        return !m_triple.getOSName().empty();
    }

    //------------------------------------------------------------------
    /// Merges fields from another ArchSpec into this ArchSpec.
    ///
    /// This will use the supplied ArchSpec to fill in any fields of
    /// the triple in this ArchSpec which were unspecified.  This can
    /// be used to refine a generic ArchSpec with a more specific one.
    /// For example, if this ArchSpec's triple is something like
    /// i386-unknown-unknown-unknown, and we have a triple which is
    /// x64-pc-windows-msvc, then merging that triple into this one
    /// will result in the triple i386-pc-windows-msvc.
    ///
    //------------------------------------------------------------------
    void
    MergeFrom(const ArchSpec &other);
    
    //------------------------------------------------------------------
    /// Change the architecture object type and CPU type.
    ///
    /// @param[in] arch_type The object type of this ArchSpec.
    ///
    /// @param[in] cpu The required CPU type.
    ///
    /// @return True if the object and CPU type were successfully set.
    //------------------------------------------------------------------
    bool
    SetArchitecture (ArchitectureType arch_type, 
                     uint32_t cpu,
                     uint32_t sub);

    //------------------------------------------------------------------
    /// Returns the byte order for the architecture specification.
    ///
    /// @return The endian enumeration for the current endianness of
    ///     the architecture specification
    //------------------------------------------------------------------
    lldb::ByteOrder
    GetByteOrder () const;

    //------------------------------------------------------------------
    /// Sets this ArchSpec's byte order.
    ///
    /// In the common case there is no need to call this method as the
    /// byte order can almost always be determined by the architecture.
    /// However, many CPU's are bi-endian (ARM, Alpha, PowerPC, etc)
    /// and the default/assumed byte order may be incorrect.
    //------------------------------------------------------------------
    void
    SetByteOrder (lldb::ByteOrder byte_order)
    {
        m_byte_order = byte_order;
    }

    uint32_t
    GetMinimumOpcodeByteSize() const;

    uint32_t
    GetMaximumOpcodeByteSize() const;

    Core
    GetCore () const
    {
        return m_core;
    }

    uint32_t
    GetMachOCPUType () const;

    uint32_t
    GetMachOCPUSubType () const;

    //------------------------------------------------------------------
    /// Architecture data byte width accessor
    ///
    /// @return the size in 8-bit (host) bytes of a minimum addressable
    /// unit from the Architecture's data bus
    //------------------------------------------------------------------
    uint32_t
    GetDataByteSize() const;

    //------------------------------------------------------------------
    /// Architecture code byte width accessor
    ///
    /// @return the size in 8-bit (host) bytes of a minimum addressable
    /// unit from the Architecture's code bus
    //------------------------------------------------------------------
    uint32_t
    GetCodeByteSize() const;
 
    //------------------------------------------------------------------
    /// Architecture tripple accessor.
    ///
    /// @return A triple describing this ArchSpec.
    //------------------------------------------------------------------
    llvm::Triple &
    GetTriple ()
    {
        return m_triple;
    }

    //------------------------------------------------------------------
    /// Architecture tripple accessor.
    ///
    /// @return A triple describing this ArchSpec.
    //------------------------------------------------------------------
    const llvm::Triple &
    GetTriple () const
    {
        return m_triple;
    }

    //------------------------------------------------------------------
    /// Architecture tripple setter.
    ///
    /// Configures this ArchSpec according to the given triple.  If the 
    /// triple has unknown components in all of the vendor, OS, and 
    /// the optional environment field (i.e. "i386-unknown-unknown")
    /// then default values are taken from the host.  Architecture and
    /// environment components are used to further resolve the CPU type
    /// and subtype, endian characteristics, etc.
    ///
    /// @return A triple describing this ArchSpec.
    //------------------------------------------------------------------
    bool
    SetTriple (const llvm::Triple &triple);

    bool
    SetTriple (const char *triple_cstr);

    bool
    SetTriple (const char *triple_cstr,
               Platform *platform);
    
    //------------------------------------------------------------------
    /// Returns the default endianness of the architecture.
    ///
    /// @return The endian enumeration for the default endianness of
    ///         the architecture.
    //------------------------------------------------------------------
    lldb::ByteOrder
    GetDefaultEndian () const;

    //------------------------------------------------------------------
    /// Returns true if 'char' is a signed type by defualt in the 
    /// architecture false otherwise
    ///
    /// @return True if 'char' is a signed type by default on the
    ///         architecture and false otherwise.
    //------------------------------------------------------------------
    bool
    CharIsSignedByDefault () const;

    //------------------------------------------------------------------
    /// Compare an ArchSpec to another ArchSpec, requiring an exact cpu
    /// type match between them.
    /// e.g. armv7s is not an exact match with armv7 - this would return false
    ///
    /// @return true if the two ArchSpecs match.
    //------------------------------------------------------------------
    bool
    IsExactMatch (const ArchSpec& rhs) const;

    //------------------------------------------------------------------
    /// Compare an ArchSpec to another ArchSpec, requiring a compatible
    /// cpu type match between them.  
    /// e.g. armv7s is compatible with armv7 - this method would return true
    ///
    /// @return true if the two ArchSpecs are compatible
    //------------------------------------------------------------------
    bool
    IsCompatibleMatch (const ArchSpec& rhs) const;

    //------------------------------------------------------------------
    /// Get a stop info override callback for the current architecture.
    ///
    /// Most platform specific code should go in lldb_private::Platform,
    /// but there are cases where no matter which platform you are on
    /// certain things hold true.
    ///
    /// This callback is currently intended to handle cases where a
    /// program stops at an instruction that won't get executed and it
    /// allows the stop reasonm, like "breakpoint hit", to be replaced
    /// with a different stop reason like "no stop reason".
    ///
    /// This is specifically used for ARM in Thumb code when we stop in
    /// an IT instruction (if/then/else) where the instruction won't get
    /// executed and therefore it wouldn't be correct to show the program
    /// stopped at the current PC. The code is generic and applies to all
    /// ARM CPUs.
    ///
    /// @return NULL or a valid stop info override callback for the
    ///     current architecture.
    //------------------------------------------------------------------
    StopInfoOverrideCallbackType
    GetStopInfoOverrideCallback () const;

protected:
    bool
    IsEqualTo (const ArchSpec& rhs, bool exact_match) const;

    llvm::Triple m_triple;
    Core m_core;
    lldb::ByteOrder m_byte_order;

    ConstString m_distribution_id;

    // Called when m_def or m_entry are changed.  Fills in all remaining
    // members with default values.
    void
    CoreUpdated (bool update_triple);
};

//------------------------------------------------------------------
/// @fn bool operator< (const ArchSpec& lhs, const ArchSpec& rhs)
/// @brief Less than operator.
///
/// Tests two ArchSpec objects to see if \a lhs is less than \a
/// rhs.
///
/// @param[in] lhs The Left Hand Side ArchSpec object to compare.
/// @param[in] rhs The Left Hand Side ArchSpec object to compare.
///
/// @return true if \a lhs is less than \a rhs
//------------------------------------------------------------------
bool operator< (const ArchSpec& lhs, const ArchSpec& rhs);

} // namespace lldb_private

#endif  // #if defined(__cplusplus)
#endif  // #ifndef liblldb_ArchSpec_h_
