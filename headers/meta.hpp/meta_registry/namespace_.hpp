/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_registry_fwd.hpp"

#include "class_.hpp"
#include "data_.hpp"
#include "enum_.hpp"
#include "function_.hpp"

namespace meta_hpp
{
    class namespace_ final {
    public:
        explicit namespace_(std::string name);

        namespace_info make_info() const;
    public:
        template < typename... Internals >
        namespace_& operator()(Internals&&...internals);
    private:
        template < typename Class >
        void add_(const class_<Class>& internal);
        void add_(const data_& internal);
        template < typename Enum >
        void add_(const enum_<Enum>& internal);
        template < typename Function >
        void add_(const function_<Function>& internal);
        void add_(const namespace_& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        class_info_map classes_;
        data_info_map datas_;
        enum_info_map enums_;
        function_info_map functions_;
        namespace_info_map namespaces_;
    };
}

namespace meta_hpp
{
    inline namespace_::namespace_(std::string name)
    : name_{std::move(name)} {}

    inline namespace_info namespace_::make_info() const {
        namespace_info info{name_};
        info.state_->classes.insert(classes_.begin(), classes_.end());
        info.state_->datas.insert(datas_.begin(), datas_.end());
        info.state_->enums.insert(enums_.begin(), enums_.end());
        info.state_->functions.insert(functions_.begin(), functions_.end());
        info.state_->namespaces.insert(namespaces_.begin(), namespaces_.end());
        return info;
    }

    template < typename... Internals >
    inline namespace_& namespace_::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Class >
    inline void namespace_::add_(const class_<Class>& internal) {
        auto info = internal.make_info();
        detail::merge_with(classes_, info.name(), info, &class_info::merge);
    }

    inline void namespace_::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }

    template < typename Enum >
    inline void namespace_::add_(const enum_<Enum>& internal) {
        auto info = internal.make_info();
        detail::merge_with(enums_, info.name(), info, &enum_info::merge);
    }

    template < typename Function >
    inline void namespace_::add_(const function_<Function>& internal) {
        auto info = internal.make_info();
        detail::merge_with(functions_, info.name(), info, &function_info::merge);
    }

    inline void namespace_::add_(const namespace_& internal) {
        auto info = internal.make_info();
        detail::merge_with(namespaces_, info.name(), info, &namespace_info::merge);
    }
}
