#! /usr/bin/perl
################################################################################
## taskwarrior - a command line task list manager.
##
## Copyright 2006-2012, Paul Beckingham, Federico Hernandez.
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included
## in all copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
## OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
## THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
##
## http://www.opensource.org/licenses/mit-license.php
##
################################################################################

use strict;
use warnings;
use Test::More tests => 9;

# Create the rc file.
if (open my $fh, '>', 'undo.rc')
{
  print $fh "data.location=.\n",
            "echo.command=no\n",
            "confirmation=no\n";
  close $fh;
  ok (-r 'undo.rc', 'Created undo.rc');
}

# Test the add/do/undo commands.
my $output = qx{../src/task rc:undo.rc add one; ../src/task rc:undo.rc info 1};
ok (-r 'pending.data', 'pending.data created');
like ($output, qr/Status\s+Pending\n/, 'Pending');

$output = qx{../src/task rc:undo.rc 1 do; ../src/task rc:undo.rc info 1};
ok (-r 'completed.data', 'completed.data created');
like ($output, qr/Status\s+Completed\n/, 'Completed');

$output = qx{../src/task rc:undo.rc undo; ../src/task rc:undo.rc info 1};
ok (-r 'completed.data', 'completed.data created');
like ($output, qr/Status\s+Pending\n/, 'Pending');

$output = qx{../src/task rc:undo.rc 1 do; ../src/task rc:undo.rc list};
like ($output, qr/No matches/, 'No matches');

# Cleanup.
unlink qw(pending.data completed.data undo.data backlog.data synch.key undo.rc);
ok (! -r 'pending.data'   &&
    ! -r 'completed.data' &&
    ! -r 'undo.data'      &&
    ! -r 'backlog.data'   &&
    ! -r 'synch.key'      &&
    ! -r 'undo.rc', 'Cleanup');

exit 0;

