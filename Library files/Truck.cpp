#include "Truck.h"
#include "Driver.h"
#include "Delivery.h"

// ------------------------ Auxillary Functions ------------------------

ostream& operator<<(ostream& os, const TruckType& truck_type)
{
	switch (truck_type)
	{
	case TruckType::Truck:
		return os << TRUCK;
	case TruckType::SemiTrailer:
		return os << SEMI_TRAILER;
	case TruckType::Refrigeration:
		return os << REFRIGERATION_TRUCK;
	case TruckType::Tanker:
		return os << TANKER_TRUCK;
	}
	return os;
}


// ------------------------ Truck Methods ------------------------

Truck::Truck(int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage)
	:m_assigned_delivery_job_num(0), m_truck_type(TruckType::Truck), m_license_plate_number(plate_number), m_model(model), m_test_date(test_date),
	m_empty_vehicle_weight(empty_vehicle_weight), m_max_load_weight(max_load_weight), m_fuel_consumption_rate(fuel_consumption_rate), m_mileage(mileage)
{
	if (m_license_plate_number < 0 || m_license_plate_number > MAX_PLATE_NUMBER)
		throw m_license_plate_number;
	if (m_empty_vehicle_weight <= 0)
		throw string("Empty Vehicle Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_max_load_weight <= 0)
		throw string("Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_fuel_consumption_rate <= 0)
		throw string("Fuel Consumption Rate ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_mileage <= 0)
		throw string("Mileage ") += NEGATIVE_NUM_ERROR_MSG;
}

//unnasigned job is 0, cannot assign a job if one is llaredy assigned
void Truck::assign_job(unsigned int assigned_job_num)
{
	if (m_assigned_delivery_job_num != 0)
		throw string(TRUCK_TAKEN_ERR_MSG);

	m_assigned_delivery_job_num = assigned_job_num;
}

void Truck::remove_job()
{
	m_assigned_delivery_job_num = 0;
}

//sets a new test date only if it is higher than the previous
void Truck::set_test_date(const Date& new_test)
{
	if (new_test < m_test_date)
		throw string(TEST_DATE_ERROR_MSG);
	m_test_date = new_test;
}

//cannot reduce mileage, hence only adding it
void Truck::add_mileage(int distance)
{
	if (distance < 0)
		throw string(NEGATIVE_NUM_ERROR_MSG);
	m_mileage += (distance*2);
}

int Truck::get_license_plate_num() const
{
	return m_license_plate_number;
}

TruckType Truck::get_truck_type() const
{
	return m_truck_type;
}

const Date& Truck::get_test_date() const
{
	return m_test_date;
}

int Truck::get_empty_vehicle_weight() const
{
	return m_empty_vehicle_weight;
}

int Truck::get_max_load_weight() const
{
	return m_max_load_weight;
}

double Truck::get_fuel_consumption_rate() const
{
	return m_fuel_consumption_rate;
}

//checks if the test is not expired
bool Truck::valid_tests() const
{
	return !m_test_date.expires_in_n_year(TEST_VALID_YEARS);
}

unsigned int Truck::get_assigned_job_number() const
{
	return m_assigned_delivery_job_num;
}

const string& Truck::get_model() const
{
	return m_model;
}

int Truck::get_mileage() const
{
	return m_mileage;
}

bool Truck::is_assigned() const
{
	return (m_assigned_delivery_job_num > 0);
}

TruckType Truck::str_to_TruckType(const string& str)
{
	string truck[4] = { TRUCK,SEMI_TRAILER,REFRIGERATION_TRUCK,TANKER_TRUCK };
	TruckType truck_type[4] = { TruckType::Truck, TruckType::SemiTrailer, TruckType::Refrigeration, TruckType::Tanker };
	for (int i = 0;i < 4;i++)
		if (str == truck[i])
			return truck_type[i];
	throw string(TRUCK_TYPE_ERR);
}

//creates a truck from a csv format string
Truck* Truck::create_Truck_from_string(const string& truck_str)
{
	stringstream truck_line(truck_str);
	string truck_data[TRUCK_PARAMETERS];
	Truck* truck = nullptr;

	for (int i = 0;i < TRUCK_PARAMETERS;i++)
		getline(truck_line, truck_data[i], ',');
	try
	{
		int plate_num = stoi(truck_data[0]), empty_vehicle_weight = stoi(truck_data[3]), max_load_weight = stoi(truck_data[4]), mileage = stoi(truck_data[7]);
		double fuel_consumption_rate = stod(truck_data[6]);
		Date test_date(truck_data[2]);
		truck = new Truck(plate_num, truck_data[1], test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage);
		truck->m_assigned_delivery_job_num = stoi(truck_data[5]);
	}
	catch (...)
	{
		if (truck)
			delete truck;
		truck = nullptr;
		throw string(TRUCK_FROM_STRING_ERR);
	}
	return truck;
}

//shows only the shared parameters between all the truck types
void Truck::show_aux(ostream& os) const
{
	os << "License Plate Number: " << m_license_plate_number << endl;
	if (m_assigned_delivery_job_num)
		os << "Assigned Delivery Job Number: " << m_assigned_delivery_job_num << endl;
	os << "Truck Type: " << m_truck_type << endl << "Model: " << m_model << endl;
	os << "Test Date: " << m_test_date << endl << "Mileage: " << m_mileage << endl;
}

//shows the shared parameters + specifics of the Truck
void Truck::show(ostream& os) const
{
	show_aux(os);
	os << "Empty Vehicle Weight: " << m_empty_vehicle_weight << endl << "Max Load Weight: " << m_max_load_weight << endl;
	os << "Fuel Consumption Rate: " << m_fuel_consumption_rate << endl;
}

ostream& operator<<(ostream& os, const Truck& truck)
{
	truck.show(os);
	return os;
}

//saves the shared parameteres to a file in a csv format
void Truck::save_to_file_aux(ofstream& ofs) const
{
	ofs << m_truck_type << ',' << m_license_plate_number << ',' << m_model << ',' << m_test_date << ',';
	ofs << m_empty_vehicle_weight << ',' << m_max_load_weight << ',' << m_assigned_delivery_job_num << ',';
	ofs << m_fuel_consumption_rate << ',' << m_mileage;
}

//saves the whole parameter set of Truck in the csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void Truck::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	for (int i = 0;i < 7;i++)
		ofs << ',' << NOT_AVAILABLE;
}

ofstream& operator<<(ofstream& ofs, const Truck& truck)
{
	truck.save_to_file(ofs);
	return ofs;
}

// ------------------------ SemiTrailer Methods ------------------------

SemiTrailer::SemiTrailer(int trailer_empty_weight, int trailer_max_load_weight, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage)
	:m_trailer_empty_weight(trailer_empty_weight), m_trailer_max_load_weight(trailer_max_load_weight),
	Truck(plate_number, model, test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage)
{
	m_truck_type = TruckType::SemiTrailer;
	if (m_trailer_max_load_weight <= 0)
		throw string("Trailer Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_trailer_empty_weight <= 0)
		throw string("Trailer Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	//checks if the attached trailer can actually be carried by the truck itself
	if (m_max_load_weight < m_trailer_empty_weight)
		throw string("Trailer ") += TOO_HEAVY_ATTECHED_ERROR_MSG;
}


//creates a semitrailer from Truck + additional specific parameters
SemiTrailer::SemiTrailer(int trailer_empty_weight, int trailer_max_load_weight, const Truck* truck)
	:m_trailer_empty_weight(trailer_empty_weight), m_trailer_max_load_weight(trailer_max_load_weight),
	Truck(*truck)
{
	m_truck_type = TruckType::SemiTrailer;
	if (m_trailer_max_load_weight <= 0)
		throw string("Trailer Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_trailer_empty_weight <= 0)
		throw string("Trailer Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	//checks if the attached trailer can actually be carried by the truck itself
	if (m_max_load_weight < m_trailer_empty_weight)
		throw string("Trailer ") += TOO_HEAVY_ATTECHED_ERROR_MSG;
}

void SemiTrailer::set_trailer(int empty_weight, int max_load_weight)
{
	if (max_load_weight <= 0)
		throw string("Trailer Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (empty_weight <= 0)
		throw string("Trailer Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	//checks if the attached trailer can actually be carried by the truck itself
	if (m_max_load_weight < empty_weight)
		throw string("Trailer ") += TOO_HEAVY_ATTECHED_ERROR_MSG;

	m_trailer_empty_weight = empty_weight;
	m_trailer_max_load_weight = max_load_weight;
}

//returns the sum of the vehicle and the trailer - the total empty weight
int SemiTrailer::get_empty_vehicle_weight() const
{
	return m_empty_vehicle_weight + m_trailer_empty_weight;
}

/*returns the minimum between the load weight of the truck itself which is max truck load weight minus the trailer empty weight
and the max load weight of the trailer*/
int SemiTrailer::get_max_load_weight() const
{
	int max_truck_load_weight = m_max_load_weight - m_trailer_empty_weight;
	return min(max_truck_load_weight, m_trailer_max_load_weight);
}

//creates a SemiTrailer form a csv format string
Truck* SemiTrailer::create_SemiTrailer_from_string(const string& semi_trailer_str)
{
	stringstream truck_line(semi_trailer_str);
	string truck_data[SEMI_TRAILER_PARAMETERS];
	SemiTrailer* semi_truck = nullptr;

	for (int i = 0;i < SEMI_TRAILER_PARAMETERS;i++)
		getline(truck_line, truck_data[i], ',');
	try
	{
		int plate_num = stoi(truck_data[0]), empty_vehicle_weight = stoi(truck_data[3]), max_load_weight = stoi(truck_data[4]), mileage = stoi(truck_data[7]);
		double fuel_consumption_rate = stod(truck_data[6]);
		Date test_date(truck_data[2]);
		int trailer_empty_weight = stoi(truck_data[8]), trailer_max_load_weight = stoi(truck_data[9]);
		semi_truck = new SemiTrailer(trailer_empty_weight, trailer_max_load_weight, plate_num, truck_data[1], test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage);
		semi_truck->m_assigned_delivery_job_num = stoi(truck_data[5]);
	}
	catch(...)
	{
		if (semi_truck)
			delete semi_truck;
		semi_truck = nullptr;
		throw string(TRUCK_FROM_STRING_ERR);
	}
	return semi_truck;
}

//shows the shared parameters + specifics of the SemiTrailer
void SemiTrailer::show(ostream& os) const
{
	show_aux(os);
	os << "Empty Vehicle Weight: " << get_empty_vehicle_weight() << endl;
	os << "Max Load Weight: " << get_max_load_weight() << endl;
	os << "Fuel Consumption Rate: " << m_fuel_consumption_rate << endl;
}

//saves the semi-trailer to a file in a csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void SemiTrailer::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	ofs << ',' << m_trailer_empty_weight << ',';
	ofs << m_trailer_max_load_weight;
	for (int i = 0;i < 5;i++)
		ofs << ',' << NOT_AVAILABLE;
}

// ------------------------ RefrigerationTruck Methods ------------------------

RefrigerationTruck::RefrigerationTruck(int min_temp, double cooling_fuel_consumption_rate, const Date& refrigeration_test, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage)
	: m_min_temp(min_temp), m_cooling_fuel_consumption_rate(cooling_fuel_consumption_rate), m_refrigerator_test_date(refrigeration_test),
	Truck(plate_number, model, test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage)
{ 
	m_truck_type = TruckType::Refrigeration;
	if (m_cooling_fuel_consumption_rate < 0)
		throw string("Cooling Fuel Consumption Rate ") += NEGATIVE_NUM_ERROR_MSG;
}

//creates a refrigeration truck from Truck + additional specific parameters
RefrigerationTruck::RefrigerationTruck(int min_temp, double cooling_fuel_consumption_rate, const Date& refrigeration_test, const Truck* truck)
	: m_min_temp(min_temp), m_cooling_fuel_consumption_rate(cooling_fuel_consumption_rate), m_refrigerator_test_date(refrigeration_test),
	Truck(*truck)
{
	m_truck_type = TruckType::Refrigeration;
	if (m_cooling_fuel_consumption_rate < 0)
		throw string("Cooling Fuel Consumption Rate ") += NEGATIVE_NUM_ERROR_MSG;
}

//sets the new test date if it is hgher than the previous one
void RefrigerationTruck::set_refrigerator_test_date(const Date& test_date)
{
	if (test_date < m_refrigerator_test_date)
		throw string(TEST_DATE_ERROR_MSG);
	m_refrigerator_test_date = test_date;
}


//returns the total fuel consumption rate which is vehicle + refrigerator consumption
double RefrigerationTruck::get_fuel_consumption_rate() const
{
	
	return (m_cooling_fuel_consumption_rate + m_fuel_consumption_rate);
}

const Date& RefrigerationTruck::get_refrigerator_test_date() const
{
	return m_refrigerator_test_date;
}

int RefrigerationTruck::get_min_temperature() const
{
	return m_min_temp;
}

//checks if all the test are not expired
bool RefrigerationTruck::valid_tests() const
{
	return (!m_test_date.expires_in_n_year(TEST_VALID_YEARS)&&!m_refrigerator_test_date.expires_in_n_year(TEST_REFRIGERATION_VALID_YEARS));
}

//creates refrigeration truck from a csv format string
Truck* RefrigerationTruck::create_RefrigerationTruck_from_string(const string& refrigeration_truck_str)
{
	stringstream truck_line(refrigeration_truck_str);
	string truck_data[TOTAL_TRUCK_PARAMETERS];
	RefrigerationTruck* refrigeration_truck = nullptr;

	for (int i = 0;i < TOTAL_TRUCK_PARAMETERS;i++)
		getline(truck_line, truck_data[i], ',');
	try
	{
		int plate_num = stoi(truck_data[0]), empty_vehicle_weight = stoi(truck_data[3]), max_load_weight = stoi(truck_data[4]), mileage = stoi(truck_data[7]);
		double fuel_consumption_rate = stod(truck_data[6]), cooling_fuel_consumption_rate = stod(truck_data[13]);
		Date test_date(truck_data[2]), refrigerator_test_date(truck_data[14]);
		int min_temperature = stoi(truck_data[12]);
		refrigeration_truck = new RefrigerationTruck(min_temperature, cooling_fuel_consumption_rate, refrigerator_test_date, plate_num, truck_data[1], test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage);
		refrigeration_truck->m_assigned_delivery_job_num = stoi(truck_data[5]);
	}
	catch (...)
	{
		if (refrigeration_truck)
			delete refrigeration_truck;
		refrigeration_truck = nullptr;
		throw string(TRUCK_FROM_STRING_ERR);
	}
	return refrigeration_truck;
}

//shows the shared parameters + specifics of the RefrigerationTruck
void RefrigerationTruck::show(ostream& os) const
{
	show_aux(os);
	os << "Empty Vehicle Weight: " << m_empty_vehicle_weight << endl;
	os << "Max Load Weight: " << m_max_load_weight << endl;
	os << "Fuel Consumption Rate: " << get_fuel_consumption_rate() << endl;
	os << "Minimum Temperature: " << m_min_temp << endl;
	os << "Refrigerator Test Date: " << m_refrigerator_test_date << endl;
}

//saves the refrigeration truck to a file in a csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void RefrigerationTruck::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	for (int i = 0;i < 4;i++)
		ofs << ',' << NOT_AVAILABLE;
	ofs << ',' << m_min_temp << ',' << m_cooling_fuel_consumption_rate << ',' << m_refrigerator_test_date;
}

// ------------------------ TankerTruck Methods ------------------------

TankerTruck::TankerTruck(int tanker_empty_weight, int tanker_max_load_weight, const Date& sealing_test, int max_volume, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage)
	: m_tanker_empty_weight(tanker_empty_weight), m_tanker_max_load_weight(tanker_max_load_weight), m_sealing_test_date(sealing_test), m_max_volume(max_volume),
	Truck(plate_number, model, test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage)
{
	m_truck_type = TruckType::Tanker;
	if (m_tanker_max_load_weight <= 0)
		throw string("Tanker Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_tanker_empty_weight <= 0)
		throw string("Tanker Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	//checks if the attached tanker can actually be carried by the truck itself
	if (m_max_load_weight < m_tanker_empty_weight)
		throw string("Tanker ") += TOO_HEAVY_ATTECHED_ERROR_MSG;
	if (m_max_volume <= 0)
		throw string("Tanker Max Volume ") += NEGATIVE_NUM_ERROR_MSG;
}

TankerTruck::TankerTruck(int tanker_empty_weight, int tanker_max_load_weight, const Date& sealing_test, int max_volume, const Truck* truck)
	: m_tanker_empty_weight(tanker_empty_weight), m_tanker_max_load_weight(tanker_max_load_weight), m_sealing_test_date(sealing_test), m_max_volume(max_volume),
	Truck(*truck)
{
	m_truck_type = TruckType::Tanker;
	if (m_tanker_max_load_weight <= 0)
		throw string("Tanker Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_tanker_empty_weight <= 0)
		throw string("Tanker Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	//checks if the attached trailer can actually be carried by the truck itself
	if (m_max_load_weight < m_tanker_empty_weight)
		throw string("Tanker ") += TOO_HEAVY_ATTECHED_ERROR_MSG;
	if (m_max_volume <= 0)
		throw string("Tanker Max Volume ") += NEGATIVE_NUM_ERROR_MSG;
}

//sets the new sealing date if it is higher than the old one
void TankerTruck::set_sealing_test_date(const Date& test_date)
{
	if (test_date < m_sealing_test_date)
		throw string(TEST_DATE_ERROR_MSG);
	m_sealing_test_date = test_date;
}

//sets a new tank - every tank has its own weight, max load weight, test date and volume
void TankerTruck::set_tank(int empty_weight, int max_load_weight, const Date& test_date, int max_volume)
{
	if (max_load_weight <= 0)
		throw string("Tanker Max Load Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (empty_weight <= 0)
		throw string("Tanker Empty Weight ") += NEGATIVE_NUM_ERROR_MSG;
	if (m_max_load_weight < empty_weight)
		throw string("Tanker ") += TOO_HEAVY_ATTECHED_ERROR_MSG;
	if (max_volume <= 0)
		throw string("Tanker Max Volume ") += NEGATIVE_NUM_ERROR_MSG;
	if (test_date < m_sealing_test_date)
		throw string(TEST_DATE_ERROR_MSG);

	m_tanker_empty_weight = empty_weight;
	m_tanker_max_load_weight = max_load_weight;
	m_sealing_test_date = test_date;
	m_max_volume = max_volume;
}

//returns the sum of the vehicle and the tank - the total empty weight
int TankerTruck::get_empty_vehicle_weight() const
{
	return m_tanker_empty_weight + m_empty_vehicle_weight;
}

/*returns the minimum between the load weight of the truck itself which is max truck load weight minus the tank empty weight
and the max load weight of the tank*/
int TankerTruck::get_max_load_weight() const
{
	int max_truck_load_weight = m_max_load_weight - m_tanker_empty_weight;
	return min(max_truck_load_weight, m_tanker_max_load_weight);
}

const Date& TankerTruck::get_sealing_test_date() const
{
	return m_sealing_test_date;
}

int TankerTruck::get_max_volume() const
{
	return m_max_volume;
}

//checks if all the test are not expired
bool TankerTruck::valid_tests() const
{
	return (!m_test_date.expires_in_n_year(TEST_VALID_YEARS) && !m_sealing_test_date.expires_in_n_year(TEST_SEALING_VALID_YEARS));
}

//creates tanker truck form a csv format string
Truck* TankerTruck::create_TankerTruck_from_string(const string& tanker_truck_str)
{
	stringstream truck_line(tanker_truck_str);
	string truck_data[TANKER_PARAMETERS];
	TankerTruck* tanker_truck = nullptr;

	for (int i = 0;i < TANKER_PARAMETERS;i++)
		getline(truck_line, truck_data[i], ',');
	try
	{
		int plate_num = stoi(truck_data[0]), empty_vehicle_weight = stoi(truck_data[3]), max_load_weight = stoi(truck_data[4]), mileage = stoi(truck_data[7]);
		double fuel_consumption_rate = stod(truck_data[6]);
		Date test_date(truck_data[2]), sealing_test_date(truck_data[10]);
		int tanker_empty_weight = stoi(truck_data[8]), tanker_max_load_weight = stoi(truck_data[9]), tanker_max_volume = stoi(truck_data[11]);
		tanker_truck = new TankerTruck(tanker_empty_weight, tanker_max_load_weight, sealing_test_date, tanker_max_volume, plate_num, truck_data[1], test_date, empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage);
		tanker_truck->m_assigned_delivery_job_num = stoi(truck_data[5]);
	}
	catch (...)
	{
		if (tanker_truck)
			delete tanker_truck;
		tanker_truck = nullptr;
		throw string(TRUCK_FROM_STRING_ERR);
	}
	return tanker_truck;
}

//shows the shared parameters + specifics of the TankerTruck
void TankerTruck::show(ostream& os) const
{
	show_aux(os);
	os << "Empty Vehicle Weight: " << get_empty_vehicle_weight() << endl;
	os << "Max Load Weight: " << get_max_load_weight() << endl;
	os << "Fuel Consumption Rate: " << m_fuel_consumption_rate << endl;
	os << "Max Volume: " << m_max_volume << endl;
	os << "Sealing Test Date: " << m_sealing_test_date << endl;
}

//saves the tanker truck to a file in a csv format
//NOT AVAILABLE is a placeholder for all the irrelevant parameters of the truck while we look at the .csv format
void TankerTruck::save_to_file(ofstream& ofs) const
{
	save_to_file_aux(ofs);
	ofs << ',' << m_tanker_empty_weight << ',';
	ofs << m_tanker_max_load_weight << ',' << m_sealing_test_date << ',' << m_max_volume;
	for (int i = 0;i < 3;i++)
		ofs << ',' << NOT_AVAILABLE;
}

// ------------------------ TruckFleet Methods ------------------------

//creates a Truck form a csv format string based on its type
Truck* TruckFleet::create_Truck_from_string(const string& truck_line) const
{
	stringstream truck_stream(truck_line);
	string truck_type_str, truck_data;
	getline(truck_stream, truck_type_str, ',');
	getline(truck_stream, truck_data);
	TruckType truck_type = Truck::str_to_TruckType(truck_type_str);
	switch (truck_type)
	{
	case TruckType::Truck:
		return Truck::create_Truck_from_string(truck_data);
	case TruckType::SemiTrailer:
		return SemiTrailer::create_SemiTrailer_from_string(truck_data);
	case TruckType::Refrigeration:
		return RefrigerationTruck::create_RefrigerationTruck_from_string(truck_data);
	case TruckType::Tanker:
		return TankerTruck::create_TankerTruck_from_string(truck_data);
	default:
		throw string(TRUCK_FROM_STRING_ERR);
	}
}

TruckFleet::TruckFleet(){}

TruckFleet::TruckFleet(const string& truck_fleet_file)
{

	ifstream truck_fleet(truck_fleet_file);
	if (!truck_fleet.is_open())
		throw truck_fleet_file;
	//ignores the 1st line since its only the header
	truck_fleet.ignore(numeric_limits<streamsize>::max(), '\n');
	truck_fleet.clear();
	Truck* truck;
	string truck_line;
	int truck_plate_num;
	while (!truck_fleet.eof())
	{
		//takes data from the csv format file, line by line and creates truck from each line
		truck = nullptr;
		getline(truck_fleet, truck_line);
		if (truck_line.empty())
			break;
		try
		{
			truck = create_Truck_from_string(truck_line);
			truck_plate_num = truck->get_license_plate_num();
			//checks that there have been no duplications in the file
			if (m_trucks.count(truck_plate_num))
				throw truck;
		}
		catch(...)
		{
			truck_fleet.close();
			if (truck)
				delete truck;
			delete_trucks();
			throw truck_fleet_file;
		}
		m_trucks[truck_plate_num] = truck;
	}

	truck_fleet.close();
}

void TruckFleet::delete_trucks()
{
	map<int, Truck*>::iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
		delete iter->second;
	m_trucks.clear();
}

TruckFleet::~TruckFleet()
{
	delete_trucks();
}

void TruckFleet::add_new_truck(Truck* truck)
{
	//the truck is added to the fleet if his License Plate Number is not a duplicate
	int license_plate = truck->get_license_plate_num();
	if (m_trucks.count(license_plate))
		throw truck;
	m_trucks[license_plate] = truck;
}

const Truck* TruckFleet::get_truck(int license_plate_number) const
{
	//if the truck exists return the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;
	else
		return m_trucks.at(license_plate_number);
}

void TruckFleet::remove_truck(int license_plate_number)
{
	//if the truck exists removes and deletes the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;
	Truck* truck = m_trucks.at(license_plate_number);
	if (truck->get_assigned_job_number() > 0)
		throw truck;

	m_trucks.erase(license_plate_number);
	delete truck;
}

void TruckFleet::assign_job(int license_plate_number, unsigned int job_num)
{
	//if the truck exists assigns job to the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;
	//inside assign job also checks if he is not already assigned
	m_trucks.at(license_plate_number)->assign_job(job_num);
}

void TruckFleet::remove_job(int license_plate_number)
{
	// if the truck exists removes job from the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	m_trucks.at(license_plate_number)->remove_job();
}

void TruckFleet::edit_truck_test_date(int license_plate_number, const Date& test_date)
{
	//if the truck exists edits the test date of the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;
	
	m_trucks.at(license_plate_number)->set_test_date(test_date);
}

void TruckFleet::add_truck_mileage(int license_plate_number, int distance)
{
	//if the truck exists adds the needed mileage to the truck, otherwise throws the License Plate Number
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	m_trucks.at(license_plate_number)->add_mileage(distance);
}

void TruckFleet::edit_truck_trailer(int license_plate_number, int empty_weight, int max_load_weight)
{
	//if the truck exists edits the trailer of the truck, otherwise throws the License Plate Number
	//if its not of the relevant type throws the TruckType
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	Truck* truck = m_trucks.at(license_plate_number);
	TruckType truck_type = truck->get_truck_type();

	if (truck_type != TruckType::SemiTrailer)
		throw truck_type;

	static_cast<SemiTrailer*>(truck)->set_trailer(empty_weight, max_load_weight);
}

void TruckFleet::edit_refrigeration_test_date(int license_plate_number, const Date& test_date)
{
	//if the truck exists edits the refrigeration test of the truck, otherwise throws the License Plate Number
	//if its not of the relevant type throws the TruckType
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	Truck* truck = m_trucks.at(license_plate_number);
	TruckType truck_type = truck->get_truck_type();

	if (truck_type != TruckType::Refrigeration)
		throw truck_type;

	static_cast<RefrigerationTruck*>(truck)->set_refrigerator_test_date(test_date);
}

void TruckFleet::edit_truck_tank(int license_plate_number, int empty_weight, int max_load_weight, const Date& test_date, int max_volume)
{
	//if the truck exists edits the tank info of the truck, otherwise throws the License Plate Number
	//if its not of the relevant type throws the TruckType
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	Truck* truck = m_trucks.at(license_plate_number);
	TruckType truck_type = truck->get_truck_type();

	if (truck_type != TruckType::Tanker)
		throw truck_type;

	static_cast<TankerTruck*>(truck)->set_tank(empty_weight, max_load_weight, test_date, max_volume);
}

void TruckFleet::edit_tank_test_date(int license_plate_number, const Date& test_date)
{
	//if the truck exists edits the sealing test date of the truck, otherwise throws the License Plate Number
	//if its not of the relevant type throws the TruckType
	if (!m_trucks.count(license_plate_number))
		throw license_plate_number;

	Truck* truck = m_trucks.at(license_plate_number);
	TruckType truck_type = truck->get_truck_type();

	if (truck_type != TruckType::Tanker)
		throw truck_type;

	static_cast<TankerTruck*>(truck)->set_sealing_test_date(test_date);
}

// saves the TruckFleet in a csv format
ofstream& operator<<(ofstream& ofs, const TruckFleet& truck_fleet)
{
	ofs << TRUCK_FLEET_HEADER << endl;
	map<int, Truck*>::const_iterator iter = truck_fleet.m_trucks.begin(), end_iter = truck_fleet.m_trucks.end();
	for (;iter != end_iter;iter++)
		ofs << *(iter->second) << endl;
	return ofs;
}

//shows on screen only the trucks License Plate Number, Truck Type, Model and Assigned Job Number if it is assigned to a job
void TruckFleet::show_reduced_fleet() const
{
	cout << "-------------------- Truck Fleet --------------------" << endl << endl;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
	{
		unsigned int assigned_job_number = iter->second->get_assigned_job_number();
		cout << "License Plate Number: " << iter->first << "\tTruck Type: " << iter->second->get_truck_type();
		if (iter->second->get_truck_type() != TruckType::Refrigeration)
			cout << '\t';
		cout << "\t\tModel: " << iter->second->get_model();
		if (iter->second->get_model().size() <= 8)
			cout << '\t';
		cout << "  \tAssigned Job Number: ";
		if (assigned_job_number > 0)
			cout << assigned_job_number;
		else
			cout << "Not Assigned";
		cout << endl << endl;
	}
}

//shows the whole truck fleet with all the parameters
ostream& operator<<(ostream& os, const TruckFleet& truck_fleet)
{
	os << "-------------------- Truck Fleet --------------------" << endl << endl;
	map<int, Truck*>::const_iterator iter = truck_fleet.m_trucks.begin(), end_iter = truck_fleet.m_trucks.end();
	for (;iter != end_iter;iter++)
		os << *(iter->second) << endl << endl;
	return os;
}

//shows only the trucks with specific truck type, given by the argument
void TruckFleet::show_by_type(TruckType truck_type) const
{
	cout << "-------------------- "<<truck_type<<" List --------------------" << endl << endl;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	
	for (;iter != end_iter;iter++)
		if (iter->second->get_truck_type() == truck_type)
			cout << *(iter->second) << endl << endl;
}

//shows only the trucks that are assigned or not assigned to a job, determined by the argument
void TruckFleet::show_by_status(bool assigned) const
{
	map<int, Truck*>::const_iterator iter = m_trucks.begin();
	map<int, Truck*>::const_iterator end_iter = m_trucks.end();

	if (assigned)
	{
		cout << "-------------------- Assigned Trucks List --------------------" << endl << endl;
		for (;iter != end_iter;iter++)
			if (iter->second->is_assigned())
				cout << *(iter->second) << endl << endl;
	}
	else
	{
		cout << "-------------------- Unassigned Trucks List --------------------" << endl << endl;
		for (;iter != end_iter;iter++)
			if (!iter->second->is_assigned())
				cout << *(iter->second) << endl << endl;
	}
}

//shows the list of trucks sorted by max load weight
void TruckFleet::show_by_max_load_weight() const
{
	int max_load_weight;
	multimap<int, Truck*> trucks_list_copy;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
	{
		max_load_weight = iter->second->get_max_load_weight();
		trucks_list_copy.insert(pair<int, Truck*>(max_load_weight, iter->second));
	}

	cout << "-------------------- Trucks List By Max Load Weight --------------------" << endl << endl;
	multimap<int, Truck*>::const_iterator iter_copy = trucks_list_copy.begin(), end_iter_copy = trucks_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;

}

//shows the list of trucks sorted by mileage
void TruckFleet::show_by_mileage() const
{
	int mileage;
	multimap<int, Truck*> trucks_list_copy;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
	{
		mileage = iter->second->get_mileage();
		trucks_list_copy.insert(pair<int, Truck*>(mileage, iter->second));
	}

	cout << "-------------------- Trucks List By Mileage --------------------" << endl << endl;
	multimap<int, Truck*>::const_iterator iter_copy = trucks_list_copy.begin(), end_iter_copy = trucks_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;
}

//shows the list of trucks sorted by test date
void TruckFleet::show_by_truck_test_date() const
{
	multimap<Date, Truck*> trucks_list_copy;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
	{
		Date test_date = iter->second->get_test_date();
		trucks_list_copy.insert(pair<Date, Truck*>(test_date, iter->second));
	}

	cout << "-------------------- Trucks List By Test Date --------------------" << endl << endl;
	multimap<Date, Truck*>::const_iterator iter_copy = trucks_list_copy.begin(), end_iter_copy = trucks_list_copy.end();
	for (;iter_copy != end_iter_copy;iter_copy++)
		cout << *(iter_copy->second) << endl << endl;
}

//checks if the truck is suitable for the given job
bool TruckFleet::suitable_truck(const DeliveryJob* delivery_job, int truck_license) const
{
	Cargo cargo_type = delivery_job->get_cargo_type();
	int cargo_weight = delivery_job->get_cargo_weight();
	Truck* truck = m_trucks.at(truck_license);
	//checks if the truck doesnt have an assignd job, its tests are not expired and it can carry the cargo
	if (truck->is_assigned() || !truck->valid_tests() || cargo_weight > truck->get_max_load_weight())
		return false;
	TruckType truck_type = truck->get_truck_type();
	int truck_min_temp, cargo_temp, truck_max_volume, cargo_volume;
	//checks if the truck type is suitable for a specifc delivery and if the specific parmaeters are suitalbe for the cargo
	switch (cargo_type)
	{
	case Cargo::regular:
		if (truck_type == TruckType::Refrigeration || truck_type == TruckType::Tanker)
			return false;
		break;
	case Cargo::refrigerated_goods:
		if (truck_type != TruckType::Refrigeration)
			return false;
		truck_min_temp = static_cast<RefrigerationTruck*>(truck)->get_min_temperature();
		cargo_temp = static_cast<const RefrigeratedGoodsDelivery*>(delivery_job)->get_refrigeration_temp();
		if (cargo_temp < truck_min_temp)
			return false;
		break;
	case Cargo::liquid:
		if (truck_type != TruckType::Tanker)
			return false;
		truck_max_volume = static_cast<TankerTruck*>(truck)->get_max_volume();
		cargo_volume = static_cast<const LiquidGoodsDelivery*>(delivery_job)->get_liquid_volume();
		if (cargo_volume > truck_max_volume)
			return false;
		break;
	}
	return true;
}

//prints all the trucks from the fleet suitable for the given delivery job
//returns true if there was at least 1 suitable truck, otherwise returns false
bool TruckFleet::show_suitable(const DeliveryJob* delivery_job) const
{
	cout << "-------------------- Suitable Trucks List --------------------" << endl << endl;
	int counter = 0;
	map<int, Truck*>::const_iterator iter = m_trucks.begin(), end_iter = m_trucks.end();
	for (;iter != end_iter;iter++)
	{
		if (!suitable_truck(delivery_job, iter->first))
			continue;
		cout << "License Plate Number: " << iter->first << "\tTruck Type: " << iter->second->get_truck_type();
		if (iter->second->get_truck_type() != TruckType::Refrigeration)
			cout << '\t';
		cout << "\tMax Load weight: " << iter->second->get_max_load_weight();
		cout << endl << endl;
		counter++;
	}
	if (counter == 0)
		cout << endl << "No available suitable trucks were found" << endl;
	return counter != 0;
}