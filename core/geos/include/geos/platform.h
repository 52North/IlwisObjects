/* include/geos/platform.h.  Generated from platform.h.in by configure.  */
#ifndef GEOS_PLATFORM_H
#define GEOS_PLATFORM_H

#define NOMINMAX

#include <cmath>
/* Set to 1 if you have `int64_t' type */
#define HAVE_INT64_T_64 1

/* Set to 1 if `long int' is 64 bits */
#define HAVE_LONG_INT_64 1

/* Set to 1 if `long long int' is 64 bits */
/* #undef HAVE_LONG_LONG_INT_64 */

/* Set to 1 if you have ieeefp.h */
#define HAVE_IEEEFP_H 1

/* Has finite */
#define HAVE_FINITE 1

/* Has isfinite */
#define HAVE_ISFINITE 1

/* Has isnan */
/* #undef HAVE_ISNAN */

#ifdef HAVE_IEEEFP_H
extern "C"
{
//#include <ieeefp.h>
}
#endif

#ifdef HAVE_INT64_T_64
extern "C"
{
#include <inttypes.h>
}
#endif

#if defined(__GNUC__) && defined(_WIN32)
/* For MingW the appropriate definitions are included in
 math.h and float.h but the definitions in 
 math.h are only included if __STRICT_ANSI__
 is not defined.  Since GEOS is compiled with -ansi that
 means those definitions are not available. */
#include <float.h>
#endif

#include <limits> // for std::numeric_limits



//Defines NaN for intel platforms
#define DoubleNotANumber std::numeric_limits<double>::quiet_NaN()

//Don't forget to define infinities
#define DoubleInfinity std::numeric_limits<double>::infinity()
#define DoubleNegInfinity -std::numeric_limits<double>::infinity()

#define DoubleMax std::numeric_limits<double>::max()

#ifdef HAVE_INT64_T_64
  typedef int64_t int64;
#else
# ifdef HAVE_LONG_LONG_INT_64
   typedef long long int int64;
# else
   typedef long long int64;
#  ifndef HAVE_LONG_INT_64
#   define INT64_IS_REALLY32 1
#   warning "Could not find 64bit integer definition!"
#  endif
# endif
#endif


#if defined(HAVE_FINITE) && !defined(HAVE_ISFINITE)
# define FINITE(x) (finite(x))
#else
# if defined(_MSC_VER)
#  define FINITE(x) _finite(static_cast<double>(x))    
# else
#  define FINITE(x) (isfinite(x))
# endif
#endif

#define ISNAN(x) (std::isnan(x))

#ifndef FINITE
#error "Can not compile without finite or isfinite function or macro"
#endif

#ifndef ISNAN
#error "Can not compile without isnan function or macro"
#endif

#endif
