#include "Delivery.h"
#include "Truck.h"
#include "Driver.h"

// ------------------------ Auxillary Functions ------------------------

ostream& operator<<(ostream& os, const Cargo& cargo_type)
{
	switch (cargo_type)
	{
	case Cargo::regular:
		return os << REGULAR_DELIVERY;
	case Cargo::refrigerated_goods:
		return os << REFRIGERATED_DELIVERY;
	case Cargo::liquid:
		return os << LIQUID_DELIVERY;
	}
	return os;
}

//------------------------ DeliverJob Methods ------------------------

bool DeliveryJob::suitable_truck_and_driver(const Truck* truck, const Driver* driver) const
{
	//checks if the truck and driver are not assigned
	if (truck->get_assigned_job_number() != 0 || driver->get_assigned_job_number() != 0)
		return false;
	//checks if the truck tests and driver license are not expired
	if (!(truck->valid_tests()) || !(driver->suitable_for_driving()))
		return false;
	TruckType truck_type = truck->get_truck_type();
	//if the truck type is not Truck or SemiTrailer they cannot take the regular delivery job
	if ((truck_type != TruckType::Truck) && (truck_type != TruckType::SemiTrailer))
		return false;
	//checks if the cargo weight is smaller than the max load weight of the truck
	if (m_cargo_weight > truck->get_max_load_weight())
		return false;

	int total_weight = m_cargo_weight + truck->get_empty_vehicle_weight();
	LicenseType license_type = driver->get_license_type();
	
	//if the driver type is CplusE he can drive any truck
	//if its C1 or C they cannot drive SemiTrailer
	//also checks if the truck weight + the cargo weight dont exceed the C1 max weight limitation
	if (license_type != LicenseType::CplusE)
	{
		if (truck_type == TruckType::SemiTrailer)
			return false;
		if (license_type == LicenseType::C1 && total_weight > MAX_WEIGHT_C1)
			return false;
	}
	return true;
}

DeliveryJob::DeliveryJob(int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance)
	: m_cargo_type(Cargo::regular), m_cargo_weight(cargo_weight), m_due_date(due_date),
	m_start_point(start_point), m_end_point(end_point), m_distance(distance),
	m_assigned_truck_license_plate(0), m_assigned_driver_id(0), m_ongoing(false), m_job_number(0)
{
	Date today;
	//cant create a job that should have been finished in the past
	if (m_due_date < today)
		throw string(DUE_DATE_ERR);

	if (m_cargo_weight <= 0)
		throw string("Cargo Weight ") + NEGATIVE_NUM_ERROR_MSG;

	if (m_distance <= 0)
		throw string("Distance ") + NEGATIVE_NUM_ERROR_MSG;
}

int DeliveryJob::get_assigned_truck_license_plate() const
{
	return m_assigned_truck_license_plate;
}

int DeliveryJob::get_assigned_driver_id() const
{
	return m_assigned_driver_id;
}

unsigned int DeliveryJob::get_job_num() const
{
	return m_job_number;
}

Cargo DeliveryJob::get_cargo_type() const
{
	return m_cargo_type;
}

int DeliveryJob::get_cargo_weight() const
{
	return m_cargo_weight;
}

const Date& DeliveryJob::get_due_date() const
{
	return m_due_date;
}

int DeliveryJob::get_distance() const
{
	return m_distance;
}

bool DeliveryJob::is_ongoing() const
{
	return m_ongoing;
}

Cargo DeliveryJob::str_to_Cargo(const string& str)
{
	string cargo[3] = { REGULAR_DELIVERY ,REFRIGERATED_DELIVERY,LIQUID_DELIVERY };
	Cargo cargo_type[3] = { Cargo::regular,Cargo::refrigerated_goods,Cargo::liquid };
	for (int i = 0;i < 3;i++)
		if (str == cargo[i])
			return cargo_type[i];
	throw string(CARGO_TYPE_ERR);
}

bool DeliveryJob::str_to_ongoing_status(const string& str)
{
	if (str == STATUS_ONGOING)
		return true;
	else if (str == STATUS_PENDING)
		return false;
	else
		throw string(ONGOING_STATUS_ERR);
}

