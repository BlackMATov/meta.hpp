/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2023, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"
#include "meta_registry.hpp"
#include "meta_states.hpp"
#include "meta_types.hpp"

namespace meta_hpp::detail
{
    template < typename Class, typename... Args >
    concept class_bind_constructor_kind            //
        = class_kind<Class>                        //
       && std::is_constructible_v<Class, Args...>; //

    template < typename Class >
    concept class_bind_destructor_kind                        //
        = class_kind<Class> && std::is_destructible_v<Class>; //

    template < typename Base, typename Class >
    concept class_bind_base_kind                //
        = class_kind<Base> && class_kind<Class> //
       && std::derived_from<Class, Base>;       //

    template < typename Member, typename Class >
    concept class_bind_member_kind                                           //
        = member_pointer_kind<Member> && class_kind<Class>                   //
       && std::is_same_v<Class, typename member_traits<Member>::class_type>; //

    template < typename Method, typename Class >
    concept class_bind_method_kind                                           //
        = method_pointer_kind<Method> && class_kind<Class>                   //
       && std::is_same_v<Class, typename method_traits<Method>::class_type>; //
}

namespace meta_hpp
{
    template < type_family Type >
    class type_bind_base {
    public:
        explicit type_bind_base(const Type& type, metadata_map metadata)
        : data_{detail::type_access(type)} {
            detail::insert_or_assign(data_->metadata, std::move(metadata));
        }

        operator Type() const noexcept {
            return Type{data_};
        }

    protected:
        using data_ptr = typename detail::type_traits<Type>::data_ptr;
        using data_ref = decltype(*std::declval<data_ptr>());

        [[nodiscard]] data_ref get_data() noexcept {
            return *data_;
        }

    private:
        data_ptr data_;
        detail::type_registry::locker locker_;
    };

    template < state_family State >
    class state_bind_base {
    public:
        explicit state_bind_base(const State& state, metadata_map metadata)
        : state_{detail::state_access(state)} {
            detail::insert_or_assign(state_->metadata, std::move(metadata));
        }

        operator State() const noexcept {
            return State{state_};
        }

    protected:
        using state_ptr = typename detail::state_traits<State>::state_ptr;
        using state_ref = decltype(*std::declval<state_ptr>());

        [[nodiscard]] state_ref get_state() noexcept {
            return *state_;
        }

