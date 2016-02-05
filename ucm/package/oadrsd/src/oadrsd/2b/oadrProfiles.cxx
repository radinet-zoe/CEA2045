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

#include "oadrProfiles.hxx"

#include "oadrProfile.hxx"

namespace oadr2b
{
  namespace oadr
  {
    // oadrProfiles
    // 

    const oadrProfiles::oadrProfile_sequence& oadrProfiles::
    oadrProfile () const
    {
      return this->oadrProfile_;
    }

    oadrProfiles::oadrProfile_sequence& oadrProfiles::
    oadrProfile ()
    {
      return this->oadrProfile_;
    }

    void oadrProfiles::
    oadrProfile (const oadrProfile_sequence& s)
    {
      this->oadrProfile_ = s;
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
  namespace oadr
  {
    // oadrProfiles
    //

    oadrProfiles::
    oadrProfiles ()
    : ::xml_schema::type (),
      oadrProfile_ (this)
    {
    }

    oadrProfiles::
    oadrProfiles (const oadrProfiles& x,
                  ::xml_schema::flags f,
                  ::xml_schema::container* c)
    : ::xml_schema::type (x, f, c),
      oadrProfile_ (x.oadrProfile_, f, this)
    {
    }

    oadrProfiles::
    oadrProfiles (const ::xercesc::DOMElement& e,
                  ::xml_schema::flags f,
                  ::xml_schema::container* c)
    : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
      oadrProfile_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
        this->parse (p, f);
      }
    }

    void oadrProfiles::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // oadrProfile
        //
        if (n.name () == "oadrProfile" && n.namespace_ () == "http://openadr.org/oadr-2.0b/2012/07")
        {
          ::std::auto_ptr< oadrProfile_type > r (
            oadrProfile_traits::create (i, f, this));

          this->oadrProfile_.push_back (r);
          continue;
        }

        break;
      }
    }

    oadrProfiles* oadrProfiles::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class oadrProfiles (*this, f, c);
    }

    oadrProfiles& oadrProfiles::
    operator= (const oadrProfiles& x)
    {
      if (this != &x)
      {
        static_cast< ::xml_schema::type& > (*this) = x;
        this->oadrProfile_ = x.oadrProfile_;
      }

      return *this;
    }

    oadrProfiles::
    ~oadrProfiles ()
    {
    }
  }
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

namespace oadr2b
{
  namespace oadr
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
  namespace oadr
  {
    void
    operator<< (::xercesc::DOMElement& e, const oadrProfiles& i)
    {
      e << static_cast< const ::xml_schema::type& > (i);

      // oadrProfile
      //
      for (oadrProfiles::oadrProfile_const_iterator
           b (i.oadrProfile ().begin ()), n (i.oadrProfile ().end ());
           b != n; ++b)
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "oadrProfile",
            "http://openadr.org/oadr-2.0b/2012/07",
            e));

        s << *b;
      }
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

