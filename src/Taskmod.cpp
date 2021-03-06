////////////////////////////////////////////////////////////////////////////////
// taskwarrior - a command line task list manager.
//
// Copyright 2006-2012, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#define L10N                                           // Localization complete.

#include <sstream>
#include <iostream>
#include <i18n.h>
#include <assert.h>
#include <Taskmod.h>

////////////////////////////////////////////////////////////////////////////////
Taskmod::Taskmod ()
{
  _timestamp = 0;
  _bAfterSet = false;
  _bBeforeSet = false;
}

////////////////////////////////////////////////////////////////////////////////
Taskmod::Taskmod (const Taskmod& other)
{
  this->_before     = other._before;
  this->_after      = other._after;
  this->_timestamp  = other._timestamp;
  this->_bAfterSet  = other._bAfterSet;
  this->_bBeforeSet = other._bBeforeSet;
}

////////////////////////////////////////////////////////////////////////////////
Taskmod::~Taskmod ()
{
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::operator< (const Taskmod &compare)
{
  return (_timestamp < compare.getTimestamp ());
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::operator> (const Taskmod &compare)
{
  return (_timestamp > compare.getTimestamp ());
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::operator== (const Taskmod& compare)
{
  return ( (compare._after     == this->_after)
        && (compare._before    == this->_before)
        && (compare._timestamp == this->_timestamp) );
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::operator!= (const Taskmod& compare)
{
  return !this->operator== (compare);
}

////////////////////////////////////////////////////////////////////////////////
Taskmod& Taskmod::operator= (const Taskmod& other)
{
  if (this != &other)
  {
    this->_before     = other._before;
    this->_after      = other._after;
    this->_timestamp  = other._timestamp;
    this->_bAfterSet  = other._bAfterSet;
    this->_bBeforeSet = other._bBeforeSet;
  }

  return *this;
}

////////////////////////////////////////////////////////////////////////////////
void Taskmod::reset (long timestamp)
{
  this->_bAfterSet  = false;
  this->_bBeforeSet = false;
  this->_timestamp  = timestamp;
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::isNew ()
{
  return !_bBeforeSet;
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::issetAfter ()
{
  return _bAfterSet;
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::issetBefore ()
{
  return _bBeforeSet;
}

////////////////////////////////////////////////////////////////////////////////
bool Taskmod::isValid ()
{
  return (_timestamp > 0) && (_bAfterSet);
}

////////////////////////////////////////////////////////////////////////////////
std::string Taskmod::getUuid ()
{
  if (!_bAfterSet)
    throw std::string (STRING_TASKMOD_BAD_INIT);

  return _after.get ("uuid");
}

////////////////////////////////////////////////////////////////////////////////
std::string Taskmod::toString ()
{
  assert (_bAfterSet);

  std::stringstream stream;
  stream << STRING_TASKMOD_TIME << _timestamp << "\n";

  if (_bBeforeSet)
  {
    stream << STRING_TASKMOD_OLD << _before.composeF4();
  }

  stream << STRING_TASKMOD_NEW << _after.composeF4();
  stream << "---\n";

  return stream.str ();
}

////////////////////////////////////////////////////////////////////////////////
void Taskmod::setAfter (const Task& after)
{
  this->_after     = after;
  this->_bAfterSet = true;
}

////////////////////////////////////////////////////////////////////////////////
void Taskmod::setBefore (const Task& before)
{
  this->_before     = before;
  this->_bBeforeSet = true;
}

////////////////////////////////////////////////////////////////////////////////
void Taskmod::setTimestamp (long timestamp)
{
  this->_timestamp = timestamp;
}

////////////////////////////////////////////////////////////////////////////////
Task& Taskmod::getAfter ()
{
  return _after;
}

////////////////////////////////////////////////////////////////////////////////
Task& Taskmod::getBefore ()
{
  return _before;
}

////////////////////////////////////////////////////////////////////////////////
long Taskmod::getTimestamp () const
{
  return _timestamp;
}

////////////////////////////////////////////////////////////////////////////////
std::string Taskmod::getTimeStr () const
{
  std::stringstream sstream;
  sstream << _timestamp;

  return sstream.str ();
}

////////////////////////////////////////////////////////////////////////////////
