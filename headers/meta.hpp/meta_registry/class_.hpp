/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "_registry_fwd.hpp"

#include "ctor_.hpp"
#include "data_.hpp"
#include "enum_.hpp"
#include "function_.hpp"
#include "member_.hpp"
#include "method_.hpp"

namespace meta_hpp
{
    template < typename Class >
    class class_ final {
    public:
        explicit class_(std::string name);

        class_info make_info() const;
    public:
        template < typename... Internals >
        class_& operator()(Internals&&...internals);
    private:
        template < typename Class2 >
        void add_(const class_<Class2>& internal);
        template < typename... Args >
        void add_(const ctor_<Args...>& internal);
        void add_(const data_& internal);
        template < typename Enum >
        void add_(const enum_<Enum>& internal);
        template < typename Function >
        void add_(const function_<Function>& internal);
        template < typename Member >
        void add_(const member_<Member>& internal);
        template < typename Method >
        void add_(const method_<Method>& internal);
        void add_(...) = delete;
    private:
        std::string name_;
        class_info_map classes_;
        ctor_info_map ctors_;
        data_info_map datas_;
        enum_info_map enums_;
        function_info_map functions_;
        member_info_map members_;
        method_info_map methods_;
    };
}

namespace meta_hpp
{
    template < typename Class >
    inline class_<Class>::class_(std::string name)
    : name_{std::move(name)} {}

    template < typename Class >
    inline class_info class_<Class>::make_info() const {
        class_info info{typename_arg<Class>, name_};
        info.state_->classes.insert(classes_.begin(), classes_.end());
        info.state_->ctors.insert(ctors_.begin(), ctors_.end());
        info.state_->datas.insert(datas_.begin(), datas_.end());
        info.state_->enums.insert(enums_.begin(), enums_.end());
        info.state_->functions.insert(functions_.begin(), functions_.end());
        info.state_->members.insert(members_.begin(), members_.end());
        info.state_->methods.insert(methods_.begin(), methods_.end());
        return info;
    }

    template < typename Class >
    template < typename... Internals >
    inline class_<Class>& class_<Class>::operator()(Internals&&...internals) {
        (add_(std::forward<Internals>(internals)), ...);
        return *this;
    }
}

namespace meta_hpp
{
    template < typename Class >
    template < typename Class2 >
    inline void class_<Class>::add_(const class_<Class2>& internal) {
        auto info = internal.make_info();
        detail::merge_with(classes_, info.name(), info, &class_info::merge);
    }

    template < typename Class >
    template < typename... Args >
    inline void class_<Class>::add_(const ctor_<Args...>& internal) {
        auto info = internal.template make_info<Class>();
        detail::merge_with(ctors_, info.type().id(), info, &ctor_info::merge);
    }

    template < typename Class >
    inline void class_<Class>::add_(const data_& internal) {
        auto info = internal.make_info();
        detail::merge_with(datas_, info.name(), info, &data_info::merge);
    }

    template < typename Class >
    template < typename Enum >
    inline void class_<Class>::add_(const enum_<Enum>& internal) {
        auto info = internal.make_info();
        detail::merge_with(enums_, info.name(), info, &enum_info::merge);
    }

    template < typename Class >
    template < typename Function >
    inline void class_<Class>::add_(const function_<Function>& internal) {
        auto info = internal.make_info();
        detail::merge_with(functions_, info.name(), info, &function_info::merge);
    }

    template < typename Class >
    template < typename Member >
    inline void class_<Class>::add_(const member_<Member>& internal) {
        auto info = internal.make_info();
        detail::merge_with(members_, info.name(), info, &member_info::merge);
    }

    template < typename Class >
    template < typename Method >
    inline void class_<Class>::add_(const method_<Method>& internal) {
        auto info = internal.make_info();
        detail::merge_with(methods_, info.name(), info, &method_info::merge);
    }
}
