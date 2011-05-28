////////////////////////////////////////////////////////////////////////////////
// taskwarrior - a command line task list manager.
//
// Copyright 2010 - 2011, Paul Beckingham, Federico Hernandez.
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
#ifndef INCLUDED_UTF8
#define INCLUDED_UTF8
#define L10N                                           // Localization complete.

#include <string>

unsigned int utf8_codepoint (const std::string&);
unsigned int utf8_next_char (const std::string&, std::string::size_type&);
std::string utf8_character (unsigned int);
int utf8_sequence (unsigned int);
int utf8_length (const std::string&);
int utf8_text_length (const std::string&);


#endif
////////////////////////////////////////////////////////////////////////////////
