// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#ifndef __PROFILER_H_INCLUDED__
#define __PROFILER_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "irrString.h"
#include "irrArray.h"
#include "ITimer.h"

namespace irr
{

class ITimer;

//! Used to store the profile data (and also used for profile group data).
struct SProfileData
{
	friend class CProfiler;

    SProfileData()
	{
		GroupIndex = 0;
		reset();
	}

	bool operator<(const SProfileData& pd) const
	{
		return Id < pd.Id;
	}

	bool operator==(const SProfileData& pd) const
	{
		return Id == pd.Id;
	}

	u32 getGroupIndex() const
	{
		return GroupIndex;
	}

	const core::stringw& getName() const
	{
		return Name;
	}

	//! Each call to Profiler::stop for this data increases the counter by 1.
	u32 getCallsCounter() const
	{
		return CountCalls;
	}

	//! Longest time any call from start/stop ever took for this id.
	u32 getLongestTime() const
	{
		return LongestTime;
	}

	//! Time spend between start/stop
	s32 getTimeSum() const
	{
		return TimeSum;
	}

	//! Convert the whole data into a string
	core::stringw getAsString() const;

	//! Return a string which describes the columns returned by getAsString
	static core::stringw makeTitleString();

private:
	// just to be used for searching as it does no initialization besides id
	SProfileData(u32 id) : Id(id) {}

	void reset();

	s32 Id;
    u32 GroupIndex;
	core::stringw Name;

    u32 CountCalls;
    u32 LongestTime;
    u32 TimeSum;

    u32 LastTimeStarted;
};

//! Code-profiler. Please check the example in the Irrlicht examples folder about how to use it.
// Implementer notes:
// The design is all about allowing to use the central start/stop mechanism with minimal time overhead.
// This is why the class works without a virtual functions interface contrary to the usual Irrlicht design.
// And also why it works with id's instead of strings in the start/stop functions even if it makes using
// the class slightly harder.
// The class comes without reference-counting because the profiler-instance is never released (TBD).
class CProfiler
{
public:
	//! Constructor. You could use this to create a new profiler, but usually getProfiler() is used to access the global instance.
    CProfiler();

	~CProfiler();

	//! Add an id with given name and group which can be used for profiling with start/stop
	/** After calling this once you can start/stop profiling for the given id.
	\param id: Should be >= 0 as negative id's are reserved for Irrlicht. Also very large numbers (near INT_MAX) might
	have been added automatically by the other add function.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data. */
    void add(s32 id, const core::stringw &name, const core::stringw &groupName);

	//! Add an automatically generated for the given name and group which can be used for profiling with start/stop.
	/** After calling this once you can start/stop profiling with the returned id.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data.
	\return Automatic id's start at INT_MAX and count down for each new id. If the name already has an id then that id will be returned. */
    s32 add(const core::stringw &name, const core::stringw &groupName);

	//! Return the number of profile data blocks. There is one for each id.
    u32 getProfileDataCount() const
    {
		return ProfileDatas.size();
    }

	//! Search for the index of the profile data by name
	/** \param result Receives the resulting data index when one was found.
	\param name String with name to search for
	\return true when found, false when not found */
	bool findDataIndex(u32 & result, const core::stringw &name) const;

	//! Get the profile data
	/** \param index A value between 0 and getProfileDataCount()-1.	Indices can change when new id's are added.*/
    const SProfileData& getProfileDataByIndex(u32 index) const
    {
		return ProfileDatas[index];
    }

	//! Get the profile data
	/** \param id Same value as used in ::add
	\return Profile data for the given id or 0 when it does not exist.	*/
    const SProfileData* getProfileDataById(u32 id)
    {
		SProfileData data(id);
    	s32 idx = ProfileDatas.binary_search(data);
		if ( idx >= 0 )
			return &ProfileDatas[idx];
		return NULL;
    }

	//! Get the number of profile groups. Will be at least 1.
	/** NOTE: The first groups is always L"overview" which is an overview for all existing groups */
    u32 getGroupCount() const
    {
		return ProfileGroups.size();
    }

    //! Get profile data for a group.
    /** NOTE: The first groups is always L"overview" which is an overview for all existing groups */
    const SProfileData& getGroupData(u32 index) const
    {
		return ProfileGroups[index];
    }

    //! Find the group index by the group-name
    /** \param result Receives the resulting group index when one was found.
	\param name String with name to search for
	\return true when found, false when not found */
	bool findGroupIndex(u32 & result, const core::stringw &name) const;


	//! Start profile-timing for the given id
	/** NOTE: you have to add the id first with one of the ::add functions */
	inline void start(s32 id);

	//! Stop profile-timing for the given id
	/** NOTE: timer must have been started first with the ::start function */
    inline void stop(s32 id);

	//! Reset profile data for the given id
    void resetDataById(s32 id);

