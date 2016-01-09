/*
 * dal_types.h
 *
 *  Created on: 9 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_DALTYPES_H_
#define DAL_DALTYPES_H_

#include <stdint.h>

typedef uint64_t datasize_t;

/*!
 * The offset within a file, expressed in bits (not bytes) to allow for
 * bitwise access
 */
typedef uint64_t offset_t;

constexpr offset_t BYTE = 8;


#endif /* DAL_DALTYPES_H_ */