//creates a DeliveryJob from a csv format string
DeliveryJob* DeliveryJob::create_DeliveryJob_from_string(const string& delivery_job_str)
{
	stringstream delivery_line(delivery_job_str);
	string delivery_data[DELIVERY_JOB_PARAMETERS];
	DeliveryJob* delivery_job = nullptr;

	for (int i = 0;i < DELIVERY_JOB_PARAMETERS;i++)
		getline(delivery_line, delivery_data[i], ',');
	try
	{
		int cargo_weight = stoi(delivery_data[5]), distance = stoi(delivery_data[9]);
		Date due_date(delivery_data[6]);
		delivery_job = new DeliveryJob(cargo_weight, due_date, delivery_data[7], delivery_data[8], distance);
		delivery_job->m_job_number = stoul(delivery_data[0]);
		delivery_job->m_assigned_driver_id = stoi(delivery_data[2]);
		delivery_job->m_assigned_truck_license_plate = stoi(delivery_data[1]);
		delivery_job->m_creation_time = Time(delivery_data[3]);
		delivery_job->m_ongoing = str_to_ongoing_status(delivery_data[4]);
	}
	catch (...)
	{
		if (delivery_job)
			delete delivery_job;
		delivery_job = nullptr;
		throw string(DELIVERY_JOB_FROM_STRING_ERR);
	}

	return delivery_job;
}

//assigns a truck and driver to the job as long as the ID and License Plate are valid
void DeliveryJob::assign_truck_and_driver(int driver_id,int truck_license)
{
	if (driver_id < MIN_ID || driver_id > MAX_ID)
		throw string("Invalid ID");

	if (truck_license < 0 || truck_license > MAX_PLATE_NUMBER)
		throw string("Invalid Truck License Plate");

	m_assigned_truck_license_plate = truck_license;
	m_assigned_driver_id = driver_id;
}

void DeliveryJob::set_job_num(unsigned int job_number)
{
	m_job_number = job_number;
}

void DeliveryJob::set_ongoing()
{
	m_ongoing = true;
}

void DeliveryJob::set_cargo_weight(int cargo_weight)
{
	if (cargo_weight <= 0)
		throw string("Cargo Weight ") + NEGATIVE_NUM_ERROR_MSG;
	m_cargo_weight = cargo_weight;
}

//changes due date as long as its bigger than today
void DeliveryJob::set_due_date(const Date& due_date)
{
	Date today;
	if (due_date < today)
		throw string(DUE_DATE_ERR);
	m_due_date = due_date;
}

void DeliveryJob::set_start_and_end_points(const string& start, const string& end, int distance)
{
	if (distance <= 0)
		throw string("Distance ") + NEGATIVE_NUM_ERROR_MSG;
	m_start_point = start;
	m_end_point = end;
	m_distance = distance;
}

//shows only the shared parameters between all the delivery job types
void DeliveryJob::show_aux(ostream& os) const
{
	os << "Job Number: " << m_job_number;
	if (m_assigned_driver_id && m_assigned_truck_license_plate)
		os << "\t\tAssigned Driver's ID: " << m_assigned_driver_id << "\t\tAssigned Truck's License Plate: " << m_assigned_truck_license_plate;
	os << endl << "Created on: " << m_creation_time << endl << "Due Date: " << m_due_date << endl << "Status: " << (m_ongoing ? (STATUS_ONGOING) : (STATUS_PENDING)) << endl;
	os << "From: " << m_start_point << endl << "To: " << m_end_point << endl << "Distance: " << m_distance << endl;
	os << "Cargo Type: " << m_cargo_type << endl << "Cargo Weight: " << m_cargo_weight << endl;
}

void DeliveryJob::show(ostream& os) const
{
	show_aux(os);
}

ostream& operator<<(ostream& os, const DeliveryJob& delivery_job)
{
	delivery_job.show(os);
	return os;
}

//saves the shared parameteres to a file in a csv format
void DeliveryJob::save_to_file_aux(ofstream& ofs) const
{
	ofs << m_cargo_type << ',' << m_job_number << ',' << m_assigned_truck_license_plate << ',';
	ofs << m_assigned_driver_id << ',' << m_creation_time << ',' << (m_ongoing ? (STATUS_ONGOING) : (STATUS_PENDING)) << ',';
	ofs << m_cargo_weight << ',' << m_due_date << ',' << m_start_point << ',' << m_end_point << ',' << m_distance;
}

