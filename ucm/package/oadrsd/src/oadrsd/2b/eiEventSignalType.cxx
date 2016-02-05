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

#include "eiEventSignalType.hxx"

#include "intervals.hxx"

#include "EiTargetType.hxx"

#include "signalNameType.hxx"

#include "SignalTypeEnumeratedType.hxx"

#include "ItemBaseType.hxx"

#include "currentValueType.hxx"

namespace oadr2b
{
  namespace ei
  {
    // eiEventSignalType
    // 

    const eiEventSignalType::intervals_type& eiEventSignalType::
    intervals () const
    {
      return this->intervals_.get ();
    }

    eiEventSignalType::intervals_type& eiEventSignalType::
    intervals ()
    {
      return this->intervals_.get ();
    }

    void eiEventSignalType::
    intervals (const intervals_type& x)
    {
      this->intervals_.set (x);
    }

    void eiEventSignalType::
    intervals (::std::auto_ptr< intervals_type > x)
    {
      this->intervals_.set (x);
    }

    const eiEventSignalType::eiTarget_optional& eiEventSignalType::
    eiTarget () const
    {
      return this->eiTarget_;
    }

    eiEventSignalType::eiTarget_optional& eiEventSignalType::
    eiTarget ()
    {
      return this->eiTarget_;
    }

    void eiEventSignalType::
    eiTarget (const eiTarget_type& x)
    {
      this->eiTarget_.set (x);
    }

    void eiEventSignalType::
    eiTarget (const eiTarget_optional& x)
    {
      this->eiTarget_ = x;
    }

    void eiEventSignalType::
    eiTarget (::std::auto_ptr< eiTarget_type > x)
    {
      this->eiTarget_.set (x);
    }

    const eiEventSignalType::signalName_type& eiEventSignalType::
    signalName () const
    {
      return this->signalName_.get ();
    }

    eiEventSignalType::signalName_type& eiEventSignalType::
    signalName ()
    {
      return this->signalName_.get ();
    }

    void eiEventSignalType::
    signalName (const signalName_type& x)
    {
      this->signalName_.set (x);
    }

    void eiEventSignalType::
    signalName (::std::auto_ptr< signalName_type > x)
    {
      this->signalName_.set (x);
    }

    const eiEventSignalType::signalType_type& eiEventSignalType::
    signalType () const
    {
      return this->signalType_.get ();
    }

    eiEventSignalType::signalType_type& eiEventSignalType::
    signalType ()
    {
      return this->signalType_.get ();
    }

    void eiEventSignalType::
    signalType (const signalType_type& x)
    {
      this->signalType_.set (x);
    }

    void eiEventSignalType::
    signalType (::std::auto_ptr< signalType_type > x)
    {
      this->signalType_.set (x);
    }

    const eiEventSignalType::signalID_type& eiEventSignalType::
    signalID () const
    {
      return this->signalID_.get ();
    }

    eiEventSignalType::signalID_type& eiEventSignalType::
    signalID ()
    {
      return this->signalID_.get ();
    }

    void eiEventSignalType::
    signalID (const signalID_type& x)
    {
      this->signalID_.set (x);
    }

    void eiEventSignalType::
    signalID (::std::auto_ptr< signalID_type > x)
    {
      this->signalID_.set (x);
    }

    const eiEventSignalType::itemBase_optional& eiEventSignalType::
    itemBase () const
    {
      return this->itemBase_;
    }

    eiEventSignalType::itemBase_optional& eiEventSignalType::
    itemBase ()
    {
      return this->itemBase_;
    }

    void eiEventSignalType::
    itemBase (const itemBase_type& x)
    {
      this->itemBase_.set (x);
    }

    void eiEventSignalType::
    itemBase (const itemBase_optional& x)
    {
      this->itemBase_ = x;
    }

    void eiEventSignalType::
    itemBase (::std::auto_ptr< itemBase_type > x)
    {
      this->itemBase_.set (x);
    }

    const eiEventSignalType::currentValue_optional& eiEventSignalType::
    currentValue () const
    {
      return this->currentValue_;
    }

    eiEventSignalType::currentValue_optional& eiEventSignalType::
    currentValue ()
    {
      return this->currentValue_;
    }

    void eiEventSignalType::
    currentValue (const currentValue_type& x)
    {
      this->currentValue_.set (x);
    }

