/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_class.hpp"
#include "meta_data.hpp"
#include "meta_function.hpp"
#include "meta_variable.hpp"

#include "meta_namespace_info.hpp"

namespace meta_hpp
{
    class namespace_ {
    public:
        explicit namespace_(std::string id)
        : info_{std::move(id)} {}

        const namespace_info& make_info() const {
            return info_;
        }

        template < typename... Internals >
        namespace_& operator()(Internals&&...internals) {
            (add_(std::forward<Internals>(internals)), ...);
            return *this;
        }
    private:
        template < typename InternalClass >
        void add_(class_<InternalClass> internal) {
            class_info info = std::move(internal).make_info();
            detail::merge_with(info_.classes_, info.id(), info, &class_info::merge);
        }

        void add_(data_ internal) {
            data_info info = std::move(internal).make_info();
            detail::merge_with(info_.datas_, info.id(), info, &data_info::merge);
        }

        template < typename FunctionType >
        void add_(function_<FunctionType> internal) {
            function_info info = std::move(internal).make_info();
            detail::merge_with(info_.functions_, info.id(), info, &function_info::merge);
        }

        void add_(namespace_ internal) {
            namespace_info info = std::move(internal).make_info();
            detail::merge_with(info_.namespaces_, info.id(), info, &namespace_info::merge);
        }

        template < typename VariableType >
        void add_(variable_<VariableType> internal) {
            variable_info info = std::move(internal).make_info();
            detail::merge_with(info_.variables_, info.id(), info, &variable_info::merge);
        }
    private:
        namespace_info info_;
    };
}
