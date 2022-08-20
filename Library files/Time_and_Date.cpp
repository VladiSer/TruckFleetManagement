#include "Time_and_Date.h"

// ------------------------ Date Methods ------------------------

//checks if the date is valid (consieders leap years)
bool Date::valid_date() const
{
	if (m_year <= 0 || m_month < 1 || m_month > 12 || m_day < 1 || m_day > 31)
		return false;

	bool leap = leap_year();
	char days[12] = { 31,28 + leap,31,30,31,30,31,31,30,31,30,31 };
	if (m_day > days[int(m_month) - 1])
		return false;
	
	return true;
}

bool Date::leap_year() const
{
	return !(m_year % 4) && ((m_year % 100) || !(m_year % 400));
}

//default constructor always creates the date of today
Date::Date()
{
	time_t t = time(NULL);
	struct tm today;
	localtime_s(&today, &t);
	m_day = today.tm_mday;
	m_month = today.tm_mon + 1;
	m_year = today.tm_year + 1900;
}

Date::Date(char day, char month, int year)
	: m_day(day), m_month(month), m_year(year)
{
	if (!(valid_date()))
		throw *this;
}

//creates a date from string 
Date::Date(const string& date_str)
{
	try
	{
		m_day = stoi(date_str.substr(0, 2));
		m_month = stoi(date_str.substr(3, 2));
		m_year = stoi(date_str.substr(6, 4));
	}
	catch (...)
	{
		throw string(DATE_STRUCTURE_ERR);
	}
	if (!valid_date())
		throw *this;
}

const Date& Date::operator=(const Date& other)
{
	m_day = other.m_day;
	m_month = other.m_month;
	m_year = other.m_year;
	return *this;
}

void Date::inc_years(int n)
{
	m_year += n;
	//if its not a leap year 29.2 is actually 1.3
	if (m_month == 2 && m_day == 29 && !leap_year())
	{
		m_day = 1;
		m_month = 3;
	}
	if (m_year <= 0)
		throw *this;
}

bool Date::expires_in_n_year(int n) const
{
	Date comp_date(*this),today;
	comp_date.inc_years(n);
	return comp_date < today;	
}

bool Date::operator<(const Date& comp_date) const
{
	if (m_year < comp_date.m_year)
		return true;
	else if (m_year == comp_date.m_year)
		if (m_month < comp_date.m_month)
			return true;
		else if (m_month == comp_date.m_month)
			if (m_day < comp_date.m_day)
				return true;
	return false;
}

void Date::show(ostream& os) const
{
	if (m_day < 10)
		os << 0;
	os << int(m_day) << '/';
	if (m_month < 10)
		os << 0;
	os << int(m_month) << '/' << m_year;
}

ostream& operator<<(ostream& os, const Date& date)
{
	date.show(os);
	return os;
}


// ------------------------ Time Methods ------------------------

//default constructor always creates the time of today and hour and minute of creation
Time::Time()
{
	time_t t = time(NULL);
	struct tm today;
	localtime_s(&today, &t);
	m_minute = today.tm_min;
	m_hour = today.tm_hour;
}

//creates a time from string 
Time::Time(const string& time_str)
	:Date(time_str.substr(0, 10))
{
	try
	{
		m_hour = stoi(time_str.substr(11, 2));
		m_minute = stoi(time_str.substr(14, 2));
	}
	catch (const Date&)
	{
		throw *this;
	}
	catch (...)
	{
		throw string(DATE_STRUCTURE_ERR);
	}
	if (m_hour < 0 || m_hour > 23 || m_minute < 0 || m_minute > 59)
		throw *this;
}


ostream& operator<<(ostream& os, const Time& time)
{
	time.show(os);
	os << ' ';
	if (time.m_hour < 10)
		os << 0;
	os << int(time.m_hour) << ':';
	if (time.m_minute < 10)
		os << 0;
	os << int(time.m_minute);
	return os;
}