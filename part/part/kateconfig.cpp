/* This file is part of the KDE libraries
   Copyright (C) 2003 Christoph Cullmann <cullmann@kde.org>

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

#include "kateconfig.h"

#include "katerenderer.h"
#include "kateview.h"
#include "katedocument.h"
#include "katefactory.h"

#include <kconfig.h>
#include <kglobalsettings.h>
#include <kdebug.h>

// $Id$

KateConfig::KateConfig ()
 : configSessionNumber (0), configIsRunning (false)
{
}

KateConfig::~KateConfig ()
{
}

void KateConfig::configStart ()
{
  configSessionNumber++;

  if (configSessionNumber > 1)
    return;

  configIsRunning = true;
}

void KateConfig::configEnd ()
{
  if (configSessionNumber == 0)
    return;

  configSessionNumber--;

  if (configSessionNumber > 0)
    return;

  configIsRunning = false;

  updateConfig ();
}

KateDocumentConfig *KateDocumentConfig::s_global = 0;
KateViewConfig *KateViewConfig::s_global = 0;
KateRendererConfig *KateRendererConfig::s_global = 0;

KateDocumentConfig::KateDocumentConfig ()
 : m_tabWidthSet (true),
   m_indentationWidthSet (true),
   m_wordWrapSet (true),
   m_wordWrapAtSet (true),
   m_undoStepsSet (true),
   m_doc (0)
{
  s_global = this;

  // init with defaults from config or really hardcoded ones
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate Document Defaults");
  readConfig (config);
}

KateDocumentConfig::KateDocumentConfig (KateDocument *doc)
 : m_tabWidthSet (false),
   m_indentationWidthSet (false),
   m_wordWrapSet (false),
   m_wordWrapAtSet (false),
   m_undoStepsSet (false),
   m_doc (doc)
{
}

KateDocumentConfig::~KateDocumentConfig ()
{
}

KateDocumentConfig *KateDocumentConfig::global ()
{
  if (!s_global)
    s_global = new KateDocumentConfig ();

  return s_global;
}

void KateDocumentConfig::readConfig (KConfig *config)
{
  configStart ();

  setTabWidth (config->readNumEntry("Tab Width", 8));

  setIndentationWidth (config->readNumEntry("Indentation Width", 2));

  setWordWrap (config->readBoolEntry("Word Wrap", false));
  setWordWrapAt (config->readNumEntry("Word Wrap Column", 80));

  setUndoSteps(config->readNumEntry("Undo Steps", 0));

  configEnd ();
}

void KateDocumentConfig::writeConfig (KConfig *config)
{
  config->writeEntry("Tab Width", tabWidth());

  config->writeEntry("Indentation Width", indentationWidth());

  config->writeEntry("Word Wrap", wordWrap());
  config->writeEntry("Word Wrap Column", wordWrapAt());

  config->writeEntry("Undo Steps", undoSteps());

  config->sync ();
}

void KateDocumentConfig::updateConfig ()
{
  if (m_doc)
  {
    m_doc->updateConfig ();
    return;
  }

  if (isGlobal())
  {
    for (uint z=0; z < KateFactory::documents()->count(); z++)
    {
      KateFactory::documents()->at(z)->updateConfig ();
    }
  }
}

int KateDocumentConfig::tabWidth () const
{
  if (m_tabWidthSet || isGlobal())
    return m_tabWidth;

  return s_global->tabWidth();
}

void KateDocumentConfig::setTabWidth (int tabWidth)
{
  if (tabWidth < 1)
    return;

  configStart ();

  m_tabWidthSet = true;
  m_tabWidth = tabWidth;

  configEnd ();
}

int KateDocumentConfig::indentationWidth () const
{
  if (m_indentationWidthSet || isGlobal())
    return m_indentationWidth;

  return s_global->indentationWidth();
}

void KateDocumentConfig::setIndentationWidth (int indentationWidth)
{
  if (indentationWidth < 1)
    return;

  configStart ();

  m_indentationWidthSet = true;
  m_indentationWidth = indentationWidth;

  configEnd ();
}

bool KateDocumentConfig::wordWrap () const
{
  if (m_wordWrapSet || isGlobal())
    return m_wordWrap;

  return s_global->wordWrap();
}

void KateDocumentConfig::setWordWrap (bool on)
{
  configStart ();

  m_wordWrapSet = true;
  m_wordWrap = on;

  configEnd ();
}

unsigned int KateDocumentConfig::wordWrapAt () const
{
  if (m_wordWrapAtSet || isGlobal())
    return m_wordWrapAt;

  return s_global->wordWrapAt();
}

void KateDocumentConfig::setWordWrapAt (unsigned int col)
{
  if (col < 1)
    return;

  configStart ();

  m_wordWrapAtSet = true;
  m_wordWrapAt = col;

  configEnd ();
}

uint KateDocumentConfig::undoSteps () const
{
  if (m_undoStepsSet || isGlobal())
    return m_undoSteps;

  return s_global->undoSteps();
}

void KateDocumentConfig::setUndoSteps (uint undoSteps)
{
  configStart ();

  m_undoStepsSet = true;
  m_undoSteps = undoSteps;

  configEnd ();
}

KateViewConfig::KateViewConfig ()
 :
   m_dynWordWrapSet (true),
   m_dynWordWrapIndicatorsSet (true),
   m_lineNumbersSet (true),
   m_iconBarSet (true),
   m_foldingBarSet (true),
   m_bookmarkSortSet (true),
   m_view (0)
{
  s_global = this;

  // init with defaults from config or really hardcoded ones
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate View Defaults");
  readConfig (config);
}

KateViewConfig::KateViewConfig (KateView *view)
 :
   m_dynWordWrapSet (false),
   m_dynWordWrapIndicatorsSet (false),
   m_lineNumbersSet (false),
   m_iconBarSet (false),
   m_foldingBarSet (false),
   m_bookmarkSortSet (false),
   m_view (view)
{
}

KateViewConfig::~KateViewConfig ()
{
}

KateViewConfig *KateViewConfig::global ()
{
  if (!s_global)
    s_global = new KateViewConfig ();

  return s_global;
}

void KateViewConfig::readConfig (KConfig *config)
{
  configStart ();

  setDynWordWrap (config->readBoolEntry( "Dynamic Word Wrap", true ));
  setDynWordWrapIndicators (config->readNumEntry( "Dynamic Word Wrap Indicators", 1 ));

  setLineNumbers (config->readBoolEntry( "Line Numbers",  false));

  setIconBar (config->readBoolEntry( "Icon Bar", false ));

  setFoldingBar (config->readBoolEntry( "Folding Bar", true));

  setBookmarkSort (config->readNumEntry( "Bookmark Menu Sorting", 0 ));

  configEnd ();
}

void KateViewConfig::writeConfig (KConfig *config)
{
  config->writeEntry( "Dynamic Word Wrap", dynWordWrap() );
  config->writeEntry( "Dynamic Word Wrap Indicators", dynWordWrapIndicators() );

  config->writeEntry( "Line Numbers", lineNumbers() );

  config->writeEntry( "Icon Bar", iconBar() );

  config->writeEntry( "Folding Bar", foldingBar() );

  config->writeEntry( "Bookmark Menu Sorting", bookmarkSort() );

  config->sync ();
}

void KateViewConfig::updateConfig ()
{
  if (m_view)
  {
    m_view->updateConfig ();
    return;
  }

  if (isGlobal())
  {
    for (uint z=0; z < KateFactory::views()->count(); z++)
    {
      KateFactory::views()->at(z)->updateConfig ();
    }
  }
}

bool KateViewConfig::dynWordWrap () const
{
  if (m_dynWordWrapSet || isGlobal())
    return m_dynWordWrap;

  return s_global->dynWordWrap();
}

void KateViewConfig::setDynWordWrap (bool wrap)
{
  configStart ();

  m_dynWordWrapSet = true;
  m_dynWordWrap = wrap;

  configEnd ();
}

int KateViewConfig::dynWordWrapIndicators () const
{
  if (m_dynWordWrapIndicatorsSet || isGlobal())
    return m_dynWordWrapIndicators;

  return s_global->dynWordWrapIndicators();
}

void KateViewConfig::setDynWordWrapIndicators (int mode)
{
  configStart ();

  m_dynWordWrapIndicatorsSet = true;
  m_dynWordWrapIndicators = mode;

  configEnd ();
}

bool KateViewConfig::lineNumbers () const
{
  if (m_lineNumbersSet || isGlobal())
    return m_lineNumbers;

  return s_global->lineNumbers();
}

void KateViewConfig::setLineNumbers (bool on)
{
  configStart ();

  m_lineNumbersSet = true;
  m_lineNumbers = on;

  configEnd ();
}

bool KateViewConfig::iconBar () const
{
  if (m_iconBarSet || isGlobal())
    return m_iconBar;

  return s_global->iconBar();
}

void KateViewConfig::setIconBar (bool on)
{
  configStart ();

  m_iconBarSet = true;
  m_iconBar = on;

  configEnd ();
}

bool KateViewConfig::foldingBar () const
{
  if (m_foldingBarSet || isGlobal())
    return m_foldingBar;

  return s_global->foldingBar();
}

void KateViewConfig::setFoldingBar (bool on)
{
  configStart ();

  m_foldingBarSet = true;
  m_foldingBar = on;

  configEnd ();
}

int KateViewConfig::bookmarkSort () const
{
  if (m_bookmarkSortSet || isGlobal())
    return m_bookmarkSort;

  return s_global->bookmarkSort();
}

void KateViewConfig::setBookmarkSort (int mode)
{
  configStart ();

  m_bookmarkSortSet = true;
  m_bookmarkSort = mode;

  configEnd ();
}

KateRendererConfig::KateRendererConfig ()
 :
   m_viewFont (new FontStruct ()),
   m_printFont (new FontStruct ()),
   m_viewFontSet (true),
   m_printFontSet (true),
   m_wordWrapMarkerSet (true),
   m_renderer (0)
{
  s_global = this;

  // init with defaults from config or really hardcoded ones
  KConfig *config = KateFactory::instance()->config();
  config->setGroup("Kate Renderer Defaults");
  readConfig (config);
}

KateRendererConfig::KateRendererConfig (KateRenderer *renderer)
 : m_viewFont (0),
   m_printFont (0),
   m_viewFontSet (false),
   m_printFontSet (false),
   m_wordWrapMarkerSet (false),
   m_renderer (renderer)
{
}

KateRendererConfig::~KateRendererConfig ()
{
  delete m_viewFont;
  delete m_printFont;
}

KateRendererConfig *KateRendererConfig::global ()
{
  if (!s_global)
    s_global = new KateRendererConfig ();

  return s_global;
};

void KateRendererConfig::readConfig (KConfig *config)
{
  configStart ();

  QFont f (KGlobalSettings::fixedFont());

  setFont(KateRendererConfig::ViewFont, config->readFontEntry("View Font", &f));
  setFont(KateRendererConfig::PrintFont, config->readFontEntry("Printer Font", &f));

  setWordWrapMarker (config->readBoolEntry("Word Wrap Marker", false ));

  configEnd ();
}

void KateRendererConfig::writeConfig (KConfig *config)
{
  config->writeEntry("View Font", *font(KateRendererConfig::ViewFont));
  config->writeEntry("Printer Font", *font(KateRendererConfig::PrintFont));

  config->writeEntry( "Word Wrap Marker", wordWrapMarker() );

  config->sync ();
}

void KateRendererConfig::updateConfig ()
{
  if (m_renderer)
  {
    m_renderer->updateConfig ();
    return;
  }

  if (isGlobal())
  {
    for (uint z=0; z < KateFactory::renderers()->count(); z++)
    {
      KateFactory::renderers()->at(z)->updateConfig ();
    }
  }
}

const FontStruct *KateRendererConfig::fontStruct (int whichFont)
{
  if (whichFont == ViewFont)
  {
    if (m_viewFontSet || isGlobal())
      return m_viewFont;

    return s_global->fontStruct (whichFont);
  }
  else
  {
    if (m_printFontSet || isGlobal())
      return m_printFont;

    return s_global->fontStruct (whichFont);
  }
}

const QFont *KateRendererConfig::font(int whichFont)
{
  return &(fontStruct (whichFont)->myFont);
}

const QFontMetrics *KateRendererConfig::fontMetrics(int whichFont)
{
  return &(fontStruct (whichFont)->myFontMetrics);
}

void KateRendererConfig::setFont(int whichFont, QFont font)
{
  configStart ();

  if (whichFont == ViewFont) {
    if (!m_viewFontSet)
    {
      m_viewFontSet = true;
      m_viewFont = new FontStruct ();
    }

     m_viewFont->setFont(font);

  } else {
    if (!m_printFontSet)
    {
      m_printFontSet = true;
      m_printFont = new FontStruct ();
    }

    m_printFont->setFont(font);
  }

  configEnd ();
}

bool KateRendererConfig::wordWrapMarker () const
{
  if (m_wordWrapMarkerSet || isGlobal())
    return m_wordWrapMarker;

  return s_global->wordWrapMarker();
}

void KateRendererConfig::setWordWrapMarker (bool on)
{
  configStart ();

  m_wordWrapMarkerSet = true;
  m_wordWrapMarker = on;

  configEnd ();
}
