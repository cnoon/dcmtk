/*
 *
 *  Copyright (C) 2011, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class DcmSpecificCharacterSet
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2011-11-01 14:54:07 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmdata/dcspchrs.h"
#include "dcmtk/dcmdata/dctypes.h"


OFTEST(dcmdata_specificCharacterSet_1)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionLibraryAvailable())
    {
        // try to select all DICOM character sets without code extensions
        OFCHECK(converter.selectCharacterSet("").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 6  ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 101").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 109").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 110").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 144").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 127").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 126").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 138").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 148").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 13 ").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 166").good());
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.selectCharacterSet("  GB18030 ").good());
        // try to select all DICOM character sets with code extensions
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 6  ").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 100").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 101").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 109").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 110").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 144").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 127").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 126").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 138").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 148").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 13 ").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 166").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 87" ).good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 159").good());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 149").good());
        // the following should fail
        OFCHECK(converter.selectCharacterSet("DCMTK").bad());
        OFCHECK(converter.selectCharacterSet(" \\ ").bad());
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 999").bad());
        OFCHECK(converter.selectCharacterSet("\\ISO_IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 87\\ISO 2022 IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 159\\ISO 2022 IR 100").bad());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 149\\ISO 2022 IR 100").bad());
        // the following should work
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 6\\ISO 2022 IR 100").good());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 100\\ISO 2022 IR 126").good());
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 144\\ISO 2022 IR 138").good());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}

OFTEST(dcmdata_specificCharacterSet_2)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionLibraryAvailable())
    {
        OFString resultStr;
        // check whether string conversion from Latin-1 to UTF-8 works
        OFCHECK(converter.selectCharacterSet("ISO_IR 100").good());
        OFCHECK(converter.convertToUTF8String("J\366rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertToUTF8String("J\351r\364me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
        // check whether string conversion from UTF-8 to UTF-8 works :-)
        OFCHECK(converter.selectCharacterSet("ISO_IR 192").good());
        OFCHECK(converter.convertToUTF8String("J\303\266rg", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\266rg");
        OFCHECK(converter.convertToUTF8String("J\303\251r\303\264me", resultStr).good());
        OFCHECK_EQUAL(resultStr, "J\303\251r\303\264me");
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}


OFTEST(dcmdata_specificCharacterSet_3)
{
    DcmSpecificCharacterSet converter;
    if (converter.isConversionLibraryAvailable())
    {
        OFString resultStr;
        // we need the PN delimiters for a standard comformant conversion
        const OFString delimiters("\\^=");
        // check whether string conversion from Japanese language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex H.3.1
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 87").good());
        OFCHECK(converter.convertToUTF8String("Yamada^Tarou=\033$B;3ED\033(B^\033$BB@O:\033(B=\033$B$d$^$@\033(B^\033$B$?$m$&\033(B", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Yamada^Tarou=\345\261\261\347\224\260^\345\244\252\351\203\216=\343\202\204\343\201\276\343\201\240^\343\201\237\343\202\215\343\201\206");
        // example taken from DICOM PS 3.5 Annex H.3.2
        OFCHECK(converter.selectCharacterSet("ISO 2022 IR 13\\ISO 2022 IR 87").good());
        OFCHECK(converter.convertToUTF8String("\324\317\300\336^\300\333\263=\033$B;3ED\033(J^\033$BB@O:\033(J=\033$B$d$^$@\033(J^\033$B$?$m$&\033(J", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "\357\276\224\357\276\217\357\276\200\357\276\236^\357\276\200\357\276\233\357\275\263=\345\261\261\347\224\260^\345\244\252\351\203\216=\343\202\204\343\201\276\343\201\240^\343\201\237\343\202\215\343\201\206");
        // check whether string conversion from Korean language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex I.2
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 149").good());
        OFCHECK(converter.convertToUTF8String("Hong^Gildong=\033$)C\373\363^\033$)C\321\316\324\327=\033$)C\310\253^\033$)C\261\346\265\277", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Hong^Gildong=\346\264\252^\345\220\211\346\264\236=\355\231\215^\352\270\270\353\217\231");
        // check whether string conversion from Chinese language to UTF-8 works
        // example taken from DICOM PS 3.5 Annex J.2
        OFCHECK(converter.selectCharacterSet("GB18030").good());
        OFCHECK(converter.convertToUTF8String("Wang^XiaoDong=\315\365^\320\241\266\253=", resultStr, delimiters).good());
        OFCHECK_EQUAL(resultStr, "Wang^XiaoDong=\347\216\213^\345\260\217\344\270\234=");
        // the following should fail (wrong character set)
        OFCHECK(converter.selectCharacterSet("\\ISO 2022 IR 166").good());
        OFCHECK(converter.convertToUTF8String("Yamada^Tarou=\033$B;3ED\033(B^\033$BB@O:\033(B=\033$B$d$^$@\033(B^\033$B$?$m$&\033(B", resultStr, delimiters).bad());
    } else {
        // in case there is no libiconv, report a warning but do not fail
        DCMDATA_WARN("Cannot test DcmSpecificCharacterSet since the underlying character set conversion library is not available");
    }
}


/*
 *
 * CVS/RCS Log:
 * $Log: tspchrs.cc,v $
 * Revision 1.2  2011-11-01 14:54:07  joergr
 * Added support for code extensions (escape sequences) according to ISO 2022
 * to the character set conversion code.
 *
 * Revision 1.1  2011-10-26 16:13:03  joergr
 * Added helper class for converting between different DICOM character sets.
 * This initial version only supports the conversion to UTF-8 (Unicode) and only
 * from DICOM characters sets without code extension techniques (i.e. ISO 2022).
 *
 *
 */