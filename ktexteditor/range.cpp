/* This file is part of the KDE libraries
   Copyright (C) 2003-2005 Hamish Rodda <rodda@kde.org>
   Copyright (C) 2002 Christian Couder <christian@kdevelop.org>
   Copyright (C) 2001, 2003 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 1999 Jochen Wilhelmy <digisnap@cs.tu-berlin.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "range.h"

using namespace KTextEditor;

Range::Range()
  : m_start(new Cursor())
  , m_end(new Cursor())
{
  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(const Cursor& start, const Cursor& end)
{
  if (start <= end) {
    m_start = new Cursor(start);
    m_end = new Cursor(end);

  } else {
    m_start = new Cursor(end);
    m_end = new Cursor(start);
  }

  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(const Cursor& start, int width)
  : m_start(new Cursor(start))
  , m_end(new Cursor(start.line(), start.column() + width))
{
  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(const Cursor& start, int endLine, int endCol)
  : m_start(new Cursor(start))
  , m_end(new Cursor(endLine, endCol))
{
  if (*m_end < *m_start) {
    Cursor* temp = m_end;
    m_end = m_start;
    m_start = temp;
  }

  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(int startLine, int startCol, int endLine, int endCol)
  : m_start(new Cursor(startLine, startCol))
  , m_end(new Cursor(endLine, endCol))
{
  if (*m_end < *m_start) {
    Cursor* temp = m_end;
    m_end = m_start;
    m_start = temp;
  }

  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(Cursor* start, Cursor* end)
  : m_start(start)
  , m_end(end)
{
  if (*m_end < *m_start) {
    Cursor temp = *m_end;
    *m_end = *m_start;
    *m_start = temp;
  }

  m_start->setRange(this);
  m_end->setRange(this);
}

Range::Range(const Range& copy)
  : m_start(new Cursor(copy.start()))
  , m_end(new Cursor(copy.end()))
{
  m_start->setRange(this);
  m_end->setRange(this);
}

Range::~Range()
{
  delete m_start;
  delete m_end;
}

bool Range::isValid( ) const
{
  return start().line() >= 0 && start().column() >= 0;
}

const Range& Range::invalid()
{
  static Range r;
  if (r.start().line() != -1)
    r.setRange(Cursor(-1,-1),Cursor(-1,-1));
  return r;
}

Range& Range::operator= (const Range& rhs)
{
  if (this == &rhs)
    return *this;
  
  setRange(rhs);
  
  return *this;
}

void Range::setRange(const Range& range)
{
  *m_start = range.start();
  *m_end = range.end();
}

void Range::setRange( const Cursor & start, const Cursor & end )
{
  if (start > end) {
    *m_start = end;
    *m_end = start;
  } else {
    *m_start = start;
    *m_end = end;
  }
}

bool Range::containsLine(int line) const
{
  return (line > start().line() || line == start().line() && !start().column()) && line < end().line();
}

bool Range::includesLine(int line) const
{
  return line >= start().line() && line <= end().line();
}

bool Range::spansColumn( int col ) const
{
  return start().column() <= col && end().column() > col;
}

bool Range::contains( const Cursor& cursor ) const
{
  return cursor >= start() && cursor < end();
}

bool Range::contains( const Range& range ) const
{
  return range.start() >= start() && range.end() <= end();
}

bool Range::overlaps( const Range& range ) const
{
  if (range.start() <= start())
    return range.end() > start();

  else if (range.end() >= end())
    return range.start() < end();

  else
    return contains(range);
}

bool Range::boundaryAt(const Cursor& cursor) const
{
  return cursor == start() || cursor == end();
}

bool Range::boundaryOnLine(int line) const
{
  return start().line() == line || end().line() == line;
}

void Range::confineToRange(const Range& range)
{
  if (start() < range.start())
    if (end() > range.end())
      setRange(range);
    else
      start() = range.start();
  else if (end() > range.end())
    end() = range.end();
}

void Range::expandToRange(const Range& range)
{
  if (start() > range.start())
    if (end() < range.end())
      setRange(range);
    else
      start() = range.start();
  else if (end() < range.end())
    end() = range.end();
}

void Range::cursorChanged( Cursor * c )
{
  if (c == m_start) {
    if (*c > *m_end)
      *m_end = *c;

  } else if (c == m_end) {
    if (*c < *m_start)
      *m_start = *c;
  }
}

// kate: space-indent on; indent-width 2; replace-tabs on;
