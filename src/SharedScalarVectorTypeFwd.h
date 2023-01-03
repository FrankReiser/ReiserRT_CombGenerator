/**
 * @file SharedScalarVectorTypeFwd.h
 * @brief The Forward Declaration for the Shared Scalar Vector Data Type
 * @authors Frank Reiser
 * @date Initiated December 27th, 2022
 */

#ifndef REISERRT_COMBGENERATOR_SHAREDSCALARVECTORTYPEFWD_H
#define REISERRT_COMBGENERATOR_SHAREDSCALARVECTORTYPEFWD_H

namespace std
{
    template < typename T > class shared_ptr;
}

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief Forward Declaration for our Scalar Vector Type
         *
         * This shared pointer type will be used to pass magnitude and phase information
         * for tones to be generated by the CombGenerator via it's `CombGenerator::reset`
         * operation. Using a shared pointer to constant data allows the information to be 'read' by
         * multiple components simultaneously.
         *
         * @note Container `std::vector` was considered for this purpose. However, the
         * usage of this data, in a larger real-time use case, called more for
         * the semantics of shared pointer over a vector. It is more straight forward
         * convertible from a memory pool or the standard heap.
         */
        using SharedScalarVectorType = std::shared_ptr< const double[] >;
    }
}

#endif //REISERRT_COMBGENERATOR_SHAREDSCALARVECTORTYPEFWD_H
