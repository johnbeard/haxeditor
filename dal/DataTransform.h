/*
 * DataTransform.h
 *
 *  Created on: 5 Jan 2016
 *      Author: John Beard
 */

#ifndef DAL_DATATRANSFORM_H_
#define DAL_DATATRANSFORM_H_

/*!
 * A transform to apply to a file for viewing (eg 2-s complement, XOR, ROT13,
 * etc)
 *
 * Not all transforms can be reversed (eg a threshold), so inheritors report if
 * the transform can be reversed, and if so, you can edit the file through
 * the transform
 */
class DataTransform
{
public:
    virtual ~DataTransform()
    {}

    virtual bool isReversible() const = 0;
};

#endif /* DAL_DATATRANSFORM_H_ */
