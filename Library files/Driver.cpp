#include "Driver.h"
#include "Truck.h"
#include "Delivery.h"



// ------------------------ Auxillary Functions ------------------------

ostream& operator<<(ostream& os, const LicenseType& license_type)
{
	switch (license_type)
	{
	case LicenseType::C:
		return os << TYPE_C;
	case LicenseType::C1:
		return os << TYPE_C1;
	case LicenseType::CplusE:
		return os << TYPE_CplusE;
	default:
		return os;
	}
}


// ------------------------ Driver Methods ------------------------


Driver::Driver(int ID, const Date& birth, const string& name, const string& surname, const Date& start_of_work, int license_number, LicenseType license_type, const Date& receipt_of_license, const Date& receipt_of_latest_permit, const Date& expiration, bool need_glasses)
	: m_assigned_delivery_job_num(0), m_ID(ID), m_date_of_birth(birth), m_name(name), m_surname(surname), m_start_of_work_date(start_of_work),
	m_license_number(license_number), m_license_type(license_type), m_date_of_receipt_of_license(receipt_of_license), m_date_of_receipt_of_latest_permit(receipt_of_latest_permit),
	m_date_of_expiration(expiration), m_need_glasses(need_glasses)
{
	if (!valid_dates())
		throw string(DRIVER_DATES_ERR);

	if (!valid_name(m_name + m_surname))
		throw string(m_name + ' ' + m_surname + DRIVER_NAME_ERR);

	if (m_ID < MIN_ID || m_ID > MAX_ID)
		throw m_ID;

	if (m_license_number < MIN_LICENSE || m_license_number > MAX_LICENSE)
		throw m_license_number;

}

bool Driver::valid_dates() const
{
	Date date_18yo(m_date_of_birth), today;
	date_18yo.inc_years(18);
	//driver is over 18 years old
	if (today < date_18yo)
		return false;
	//driver started working after 18th birthday
	if (m_start_of_work_date < date_18yo)
		return false;
	//driver recieved license before today
	if (today < m_date_of_receipt_of_license)
		return false;
	//driver recieved his latest permit before today, but after the basic license
	if (today < m_date_of_receipt_of_latest_permit || m_date_of_receipt_of_latest_permit < m_date_of_receipt_of_license)
		return false;
	//expiration date is not lower than than the recipt of latest permit
	if (m_date_of_expiration < m_date_of_receipt_of_latest_permit)
		return false;

	return true;
}

