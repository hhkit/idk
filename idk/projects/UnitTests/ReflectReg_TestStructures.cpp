#include "pch.h"
#include "TestStructures.h"
#include <ReflectReg_Common.inl>

using namespace idk;

REFLECT_ENUM(testenum, "testenum")
REFLECT_ENUM(testserialize_enum, "testserialize_enum")
REFLECT_ENUM(testenumber, "testenumber")

REFLECT_BEGIN(decltype(reflect_this::hashtable), "hash_table<Guid,string>")
REFLECT_END()
REFLECT_BEGIN(reflect_this, "reflect_this")
REFLECT_VARS(vec, f, container, blaze_it, hashtable)
REFLECT_END()

REFLECT_BEGIN(unknowntest, "unknowntest")
REFLECT_PARENT(reflect_this)
REFLECT_VARS(t)
REFLECT_END()

REFLECT_BEGIN(varianttest, "varianttest")
REFLECT_VARS(uniform)
REFLECT_END()

REFLECT_BEGIN(serialize_this, "serialize_this")
REFLECT_VARS(guid, vec, f)
REFLECT_END()

REFLECT_BEGIN(serialize_this_bs, "serialize_this_bs")
REFLECT_VARS(start, string_vec, mid, hashtable, end)
REFLECT_END()

REFLECT_BEGIN(serialass, "serialass")
REFLECT_PARENT(serialize_this)
REFLECT_VARS(var, x)
REFLECT_END()

REFLECT_BEGIN(structthatcontainsdyn, "structthatcontainsdyn")
REFLECT_VARS(dyn)
REFLECT_END()

REFLECT_BEGIN(structonparse, "structonparse")
REFLECT_VARS(x)
REFLECT_END()