    private:
        state_ptr state_;
        detail::state_registry::locker locker_;
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    class array_bind final : public type_bind_base<array_type> {
    public:
        explicit array_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::class_kind Class >
    class class_bind final : public type_bind_base<class_type> {
    public:
        explicit class_bind(metadata_map metadata);

        template < detail::class_bind_base_kind<Class>... Bases >
        class_bind& base_();

        template < typename... Args, typename... Opts >
            requires detail::class_bind_constructor_kind<Class, Args...>
        class_bind& constructor_(Opts&&... opts);

        template < typename... Opts >
            requires detail::class_bind_destructor_kind<Class>
        class_bind& destructor_(Opts&&... opts);

        template < detail::function_pointer_kind Function, typename... Opts >
        class_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

        template < detail::class_bind_member_kind<Class> Member, typename... Opts >
        class_bind& member_(std::string name, Member member_ptr, Opts&&... opts);

        template < detail::class_bind_method_kind<Class> Method, typename... Opts >
        class_bind& method_(std::string name, Method method_ptr, Opts&&... opts);

        template < typename Type >
        class_bind& typedef_(std::string name);

        template < detail::pointer_kind Pointer, typename... Opts >
        class_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < detail::enum_kind Enum >
    class enum_bind final : public type_bind_base<enum_type> {
    public:
        explicit enum_bind(metadata_map metadata);

        template < typename... Opts >
        enum_bind& evalue_(std::string name, Enum value, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < detail::function_kind Function >
    class function_bind final : public type_bind_base<function_type> {
    public:
        explicit function_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::member_pointer_kind Member >
    class member_bind final : public type_bind_base<member_type> {
    public:
        explicit member_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::method_pointer_kind Method >
    class method_bind final : public type_bind_base<method_type> {
    public:
        explicit method_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::nullptr_kind Nullptr >
    class nullptr_bind final : public type_bind_base<nullptr_type> {
    public:
        explicit nullptr_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::number_kind Number >
    class number_bind final : public type_bind_base<number_type> {
    public:
        explicit number_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::pointer_kind Pointer >
    class pointer_bind final : public type_bind_base<pointer_type> {
    public:
        explicit pointer_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::reference_kind Reference >
    class reference_bind final : public type_bind_base<reference_type> {
    public:
        explicit reference_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    template < detail::void_kind Void >
    class void_bind final : public type_bind_base<void_type> {
    public:
        explicit void_bind(metadata_map metadata);
    };
}

namespace meta_hpp
{
    class scope_bind final : public state_bind_base<scope> {
    public:
        explicit scope_bind(const scope& scope, metadata_map metadata);

        template < detail::function_pointer_kind Function, typename... Opts >
        scope_bind& function_(std::string name, Function function_ptr, Opts&&... opts);

        template < typename Type >
        scope_bind& typedef_(std::string name);

        template < detail::pointer_kind Pointer, typename... Opts >
        scope_bind& variable_(std::string name, Pointer variable_ptr, Opts&&... opts);
    };
}

namespace meta_hpp
{
    template < detail::array_kind Array >
    array_bind<Array> array_(metadata_map metadata = {}) {
        return array_bind<Array>{std::move(metadata)};
    }

    template < detail::class_kind Class >
    class_bind<Class> class_(metadata_map metadata = {}) {
        return class_bind<Class>{std::move(metadata)};
    }

    template < detail::enum_kind Enum >
    enum_bind<Enum> enum_(metadata_map metadata = {}) {
        return enum_bind<Enum>{std::move(metadata)};
    }

    template < detail::function_kind Function >
    function_bind<Function> function_(metadata_map metadata = {}) {
        return function_bind<Function>{std::move(metadata)};
    }

    template < detail::member_pointer_kind Member >
    member_bind<Member> member_(metadata_map metadata = {}) {
        return member_bind<Member>{std::move(metadata)};
    }

    template < detail::method_pointer_kind Method >
    method_bind<Method> method_(metadata_map metadata = {}) {
        return method_bind<Method>{std::move(metadata)};
    }

    template < detail::nullptr_kind Nullptr >
    nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {}) {
        return nullptr_bind<Nullptr>{std::move(metadata)};
    }

    template < detail::number_kind Number >
    number_bind<Number> number_(metadata_map metadata = {}) {
        return number_bind<Number>{std::move(metadata)};
    }

    template < detail::pointer_kind Pointer >
    pointer_bind<Pointer> pointer_(metadata_map metadata = {}) {
        return pointer_bind<Pointer>{std::move(metadata)};
    }

    template < detail::reference_kind Reference >
    reference_bind<Reference> reference_(metadata_map metadata = {}) {
        return reference_bind<Reference>{std::move(metadata)};
    }

    template < detail::void_kind Void >
    void_bind<Void> void_(metadata_map metadata = {}) {
        return void_bind<Void>{std::move(metadata)};
    }
}

namespace meta_hpp
{
    inline scope_bind local_scope_(std::string name, metadata_map metadata = {}) {
        scope local_scope{detail::scope_state::make(std::move(name), std::move(metadata))};
        return scope_bind{local_scope, {}};
    }

    inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
        scope static_scope{resolve_scope(name)};
        return scope_bind{static_scope, std::move(metadata)};
    }

    inline scope_bind extend_scope_(const scope& scope, metadata_map metadata = {}) {
        return scope_bind{scope, std::move(metadata)};
    }
}

namespace meta_hpp
{
    class arguments_bind final {
    public:
        class argument_info final {
        public:
            argument_info() = default;
            ~argument_info() = default;

            argument_info(argument_info&&) = default;
            argument_info& operator=(argument_info&&) = default;

            argument_info(const argument_info&) = delete;
            argument_info& operator=(const argument_info&) = delete;

            argument_info(std::string name)
            : name_{std::move(name)} {}

            argument_info(std::string name, metadata_map metadata)
            : name_{std::move(name)}
            , metadata_{std::move(metadata)} {}

            [[nodiscard]] std::string& get_name() noexcept {
                return name_;
            }

            [[nodiscard]] const std::string& get_name() const noexcept {
                return name_;
            }

            [[nodiscard]] metadata_map& get_metadata() noexcept {
                return metadata_;
            }

            [[nodiscard]] const metadata_map& get_metadata() const noexcept {
                return metadata_;
            }

        private:
            std::string name_;
            metadata_map metadata_;
        };

    public:
        using values_t = std::vector<argument_info>;

        operator values_t() && {
            return std::move(values_);
        }

        template < typename... Opts >
        static values_t from_opts(Opts&&... opts) {
            arguments_bind bind;

            const auto process_opt = detail::overloaded{
                [&bind](arguments_bind b) { bind(std::move(b)); },
                [&bind](arguments_bind::values_t vs) { bind(std::move(vs)); },
                [](auto&&) {}, // // ignore other opts
            };

            (process_opt(META_HPP_FWD(opts)), ...);
            return std::move(bind.values_);
        }

    public:
        arguments_bind() = default;
        ~arguments_bind() = default;

        arguments_bind(arguments_bind&&) = default;
        arguments_bind(const arguments_bind&) = delete;

        arguments_bind& operator=(arguments_bind&&) = default;
        arguments_bind& operator=(const arguments_bind&) = delete;

        arguments_bind& operator()(std::string name) & {
            values_.emplace_back(std::move(name));
            return *this;
        }

        arguments_bind operator()(std::string name) && {
            (*this)(std::move(name));
            return std::move(*this);
        }

        arguments_bind& operator()(std::string name, metadata_map metadata) & {
            values_.emplace_back(std::move(name), std::move(metadata));
            return *this;
        }

        arguments_bind operator()(std::string name, metadata_map metadata) && {
            (*this)(std::move(name), std::move(metadata));
            return std::move(*this);
        }

        arguments_bind& operator()(values_t values) & {
            values_.insert( //
                values_.end(),
                std::make_move_iterator(values.begin()),
                std::make_move_iterator(values.end())
            );
            return *this;
        }

        arguments_bind operator()(values_t values) && {
            (*this)(std::move(values));
            return std::move(*this);
        }

        arguments_bind& operator()(arguments_bind bind) & {
            values_.insert( //
                values_.end(),
                std::make_move_iterator(bind.values_.begin()),
                std::make_move_iterator(bind.values_.end())
            );
            return *this;
        }

        arguments_bind operator()(arguments_bind bind) && {
            (*this)(std::move(bind));
            return std::move(*this);
        }

    private:
        values_t values_;
    };

    inline arguments_bind arguments_() {
        return arguments_bind{};
    }

    inline arguments_bind argument_(std::string name) {
        return arguments_()(std::move(name));
    }

    inline arguments_bind argument_(std::string name, metadata_map metadata) {
        return arguments_()(std::move(name), std::move(metadata));
    }
}

namespace meta_hpp
{
    class metadata_bind final {
    public:
        using values_t = metadata_map;

        operator values_t() && {
            return std::move(values_);
        }

        template < typename... Opts >
        static values_t from_opts(Opts&&... opts) {
            metadata_bind bind;

            const auto process_opt = detail::overloaded{
                [&bind](metadata_bind b) { bind(std::move(b)); },
                [&bind](metadata_bind::values_t vs) { bind(std::move(vs)); },
                [](auto&&) {}, // // ignore other opts
            };

            (process_opt(META_HPP_FWD(opts)), ...);
            return std::move(bind.values_);
        }

    public:
        metadata_bind() = default;
        ~metadata_bind() = default;

        metadata_bind(metadata_bind&&) = default;
        metadata_bind(const metadata_bind&) = delete;

        metadata_bind& operator=(metadata_bind&&) = default;
        metadata_bind& operator=(const metadata_bind&) = delete;

        metadata_bind& operator()(std::string name, uvalue value) & {
            values_.insert_or_assign(std::move(name), std::move(value));
            return *this;
        }

        metadata_bind operator()(std::string name, uvalue value) && {
            (*this)(std::move(name), std::move(value));
            return std::move(*this);
        }

        metadata_bind& operator()(values_t values) & {
            detail::insert_or_assign(values_, std::move(values));
            return *this;
        }

        metadata_bind operator()(values_t values) && {
            (*this)(std::move(values));
            return std::move(*this);
        }

        metadata_bind& operator()(metadata_bind bind) & {
            detail::insert_or_assign(values_, std::move(bind.values_));
            return *this;
        }

        metadata_bind operator()(metadata_bind bind) && {
            (*this)(std::move(bind));
            return std::move(*this);
        }

    private:
        values_t values_;
    };

    inline metadata_bind metadata_() {
        return metadata_bind{};
    }

    inline metadata_bind metadata_(std::string name, uvalue value) {
        return metadata_()(std::move(name), std::move(value));
    }
}
