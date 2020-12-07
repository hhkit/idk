#pragma once
#include <idk.h>
#include <util/enum.h>
#include <res/Guid.h>
#include <reflect/reflect.h>

namespace idk
{
    ENUM(testenum, char, IVAN = 5, IS, A, WEEB)
    ENUM(testserialize_enum, int, PI, TAU)
    ENUM(testenumber, int, One = 1, Two, Three)

    struct reflect_this
    {
        vec4 vec;
        int f = 69;
        vector<string> container;
        double blaze_it = 420.0;
        hash_table<Guid, string> hashtable;
    };

    struct unknowntest : reflect_this
    {
        testenum t = testenum::A;
    };

    struct varianttest
    {
        UniformInstanceValue uniform;
    };

    struct serialize_this
    {
        Guid guid;
        vec4 vec;
        int f = 69;
    };

    struct serialize_this_bs
    {
        int start = 0;
        vector<string> string_vec;
        float mid = 1.0f;
        hash_table<Guid, string> hashtable;
        char end = '2';
    };

    struct structthatcontainsdyn
    {
        reflect::dynamic dyn;
    };

    struct serialass : serialize_this
    {
        UniformInstanceValue var;
        testserialize_enum x = testserialize_enum::TAU;
    };

    struct structonparse
    {
        int x = 0;
        void on_parse()
        {
            x = 999;
        }
    };
}