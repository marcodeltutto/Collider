// 
//
// IwGame - Cross Platform Multi-purpose Game Engine using the Marmalade SDK
//
// Developed by Matthew Hopwood of Pocketeers Limited - www.pocketeers.co.uk
//
// For updates, tutorials and more details check out my blog at www.drmop.com
//
// This code is provided free of charge and without any warranty whatsoever. The only restriction to its usage is that this header must remain intact and visible in all IwGame engine files.
// If you use this engine in your product, whilst it is not mandatory, a mention of the IwGame engine would be nice or a quick email to let us know where it is being used.
//
//

#if !defined(_CIW_GAME_UTIL_H_)
#define _CIW_GAME_UTIL_H_

#include "s3e.h"
#include "IwList.h"

#ifndef NULL
    #define NULL 0
#endif

#define SAFE_DELETE(x) if (x != NULL) { delete x; x = NULL; }
#define SAFE_DELETE_ARRAY(x) if (x != NULL) { delete [] x; x = NULL; }

#define	IW_GAME_HASH(x)	CIwGameString::CalculateHash(x)

#define	FRAME_SPEED_LOCK_MS		16.67f

//
//
// SINGLETONS
//
//
//
// Define a class as a singleton (Add to class definition in header file)
//
#define CDEFINE_SINGLETON(the_class)				\
private:										\
	static the_class* _instance;				\
	the_class() {}								\
	~the_class() {}								\
	the_class(const the_class &);				\
	the_class& operator=(const the_class &);	\
public:											\
	static void Create();						\
	static void Destroy();						\
	static the_class* getInstance();

//
// Declare singleton methods (Add to source file)
//
#define CDECLARE_SINGLETON(the_class)		\
the_class* the_class::_instance = NULL;		\
void the_class::Create()					\
{											\
	if (_instance == NULL)					\
		_instance = new the_class;			\
}											\
void the_class::Destroy()					\
{											\
	if (_instance != NULL)					\
	{										\
		delete _instance;					\
		_instance = NULL;					\
	}										\
}											\
the_class* the_class::getInstance()			\
{											\
  return _instance;							\
}


//
//
//
//
// CIwGameError - Game error logging class
//
//
//
//
class CIwGameError
{
public:
	static void LogError(const char* message);
	static void LogError(const char* message, const char* data);
};

//
//
//
//
// CIwGameCallback - Game callback type
//
//
//
//
typedef int32 (*CIwGameCallback)(void* caller, void* data);
typedef void* CIwGameCallbackData;

//
//
//
//
// CIwGameTimer (passive - needs to be polled with HasTimedOut)
//
//
//
//
class CIwGameTimer
{
protected:
	bool		Started;			// true if timer started
	uint64 		LastTime;			// Time that the timer was set (milliseconds)

	// Properties
private:
	uint64		Duration;			// Duration of timer in milliseconds
	bool		AutoReset;			// Auto reset the timer when it times out
public:
	void		setDuration(uint64 millseconds_duration, bool start = true)
	{
		Duration = millseconds_duration;
		if (start)
			Start();
	}
	uint64		getDuration() const				{ return Duration; }
	void		setAutoReset(bool auto_reset)	{ AutoReset = auto_reset; }
	bool		getAutoReset() const			{ return AutoReset; }
	bool		hasStarted() const				{ return Started; }
	bool		hasStopped() const				{ return !Started; }
	// Properties end

public:
	CIwGameTimer() : LastTime(0), Duration(0), AutoReset(false), Started(false) { }
	CIwGameTimer(int millseconds_duration)
	{
		LastTime = 0;
		Duration = 0;
		AutoReset = false;
		Started = false;
		setDuration(millseconds_duration);
	}
	~CIwGameTimer() {	}
	
	virtual bool HasTimedOut()
	{
		if (!Started)
			return false;

		if (Duration == 0)	// Timer of 0 duration never times out
		{
			return false;
		}
		
		uint64 time_diff = GetElapsedTime();
	
		if (time_diff > Duration)
		{
			if (AutoReset)
			{
				LastTime = s3eTimerGetMs();
			}
			else
				Started = false;
			return true;
		}
	
		return false;
	}
	uint64		GetElapsedTime() const
	{
		return s3eTimerGetMs() - LastTime;
	}
	uint64		GetTimeDiff(uint64 this_time) const { return this_time - LastTime; }
	uint64		GetTimeDiff() const					{ return s3eTimerGetMs() - LastTime; }
	uint64		GetTimeLeft() const					{ return Duration - (s3eTimerGetMs() - LastTime); }
	void		Reset()								{ setDuration(Duration); }
	void		Start()
	{
		LastTime = s3eTimerGetMs();
		Started = true;
	}
	void		Stop()								{ Started = false; }

	static uint64	GetCurrentTimeMs()				{ return s3eTimerGetMs(); }

};

