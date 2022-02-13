/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/enum.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2019-2022, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

#if !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) && !defined(_CPPUNWIND)
#  define ENUM_HPP_NO_EXCEPTIONS
#endif

namespace enum_hpp
{
    constexpr std::size_t invalid_index = std::size_t(-1);
    constexpr std::string_view empty_string = std::string_view();

    class exception final : public std::runtime_error {
    public:
        explicit exception(const char* what)
        : std::runtime_error(what) {}
    };
}

namespace enum_hpp
{
    template < typename Enum >
    using traits_t = decltype(enum_hpp_adl_find_registered_traits(std::declval<Enum>()));

    template < typename Enum >
    using underlying_type = typename traits_t<Enum>::underlying_type;

    template < typename Enum >
    constexpr std::size_t size() noexcept {
        return traits_t<Enum>::size;
    }

    template < typename Enum >
    constexpr const std::array<Enum, size<Enum>()>& values() noexcept {
        return traits_t<Enum>::values;
    }

    template < typename Enum >
    constexpr const std::array<std::string_view, size<Enum>()>& names() noexcept {
        return traits_t<Enum>::names;
    }

    template < typename Enum >
    constexpr typename traits_t<Enum>::underlying_type to_underlying(Enum e) noexcept {
        return traits_t<Enum>::to_underlying(e);
    }

    template < typename Enum >
    constexpr std::optional<std::string_view> to_string(Enum e) noexcept {
        return traits_t<Enum>::to_string(e);
    }

    template < typename Enum >
    constexpr std::string_view to_string_or_empty(Enum e) noexcept {
        return traits_t<Enum>::to_string_or_empty(e);
    }

    template < typename Enum >
    std::string_view to_string_or_throw(Enum e) {
        return traits_t<Enum>::to_string_or_throw(e);
    }

    template < typename Enum >
    constexpr std::optional<Enum> from_string(std::string_view name) noexcept {
        return traits_t<Enum>::from_string(name);
    }

    template < typename Enum >
    constexpr Enum from_string_or_default(std::string_view name, Enum def) noexcept {
        return traits_t<Enum>::from_string_or_default(name, def);
    }

    template < typename Enum >
    Enum from_string_or_throw(std::string_view name) {
        return traits_t<Enum>::from_string_or_throw(name);
    }

    template < typename Enum >
    constexpr std::optional<std::size_t> to_index(Enum e) noexcept {
        return traits_t<Enum>::to_index(e);
    }

    template < typename Enum >
    constexpr std::size_t to_index_or_invalid(Enum e) noexcept {
        return traits_t<Enum>::to_index_or_invalid(e);
    }

    template < typename Enum >
    std::size_t to_index_or_throw(Enum e) {
        return traits_t<Enum>::to_index_or_throw(e);
    }

    template < typename Enum >
    constexpr std::optional<Enum> from_index(std::size_t index) noexcept {
        return traits_t<Enum>::from_index(index);
    }

    template < typename Enum >
    constexpr Enum from_index_or_default(std::size_t index, Enum def) noexcept {
        return traits_t<Enum>::from_index_or_default(index, def);
    }

    template < typename Enum >
    Enum from_index_or_throw(std::size_t index) {
        return traits_t<Enum>::from_index_or_throw(index);
    }
}

namespace enum_hpp::detail
{
    inline void throw_exception_with [[noreturn]] (const char* what) {
    #ifndef ENUM_HPP_NO_EXCEPTIONS
        throw ::enum_hpp::exception(what);
    #else
        (void)what;
        std::abort();
    #endif
    }

    template < typename Enum >
    struct ignore_assign final {
        Enum value;

        constexpr explicit ignore_assign(Enum value) noexcept
        : value(value) {}

        template < typename Other >
        // NOLINTNEXTLINE(readability-named-parameter)
        constexpr ignore_assign& operator=(const Other&) noexcept {
            return *this;
        }
    };

    constexpr bool is_end_of_name(char ch) noexcept {
        switch ( ch ) {
            case ' ':
            case '=':
            case '\r':
            case '\n':
            case '\t':
                return true;
            default:
                return false;
        }
    }

    constexpr std::string_view trim_raw_name(std::string_view raw_name) noexcept {
        for ( std::size_t i = 0; i < raw_name.size(); ++i ) {
            if ( is_end_of_name(raw_name[i]) ) {
                return raw_name.substr(0, i);
            }
        }
        return raw_name;
    }
}

//
// ENUM_HPP_GENERATE_FIELDS
//

#define ENUM_HPP_GENERATE_FIELDS_OP(d, i, x)\
    x,

#define ENUM_HPP_GENERATE_FIELDS(Fields)\
    ENUM_HPP_PP_SEQ_FOR_EACH(ENUM_HPP_GENERATE_FIELDS_OP, _, Fields)

//
// ENUM_HPP_GENERATE_VALUES
//

#define ENUM_HPP_GENERATE_VALUES_OP(Enum, i, x)\
    ((::enum_hpp::detail::ignore_assign<Enum>)Enum::x).value,

#define ENUM_HPP_GENERATE_VALUES(Enum, Fields)\
    ENUM_HPP_PP_SEQ_FOR_EACH(ENUM_HPP_GENERATE_VALUES_OP, Enum, Fields)

//
// ENUM_HPP_GENERATE_NAMES
//

#define ENUM_HPP_GENERATE_NAMES_OP(d, i, x)\
    ::enum_hpp::detail::trim_raw_name(ENUM_HPP_PP_STRINGIZE(x)),

#define ENUM_HPP_GENERATE_NAMES(Fields)\
    ENUM_HPP_PP_SEQ_FOR_EACH(ENUM_HPP_GENERATE_NAMES_OP, _, Fields)

//
// ENUM_HPP_GENERATE_VALUE_TO_NAME_CASES
//

#define ENUM_HPP_GENERATE_VALUE_TO_NAME_CASES_OP(Enum, i, x)\
    case values[i]: return names[i];

#define ENUM_HPP_GENERATE_VALUE_TO_NAME_CASES(Enum, Fields)\
    ENUM_HPP_PP_SEQ_FOR_EACH(ENUM_HPP_GENERATE_VALUE_TO_NAME_CASES_OP, Enum, Fields)

//
// ENUM_HPP_GENERATE_VALUE_TO_INDEX_CASES
//

#define ENUM_HPP_GENERATE_VALUE_TO_INDEX_CASES_OP(Enum, i, x)\
    case values[i]: return i;

#define ENUM_HPP_GENERATE_VALUE_TO_INDEX_CASES(Enum, Fields)\
    ENUM_HPP_PP_SEQ_FOR_EACH(ENUM_HPP_GENERATE_VALUE_TO_INDEX_CASES_OP, Enum, Fields)

//
// ENUM_HPP_DECL
//

#define ENUM_HPP_DECL(Enum, Type, Fields)\
    enum Enum : Type {\
        ENUM_HPP_GENERATE_FIELDS(Fields)\
    };\
    ENUM_HPP_TRAITS_DECL(Enum, Fields)

//
// ENUM_HPP_CLASS_DECL
//

#define ENUM_HPP_CLASS_DECL(Enum, Type, Fields)\
    enum class Enum : Type {\
        ENUM_HPP_GENERATE_FIELDS(Fields)\
    };\
    ENUM_HPP_TRAITS_DECL(Enum, Fields)

//
// ENUM_HPP_TRAITS_DECL
//

