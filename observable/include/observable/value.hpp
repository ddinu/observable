#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <observable/subject.hpp>
#include <observable/subscription.hpp>
#include <observable/detail/type_traits.hpp>

#include <observable/detail/compiler_config.hpp>
OBSERVABLE_BEGIN_CONFIGURE_WARNINGS

namespace observable {

//! \cond
template <typename ValueType, typename ...>
class value;

template <typename ValueType>
class value_updater;

namespace detail {

struct equal_to
{
    template <typename A, typename B>
    auto operator()(A && a, B && b) const
        -> std::enable_if_t<are_equality_comparable<A, B>::value, bool>
    {
        return std::equal_to<> { }(std::forward<A>(a), std::forward<B>(b));
    }

    template <typename A, typename B>
    auto operator()(A &&, B &&) const noexcept
        -> std::enable_if_t<!are_equality_comparable<A, B>::value, bool>
    {
        return false;
    }
};

}
//! \endcond

//! Exception thrown if you try to set a value that has an associated updater.
struct readonly_value : std::runtime_error
{
    readonly_value() = delete;
    using std::runtime_error::runtime_error;
};

//! Get notified when a value-type changes.
//!
//! When setting a new value, if the new value is different than the existing one,
//! any subscribed observers will be notified.
//!
//! Equality will be checked using ``std::equal_to<ValueType>`` if the ValueType
//! is EqualityComparable, else all values will be assumed to be unequal.
//!
//! \warning None of the methods in this class can be safely called concurrently.
//!
//! \tparam ValueType The value-type that will be stored inside the observable.
//!                   This type will need to be at least movable.
//!
//! \ingroup observable
template <typename ValueType>
class value<ValueType>
{
    using void_subject = subject<void()>;
    using value_subject = subject<void(ValueType const &)>;

public:
    //! The observable value's stored value type.
    using value_type = ValueType;

    //! Create a default-constructed observable value.
    //!
    //! Depending on the value type, the stored value will be either uninitialized
    //! or it will be default constructed.
    value() =default;

    //! Create an initialized observable value.
    //!
    //! \param initial_value The observable's initial value.
    explicit value(ValueType initial_value)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value) :
        value_ { std::move(initial_value) }
    { }

    //! Create an initialized observable value.
    //!
    //! \param initial_value The observable's initial value.
    //! \param equal A functor to be used for comparing values. The functor must
    //!              have a signature compatible with the one below:
    //!
    //!                 bool(ValueType const &, ValueType const &)
    //!
    //!              The comparator must return true if both of its parameters
    //!              are equal.
    template <typename EqualityComparator>
    value(ValueType initial_value, EqualityComparator equal)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value &&
                 std::is_nothrow_move_constructible<EqualityComparator>::value) :
        value_ { std::move(initial_value) },
        eq_ { std::move(equal) }
    { }

    //! Create an initialized value that will be updated by the provided
    //! value_updater.
    //!
    //! \param ud A value_updater that will be stored by the value.
    template <typename UpdaterType>
    explicit value(std::unique_ptr<UpdaterType> && ud) :
        updater_ { std::move(ud) }
    {
        using namespace std::placeholders;

        updater_->set_value_notifier(std::bind(&value<ValueType>::set_impl,
                                               this,
                                                std::placeholders::_1));
        set_impl(updater_->get());
    }

    //! Convert the observable value to its stored value type.
    explicit operator ValueType const &() const noexcept { return value_; }

    //! Retrieve the stored value.
    auto get() const noexcept -> ValueType const & { return value_; }

    //! Subscribe to changes to the observable value.
    //!
    //! These subscriptions will be triggered whenever the stored value changes.
    //!
    //! \param[in] observer A callable that will be called whenever the value
    //!                     changes. The observer must satisfy the Callable
    //!                     concept.
    //!
    //! \tparam Callable A callable taking no parameters or a callable taking one
    //!                  parameter that will be called with the new value:
    //!
    //!                  - ``void()`` -- will be called when the value changes but
    //!                    will not receive the new value.
    //!
    //!                  - ``void(T const &)`` or ``void(T)`` -- will be called
    //!                    with the new value. The expression
    //!                    ``T { value.get() }`` must be correct.
    //!
    //! \see subject<void(Args ...)>::subscribe()
    template <typename Callable>
    auto subscribe(Callable && observer) const
    {
        static_assert(detail::is_compatible_with_subject<Callable, void_subject>::value ||
                      detail::is_compatible_with_subject<Callable, value_subject>::value,
                      "Observer is not valid. Please provide a void observer or an "
                      "observer that takes a ValueType as its only argument.");

        return subscribe_impl(std::forward<Callable>(observer));
    }

    //! Subscribe to changes to the observable value and also call the observer
    //! callback immediately with the current value.
    //!
    //! If the observer throws an exception during the initial call, it will
    //! not be subscribed.
    //!
    //! \note The observer is not subscribed during the initial call.
    //!
    //! \param[in] observer A callable that will be called whenever the value
    //!                     changes. The observer must satisfy the Callable
    //!                     concept.
    //!
    //! \tparam Callable A callable taking no parameters or a callable taking
    //!                  one parameter that will be called with the new value:
    //!
    //!                  - ``void()`` -- will be called when the value changes
    //!                    but will not receive the new value.
    //!
    //!                  - ``void(T const &)`` or ``void(T)`` -- will be called
    //!                    with the new value. The expression
    //!                    ``T { value.get() }`` must be correct.
    //!
    //! \see subscribe()
    template <typename Callable>
    auto subscribe_and_call(Callable && observer) const
    {
        call_impl(observer);
        return subscribe(std::forward<Callable>(observer));
    }

    //! Set a new value, possibly notifying any subscribed observers.
    //!
    //! If the new value compares equal to the existing value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    //!
    //! \param new_value The new value to set.
    //! \throw readonly_value if the value has an associated updater.
    //! \see subject<void(Args ...)>::notify()
    void set(ValueType new_value)
    {
        if(updater_)
            throw readonly_value {
                "Can't set a value that has an associated updater. These values "
                "are readonly."
            };

        set_impl(std::move(new_value));
    }

    //! Set a new value. Will just call set(ValueType &&).
    //!
    //! \see set(ValueType &&)
    auto operator=(ValueType new_value) -> value &
    {
        set(std::move(new_value));
        return *this;
    }

    //! Subject notified after the value has been moved.
    //!
    //! The subject's parameter is a reference to the value instance that has been
    //! moved into.
    //!
    //! \warning The subject of the moved-into value will be notified, not the
    //!          moved-from value's subject.
    subject<void(value<ValueType> &), value<ValueType>> moved;

    //! Subject notified before the value is destroyed.
    subject<void(), value<ValueType>> destroyed;

    //! Destructor.
    ~value() { destroyed.notify(); }

