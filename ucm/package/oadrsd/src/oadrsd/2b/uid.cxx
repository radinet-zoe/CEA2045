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

#include "uid.hxx"

namespace icalendar_2_0
{
  // uid
  // 

  const uid::text_type& uid::
  text () const
  {
    return this->text_.get ();
  }

  uid::text_type& uid::
  text ()
  {
    return this->text_.get ();
  }

  void uid::
  text (const text_type& x)
  {
    this->text_.set (x);
  }

  void uid::
  text (::std::auto_ptr< text_type > x)
  {
    this->text_.set (x);
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

namespace icalendar_2_0
{
  // uid
  //

  uid::
  uid (const text_type& text)
  : ::xml_schema::type (),
    text_ (text, this)
  {
  }

  uid::
  uid (::std::auto_ptr< text_type > text)
  : ::xml_schema::type (),
    text_ (text, this)
  {
  }

  uid::
  uid (const uid& x,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
  : ::xml_schema::type (x, f, c),
    text_ (x.text_, f, this)
  {
  }

  uid::
  uid (const ::xercesc::DOMElement& e,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
  : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
    text_ (this)
  {
    if ((f & ::xml_schema::flags::base) == 0)
    {
      ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
      this->parse (p, f);
    }
  }

  void uid::
  parse (::xsd::cxx::xml::dom::parser< char >& p,
         ::xml_schema::flags f)
  {
    for (; p.more_elements (); p.next_element ())
    {
      const ::xercesc::DOMElement& i (p.cur_element ());
      const ::xsd::cxx::xml::qualified_name< char > n (
        ::xsd::cxx::xml::dom::name< char > (i));

      // text
      //
      {
        ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
          ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
            "text",
            "urn:ietf:params:xml:ns:icalendar-2.0",
            &::xsd::cxx::tree::factory_impl< text_type >,
            true, true, i, n, f, this));

        if (tmp.get () != 0)
        {
          if (!text_.present ())
          {
            ::std::auto_ptr< text_type > r (
              dynamic_cast< text_type* > (tmp.get ()));

            if (r.get ())
              tmp.release ();
            else
              throw ::xsd::cxx::tree::not_derived< char > ();

            this->text_.set (r);
            continue;
          }
        }
      }

      break;
    }

    if (!text_.present ())
    {
      throw ::xsd::cxx::tree::expected_element< char > (
        "text",
        "urn:ietf:params:xml:ns:icalendar-2.0");
    }
  }

  uid* uid::
  _clone (::xml_schema::flags f,
          ::xml_schema::container* c) const
  {
    return new class uid (*this, f, c);
  }

  uid& uid::
  operator= (const uid& x)
  {
    if (this != &x)
    {
      static_cast< ::xml_schema::type& > (*this) = x;
      this->text_ = x.text_;
    }

    return *this;
  }

  uid::
  ~uid ()
  {
  }
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

namespace icalendar_2_0
{
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

namespace icalendar_2_0
{
  void
  operator<< (::xercesc::DOMElement& e, const uid& i)
  {
    e << static_cast< const ::xml_schema::type& > (i);

    // text
    //
    {
      ::xsd::cxx::tree::type_serializer_map< char >& tsm (
        ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

      const uid::text_type& x (i.text ());
      if (typeid (uid::text_type) == typeid (x))
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "text",
            "urn:ietf:params:xml:ns:icalendar-2.0",
            e));

        s << x;
      }
      else
        tsm.serialize (
          "text",
          "urn:ietf:params:xml:ns:icalendar-2.0",
          true, true, e, x);
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>