//saves the whole parameter set of DeliveryJob in the csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the delivery job while we look at the .csv format
void DeliveryJob::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	ofs << ',' << NOT_AVAILABLE << ',' << NOT_AVAILABLE;
}

// auxillary function for ofstream operator overload
ofstream& operator<<(ofstream& ofs, const DeliveryJob& delivery_job)
{
	delivery_job.save_to_file(ofs);
	return ofs;
}

//------------------------ RefrigeratedGoodsDelivery Methods ------------------------

bool RefrigeratedGoodsDelivery::suitable_truck_and_driver(const Truck* truck,const Driver* driver) const
{
	//checks if the truck and driver are not assigned
	if (truck->get_assigned_job_number() != 0 || driver->get_assigned_job_number() != 0)
		return false;
	//checks if the truck tests and driver license are not expired
	if (!(truck->valid_tests()) || !(driver->suitable_for_driving()))
		return false;
	TruckType truck_type = truck->get_truck_type();
	//if the truck type is not Refrigeration Truck it cannot take the Refrigerated goods delivery
	if (truck_type != TruckType::Refrigeration)
		return false;
	//checks if the cargo weight is smaller than the max load weight of the truck
	if (m_cargo_weight > truck->get_max_load_weight())
		return false;
	//checks if the minimal temperature of the refrigeration truck is smaller than the minimal temperature needed for the delivery
	if (m_refrigeration_temp < ((RefrigerationTruck*)truck)->get_min_temperature())
		return false;

	int total_weight = m_cargo_weight + truck->get_empty_vehicle_weight();
	LicenseType license_type = driver->get_license_type();

	//if the driver type is CplusE he can drive any truck
	//hence only checks if the truck weight + the cargo weight dont exceed the C1 max weight limitation
	if (license_type == LicenseType::C1 && total_weight > MAX_WEIGHT_C1)
		return false;

	return true;
}

RefrigeratedGoodsDelivery::RefrigeratedGoodsDelivery(int refrigeration_temp, int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance)
	: m_refrigeration_temp(refrigeration_temp), DeliveryJob(cargo_weight, due_date, start_point, end_point, distance) 
{
	m_cargo_type = Cargo::refrigerated_goods;
}

//creates a Refrigerated Goods Delivery from Delivery Job + additional specific parameters
RefrigeratedGoodsDelivery::RefrigeratedGoodsDelivery(int refrigeration_temp, const DeliveryJob* delivery_job)
	: m_refrigeration_temp(refrigeration_temp), DeliveryJob(*delivery_job)
{
	m_cargo_type = Cargo::refrigerated_goods;
}

void RefrigeratedGoodsDelivery::set_refregeration_temp(int refrigeration_temp)
{
	m_refrigeration_temp = refrigeration_temp;
}

int RefrigeratedGoodsDelivery::get_refrigeration_temp() const
{
	return m_refrigeration_temp;
}

//creates a Refrigerated Goods Delivery from a csv format string
DeliveryJob* RefrigeratedGoodsDelivery::create_RefrigeratedGoodsDelivery_from_string(const string& refrigerated_delivery_str)
{
	stringstream delivery_line(refrigerated_delivery_str);
	string delivery_data[REFRIGERATED_DELIVERY_PARAMETERS];
	RefrigeratedGoodsDelivery* refrigerated_delivery_job = nullptr;

	for (int i = 0;i < REFRIGERATED_DELIVERY_PARAMETERS;i++)
		getline(delivery_line, delivery_data[i], ',');
	try
	{
		int cargo_weight = stoi(delivery_data[5]), distance = stoi(delivery_data[9]), refrigeration_temp = stoi(delivery_data[10]);
		Date due_date(delivery_data[6]);
		refrigerated_delivery_job = new RefrigeratedGoodsDelivery(refrigeration_temp, cargo_weight, due_date, delivery_data[7], delivery_data[8], distance);
		refrigerated_delivery_job->m_job_number = stoul(delivery_data[0]);
		refrigerated_delivery_job->m_assigned_driver_id = stoi(delivery_data[2]);
		refrigerated_delivery_job->m_assigned_truck_license_plate = stoi(delivery_data[1]);
		refrigerated_delivery_job->m_creation_time = Time(delivery_data[3]);
		refrigerated_delivery_job->m_ongoing = str_to_ongoing_status(delivery_data[4]);
	}
	catch(...)
	{
		if (refrigerated_delivery_job)
			delete refrigerated_delivery_job;
		refrigerated_delivery_job = nullptr;
		throw string(DELIVERY_JOB_FROM_STRING_ERR);
	}
	return refrigerated_delivery_job;
}

