/*
 *
 *  Copyright (C) 1994-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmSignedLong
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcvrsl.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


// ********************************


DcmSignedLong::DcmSignedLong(const DcmTag &tag,
                             const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmSignedLong::DcmSignedLong(const DcmSignedLong &old)
  : DcmElement(old)
{
}


DcmSignedLong::~DcmSignedLong()
{
}


DcmSignedLong &DcmSignedLong::operator=(const DcmSignedLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


OFCondition DcmSignedLong::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmSignedLong &, rhs);
  }
  return EC_Normal;
}


// ********************************


DcmEVR DcmSignedLong::ident() const
{
    return EVR_SL;
}


OFCondition DcmSignedLong::checkValue(const OFString &vm,
                                      const OFBool /*oldFormat*/)
{
    /* check VM only */
    return DcmElement::checkVM(getVM(), vm);
}


unsigned long DcmSignedLong::getVM()
{
    return getLengthField() / sizeof(Sint32);
}


// ********************************


void DcmSignedLong::print(STD_NAMESPACE ostream&out,
                          const size_t flags,
                          const int level,
                          const char * /*pixelFileName*/,
                          size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get signed integer data */
        Sint32 *sintVals;
        errorFlag = getSint32Array(sintVals);
        if (sintVals != NULL)
        {
            const unsigned long count = getVM();
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : OFstatic_cast(unsigned long, -1) /*unlimited*/;
            unsigned long printedLength = 0;
            unsigned long newLength = 0;
            char buffer[32];
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            for (unsigned int i = 0; i < count; i++, sintVals++)
            {
                /* check whether first value is printed (omit delimiter) */
#if SIZEOF_LONG == 8
                if (i == 0)
                    sprintf(buffer, "%d", *sintVals);
                else
                    sprintf(buffer, "\\%d", *sintVals);
#else
                if (i == 0)
                    sprintf(buffer, "%ld", *sintVals);
                else
                    sprintf(buffer, "\\%ld", *sintVals);
#endif
                /* check whether current value sticks to the length limit */
                newLength = printedLength + strlen(buffer);
                if ((newLength <= maxLength) && ((i + 1 == count) || (newLength + 3 <= maxLength)))
                {
                    out << buffer;
                    printedLength = newLength;
                } else {
                    /* check whether output has been truncated */
                    if (i + 1 < count)
                    {
                        out << "...";
                        printedLength += 3;
                    }
                    break;
                }
            }
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmSignedLong::getSint32(Sint32 &sintVal,
                                     const unsigned long pos)
{
    /* get signed integer data */
    Sint32 *sintValues = NULL;
    errorFlag = getSint32Array(sintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (sintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            sintVal = sintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        sintVal = 0;
    return errorFlag;
}


OFCondition DcmSignedLong::getSint32Array(Sint32 *&sintVals)
{
    sintVals = OFstatic_cast(Sint32 *, getValue());
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool /*normalize*/)
{
    Sint32 sintVal;
    /* get the specified numeric value */
    errorFlag = getSint32(sintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%li", OFstatic_cast(long, sintVal));
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putSint32(const Sint32 sintVal,
                                     const unsigned long pos)
{
    Sint32 val = sintVal;
    errorFlag = changeValue(&val, sizeof(Sint32) * pos, sizeof(Sint32));
    return errorFlag;
}


OFCondition DcmSignedLong::putSint32Array(const Sint32 *sintVals,
                                          const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints > 0)
    {
        /* check for valid data */
        if (sintVals != NULL)
            errorFlag = putValue(sintVals, sizeof(Sint32) * OFstatic_cast(Uint32, numSints));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putString(const char *stringVal)
{
    /* determine length of the string value */
    const Uint32 stringLen = (stringVal != NULL) ? strlen(stringVal) : 0;
    /* call the real function */
    return putString(stringVal, stringLen);
}


OFCondition DcmSignedLong::putString(const char *stringVal,
                                     const Uint32 stringLen)
{
    errorFlag = EC_Normal;
    /* determine VM of the string */
    const unsigned long vm = DcmElement::determineVM(stringVal, stringLen);
    if (vm > 0)
    {
        Sint32 *field = new Sint32[vm];
        OFString value;
        size_t pos = 0;
        /* retrieve signed integer data from character string */
        for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
        {
            /* get specified value from multi-valued string */
            pos = DcmElement::getValueFromString(stringVal, pos, stringLen, value);
            if (value.empty() ||
#if SIZEOF_LONG == 8
                (sscanf(value.c_str(), "%d", &field[i]) != 1)
#else
                (sscanf(value.c_str(), "%ld", &field[i]) != 1)
#endif
                )
            {
                errorFlag = EC_CorruptedData;
            }
        }
        /* set binary data as the element value */
        if (errorFlag.good())
            errorFlag = putSint32Array(field, vm);
        /* delete temporary buffer */
        delete[] field;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (getLengthField() % (sizeof(Sint32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            setLengthField(getLengthField() - (getLengthField() % (sizeof(Sint32))));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