    void eiEventSignalType::
    currentValue (const currentValue_optional& x)
    {
      this->currentValue_ = x;
    }

    void eiEventSignalType::
    currentValue (::std::auto_ptr< currentValue_type > x)
    {
      this->currentValue_.set (x);
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
    // eiEventSignalType
    //

    eiEventSignalType::
    eiEventSignalType (const intervals_type& intervals,
                       const signalName_type& signalName,
                       const signalType_type& signalType,
                       const signalID_type& signalID)
    : ::xml_schema::type (),
      intervals_ (intervals, this),
      eiTarget_ (this),
      signalName_ (signalName, this),
      signalType_ (signalType, this),
      signalID_ (signalID, this),
      itemBase_ (this),
      currentValue_ (this)
    {
    }

    eiEventSignalType::
    eiEventSignalType (::std::auto_ptr< intervals_type > intervals,
                       const signalName_type& signalName,
                       const signalType_type& signalType,
                       const signalID_type& signalID)
    : ::xml_schema::type (),
      intervals_ (intervals, this),
      eiTarget_ (this),
      signalName_ (signalName, this),
      signalType_ (signalType, this),
      signalID_ (signalID, this),
      itemBase_ (this),
      currentValue_ (this)
    {
    }

    eiEventSignalType::
    eiEventSignalType (::std::auto_ptr< intervals_type > intervals,
                       ::std::auto_ptr< signalName_type > signalName,
                       ::std::auto_ptr< signalType_type > signalType,
                       ::std::auto_ptr< signalID_type > signalID)
    : ::xml_schema::type (),
      intervals_ (intervals, this),
      eiTarget_ (this),
      signalName_ (signalName, this),
      signalType_ (signalType, this),
      signalID_ (signalID, this),
      itemBase_ (this),
      currentValue_ (this)
    {
    }

    eiEventSignalType::
    eiEventSignalType (const eiEventSignalType& x,
                       ::xml_schema::flags f,
                       ::xml_schema::container* c)
    : ::xml_schema::type (x, f, c),
      intervals_ (x.intervals_, f, this),
      eiTarget_ (x.eiTarget_, f, this),
      signalName_ (x.signalName_, f, this),
      signalType_ (x.signalType_, f, this),
      signalID_ (x.signalID_, f, this),
      itemBase_ (x.itemBase_, f, this),
      currentValue_ (x.currentValue_, f, this)
    {
    }

    eiEventSignalType::
    eiEventSignalType (const ::xercesc::DOMElement& e,
                       ::xml_schema::flags f,
                       ::xml_schema::container* c)
    : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
      intervals_ (this),
      eiTarget_ (this),
      signalName_ (this),
      signalType_ (this),
      signalID_ (this),
      itemBase_ (this),
      currentValue_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
        this->parse (p, f);
      }
    }

    void eiEventSignalType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // intervals
        //
        if (n.name () == "intervals" && n.namespace_ () == "urn:ietf:params:xml:ns:icalendar-2.0:stream")
        {
          ::std::auto_ptr< intervals_type > r (
            intervals_traits::create (i, f, this));

          if (!intervals_.present ())
          {
            this->intervals_.set (r);
            continue;
          }
        }