//shows the shared parameters + specifics of the Refrigerated Goods Delivery
void RefrigeratedGoodsDelivery::show(ostream& os) const
{
	show_aux(os);
	os << "Refrigeration temperature: " << m_refrigeration_temp << endl;
}

//saves the whole parameter set of Refrigerated Goods Delivery in the csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void RefrigeratedGoodsDelivery::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	ofs << ',' << m_refrigeration_temp << ',' << NOT_AVAILABLE;
}

//------------------------ LiquidGoodsDelivery Methods ------------------------

bool LiquidGoodsDelivery::suitable_truck_and_driver(const Truck* truck, const Driver* driver) const
{
	//checks if the truck and driver are not assigned
	if (truck->get_assigned_job_number() != 0 || driver->get_assigned_job_number() != 0)
		return false;
	//checks if the truck tests and driver license are not expired
	if (!(truck->valid_tests()) || !(driver->suitable_for_driving()))
		return false;
	TruckType truck_type = truck->get_truck_type();
	//if the truck type is not Tanker Truck it cannot take the liquid goods delivery job
	if (truck_type != TruckType::Tanker)
		return false;
	//checks if the cargo weight is smaller than the max load weight of the truck
	if (m_cargo_weight > truck->get_max_load_weight())
		return false;
	//checks if the cargo liquid volume is smaller than the max volume of the tank
	if (m_liquid_volume > ((TankerTruck*)truck)->get_max_volume())
		return false;

	LicenseType license_type = driver->get_license_type();

	//only driver with C+E license type can drive the tanker
	if (license_type != LicenseType::CplusE)
		return false;

	return true;
}

LiquidGoodsDelivery::LiquidGoodsDelivery(int liquid_volume, int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance)
	: m_liquid_volume(liquid_volume), DeliveryJob(cargo_weight, due_date, start_point, end_point, distance)
{
	m_cargo_type = Cargo::liquid;
	if (m_liquid_volume <= 0)
		throw string("Liquid Volume ") + NEGATIVE_NUM_ERROR_MSG;
}

//creates a Liquid Goods Delivery from Delivery Job + additional specific parameters
LiquidGoodsDelivery::LiquidGoodsDelivery(int liquid_volume, const DeliveryJob* delivery_job)
	: m_liquid_volume(liquid_volume), DeliveryJob(*delivery_job)
{
	m_cargo_type = Cargo::liquid;
	if (m_liquid_volume <= 0)
		throw string("Liquid Volume ") + NEGATIVE_NUM_ERROR_MSG;
}

void LiquidGoodsDelivery::set_liquid_volume(int liquid_volume)
{
	if (liquid_volume <= 0)
		throw string("Liquid Volume ") + NEGATIVE_NUM_ERROR_MSG;
	m_liquid_volume = liquid_volume;
}

int LiquidGoodsDelivery::get_liquid_volume() const
{
	return m_liquid_volume;
}

