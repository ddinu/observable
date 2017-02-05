#pragma once
#include <algorithm>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "observable/subject.hpp"
#include "observable/subscription.hpp"
#include "observable/detail/type_traits.hpp"

namespace observable {

//! \cond
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

template <typename ValueType,
          typename EqualityComparator=detail::equal_to,
          typename ...>
class value;

template <typename ValueType>
class value_updater;
//! \endcond

//! Get notified when a value-type changes.
//!
//! When setting a new value, if the new value is different than the existing one,
//! any subscribed observers will be notified.
//!
//! \warning None of the methods in this class can be safely called concurrently.
//!
//! \tparam ValueType The value-type that will be stored inside the observable.
//!                   This type will need to be at least movable.
//! \tparam EqualityComparator A comparator to use when checking if new values
//!                            are different than the stored value. Default is
//!                            ``std::equal_to<>`` if ``ValueType`` is equality-
//!                            comparable, else a comparator that always compares
//!                            false.
//!
//! \ingroup observable
template <typename ValueType, typename EqualityComparator>
class value<ValueType, EqualityComparator>
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

    //! Create an initialized value that will be updated by the provided
    //! value_updater.
    //!
    //! \param ud A value_updater that will be stored by the value.
    template <typename UpdaterType>
    explicit value(std::unique_ptr<UpdaterType> && ud) :
        updater_ { std::move(ud) }
    {
        using namespace std::placeholders;

        updater_->set_value_notifier(
                    std::bind(&value<ValueType, EqualityComparator>::set,
                              this,
                              _1));
        set(updater_->get());
    }

    //! Convert the observable value to its stored value type.
    explicit operator ValueType const &() const noexcept { return value_; }

    //! Retrieve the stored value.
    auto get() const noexcept -> ValueType const & { return value_; }

    //! Subscribe to changes to the observable value.
    //!
    //! These subscriptions will be triggered whenever the stored value changes.
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
    auto subscribe(Callable && callable)
    {
        static_assert(detail::is_compatible_with_subject<Callable, void_subject>::value ||
                      detail::is_compatible_with_subject<Callable, value_subject>::value,
                      "Observer is not valid. Please provide a void observer or an "
                      "observer that takes a ValueType as its only argument.");

        return subscribe_impl(std::forward<Callable>(callable));
    }

    //! Set a new value, possibly notifying any subscribed observers.
    //!
    //! If the new value compares equal to the existing value, this method has no
    //! effect. The comparison is performed using the EqualityComparator.
    //!
    //! \param new_value The new value to set.
    //! \see subject<void(Args ...)>::notify()
    void set(ValueType new_value)
    {
        if(eq_(new_value, value_))
            return;

        value_ = std::move(new_value);
        void_observers_.notify();
        value_observers_.notify(value_);
    }

    //! Set a new value. Will just call set(ValueType &&).
    //!
    //! \see set(ValueType &&)
    auto operator=(ValueType new_value) -> value &
    {
        set(std::move(new_value));
        return *this;
    }

public:
    //! Observable values are **not** copy-constructible.
    value(value<ValueType, EqualityComparator> const &) =delete;

    //! Observable values are **not** copy-assignable.
    auto operator=(value<ValueType, EqualityComparator> const &)
            -> value<ValueType, EqualityComparator> & =delete;

    //! Observable values are move-constructible.
    template <typename OtherEqualityComparator>
    value(value<ValueType, OtherEqualityComparator> && other)
        noexcept(std::is_nothrow_move_constructible<ValueType>::value &&
                 std::is_nothrow_move_constructible<void_subject>::value &&
                 std::is_nothrow_move_constructible<value_subject>::value) :
        value_ { std::move(other.value_) },
        void_observers_ { std::move(other.void_observers_) },
        value_observers_ { std::move(other.value_observers_) },
        updater_ { std::move(other.updater_) }
    {
        using namespace std::placeholders;
        if(updater_)
            updater_->set_value_notifier(
                        std::bind(&value<ValueType, EqualityComparator>::set,
                                  this,
                                  _1));
    }

    //! Observable values are move-assignable.
    template <typename OtherEqualityComparator>
    auto operator=(value<ValueType, OtherEqualityComparator> && other)
        noexcept(std::is_nothrow_move_assignable<ValueType>::value &&
                 std::is_nothrow_move_assignable<void_subject>::value &&
                 std::is_nothrow_move_assignable<value_subject>::value)
        -> value<ValueType, EqualityComparator> &
    {
        using namespace std::placeholders;

        value_ = std::move(other.value_);
        void_observers_ = std::move(other.void_observers_);
        value_observers_ = std::move(other.value_observers_);
        updater_ = std::move(other.updater_);

        if(updater_)
            updater_->set_value_notifier(
                        std::bind(&value<ValueType, EqualityComparator>::set,
                                  this,
                                  _1));
        return *this;
    }

private:
    template <typename Callable>
    auto subscribe_impl(Callable && observer) ->
        std::enable_if_t<detail::is_compatible_with_subject<Callable,
                                                            void_subject>::value,
                         infinite_subscription>
    {
        return void_observers_.subscribe(std::forward<Callable>(observer));
    }

    template <typename Callable>
    auto subscribe_impl(Callable && observer) ->
        std::enable_if_t<detail::is_compatible_with_subject<Callable,
                                                            value_subject>::value,
                         infinite_subscription>
    {
        return value_observers_.subscribe(std::forward<Callable>(observer));
    }

private:
    ValueType value_;
    void_subject void_observers_;
    value_subject value_observers_;
    std::unique_ptr<value_updater<ValueType>> updater_;
    EqualityComparator eq_;

    template <typename, typename, typename ...>
    friend class value;
};

//! Value specialization that can be used inside a class, as a member, to
//! prevent external code from calling set(), but still allow anyone to
//! subscribe.
//!
//! \see value<ValueType, EqualityComparator>
//!
//! This specialization is exactly the same as the main value specialization, but
//! its setters are only accessible from inside the EnclosingType.
//!
//! \tparam ValueType The value-type that will be stored inside the observable.
//! \tparam EqualityComparator A comparator to use when checking if new values
//!                            are different than the stored value.
//! \tparam EnclosingType A type that will have access to the value's setters.
//!
//! \ingroup observable
template <typename ValueType, typename EqualityComparator, typename EnclosingType>
class value<ValueType, EqualityComparator, EnclosingType> :
    public value<ValueType, EqualityComparator>
{
public:
    using value<ValueType, EqualityComparator>::value;

    value() =default;

private:
    using value<ValueType, EqualityComparator>::set;
    using value<ValueType, EqualityComparator>::operator=;

    //! Create an initialized observable value.
    //!
    //! \param initial_value The observable's initial value.
    value(ValueType initial_value) :
        value<ValueType, EqualityComparator>(initial_value)
    { }

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
        template <typename ValueType, typename EqualityComparator=std::equal_to<>>
        using type = value<ValueType, EqualityComparator, EnclosingType>;
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
    ::observable::detail::prop_<Observable_Property_EnclosingType_>::type

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
