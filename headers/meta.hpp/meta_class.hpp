/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_ctor.hpp"
#include "meta_data.hpp"
#include "meta_field.hpp"
#include "meta_function.hpp"
#include "meta_method.hpp"
#include "meta_variable.hpp"

#include "meta_class_info.hpp"

namespace meta_hpp
{
    template < typename Class >
    class class_ {
    public:
        explicit class_(std::string id)
        : info_{detail::typename_arg<Class>, std::move(id)} {}

        const class_info& make_info() const {
            return info_;
        }

        template < typename... Internals >
        class_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        template < typename InternalClass >
        void add_(class_<InternalClass> internal) {
            class_info info = std::move(internal).make_info();
            detail::merge_with(info_.classes_, info.id(), info, &class_info::merge);
        }

        template < typename... Args >
        void add_(ctor_<Args...> internal) {
            ctor_info info = std::move(internal).template make_info<Class>();
            detail::merge_with(info_.ctors_, info.family(), info, &ctor_info::merge);
        }

        void add_(data_ internal) {
            data_info info = std::move(internal).make_info();
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }

        template < typename FieldType >
        void add_(field_<FieldType> internal) {
            field_info info = std::move(internal).make_info();
            detail::merge_with(info_.fields_, info.id(), info, &field_info::merge);
        }

        template < typename FunctionType >
        void add_(function_<FunctionType> internal) {
            function_info info = std::move(internal).make_info();
            detail::merge_with(info_.functions_, info.id(), info, &function_info::merge);
        }

        template < typename MethodType >
        void add_(method_<MethodType> internal) {
            method_info info = std::move(internal).make_info();
            detail::merge_with(info_.methods_, info.id(), info, &method_info::merge);
        }

        template < typename VariableType >
        void add_(variable_<VariableType> internal) {
            variable_info info = std::move(internal).make_info();
            detail::merge_with(info_.variables_, info.id(), info, &variable_info::merge);
        }
    private:
        class_info info_;
    };
}
