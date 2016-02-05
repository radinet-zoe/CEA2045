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

#include "oadrCreatedEventType.hxx"

#include "eiCreatedEvent.hxx"

#include "schemaVersionType.hxx"

namespace oadr2b
{
  namespace oadr
  {
    // oadrCreatedEventType
    // 

    const oadrCreatedEventType::eiCreatedEvent_type& oadrCreatedEventType::
    eiCreatedEvent () const
    {
      return this->eiCreatedEvent_.get ();
    }

    oadrCreatedEventType::eiCreatedEvent_type& oadrCreatedEventType::
    eiCreatedEvent ()
    {
      return this->eiCreatedEvent_.get ();
    }

    void oadrCreatedEventType::
    eiCreatedEvent (const eiCreatedEvent_type& x)
    {
      this->eiCreatedEvent_.set (x);
    }

    void oadrCreatedEventType::
    eiCreatedEvent (::std::auto_ptr< eiCreatedEvent_type > x)
    {
      this->eiCreatedEvent_.set (x);
    }

    const oadrCreatedEventType::schemaVersion_optional& oadrCreatedEventType::
    schemaVersion () const
    {
      return this->schemaVersion_;
    }

    oadrCreatedEventType::schemaVersion_optional& oadrCreatedEventType::
    schemaVersion ()
    {
      return this->schemaVersion_;
    }

    void oadrCreatedEventType::
    schemaVersion (const schemaVersion_type& x)
    {
      this->schemaVersion_.set (x);
    }

    void oadrCreatedEventType::
    schemaVersion (const schemaVersion_optional& x)
    {
      this->schemaVersion_ = x;
    }

    void oadrCreatedEventType::
    schemaVersion (::std::auto_ptr< schemaVersion_type > x)
    {
      this->schemaVersion_.set (x);
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
    // oadrCreatedEventType
    //

    oadrCreatedEventType::
    oadrCreatedEventType (const eiCreatedEvent_type& eiCreatedEvent)
    : ::xml_schema::type (),
      eiCreatedEvent_ (eiCreatedEvent, this),
      schemaVersion_ (this)
    {
    }

    oadrCreatedEventType::
    oadrCreatedEventType (::std::auto_ptr< eiCreatedEvent_type > eiCreatedEvent)
    : ::xml_schema::type (),
      eiCreatedEvent_ (eiCreatedEvent, this),
      schemaVersion_ (this)
    {
    }

    oadrCreatedEventType::
    oadrCreatedEventType (const oadrCreatedEventType& x,
                          ::xml_schema::flags f,
                          ::xml_schema::container* c)
    : ::xml_schema::type (x, f, c),
      eiCreatedEvent_ (x.eiCreatedEvent_, f, this),
      schemaVersion_ (x.schemaVersion_, f, this)
    {
    }

    oadrCreatedEventType::
    oadrCreatedEventType (const ::xercesc::DOMElement& e,
                          ::xml_schema::flags f,
                          ::xml_schema::container* c)
    : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
      eiCreatedEvent_ (this),
      schemaVersion_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, true);
        this->parse (p, f);
      }
    }

    void oadrCreatedEventType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // eiCreatedEvent
        //
        if (n.name () == "eiCreatedEvent" && n.namespace_ () == "http://docs.oasis-open.org/ns/energyinterop/201110/payloads")
        {
          ::std::auto_ptr< eiCreatedEvent_type > r (
            eiCreatedEvent_traits::create (i, f, this));

          if (!eiCreatedEvent_.present ())
          {
            this->eiCreatedEvent_.set (r);
            continue;
          }
        }

        break;
      }

      if (!eiCreatedEvent_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "eiCreatedEvent",
          "http://docs.oasis-open.org/ns/energyinterop/201110/payloads");
      }

      while (p.more_attributes ())
      {
        const ::xercesc::DOMAttr& i (p.next_attribute ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        if (n.name () == "schemaVersion" && n.namespace_ () == "http://docs.oasis-open.org/ns/energyinterop/201110")
        {
          this->schemaVersion_.set (schemaVersion_traits::create (i, f, this));
          continue;
        }
      }
    }

    oadrCreatedEventType* oadrCreatedEventType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class oadrCreatedEventType (*this, f, c);
    }

    oadrCreatedEventType& oadrCreatedEventType::
    operator= (const oadrCreatedEventType& x)
    {
      if (this != &x)
      {
        static_cast< ::xml_schema::type& > (*this) = x;
        this->eiCreatedEvent_ = x.eiCreatedEvent_;
        this->schemaVersion_ = x.schemaVersion_;
      }

      return *this;
    }

    oadrCreatedEventType::
    ~oadrCreatedEventType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, oadrCreatedEventType >
    _xsd_oadrCreatedEventType_type_factory_init (
      "oadrCreatedEventType",
      "http://openadr.org/oadr-2.0b/2012/07");
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
    operator<< (::xercesc::DOMElement& e, const oadrCreatedEventType& i)
    {
      e << static_cast< const ::xml_schema::type& > (i);

      // eiCreatedEvent
      //
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "eiCreatedEvent",
            "http://docs.oasis-open.org/ns/energyinterop/201110/payloads",
            e));

        s << i.eiCreatedEvent ();
      }

      // schemaVersion
      //
      if (i.schemaVersion ())
      {
        ::xercesc::DOMAttr& a (
          ::xsd::cxx::xml::dom::create_attribute (
            "schemaVersion",
            "http://docs.oasis-open.org/ns/energyinterop/201110",
            e));

        a << *i.schemaVersion ();
      }
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, oadrCreatedEventType >
    _xsd_oadrCreatedEventType_type_serializer_init (
      "oadrCreatedEventType",
      "http://openadr.org/oadr-2.0b/2012/07");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