#define ENUM_HPP_TRAITS_DECL(Enum, Fields)\
    struct Enum##_traits final {\
    private:\
        enum enum_names_for_this_score_ {\
            ENUM_HPP_GENERATE_FIELDS(Fields)\
        };\
    public:\
        using enum_type = Enum;\
        using underlying_type = std::underlying_type_t<enum_type>;\
        static constexpr std::size_t size = ENUM_HPP_PP_SEQ_SIZE(Fields);\
        \
        static constexpr const std::array<enum_type, size> values = {\
            { ENUM_HPP_GENERATE_VALUES(Enum, Fields) }\
        };\
        \
        static constexpr const std::array<std::string_view, size> names = {\
            { ENUM_HPP_GENERATE_NAMES(Fields) }\
        };\
    public:\
        [[maybe_unused]] static constexpr underlying_type to_underlying(enum_type e) noexcept {\
            return static_cast<underlying_type>(e);\
        }\
        [[maybe_unused]] static constexpr std::optional<std::string_view> to_string(enum_type e) noexcept {\
            switch ( e ) {\
                ENUM_HPP_GENERATE_VALUE_TO_NAME_CASES(Enum, Fields)\
                default: return std::nullopt;\
            }\
        }\
        [[maybe_unused]] static constexpr std::string_view to_string_or_empty(enum_type e) noexcept {\
            if ( auto s = to_string(e) ) {\
                return *s;\
            }\
            return ::enum_hpp::empty_string;\
        }\
        [[maybe_unused]] static std::string_view to_string_or_throw(enum_type e) {\
            if ( auto s = to_string(e) ) {\
                return *s;\
            }\
            ::enum_hpp::detail::throw_exception_with(#Enum "_traits::to_string_or_throw(): invalid argument");\
        }\
        [[maybe_unused]] static constexpr std::optional<enum_type> from_string(std::string_view name) noexcept {\
            for ( std::size_t i = 0; i < size; ++i) {\
                if ( name == names[i] ) {\
                    return values[i];\
                }\
            }\
            return std::nullopt;\
        }\
        [[maybe_unused]] static constexpr enum_type from_string_or_default(std::string_view name, enum_type def) noexcept {\
            if ( auto e = from_string(name) ) {\
                return *e;\
            }\
            return def;\
        }\
        [[maybe_unused]] static enum_type from_string_or_throw(std::string_view name) {\
            if ( auto e = from_string(name) ) {\
                return *e;\
            }\
            ::enum_hpp::detail::throw_exception_with(#Enum "_traits::from_string_or_throw(): invalid argument");\
        }\
        [[maybe_unused]] static constexpr std::optional<std::size_t> to_index(enum_type e) noexcept {\
            switch ( e ) {\
                ENUM_HPP_GENERATE_VALUE_TO_INDEX_CASES(Enum, Fields)\
                default: return std::nullopt;\
            }\
        }\
        [[maybe_unused]] static constexpr std::size_t to_index_or_invalid(enum_type e) noexcept {\
            if ( auto i = to_index(e) ) {\
                return *i;\
            }\
            return ::enum_hpp::invalid_index;\
        }\
        [[maybe_unused]] static std::size_t to_index_or_throw(enum_type e) {\
            if ( auto i = to_index(e) ) {\
                return *i;\
            }\
            ::enum_hpp::detail::throw_exception_with(#Enum "_traits::to_index_or_throw(): invalid argument");\
        }\
        [[maybe_unused]] static constexpr std::optional<enum_type> from_index(std::size_t index) noexcept {\
            if ( index < size ) {\
                return values[index];\
            }\
            return std::nullopt;\
        }\
        [[maybe_unused]] static constexpr enum_type from_index_or_default(std::size_t index, enum_type def) noexcept {\
            if ( auto e = from_index(index) ) {\
                return *e;\
            }\
            return def;\
        }\
        [[maybe_unused]] static enum_type from_index_or_throw(std::size_t index) {\
            if ( auto e = from_index(index) ) {\
                return *e;\
            }\
            ::enum_hpp::detail::throw_exception_with(#Enum "_traits::from_index_or_throw(): invalid argument");\
        }\
    };

//
// ENUM_HPP_REGISTER_TRAITS
//

#define ENUM_HPP_REGISTER_TRAITS(Enum)\
    constexpr Enum##_traits enum_hpp_adl_find_registered_traits [[maybe_unused]] (Enum) noexcept {\
        return Enum##_traits{};\
    }

// -----------------------------------------------------------------------------
//
// ENUM_HPP_PP
//
// -----------------------------------------------------------------------------

//
// ENUM_HPP_PP_CAT
//

#define ENUM_HPP_PP_CAT(x, y) ENUM_HPP_PP_CAT_I(x, y)
#define ENUM_HPP_PP_CAT_I(x, y) x ## y

//
// ENUM_HPP_PP_STRINGIZE
//

#define ENUM_HPP_PP_STRINGIZE(x) ENUM_HPP_PP_STRINGIZE_I(x)
#define ENUM_HPP_PP_STRINGIZE_I(x) #x

//
// ENUM_HPP_PP_SEQ_HEAD
//

#define ENUM_HPP_PP_SEQ_HEAD(seq) ENUM_HPP_PP_SEQ_HEAD_II((ENUM_HPP_PP_SEQ_HEAD_I seq))
#define ENUM_HPP_PP_SEQ_HEAD_I(x) x, ENUM_HPP_PP_NOTHING
#define ENUM_HPP_PP_SEQ_HEAD_II(p) ENUM_HPP_PP_SEQ_HEAD_III p
#define ENUM_HPP_PP_SEQ_HEAD_III(x, _) x

//
// ENUM_HPP_PP_SEQ_TAIL
//

#define ENUM_HPP_PP_SEQ_TAIL(seq) ENUM_HPP_PP_SEQ_TAIL_I seq
#define ENUM_HPP_PP_SEQ_TAIL_I(_)

//
// ENUM_HPP_PP_INC
//

#define ENUM_HPP_PP_INC(x) ENUM_HPP_PP_INC_I(x)
#define ENUM_HPP_PP_INC_I(x) ENUM_HPP_PP_INC_ ## x

#define ENUM_HPP_PP_INC_0 1
#define ENUM_HPP_PP_INC_1 2
#define ENUM_HPP_PP_INC_2 3
#define ENUM_HPP_PP_INC_3 4
#define ENUM_HPP_PP_INC_4 5
#define ENUM_HPP_PP_INC_5 6
#define ENUM_HPP_PP_INC_6 7
#define ENUM_HPP_PP_INC_7 8
#define ENUM_HPP_PP_INC_8 9
#define ENUM_HPP_PP_INC_9 10
#define ENUM_HPP_PP_INC_10 11
#define ENUM_HPP_PP_INC_11 12
#define ENUM_HPP_PP_INC_12 13
#define ENUM_HPP_PP_INC_13 14
#define ENUM_HPP_PP_INC_14 15
#define ENUM_HPP_PP_INC_15 16
#define ENUM_HPP_PP_INC_16 17
#define ENUM_HPP_PP_INC_17 18
#define ENUM_HPP_PP_INC_18 19
#define ENUM_HPP_PP_INC_19 20
#define ENUM_HPP_PP_INC_20 21
#define ENUM_HPP_PP_INC_21 22
#define ENUM_HPP_PP_INC_22 23
#define ENUM_HPP_PP_INC_23 24
#define ENUM_HPP_PP_INC_24 25
#define ENUM_HPP_PP_INC_25 26
#define ENUM_HPP_PP_INC_26 27
#define ENUM_HPP_PP_INC_27 28
#define ENUM_HPP_PP_INC_28 29
#define ENUM_HPP_PP_INC_29 30
#define ENUM_HPP_PP_INC_30 31
#define ENUM_HPP_PP_INC_31 32
#define ENUM_HPP_PP_INC_32 33
#define ENUM_HPP_PP_INC_33 34
#define ENUM_HPP_PP_INC_34 35
#define ENUM_HPP_PP_INC_35 36
#define ENUM_HPP_PP_INC_36 37
#define ENUM_HPP_PP_INC_37 38
#define ENUM_HPP_PP_INC_38 39
#define ENUM_HPP_PP_INC_39 40
#define ENUM_HPP_PP_INC_40 41
#define ENUM_HPP_PP_INC_41 42
#define ENUM_HPP_PP_INC_42 43
#define ENUM_HPP_PP_INC_43 44
#define ENUM_HPP_PP_INC_44 45
#define ENUM_HPP_PP_INC_45 46
#define ENUM_HPP_PP_INC_46 47
#define ENUM_HPP_PP_INC_47 48
#define ENUM_HPP_PP_INC_48 49
#define ENUM_HPP_PP_INC_49 50
#define ENUM_HPP_PP_INC_50 51
#define ENUM_HPP_PP_INC_51 52
#define ENUM_HPP_PP_INC_52 53
#define ENUM_HPP_PP_INC_53 54
#define ENUM_HPP_PP_INC_54 55
#define ENUM_HPP_PP_INC_55 56
#define ENUM_HPP_PP_INC_56 57
#define ENUM_HPP_PP_INC_57 58
#define ENUM_HPP_PP_INC_58 59
#define ENUM_HPP_PP_INC_59 60
#define ENUM_HPP_PP_INC_60 61
#define ENUM_HPP_PP_INC_61 62
#define ENUM_HPP_PP_INC_62 63
#define ENUM_HPP_PP_INC_63 64
#define ENUM_HPP_PP_INC_64 65
#define ENUM_HPP_PP_INC_65 66
#define ENUM_HPP_PP_INC_66 67
#define ENUM_HPP_PP_INC_67 68
#define ENUM_HPP_PP_INC_68 69
#define ENUM_HPP_PP_INC_69 70
#define ENUM_HPP_PP_INC_70 71
#define ENUM_HPP_PP_INC_71 72
#define ENUM_HPP_PP_INC_72 73
#define ENUM_HPP_PP_INC_73 74
#define ENUM_HPP_PP_INC_74 75
#define ENUM_HPP_PP_INC_75 76
#define ENUM_HPP_PP_INC_76 77
#define ENUM_HPP_PP_INC_77 78
#define ENUM_HPP_PP_INC_78 79
#define ENUM_HPP_PP_INC_79 80
#define ENUM_HPP_PP_INC_80 81
#define ENUM_HPP_PP_INC_81 82
#define ENUM_HPP_PP_INC_82 83
#define ENUM_HPP_PP_INC_83 84
#define ENUM_HPP_PP_INC_84 85
#define ENUM_HPP_PP_INC_85 86
#define ENUM_HPP_PP_INC_86 87
#define ENUM_HPP_PP_INC_87 88
#define ENUM_HPP_PP_INC_88 89
#define ENUM_HPP_PP_INC_89 90
#define ENUM_HPP_PP_INC_90 91
#define ENUM_HPP_PP_INC_91 92
#define ENUM_HPP_PP_INC_92 93
#define ENUM_HPP_PP_INC_93 94
#define ENUM_HPP_PP_INC_94 95
#define ENUM_HPP_PP_INC_95 96
#define ENUM_HPP_PP_INC_96 97
#define ENUM_HPP_PP_INC_97 98
#define ENUM_HPP_PP_INC_98 99
#define ENUM_HPP_PP_INC_99 100
#define ENUM_HPP_PP_INC_100 101
#define ENUM_HPP_PP_INC_101 102
#define ENUM_HPP_PP_INC_102 103
#define ENUM_HPP_PP_INC_103 104
#define ENUM_HPP_PP_INC_104 105
#define ENUM_HPP_PP_INC_105 106
#define ENUM_HPP_PP_INC_106 107
#define ENUM_HPP_PP_INC_107 108
#define ENUM_HPP_PP_INC_108 109
#define ENUM_HPP_PP_INC_109 110
#define ENUM_HPP_PP_INC_110 111
#define ENUM_HPP_PP_INC_111 112
#define ENUM_HPP_PP_INC_112 113
#define ENUM_HPP_PP_INC_113 114
#define ENUM_HPP_PP_INC_114 115
#define ENUM_HPP_PP_INC_115 116
#define ENUM_HPP_PP_INC_116 117
#define ENUM_HPP_PP_INC_117 118
#define ENUM_HPP_PP_INC_118 119
#define ENUM_HPP_PP_INC_119 120
#define ENUM_HPP_PP_INC_120 121
#define ENUM_HPP_PP_INC_121 122
#define ENUM_HPP_PP_INC_122 123
#define ENUM_HPP_PP_INC_123 124
#define ENUM_HPP_PP_INC_124 125
#define ENUM_HPP_PP_INC_125 126
#define ENUM_HPP_PP_INC_126 127
#define ENUM_HPP_PP_INC_127 128
#define ENUM_HPP_PP_INC_128 129
#define ENUM_HPP_PP_INC_129 130
#define ENUM_HPP_PP_INC_130 131
#define ENUM_HPP_PP_INC_131 132
#define ENUM_HPP_PP_INC_132 133
#define ENUM_HPP_PP_INC_133 134
#define ENUM_HPP_PP_INC_134 135
#define ENUM_HPP_PP_INC_135 136
#define ENUM_HPP_PP_INC_136 137
#define ENUM_HPP_PP_INC_137 138
#define ENUM_HPP_PP_INC_138 139
#define ENUM_HPP_PP_INC_139 140
#define ENUM_HPP_PP_INC_140 141
#define ENUM_HPP_PP_INC_141 142
#define ENUM_HPP_PP_INC_142 143
#define ENUM_HPP_PP_INC_143 144
#define ENUM_HPP_PP_INC_144 145
#define ENUM_HPP_PP_INC_145 146
#define ENUM_HPP_PP_INC_146 147
#define ENUM_HPP_PP_INC_147 148
#define ENUM_HPP_PP_INC_148 149
#define ENUM_HPP_PP_INC_149 150
#define ENUM_HPP_PP_INC_150 151
#define ENUM_HPP_PP_INC_151 152
#define ENUM_HPP_PP_INC_152 153
#define ENUM_HPP_PP_INC_153 154
#define ENUM_HPP_PP_INC_154 155
#define ENUM_HPP_PP_INC_155 156
#define ENUM_HPP_PP_INC_156 157
#define ENUM_HPP_PP_INC_157 158
#define ENUM_HPP_PP_INC_158 159
#define ENUM_HPP_PP_INC_159 160
#define ENUM_HPP_PP_INC_160 161
#define ENUM_HPP_PP_INC_161 162
#define ENUM_HPP_PP_INC_162 163
#define ENUM_HPP_PP_INC_163 164
#define ENUM_HPP_PP_INC_164 165
#define ENUM_HPP_PP_INC_165 166
#define ENUM_HPP_PP_INC_166 167
#define ENUM_HPP_PP_INC_167 168
#define ENUM_HPP_PP_INC_168 169
#define ENUM_HPP_PP_INC_169 170
#define ENUM_HPP_PP_INC_170 171
#define ENUM_HPP_PP_INC_171 172
#define ENUM_HPP_PP_INC_172 173
#define ENUM_HPP_PP_INC_173 174
#define ENUM_HPP_PP_INC_174 175
#define ENUM_HPP_PP_INC_175 176
#define ENUM_HPP_PP_INC_176 177
#define ENUM_HPP_PP_INC_177 178
#define ENUM_HPP_PP_INC_178 179
#define ENUM_HPP_PP_INC_179 180
#define ENUM_HPP_PP_INC_180 181
#define ENUM_HPP_PP_INC_181 182
#define ENUM_HPP_PP_INC_182 183
#define ENUM_HPP_PP_INC_183 184
#define ENUM_HPP_PP_INC_184 185
#define ENUM_HPP_PP_INC_185 186
#define ENUM_HPP_PP_INC_186 187
#define ENUM_HPP_PP_INC_187 188
#define ENUM_HPP_PP_INC_188 189
#define ENUM_HPP_PP_INC_189 190
#define ENUM_HPP_PP_INC_190 191
#define ENUM_HPP_PP_INC_191 192
#define ENUM_HPP_PP_INC_192 193
#define ENUM_HPP_PP_INC_193 194
#define ENUM_HPP_PP_INC_194 195
#define ENUM_HPP_PP_INC_195 196
#define ENUM_HPP_PP_INC_196 197
#define ENUM_HPP_PP_INC_197 198
#define ENUM_HPP_PP_INC_198 199
#define ENUM_HPP_PP_INC_199 200
#define ENUM_HPP_PP_INC_200 201
#define ENUM_HPP_PP_INC_201 202
#define ENUM_HPP_PP_INC_202 203
#define ENUM_HPP_PP_INC_203 204
#define ENUM_HPP_PP_INC_204 205
#define ENUM_HPP_PP_INC_205 206
#define ENUM_HPP_PP_INC_206 207
#define ENUM_HPP_PP_INC_207 208
#define ENUM_HPP_PP_INC_208 209
#define ENUM_HPP_PP_INC_209 210
#define ENUM_HPP_PP_INC_210 211
#define ENUM_HPP_PP_INC_211 212
#define ENUM_HPP_PP_INC_212 213
#define ENUM_HPP_PP_INC_213 214
#define ENUM_HPP_PP_INC_214 215
#define ENUM_HPP_PP_INC_215 216
#define ENUM_HPP_PP_INC_216 217
#define ENUM_HPP_PP_INC_217 218
#define ENUM_HPP_PP_INC_218 219
#define ENUM_HPP_PP_INC_219 220
#define ENUM_HPP_PP_INC_220 221
#define ENUM_HPP_PP_INC_221 222
#define ENUM_HPP_PP_INC_222 223
#define ENUM_HPP_PP_INC_223 224
#define ENUM_HPP_PP_INC_224 225
#define ENUM_HPP_PP_INC_225 226
#define ENUM_HPP_PP_INC_226 227
#define ENUM_HPP_PP_INC_227 228
#define ENUM_HPP_PP_INC_228 229
#define ENUM_HPP_PP_INC_229 230
#define ENUM_HPP_PP_INC_230 231
#define ENUM_HPP_PP_INC_231 232
#define ENUM_HPP_PP_INC_232 233
#define ENUM_HPP_PP_INC_233 234
#define ENUM_HPP_PP_INC_234 235
#define ENUM_HPP_PP_INC_235 236
#define ENUM_HPP_PP_INC_236 237
#define ENUM_HPP_PP_INC_237 238
#define ENUM_HPP_PP_INC_238 239
#define ENUM_HPP_PP_INC_239 240
#define ENUM_HPP_PP_INC_240 241
#define ENUM_HPP_PP_INC_241 242
#define ENUM_HPP_PP_INC_242 243
#define ENUM_HPP_PP_INC_243 244
#define ENUM_HPP_PP_INC_244 245
#define ENUM_HPP_PP_INC_245 246
#define ENUM_HPP_PP_INC_246 247
#define ENUM_HPP_PP_INC_247 248
#define ENUM_HPP_PP_INC_248 249
#define ENUM_HPP_PP_INC_249 250
#define ENUM_HPP_PP_INC_250 251
#define ENUM_HPP_PP_INC_251 252
#define ENUM_HPP_PP_INC_252 253
#define ENUM_HPP_PP_INC_253 254
#define ENUM_HPP_PP_INC_254 255
#define ENUM_HPP_PP_INC_255 256

//
// ENUM_HPP_PP_SEQ_SIZE
//

#define ENUM_HPP_PP_SEQ_SIZE(seq)\
    ENUM_HPP_PP_CAT(ENUM_HPP_PP_SEQ_SIZE_, ENUM_HPP_PP_SEQ_SIZE_0 seq)

#define ENUM_HPP_PP_SEQ_SIZE_0(_) ENUM_HPP_PP_SEQ_SIZE_1
#define ENUM_HPP_PP_SEQ_SIZE_1(_) ENUM_HPP_PP_SEQ_SIZE_2
#define ENUM_HPP_PP_SEQ_SIZE_2(_) ENUM_HPP_PP_SEQ_SIZE_3
#define ENUM_HPP_PP_SEQ_SIZE_3(_) ENUM_HPP_PP_SEQ_SIZE_4
#define ENUM_HPP_PP_SEQ_SIZE_4(_) ENUM_HPP_PP_SEQ_SIZE_5
#define ENUM_HPP_PP_SEQ_SIZE_5(_) ENUM_HPP_PP_SEQ_SIZE_6
#define ENUM_HPP_PP_SEQ_SIZE_6(_) ENUM_HPP_PP_SEQ_SIZE_7
#define ENUM_HPP_PP_SEQ_SIZE_7(_) ENUM_HPP_PP_SEQ_SIZE_8
#define ENUM_HPP_PP_SEQ_SIZE_8(_) ENUM_HPP_PP_SEQ_SIZE_9
#define ENUM_HPP_PP_SEQ_SIZE_9(_) ENUM_HPP_PP_SEQ_SIZE_10
#define ENUM_HPP_PP_SEQ_SIZE_10(_) ENUM_HPP_PP_SEQ_SIZE_11
#define ENUM_HPP_PP_SEQ_SIZE_11(_) ENUM_HPP_PP_SEQ_SIZE_12
#define ENUM_HPP_PP_SEQ_SIZE_12(_) ENUM_HPP_PP_SEQ_SIZE_13
#define ENUM_HPP_PP_SEQ_SIZE_13(_) ENUM_HPP_PP_SEQ_SIZE_14
#define ENUM_HPP_PP_SEQ_SIZE_14(_) ENUM_HPP_PP_SEQ_SIZE_15
#define ENUM_HPP_PP_SEQ_SIZE_15(_) ENUM_HPP_PP_SEQ_SIZE_16
#define ENUM_HPP_PP_SEQ_SIZE_16(_) ENUM_HPP_PP_SEQ_SIZE_17
#define ENUM_HPP_PP_SEQ_SIZE_17(_) ENUM_HPP_PP_SEQ_SIZE_18
#define ENUM_HPP_PP_SEQ_SIZE_18(_) ENUM_HPP_PP_SEQ_SIZE_19
#define ENUM_HPP_PP_SEQ_SIZE_19(_) ENUM_HPP_PP_SEQ_SIZE_20
#define ENUM_HPP_PP_SEQ_SIZE_20(_) ENUM_HPP_PP_SEQ_SIZE_21
#define ENUM_HPP_PP_SEQ_SIZE_21(_) ENUM_HPP_PP_SEQ_SIZE_22
#define ENUM_HPP_PP_SEQ_SIZE_22(_) ENUM_HPP_PP_SEQ_SIZE_23
#define ENUM_HPP_PP_SEQ_SIZE_23(_) ENUM_HPP_PP_SEQ_SIZE_24
#define ENUM_HPP_PP_SEQ_SIZE_24(_) ENUM_HPP_PP_SEQ_SIZE_25
#define ENUM_HPP_PP_SEQ_SIZE_25(_) ENUM_HPP_PP_SEQ_SIZE_26
#define ENUM_HPP_PP_SEQ_SIZE_26(_) ENUM_HPP_PP_SEQ_SIZE_27
#define ENUM_HPP_PP_SEQ_SIZE_27(_) ENUM_HPP_PP_SEQ_SIZE_28
#define ENUM_HPP_PP_SEQ_SIZE_28(_) ENUM_HPP_PP_SEQ_SIZE_29
#define ENUM_HPP_PP_SEQ_SIZE_29(_) ENUM_HPP_PP_SEQ_SIZE_30
#define ENUM_HPP_PP_SEQ_SIZE_30(_) ENUM_HPP_PP_SEQ_SIZE_31
#define ENUM_HPP_PP_SEQ_SIZE_31(_) ENUM_HPP_PP_SEQ_SIZE_32
#define ENUM_HPP_PP_SEQ_SIZE_32(_) ENUM_HPP_PP_SEQ_SIZE_33
#define ENUM_HPP_PP_SEQ_SIZE_33(_) ENUM_HPP_PP_SEQ_SIZE_34
#define ENUM_HPP_PP_SEQ_SIZE_34(_) ENUM_HPP_PP_SEQ_SIZE_35
#define ENUM_HPP_PP_SEQ_SIZE_35(_) ENUM_HPP_PP_SEQ_SIZE_36
#define ENUM_HPP_PP_SEQ_SIZE_36(_) ENUM_HPP_PP_SEQ_SIZE_37
#define ENUM_HPP_PP_SEQ_SIZE_37(_) ENUM_HPP_PP_SEQ_SIZE_38
#define ENUM_HPP_PP_SEQ_SIZE_38(_) ENUM_HPP_PP_SEQ_SIZE_39
#define ENUM_HPP_PP_SEQ_SIZE_39(_) ENUM_HPP_PP_SEQ_SIZE_40
#define ENUM_HPP_PP_SEQ_SIZE_40(_) ENUM_HPP_PP_SEQ_SIZE_41
#define ENUM_HPP_PP_SEQ_SIZE_41(_) ENUM_HPP_PP_SEQ_SIZE_42
#define ENUM_HPP_PP_SEQ_SIZE_42(_) ENUM_HPP_PP_SEQ_SIZE_43
#define ENUM_HPP_PP_SEQ_SIZE_43(_) ENUM_HPP_PP_SEQ_SIZE_44
#define ENUM_HPP_PP_SEQ_SIZE_44(_) ENUM_HPP_PP_SEQ_SIZE_45
#define ENUM_HPP_PP_SEQ_SIZE_45(_) ENUM_HPP_PP_SEQ_SIZE_46
#define ENUM_HPP_PP_SEQ_SIZE_46(_) ENUM_HPP_PP_SEQ_SIZE_47
#define ENUM_HPP_PP_SEQ_SIZE_47(_) ENUM_HPP_PP_SEQ_SIZE_48
#define ENUM_HPP_PP_SEQ_SIZE_48(_) ENUM_HPP_PP_SEQ_SIZE_49
#define ENUM_HPP_PP_SEQ_SIZE_49(_) ENUM_HPP_PP_SEQ_SIZE_50
#define ENUM_HPP_PP_SEQ_SIZE_50(_) ENUM_HPP_PP_SEQ_SIZE_51
#define ENUM_HPP_PP_SEQ_SIZE_51(_) ENUM_HPP_PP_SEQ_SIZE_52
#define ENUM_HPP_PP_SEQ_SIZE_52(_) ENUM_HPP_PP_SEQ_SIZE_53
#define ENUM_HPP_PP_SEQ_SIZE_53(_) ENUM_HPP_PP_SEQ_SIZE_54
#define ENUM_HPP_PP_SEQ_SIZE_54(_) ENUM_HPP_PP_SEQ_SIZE_55
#define ENUM_HPP_PP_SEQ_SIZE_55(_) ENUM_HPP_PP_SEQ_SIZE_56
#define ENUM_HPP_PP_SEQ_SIZE_56(_) ENUM_HPP_PP_SEQ_SIZE_57
#define ENUM_HPP_PP_SEQ_SIZE_57(_) ENUM_HPP_PP_SEQ_SIZE_58
#define ENUM_HPP_PP_SEQ_SIZE_58(_) ENUM_HPP_PP_SEQ_SIZE_59
#define ENUM_HPP_PP_SEQ_SIZE_59(_) ENUM_HPP_PP_SEQ_SIZE_60
#define ENUM_HPP_PP_SEQ_SIZE_60(_) ENUM_HPP_PP_SEQ_SIZE_61
#define ENUM_HPP_PP_SEQ_SIZE_61(_) ENUM_HPP_PP_SEQ_SIZE_62
#define ENUM_HPP_PP_SEQ_SIZE_62(_) ENUM_HPP_PP_SEQ_SIZE_63
#define ENUM_HPP_PP_SEQ_SIZE_63(_) ENUM_HPP_PP_SEQ_SIZE_64
#define ENUM_HPP_PP_SEQ_SIZE_64(_) ENUM_HPP_PP_SEQ_SIZE_65
#define ENUM_HPP_PP_SEQ_SIZE_65(_) ENUM_HPP_PP_SEQ_SIZE_66
#define ENUM_HPP_PP_SEQ_SIZE_66(_) ENUM_HPP_PP_SEQ_SIZE_67
#define ENUM_HPP_PP_SEQ_SIZE_67(_) ENUM_HPP_PP_SEQ_SIZE_68
#define ENUM_HPP_PP_SEQ_SIZE_68(_) ENUM_HPP_PP_SEQ_SIZE_69
#define ENUM_HPP_PP_SEQ_SIZE_69(_) ENUM_HPP_PP_SEQ_SIZE_70
#define ENUM_HPP_PP_SEQ_SIZE_70(_) ENUM_HPP_PP_SEQ_SIZE_71
#define ENUM_HPP_PP_SEQ_SIZE_71(_) ENUM_HPP_PP_SEQ_SIZE_72
#define ENUM_HPP_PP_SEQ_SIZE_72(_) ENUM_HPP_PP_SEQ_SIZE_73
#define ENUM_HPP_PP_SEQ_SIZE_73(_) ENUM_HPP_PP_SEQ_SIZE_74
#define ENUM_HPP_PP_SEQ_SIZE_74(_) ENUM_HPP_PP_SEQ_SIZE_75
#define ENUM_HPP_PP_SEQ_SIZE_75(_) ENUM_HPP_PP_SEQ_SIZE_76
#define ENUM_HPP_PP_SEQ_SIZE_76(_) ENUM_HPP_PP_SEQ_SIZE_77
#define ENUM_HPP_PP_SEQ_SIZE_77(_) ENUM_HPP_PP_SEQ_SIZE_78
#define ENUM_HPP_PP_SEQ_SIZE_78(_) ENUM_HPP_PP_SEQ_SIZE_79
#define ENUM_HPP_PP_SEQ_SIZE_79(_) ENUM_HPP_PP_SEQ_SIZE_80
#define ENUM_HPP_PP_SEQ_SIZE_80(_) ENUM_HPP_PP_SEQ_SIZE_81
#define ENUM_HPP_PP_SEQ_SIZE_81(_) ENUM_HPP_PP_SEQ_SIZE_82
#define ENUM_HPP_PP_SEQ_SIZE_82(_) ENUM_HPP_PP_SEQ_SIZE_83
#define ENUM_HPP_PP_SEQ_SIZE_83(_) ENUM_HPP_PP_SEQ_SIZE_84
#define ENUM_HPP_PP_SEQ_SIZE_84(_) ENUM_HPP_PP_SEQ_SIZE_85
#define ENUM_HPP_PP_SEQ_SIZE_85(_) ENUM_HPP_PP_SEQ_SIZE_86
#define ENUM_HPP_PP_SEQ_SIZE_86(_) ENUM_HPP_PP_SEQ_SIZE_87
#define ENUM_HPP_PP_SEQ_SIZE_87(_) ENUM_HPP_PP_SEQ_SIZE_88
#define ENUM_HPP_PP_SEQ_SIZE_88(_) ENUM_HPP_PP_SEQ_SIZE_89
#define ENUM_HPP_PP_SEQ_SIZE_89(_) ENUM_HPP_PP_SEQ_SIZE_90
#define ENUM_HPP_PP_SEQ_SIZE_90(_) ENUM_HPP_PP_SEQ_SIZE_91
#define ENUM_HPP_PP_SEQ_SIZE_91(_) ENUM_HPP_PP_SEQ_SIZE_92
#define ENUM_HPP_PP_SEQ_SIZE_92(_) ENUM_HPP_PP_SEQ_SIZE_93
#define ENUM_HPP_PP_SEQ_SIZE_93(_) ENUM_HPP_PP_SEQ_SIZE_94
#define ENUM_HPP_PP_SEQ_SIZE_94(_) ENUM_HPP_PP_SEQ_SIZE_95
#define ENUM_HPP_PP_SEQ_SIZE_95(_) ENUM_HPP_PP_SEQ_SIZE_96
#define ENUM_HPP_PP_SEQ_SIZE_96(_) ENUM_HPP_PP_SEQ_SIZE_97
#define ENUM_HPP_PP_SEQ_SIZE_97(_) ENUM_HPP_PP_SEQ_SIZE_98
#define ENUM_HPP_PP_SEQ_SIZE_98(_) ENUM_HPP_PP_SEQ_SIZE_99
#define ENUM_HPP_PP_SEQ_SIZE_99(_) ENUM_HPP_PP_SEQ_SIZE_100
#define ENUM_HPP_PP_SEQ_SIZE_100(_) ENUM_HPP_PP_SEQ_SIZE_101
#define ENUM_HPP_PP_SEQ_SIZE_101(_) ENUM_HPP_PP_SEQ_SIZE_102
#define ENUM_HPP_PP_SEQ_SIZE_102(_) ENUM_HPP_PP_SEQ_SIZE_103
#define ENUM_HPP_PP_SEQ_SIZE_103(_) ENUM_HPP_PP_SEQ_SIZE_104
#define ENUM_HPP_PP_SEQ_SIZE_104(_) ENUM_HPP_PP_SEQ_SIZE_105
#define ENUM_HPP_PP_SEQ_SIZE_105(_) ENUM_HPP_PP_SEQ_SIZE_106
#define ENUM_HPP_PP_SEQ_SIZE_106(_) ENUM_HPP_PP_SEQ_SIZE_107
#define ENUM_HPP_PP_SEQ_SIZE_107(_) ENUM_HPP_PP_SEQ_SIZE_108
#define ENUM_HPP_PP_SEQ_SIZE_108(_) ENUM_HPP_PP_SEQ_SIZE_109
#define ENUM_HPP_PP_SEQ_SIZE_109(_) ENUM_HPP_PP_SEQ_SIZE_110
#define ENUM_HPP_PP_SEQ_SIZE_110(_) ENUM_HPP_PP_SEQ_SIZE_111
#define ENUM_HPP_PP_SEQ_SIZE_111(_) ENUM_HPP_PP_SEQ_SIZE_112
#define ENUM_HPP_PP_SEQ_SIZE_112(_) ENUM_HPP_PP_SEQ_SIZE_113
#define ENUM_HPP_PP_SEQ_SIZE_113(_) ENUM_HPP_PP_SEQ_SIZE_114
#define ENUM_HPP_PP_SEQ_SIZE_114(_) ENUM_HPP_PP_SEQ_SIZE_115
#define ENUM_HPP_PP_SEQ_SIZE_115(_) ENUM_HPP_PP_SEQ_SIZE_116
#define ENUM_HPP_PP_SEQ_SIZE_116(_) ENUM_HPP_PP_SEQ_SIZE_117
#define ENUM_HPP_PP_SEQ_SIZE_117(_) ENUM_HPP_PP_SEQ_SIZE_118
#define ENUM_HPP_PP_SEQ_SIZE_118(_) ENUM_HPP_PP_SEQ_SIZE_119
#define ENUM_HPP_PP_SEQ_SIZE_119(_) ENUM_HPP_PP_SEQ_SIZE_120
#define ENUM_HPP_PP_SEQ_SIZE_120(_) ENUM_HPP_PP_SEQ_SIZE_121
#define ENUM_HPP_PP_SEQ_SIZE_121(_) ENUM_HPP_PP_SEQ_SIZE_122
#define ENUM_HPP_PP_SEQ_SIZE_122(_) ENUM_HPP_PP_SEQ_SIZE_123
#define ENUM_HPP_PP_SEQ_SIZE_123(_) ENUM_HPP_PP_SEQ_SIZE_124
#define ENUM_HPP_PP_SEQ_SIZE_124(_) ENUM_HPP_PP_SEQ_SIZE_125
#define ENUM_HPP_PP_SEQ_SIZE_125(_) ENUM_HPP_PP_SEQ_SIZE_126
#define ENUM_HPP_PP_SEQ_SIZE_126(_) ENUM_HPP_PP_SEQ_SIZE_127
#define ENUM_HPP_PP_SEQ_SIZE_127(_) ENUM_HPP_PP_SEQ_SIZE_128
#define ENUM_HPP_PP_SEQ_SIZE_128(_) ENUM_HPP_PP_SEQ_SIZE_129
#define ENUM_HPP_PP_SEQ_SIZE_129(_) ENUM_HPP_PP_SEQ_SIZE_130
#define ENUM_HPP_PP_SEQ_SIZE_130(_) ENUM_HPP_PP_SEQ_SIZE_131
#define ENUM_HPP_PP_SEQ_SIZE_131(_) ENUM_HPP_PP_SEQ_SIZE_132
#define ENUM_HPP_PP_SEQ_SIZE_132(_) ENUM_HPP_PP_SEQ_SIZE_133
#define ENUM_HPP_PP_SEQ_SIZE_133(_) ENUM_HPP_PP_SEQ_SIZE_134
#define ENUM_HPP_PP_SEQ_SIZE_134(_) ENUM_HPP_PP_SEQ_SIZE_135
#define ENUM_HPP_PP_SEQ_SIZE_135(_) ENUM_HPP_PP_SEQ_SIZE_136
#define ENUM_HPP_PP_SEQ_SIZE_136(_) ENUM_HPP_PP_SEQ_SIZE_137
#define ENUM_HPP_PP_SEQ_SIZE_137(_) ENUM_HPP_PP_SEQ_SIZE_138
#define ENUM_HPP_PP_SEQ_SIZE_138(_) ENUM_HPP_PP_SEQ_SIZE_139
#define ENUM_HPP_PP_SEQ_SIZE_139(_) ENUM_HPP_PP_SEQ_SIZE_140
#define ENUM_HPP_PP_SEQ_SIZE_140(_) ENUM_HPP_PP_SEQ_SIZE_141
#define ENUM_HPP_PP_SEQ_SIZE_141(_) ENUM_HPP_PP_SEQ_SIZE_142
#define ENUM_HPP_PP_SEQ_SIZE_142(_) ENUM_HPP_PP_SEQ_SIZE_143
#define ENUM_HPP_PP_SEQ_SIZE_143(_) ENUM_HPP_PP_SEQ_SIZE_144
#define ENUM_HPP_PP_SEQ_SIZE_144(_) ENUM_HPP_PP_SEQ_SIZE_145
#define ENUM_HPP_PP_SEQ_SIZE_145(_) ENUM_HPP_PP_SEQ_SIZE_146
#define ENUM_HPP_PP_SEQ_SIZE_146(_) ENUM_HPP_PP_SEQ_SIZE_147
#define ENUM_HPP_PP_SEQ_SIZE_147(_) ENUM_HPP_PP_SEQ_SIZE_148
#define ENUM_HPP_PP_SEQ_SIZE_148(_) ENUM_HPP_PP_SEQ_SIZE_149
#define ENUM_HPP_PP_SEQ_SIZE_149(_) ENUM_HPP_PP_SEQ_SIZE_150
#define ENUM_HPP_PP_SEQ_SIZE_150(_) ENUM_HPP_PP_SEQ_SIZE_151
#define ENUM_HPP_PP_SEQ_SIZE_151(_) ENUM_HPP_PP_SEQ_SIZE_152
#define ENUM_HPP_PP_SEQ_SIZE_152(_) ENUM_HPP_PP_SEQ_SIZE_153
#define ENUM_HPP_PP_SEQ_SIZE_153(_) ENUM_HPP_PP_SEQ_SIZE_154
#define ENUM_HPP_PP_SEQ_SIZE_154(_) ENUM_HPP_PP_SEQ_SIZE_155
#define ENUM_HPP_PP_SEQ_SIZE_155(_) ENUM_HPP_PP_SEQ_SIZE_156
#define ENUM_HPP_PP_SEQ_SIZE_156(_) ENUM_HPP_PP_SEQ_SIZE_157
#define ENUM_HPP_PP_SEQ_SIZE_157(_) ENUM_HPP_PP_SEQ_SIZE_158
#define ENUM_HPP_PP_SEQ_SIZE_158(_) ENUM_HPP_PP_SEQ_SIZE_159
#define ENUM_HPP_PP_SEQ_SIZE_159(_) ENUM_HPP_PP_SEQ_SIZE_160
#define ENUM_HPP_PP_SEQ_SIZE_160(_) ENUM_HPP_PP_SEQ_SIZE_161
#define ENUM_HPP_PP_SEQ_SIZE_161(_) ENUM_HPP_PP_SEQ_SIZE_162
#define ENUM_HPP_PP_SEQ_SIZE_162(_) ENUM_HPP_PP_SEQ_SIZE_163
#define ENUM_HPP_PP_SEQ_SIZE_163(_) ENUM_HPP_PP_SEQ_SIZE_164
#define ENUM_HPP_PP_SEQ_SIZE_164(_) ENUM_HPP_PP_SEQ_SIZE_165
#define ENUM_HPP_PP_SEQ_SIZE_165(_) ENUM_HPP_PP_SEQ_SIZE_166
#define ENUM_HPP_PP_SEQ_SIZE_166(_) ENUM_HPP_PP_SEQ_SIZE_167
#define ENUM_HPP_PP_SEQ_SIZE_167(_) ENUM_HPP_PP_SEQ_SIZE_168
#define ENUM_HPP_PP_SEQ_SIZE_168(_) ENUM_HPP_PP_SEQ_SIZE_169
#define ENUM_HPP_PP_SEQ_SIZE_169(_) ENUM_HPP_PP_SEQ_SIZE_170
#define ENUM_HPP_PP_SEQ_SIZE_170(_) ENUM_HPP_PP_SEQ_SIZE_171
#define ENUM_HPP_PP_SEQ_SIZE_171(_) ENUM_HPP_PP_SEQ_SIZE_172
#define ENUM_HPP_PP_SEQ_SIZE_172(_) ENUM_HPP_PP_SEQ_SIZE_173
#define ENUM_HPP_PP_SEQ_SIZE_173(_) ENUM_HPP_PP_SEQ_SIZE_174
#define ENUM_HPP_PP_SEQ_SIZE_174(_) ENUM_HPP_PP_SEQ_SIZE_175
#define ENUM_HPP_PP_SEQ_SIZE_175(_) ENUM_HPP_PP_SEQ_SIZE_176
#define ENUM_HPP_PP_SEQ_SIZE_176(_) ENUM_HPP_PP_SEQ_SIZE_177
#define ENUM_HPP_PP_SEQ_SIZE_177(_) ENUM_HPP_PP_SEQ_SIZE_178
#define ENUM_HPP_PP_SEQ_SIZE_178(_) ENUM_HPP_PP_SEQ_SIZE_179
#define ENUM_HPP_PP_SEQ_SIZE_179(_) ENUM_HPP_PP_SEQ_SIZE_180
#define ENUM_HPP_PP_SEQ_SIZE_180(_) ENUM_HPP_PP_SEQ_SIZE_181
#define ENUM_HPP_PP_SEQ_SIZE_181(_) ENUM_HPP_PP_SEQ_SIZE_182
#define ENUM_HPP_PP_SEQ_SIZE_182(_) ENUM_HPP_PP_SEQ_SIZE_183
#define ENUM_HPP_PP_SEQ_SIZE_183(_) ENUM_HPP_PP_SEQ_SIZE_184
#define ENUM_HPP_PP_SEQ_SIZE_184(_) ENUM_HPP_PP_SEQ_SIZE_185
#define ENUM_HPP_PP_SEQ_SIZE_185(_) ENUM_HPP_PP_SEQ_SIZE_186
#define ENUM_HPP_PP_SEQ_SIZE_186(_) ENUM_HPP_PP_SEQ_SIZE_187
#define ENUM_HPP_PP_SEQ_SIZE_187(_) ENUM_HPP_PP_SEQ_SIZE_188
#define ENUM_HPP_PP_SEQ_SIZE_188(_) ENUM_HPP_PP_SEQ_SIZE_189
#define ENUM_HPP_PP_SEQ_SIZE_189(_) ENUM_HPP_PP_SEQ_SIZE_190
#define ENUM_HPP_PP_SEQ_SIZE_190(_) ENUM_HPP_PP_SEQ_SIZE_191
#define ENUM_HPP_PP_SEQ_SIZE_191(_) ENUM_HPP_PP_SEQ_SIZE_192
#define ENUM_HPP_PP_SEQ_SIZE_192(_) ENUM_HPP_PP_SEQ_SIZE_193
#define ENUM_HPP_PP_SEQ_SIZE_193(_) ENUM_HPP_PP_SEQ_SIZE_194
#define ENUM_HPP_PP_SEQ_SIZE_194(_) ENUM_HPP_PP_SEQ_SIZE_195
#define ENUM_HPP_PP_SEQ_SIZE_195(_) ENUM_HPP_PP_SEQ_SIZE_196
#define ENUM_HPP_PP_SEQ_SIZE_196(_) ENUM_HPP_PP_SEQ_SIZE_197
#define ENUM_HPP_PP_SEQ_SIZE_197(_) ENUM_HPP_PP_SEQ_SIZE_198
#define ENUM_HPP_PP_SEQ_SIZE_198(_) ENUM_HPP_PP_SEQ_SIZE_199
#define ENUM_HPP_PP_SEQ_SIZE_199(_) ENUM_HPP_PP_SEQ_SIZE_200
#define ENUM_HPP_PP_SEQ_SIZE_200(_) ENUM_HPP_PP_SEQ_SIZE_201
#define ENUM_HPP_PP_SEQ_SIZE_201(_) ENUM_HPP_PP_SEQ_SIZE_202
#define ENUM_HPP_PP_SEQ_SIZE_202(_) ENUM_HPP_PP_SEQ_SIZE_203
#define ENUM_HPP_PP_SEQ_SIZE_203(_) ENUM_HPP_PP_SEQ_SIZE_204
#define ENUM_HPP_PP_SEQ_SIZE_204(_) ENUM_HPP_PP_SEQ_SIZE_205
#define ENUM_HPP_PP_SEQ_SIZE_205(_) ENUM_HPP_PP_SEQ_SIZE_206
#define ENUM_HPP_PP_SEQ_SIZE_206(_) ENUM_HPP_PP_SEQ_SIZE_207
#define ENUM_HPP_PP_SEQ_SIZE_207(_) ENUM_HPP_PP_SEQ_SIZE_208
#define ENUM_HPP_PP_SEQ_SIZE_208(_) ENUM_HPP_PP_SEQ_SIZE_209
#define ENUM_HPP_PP_SEQ_SIZE_209(_) ENUM_HPP_PP_SEQ_SIZE_210
#define ENUM_HPP_PP_SEQ_SIZE_210(_) ENUM_HPP_PP_SEQ_SIZE_211
#define ENUM_HPP_PP_SEQ_SIZE_211(_) ENUM_HPP_PP_SEQ_SIZE_212
#define ENUM_HPP_PP_SEQ_SIZE_212(_) ENUM_HPP_PP_SEQ_SIZE_213
#define ENUM_HPP_PP_SEQ_SIZE_213(_) ENUM_HPP_PP_SEQ_SIZE_214
#define ENUM_HPP_PP_SEQ_SIZE_214(_) ENUM_HPP_PP_SEQ_SIZE_215
#define ENUM_HPP_PP_SEQ_SIZE_215(_) ENUM_HPP_PP_SEQ_SIZE_216
#define ENUM_HPP_PP_SEQ_SIZE_216(_) ENUM_HPP_PP_SEQ_SIZE_217
#define ENUM_HPP_PP_SEQ_SIZE_217(_) ENUM_HPP_PP_SEQ_SIZE_218
#define ENUM_HPP_PP_SEQ_SIZE_218(_) ENUM_HPP_PP_SEQ_SIZE_219
#define ENUM_HPP_PP_SEQ_SIZE_219(_) ENUM_HPP_PP_SEQ_SIZE_220
#define ENUM_HPP_PP_SEQ_SIZE_220(_) ENUM_HPP_PP_SEQ_SIZE_221
#define ENUM_HPP_PP_SEQ_SIZE_221(_) ENUM_HPP_PP_SEQ_SIZE_222
#define ENUM_HPP_PP_SEQ_SIZE_222(_) ENUM_HPP_PP_SEQ_SIZE_223
#define ENUM_HPP_PP_SEQ_SIZE_223(_) ENUM_HPP_PP_SEQ_SIZE_224
#define ENUM_HPP_PP_SEQ_SIZE_224(_) ENUM_HPP_PP_SEQ_SIZE_225
#define ENUM_HPP_PP_SEQ_SIZE_225(_) ENUM_HPP_PP_SEQ_SIZE_226
#define ENUM_HPP_PP_SEQ_SIZE_226(_) ENUM_HPP_PP_SEQ_SIZE_227
#define ENUM_HPP_PP_SEQ_SIZE_227(_) ENUM_HPP_PP_SEQ_SIZE_228
#define ENUM_HPP_PP_SEQ_SIZE_228(_) ENUM_HPP_PP_SEQ_SIZE_229
#define ENUM_HPP_PP_SEQ_SIZE_229(_) ENUM_HPP_PP_SEQ_SIZE_230
#define ENUM_HPP_PP_SEQ_SIZE_230(_) ENUM_HPP_PP_SEQ_SIZE_231
#define ENUM_HPP_PP_SEQ_SIZE_231(_) ENUM_HPP_PP_SEQ_SIZE_232
#define ENUM_HPP_PP_SEQ_SIZE_232(_) ENUM_HPP_PP_SEQ_SIZE_233
#define ENUM_HPP_PP_SEQ_SIZE_233(_) ENUM_HPP_PP_SEQ_SIZE_234
#define ENUM_HPP_PP_SEQ_SIZE_234(_) ENUM_HPP_PP_SEQ_SIZE_235
#define ENUM_HPP_PP_SEQ_SIZE_235(_) ENUM_HPP_PP_SEQ_SIZE_236
#define ENUM_HPP_PP_SEQ_SIZE_236(_) ENUM_HPP_PP_SEQ_SIZE_237
#define ENUM_HPP_PP_SEQ_SIZE_237(_) ENUM_HPP_PP_SEQ_SIZE_238
#define ENUM_HPP_PP_SEQ_SIZE_238(_) ENUM_HPP_PP_SEQ_SIZE_239
#define ENUM_HPP_PP_SEQ_SIZE_239(_) ENUM_HPP_PP_SEQ_SIZE_240
#define ENUM_HPP_PP_SEQ_SIZE_240(_) ENUM_HPP_PP_SEQ_SIZE_241
#define ENUM_HPP_PP_SEQ_SIZE_241(_) ENUM_HPP_PP_SEQ_SIZE_242
#define ENUM_HPP_PP_SEQ_SIZE_242(_) ENUM_HPP_PP_SEQ_SIZE_243
#define ENUM_HPP_PP_SEQ_SIZE_243(_) ENUM_HPP_PP_SEQ_SIZE_244
#define ENUM_HPP_PP_SEQ_SIZE_244(_) ENUM_HPP_PP_SEQ_SIZE_245
#define ENUM_HPP_PP_SEQ_SIZE_245(_) ENUM_HPP_PP_SEQ_SIZE_246
#define ENUM_HPP_PP_SEQ_SIZE_246(_) ENUM_HPP_PP_SEQ_SIZE_247
#define ENUM_HPP_PP_SEQ_SIZE_247(_) ENUM_HPP_PP_SEQ_SIZE_248
#define ENUM_HPP_PP_SEQ_SIZE_248(_) ENUM_HPP_PP_SEQ_SIZE_249
#define ENUM_HPP_PP_SEQ_SIZE_249(_) ENUM_HPP_PP_SEQ_SIZE_250
#define ENUM_HPP_PP_SEQ_SIZE_250(_) ENUM_HPP_PP_SEQ_SIZE_251
#define ENUM_HPP_PP_SEQ_SIZE_251(_) ENUM_HPP_PP_SEQ_SIZE_252
#define ENUM_HPP_PP_SEQ_SIZE_252(_) ENUM_HPP_PP_SEQ_SIZE_253
#define ENUM_HPP_PP_SEQ_SIZE_253(_) ENUM_HPP_PP_SEQ_SIZE_254
#define ENUM_HPP_PP_SEQ_SIZE_254(_) ENUM_HPP_PP_SEQ_SIZE_255
#define ENUM_HPP_PP_SEQ_SIZE_255(_) ENUM_HPP_PP_SEQ_SIZE_256

# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_0 0
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_1 1
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_2 2
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_3 3
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_4 4
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_5 5
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_6 6
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_7 7
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_8 8
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_9 9
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_10 10
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_11 11
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_12 12
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_13 13
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_14 14
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_15 15
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_16 16
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_17 17
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_18 18
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_19 19
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_20 20
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_21 21
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_22 22
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_23 23
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_24 24
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_25 25
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_26 26
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_27 27
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_28 28
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_29 29
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_30 30
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_31 31
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_32 32
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_33 33
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_34 34
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_35 35
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_36 36
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_37 37
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_38 38
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_39 39
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_40 40
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_41 41
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_42 42
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_43 43
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_44 44
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_45 45
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_46 46
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_47 47
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_48 48
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_49 49
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_50 50
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_51 51
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_52 52
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_53 53
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_54 54
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_55 55
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_56 56
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_57 57
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_58 58
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_59 59
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_60 60
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_61 61
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_62 62
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_63 63
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_64 64
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_65 65
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_66 66
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_67 67
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_68 68
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_69 69
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_70 70
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_71 71
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_72 72
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_73 73
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_74 74
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_75 75
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_76 76
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_77 77
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_78 78
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_79 79
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_80 80
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_81 81
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_82 82
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_83 83
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_84 84
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_85 85
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_86 86
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_87 87
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_88 88
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_89 89
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_90 90
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_91 91
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_92 92
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_93 93
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_94 94
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_95 95
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_96 96
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_97 97
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_98 98
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_99 99
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_100 100
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_101 101
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_102 102
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_103 103
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_104 104
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_105 105
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_106 106
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_107 107
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_108 108
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_109 109
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_110 110
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_111 111
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_112 112
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_113 113
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_114 114
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_115 115
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_116 116
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_117 117
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_118 118
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_119 119
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_120 120
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_121 121
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_122 122
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_123 123
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_124 124
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_125 125
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_126 126
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_127 127
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_128 128
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_129 129
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_130 130
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_131 131
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_132 132
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_133 133
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_134 134
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_135 135
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_136 136
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_137 137
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_138 138
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_139 139
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_140 140
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_141 141
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_142 142
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_143 143
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_144 144
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_145 145
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_146 146
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_147 147
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_148 148
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_149 149
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_150 150
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_151 151
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_152 152
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_153 153
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_154 154
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_155 155
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_156 156
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_157 157
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_158 158
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_159 159
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_160 160
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_161 161
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_162 162
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_163 163
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_164 164
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_165 165
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_166 166
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_167 167
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_168 168
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_169 169
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_170 170
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_171 171
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_172 172
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_173 173
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_174 174
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_175 175
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_176 176
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_177 177
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_178 178
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_179 179
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_180 180
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_181 181
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_182 182
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_183 183
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_184 184
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_185 185
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_186 186
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_187 187
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_188 188
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_189 189
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_190 190
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_191 191
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_192 192
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_193 193
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_194 194
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_195 195
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_196 196
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_197 197
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_198 198
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_199 199
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_200 200
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_201 201
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_202 202
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_203 203
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_204 204
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_205 205
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_206 206
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_207 207
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_208 208
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_209 209
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_210 210
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_211 211
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_212 212
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_213 213
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_214 214
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_215 215
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_216 216
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_217 217
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_218 218
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_219 219
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_220 220
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_221 221
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_222 222
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_223 223
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_224 224
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_225 225
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_226 226
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_227 227
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_228 228
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_229 229
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_230 230
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_231 231
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_232 232
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_233 233
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_234 234
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_235 235
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_236 236
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_237 237
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_238 238
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_239 239
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_240 240
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_241 241
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_242 242
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_243 243
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_244 244
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_245 245
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_246 246
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_247 247
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_248 248
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_249 249
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_250 250
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_251 251
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_252 252
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_253 253
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_254 254
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_255 255
# define ENUM_HPP_PP_SEQ_SIZE_ENUM_HPP_PP_SEQ_SIZE_256 256

//
// ENUM_HPP_PP_SEQ_FOR_EACH
//

#define ENUM_HPP_PP_SEQ_FOR_EACH(m, d, s)\
    ENUM_HPP_PP_CAT(ENUM_HPP_PP_SEQ_FOR_EACH_, ENUM_HPP_PP_SEQ_SIZE(s)) (m, d, 0, s)

#define ENUM_HPP_PP_SEQ_FOR_EACH_1(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_2(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_1(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_3(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_2(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_4(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_3(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_5(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_4(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_6(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_5(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_7(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_6(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_8(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_7(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_9(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_8(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_10(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_9(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_11(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_10(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_12(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_11(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_13(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_12(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_14(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_13(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_15(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_14(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_16(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_15(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_17(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_16(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_18(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_17(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_19(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_18(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_20(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_19(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_21(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_20(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_22(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_21(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_23(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_22(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_24(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_23(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_25(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_24(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_26(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_25(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_27(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_26(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_28(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_27(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_29(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_28(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_30(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_29(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_31(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_30(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_32(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_31(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_33(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_32(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_34(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_33(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_35(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_34(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_36(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_35(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_37(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_36(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_38(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_37(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_39(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_38(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_40(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_39(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_41(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_40(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_42(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_41(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_43(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_42(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_44(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_43(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_45(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_44(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_46(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_45(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_47(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_46(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_48(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_47(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_49(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_48(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_50(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_49(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_51(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_50(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_52(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_51(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_53(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_52(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_54(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_53(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_55(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_54(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_56(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_55(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_57(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_56(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_58(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_57(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_59(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_58(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_60(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_59(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_61(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_60(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_62(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_61(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_63(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_62(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_64(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_63(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_65(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_64(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_66(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_65(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_67(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_66(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_68(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_67(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_69(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_68(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_70(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_69(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_71(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_70(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_72(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_71(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_73(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_72(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_74(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_73(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_75(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_74(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_76(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_75(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_77(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_76(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_78(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_77(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_79(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_78(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_80(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_79(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_81(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_80(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_82(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_81(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_83(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_82(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_84(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_83(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_85(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_84(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_86(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_85(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_87(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_86(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_88(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_87(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_89(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_88(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_90(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_89(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_91(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_90(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_92(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_91(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_93(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_92(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_94(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_93(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_95(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_94(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_96(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_95(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_97(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_96(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_98(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_97(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_99(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_98(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_100(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_99(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_101(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_100(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_102(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_101(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_103(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_102(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_104(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_103(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_105(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_104(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_106(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_105(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_107(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_106(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_108(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_107(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_109(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_108(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_110(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_109(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_111(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_110(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_112(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_111(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_113(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_112(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_114(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_113(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_115(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_114(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_116(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_115(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_117(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_116(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_118(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_117(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_119(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_118(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_120(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_119(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_121(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_120(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_122(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_121(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_123(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_122(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_124(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_123(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_125(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_124(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_126(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_125(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_127(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_126(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_128(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_127(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_129(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_128(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_130(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_129(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_131(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_130(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_132(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_131(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_133(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_132(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_134(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_133(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_135(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_134(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_136(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_135(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_137(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_136(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_138(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_137(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_139(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_138(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_140(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_139(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_141(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_140(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_142(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_141(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_143(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_142(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_144(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_143(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_145(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_144(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_146(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_145(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_147(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_146(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_148(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_147(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_149(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_148(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_150(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_149(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_151(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_150(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_152(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_151(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_153(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_152(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_154(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_153(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_155(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_154(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_156(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_155(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_157(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_156(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_158(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_157(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_159(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_158(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_160(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_159(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_161(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_160(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_162(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_161(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_163(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_162(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_164(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_163(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_165(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_164(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_166(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_165(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_167(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_166(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_168(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_167(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_169(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_168(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_170(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_169(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_171(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_170(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_172(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_171(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_173(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_172(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_174(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_173(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_175(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_174(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_176(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_175(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_177(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_176(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_178(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_177(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_179(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_178(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_180(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_179(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_181(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_180(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_182(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_181(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_183(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_182(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_184(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_183(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_185(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_184(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_186(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_185(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_187(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_186(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_188(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_187(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_189(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_188(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_190(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_189(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_191(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_190(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_192(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_191(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_193(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_192(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_194(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_193(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_195(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_194(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_196(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_195(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_197(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_196(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_198(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_197(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_199(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_198(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_200(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_199(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_201(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_200(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_202(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_201(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_203(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_202(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_204(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_203(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_205(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_204(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_206(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_205(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_207(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_206(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_208(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_207(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_209(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_208(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_210(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_209(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_211(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_210(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_212(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_211(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_213(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_212(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_214(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_213(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_215(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_214(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_216(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_215(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_217(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_216(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_218(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_217(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_219(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_218(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_220(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_219(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_221(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_220(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_222(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_221(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_223(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_222(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_224(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_223(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_225(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_224(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_226(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_225(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_227(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_226(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_228(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_227(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_229(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_228(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_230(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_229(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_231(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_230(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_232(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_231(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_233(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_232(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_234(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_233(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_235(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_234(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_236(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_235(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_237(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_236(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_238(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_237(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_239(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_238(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_240(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_239(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_241(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_240(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_242(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_241(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_243(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_242(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_244(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_243(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_245(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_244(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_246(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_245(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_247(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_246(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_248(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_247(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_249(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_248(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_250(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_249(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_251(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_250(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_252(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_251(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_253(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_252(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_254(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_253(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_255(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_254(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
#define ENUM_HPP_PP_SEQ_FOR_EACH_256(m, d, i, s) m(d, i, ENUM_HPP_PP_SEQ_HEAD(s)) ENUM_HPP_PP_SEQ_FOR_EACH_255(m, d, ENUM_HPP_PP_INC(i), ENUM_HPP_PP_SEQ_TAIL(s))
