#pragma once
#include <chrono>
//#include <boost/serialization/access.hpp>

using Duration = std::chrono::duration<double>;
using Timepoint = std::chrono::time_point<std::chrono::steady_clock, Duration>;

struct Timestep
{
	Timestep(Timepoint prev, Timepoint current, Timepoint first)
		: m_previous{ prev }, m_current{ current }, m_first{ first }
	{}
	Timestep operator=(Timestep& other) // NOTE: What's the benefit of having timepoints const?
	{
		return Timestep{ other.m_previous,other.m_current, other.m_first };
	}
	auto dt() const { return m_current - m_previous; }
	auto current() const { return m_current - m_first; }
	auto last() const { return m_previous - m_first; }
	const Timepoint m_previous, m_current, m_first;
};
//
//// SERIALISATION
//namespace boost {
//    namespace serialization {
//
//        // DURATION
//        template<class Archive>
//        void save(Archive& ar, Duration& g, const unsigned int version)
//        {
//            double c{ g.count() };
//            ar& c;
//        }
//        template<class Archive>
//        void load(Archive& ar, Duration& g, const unsigned int version)
//        {
//            double c{ 0. };
//            ar& c;
//            g = Duration{ c };
//        };
//        template<class Archive>
//        void serialize(Archive& ar, Duration& duration, const unsigned int version)
//        {
//            boost::serialization::split_member(ar, duration, file_version);
//        }
//    } // namespace serialization
//} // namespace boost