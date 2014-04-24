// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#include "CProfiler.h"
#include "CTimer.h"
#include <limits.h>

namespace irr
{
IRRLICHT_API CProfiler& IRRCALLCONV getProfiler()
{
	static CProfiler profiler;
	return profiler;
}


//! Convert the whole data into a string
core::stringw SProfileData::getAsString() const
{
	if ( CountCalls > 0 )
	{
#ifdef _MSC_VER
#pragma warning(disable:4996)	// 'sprintf' was declared deprecated
#endif
		// Can't use swprintf as it fails on some platforms (especially mobile platforms)
		// Can't use Irrlicht functions because we have no string formatting.
		char dummy[1023];
		sprintf(dummy, "%-15.15s%-12u%-12u%-12u%-12u",
			core::stringc(Name).c_str(), CountCalls, TimeSum,
			TimeSum / CountCalls, LongestTime);
		dummy[1022] = 0;

		return core::stringw(dummy);
#ifdef _MSC_VER
#pragma warning(default :4996)	// 'sprintf' was declared deprecated
#endif
	}
	else
	{
		return Name;
	}
}

//! Return a string which describes the columns returned by getAsString
core::stringw SProfileData::makeTitleString()
{
	return core::stringw("name           calls       time(sum)   time(avg)   time(max)");
}

void SProfileData::reset()
{
	CountCalls = 0;
	LongestTime = 0;
	TimeSum = 0;
	LastTimeStarted = 0;
}



CProfiler::CProfiler()
: Timer(0)
, NextAutoId(INT_MAX)
{
	Timer = new CTimer(true);

	addGroup(L"overview");
}

CProfiler::~CProfiler()
{
	if ( Timer )
		Timer->drop();
}

s32 CProfiler::add(const core::stringw &name, const core::stringw &groupName)
{
	u32 index;
	if ( findDataIndex(index, name) )
	{
		add( ProfileDatas[index].Id, name, groupName );
		return ProfileDatas[index].Id;
	}
	else
	{
		s32 id = NextAutoId;
		--NextAutoId;
		add( id, name, groupName );
		return id;
	}
}

void CProfiler::add(s32 id, const core::stringw &name, const core::stringw &groupName)
{
	u32 groupIdx;
	if ( !findGroupIndex(groupIdx, groupName) )
	{
		groupIdx = addGroup(groupName);
	}

	SProfileData data(id);
	s32 idx = ProfileDatas.binary_search(data);
	if ( idx < 0 )
	{
		data.reset();
		data.GroupIndex = groupIdx;
		data.Name = name;

		ProfileDatas.push_back(data);
		ProfileDatas.sort();
	}
	else
	{
		// only reset on group changes, otherwise we want to keep the data or coding CProfileScope would become tricky.
		if ( groupIdx != ProfileDatas[idx].GroupIndex )
		{
			resetDataByIndex((u32)idx);
			ProfileDatas[idx].GroupIndex = groupIdx;
		}
		ProfileDatas[idx].Name = name;
	}
}

u32 CProfiler::addGroup(const core::stringw &name)
{
    SProfileData group;
	group.Id = -1;	// Id for groups doesn't matter so far
	group.Name = name;
    ProfileGroups.push_back(group);
    return ProfileGroups.size()-1;
}

bool CProfiler::findDataIndex(u32 & result, const core::stringw &name) const
{
	for ( u32 i=0; i < ProfileDatas.size(); ++i )
	{
		if ( ProfileDatas[i].Name == name )
		{
			result = i;
			return true;
		}
	}

	return false;
}

bool CProfiler::findGroupIndex(u32 & result, const core::stringw &name) const
{
	for ( u32 i=0; i < ProfileGroups.size(); ++i )
	{
		if ( ProfileGroups[i].Name == name )
		{
			result = i;
			return true;
		}
	}

	return false;
}

void CProfiler::resetDataById(s32 id)
{
	s32 idx = ProfileDatas.binary_search(SProfileData(id));
    if ( idx >= 0 )
    {
		resetDataByIndex((u32)idx);
    }
}

void CProfiler::resetDataByIndex(u32 index)
{
	SProfileData &data = ProfileDatas[index];

	SProfileData & group = ProfileGroups[data.GroupIndex];
	group.CountCalls -= data.CountCalls;
	group.TimeSum -= data.TimeSum;

	data.reset();
}

//! Reset profile data for a whole group
void CProfiler::resetGroup(u32 index)
{
	for ( u32 i=0; i<ProfileDatas.size(); ++i )
    {
		if ( ProfileDatas[i].GroupIndex == index )
			ProfileDatas[i].reset();
    }
    if ( index < ProfileGroups.size() )
		ProfileGroups[index].reset();
}

void CProfiler::resetAll()
{
	for ( u32 i=0; i<ProfileDatas.size(); ++i )
    {
		ProfileDatas[i].reset();
    }

	for ( u32 i=0; i<ProfileGroups.size(); ++i )
    {
		ProfileGroups[i].reset();
    }
}

void CProfiler::printAll(core::stringw &ostream, bool includeOverview, bool suppressUncalled) const
{
    ostream += SProfileData::makeTitleString();
    ostream += L"\n";
	for ( u32 i=includeOverview ?0:1; i<ProfileGroups.size(); ++i )
    {
        printGroup( ostream, i, suppressUncalled );
    }
}

void CProfiler::printGroup(core::stringw &ostream, u32 idxGroup, bool suppressUncalled) const
{
    ostream += ProfileGroups[idxGroup].getAsString();
    ostream += L"\n";

	// print overview for groups
    if ( idxGroup == 0 )
    {
		for ( u32 i=0; i<ProfileGroups.size(); ++i )
        {
            if ( !suppressUncalled || ProfileGroups[i].CountCalls > 0)
            {
                ostream += ProfileGroups[i].getAsString();
                ostream += L"\n";
            }
        }
    }
	// print all data in a group
    else
    {
		for ( u32 i=0; i<ProfileDatas.size(); ++i )
        {
            if ( (!suppressUncalled || ProfileDatas[i].CountCalls > 0)
				&& ProfileDatas[i].GroupIndex == idxGroup )
            {
                ostream += ProfileDatas[i].getAsString();
                ostream += L"\n";
            }
        }
    }
}

} // namespace irr
