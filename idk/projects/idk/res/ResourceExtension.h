#pragma once

#define EXTENSION(EXT) \
static constexpr string_view ext = EXT; \
static_assert(ext[0] == '.', "Extensions must start with a period!");
