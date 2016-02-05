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

#include "oadrUpdatedReportType.hxx"

#include "EiResponseType.hxx"

#include "oadrCancelReportType.hxx"

#include "schemaVersionType.hxx"

namespace oadr2b
{
  namespace oadr
  {
    // oadrUpdatedReportType
    // 

    const oadrUpdatedReportType::eiResponse_type& oadrUpdatedReportType::
    eiResponse () const
    {
      return this->eiResponse_.get ();
    }

    oadrUpdatedReportType::eiResponse_type& oadrUpdatedReportType::
    eiResponse ()
    {
      return this->eiResponse_.get ();
    }

    void oadrUpdatedReportType::
    eiResponse (const eiResponse_type& x)
    {
      this->eiResponse_.set (x);
    }

    void oadrUpdatedReportType::
    eiResponse (::std::auto_ptr< eiResponse_type > x)
    {
      this->eiResponse_.set (x);
    }

    const oadrUpdatedReportType::oadrCancelReport_optional& oadrUpdatedReportType::
    oadrCancelReport () const
    {
      return this->oadrCancelReport_;
    }

    oadrUpdatedReportType::oadrCancelReport_optional& oadrUpdatedReportType::
    oadrCancelReport ()
    {
      return this->oadrCancelReport_;
    }

    void oadrUpdatedReportType::
    oadrCancelReport (const oadrCancelReport_type& x)
    {
      this->oadrCancelReport_.set (x);
    }

    void oadrUpdatedReportType::
    oadrCancelReport (const oadrCancelReport_optional& x)
    {
      this->oadrCancelReport_ = x;
    }

    void oadrUpdatedReportType::
    oadrCancelReport (::std::auto_ptr< oadrCancelReport_type > x)
    {
      this->oadrCancelReport_.set (x);
    }

    const oadrUpdatedReportType::venID_optional& oadrUpdatedReportType::
    venID () const
    {
      return this->venID_;
    }

    oadrUpdatedReportType::venID_optional& oadrUpdatedReportType::
    venID ()
    {
      return this->venID_;
    }

    void oadrUpdatedReportType::
    venID (const venID_type& x)
    {
      this->venID_.set (x);
    }

    void oadrUpdatedReportType::
    venID (const venID_optional& x)
    {
      this->venID_ = x;
    }

    void oadrUpdatedReportType::
    venID (::std::auto_ptr< venID_type > x)
    {
      this->venID_.set (x);
    }

    const oadrUpdatedReportType::schemaVersion_optional& oadrUpdatedReportType::
    schemaVersion () const
    {
      return this->schemaVersion_;
    }

    oadrUpdatedReportType::schemaVersion_optional& oadrUpdatedReportType::
    schemaVersion ()
    {
      return this->schemaVersion_;
    }

    void oadrUpdatedReportType::
    schemaVersion (const schemaVersion_type& x)
    {
      this->schemaVersion_.set (x);
    }

    void oadrUpdatedReportType::
    schemaVersion (const schemaVersion_optional& x)
    {
      this->schemaVersion_ = x;
    }

    void oadrUpdatedReportType::
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
    // oadrUpdatedReportType
    //

    oadrUpdatedReportType::
    oadrUpdatedReportType (const eiResponse_type& eiResponse)
    : ::xml_schema::type (),
      eiResponse_ (eiResponse, this),
      oadrCancelReport_ (this),
      venID_ (this),
      schemaVersion_ (this)
    {
    }

    oadrUpdatedReportType::
    oadrUpdatedReportType (::std::auto_ptr< eiResponse_type > eiResponse)
    : ::xml_schema::type (),
      eiResponse_ (eiResponse, this),
      oadrCancelReport_ (this),
      venID_ (this),
      schemaVersion_ (this)
    {
    }

    oadrUpdatedReportType::
    oadrUpdatedReportType (const oadrUpdatedReportType& x,
                           ::xml_schema::flags f,
                           ::xml_schema::container* c)
    : ::xml_schema::type (x, f, c),
      eiResponse_ (x.eiResponse_, f, this),
      oadrCancelReport_ (x.oadrCancelReport_, f, this),
      venID_ (x.venID_, f, this),
      schemaVersion_ (x.schemaVersion_, f, this)
    {
    }

    oadrUpdatedReportType::
    oadrUpdatedReportType (const ::xercesc::DOMElement& e,
                           ::xml_schema::flags f,
                           ::xml_schema::container* c)
    : ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
      eiResponse_ (this),
      oadrCancelReport_ (this),
      venID_ (this),
      schemaVersion_ (this)
    {
      if ((f & ::xml_schema::flags::base) == 0)
      {
        ::xsd::cxx::xml::dom::parser< char > p (e, true, true);
        this->parse (p, f);
      }
    }