public:
    //! Observable values are **not** copy-constructible.
    value(value<ValueType> const &) =delete;

    //! Observable values are **not** copy-assignable.
    auto operator=(value<ValueType> const &) -> value<ValueType> & =delete;

    //! Observable values are move-constructible.
    template <typename = std::enable_if_t<std::is_move_constructible<ValueType>::value>>
    value(value<ValueType> && other)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value) :
        moved { std::move(other.moved) },
        destroyed { std::move(other.destroyed) },
        value_(std::move(other.value_)),
        eq_ { std::move(other.eq_) },
        void_observers_ { std::move(other.void_observers_) },
        value_observers_ { std::move(other.value_observers_) },
        updater_ { std::move(other.updater_) }
    {
        using namespace std::placeholders;
        if(updater_)
            updater_->set_value_notifier(std::bind(&value<ValueType>::set_impl,
                                                   this,
                                                   std::placeholders::_1));

        moved.notify(*this);
        other.destroyed = decltype(destroyed) { };
    }

    //! Observable values are move-assignable.
    template <typename =  std::enable_if_t<std::is_move_assignable<ValueType>::value>>
    auto operator=(value<ValueType> && other)
        noexcept(std::is_nothrow_move_assignable<ValueType>::value)
        -> value<ValueType> &
    {
        using namespace std::placeholders;

        moved = std::move(other.moved);
        destroyed = std::move(other.destroyed);

        value_ = std::move(other.value_);
        void_observers_ = std::move(other.void_observers_);
        value_observers_ = std::move(other.value_observers_);
        updater_ = std::move(other.updater_);
        eq_ = std::move(other.eq_);

        if(updater_)
            updater_->set_value_notifier(std::bind(&value<ValueType>::set_impl,
                                                   this,
                                                   std::placeholders::_1));

        moved.notify(*this);
        other.destroyed = decltype(destroyed) { };
        return *this;
    }

private:
    template <typename Callable>
    auto subscribe_impl(Callable && observer) const ->
        std::enable_if_t<detail::is_compatible_with_subject<Callable, void_subject>::value &&
                         !detail::is_compatible_with_subject<Callable, value_subject>::value,
                         infinite_subscription>
    {
        return void_observers_.subscribe(std::forward<Callable>(observer));
    }

    template <typename Callable>
    auto subscribe_impl(Callable && observer) const ->
        std::enable_if_t<detail::is_compatible_with_subject<Callable,
                                                            value_subject>::value,
                         infinite_subscription>
    {
        return value_observers_.subscribe(std::forward<Callable>(observer));
    }

    template <typename Callable>
    auto call_impl(Callable && observer) const ->
            std::enable_if_t<detail::is_compatible_with_subject<Callable, void_subject>::value &&
            !detail::is_compatible_with_subject<Callable, value_subject>::value>
    {
        observer();
    }

    template <typename Callable>
    auto call_impl(Callable && observer) const ->
            std::enable_if_t<detail::is_compatible_with_subject<Callable,
            value_subject>::value>
    {
        observer(value_);
    }

    void set_impl(ValueType new_value)
    {
        if(eq_(value_, new_value))
            return;

        value_ = std::move(new_value);
        void_observers_.notify();
        value_observers_.notify(value_);
    }

