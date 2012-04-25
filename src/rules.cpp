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

#include <iostream>
#include <stdlib.h>
#include <Context.h>
#include <Date.h>
#include <text.h>
#include <util.h>
#include <main.h>

extern Context context;

static std::map <std::string, Color> gsColor;
static std::vector <std::string> gsPrecedence;

////////////////////////////////////////////////////////////////////////////////
void initializeColorRules ()
{
  gsColor.clear ();
  gsPrecedence.clear ();

  // Load all the configuration values, filter to only the ones that begin with
  // "color.", then store name/value in gsColor, and name in rules.
  std::vector <std::string> rules;
  std::vector <std::string> variables;
  context.config.all (variables);
  std::vector <std::string>::iterator v;
  for (v = variables.begin (); v != variables.end (); ++v)
  {
    if (v->substr (0, 6) == "color.")
    {
      Color c (context.config.get (*v));
      gsColor[*v] = c;

      rules.push_back (*v);
    }
  }

  // Load the rule.precedence.color list, split it, then autocomplete against
  // the 'rules' vector loaded above.
  std::vector <std::string> results;
  std::vector <std::string> precedence;
  split (precedence, context.config.get ("rule.precedence.color"), ',');

  std::vector <std::string>::iterator p;
  for (p = precedence.begin (); p != precedence.end (); ++p)
  {
    // Add the leading "color." string.
    std::string rule = "color." + *p;
    autoComplete (rule, rules, results, 3); // Hard-coded 3.

    std::vector <std::string>::iterator r;
    for (r = results.begin (); r != results.end (); ++r)
      gsPrecedence.push_back (*r);
  }
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeBlocked (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (dependencyIsBlocked (task))
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeTagged (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.getTagCount ())
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizePriorityL (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.get ("priority") == "L")
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizePriorityM (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.get ("priority") == "M")
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizePriorityH (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.get ("priority") == "H")
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizePriorityD (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.get ("priority") == "D")
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizePriorityNone (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.get ("priority") == "")
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeActive (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial () &&
      task.has ("start")          &&
      !task.has ("end"))
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeTag (Task& task, const std::string& rule, Color& c)
{
  if (task.hasTag (rule.substr (10)))
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeProject (Task& task, const std::string& rule, Color& c)
{
  // Observe the case sensitivity setting.
  bool sensitive = context.config.getBoolean ("search.case.sensitive");

  std::string project = task.get ("project");
  std::string rule_trunc = rule.substr (14);

  // Match project names leftmost.
  if (rule_trunc.length () <= project.length ())
    if (compare (rule_trunc, project.substr (0, rule_trunc.length ()), sensitive))
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeProjectNone (Task& task, const std::string& rule, Color& c)
{
  if (task.get ("project") == "")
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeTagNone (Task& task, const std::string& rule, Color& c)
{
  if (task.getTagCount () == 0)
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeKeyword (Task& task, const std::string& rule, Color& c)
{
  // Observe the case sensitivity setting.
  bool sensitive = context.config.getBoolean ("search.case.sensitive");

  // The easiest thing to check is the description, because it is just one
  // attribute.
  if (find (task.get ("description"), rule.substr (14), sensitive) != std::string::npos)
    c.blend (gsColor[rule]);

  // Failing the description check, look at all annotations, returning on the
  // first match.
  else
  {
    Task::iterator it;
    for (it = task.begin (); it != task.end (); ++it)
    {
      if (it->first.substr (0, 11) == "annotation_" &&
          find (it->second, rule.substr (14), sensitive) != std::string::npos)
      {
        c.blend (gsColor[rule]);
        return;
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeDue (Task& task, const std::string& rule, Color& c)
{
  Task::status status = task.getStatus ();

  if (task.has ("due")          &&
      status != Task::completed &&
      status != Task::deleted)
  {
    if (getDueState (task.get ("due")) == 1)
      c.blend (gsColor[rule]);
  }
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeDueToday (Task& task, const std::string& rule, Color& c)
{
  Task::status status = task.getStatus ();

  if (task.has ("due")          &&
      status != Task::completed &&
      status != Task::deleted)
  {
    if (getDueState (task.get ("due")) == 2)
      c.blend (gsColor[rule]);
  }
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeOverdue (Task& task, const std::string& rule, Color& c)
{
  Task::status status = task.getStatus ();

  if (task.has ("due")          &&
      status != Task::completed &&
      status != Task::deleted)
  {
    if (getDueState (task.get ("due")) == 3)
      c.blend (gsColor[rule]);
  }
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeRecurring (Task& task, const std::string& rule, Color& c)
{
  if (gsColor[rule].nontrivial ())
    if (task.has ("recur"))
      c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeCompleted (Task& task, const std::string& rule, Color& c)
{
  if (task.getStatus () == Task::completed)
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
static void colorizeDeleted (Task& task, const std::string& rule, Color& c)
{
  if (task.getStatus () == Task::completed)
    c.blend (gsColor[rule]);
}

////////////////////////////////////////////////////////////////////////////////
void autoColorize (Task& task, Color& c)
{
  // The special tag 'nocolor' overrides all auto and specific colorization.
  if (!context.color () ||
      task.hasTag ("nocolor"))
  {
    c = Color ();
    return;
  }

  // Note: c already contains colors specifically assigned via command.
  // Note: These rules form a hierarchy - the last rule is King, hence the
  //       reverse iterator.
  std::vector <std::string>::reverse_iterator r;
  for (r = gsPrecedence.rbegin (); r != gsPrecedence.rend (); ++r)
  {
         if (*r == "color.blocked")                colorizeBlocked      (task, *r, c);
    else if (*r == "color.tagged")                 colorizeTagged       (task, *r, c);
    else if (*r == "color.pri.L")                  colorizePriorityL    (task, *r, c);
    else if (*r == "color.pri.M")                  colorizePriorityM    (task, *r, c);
    else if (*r == "color.pri.H")                  colorizePriorityH    (task, *r, c);
    else if (*r == "color.pri.D")                  colorizePriorityD    (task, *r, c);
    else if (*r == "color.pri.none")               colorizePriorityNone (task, *r, c);
    else if (*r == "color.active")                 colorizeActive       (task, *r, c);
    else if (*r == "color.project.none")           colorizeProjectNone  (task, *r, c);
    else if (*r == "color.tag.none")               colorizeTagNone      (task, *r, c);
    else if (*r == "color.due")                    colorizeDue          (task, *r, c);
    else if (*r == "color.due.today")              colorizeDueToday     (task, *r, c);
    else if (*r == "color.overdue")                colorizeOverdue      (task, *r, c);
    else if (*r == "color.recurring")              colorizeRecurring    (task, *r, c);
    else if (*r == "color.completed")              colorizeCompleted    (task, *r, c);
    else if (*r == "color.deleted")                colorizeDeleted      (task, *r, c);

    // Wildcards
    else if (r->substr (0,  9) == "color.tag")     colorizeTag          (task, *r, c);
    else if (r->substr (0, 13) == "color.project") colorizeProject      (task, *r, c);
    else if (r->substr (0, 13) == "color.keyword") colorizeKeyword      (task, *r, c);
  }
}

////////////////////////////////////////////////////////////////////////////////
std::string colorizeHeader (const std::string& input)
{
  if (gsColor["color.header"].nontrivial ())
    return gsColor["color.header"].colorize (input);

  return input;
}

////////////////////////////////////////////////////////////////////////////////
std::string colorizeFootnote (const std::string& input)
{
  if (gsColor["color.footnote"].nontrivial ())
    return gsColor["color.footnote"].colorize (input);

  return input;
}

////////////////////////////////////////////////////////////////////////////////
std::string colorizeDebug (const std::string& input)
{
  if (gsColor["color.debug"].nontrivial ())
    return gsColor["color.debug"].colorize (input);

  return input;
}

////////////////////////////////////////////////////////////////////////////////
