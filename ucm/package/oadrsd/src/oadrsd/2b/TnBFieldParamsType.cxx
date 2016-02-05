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

#include "TnBFieldParamsType.hxx"

namespace oadr2b
{
  namespace dsig11
  {
    // TnBFieldParamsType
    // 

    const TnBFieldParamsType::K_type& TnBFieldParamsType::
    K () const
    {
      return this->K_.get ();
    }

    TnBFieldParamsType::K_type& TnBFieldParamsType::
    K ()
    {
      return this->K_.get ();
    }

    void TnBFieldParamsType::
    K (const K_type& x)
    {
      this->K_.set (x);
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
  namespace dsig11
  {
    // TnBFieldParamsType
    //

    TnBFieldParamsType::
    TnBFieldParamsType (const M_type& M,
                        const K_type& K)
    : ::oadr2b::dsig11::CharTwoFieldParamsType (M),
      K_ (K, this)
    {
    }

    TnBFieldParamsType::
    TnBFieldParamsType (const TnBFieldParamsType& x,
                        ::xml_schema::flags f,
                        ::xml_schema::container* c)
    : ::oadr2b::dsig11::CharTwoFieldParamsType (x, f, c),
      K_ (x.K_, f, this)
    {
    }

    TnBFieldParamsType::
    TnBFieldParamsType (const ::xercesc::DOMElement& e,
                        ::xml_schema::flags f,
                        ::xml_schema::container* c)
    : ::oadr2b::dsig11::CharTwoFieldParamsType (e, f | ::xml_schema::flags::base, c),
      K_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
        this->parse (p, f);
      }
    }

    void TnBFieldParamsType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      this->::oadr2b::dsig11::CharTwoFieldParamsType::parse (p, f);

      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // K
        //
        if (n.name () == "K" && n.namespace_ () == "http://www.w3.org/2009/xmldsig11#")
        {
          if (!K_.present ())
          {
            this->K_.set (K_traits::create (i, f, this));
            continue;
          }
        }

        break;
      }

      if (!K_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "K",
          "http://www.w3.org/2009/xmldsig11#");
      }
    }

    TnBFieldParamsType* TnBFieldParamsType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class TnBFieldParamsType (*this, f, c);
    }

    TnBFieldParamsType& TnBFieldParamsType::
    operator= (const TnBFieldParamsType& x)
    {
      if (this != &x)
      {
        static_cast< ::oadr2b::dsig11::CharTwoFieldParamsType& > (*this) = x;
        this->K_ = x.K_;
      }

      return *this;
    }

    TnBFieldParamsType::
    ~TnBFieldParamsType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, TnBFieldParamsType >
    _xsd_TnBFieldParamsType_type_factory_init (
      "TnBFieldParamsType",
      "http://www.w3.org/2009/xmldsig11#");
  }
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

namespace oadr2b
{
  namespace dsig11
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
  namespace dsig11
  {
    void
    operator<< (::xercesc::DOMElement& e, const TnBFieldParamsType& i)
    {
      e << static_cast< const ::oadr2b::dsig11::CharTwoFieldParamsType& > (i);

      // K
      //
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "K",
            "http://www.w3.org/2009/xmldsig11#",
            e));

        s << i.K ();
      }
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, TnBFieldParamsType >
    _xsd_TnBFieldParamsType_type_serializer_init (
      "TnBFieldParamsType",
      "http://www.w3.org/2009/xmldsig11#");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

