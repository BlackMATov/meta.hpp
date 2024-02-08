/*******************************************************************************
 * This file is part of the "https://github.com/blackmatov/meta.hpp"
 * For conditions of distribution and use, see copyright notice in LICENSE.md
 * Copyright (C) 2021-2024, by Matvey Cherevko (blackmatov@gmail.com)
 ******************************************************************************/

#pragma once

#include "meta_base.hpp"

#include "meta_binds.hpp"
#include "meta_binds/array_bind.hpp"
#include "meta_binds/class_bind.hpp"
#include "meta_binds/enum_bind.hpp"
#include "meta_binds/function_bind.hpp"
#include "meta_binds/member_bind.hpp"
#include "meta_binds/method_bind.hpp"
#include "meta_binds/nullptr_bind.hpp"
#include "meta_binds/number_bind.hpp"
#include "meta_binds/pointer_bind.hpp"
#include "meta_binds/reference_bind.hpp"
#include "meta_binds/scope_bind.hpp"
#include "meta_binds/void_bind.hpp"

#include "meta_details.hpp"

#include "meta_indices.hpp"
#include "meta_indices/argument_index.hpp"
#include "meta_indices/constructor_index.hpp"
#include "meta_indices/destructor_index.hpp"
#include "meta_indices/evalue_index.hpp"
#include "meta_indices/function_index.hpp"
#include "meta_indices/member_index.hpp"
#include "meta_indices/method_index.hpp"
#include "meta_indices/scope_index.hpp"
#include "meta_indices/variable_index.hpp"

#include "meta_invoke.hpp"
#include "meta_invoke/invoke.hpp"

#include "meta_policies.hpp"

#include "meta_registry.hpp"

#include "meta_states.hpp"
#include "meta_states/argument.hpp"
#include "meta_states/constructor.hpp"
#include "meta_states/destructor.hpp"
#include "meta_states/evalue.hpp"
#include "meta_states/function.hpp"
#include "meta_states/member.hpp"
#include "meta_states/method.hpp"
#include "meta_states/scope.hpp"
#include "meta_states/variable.hpp"

#include "meta_types.hpp"
#include "meta_types/any_type.hpp"
#include "meta_types/array_type.hpp"
#include "meta_types/class_type.hpp"
#include "meta_types/constructor_type.hpp"
#include "meta_types/destructor_type.hpp"
#include "meta_types/enum_type.hpp"
#include "meta_types/function_type.hpp"
#include "meta_types/member_type.hpp"
#include "meta_types/method_type.hpp"
#include "meta_types/nullptr_type.hpp"
#include "meta_types/number_type.hpp"
#include "meta_types/pointer_type.hpp"
#include "meta_types/reference_type.hpp"
#include "meta_types/void_type.hpp"

#include "meta_ucast.hpp"
#include "meta_ucast/ucast.hpp"

#include "meta_uresult.hpp"
#include "meta_uresult/uresult.hpp"

#include "meta_uvalue.hpp"
#include "meta_uvalue/uvalue.hpp"
