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

#include <hpp/model/joint.hh>
#include <hpp/walkgen/foot-trajectory.hh>
#include <hpp/model/device.hh>
#include <hpp/model/joint-configuration.hh>

namespace hpp {
  namespace walkgen {
    static size_type footConfigSize = 5;
    static size_type footNumberDof = 4;
    static model::rotationJointConfig::UnBounded SO2;

    model::DevicePtr_t createFootDevice ()
    {
      model::DevicePtr_t device = model::Device::create ("foot");
      Transform3f position;
      model::JointPtr_t root = new model::JointTranslation <3> (position);
      device->rootJoint (root);

      /// Rotate around z:
      fcl::Matrix3f R;
      R (0,0) = 0; R (0,1) = 0; R (0,2) =-1;
      R (1,0) = 0; R (1,1) = 1; R (1,2) = 0;
      R (2,0) = 1; R (2,1) = 0; R (2,2) = 0;

      position.setRotation (R);
      model::JointPtr_t yaw = new model::jointRotation::UnBounded (position);
      root->addChildJoint (yaw);

      R (0,0) = 1; R (0,1) = 0; R (0,2) = 0;
      R (1,0) = 0; R (1,1) = 1; R (1,2) = 0;
      R (2,0) = 0; R (2,1) = 0; R (2,2) = 1;
      position.setRotation (R);
      model::JointPtr_t foot_print = new model::JointAnchor (position);
      foot_print->name ("foot");
      yaw->addChildJoint (foot_print);
      device->controlComputation (model::Device::JOINT_POSITION);
      return device;
    }

    value_type Step::computeAngle (const FootPrint& start, const FootPrint& end)
      const
    {
      vector1_t result;
      SO2.difference (end.orientation, start.orientation, 0, 0, result);
      return result [0];
    }

    StepPtr_t Step::create (const FootPrint& start, const FootPrint& end,
			    const value_type& stepLow,
			    const value_type& stepHigh,
			    const value_type& duration)
    {
      return StepPtr_t (new Step (start, end, stepLow, stepHigh, duration));
    }

    Step::Step (const FootPrint& start, const FootPrint& end,
		const value_type& stepLow, const value_type& stepHigh,
		const value_type& duration) :
      Path (std::make_pair (0, duration), footConfigSize, footNumberDof),
      h0_ (start.position), h2_ (3*(end.position - start.position)),
      h3_ (2*(start.position - end.position)),
      v0_ (stepLow), v1_ (0), v2_ (3*(stepHigh - stepLow)),
      v3_ (2*(stepLow - stepHigh)), initialOrientation_ (start.orientation),
      angle_ (computeAngle (start, end)),
      omega0_ (0.), omega1_ (0.), omega2_ (3*angle_), omega3_ (-2*angle_),
      initial_ (footConfigSize), final_ (footConfigSize)
    {
      h1_.setZero ();
      initial_.segment <2> (0) = start.position;
      initial_.segment <2> (2) = start.orientation;
      initial_ [4] = stepLow;
      final_.segment <2> (0) = end.position;
      final_.segment <2> (2) = end.orientation;
      final_ [4] = stepHigh;
    }

    bool Step::impl_compute (ConfigurationOut_t configuration,
			     value_type t) const
    {
      // Scale parameter between 0 and 1
      value_type duration = length ();
      value_type th = t/duration;
      value_type tv;
      if (t <= duration/2.) {
	tv = 2.*t/(duration);
      } else {
	tv = 2.-(2.*t/(duration));
      }
      configuration.head <2> () = h0_ + th*(h1_ + th*(h2_ + th*h3_));
      configuration [2] = v0_ + tv*(v1_ + tv*(v2_ + tv*v3_));
      // Orientation
      vector1_t omega;
      omega [0] = th*th*(omega2_ + th*omega3_);
      SO2.integrate (initialOrientation_, omega, 0, 0,
		     configuration.segment <2> (3));
      return true;
    }

    SupportFootPtr_t SupportFoot::create (const FootPrint& position,
					  const value_type& footHeight,
					  const value_type& duration)
    {
      return SupportFootPtr_t (new SupportFoot
			       (position, footHeight, duration));
    }

    SupportFoot::SupportFoot (const FootPrint& position,
			      const value_type& footHeight,
			      const value_type& duration) :
      Path (std::make_pair (0, duration), footConfigSize, footNumberDof),
      h0_ (position.position), v0_ (footHeight),
      orientation_ (position.orientation), configuration_ (footConfigSize)
    {
      configuration_.segment <2> (0) = position.position;
      configuration_ [2] = footHeight;
      configuration_.segment <2> (3) = position.orientation;
    }

    bool SupportFoot::impl_compute (ConfigurationOut_t configuration,
				    value_type) const
    {
      configuration.head <2> () = h0_;
      configuration [2] = v0_;
      configuration.segment <2> (3) = orientation_;
      return true;
    }

  } // namespace walkgen
} // namespace hpp
