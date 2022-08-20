#ifndef DRIVER_H
#define DRIVER_H

#include "Time_and_Date.h"
#include <string>
#include <map>
#include <set>

#define MAX_WEIGHT_C1 12000 

#define TYPE_C "C" 
#define TYPE_C1 "C1"
#define TYPE_CplusE "C+E"

#define YES "Yes"
#define NO "No"

#define MIN_ID 10000000
#define MAX_ID 999999999

#define MIN_LICENSE 1000000
#define MAX_LICENSE	9999999

#define DRIVER_PARAMETERS 12

#define DRIVER_LIST_HEADER  "ID,Date Of Birth,Name,Surname,Start Of Work Date,Delivery Job Number,License Number,License Type,Date Of Receipt Of License,Date Of Receipt Of Latest Permit,Date Of Expiration,Need Glasses"

#define DRIVER_NAME_ERR " is not a valid name"
#define LICENSE_TYPE_ERR "Invalid License Type"
#define DRIVER_TAKEN_ERR_MSG "Driver is already on a Delivery Job"
#define DRIVER_DATES_ERR "Invalid Driver dates"
#define DRIVER_NEED_GLASSES_ERR "The string does not match to \"Yes\" or \"No\""
#define DRIVER_FROM_STRING_ERR "Invalid driver data in csv format string"

class DeliveryJob;

enum class LicenseType { C1, C, CplusE };
ostream& operator<<(ostream& os, const LicenseType& license_type);

class Driver
{
private:
	unsigned int m_assigned_delivery_job_num; // 0 if not assigned

	//general info
	int m_ID;
	Date m_date_of_birth; 
	string m_name;
	string m_surname;
	Date m_start_of_work_date;

	//license info
	int m_license_number;
	LicenseType m_license_type;
	Date m_date_of_receipt_of_license;
	Date m_date_of_receipt_of_latest_permit;
	Date m_date_of_expiration;
	bool m_need_glasses;

	bool valid_dates() const;
	bool valid_name(const string& name) const;
	
public:
	Driver(int ID, const Date& birth, const string& name, const string& surname, const Date& start_of_work, int license_number, LicenseType license_type, const Date& receipt_of_license, const Date& receipt_of_latest_permit, const Date& expiration, bool need_glasses);
	void assign_job(unsigned int assigned_job_num);
	void remove_job();
	bool suitable_for_driving() const; //checks if the license is not expired
	bool is_assigned() const;
	static Driver* create_Driver_from_string(const string& driver_str);
	static LicenseType str_to_LicenseType(const string& str);
	static bool str_to_need_glasses(const string& str);

	void set_name(const string& name);
	void set_surname(const string& surname);
	void set_license_type(LicenseType license_type);
	void set_date_of_receipt_of_latest_permit(const Date& receipt_of_latest_permit);
	void set_date_of_expiration(const Date& expiration);
	void set_need_glasses(bool need_glasses);

	int get_id_num() const;
	int get_license_num() const;
	const string& get_name() const;
	const string& get_surname() const;
	unsigned int get_assigned_job_number() const;
	const Date& get_date_of_expiration() const;
	LicenseType get_license_type() const;

	friend ofstream& operator<<(ofstream& ofs, const Driver& driver);
	friend ostream& operator<<(ostream& os, const Driver& driver);
};

class DriverList
{
private:
	map<int, Driver*> m_drivers_id;
	map<int,Driver*> m_drivers_license;

	bool suitable_driver(const DeliveryJob* delivery_job, int driver_id) const;
	void delete_drivers();
public:
	DriverList();
	DriverList(const string& driver_list_file);
	~DriverList();
	void add_new_driver(Driver* driver);
	const Driver* get_driver_by_id(int ID) const;
	const Driver* get_driver_by_license(int license) const;
	void remove_driver_by_id(int ID);
	void remove_driver_by_license(int license);
	void assign_job(int id, unsigned int job_num);
	void remove_job(int id);
	
	void edit_driver_name(int id, const string& name);
	void edit_driver_surname(int id, const string& surname);
	void edit_driver_license_type(int id, const string& license_type, const string& latest_permit_receipt);
	void edit_driver_expiration_date(int id, const string& expiration_date);
	void edit_driver_glasses_limitation(int id, bool need_glasses);

	void show_reduced_list() const;
	void show_by_type(LicenseType license_type) const;
	void show_by_status(bool assigned) const;
	void show_by_expiration_date() const;
	bool show_suitable(const DeliveryJob* delivery_job) const;

	friend ofstream& operator<<(ofstream& ofs, const DriverList& driver_list);
	friend ostream& operator<<(ostream& os, const DriverList& driver_list);
};

#endif // !DRIVER_H

