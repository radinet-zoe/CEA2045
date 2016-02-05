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

#include "IntervalType.hxx"

#include "dtstart.hxx"

#include "DurationPropType.hxx"

#include "uid.hxx"

#include "StreamPayloadBaseType.hxx"

namespace oadr2b
{
  namespace ei
  {
    // IntervalType
    // 
/*
  IntervalType::signalPayloadType_optional& IntervalType::
  signalPayload ()
     {
       return this->signalPayload_;
     }

  const IntervalType::signalPayloadType_optional& IntervalType::
  signalPayload () const
      {
        return this->signalPayload_;
      }

*/

    const IntervalType::dtstart_optional& IntervalType::
    dtstart () const
    {
      return this->dtstart_;
    }

    IntervalType::dtstart_optional& IntervalType::
    dtstart ()
    {
      return this->dtstart_;
    }

    void IntervalType::
    dtstart (const dtstart_type& x)
    {
      this->dtstart_.set (x);
    }

    void IntervalType::
    dtstart (const dtstart_optional& x)
    {
      this->dtstart_ = x;
    }

    void IntervalType::
    dtstart (::std::auto_ptr< dtstart_type > x)
    {
      this->dtstart_.set (x);
    }

    const IntervalType::duration_optional& IntervalType::
    duration () const
    {
      return this->duration_;
    }

    IntervalType::duration_optional& IntervalType::
    duration ()
    {
      return this->duration_;
    }

    void IntervalType::
    duration (const duration_type& x)
    {
      this->duration_.set (x);
    }

    void IntervalType::
    duration (const duration_optional& x)
    {
      this->duration_ = x;
    }

    void IntervalType::
    duration (::std::auto_ptr< duration_type > x)
    {
      this->duration_.set (x);
    }

    const IntervalType::uid_optional& IntervalType::
    uid () const
    {
      return this->uid_;
    }

    IntervalType::uid_optional& IntervalType::
    uid ()
    {
      return this->uid_;
    }

    void IntervalType::
    uid (const uid_type& x)
    {
      //this->uid_.set (x);
    }

    void IntervalType::
    uid (const uid_optional& x)
    {
      //this->uid_ = x;
    }

    void IntervalType::
    uid (::std::auto_ptr< uid_type > x)
    {
      //this->uid_.set (x);
    }

    const IntervalType::streamPayloadBase_sequence& IntervalType::
    streamPayloadBase () const
    {
      return this->streamPayloadBase_;
    }

    IntervalType::streamPayloadBase_sequence& IntervalType::
    streamPayloadBase ()
    {
      return this->streamPayloadBase_;
    }

    void IntervalType::
    streamPayloadBase (const streamPayloadBase_sequence& s)
    {
      this->streamPayloadBase_ = s;
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
    // IntervalType
    //

    IntervalType::
    IntervalType ()
    : ::xml_schema::type (),
      dtstart_ (this),
      duration_ (this),
      uid_ (this),
      streamPayloadBase_ (this)
    {
    }

    IntervalType::
    IntervalType (const IntervalType& x,
                  ::xml_schema::flags f,
                  ::xml_schema::container* c)
    : ::xml_schema::type (x, f, c),
      dtstart_ (x.dtstart_, f, this),
      duration_ (x.duration_, f, this),
      uid_ (x.uid_, f, this),
      streamPayloadBase_ (x.streamPayloadBase_, f, this)
    {
    }

    IntervalType::
    IntervalType (const ::xercesc::DOMElement& e,
                  ::xml_schema::flags f,
                  ::xml_schema::container* c)
    : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
      dtstart_ (this),
      duration_ (this),
      uid_ (this),
      streamPayloadBase_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
        this->parse (p, f);
      }
    }

    void IntervalType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // dtstart
        //
        if (n.name () == "dtstart" && n.namespace_ () == "urn:ietf:params:xml:ns:icalendar-2.0")
        {
          ::std::auto_ptr< dtstart_type > r (
            dtstart_traits::create (i, f, this));

          if (!this->dtstart_)
          {
            this->dtstart_.set (r);
            continue;
          }
        }

        // duration
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "duration",
              "urn:ietf:params:xml:ns:icalendar-2.0",
              &::xsd::cxx::tree::factory_impl< duration_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->duration_)
            {
              ::std::auto_ptr< duration_type > r (
                dynamic_cast< duration_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->duration_.set (r);
              continue;
            }
          }
        }

        // uid
        //
        if (n.name () == "uid" && n.namespace_ () == "urn:ietf:params:xml:ns:icalendar-2.0")
        {
          ::std::auto_ptr< uid_type > r (
            uid_traits::create (i, f, this));

          if (!this->uid_)
          {
            this->uid_.set (r);
            continue;
          }
        }

        // streamPayloadBase
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "streamPayloadBase",
              "urn:ietf:params:xml:ns:icalendar-2.0:stream",
              &::xsd::cxx::tree::factory_impl< streamPayloadBase_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            ::std::auto_ptr< streamPayloadBase_type > r (
              dynamic_cast< streamPayloadBase_type* > (tmp.get ()));

            if (r.get ())
              tmp.release ();
            else
              throw ::xsd::cxx::tree::not_derived< char > ();

            this->streamPayloadBase_.push_back (r);
            continue;
          }
        }

        break;
      }
    }

    IntervalType* IntervalType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class IntervalType (*this, f, c);
    }

    IntervalType& IntervalType::
    operator= (const IntervalType& x)
    {
      if (this != &x)
      {
        static_cast< ::xml_schema::type& > (*this) = x;
        this->dtstart_ = x.dtstart_;
        this->duration_ = x.duration_;
        this->uid_ = x.uid_;
        this->streamPayloadBase_ = x.streamPayloadBase_;
      }

      return *this;
    }

    IntervalType::
    ~IntervalType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, IntervalType >
    _xsd_IntervalType_type_factory_init (
      "IntervalType",
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
    operator<< (::xercesc::DOMElement& e, const IntervalType& i)
    {
      e << static_cast< const ::xml_schema::type& > (i);

      // dtstart
      //
      if (i.dtstart ())
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "dtstart",
            "urn:ietf:params:xml:ns:icalendar-2.0",
            e));

        s << *i.dtstart ();
      }

      // duration
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.duration ())
        {
          const IntervalType::duration_type& x (*i.duration ());
          if (typeid (IntervalType::duration_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "duration",
                "urn:ietf:params:xml:ns:icalendar-2.0",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "duration",
              "urn:ietf:params:xml:ns:icalendar-2.0",
              true, true, e, x);
        }
      }

      // uid
      //
      if (i.uid ())
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "uid",
            "urn:ietf:params:xml:ns:icalendar-2.0",
            e));

        s << *i.uid ();
      }

      // streamPayloadBase
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        for (IntervalType::streamPayloadBase_const_iterator
             b (i.streamPayloadBase ().begin ()), n (i.streamPayloadBase ().end ());
             b != n; ++b)
        {
          if (typeid (IntervalType::streamPayloadBase_type) == typeid (*b))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "streamPayloadBase",
                "urn:ietf:params:xml:ns:icalendar-2.0:stream",
                e));

            s << *b;
          }
          else
            tsm.serialize (
              "streamPayloadBase",
              "urn:ietf:params:xml:ns:icalendar-2.0:stream",
              true, true, e, *b);
        }
      }
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, IntervalType >
    _xsd_IntervalType_type_serializer_init (
      "IntervalType",
      "http://docs.oasis-open.org/ns/energyinterop/201110");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

