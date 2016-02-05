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

#include "FrequencyType.hxx"

#include "SiScaleCodeType.hxx"

namespace oadr2b
{
  namespace oadr
  {
    // FrequencyType
    // 

    const FrequencyType::itemDescription_type& FrequencyType::
    itemDescription () const
    {
      return this->itemDescription_.get ();
    }

    FrequencyType::itemDescription_type& FrequencyType::
    itemDescription ()
    {
      return this->itemDescription_.get ();
    }

    void FrequencyType::
    itemDescription (const itemDescription_type& x)
    {
      this->itemDescription_.set (x);
    }

    void FrequencyType::
    itemDescription (::std::auto_ptr< itemDescription_type > x)
    {
      this->itemDescription_.set (x);
    }

    const FrequencyType::itemDescription_type& FrequencyType::
    itemDescription_default_value ()
    {
      return itemDescription_default_value_;
    }

    const FrequencyType::itemUnits_type& FrequencyType::
    itemUnits () const
    {
      return this->itemUnits_.get ();
    }

    FrequencyType::itemUnits_type& FrequencyType::
    itemUnits ()
    {
      return this->itemUnits_.get ();
    }

    void FrequencyType::
    itemUnits (const itemUnits_type& x)
    {
      this->itemUnits_.set (x);
    }

    void FrequencyType::
    itemUnits (::std::auto_ptr< itemUnits_type > x)
    {
      this->itemUnits_.set (x);
    }

    const FrequencyType::itemUnits_type& FrequencyType::
    itemUnits_default_value ()
    {
      return itemUnits_default_value_;
    }

    const FrequencyType::siScaleCode_type& FrequencyType::
    siScaleCode () const
    {
      return this->siScaleCode_.get ();
    }

    FrequencyType::siScaleCode_type& FrequencyType::
    siScaleCode ()
    {
      return this->siScaleCode_.get ();
    }

    void FrequencyType::
    siScaleCode (const siScaleCode_type& x)
    {
      this->siScaleCode_.set (x);
    }