        // eiTarget
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "eiTarget",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< eiTarget_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->eiTarget_)
            {
              ::std::auto_ptr< eiTarget_type > r (
                dynamic_cast< eiTarget_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->eiTarget_.set (r);
              continue;
            }
          }
        }

        // signalName
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "signalName",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< signalName_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!signalName_.present ())
            {
              ::std::auto_ptr< signalName_type > r (
                dynamic_cast< signalName_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->signalName_.set (r);
              continue;
            }
          }
        }

        // signalType
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "signalType",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< signalType_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!signalType_.present ())
            {
              ::std::auto_ptr< signalType_type > r (
                dynamic_cast< signalType_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->signalType_.set (r);
              continue;
            }
          }
        }

        // signalID
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "signalID",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< signalID_type >,
              false, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!signalID_.present ())
            {
              ::std::auto_ptr< signalID_type > r (
                dynamic_cast< signalID_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->signalID_.set (r);
              continue;
            }
          }
        }

        // itemBase
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "itemBase",
              "http://docs.oasis-open.org/ns/emix/2011/06",
              &::xsd::cxx::tree::factory_impl< itemBase_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->itemBase_)
            {
              ::std::auto_ptr< itemBase_type > r (
                dynamic_cast< itemBase_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->itemBase_.set (r);
              continue;
            }
          }
        }

        // currentValue
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "currentValue",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< currentValue_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->currentValue_)
            {
              ::std::auto_ptr< currentValue_type > r (
                dynamic_cast< currentValue_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->currentValue_.set (r);
              continue;
            }
          }
        }

        break;
      }

      if (!intervals_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "intervals",
          "urn:ietf:params:xml:ns:icalendar-2.0:stream");
      }

      if (!signalName_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "signalName",
          "http://docs.oasis-open.org/ns/energyinterop/201110");
      }

      if (!signalType_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "signalType",
          "http://docs.oasis-open.org/ns/energyinterop/201110");
      }

      if (!signalID_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "signalID",
          "http://docs.oasis-open.org/ns/energyinterop/201110");
      }
    }

    eiEventSignalType* eiEventSignalType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class eiEventSignalType (*this, f, c);
    }

    eiEventSignalType& eiEventSignalType::
    operator= (const eiEventSignalType& x)
    {
      if (this != &x)
      {
        static_cast< ::xml_schema::type& > (*this) = x;
        this->intervals_ = x.intervals_;
        this->eiTarget_ = x.eiTarget_;
        this->signalName_ = x.signalName_;
        this->signalType_ = x.signalType_;
        this->signalID_ = x.signalID_;
        this->itemBase_ = x.itemBase_;
        this->currentValue_ = x.currentValue_;
      }

      return *this;
    }

    eiEventSignalType::
    ~eiEventSignalType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, eiEventSignalType >
    _xsd_eiEventSignalType_type_factory_init (
      "eiEventSignalType",
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
    operator<< (::xercesc::DOMElement& e, const eiEventSignalType& i)
    {
      e << static_cast< const ::xml_schema::type& > (i);

      // intervals
      //
      {
        ::xercesc::DOMElement& s (
          ::xsd::cxx::xml::dom::create_element (
            "intervals",
            "urn:ietf:params:xml:ns:icalendar-2.0:stream",
            e));

        s << i.intervals ();
      }

      // eiTarget
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.eiTarget ())
        {
          const eiEventSignalType::eiTarget_type& x (*i.eiTarget ());
          if (typeid (eiEventSignalType::eiTarget_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "eiTarget",
                "http://docs.oasis-open.org/ns/energyinterop/201110",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "eiTarget",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              true, true, e, x);
        }
      }

      // signalName
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const eiEventSignalType::signalName_type& x (i.signalName ());
        if (typeid (eiEventSignalType::signalName_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "signalName",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "signalName",
            "http://docs.oasis-open.org/ns/energyinterop/201110",
            true, true, e, x);
      }

      // signalType
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const eiEventSignalType::signalType_type& x (i.signalType ());
        if (typeid (eiEventSignalType::signalType_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "signalType",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "signalType",
            "http://docs.oasis-open.org/ns/energyinterop/201110",
            true, true, e, x);
      }

      // signalID
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const eiEventSignalType::signalID_type& x (i.signalID ());
        if (typeid (eiEventSignalType::signalID_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "signalID",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "signalID",
            "http://docs.oasis-open.org/ns/energyinterop/201110",
            false, true, e, x);
      }

      // itemBase
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.itemBase ())
        {
          const eiEventSignalType::itemBase_type& x (*i.itemBase ());
          if (typeid (eiEventSignalType::itemBase_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "itemBase",
                "http://docs.oasis-open.org/ns/emix/2011/06",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "itemBase",
              "http://docs.oasis-open.org/ns/emix/2011/06",
              true, true, e, x);
        }
      }

      // currentValue
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.currentValue ())
        {
          const eiEventSignalType::currentValue_type& x (*i.currentValue ());
          if (typeid (eiEventSignalType::currentValue_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "currentValue",
                "http://docs.oasis-open.org/ns/energyinterop/201110",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "currentValue",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              true, true, e, x);
        }
      }
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, eiEventSignalType >
    _xsd_eiEventSignalType_type_serializer_init (
      "eiEventSignalType",
      "http://docs.oasis-open.org/ns/energyinterop/201110");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>