	//! Reset profile data for the given index
    void resetDataByIndex(u32 index);

    //! Reset profile data for a whole group
    void resetGroup(u32 index);

    //! Reset all profile data
    /** NOTE: This is not deleting id's or groups, just resetting all timers to 0. */
    void resetAll();

	//! Write all profile-data into a string
	/** \param result Receives the result string.
	\param includeOverview When true a group-overview is attached first
	\param suppressUncalled When true elements which got never called are not printed */
    void printAll(core::stringw &result, bool includeOverview=false,bool suppressUncalled=true) const;

	//! Write the profile data of one group into a string
	/** \param result Receives the result string.
	\param groupIndex_	*/
    void printGroup(core::stringw &result, u32 groupIndex, bool suppressUncalled) const;

protected:

    u32 addGroup(const core::stringw &name);

private:
    ITimer * Timer;	// I would prefer using os::Timer, but os.h is not in the public interface so far.
    s32 NextAutoId;	// for giving out id's automatically
	core::array<SProfileData> ProfileDatas;
    core::array<SProfileData> ProfileGroups;
};

void CProfiler::start(s32 id)
{
	s32 idx = ProfileDatas.binary_search(SProfileData(id));
	if ( idx >= 0 && Timer )
	{
		ProfileDatas[idx].LastTimeStarted = Timer->getRealTime();
	}
}

void CProfiler::stop(s32 id)
{
	if ( Timer )
	{
		u32 timeNow = Timer->getRealTime();
		s32 idx = ProfileDatas.binary_search(SProfileData(id));
		if ( idx >= 0 )
		{
			SProfileData &data = ProfileDatas[idx];
			if ( data.LastTimeStarted != 0 )
			{
				// update data for this id
				++data.CountCalls;
				u32 diffTime = timeNow - data.LastTimeStarted;
				data.TimeSum += diffTime;
				if ( diffTime > data.LongestTime )
					data.LongestTime = diffTime;
				data.LastTimeStarted = 0;

				// update data of it's group
				SProfileData & group = ProfileGroups[data.GroupIndex];
				++group.CountCalls;
				group.TimeSum += diffTime;
				if ( diffTime > group.LongestTime )
					group.LongestTime = diffTime;
				group.LastTimeStarted = 0;
			}
		}
	}
}

//! Access the Irrlicht profiler object.
/** Profiler is always accessible, except in destruction of global objects.
If you want to get internal profiling information about the engine itself
you will have to re-compile the engine with _IRR_COMPILE_WITH_PROFILING_ enabled.
But you can use the profiler for profiling your own projects without that. */
IRRLICHT_API CProfiler& IRRCALLCONV getProfiler();

//! Class where the objects profile their own life-time.
/** This is a compfort wrapper around the CProfiler start/stop mechanism which is easier to use
when you want to profile a scope. You only have to create an object and it will profile it's own lifetime
for the given id. */
class CProfileScope
{
public:
	//! Construct with an known id.
	/** This is the fastest scope constructor, but the id must have been added before.
	\param id Any id which you did add to the profiler before. */
	CProfileScope(s32 id)
	: Id(id), Profiler(getProfiler())
	{
		Profiler.start(Id);
	}

	//! Object will create the given name, groupName combination for the id if it doesn't exist already
	/** \param id: Should be >= 0 as negative id's are reserved for Irrlicht. Also very large numbers (near INT_MAX) might
	have been created already by the autoamtic add function of ::CProfiler.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data. */
	CProfileScope(s32 id, const core::stringw &name, const core::stringw &groupName)
	: Id(id), Profiler(getProfiler())
	{
		Profiler.add(Id, name, groupName);
		Profiler.start(Id);
	}

	//! Object will create an id for the given name, groupName combination if they don't exist already
	/** Slowest scope constructor, but usually still fine unless speed is very critical.
	\param name: Name for displaying profile data.
	\param groupName: Each id belongs into a group - this helps on displaying profile data. */
	CProfileScope(const core::stringw &name, const core::stringw &groupName)
	: Profiler(getProfiler())
	{
		Id = Profiler.add(name, groupName);
		Profiler.start(Id);
	}

	~CProfileScope()
	{
		Profiler.stop(Id);
	}

protected:
	s32 Id;
	CProfiler& Profiler;
};
} // namespace irr

//! For internal engine use:
//! Code inside IRR_PROFILE is only executed when _IRR_COMPILE_WITH_PROFILING_ is set
//! This allows disabling all profiler code completely by changing that define.
//! It's generally useful to wrap profiler-calls in application code with a similar macro.
#ifdef _IRR_COMPILE_WITH_PROFILING_
	#define IRR_PROFILE(X) X
#else
	#define IRR_PROFILE(X)
#endif // IRR_PROFILE

#endif // __PROFILER_H_INCLUDED__
