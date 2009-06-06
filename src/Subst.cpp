////////////////////////////////////////////////////////////////////////////////
// task - a command line task list manager.
//
// Copyright 2006 - 2009, Paul Beckingham.
// All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the
//
//     Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor,
//     Boston, MA
//     02110-1301
//     USA
//
////////////////////////////////////////////////////////////////////////////////

#include "Subst.h"
#include "Nibbler.h"

////////////////////////////////////////////////////////////////////////////////
Subst::Subst ()
: mFrom ("")
, mTo ("")
, mGlobal (false)
{
}

////////////////////////////////////////////////////////////////////////////////
Subst::Subst (const std::string& input)
{
  parse (input);
}

////////////////////////////////////////////////////////////////////////////////
Subst::Subst (const Subst& other)
{
  mFrom   = other.mFrom;
  mTo     = other.mTo;
  mGlobal = other.mGlobal;
}

////////////////////////////////////////////////////////////////////////////////
Subst& Subst::operator= (const Subst& other)
{
  if (this != &other)
  {
    mFrom   = other.mFrom;
    mTo     = other.mTo;
    mGlobal = other.mGlobal;
  }

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
Subst::~Subst ()
{
}

////////////////////////////////////////////////////////////////////////////////
bool Subst::parse (const std::string& input)
{
  Nibbler n (input);
  if (n.skip     ('/')        &&
      n.getUntil ('/', mFrom) &&
      n.skip     ('/')        &&
      n.getUntil ('/', mTo)   &&
      n.skip     ('/'))
  {
    mGlobal = n.skip ('g');
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////
void Subst::apply (
  std::string& description,
  std::vector <Att>& annotations) const
{
  if (mFrom != "")
  {
    std::string::size_type pattern;

    if (mGlobal)
    {
      // Perform all subs on description.
      while ((pattern = description.find (mFrom)) != std::string::npos)
        description.replace (pattern, mFrom.length (), mTo);

      // Perform all subs on annotations.
      std::vector <Att>::iterator i;
      for (i = annotations.begin (); i != annotations.end (); ++i)
      {
        std::string description = i->value ();
        while ((pattern = description.find (mFrom)) != std::string::npos)
        {
          description.replace (pattern, mFrom.length (), mTo);
          i->value (description);
        }
      }
    }
    else
    {
      // Perform first description substitution.
      if ((pattern = description.find (mFrom)) != std::string::npos)
        description.replace (pattern, mFrom.length (), mTo);

      // Failing that, perform the first annotation substitution.
      else
      {
        std::vector <Att>::iterator i;
        for (i = annotations.begin (); i != annotations.end (); ++i)
        {
          std::string description = i->value ();
          if ((pattern = description.find (mFrom)) != std::string::npos)
          {
            description.replace (pattern, mFrom.length (), mTo);
            break;
          }
        }
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////