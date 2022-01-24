#pragma once

// Stringify
#define OBSERVABLE_S(a) #a

/**
 * Configure the compiler warnings.
 */
#if defined(__clang__)
    #define OBSERVABLE_BEGIN_CONFIGURE_WARNINGS \
        _Pragma(OBSERVABLE_S(clang diagnostic push)) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wweak-vtables")) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wc++98-compat")) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wpadded")) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wdocumentation-unknown-command")) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wglobal-constructors")) \
        _Pragma(OBSERVABLE_S(clang diagnostic ignored "-Wexit-time-destructors"))
#elif defined(__GNUC__) || defined(__GNUG__)
    #define OBSERVABLE_BEGIN_CONFIGURE_WARNINGS \
        _Pragma(OBSERVABLE_S(GCC diagnostic push))
#elif defined(_MSC_VER)
    #define OBSERVABLE_BEGIN_CONFIGURE_WARNINGS \
        __pragma(warning(push))
#endif

/**
 * Restore the original compiler warning configuration.
 */
#if defined(__clang__)
    #define OBSERVABLE_END_CONFIGURE_WARNINGS \
        _Pragma(OBSERVABLE_S(clang diagnostic pop))
#elif defined(__GNUC__) || defined(__GNUG__)
    #define OBSERVABLE_END_CONFIGURE_WARNINGS \
        _Pragma(OBSERVABLE_S(GCC diagnostic pop))
#elif defined(_MSC_VER)
    #define OBSERVABLE_END_CONFIGURE_WARNINGS \
        __pragma(warning(pop))
#endif