//creates liquid goods delivery from a csv format string
DeliveryJob* LiquidGoodsDelivery::create_LiquidGoodsDelivery_from_string(const string& liquid_delivery_str)
{
	stringstream delivery_line(liquid_delivery_str);
	string delivery_data[TOTAL_DELIVERY_JOB_PARAMETERS];
	LiquidGoodsDelivery* liquid_delivery_job = nullptr;

	for (int i = 0;i < TOTAL_DELIVERY_JOB_PARAMETERS;i++)
		getline(delivery_line, delivery_data[i], ',');
	try
	{
		int cargo_weight = stoi(delivery_data[5]), distance = stoi(delivery_data[9]), liquid_volume = stoi(delivery_data[11]);
		Date due_date(delivery_data[6]);
		liquid_delivery_job = new LiquidGoodsDelivery(liquid_volume, cargo_weight, due_date, delivery_data[7], delivery_data[8], distance);
		liquid_delivery_job->m_job_number = stoul(delivery_data[0]);
		liquid_delivery_job->m_assigned_driver_id = stoi(delivery_data[2]);
		liquid_delivery_job->m_assigned_truck_license_plate = stoi(delivery_data[1]);
		liquid_delivery_job->m_creation_time = Time(delivery_data[3]);
		liquid_delivery_job->m_ongoing = str_to_ongoing_status(delivery_data[4]);
	}
	catch (...)
	{
		if (liquid_delivery_job)
			delete liquid_delivery_job;
		liquid_delivery_job = nullptr;
		throw string(DELIVERY_JOB_FROM_STRING_ERR);
	}
	return liquid_delivery_job;
}

//shows the shared parameters + specifics of the Liquid Goods Delivery
void LiquidGoodsDelivery::show(ostream& os) const
{
	show_aux(os);
	os << "Liquid Volume: " << m_liquid_volume << endl;
}

//saves the Liquid Goods Delivery to a file in a csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void LiquidGoodsDelivery::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	ofs << ',' << NOT_AVAILABLE << ',' << m_liquid_volume;
}

//------------------------ JobList Methods ------------------------

//creates DeliveryJob from csv format string by type
DeliveryJob* JobList::create_DeliveryJob_from_string(const string& job_line)
{
	string cargo_type_str, job_data;
	stringstream job_stream(job_line);
	getline(job_stream, cargo_type_str, ',');
	getline(job_stream, job_data);

	Cargo cargo_type = DeliveryJob::str_to_Cargo(cargo_type_str);

	switch (cargo_type)
	{
	case Cargo::regular:
		return DeliveryJob::create_DeliveryJob_from_string(job_data);
	case Cargo::refrigerated_goods:
		return RefrigeratedGoodsDelivery::create_RefrigeratedGoodsDelivery_from_string(job_data);
	case Cargo::liquid:
		return LiquidGoodsDelivery::create_LiquidGoodsDelivery_from_string(job_data);
	default:
		throw string(DELIVERY_JOB_FROM_STRING_ERR);
	}
}

JobList::JobList()
	:m_job_counter(0) { } //job counter allways starts from 0 if no file is loaded, since there are no jobs

JobList::JobList(const string& job_list_file)
{
	ifstream job_list(job_list_file);
	if (!job_list.is_open())
		throw job_list_file;
	//ignores the 1st line since its only the header
	job_list.ignore(numeric_limits<streamsize>::max(), ',');
	job_list.clear();
	string job_counter;
	unsigned int job_num;
	bool status;
	getline(job_list, job_counter, ',');
	m_job_counter = stoi(job_counter);
	job_list.ignore(numeric_limits<streamsize>::max(), '\n');
	job_list.ignore(numeric_limits<streamsize>::max(), '\n');
	job_list.clear();
	DeliveryJob* delivery_job;
	string job_line;
	while (!job_list.eof())
	{
		//takes data from the csv format file, line by line and creates truck from each line
		delivery_job = nullptr;
		getline(job_list, job_line);
			if (job_line.empty())
				break;
		try
		{
			delivery_job = create_DeliveryJob_from_string(job_line);
			job_num = delivery_job->get_job_num();
			status = delivery_job->is_ongoing();
			//checks that there have been no duplications in the file
			if (m_ongoing_jobs.count(job_num) || m_pending_jobs.count(job_num))
				throw delivery_job;
		}
		catch(...)
		{
			job_list.close();
			if (delivery_job)
				delete delivery_job;
			delete_jobs(true);
			delete_jobs(false);
			throw job_list_file;
		}
		if (status)
			m_ongoing_jobs[job_num] = delivery_job;
		else
			m_pending_jobs[job_num] = delivery_job;
	}
	job_list.close();
}

void JobList::delete_jobs(bool ongoing_job)
{
	map<unsigned int, DeliveryJob*>& job_list = ongoing_job ? m_ongoing_jobs : m_pending_jobs;
	map<unsigned int, DeliveryJob*>::iterator iter = job_list.begin(), end_iter = job_list.end();
	for (;iter != end_iter;iter++)
		delete iter->second;
	job_list.clear();
}

