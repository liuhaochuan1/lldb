//===-- LibStdcpp.cpp ---------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lldb/lldb-python.h"

#include "lldb/DataFormatters/CXXFormatterFunctions.h"

#include "lldb/Core/DataBufferHeap.h"
#include "lldb/Core/Error.h"
#include "lldb/Core/Stream.h"
#include "lldb/Core/ValueObject.h"
#include "lldb/Core/ValueObjectConstResult.h"
#include "lldb/Host/Endian.h"
#include "lldb/Symbol/ClangASTContext.h"
#include "lldb/Target/ObjCLanguageRuntime.h"
#include "lldb/Target/Target.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::formatters;

/*
 (std::_Rb_tree_iterator<std::pair<const int, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >) ibeg = {
 (_Base_ptr) _M_node = 0x0000000100103910 {
 (std::_Rb_tree_color) _M_color = _S_black
 (std::_Rb_tree_node_base::_Base_ptr) _M_parent = 0x00000001001038c0
 (std::_Rb_tree_node_base::_Base_ptr) _M_left = 0x0000000000000000
 (std::_Rb_tree_node_base::_Base_ptr) _M_right = 0x0000000000000000
 }
 }
 */

lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::LibstdcppMapIteratorSyntheticFrontEnd (lldb::ValueObjectSP valobj_sp) :
    SyntheticChildrenFrontEnd(*valobj_sp.get()),
    m_exe_ctx_ref(),
    m_pair_address(0),
    m_pair_type(),
    m_options(),
    m_pair_sp()
{
    if (valobj_sp)
        Update();
    m_options.SetCoerceToId(false);
    m_options.SetUnwindOnError(true);
    m_options.SetKeepInMemory(true);
    m_options.SetUseDynamic(lldb::eDynamicCanRunTarget);
}

bool
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::Update()
{
    ValueObjectSP valobj_sp = m_backend.GetSP();
    if (!valobj_sp)
        return false;
    
    TargetSP target_sp(valobj_sp->GetTargetSP());
    
    if (!target_sp)
        return false;
    
    bool is_64bit = (target_sp->GetArchitecture().GetAddressByteSize() == 8);
    
    if (!valobj_sp)
        return false;
    m_exe_ctx_ref = valobj_sp->GetExecutionContextRef();
    
    ValueObjectSP _M_node_sp(valobj_sp->GetChildMemberWithName(ConstString("_M_node"), true));
    if (!_M_node_sp)
        return false;
    
    m_pair_address = _M_node_sp->GetValueAsUnsigned(0);
    if (m_pair_address == 0)
        return false;
    
    m_pair_address += (is_64bit ? 32 : 16);
    
    ClangASTType my_type(valobj_sp->GetClangType());
    if (my_type.GetNumTemplateArguments() >= 1)
    {
        TemplateArgumentKind kind;
        ClangASTType pair_type = my_type.GetTemplateArgument(0, kind);
        if (kind != eTemplateArgumentKindType && kind != eTemplateArgumentKindTemplate && kind != eTemplateArgumentKindTemplateExpansion)
            return false;
        m_pair_type = pair_type;
    }
    else
        return false;
    
    return true;
}

size_t
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::CalculateNumChildren ()
{
    return 2;
}

lldb::ValueObjectSP
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::GetChildAtIndex (size_t idx)
{
    if (m_pair_address != 0 && m_pair_type)
    {
        if (!m_pair_sp)
            m_pair_sp = CreateValueObjectFromAddress("pair", m_pair_address, m_exe_ctx_ref, m_pair_type);
        if (m_pair_sp)
            return m_pair_sp->GetChildAtIndex(idx, true);
    }
    return lldb::ValueObjectSP();
}

bool
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::MightHaveChildren ()
{
    return true;
}

size_t
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::GetIndexOfChildWithName (const ConstString &name)
{
    if (name == ConstString("first"))
        return 0;
    if (name == ConstString("second"))
        return 1;
    return UINT32_MAX;
}

lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEnd::~LibstdcppMapIteratorSyntheticFrontEnd ()
{}

SyntheticChildrenFrontEnd*
lldb_private::formatters::LibstdcppMapIteratorSyntheticFrontEndCreator (CXXSyntheticChildren*, lldb::ValueObjectSP valobj_sp)
{
    if (!valobj_sp)
        return NULL;
    return (new LibstdcppMapIteratorSyntheticFrontEnd(valobj_sp));
}

/*
 (lldb) fr var ibeg --ptr-depth 1
 (__gnu_cxx::__normal_iterator<int *, std::vector<int, std::allocator<int> > >) ibeg = {
 _M_current = 0x00000001001037a0 {
 *_M_current = 1
 }
 }
 */

SyntheticChildrenFrontEnd*
lldb_private::formatters::LibStdcppVectorIteratorSyntheticFrontEndCreator (CXXSyntheticChildren*, lldb::ValueObjectSP valobj_sp)
{
    static ConstString g_item_name;
    if (!g_item_name)
        g_item_name.SetCString("_M_current");
    if (!valobj_sp)
        return NULL;
    return (new VectorIteratorSyntheticFrontEnd(valobj_sp,g_item_name));
}
