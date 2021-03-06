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

#ifndef INCLUDED_TDB2
#define INCLUDED_TDB2
#define L10N                                           // Localization complete.

#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <ViewText.h>
#include <File.h>
#include <Task.h>

// TF2 Class represents a single file in the task database.
class TF2
{
public:
  TF2 ();
  ~TF2 ();

  void target (const std::string&);

  const std::vector <Task>&        get_tasks ();
  const std::vector <std::string>& get_lines ();
  const std::string&               get_contents ();

  void add_task (const Task&);
  bool modify_task (const Task&);
  void add_line (const std::string&);
  void clear_lines ();
  void commit ();

  void load_tasks ();
  void load_lines ();
  void load_contents ();

  // ID <--> UUID mapping.
  std::string uuid (int);
  int id (const std::string&);

  void has_ids ();
  void clear ();
  const std::string dump ();

public:
  bool _read_only;
  bool _dirty;
  bool _loaded_tasks;
  bool _loaded_lines;
  bool _loaded_contents;
  bool _has_ids;
  std::vector <Task> _tasks;
  std::vector <Task> _added_tasks;
  std::vector <Task> _modified_tasks;
  std::vector <std::string> _lines;
  std::vector <std::string> _added_lines;
  std::string _contents;
  File _file;

private:
  std::map <int, std::string> _I2U; // ID -> UUID map
  std::map <std::string, int> _U2I; // UUID -> ID map
};

// TDB2 Class represents all the files in the task database.
class TDB2
{
public:
  TDB2 ();
  ~TDB2 ();

  void set_location (const std::string&);
  void add (Task&);
  void modify (Task&);
  void commit ();
  void synch ();
  void merge (const std::string&);
  void revert ();
  int  gc ();
  int  next_id ();

  // Generalized task accessors.
  const std::vector <Task> all_tasks ();
  bool get (int, Task&);
  bool get (const std::string&, Task&);
  const std::vector <Task> siblings (Task&);
  const std::vector <Task> children (Task&);

  // ID <--> UUID mapping.
  std::string uuid (int);
  int id (const std::string&);

  // Read-only mode.
  bool read_only ();

  void clear ();
  void dump ();

private:
  bool verifyUniqueUUID (const std::string&);

public:
  TF2 pending;
  TF2 completed;
  TF2 undo;
//  TF2 backlog;
//  TF2 synch_key;

private:
  std::string _location;
  int _id;
};

#endif
////////////////////////////////////////////////////////////////////////////////
