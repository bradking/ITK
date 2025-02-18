// This is core/vnl/algo/vnl_determinant.h
#ifndef vnl_algo_determinant_h_
#define vnl_algo_determinant_h_
//:
// \file
// \brief calculates the determinant of a matrix
// \author fsm
//
//  Evaluation of determinants of any size. For small
//  matrices, will use the direct routines (no netlib)
//  but for larger matrices, a matrix decomposition
//  such as SVD or QR will be used.
//
// \verbatim
//  Modifications
//   dac (Manchester) 26/03/2001: tidied up documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Sep.2003 - Peter Vanroose - specialisation for int added
// \endverbatim

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_algo_export.h>

//: direct evaluation for 2x2 matrix
template <class T>
T
vnl_determinant(const T * row0, const T * row1);

//: direct evaluation for 3x3 matrix
template <class T>
T
vnl_determinant(const T * row0, const T * row1, const T * row2);

//: direct evaluation for 4x4 matrix
template <class T>
T
vnl_determinant(const T * row0, const T * row1, const T * row2, const T * row3);

// overload for int.  Cannot specialize the template because gcc
// 2.95.4 can't handle the default value.  This overload must appear
// before the template declaration because VC.net 7.0 gets confused
// otherwise.
int VNL_ALGO_EXPORT
vnl_determinant(const vnl_matrix<int> & M, bool balance = false);

//: evaluation using direct methods for sizes of 2x2, 3x3, and 4x4 or qr decomposition for other matrices.
//  \relatesalso vnl_matrix
template <class T>
T
vnl_determinant(const vnl_matrix<T> & M, bool balance = false);

//: evaluation using direct methods for sizes of 2x2, 3x3, and 4x4 or qr decomposition for other matrices.
//  convenience overload from vnl_matrix<T> variant
//  \relatesalso vnl_matrix_fixed
template <class T, unsigned m, unsigned n>
inline T
vnl_determinant(const vnl_matrix_fixed<T, m, n> & M, bool balance = false)
{
  return vnl_determinant(M.as_ref(), balance);
}


#define VNL_DETERMINANT_INSTANTIATE(T) extern "you must include vnl/algo/vnl_determinant.hxx first"

#endif // vnl_algo_determinant_h_