    void oadrUpdatedReportType::
    parse (::xsd::cxx::xml::dom::parser< char >& p,
           ::xml_schema::flags f)
    {
      for (; p.more_elements (); p.next_element ())
      {
        const ::xercesc::DOMElement& i (p.cur_element ());
        const ::xsd::cxx::xml::qualified_name< char > n (
          ::xsd::cxx::xml::dom::name< char > (i));

        // eiResponse
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "eiResponse",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< eiResponse_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!eiResponse_.present ())
            {
              ::std::auto_ptr< eiResponse_type > r (
                dynamic_cast< eiResponse_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->eiResponse_.set (r);
              continue;
            }
          }
        }

        // oadrCancelReport
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "oadrCancelReport",
              "http://openadr.org/oadr-2.0b/2012/07",
              &::xsd::cxx::tree::factory_impl< oadrCancelReport_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->oadrCancelReport_)
            {
              ::std::auto_ptr< oadrCancelReport_type > r (
                dynamic_cast< oadrCancelReport_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->oadrCancelReport_.set (r);
              continue;
            }
          }
        }

        // venID
        //
        {
          ::std::auto_ptr< ::xsd::cxx::tree::type > tmp (
            ::xsd::cxx::tree::type_factory_map_instance< 0, char > ().create (
              "venID",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              &::xsd::cxx::tree::factory_impl< venID_type >,
              true, true, i, n, f, this));

          if (tmp.get () != 0)
          {
            if (!this->venID_)
            {
              ::std::auto_ptr< venID_type > r (
                dynamic_cast< venID_type* > (tmp.get ()));

              if (r.get ())
                tmp.release ();
              else
                throw ::xsd::cxx::tree::not_derived< char > ();

              this->venID_.set (r);
              continue;
            }
          }
        }

        break;
      }

      if (!eiResponse_.present ())
      {
        throw ::xsd::cxx::tree::expected_element< char > (
          "eiResponse",
          "http://docs.oasis-open.org/ns/energyinterop/201110");
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

    oadrUpdatedReportType* oadrUpdatedReportType::
    _clone (::xml_schema::flags f,
            ::xml_schema::container* c) const
    {
      return new class oadrUpdatedReportType (*this, f, c);
    }

    oadrUpdatedReportType& oadrUpdatedReportType::
    operator= (const oadrUpdatedReportType& x)
    {
      if (this != &x)
      {
        static_cast< ::xml_schema::type& > (*this) = x;
        this->eiResponse_ = x.eiResponse_;
        this->oadrCancelReport_ = x.oadrCancelReport_;
        this->venID_ = x.venID_;
        this->schemaVersion_ = x.schemaVersion_;
      }

      return *this;
    }

    oadrUpdatedReportType::
    ~oadrUpdatedReportType ()
    {
    }

    static
    const ::xsd::cxx::tree::type_factory_initializer< 0, char, oadrUpdatedReportType >
    _xsd_oadrUpdatedReportType_type_factory_init (
      "oadrUpdatedReportType",
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
    operator<< (::xercesc::DOMElement& e, const oadrUpdatedReportType& i)
    {
      e << static_cast< const ::xml_schema::type& > (i);

      // eiResponse
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        const oadrUpdatedReportType::eiResponse_type& x (i.eiResponse ());
        if (typeid (oadrUpdatedReportType::eiResponse_type) == typeid (x))
        {
          ::xercesc::DOMElement& s (
            ::xsd::cxx::xml::dom::create_element (
              "eiResponse",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              e));

          s << x;
        }
        else
          tsm.serialize (
            "eiResponse",
            "http://docs.oasis-open.org/ns/energyinterop/201110",
            true, true, e, x);
      }

      // oadrCancelReport
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.oadrCancelReport ())
        {
          const oadrUpdatedReportType::oadrCancelReport_type& x (*i.oadrCancelReport ());
          if (typeid (oadrUpdatedReportType::oadrCancelReport_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "oadrCancelReport",
                "http://openadr.org/oadr-2.0b/2012/07",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "oadrCancelReport",
              "http://openadr.org/oadr-2.0b/2012/07",
              true, true, e, x);
        }
      }

      // venID
      //
      {
        ::xsd::cxx::tree::type_serializer_map< char >& tsm (
          ::xsd::cxx::tree::type_serializer_map_instance< 0, char > ());

        if (i.venID ())
        {
          const oadrUpdatedReportType::venID_type& x (*i.venID ());
          if (typeid (oadrUpdatedReportType::venID_type) == typeid (x))
          {
            ::xercesc::DOMElement& s (
              ::xsd::cxx::xml::dom::create_element (
                "venID",
                "http://docs.oasis-open.org/ns/energyinterop/201110",
                e));

            s << x;
          }
          else
            tsm.serialize (
              "venID",
              "http://docs.oasis-open.org/ns/energyinterop/201110",
              true, true, e, x);
        }
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
    const ::xsd::cxx::tree::type_serializer_initializer< 0, char, oadrUpdatedReportType >
    _xsd_oadrUpdatedReportType_type_serializer_init (
      "oadrUpdatedReportType",
      "http://openadr.org/oadr-2.0b/2012/07");
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsd/cxx/post.hxx>

