#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace observable { namespace detail {

//! Collection for holding and calling heterogeneous generic functions.
//!
//! \note This class needs RTTI to be enabled.
//! \warning This class is copyable but all function instances will be shared
//!          between copies (i.e. a shallow copy).
//! \warning None of the methods defined by this class are safe to be called
//!          concurrently.
class function_collection
{
public:
    //! Identifier for a function that has been inserted. You can use this to
    //! remove a previously added function.
    using function_id = std::intptr_t;

    //! Insert a function into the collection.
    //!
    //! \param function The function to be inserted.
    //! \tparam The function's _normalized_ type (i.e. not a function pointer, but
    //!         the actual function type, for example: `int(double)` ).
    //! \return Function id that can be used to remove the function from the
    //!         collection.
    template <typename FunctionSignature>
    function_id insert(std::function<FunctionSignature> function)
    {
        assert(function);

        auto function_ptr = std::make_shared<decltype(function)>(move(function));
        functions_.emplace(key<FunctionSignature>(), function_ptr);

        return reinterpret_cast<function_id>(function_ptr.get());
    }

    //! Remove a function from the collection.
    //!
    //! If no function with the provided id exists in the collection, this method
    //! does nothing.
    //!
    //! \param id Function id returned by insert.
    //! \return True if the function was a member of the collection and has been
    //!         removed.
    bool remove(function_id const & id)
    {
        auto it = find_if(begin(functions_),
                          end(functions_),
                          [&](auto && kv) {
                              assert(kv.second);
                              return reinterpret_cast<function_id>(kv.second.get()) == id;
                          });

        if(it == end(functions_))
            return false;

        functions_.erase(it);
        return true;
    }

    //! Call all functions that match the provided function signature with the
    //! provided arguments.
    //!
    //! If no function with the provided signature exists, this method does
    //! nothing.
    //!
    //! \param args The arguments to pass to all functions that will be called.
    //! \tparam FunctionSignature The signature of the functions that will be
    //!                           called.
    //! \tparam Arguments Types of the function's arguments.
    //! \warning Return values of the called functions will be ignored.
    //! \return The number of functions that were called.
    template <typename FunctionSignature, typename ... Argument>
    std::size_t call_all(Argument && ... argument) const
    {
        std::size_t call_count = 0;
        auto range = functions_.equal_range(key<FunctionSignature>());

        for(auto i = range.first; i != range.second; ++i, ++call_count)
        {
            assert(i->second);
            auto function = reinterpret_cast<
                                std::function<FunctionSignature> *>(
                                        i->second.get());
            assert(*function);

            (void)(*function)(std::forward<Argument>(argument) ...);
        }

        return call_count;
    }

    //! Check if a function id belongs to this collection.
    bool contains(function_id const & id) const
    {
        return find_if(begin(functions_),
                       end(functions_),
                       [&](auto && kv) {
                           return reinterpret_cast<function_id>(kv.second.get()) == id;
                       }) != end(functions_);
    }

    //! Retrieve the number of functions in the collection.
    std::size_t size() const
    {
        return functions_.size();
    }

    //! Returns true if the collection is empty.
    bool empty() const
    {
        return functions_.empty();
    }

private:
    using function_key = std::type_index;

    //! Retrieve the type index of the provided type.
    template <typename FunctionSignature>
    static constexpr auto key()
    {
        return std::type_index { typeid(FunctionSignature) };
    }

private:
    std::unordered_multimap<
            function_key,
            std::shared_ptr<void>> functions_; //!< Functions by type
};

} }
