#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <typeinfo>
#include <type_traits>
#include <utility>

namespace observable { namespace detail {

//! Wrap a std::function<void(...)> to hide its type.
class function_wrapper
{
    static constexpr auto storage_size = sizeof(std::function<void()>);
    static constexpr auto storage_align = alignof(std::function<void()>);

    template <typename Function, typename FunctionType>
    struct trampoline;

    template <typename Function, typename ... Arguments>
    struct trampoline<Function, void(Arguments ...)>
    {
        explicit constexpr trampoline(Function * function) noexcept :
            function_(function)
        {
        }

        void operator()(Arguments ... arguments)
        {
            (*function_)(arguments ...);
        }

    private:
        Function * function_;
    };

public:
    using id_type = std::size_t;
    using type_id_type = std::size_t;

    //! Create a new function wrapper.
    template <typename FunctionType>
    function_wrapper(std::function<FunctionType> fun,
                     std::size_t type_id,
                     std::size_t instance_id) :
        type_id_(type_id),
        instance_id_(instance_id)
    {
        using function_type = std::function<FunctionType>;
        static_assert(sizeof(function_type) <= storage_size,
                      "Function too big for wrapper storage.");

#if !defined(NDEBUG)
        type_ = &typeid(function_type);
#endif

        function_ = new (&storage_) function_type { std::move(fun) };

        delete_ = [](auto * f) noexcept {
            reinterpret_cast<function_type *>(f)->~function();
        };

        copy_ = [](auto const * src_function,
                   auto * & dst_function,
                   auto & dst_storage) {
                    dst_function = new(&dst_storage) function_type {
                        *reinterpret_cast<function_type const *>(src_function)
                    };
                };

        move_ = [](auto * & src_function,
                   auto * & dst_function,
                   auto & dst_storage) {
                    dst_function = new(&dst_storage) function_type {
                        std::move(*reinterpret_cast<function_type *>(src_function))
                    };
                    src_function = nullptr;
                };
    }

    //! Create an invalid function wrapper.
    function_wrapper() =default;

    //! Retrieve a process-unique type id for the wrapped type.
    auto type_id() const noexcept { return type_id_; }

    //! Retrieve a process-unique id identifying this wrapper instance.
    auto instance_id() const noexcept { return instance_id_; }

    //! Call the wrapped function with the provided arguments.
    //!
    //! \warning The provided argument types must exactly match the wrapped
    //!          function's arguments.
    template <typename FunctionType>
    auto function() const
#if defined(NDEBUG)
        noexcept
#endif
    {
        using function_type = std::function<FunctionType>;
        static_assert(sizeof(function_type) <= storage_size,
                      "Function too big for wrapper storage.");
        assert(*type_ == typeid(function_type));

        auto fun = reinterpret_cast<function_type const *>(function_);

        return trampoline<function_type const, FunctionType> { fun };
    }

    //! Destructor.
    ~function_wrapper()
    {
        if(function_)
            delete_(function_);
    }

    //! Function wrappers are copy-constructible.
    function_wrapper(function_wrapper const & other) :
        type_id_(other.type_id_),
        instance_id_(other.instance_id_),
        delete_(other.delete_),
        copy_(other.copy_),
        move_(other.move_)
    {
        copy_(other.function_, function_, storage_);

#if !defined(NDEBUG)
        type_ = other.type_;
#endif
    }

    //! Function wrappers are move-constructible.
    function_wrapper(function_wrapper && other) :
        type_id_(std::move(other.type_id_)),
        instance_id_(std::move(other.instance_id_)),
        delete_(std::move(other.delete_)),
        copy_(std::move(other.copy_)),
        move_(std::move(other.move_))
    {
        move_(other.function_, function_, storage_);

        other.delete_ = [](auto) { };
        other.copy_ = [](auto, auto, auto) { };
        other.move_ = [](auto, auto, auto) { };
        other.function_ = nullptr;

#if !defined(NDEBUG)
        type_ = std::move(other.type_);
#endif
    }

    //! Function wrappers are copy-assignable and move-assignable.
    function_wrapper & operator=(function_wrapper other)
    {
        swap(*this, other);
        return *this;
    }

    //! Swap two function wrappers.
    friend void swap(function_wrapper & a, function_wrapper & b)
    {
        using std::swap;
        swap(a.type_id_, b.type_id_);
        swap(a.instance_id_, b.instance_id_);

        {
            decltype(a.function_) function = nullptr;
            decltype(a.storage_) storage;

            a.move_(a.function_, function, storage);
            b.move_(b.function_, a.function_, a.storage_);
            a.move_(function, b.function_, b.storage_);
        }

        swap(a.delete_, b.delete_);
        swap(a.copy_, b.copy_);
        swap(a.move_, b.move_);

#if !defined(NDEBUG)
        swap(a.type_, b.type_);
#endif
    }

private:
    type_id_type type_id_ = 0;
    id_type instance_id_ = 0;
    void * function_ = nullptr;
    typename std::aligned_storage<storage_size, storage_align>::type storage_;

    std::function<void(void *)> delete_ = [](auto) { };
    std::function<void(void const *, void * &, decltype(storage_) &)> copy_ = [](auto, auto, auto) { };
    std::function<void(void * &, void * &, decltype(storage_) &)> move_ = [](auto, auto, auto) { };

#if !defined(NDEBUG)
    std::type_info const * type_;
#endif
};


} }