JobList::~JobList()
{
	delete_jobs(true);
	delete_jobs(false);
}

//there is no check for a uniqness of a job number since the counter is always raising.
void JobList::add_new_job(DeliveryJob* delivery_job)
{
	m_job_counter++;
	delivery_job->set_job_num(m_job_counter);
	m_pending_jobs[m_job_counter] = delivery_job;
}

const DeliveryJob* JobList::get_job(unsigned int job_num) const
{
	//if the job exists return the delivery job, otherwise throws the Job Num
	if (m_pending_jobs.count(job_num))
		return m_pending_jobs.at(job_num);
	else if (m_ongoing_jobs.count(job_num))
		return m_ongoing_jobs.at(job_num);

	throw job_num;
}

void JobList::remove_job(unsigned int job_num)
{
	//if the delivery job exists removes and deletes it, otherwise throws the Job Num
	if (m_pending_jobs.count(job_num))
	{
		delete m_pending_jobs.at(job_num);
		m_pending_jobs.erase(job_num);
	}
	else if (m_ongoing_jobs.count(job_num))
	{
		delete m_ongoing_jobs.at(job_num);
		m_ongoing_jobs.erase(job_num);
	}
	else
		throw job_num;
}


void JobList::edit_job_cargo_weight(unsigned int job_num, int cargo_weight)
{
	//if the job exists edits the cargo weight of the job, otherwise throws the Job Num
	//if it is ongoing throws error msg
	if (m_ongoing_jobs.count(job_num))
		throw string(ONGOING_JOB_ERR);
	if (!m_pending_jobs.count(job_num))
		throw job_num;

	m_pending_jobs.at(job_num)->set_cargo_weight(cargo_weight);
}

void JobList::edit_job_due_date(unsigned int job_num, const Date& due_date)
{
	//if the job exists edits the due date of the job, otherwise throws the Job Num
	if (m_ongoing_jobs.count(job_num))
		m_ongoing_jobs.at(job_num)->set_due_date(due_date);
	else if (m_pending_jobs.count(job_num))
		m_pending_jobs.at(job_num)->set_due_date(due_date);
	else
		throw job_num;
}

void JobList::edit_job_start_and_end_points(unsigned int job_num, const string& start, const string& end, int distance)
{
	//if the job exists edits the start point end point and distance of the job, otherwise throws the Job Num
	//if it is ongoing throws error msg
	if (m_ongoing_jobs.count(job_num))
		throw string(ONGOING_JOB_ERR);
	if (!m_pending_jobs.count(job_num))
		throw job_num;

	m_pending_jobs.at(job_num)->set_start_and_end_points(start, end, distance);
}

void JobList::edit_job_refregeration_temp(unsigned int job_num, int refrigeration_temp)
{
	//if the job exists edits the refrigeration temperature of the job, otherwise throws the Job Num
	//if it is ongoing throws error msg
	//if it is not refrigeration goods delivery throws cargo type
	if (m_ongoing_jobs.count(job_num))
		throw string(ONGOING_JOB_ERR);
	if (!m_pending_jobs.count(job_num))
		throw job_num;

	DeliveryJob* delivery_job = m_pending_jobs.at(job_num);
	Cargo cargo_type = delivery_job->get_cargo_type();

	if (cargo_type != Cargo::refrigerated_goods)
		throw cargo_type;

	static_cast<RefrigeratedGoodsDelivery*>(delivery_job)->set_refregeration_temp(refrigeration_temp);
}

void JobList::edit_job_liquid_volume(unsigned int job_num, int liquid_volume)
{
	//if the job exists edits the liquid volume of the job, otherwise throws the Job Num
	//if it is ongoing throws error msg
	//if it is not liquid goods delivery throws cargo type
	if (m_ongoing_jobs.count(job_num))
		throw string(ONGOING_JOB_ERR);
	if (!m_pending_jobs.count(job_num))
		throw job_num;

	DeliveryJob* delivery_job = m_pending_jobs.at(job_num);
	Cargo cargo_type = delivery_job->get_cargo_type();

	if (cargo_type != Cargo::liquid)
		throw cargo_type;

	static_cast<LiquidGoodsDelivery*>(delivery_job)->set_liquid_volume(liquid_volume);
}

