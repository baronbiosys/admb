/*
 * $Id$
 *
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
/**
 * \file
 * Description not yet available.
 */
// file fvar.cpp
// constructors, destructors and misc functions involving class dvariable

#include "fvar.hpp"

#ifdef __TURBOC__
  #pragma hdrstop
  #include <iostream.h>
#endif

#ifdef __ZTC__
  #include <iostream.hpp>
#endif


#include <stdio.h>
#ifndef __SUN__
#endif
#include <math.h>

/**
 * Description not yet available.
 * \param
 */
dmatrix& dmatrix::operator=(const double x)
{
  for (int i=rowmin();i<=rowmax();i++)
  {
    elem(i)=x;
  }
  return (*this);
}

/**
 * Description not yet available.
 * \param
 */
dmatrix& dmatrix::operator/=(const double x)
{
  for (int i=rowmin();i<=rowmax();i++)
  {
    elem(i)/=x;
  }
  return (*this);
}

/**
 * Description not yet available.
 * \param
 */
dmatrix& dmatrix::operator*=(const double x)
{
  for (int i=rowmin();i<=rowmax();i++)
  {
    elem(i)*=x;
  }
  return (*this);
}
