/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_fwd.hpp"

#include "meta_class_info.hpp"
#include "meta_field_info.hpp"
#include "meta_function_info.hpp"
#include "meta_method_info.hpp"
#include "meta_namespace_info.hpp"
#include "meta_variable_info.hpp"

namespace meta_hpp
{
    class type {
    public:
        type() = delete;

        type(type&&) = default;
        type(const type&) = default;

        type& operator=(type&&) = default;
        type& operator=(const type&) = default;

        template < typename Info >
        type(Info&& info)
        : info_{std::forward<Info>(info)} {}

        bool is_class() const noexcept {
            return std::holds_alternative<class_info>(info_);
        }

        bool is_field() const noexcept {
            return std::holds_alternative<field_info>(info_);
        }

        bool is_function() const noexcept {
            return std::holds_alternative<function_info>(info_);
        }

        bool is_method() const noexcept {
            return std::holds_alternative<method_info>(info_);
        }

        bool is_namespace() const noexcept {
            return std::holds_alternative<namespace_info>(info_);
        }

        bool is_variable() const noexcept {
            return std::holds_alternative<variable_info>(info_);
        }

        std::optional<class_info> get_class() const {
            if ( const class_info* info = std::get_if<class_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        std::optional<field_info> get_field() const {
            if ( const field_info* info = std::get_if<field_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        std::optional<function_info> get_function() const {
            if ( const function_info* info = std::get_if<function_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        std::optional<method_info> get_method() const {
            if ( const method_info* info = std::get_if<method_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        std::optional<namespace_info> get_namespace() const {
            if ( const namespace_info* info = std::get_if<namespace_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        std::optional<variable_info> get_variable() const {
            if ( const variable_info* info = std::get_if<variable_info>(&info_); info ) {
                return *info;
            }
            return std::nullopt;
        }

        void merge(const type& other) {
            if ( info_.index() != other.info_.index() ) {
                throw std::logic_error("type::merge failed");
            }
            std::visit(overloaded {
                [&other](auto& info){
                    using info_type = std::remove_reference_t<decltype(info)>;
                    info.merge(std::get<info_type>(other.info_));
                }
            }, info_);
        }
    private:
        std::variant<
            class_info,
            field_info,
            function_info,
            method_info,
            namespace_info,
            variable_info> info_;
    };
}
