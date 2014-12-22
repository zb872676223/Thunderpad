//
//  This file is part of Thunderpad
//
//  Copyright (c) 2013-2014 Alex Spataru <alex.racotta@gmail.com>
//  Please check the license.txt file for more information.
//

#ifndef SYNTAX_HIGHLIGHTER_H
#define SYNTAX_HIGHLIGHTER_H

#include <QDebug>
#include <QRegExp>
#include <QStringList>
#include <QTextStream>
#include <QTextCharFormat>
#include <QXmlStreamReader>
#include <QSyntaxHighlighter>

#include "theme.h"

/*!
 * \class SyntaxHighlighter
 *
 * The \c SyntaxHighlighter is in charge of highlighting the contents
 * of the document of the \c Editor and highlighting relevant matches,
 * such as keywords, types and comments.
 *
 * The \c SyntaxHighlighter uses the highlighting definitions specified
 * by the XML files found in the resources folder of Thunderpad.
 */

class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  explicit SyntaxHighlighter(QTextDocument *parent);

public slots:

  /*!
   * \brief updateColor
   * \param theme
   *
   * Changes the colors to be used to highlight
   * the contents of the document.
   */

  void updateColor(Theme *theme);

  /*!
   * \brief setLanguage
   * \param lang
   *
   * Searches, loads and configures a syntax definition
   * file for the given language.
   */

  void setLanguage(const QString &lang);

  /*!
   * \brief detectLanguage
   * \param file
   *
   * Decides which definition to use based on
   * the extension of the given file.
   */

  void detectLanguage(const QString &file);

protected:
  void highlightBlock(const QString &text);

private:
  struct HighlightingRule {
    QRegExp pattern;
    QTextCharFormat format;
  };

  QVector<HighlightingRule> m_highlighting_rules;

  QString m_lang;

  QTextCharFormat m_others_format;
  QTextCharFormat m_comments_format;
  QTextCharFormat m_keywords_format;
  QTextCharFormat m_functions_format;
};

#endif