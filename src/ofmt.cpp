/***************************************************************************
                          ofmt.cpp  -  formatted input/output
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

// to be included from datatypes.cpp
#ifdef INCLUDE_OFMT_CPP

#include <sstream>

//#include "datatypes.hpp"
//#include "dstructgdl.hpp"
#include "ofmt.hpp"

using namespace std;

template <>
void OutFixed<DComplex>( ostream& os, DComplex val, int w, int d)
{
  OutFixed( os, val.real(), w, d);
  OutFixed( os, val.imag(), w, d);
}

template <>
void OutFixed<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d)
{
  OutFixed( os, val.real(), w, d);
  OutFixed( os, val.imag(), w, d);
}

template <>
void OutScientific<DComplex>( ostream& os, DComplex val, int w, int d)
{
  OutScientific( os, val.real(), w, d);
  OutScientific( os, val.imag(), w, d);
}

template <>
void OutScientific<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d)
{
  OutScientific( os, val.real(), w, d);
  OutScientific( os, val.imag(), w, d);
}

template <>
void OutAuto<DComplex>( ostream& os, DComplex val, int w, int d)
{
  OutAuto( os, val.real(), w, d);
  OutAuto( os, val.imag(), w, d);
}

template <>
void OutAuto<DComplexDbl>( ostream& os, DComplexDbl val, int w, int d)
{
  OutAuto( os, val.real(), w, d);
  OutAuto( os, val.imag(), w, d);
}

void SetField( int& w, int& d, SizeT defPrec, SizeT maxPrec, SizeT wDef)
{
  if( w == -1)     // (X)
    {
    d = maxPrec;
    w = wDef;
    }
  else if( w == 0) // (X0)
    {
      if( d <= 0) d = defPrec;
    }
  else if( d < 0) 
    d = maxPrec;
}

// common code for all struct output functions
void DStructGDL::OFmtAll( SizeT offs, SizeT r,
			  SizeT& firstOut, SizeT& firstOffs,
			  SizeT& tCount, SizeT& tCountOut)
{
  SizeT nTrans = ToTransfer();

  // transfer count
  tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  tCountOut = tCount;

  // find first Element
  SizeT oneElTr = nTrans / N_Elements();

  SizeT firstEl = offs / oneElTr;
  firstOffs =  offs % oneElTr;

  // find first tag
  SizeT nB = 0;
  SizeT nTags=desc->NTags();
  SizeT firstTag = 0;
  for( firstTag=0; firstTag < nTags; firstTag++)
      {
	SizeT tt=dd[firstTag]->ToTransfer();
	nB += tt;
	if( nB > firstOffs)
	  {
	    nB -= tt;
	    break;
	  }
      }

  firstOut = firstEl * desc->NTags() + firstTag;
  firstOffs -= nB;
}

// A code ****************************************************
// other
template<class Sp> SizeT 
Data_<Sp>::OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  DStringGDL* stringVal = static_cast<DStringGDL*>
    ( this->Convert2( STRING, BaseGDL::COPY));
  SizeT retVal = stringVal->OFmtA( os, offs, r, w);
  delete stringVal;
  return retVal;
}
// string
template<> SizeT Data_<SpDString>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  (*os) << right;

  if( w <= 0)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << dd[ i];
  else
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << setw(w) << dd[ i].substr(0,w);

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  const int len = 13; 	
  
  if( w < 0) w = 0;
  
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;
  
  SizeT firstEl = offs / 2;
  
  (*os) << right;

  if( offs & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( dd[ firstEl++].imag(), len);
      else
	(*os) << setw(w) << i2s( dd[ firstEl++].imag(), len);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( w <= 0)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << i2s( dd[ i].real(), len);
	(*os) << i2s( dd[ i].imag(), len);
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << setw(w) << i2s( dd[ i].real(), len);
	(*os) << setw(w) << i2s( dd[ i].imag(), len);
      }
  
  if( tCount & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( dd[ endEl++].real(), len);
      else
	(*os) << setw(w) << i2s( dd[ endEl++].real(), len);
    }

  return tCountOut;
}
template<> SizeT Data_<SpDComplexDbl>::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  const int len = 16; 	
  
  if( w < 0) w = 0;
  
  SizeT nTrans = ToTransfer();
  
  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;
  
  SizeT firstEl = offs / 2;
  
  (*os) << right;

  if( offs & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( dd[ firstEl++].imag(), len);
      else
	(*os) << setw(w) << i2s( dd[ firstEl++].imag(), len);
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( w <= 0)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << i2s( dd[ i].real(), len);
	(*os) << i2s( dd[ i].imag(), len);
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << setw(w) << i2s( dd[ i].real(), len);
	(*os) << setw(w) << i2s( dd[ i].imag(), len);
      }
  
  if( tCount & 0x01)
    {
      if( w <= 0)
	(*os) << i2s( dd[ endEl++].real(), len);
      else
	(*os) << setw(w) << i2s( dd[ endEl++].real(), len);
    }

  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtA( ostream* os, SizeT offs, SizeT r, int w) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = dd[ firstOut]->OFmtA( os, firstOffs, tCount, w);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = dd[ i]->OFmtA( os, 0, tCount, w);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }

  return tCountOut;
}

// F code ****************************************************
// other
template<class Sp> SizeT Data_<Sp>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  DDoubleGDL* cVal = static_cast<DDoubleGDL*>
    ( this->Convert2( DOUBLE, BaseGDL::COPY));
  SizeT retVal = cVal->OFmtF( os, offs, r, w, d, oMode);
  delete cVal;
  return retVal;
}
// double
template<> SizeT Data_<SpDDouble>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;
 
  SetField( w, d, 6,  16, 25);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, dd[ i], w, d);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, dd[ i], w, d);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, dd[ i], w, d);
    }
  
  return tCount;
}
// float (same code as double)
template<> SizeT Data_<SpDFloat>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  SetField( w, d, 6, 7, 15);

  if( oMode == AUTO) // G
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutAuto( *os, dd[ i], w, d);
    }
  else if( oMode == FIXED) // F, D
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutFixed( *os, dd[ i], w, d);
    }
  else if ( oMode == SCIENTIFIC) // E 
    {
      for( SizeT i=offs; i<endEl; ++i)
	OutScientific( *os, dd[ i], w, d);
    }

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetField( w, d, 6, 7, 15);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, dd[ endEl].real(), w, d);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed( *os, dd[ endEl].real(), w, d);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, dd[ endEl].real(), w, d);
	}
    }
  
  return tCountOut;
}
// same code a float
template<> SizeT Data_<SpDComplexDbl>::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  SetField( w, d, 6, 16, 25);

  if( oMode == AUTO)
    {
      if( offs & 0x01)
	{
	  OutAuto( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;
      
      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutAuto( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutAuto( *os, dd[ endEl].real(), w, d);
	}
    }
  else if( oMode == FIXED)
    {
      if( offs & 0x01)
	{
	  OutFixed( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutFixed( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutFixed( *os, dd[ endEl].real(), w, d);
	}
    }
  else if ( oMode == SCIENTIFIC)
    {
      if( offs & 0x01)
	{
	  OutScientific( *os, dd[ firstEl++].imag(), w, d);
	  tCount--;
	}

      SizeT endEl = firstEl + tCount / 2;

      for( SizeT i= firstEl; i<endEl; ++i)
	{
	  OutScientific( *os, dd[ i], w, d);
	}
  
      if( tCount & 0x01)
	{
	  OutScientific( *os, dd[ endEl].real(), w, d);
	}
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtF( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = dd[ firstOut]->OFmtF( os, firstOffs, tCount, w, d, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = dd[ i]->OFmtF( os, 0, tCount, w, d, oMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }

  return tCountOut;
}

// I code ****************************************************
// other

const int iFmtWidth[] = { -1,  7,  7, 12, 12, 12, 12, 12,  // UNDEF-STRING
			  -1, 12, -1, -1,  7, 12, 22, 22}; // STRUCT-ULONG64

template<class Sp> SizeT Data_<Sp>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  DLongGDL* cVal = static_cast<DLongGDL*>
    ( this->Convert2( LONG, BaseGDL::COPY));
  if( w < 0)
    w = iFmtWidth[ t]; 
  SizeT retVal = cVal->OFmtI( os, offs, r, w, d, oMode);
  delete cVal;
  return retVal;
}
// LONG
template<> SizeT Data_<SpDLong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 12;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << dec << setw(w) << dd[ i];
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << dd[ i];
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << dd[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << dd[ i];

  return tCount;
}
// ULONG
template<> SizeT Data_<SpDULong>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 12;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << dec << setw(w) << dd[ i];
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << dd[ i];
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << dd[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << dd[ i];

  return tCount;
}
// LONG64
template<> SizeT Data_<SpDLong64>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 22;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << dec << setw(w) << dd[ i];
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << dd[ i];
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << dd[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << dd[ i];

  return tCount;
}
// ULONG64
template<> SizeT Data_<SpDULong64>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 22;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;

  SizeT endEl = offs + tCount;

  if( oMode == DEC)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << dec << setw(w) << dd[ i];
  else if ( oMode == OCT)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << oct << setw(w) << dd[ i];
  else if ( oMode == HEX)
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << uppercase << hex << setw(w) << dd[ i];
  else // HEXL
    for( SizeT i=offs; i<endEl; ++i)
      (*os) << nouppercase << hex << setw(w) << dd[ i];

  return tCount;
}
// complex
template<> SizeT Data_<SpDComplex>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 12;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      if( oMode == DEC)
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ firstEl++].imag();
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << static_cast<long int>(dd[ firstEl++].imag());
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ firstEl++].imag());
      else
	(*os) << hex << setw(w) << static_cast<long int>(dd[ firstEl++].imag());
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( oMode == DEC)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ i].real();
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ i].imag();
      }
  else if ( oMode == OCT)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << oct << setw(w) << static_cast<long int>(dd[ i].real());
	(*os) << oct << setw(w) << static_cast<long int>(dd[ i].imag());
      }
  else if ( oMode == HEX)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << uppercase << hex << setw(w) << static_cast<long int>(dd[ i].real());
	(*os) << uppercase << hex << setw(w) << static_cast<long int>(dd[ i].imag());
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ i].real());
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ i].imag());
      }
  
  if( tCount & 0x01)
    {
      if( oMode == DEC)
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ endEl].real();
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << static_cast<long int>(dd[ endEl].real());
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ endEl].real());
      else
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ endEl].real());
    }
  
  return tCountOut;
}
template<> SizeT Data_<SpDComplexDbl>::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  if( w < 0) w = 12;

  SizeT nTrans = ToTransfer();

  // transfer count
  SizeT tCount = nTrans - offs;
  if( r < tCount) tCount = r;
  SizeT tCountOut = tCount;

  SizeT firstEl = offs / 2;

  if( offs & 0x01)
    {
      if( oMode == DEC)
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ firstEl++].imag();
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << static_cast<long int>(dd[ firstEl++].imag());
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ firstEl++].imag());
      else
	(*os) << hex << setw(w) << static_cast<long int>(dd[ firstEl++].imag());
      tCount--;
    }

  SizeT endEl = firstEl + tCount / 2;

  if( oMode == DEC)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ i].real();
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ i].imag();
      }
  else if ( oMode == OCT)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << oct << setw(w) << static_cast<long int>(dd[ i].real());
	(*os) << oct << setw(w) << static_cast<long int>(dd[ i].imag());
      }
  else if ( oMode == HEX)
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << uppercase << hex << setw(w) << static_cast<long int>(dd[ i].real());
	(*os) << uppercase << hex << setw(w) << static_cast<long int>(dd[ i].imag());
      }
  else
    for( SizeT i= firstEl; i<endEl; ++i)
      {
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ i].real());
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ i].imag());
      }
  
  if( tCount & 0x01)
    {
      if( oMode == DEC)
	(*os) << noshowpoint << setprecision(0) << setw(w) << dd[ endEl].real();
      else if ( oMode == OCT)
	(*os) << oct << setw(w) << static_cast<long int>(dd[ endEl].real());
      else if ( oMode == HEX)
	(*os) << uppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ endEl].real());
      else
	(*os) << nouppercase << hex << setw(w) 
	      << static_cast<long int>(dd[ endEl].real());
    }
  
  return tCountOut;
}
// struct
SizeT DStructGDL::
OFmtI( ostream* os, SizeT offs, SizeT r, int w, int d, 
       BaseGDL::IOMode oMode) 
{
  SizeT firstOut, firstOffs, tCount, tCountOut;
  OFmtAll( offs, r, firstOut, firstOffs, tCount, tCountOut);

  SizeT trans = dd[ firstOut]->OFmtI( os, firstOffs, tCount, w, d, oMode);
  if( trans >= tCount) return tCountOut;
  tCount -= trans;

  SizeT ddSize = dd.size();
  for( SizeT i = (firstOut+1); i < ddSize; ++i)
    {
      trans = dd[ i]->OFmtI( os, 0, tCount, w, d, oMode);
      if( trans >= tCount) return tCountOut;
      tCount -= trans;
    }
  
  return tCountOut;
}


//#include "instantiate_templates.hpp"

#endif