void JobList::start_job(unsigned int job_num, int driver_id,int truck_license)
{
	//if the job exists assigns it the id and the truck license, otherwise throws the Job Num
	//if it is ongoing throws error msg
	if (!m_pending_jobs.count(job_num))
		if (m_ongoing_jobs.count(job_num))
			throw string(ALREADY_ONGOING_JOB_ERR);
		else
			throw job_num;

	//moves the job from pending map to ongoing map
	m_ongoing_jobs[job_num] = m_pending_jobs.at(job_num);
	m_pending_jobs.erase(job_num);
	m_ongoing_jobs.at(job_num)->set_ongoing();
	//also checks inside if the ID and License Plate are valid
	m_ongoing_jobs[job_num]->assign_truck_and_driver(driver_id, truck_license);
}

// auxillary function for ofstream operator overload
void JobList::save_to_csv_format_file_by_status(bool ongoing_job, ofstream& ofs) const
{
	map<unsigned int, DeliveryJob*>::const_iterator iter = ongoing_job ? m_ongoing_jobs.begin() : m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = ongoing_job ? m_ongoing_jobs.end() : m_pending_jobs.end();

	for (;iter != end_iter;iter++)
		ofs << *(iter->second) << endl;
}

// saves the JobList in a csv format
ofstream& operator<<(ofstream& ofs, const JobList& job_list)
{
	ofs << NUM_OF_JOBS_CREATED_HEADER << ',' << job_list.m_job_counter << ',' << endl << JOB_LIST_HEADER << endl;
	job_list.save_to_csv_format_file_by_status(false, ofs);
	job_list.save_to_csv_format_file_by_status(true, ofs);
	return ofs;
}

//shows on screen only the delivery jobs Job Number, Due Date, Cargo Type 
void JobList::show_reduced_list() const
{
	cout << "-------------------- Pending Delivery Jobs --------------------" << endl << endl;
	map<unsigned int, DeliveryJob*>::const_iterator iter = m_pending_jobs.begin(), end_iter = m_pending_jobs.end();
	for (;iter != end_iter;iter++)
		cout << "Job Number: " << iter->first << "\tDue Date: " << iter->second->get_due_date() << "\tCargo Type: " << iter->second->get_cargo_type() << endl;

	cout << endl << endl << "-------------------- Ongoing Delivery Jobs --------------------" << endl << endl;
	iter = m_ongoing_jobs.begin();
	end_iter = m_ongoing_jobs.end();
	for (;iter != end_iter;iter++)
		cout << "Job Number: " << iter->first << "\tDue Date: " << iter->second->get_due_date() << "\tCargo Type: " << iter->second->get_cargo_type() << endl;
}

//show only the jobs with ongoing or pending status, determined by the argument
void JobList::show_by_status(bool ongoing_job, ostream& os) const
{
	map<unsigned int, DeliveryJob*>::const_iterator iter = ongoing_job ? m_ongoing_jobs.begin() : m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = ongoing_job ? m_ongoing_jobs.end() : m_pending_jobs.end();

	if (ongoing_job)
		os << "-------------------- Ongoing Delivery Jobs --------------------" << endl << endl;
	else
		os << "-------------------- Pending Delivery Jobs --------------------" << endl << endl;

	for (;iter != end_iter;iter++)
		os << *(iter->second) << endl << endl;
}

ostream& operator<<(ostream& os, const JobList& job_list)
{
	job_list.show_by_status(false, os);
	os << endl << endl;
	job_list.show_by_status(true, os);
	return os;
}

//shows only the delivery jobs with specific cargo type, given by the argument
void JobList::show_by_type(Cargo cargo_type) const
{
	cout << "---------------------------------------- Delivery Jobs with: " << cargo_type << " ----------------------------------------" << endl << endl;

	map<unsigned int, DeliveryJob*>::const_iterator iter = m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = m_pending_jobs.end();

	cout << "-------------------- Pending "<<cargo_type<<" Jobs --------------------" << endl << endl;

	for (;iter != end_iter;iter++)
		if (iter->second->get_cargo_type() == cargo_type)
			cout << *(iter->second) << endl << endl;

	iter = m_ongoing_jobs.begin();
	end_iter = m_ongoing_jobs.end();

	cout << "-------------------- Ongoing "<<cargo_type<<" Jobs --------------------" << endl << endl;

	for (;iter != end_iter;iter++)
		if (iter->second->get_cargo_type() == cargo_type)
			cout << *(iter->second) << endl << endl;

}