//
//
//
// CIwGameUtils - Pure static utility class
//
//
//
class CIwGameUtils
{
	enum eDeviceType
	{
		DeviceType_Unsupported	= -1,
		DeviceType_iPhone		= 0,
		DeviceType_iPad			= 1,
		DeviceType_Android		= 2,
		DeviceType_Bada			= 3,
		DeviceType_QNX			= 4,
		DeviceType_Symbian		= 5,
		DeviceType_WinMobile	= 6,
		DeviceType_WebOS		= 7,
		DeviceType_Windows		= 8,
		DeviceType_OSX			= 9,
	};

public:
	static const char*	GetGraphicModeName(int width, int height);
	static int			GetGraphicModeIndex(int width, int height);
	static int			CountDigits(int number);
	static eDeviceType	GetDeviceType();
};

//
//
//
// CIwGameArray -	Basic resizable slot array - A slot array differs from a normal array in that the array is not shrank and elements are not compacted when elements are removed. Instead 
//					array elements that are removed are marked with NULL. With this, the Length of the array denotes the total number of items added to the array and not the number of valid 
//					elements. To get the number of valid elements call count()
//
//
//
template <class T>
class CIwGameSlotArray
{
	// Properties
protected:
	int		Growth;			// Number of elements to grow array when we run out of space
public:
	void	setGrowth(int growth)	{ Growth = growth; }
	int		getSize() const			{ return Size; }
	// Properties End
protected:
	T*		Elements;		// Array elements
	int		Size;			// Max size of array
	int		LastFreedSlot;	// Slot that was last freed

public:
	CIwGameSlotArray()
	{
		LastFreedSlot = 0;
		Growth = 4;
		Size = 4;
		Elements = new T[Size]();
		for (int t = 0; t < Size; t++)
			Elements[t] = NULL;
	}
	CIwGameSlotArray(int size, int growth)
	{
		LastFreedSlot = 0;
		Growth = growth;
		Size = size;
		Elements = new T[Size]();
		for (int t = 0; t < Size; t++)
			Elements[t] = NULL;
	}
	
	~CIwGameSlotArray()
	{
		SAFE_DELETE(Elements)
	}

	int count() const
	{
		int count = 0;
		for (int t = 0; t < Size; t++)
		{
			if (Elements[t] != NULL)
				count++;
		}
		return count;
	}
	bool add(T element)
	{
		if (LastFreedSlot >= 0)
		{
			Elements[LastFreedSlot] = element;
			if (LastFreedSlot < Size - 1 && Elements[LastFreedSlot + 1] == NULL)	// Optimisation - If next slot is free then set LastFreedSlot as the next slot
				LastFreedSlot++;
			else
				LastFreedSlot = -1;
			return true;
		}
		// find a free slot
		int t;
		for (t = 0; t < Size; t++)
		{
			if (Elements[t] == NULL)
			{
				Elements[t] = element;
				if (t < Size - 1 && Elements[t + 1] == NULL)	// Optimisation - If next slot is free then set LastFreedSlot as the next slot
					LastFreedSlot = t + 1;
				return true;
			}
		}

		// If we are not allowed to grow then return error
		if (Growth == 0)
			return false;

		// No free slots so grow the array
		grow(Growth);
		Elements[t] = element;
		LastFreedSlot = t + 1;

		return true;
	}

	void remove(T element)
	{
		for (int t = 0; t < Size; t++)
		{
			if (Elements[t] == element)
			{
				Elements[t] = NULL;
				LastFreedSlot = t;
				break;
			}
		}
	}

	void remove(int index)
	{
		if (index < 0 || index >= Size)
			return;
		Elements[index] = NULL;
		LastFreedSlot = index;
	}

	T element_at(int index) const
	{
		if (index < 0 || index >= Size)
			return NULL;
		return Elements[index];
	}

	bool set(int index, T element)
	{
		if (index < 0 || index >= Size)
			return false;
		Elements[index] = element;
		if (index == LastFreedSlot)
			LastFreedSlot = -1;

		return true;
	}

	void grow(int growth)
	{
		if (growth <= 0)
			return;
		T* elements = new T[Size + growth]();
		for (int t = 0; t < Size; t++)
			elements[t] = Elements[t];
		for (int t = 0; t < growth; t++)
			elements[t + Size] = NULL;
		delete [] Elements;
		Elements = elements;
		Size += growth;
	}

	void growToSize(int new_size)
	{
		if (new_size <= 0 || new_size <= Size)
			return;
		grow(new_size - Size);
	}

	void resize(int size)
	{
		if (size <= 0 || size == Size)
			return;

		if (size > Size)
			grow(size - Size);
		else
		{
			T* elements = new T[size]();
			for (int t = 0; t < size; t++)
				elements[t] = Elements[t];

			delete Elements;
			Elements = elements;
			Size = size;
			if (LastFreedSlot >= size)
				LastFreedSlot = -1;
		}
	}

	int getLastUsedSlot() const
	{
		for (int t = Size - 1; t >= 0; t--)
		{
			if (Elements[t] != NULL)
				return t;
		}

		return Size - 1;
	}

	void clear(bool delete_elements = false)
	{
		for (int t = 0; t < Size; t++)
		{
			if (delete_elements)
			{
				SAFE_DELETE(Elements[t])
			}
			Elements[t] = NULL;
		}
		LastFreedSlot = 0;
	}
};


#endif	// _CIW_GAME_UTIL_H_
