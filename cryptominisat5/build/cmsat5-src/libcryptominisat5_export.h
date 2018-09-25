
#ifndef LIBCRYPTOMINISAT5_EXPORT_H
#define LIBCRYPTOMINISAT5_EXPORT_H

#ifdef LIBCRYPTOMINISAT5_STATIC_DEFINE
#  define LIBCRYPTOMINISAT5_EXPORT
#  define LIBCRYPTOMINISAT5_NO_EXPORT
#else
#  ifndef LIBCRYPTOMINISAT5_EXPORT
#    ifdef libcryptominisat5_EXPORTS
        /* We are building this library */
#      define LIBCRYPTOMINISAT5_EXPORT 
#    else
        /* We are using this library */
#      define LIBCRYPTOMINISAT5_EXPORT 
#    endif
#  endif

#  ifndef LIBCRYPTOMINISAT5_NO_EXPORT
#    define LIBCRYPTOMINISAT5_NO_EXPORT 
#  endif
#endif

#ifndef LIBCRYPTOMINISAT5_DEPRECATED
#  define LIBCRYPTOMINISAT5_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBCRYPTOMINISAT5_DEPRECATED_EXPORT
#  define LIBCRYPTOMINISAT5_DEPRECATED_EXPORT LIBCRYPTOMINISAT5_EXPORT LIBCRYPTOMINISAT5_DEPRECATED
#endif

#ifndef LIBCRYPTOMINISAT5_DEPRECATED_NO_EXPORT
#  define LIBCRYPTOMINISAT5_DEPRECATED_NO_EXPORT LIBCRYPTOMINISAT5_NO_EXPORT LIBCRYPTOMINISAT5_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBCRYPTOMINISAT5_NO_DEPRECATED
#    define LIBCRYPTOMINISAT5_NO_DEPRECATED
#  endif
#endif

#endif