private:
    ValueType value_;

    std::function<bool(ValueType const &, ValueType const &)> eq_ {
        [](auto && a, auto && b) { return detail::equal_to { }(a, b); }
    };

    mutable void_subject void_observers_;
    mutable value_subject value_observers_;
    std::unique_ptr<value_updater<ValueType>> updater_;

    template <typename, typename ...>
    friend class value;
};

//! Value specialization that can be used inside a class, as a member, to
//! prevent external code from calling set(), but still allow anyone to
//! subscribe.
//!
//! \see value<ValueType>
//!
//! This specialization is exactly the same as the main value specialization, but
//! its setters are only accessible from inside the EnclosingType.
//!
//! \tparam ValueType The value-type that will be stored inside the observable.
//! \tparam EnclosingType A type that will have access to the value's setters.
//!
//! \ingroup observable
template <typename ValueType, typename EnclosingType>
class value<ValueType, EnclosingType> :
    public value<ValueType>
{
public:
    using value<ValueType>::value;

    value() =default;

private:
    using value<ValueType>::set;
    using value<ValueType>::operator=;

    value(value<ValueType, EnclosingType> &&) =default;

    auto operator=(value<ValueType, EnclosingType> &&)
         -> value<ValueType, EnclosingType> & =default;

    value(value<ValueType> && other) :
        value<ValueType> { std::move(other) }
    { }

    auto operator=(value<ValueType> && other) -> value<ValueType, EnclosingType> &
    {
        *static_cast<value<ValueType> *>(this) = std::move(other);
        return *this;
    }

    friend EnclosingType;
};

// Properties

//! Macro that enables observable properties for a class.
//!
//! You **must** use this macro inside a class that will have observable_property
//! members.
//!
//! To enable observable properties, just use the macro, with the class type as
//! a parameter before declaring the first observable property member.
//!
//! Example:
//!
//!     class MyClass
//!     {
//!         OBSERVABLE_PROPERTIES(MyClass)
//!
//!     public:
//!         observable_property<int> my_val;
//!     };
//!
//! \ingroup observable
#define OBSERVABLE_PROPERTIES(EnclosingType) \
    using Observable_Property_EnclosingType_ = EnclosingType;

namespace detail {
    //! \cond
    template <typename EnclosingType>
    struct prop_
    {
        template <typename ValueType>
        using type = value<ValueType, EnclosingType>;
    };
    //! \endcond
}

//! Declare an observable property member of a class.
//!
//! \note You must use the \ref OBSERVABLE_PROPERTIES macro before declaring any
//!       observable_property members inside a class.
//!
//! The macro expands to an observable value that takes two template parameters:
//! ValueType and EqualityComparator.
//!
//! The value's setters will only be accessible from inside the class passed as
//! a parameter to the \ref OBSERVABLE_PROPERTIES macro.
//!
//! \see observable::value<ValueType, EqualityComparator>
//! \see observable::value<ValueType, EqualityComparator, EnclosingType>
//! \ingroup observable
#define observable_property \
    typename ::observable::detail::prop_<Observable_Property_EnclosingType_>::type

//! Interface used to update a value.
//!
//! \ingroup observable
template <typename ValueType>
class value_updater
{
public:
    //! Set a functor that can be used to notify the value to be updated of a
    //! change.
    //!
    //! \param[in] notifier Functor that will notify the value of a change.
    virtual void set_value_notifier(std::function<void(ValueType &&)> const & notifier) =0;

    //! Retrieve the current value.
    virtual auto get() const -> ValueType =0;

    //! Destructor.
    virtual ~value_updater() { }

    //! Value updaters are default-constructible.
    value_updater() =default;

    //! Value updaters are copy-constructible.
    value_updater(value_updater const &) =default;

    //! Value updaters are move-constructible.
    value_updater(value_updater &&) =default;

    //! Value updaters are copy-assignable.
    value_updater & operator=(value_updater const &) =default;

    //! Value updaters are move-assignable.
    value_updater & operator=(value_updater &&) =default;
};

//! \cond
template <typename ...>
struct is_value_ : std::false_type { };

template <typename T, typename ... R>
struct is_value_<value<T, R ...>> : std::true_type { };
//! \endcond

//! Check if a type is a value.
//!
//! \ingroup observable_detail
template <typename T>
struct is_value : is_value_<std::decay_t<T>> { };

}

OBSERVABLE_END_CONFIGURE_WARNINGS