//shows the list of delivery jobs sorted by cargo weight - auxillary
void JobList::show_by_cargo_weight_aux(bool ongoing_job) const
{
	int cargo_weight;
	multimap<int, DeliveryJob*> delivery_list_copy;
	map<unsigned int, DeliveryJob*>::const_iterator iter = ongoing_job ? m_ongoing_jobs.begin() : m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = ongoing_job ? m_ongoing_jobs.end() : m_pending_jobs.end();
	for (;iter != end_iter;iter++)
	{
		cargo_weight = iter->second->get_cargo_weight();
		delivery_list_copy.insert(pair<int, DeliveryJob*>(cargo_weight, iter->second));
	}

	multimap<int, DeliveryJob*>::const_iterator iter_copy = delivery_list_copy.begin(), end_iter_copy = delivery_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;

}

//shows the list of delivery jobs sorted by cargo weight
void JobList::show_by_cargo_weight() const
{
	cout << "---------------------------------------- Delivery Jobs By Cargo Weight ----------------------------------------" << endl << endl;

	cout << "-------------------- Pending Delivery Jobs --------------------" << endl << endl;
	show_by_cargo_weight_aux(false);
	cout << "-------------------- Ongoing Delivery Jobs --------------------" << endl << endl;
	show_by_cargo_weight_aux(true);
}

//shows the list of delivery jobs sorted by distance - auxillary
void JobList::show_by_distance_aux(bool ongoing_job) const
{
	int distance;
	multimap<int, DeliveryJob*> delivery_list_copy;
	map<unsigned int, DeliveryJob*>::const_iterator iter = ongoing_job ? m_ongoing_jobs.begin() : m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = ongoing_job ? m_ongoing_jobs.end() : m_pending_jobs.end();
	for (;iter != end_iter;iter++)
	{
		distance = iter->second->get_distance();
		delivery_list_copy.insert(pair<int, DeliveryJob*>(distance, iter->second));
	}
	
	multimap<int, DeliveryJob*>::const_iterator iter_copy = delivery_list_copy.begin(), end_iter_copy = delivery_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;

}

//shows the list of delivery jobs sorted by distance
void JobList::show_by_distance() const
{
	cout << "---------------------------------------- Delivery Jobs By Distance ----------------------------------------" << endl << endl;

	cout << "-------------------- Pending Delivery Jobs --------------------" << endl << endl;
	show_by_distance_aux(false);
	cout << "-------------------- Ongoing Delivery Jobs --------------------" << endl << endl;
	show_by_distance_aux(true);
}

//shows the list of delivery jobs sorted by due date - auxillary
void JobList::show_by_due_date_aux(bool ongoing_job) const
{

	multimap<Date, DeliveryJob*> delivery_list_copy;
	map<unsigned int, DeliveryJob*>::const_iterator iter = ongoing_job ? m_ongoing_jobs.begin() : m_pending_jobs.begin();
	map<unsigned int, DeliveryJob*>::const_iterator end_iter = ongoing_job ? m_ongoing_jobs.end() : m_pending_jobs.end();
	for (;iter != end_iter;iter++)
	{
		Date due_date = iter->second->get_due_date();
		delivery_list_copy.insert(pair<Date, DeliveryJob*>(due_date, iter->second));
	}

	multimap<Date, DeliveryJob*>::const_iterator iter_copy = delivery_list_copy.begin(), end_iter_copy = delivery_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;
	
}

//shows the list of delivery jobs sorted by due date
void JobList::show_by_due_date() const
{
	cout << "---------------------------------------- Delivery Jobs By Due Date ----------------------------------------" << endl << endl;

	cout << "-------------------- Pending Delivery Jobs --------------------" << endl << endl;
	show_by_due_date_aux(false);
	cout << "-------------------- Ongoing Delivery Jobs --------------------" << endl << endl;
	show_by_due_date_aux(true);
}
