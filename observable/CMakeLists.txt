add_library(observable INTERFACE)

add_custom_target(observable_headers # Just to generate a project in IDEs.
    SOURCES
        include/observable/observable.hpp
        include/observable/observe.hpp
        include/observable/subject.hpp
        include/observable/subscription.hpp
        include/observable/value.hpp
        include/observable/expressions/expression.hpp
        include/observable/expressions/filters.hpp
        include/observable/expressions/math.hpp
        include/observable/expressions/operators.hpp
        include/observable/expressions/tree.hpp
        include/observable/expressions/utility.hpp
        include/observable/detail/collection.hpp
        include/observable/detail/type_traits.hpp
)

target_include_directories(observable
    INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS observable EXPORT Observable
    INCLUDES DESTINATION include
)

install(EXPORT Observable NAMESPACE Observable::
    FILE ObservableTargets.cmake
    DESTINATION lib/cmake/observable)