//checks if the name contains only letters
bool Driver::valid_name(const string& name) const
{
	for (unsigned int i = 0;i < name.size();i++)
	{
		char c = name[i];
		if (!(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'))
			return false;
	}
	return true;
}

LicenseType Driver::str_to_LicenseType(const string& str)
{
	string license[3] = { TYPE_C1,TYPE_C,TYPE_CplusE };
	LicenseType license_type[3] = { LicenseType::C1,LicenseType::C,LicenseType::CplusE };
	for (int i = 0;i < 3;i++)
		if (str == license[i])
			return license_type[i];

	throw string(LICENSE_TYPE_ERR);
}

bool Driver::str_to_need_glasses(const string& str)
{
	if (str == YES)
		return true;
	else if (str == NO)
		return false;
	else
		throw string(DRIVER_NEED_GLASSES_ERR);
}

//cannot assign job if driver is already assigned
void Driver::assign_job(unsigned int assigned_job_num)
{
	if (m_assigned_delivery_job_num != 0)
		throw string(DRIVER_TAKEN_ERR_MSG);
	m_assigned_delivery_job_num = assigned_job_num;
}

void Driver::remove_job()
{
	m_assigned_delivery_job_num = 0;
}

bool Driver::suitable_for_driving() const
{
	Date today;
	//not suitable for driving if the license is expired
	if (m_date_of_expiration < today)
		return false;
	return true;
}

void Driver::set_name(const string& name)
{
	if (!valid_name(name))
		throw string(name + DRIVER_NAME_ERR);

	m_name = name;
}

void Driver::set_surname(const string& surname)
{
	if (!valid_name(surname))
		throw string(surname + DRIVER_NAME_ERR);

	m_surname = surname;
}

void Driver::set_license_type(LicenseType license_type)
{
	m_license_type = license_type;
}

void Driver::set_date_of_receipt_of_latest_permit(const Date& receipt_of_latest_permit)
{
	Date today;
	//checks if the receipt of latest permit didnt happen "in the future" and its higher than the current one
	if (today < receipt_of_latest_permit || receipt_of_latest_permit < m_date_of_receipt_of_latest_permit)
		throw string(DRIVER_DATES_ERR);

	m_date_of_receipt_of_latest_permit = receipt_of_latest_permit;
}

void Driver::set_date_of_expiration(const Date& expiration)
{
	Date today;
	//checks if the expiration date is higher than today
	if (expiration < today)
		throw string(DRIVER_DATES_ERR);

	m_date_of_expiration = expiration;
}

void Driver::set_need_glasses(bool need_glasses)
{
	m_need_glasses = need_glasses;
}

int Driver::get_id_num() const
{
	return m_ID;
}

int Driver::get_license_num() const
{
	return m_license_number;
}

const string& Driver::get_name() const
{
	return m_name;
}

const string& Driver::get_surname() const
{
	return m_surname;
}

unsigned int Driver::get_assigned_job_number() const
{
	return m_assigned_delivery_job_num;
}

const Date& Driver::get_date_of_expiration() const
{
	return m_date_of_expiration;
}


LicenseType Driver::get_license_type() const
{
	return m_license_type;
}

bool Driver::is_assigned() const
{
	return (m_assigned_delivery_job_num > 0);
}

//creates a driver from a csv format string
Driver* Driver::create_Driver_from_string(const string& driver_str)
{
	stringstream driver_line(driver_str);
	string driver_data[DRIVER_PARAMETERS];
	Driver* driver = nullptr;

	for (int i = 0;i < DRIVER_PARAMETERS;i++)
		getline(driver_line, driver_data[i], ',');

	try
	{
		int ID = stoi(driver_data[0]), license_number = stoi(driver_data[6]);
		Date date_of_birth(driver_data[1]), start_of_work_date(driver_data[4]);
		Date date_of_receipt_of_licenese(driver_data[8]), date_of_receipt_of_latest_permit(driver_data[9]), date_of_expiration(driver_data[10]);
		LicenseType license_type = Driver::str_to_LicenseType(driver_data[7]);
		bool need_glasses = Driver::str_to_need_glasses(driver_data[11]);
		driver = new Driver(ID, date_of_birth, driver_data[2], driver_data[3], start_of_work_date, license_number, license_type, date_of_receipt_of_licenese, date_of_receipt_of_latest_permit, date_of_expiration, need_glasses);
		driver->m_assigned_delivery_job_num = stoi(driver_data[5]);
	}
	catch(...)
	{
		if (driver)
			delete driver;
		driver = nullptr;
		throw string(DRIVER_FROM_STRING_ERR);
	}
	return driver;
}

//writes data to the file in a csv format
ofstream& operator<<(ofstream& ofs, const Driver& driver)
{
	ofs << driver.m_ID << ','<<driver.m_date_of_birth<<','<<driver.m_name<<','<<driver.m_surname<<',';
	ofs << driver.m_start_of_work_date << ',' << driver.m_assigned_delivery_job_num << ',';
	ofs << driver.m_license_number << ',' << driver.m_license_type << ',' << driver.m_date_of_receipt_of_license << ',';
	ofs << driver.m_date_of_receipt_of_latest_permit << ',' << driver.m_date_of_expiration << ',';
	ofs << ((driver.m_need_glasses) ? YES : NO);
	return ofs;	
}

ostream& operator<<(ostream& os, const Driver& driver)
{
	os << "ID: " << driver.m_ID << endl << "Full Name: " << driver.get_name() << ' ' << driver.get_surname() << endl;
	os << "Date of Birth: " << driver.m_date_of_birth << endl << "Start of work date: " << driver.m_start_of_work_date << endl;
	if (driver.m_assigned_delivery_job_num)
		os << "Assigned Delivery Job Number: " << driver.m_assigned_delivery_job_num << endl;
	os << "License Number: " << driver.m_license_number << endl << "License Type: " << driver.m_license_type << endl;
	os << "Need Glasses: " << (driver.m_need_glasses ? YES : NO) << endl;
	os << "Expiration Date: " << driver.m_date_of_expiration << endl << "Date of Receipt: " << driver.m_date_of_receipt_of_license << endl;
	os << "Date of Receipt of latest Permit: " << driver.m_date_of_receipt_of_latest_permit << endl;

	return os;
}


// ------------------------ DriverList Methods ------------------------

DriverList::DriverList() { }

DriverList::DriverList(const string& driver_list_file)
{
	ifstream driver_list(driver_list_file);
	if (!driver_list.is_open())
		throw driver_list_file;
	//ignores the 1st line since its only the header
	driver_list.ignore(numeric_limits<streamsize>::max(), '\n');
	driver_list.clear();
	int ID, license;
	Driver* driver;
	string driver_line;
	while (!driver_list.eof())
	{
		//takes data from the csv format file, line by line and creates driver from each line
		driver = nullptr;
		getline(driver_list, driver_line);
		if (driver_line.empty())
			break;
		try
		{
			driver = Driver::create_Driver_from_string(driver_line);
			ID = driver->get_id_num();
			license = driver->get_license_num();

			//checks that there have been no duplications in the file
			if (m_drivers_id.count(ID) || m_drivers_license.count(license))
				throw driver;
		}
		catch (...)
		{
			driver_list.close();
			if (driver)
				delete driver;
			delete_drivers();
			throw driver_list_file;
		}
		m_drivers_id[ID] = driver;
		m_drivers_license[license] = driver;
	}
	driver_list.close();
}

void DriverList::delete_drivers()
{
	map<int, Driver*>::iterator iter = m_drivers_id.begin(), end_iter = m_drivers_id.end();
	for (;iter != end_iter;iter++)
		delete iter->second;
	m_drivers_id.clear();
	m_drivers_license.clear();
}

DriverList::~DriverList()
{
	delete_drivers();
}

void DriverList::add_new_driver(Driver* driver)
{
	//the driver is added to the list if his ID and License are not duplicates
	int id = driver->get_id_num(), license = driver->get_license_num();
	if (m_drivers_id.count(id) || m_drivers_license.count(license))
		throw driver;
	m_drivers_id[id] = driver;
	m_drivers_license[license] = driver;
}

const Driver* DriverList::get_driver_by_id(int ID) const
{
	//if the driver exists return the driver, otherwise throws the ID
	if (!(m_drivers_id.count(ID)))
		throw ID;
	else
		return m_drivers_id.at(ID);
}

const Driver* DriverList::get_driver_by_license(int license) const
{
	//if the driver exists return the driver, otherwise throws the License
	if (!(m_drivers_license.count(license)))
		throw license;
	else
		return m_drivers_license.at(license);
}

void DriverList::remove_driver_by_id(int ID)
{
	//if the driver exists removes and deletes the driver, otherwise throws the ID
	if (!(m_drivers_id.count(ID)))
		throw ID;
	Driver* driver = m_drivers_id.at(ID);
	if (driver->get_assigned_job_number() > 0)
		throw driver;

	m_drivers_id.erase(ID);
	m_drivers_license.erase(driver->get_license_num());
	delete driver;
}

void DriverList::remove_driver_by_license(int license)
{
	//if the driver exists removes and deletes the driver, otherwise throws the License
	if (!(m_drivers_license.count(license)))
		throw license;
	Driver* driver = m_drivers_license.at(license);
	if (driver->get_assigned_job_number() > 0)
		throw driver;

	m_drivers_license.erase(license);
	m_drivers_id.erase(driver->get_id_num());
	delete driver;
}

// saves the DriverList in a csv format
ofstream& operator<<(ofstream& ofs, const DriverList& driver_list)
{
	ofs << DRIVER_LIST_HEADER << endl;
	map<int, Driver*>::const_iterator iter = driver_list.m_drivers_id.begin(), end_iter = driver_list.m_drivers_id.end();
	for (;iter != end_iter;iter++)
		ofs << *(iter->second) << endl;
	return ofs;
}

void DriverList::assign_job(int id, unsigned int job_num)
{
	//if the driver exists assigns job to the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	//inside assign job also checks if he is not already assigned
	m_drivers_id.at(id)->assign_job(job_num);
}

void DriverList::remove_job(int id)
{
	//if the driver exists removes job from the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	m_drivers_id.at(id)->remove_job();
}

void DriverList::edit_driver_name(int id, const string& name)
{
	//if the driver exists edits the name of the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	//inside set name also checks if the name is valid
	m_drivers_id.at(id)->set_name(name);
}

void DriverList::edit_driver_surname(int id, const string& surname)
{
	//if the driver exists edits the surname of the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	//inside set name also checks if the surname is valid
	m_drivers_id.at(id)->set_surname(surname);
}

void DriverList::edit_driver_license_type(int id, const string& license_type, const string& latest_permit_receipt)
{
	//if the driver exists edits the license type and the latest permit receipt date of the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	m_drivers_id.at(id)->set_license_type(Driver::str_to_LicenseType(license_type));
	//inside set date of of receipt of latest permit checks if the date of receipt is valid
	//Date constructor checks if the string format is valid and if the date itself is valid
	m_drivers_id.at(id)->set_date_of_receipt_of_latest_permit(Date(latest_permit_receipt));
}

void DriverList::edit_driver_expiration_date(int id, const string& expiration_date)
{
	//if the driver exists edits the license expiration date of the driver, otherwise throws the ID
	if (!m_drivers_id.count(id))
		throw id;
	//inside set date of expiration checks if the date of expiration is valid
	//Date constructor checks if the string format is valid and if the date itself is valid
	m_drivers_id.at(id)->set_date_of_expiration(Date(expiration_date));
}

void DriverList::edit_driver_glasses_limitation(int id, bool need_glasses)
{
	//if the driver exists edits the glasses limitation of the driver, otherwise throws ID
	if (!m_drivers_id.count(id))
		throw id;
	m_drivers_id.at(id)->set_need_glasses(need_glasses);
}

//shows on screen only the drivers ID, Full name, License and Assigned Job Number if he is assigned to a job
void DriverList::show_reduced_list() const
{
	cout << "-------------------- Driver List --------------------" << endl << endl;
	map<int, Driver*>::const_iterator iter = m_drivers_id.begin(), end_iter = m_drivers_id.end();
	for (;iter != end_iter;iter++)
	{
		unsigned int assigned_job_number = iter->second->get_assigned_job_number();
		string full_name = iter->second->get_name() + ' ' + iter->second->get_surname();
		cout << "Driver's ID: " << iter->first << "\tName: " << full_name;
		if (full_name.length() <= 9)
			cout << '\t';
		cout << "\tLicense Type: " << iter->second->get_license_type() << "  \tAssigned Job Number: ";
		if (assigned_job_number > 0)
			cout << assigned_job_number;
		else
			cout << "Not Assigned";
		cout << endl << endl;
	}
}

//shows the whole driver list with all the parameters
ostream& operator<<(ostream& os, const DriverList& driver_list)
{
	os << "-------------------- Driver List --------------------" << endl << endl;
	map<int, Driver*>::const_iterator iter = driver_list.m_drivers_id.begin(), end_iter = driver_list.m_drivers_id.end();
	for (;iter != end_iter;iter++)
		os << *(iter->second) << endl << endl;
	return os;
}

//shows only the drivers with specific license type, given by the argument
void DriverList::show_by_type(LicenseType license_type) const
{
	cout << "-------------------- Drivers with "<<license_type<<" license type --------------------" << endl << endl;
	map<int, Driver*>::const_iterator iter = m_drivers_id.begin(), end_iter = m_drivers_id.end();
	
	for (;iter != end_iter;iter++)
		if(iter->second->get_license_type()==license_type)
			cout << *(iter->second) << endl << endl;
}

//shows only the drivers that are assigned or not assigned to a job, determined by the argument
void DriverList::show_by_status(bool assigned) const
{
	map<int, Driver*>::const_iterator iter =  m_drivers_id.begin();
	map<int, Driver*>::const_iterator end_iter = m_drivers_id.end();

	if (assigned)
	{
		cout << "-------------------- Assigned Drivers List --------------------" << endl << endl;

		for (;iter != end_iter;iter++)
			if (iter->second->is_assigned())
				cout << *(iter->second) << endl << endl;
	}
	else
	{
		cout << "-------------------- Unassigned Drivers List --------------------" << endl << endl;

		for (;iter != end_iter;iter++)
			if (!iter->second->is_assigned())
				cout << *(iter->second) << endl << endl;
	}
}

//shows the list of drivers sorted by expiration date
void DriverList::show_by_expiration_date() const
{
	//implemented by copying the current map into a multimap with a expiration date key and being shown afterwards

	//copying the map into a multimap
	multimap<Date, Driver*> drivers_list_copy;
	map<int, Driver*>::const_iterator iter = m_drivers_id.begin(), end_iter = m_drivers_id.end();
	for (;iter != end_iter;iter++)
	{
		Date expirtion_date = iter->second->get_date_of_expiration();
		drivers_list_copy.insert(pair<Date, Driver*>(expirtion_date, iter->second));
	}

	//printing the multimap by key
	cout << "-------------------- Drivers List By License Expiration Date --------------------" << endl << endl;
	multimap<Date, Driver*>::const_iterator iter_copy = drivers_list_copy.begin(), end_iter_copy = drivers_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;
}

//checks if the driver is suitable for the given job
bool DriverList::suitable_driver(const DeliveryJob* delivery_job, int driver_id) const
{
	Cargo cargo_type = delivery_job->get_cargo_type();
	int cargo_weight = delivery_job->get_cargo_weight();
	Driver* driver = m_drivers_id.at(driver_id);
	//checks if the driver doesnt have an assignd job and his license is not expired
	if (driver->is_assigned() || !driver->suitable_for_driving())
		return false;
	LicenseType license_type = driver->get_license_type();
	//checks if the drivers license type is enough to drive a specific truck for a specifc delivery
	//and if it is inside the weight limitation of his license type
	if (cargo_type == Cargo::liquid)
		if (license_type != LicenseType::CplusE)
			return false;
	else
		if (license_type == LicenseType::C1 && cargo_weight > MAX_WEIGHT_C1)
			return false;
	
	return true;
}

//prints all the drivers from the list suitable for the given delivery job
//returns true if there was at least 1 suitable driver, otherwise returns false
bool DriverList::show_suitable(const DeliveryJob* delivery_job) const
{
	cout << "-------------------- Suitable Drivers List --------------------" << endl << endl;
	int counter = 0;
	map<int, Driver*>::const_iterator iter = m_drivers_id.begin(), end_iter = m_drivers_id.end();
	for (;iter != end_iter;iter++)
	{
		if (!suitable_driver(delivery_job, iter->first))
			continue;
		string full_name = iter->second->get_name() + ' ' + iter->second->get_surname();
		cout << "Driver's ID: " << iter->first << "\tName: " << full_name;
		if (full_name.length() <= 9)
			cout << '\t';
		cout << "\tLicense Type: " << iter->second->get_license_type();
		cout << endl << endl;
		counter++;
	}
	if (counter == 0)
		cout << endl << "No available suitable drivers were found" << endl;
	return counter != 0;
}