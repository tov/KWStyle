/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsParser.h
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwsParser_h
#define __kwsParser_h

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>

namespace kws
{

#define NUMBER_ERRORS 10

typedef enum
  {
  SPACE,
  TAB
  } IndentType;

typedef enum
  {
  LINE_LENGTH = 1,
  IVAR_PUBLIC = 2,
  IVAR_REGEX = 3,
  SEMICOLON_SPACE = 4,
  DECL_ORDER = 5,
  EOF_NEW_LINE = 6,
  TABS = 7,
  INDENT = 8,
  HEADER = 9,
  NDEFINE = 10
  } ErrorType;

const char ErrorTag[NUMBER_ERRORS][4] = {
   {'L','E','N','\0'},
   {'I','V','P','\0'},
   {'I','V','R','\0'},
   {'S','E','M','\0'},
   {'D','C','L','\0'},
   {'E','O','F','\0'},
   {'T','A','B','\0'},
   {'I','N','D','\0'},
   {'H','R','D','\0'},
   {'D','E','F','\0'}
  };


typedef struct
  {
  unsigned long line; // main line of the error
  unsigned long line2; // second line of error if the error is covering several lines
  unsigned long number;
  std::string description; 
  } Error;

typedef struct
  {
  unsigned long line; // main line of the error
  unsigned long line2; // second line of error if the error is covering several lines
  unsigned long number;
  std::string description; 
  } Info;

class Parser
{
public:
  
  typedef std::vector<Error> ErrorVectorType;

  /** Set the buffer to analyze */
  void SetBuffer(std::string buffer) 
    {
    m_Buffer = buffer;
    this->RemoveComments();
    }
 
   /** Return the error tag as string given the error number */
  std::string GetErrorTag(unsigned long number) const;

  /** Return the erro vector */
  const ErrorVectorType & GetErrors() const {return m_ErrorList;}

  /** Return the last error message */
  std::string GetLastErrors();

  /** Return the information vector */
  std::string GetInfo();

  /** Check if the file contains tabs */
  bool CheckTabs();

  /** Check the indent size 
   *  Not in the header file if there is one */
  bool CheckIndent(IndentType,unsigned long size);

  /** Check the number of character per line */
  bool CheckLineLength(unsigned long max);

  /** Check if the internal parameters of the class are correct */
  bool CheckInternalVariables(const char* regEx);

  /** Check the order of the declaration */
  bool CheckDeclarationOrder(unsigned int posPublic, unsigned int posProtected, unsigned int posPrivate);

  /** Check the number of space between the end of the declaration
   *  and the semicolon */
  bool CheckSemicolonSpace(unsigned long max);

  /** Check if the end of the file has a new line */
  bool CheckEndOfFileNewLine();

  /** Check header */
  bool CheckHeader(const char* filename,bool considerSpaceEOL = true);

  /** Check if the #ifndef/#define is defined correctly for the class */
  bool CheckIfNDefDefine(const char* match);

  /** Remove the comments. */
  void RemoveComments();

  /** Clear the error list */
  void ClearErrors() {m_ErrorList.clear();}
  
  /** Clear the info list */
  void ClearInfo() {m_InfoList.clear();}

  /** Set the filename of the file we are checking */
  void SetFilename(const char* filename) {m_Filename = filename;}
  std::string GetFilename() const {return m_Filename;}

  /** Return the number of lines */
  unsigned long GetNumberOfLines() const;

  /** Return the line */
  std::string GetLine(unsigned long i) const;

protected:

  /** Find an ivar in the source code */
  std::string Parser::FindInternalVariable(long int start, long int end,long int& pos);

  /** Reduces multiple spaces in buffer to one. */
  void ReduceMultipleSpaces(std::string & buffer);

  /** Removes all val chars from string. */
  void RemoveChar(std::string & buffer, char val) const;

  /** Find the line number in the commented text given the character description */
  long int GetLineNumber(long int pos,bool withoutComments=false);

  /** Find the parameters. */
  //void FindAndAddParameters(std::string buffer, XMLDescription &desc, size_t startPos=0) const;

  /** Find the typedefs (only the public ones). */
  //void FindAndAddTypeDefinitions(const std::string & buffer, XMLDescription &desc, size_t startPos=0) const;

  /** Find the name of the filter and templates, etc ... Returns the position 
      within the file where class definition starts. */
  //size_t FindAndAddName(const std::string & buffer, XMLDescription &desc, std::string fileName) const;

  /** Find name spaces. */
  //void FindAndAddNameSpace(const std::string & buffer, XMLDescription &desc) const;

  /** Find the closing bracket given the position of the opening bracket. */
  long int FindClosingChar(char openChar, char closeChar, long int pos) const;

 
  /** Find the constructor in the file. */
  long FindConstructor(const std::string & buffer, const std::string & className, bool headerfile=true, size_t startPos=0) const;

  /** Find first occurrence of a SetFunction which is not implemented by a macro. 
      Return position after function ")" if found or any position after "void" if not found.
      flag = true indicates successful finding, otherwise false.
      isVoidSetFunc is returned as true if a function like "void Set...()" or
      "void Set...(void)" is detected.
   */
  size_t FindAndAddNonMacroSetFunction(std::string buffer, long startPos,
                                       std::string &paramName, 
                                       std::string &paramType,
                                       bool &flag,
                                       bool &isVoidSetFunc) const;

  /** Starting from startPos scan for all SetFunctions which are not implemented 
    by a macro. Add their type and name to the list of parameters. */
  //void FindAndAddNonMacroSetFunctions(std::string buffer, XMLDescription &desc, long startPos) const;

  /** Find the defaults var values in the .txx or .cxx file. */
  //void FindAndAddDefaultValues(std::string buffer, long start, XMLDescription &desc) const;

  /** Return true if the position pos is between <>. */
  bool IsBetweenBrackets(std::string & buf, long int pos) const;

  /** Removes ass CtrlN characters from the buffer. */
  void RemoveCtrlN(std::string & buffer) const;

  /** Find public area in source code. */
  void FindPublicArea(long &before, long &after, size_t startPos=0) const;

  /** Find protected area in source code. */
  void FindProtectedArea(long &before, long &after, size_t startPos=0) const;

  /** Find private area in source code. */
  void FindPrivateArea(long &before, long &after, size_t startPos=0) const;

  /** Find the maximum setInput method. In some classes it helps to detect
      the correct number of inputs. */
  int FindSetInputNum(const std::string & buffer, size_t startPos=0) const;

  /** Return the position of the template if the class has a template otherwise -1. */
  long int IsTemplated(const std::string & buffer, long int pos) const;

  /** Find the comments. */
  //bool FindAndAddComments(const std::string & buffer, XMLDescription &desc) const;

  /** Remove string area from start to end (incl) from buffer if filename contains className. */
  std::string RemoveArea(const std::string fileName,
                         std::string buffer, 
                         const std::string className,
                         const std::string &start, const std::string &end);


private:

  ErrorVectorType m_ErrorList;
  std::vector<Info> m_InfoList;
  std::string m_Buffer;
  std::string m_BufferNoComment;
  std::vector<long int> m_Positions;
  typedef std::pair<long int, long int> PairType;
  std::vector<PairType> m_CommentPositions;
  std::string m_Filename;
};

} // end namespace kws

#endif
