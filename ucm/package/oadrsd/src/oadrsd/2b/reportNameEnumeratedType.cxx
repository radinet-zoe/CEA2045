// Copyright (c) 2005-2011 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

#include <xsd/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#include "reportNameEnumeratedType.hxx"

namespace oadr2b
{
  namespace ei
  {
    // reportNameEnumeratedType
    // 

    reportNameEnumeratedType::
    reportNameEnumeratedType (value v)
    : ::xml_schema::token (_xsd_reportNameEnumeratedType_literals_[v])
    {
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const char* v)
    : ::xml_schema::token (v)
    {
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const ::std::string& v)
    : ::xml_schema::token (v)
    {
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const ::xml_schema::token& v)
    : ::xml_schema::token (v)
    {
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const reportNameEnumeratedType& v,
                              ::xml_schema::flags f,
                              ::xml_schema::container* c)
    : ::xml_schema::token (v, f, c)
    {
    }

    reportNameEnumeratedType& reportNameEnumeratedType::
    operator= (value v)
    {
      static_cast< ::xml_schema::token& > (*this) = 
      ::xml_schema::token (_xsd_reportNameEnumeratedType_literals_[v]);

      return *this;
    }
  }
}

#include <xsd/cxx/xml/dom/parsing-source.hxx>

#include <xsd/cxx/tree/type-factory-map.hxx>

namespace _xsd
{
  static
  const ::xsd::cxx::tree::type_factory_plate< 0, char >
  type_factory_plate_init;
}

namespace oadr2b
{
  namespace ei
  {
    // reportNameEnumeratedType
    //

    reportNameEnumeratedType::
    reportNameEnumeratedType (const ::xercesc::DOMElement& e,
                              ::xml_schema::flags f,
                              ::xml_schema::container* c)
    : ::xml_schema::token (e, f, c)
    {
      _xsd_reportNameEnumeratedType_convert ();
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const ::xercesc::DOMAttr& a,
                              ::xml_schema::flags f,
                              ::xml_schema::container* c)
    : ::xml_schema::token (a, f, c)
    {
      _xsd_reportNameEnumeratedType_convert ();
    }

    reportNameEnumeratedType::
    reportNameEnumeratedType (const ::std::string& s,
                              const ::xercesc::DOMElement* e,
                              ::xml_schema::flags f,
                              ::xml_schema::container* c)
    : ::xml_schema::token (s, e, f, c)
    {
      _xsd_reportNameEnumeratedType_convert ();
    }

    reportNameEnumeratedType* reportNameEnumeratedType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class reportNameEnumeratedType (*this, f, c);
    }

    reportNameEnumeratedType::value reportNameEnumeratedType::
    _xsd_reportNameEnumeratedType_convert () const
    {
      ::xsd::cxx::tree::enum_comparator< char > c (_xsd_reportNameEnumeratedType_literals_);
      const value* i (::std::lower_bound (
                        _xsd_reportNameEnumeratedType_indexes_,
                        _xsd_reportNameEnumeratedType_indexes_ + 8,
                        *this,
                        c));

      if (i == _xsd_reportNameEnumeratedType_indexes_ + 8 || _xsd_reportNameEnumeratedType_literals_[*i] != *this)
      {
        throw ::xsd::cxx::tree::unexpected_enumerator < char > (*this);
      }

      return *i;
    }

    const char* const reportNameEnumeratedType::
    _xsd_reportNameEnumeratedType_literals_[8] =
    {
      "METADATA_HISTORY_USAGE",
      "HISTORY_USAGE",
      "METADATA_HISTORY_GREENBUTTON",
      "HISTORY_GREENBUTTON",
      "METADATA_TELEMETRY_USAGE",
      "TELEMETRY_USAGE",
      "METADATA_TELEMETRY_STATUS",
      "TELEMETRY_STATUS"
    };

    const reportNameEnumeratedType::value reportNameEnumeratedType::
    _xsd_reportNameEnumeratedType_indexes_[8] =
    {
      ::oadr2b::ei::reportNameEnumeratedType::HISTORY_GREENBUTTON,
      ::oadr2b::ei::reportNameEnumeratedType::HISTORY_USAGE,
      ::oadr2b::ei::reportNameEnumeratedType::METADATA_HISTORY_GREENBUTTON,
      ::oadr2b::ei::reportNameEnumeratedType::METADATA_HISTORY_USAGE,
      ::oadr2b::ei::reportNameEnumeratedType::METADATA_TELEMETRY_STATUS,
      ::oadr2b::ei::reportNameEnumeratedType::METADATA_TELEMETRY_USAGE,
      ::oadr2b::ei::reportNameEnumeratedType::TELEMETRY_STATUS,
      ::oadr2b::ei::reportNameEnumeratedType::TELEMETRY_USAGE
    };

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, reportNameEnumeratedType >
    _xsd_reportNameEnumeratedType_type_factory_init (
      "reportNameEnumeratedType",
      "http://docs.oasis-open.org/ns/energyinterop/201110");
  }
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

namespace oadr2b
{
  namespace ei
  {
  }
}

#include <ostream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

#include <xsd/cxx/tree/type-serializer-map.hxx>

namespace _xsd
{
  static
  const ::xsd::cxx::tree::type_serializer_plate< 0, char >
  type_serializer_plate_init;
}

namespace oadr2b
{
  namespace ei
  {
    void
    operator<< (::xercesc::DOMElement& e, const reportNameEnumeratedType& i)
    {
      e << static_cast< const ::xml_schema::token& > (i);
    }

    void
    operator<< (::xercesc::DOMAttr& a, const reportNameEnumeratedType& i)
    {
      a << static_cast< const ::xml_schema::token& > (i);
    }

    void
    operator<< (::xml_schema::list_stream& l,
                const reportNameEnumeratedType& i)
    {
      l << static_cast< const ::xml_schema::token& > (i);
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, reportNameEnumeratedType >
    _xsd_reportNameEnumeratedType_type_serializer_init (
      "reportNameEnumeratedType",
      "http://docs.oasis-open.org/ns/energyinterop/201110");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