    void FrequencyType::
    siScaleCode (::std::auto_ptr< siScaleCode_type > x)
    {
      this->siScaleCode_.set (x);
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
    // FrequencyType
    //

    const FrequencyType::itemDescription_type FrequencyType::itemDescription_default_value_ (
      "Frequency");

    const FrequencyType::itemUnits_type FrequencyType::itemUnits_default_value_ (
      "Hz");

    FrequencyType::
    FrequencyType (const itemDescription_type& itemDescription,
                   const itemUnits_type& itemUnits,
                   const siScaleCode_type& siScaleCode)
    : ::oadr2b::emix::ItemBaseType (),
      itemDescription_ (itemDescription, this),
      itemUnits_ (itemUnits, this),
      siScaleCode_ (siScaleCode, this)
    {
    }

    FrequencyType::
    FrequencyType (::std::auto_ptr< itemDescription_type > itemDescription,
                   ::std::auto_ptr< itemUnits_type > itemUnits,
                   ::std::auto_ptr< siScaleCode_type > siScaleCode)
    : ::oadr2b::emix::ItemBaseType (),
      itemDescription_ (itemDescription, this),
      itemUnits_ (itemUnits, this),
      siScaleCode_ (siScaleCode, this)
    {
    }

    FrequencyType::
    FrequencyType (const FrequencyType& x,
                   ::xml_schema::flags f,
                   ::xml_schema::container* c)
    : ::oadr2b::emix::ItemBaseType (x, f, c),
      itemDescription_ (x.itemDescription_, f, this),
      itemUnits_ (x.itemUnits_, f, this),
      siScaleCode_ (x.siScaleCode_, f, this)
    {
    }

    FrequencyType::
    FrequencyType (const ::xercesc::DOMElement& e,
                   ::xml_schema::flags f,
                   ::xml_schema::container* c)
    : ::oadr2b::emix::ItemBaseType (e, f | ::xml_schema::flags::base, c),
      itemDescription_ (this),
      itemUnits_ (this),
      siScaleCode_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, false);
        this->parse (p, f);
      }
    }

    void FrequencyType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // itemDescription
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "itemDescription",
              "http://openadr.org/oadr-2.0b/2012/07",
              &::xsd::cxx::tree::factory_impl< itemDescription_type >,
              false, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!itemDescription_.present ())
            {
              ::std::auto_ptr< itemDescription_type > r (
                dynamic_cast< itemDescription_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->itemDescription_.set (r);
              continue;
            }
          }
        }

        // itemUnits
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "itemUnits",
              "http://openadr.org/oadr-2.0b/2012/07",
              &::xsd::cxx::tree::factory_impl< itemUnits_type >,
              false, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!itemUnits_.present ())
            {
              ::std::auto_ptr< itemUnits_type > r (
                dynamic_cast< itemUnits_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->itemUnits_.set (r);
              continue;
            }
          }
        }

        // siScaleCode
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "siScaleCode",
              "http://docs.oasis-open.org/ns/emix/2011/06/siscale",
              &::xsd::cxx::tree::factory_impl< siScaleCode_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!siScaleCode_.present ())
            {
              ::std::auto_ptr< siScaleCode_type > r (
                dynamic_cast< siScaleCode_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->siScaleCode_.set (r);
              continue;
            }
          }
        }

        break;
      }

      if (!itemDescription_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "itemDescription",
          "http://openadr.org/oadr-2.0b/2012/07");
      }

      if (!itemUnits_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "itemUnits",
          "http://openadr.org/oadr-2.0b/2012/07");
      }

      if (!siScaleCode_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "siScaleCode",
          "http://docs.oasis-open.org/ns/emix/2011/06/siscale");
      }
    }

    FrequencyType* FrequencyType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class FrequencyType (*this, f, c);
    }

    FrequencyType& FrequencyType::
    operator= (const FrequencyType& x)
    {
      if (this != &x)
      {
        static_cast< ::oadr2b::emix::ItemBaseType& > (*this) = x;
        this->itemDescription_ = x.itemDescription_;
        this->itemUnits_ = x.itemUnits_;
        this->siScaleCode_ = x.siScaleCode_;
      }

      return *this;
    }

    FrequencyType::
    ~FrequencyType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, FrequencyType >
    _xsd_FrequencyType_type_factory_init (
      "FrequencyType",
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
    operator<< (::xercesc::DOMElement& e, const FrequencyType& i)
    {
      e << static_cast< const ::oadr2b::emix::ItemBaseType& > (i);

      // itemDescription
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const FrequencyType::itemDescription_type& x (i.itemDescription ());
        if (typeid (FrequencyType::itemDescription_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "itemDescription",
              "http://openadr.org/oadr-2.0b/2012/07",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "itemDescription",
            "http://openadr.org/oadr-2.0b/2012/07",
            false, true, e, x);
      }

      // itemUnits
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const FrequencyType::itemUnits_type& x (i.itemUnits ());
        if (typeid (FrequencyType::itemUnits_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "itemUnits",
              "http://openadr.org/oadr-2.0b/2012/07",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "itemUnits",
            "http://openadr.org/oadr-2.0b/2012/07",
            false, true, e, x);
      }

      // siScaleCode
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const FrequencyType::siScaleCode_type& x (i.siScaleCode ());
        if (typeid (FrequencyType::siScaleCode_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "siScaleCode",
              "http://docs.oasis-open.org/ns/emix/2011/06/siscale",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "siScaleCode",
            "http://docs.oasis-open.org/ns/emix/2011/06/siscale",
            true, true, e, x);
      }
    }

    static
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, FrequencyType >
    _xsd_FrequencyType_type_serializer_init (
      "FrequencyType",
      "http://openadr.org/oadr-2.0b/2012/07");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

