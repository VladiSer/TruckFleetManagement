#ifndef DATE_H
#define DATE_H

#include <time.h>
#include <iostream>
#include <string>
using namespace std;

#define DATE_STRUCTURE_ERR "Invalid Date structure"

class Date
{
protected:
	char m_day;
	char m_month; 
	int m_year;

	bool valid_date() const;
	bool leap_year() const;
	void show(ostream& os) const;
public:
	Date();
	Date(char day, char month, int year);
	Date(const string& date_str);
	const Date& operator=(const Date& other);
	void inc_years(int n); //increments the date by n years
	bool expires_in_n_year(int n) const;//returns true if the date is smaller than today in n years
	bool operator<(const Date& comp_date) const;

	friend ostream& operator<<(ostream& os, const Date& date);
};

class Time : public Date
{
private:
	char m_minute;
	char m_hour;
public:
	Time();
	Time(const string& time_str);

	friend ostream& operator<<(ostream& os, const Time& time);
};

#endif // !DATE_H
