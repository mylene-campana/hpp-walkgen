//
// Copyright (c) 2015 CNRS
// Authors: Florent Lamiraux
//
// This file is part of hpp-walkgen
// hpp-walkgen is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// hpp-walkgen is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Lesser Public License for more details.  You should have
// received a copy of the GNU Lesser General Public License along with
// hpp-walkgen  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef HPP_WALKGEN_FOOT_PRINT_HH
# define HPP_WALKGEN_FOOT_PRINT_HH

# include <hpp/walkgen/config.hh>
# include <hpp/walkgen/fwd.hh>

namespace hpp {
  namespace walkgen {
    /// Position of a foot on the (horizontal) ground
    struct FootPrint {
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
      FootPrint (const value_type& abs, const value_type& ord) :
	position (abs, ord)
      {
      }
      vector2_t position;
      const value_type& operator[] (size_type index) const
      {
	return position [index];
      }
    }; // struct FootPrint
    typedef std::vector <FootPrint, Eigen::aligned_allocator <FootPrint> >
    FootPrints_t;
  } // namespace walkgen
} // namespace hpp

#endif // HPP_WALKGEN_FOOT_PRINT_HH
