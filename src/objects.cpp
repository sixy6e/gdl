/***************************************************************************
                          objects.cpp  -  global structures
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <ios>

#include "str.hpp"
#include "gdlexception.hpp"
#include "initsysvar.hpp"
#include "dnodefactory.hpp"

#include "objects.hpp"
#include "graphics.hpp"

#ifdef HAVE_LIBWXWIDGETS
#include "gdlwidget.hpp"
#endif

#ifdef USE_PYTHON
#include "gdlpython.hpp"
#endif

using namespace std;

// instantiate the global lists
VarListT      sysVarList;
VarListT      sysVarRdOnlyList;

FunListT      funList;
ProListT      proList;

LibFunListT   libFunList;
LibProListT   libProList;

CommonListT   commonList;

StructListT   structList;

GDLFileListT  fileUnits;

// flag for control-c
volatile bool sigControlC;
int           debugMode;

// instantiate own AST factory
//_DNodeFactory DNodeFactory;
antlr::ASTFactory DNodeFactory("DNode",DNode::factory);

void ResetObjects()
{
  Graphics::DestroyDevices();

  Purge(sysVarList);
  Purge(funList);
  Purge(proList);
  Purge(commonList);
  Purge(structList);
  // no purging of library

#ifdef USE_PYTHON
  // numarray cannot be restarted
  //  PythonEnd();
#endif
}

// initialize struct descriptors which are not system variables
void InitStructs()
{
  SpDLong   aLong;
  SpDString aString;
  SpDByte   aByte;
  SpDLong64 aLong64;
  SpDFloat  aFloat;

  DStructDesc* fstat = new DStructDesc( "FSTAT");
  fstat->AddTag("UNIT", &aLong);
  fstat->AddTag("NAME", &aString);
  fstat->AddTag("OPEN", &aByte);
  fstat->AddTag("ISATTY", &aByte);
  fstat->AddTag("ISAGUI", &aByte);
  fstat->AddTag("INTERACTIVE", &aByte);
  fstat->AddTag("XDR", &aByte);
  fstat->AddTag("COMPRESS", &aByte);
  fstat->AddTag("READ", &aByte);
  fstat->AddTag("WRITE", &aByte);
  fstat->AddTag("ATIME", &aLong64);
  fstat->AddTag("CTIME", &aLong64);
  fstat->AddTag("MTIME", &aLong64);
  fstat->AddTag("TRANSFER_COUNT", &aLong);
  fstat->AddTag("CUR_PTR", &aLong);
  fstat->AddTag("SIZE", &aLong);
  fstat->AddTag("REC_LEN", &aLong);
  // insert into structList
  structList.push_back( fstat);

  DStructDesc* machar = new DStructDesc( "MACHAR");
  machar->AddTag("IBETA", &aLong);
  machar->AddTag("IT", &aLong);
  machar->AddTag("IRND", &aLong);
  machar->AddTag("NGRD", &aLong);
  machar->AddTag("MACHEP", &aLong);
  machar->AddTag("NEGEP", &aLong);
  machar->AddTag("IEXP", &aLong);
  machar->AddTag("MINEXP", &aLong);
  machar->AddTag("MAXEXP", &aLong);
  machar->AddTag("EPS", &aFloat);
  machar->AddTag("EPSNEG", &aFloat);
  machar->AddTag("XMIN", &aFloat);
  machar->AddTag("XMAX", &aFloat);
  // insert into structList
  structList.push_back( machar);
}

void InitObjects()
{
  sigControlC = false;
  debugMode   = 0;

  fileUnits.resize( maxLun); // 0-127 -> 1-128 within GDL for files

  // initialize GDL system variables
  SysVar::InitSysVar();

  // initialize struct descriptors which are not system variables
  InitStructs();

  // graphic devices must be initialized after system variables
  // !D must already exist
  Graphics::Init();

#ifdef HAVE_LIBWXWIDGETS
  // initialize widget system
  GDLWidget::Init();
#endif
}

// returns GDL lun, 0 on failure
DLong GetLUN()
{
  for( DLong lun=maxUserLun+1; lun <= fileUnits.size(); lun++)
    if( !fileUnits[ lun-1].InUse())
      return lun;
  
  return 0;
}

// for semantic predicate
bool IsFun(antlr::RefToken rT1)
{
  antlr::Token& T1=*rT1;

  // search for T1.getText() in function table and path
  string searchName=StrUpCase(T1.getText());

  //  cout << "IsFun: Searching for: " << searchName << endl;

  unsigned fLSize=funList.size();
  for( unsigned f=0; f<fLSize; f++)
    {
      if( funList[f]->Name() == searchName) return true;
    }

  //  cout << "Not found: " << searchName << endl;

  return false;
}

int ProIx(const string& n)
{
SizeT nF=proList.size();
for( SizeT i=0; i<nF; i++) if( Is_eq<DPro>(n)(proList[i])) 
  return (int)i;
return -1;
}

int FunIx(const string& n)
{
SizeT nF=funList.size();
for( SizeT i=0; i<nF; i++) if( Is_eq<DFun>(n)(funList[i]))
  return (int)i;
return -1;
}

int LibProIx(const string& n)
{
  SizeT nF=libProList.size();
  for( SizeT i=0; i<nF; i++) 
    {
      if( Is_eq<DLibPro>(n)(libProList[i])) return (int)i;
    }
  return -1;
}

int LibFunIx(const string& n)
{
  SizeT nF=libFunList.size();
  
  for( SizeT i=0; i<nF; i++) 
    {
      if( Is_eq<DLibFun>(n)(libFunList[i])) return (int)i;
    }
  return -1;
}

// returns the endian of the current machine
const bool BigEndian()
{
  // a long should at least have two bytes
  // big endian -> msb first (msb is 0 here)
  static const unsigned long int s = 0x0001;
  static const bool bigEndian = !(*reinterpret_cast<const unsigned char*>( &s));
  return bigEndian;
}

// test---------------

void breakpoint()
{
  static SizeT num=1;
  cout << "objects.cpp: at breakpoint(): " << num << endl;
  num++;
}
