#ifndef TRUCK_H
#define TRUCK_H

#include "Time_and_Date.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
using namespace std;

#define min(a,b) ( ( (a) < (b) ) ? (a) : (b) )

#define MAX_PLATE_NUMBER 99999999
#define TEST_VALID_YEARS 1
#define TEST_REFRIGERATION_VALID_YEARS 3
#define TEST_SEALING_VALID_YEARS 5

#define TRUCK_PARAMETERS 8
#define SEMI_TRAILER_PARAMETERS 10
#define TANKER_PARAMETERS 12
#define TOTAL_TRUCK_PARAMETERS 15

#define TEST_DATE_ERROR_MSG "Current test date is greater than the new one given"
#define NEGATIVE_NUM_ERROR_MSG "must be a positive number"
#define TOO_HEAVY_ATTECHED_ERROR_MSG "Empty Weight can not be smaller then the truck's max load weight"
#define TRUCK_TAKEN_ERR_MSG "Truck is already on a Delivery Job"
#define TRUCK_TYPE_ERR "Invalid Truck Type"
#define TRUCK_FROM_STRING_ERR "Invalid truck data in csv format string"

#define TRUCK "Truck"
#define SEMI_TRAILER "SemiTrailer"
#define REFRIGERATION_TRUCK "RefrigerationTruck"
#define TANKER_TRUCK "TankerTruck"

#define NOT_AVAILABLE "N/A"

#define TRUCK_FLEET_HEADER "Truck Type,Plate Number,Model,Test Date,Empty Vehicle Weight,Max Load Weight,Delivery Job Number,Fuel Consumption Rate,Mileage,Trailer/Tanker Empty Weight,Trailer/Tanker Max Load Weight,Sealing test Date,Tanker Max Volume,Min Temperature,Cooling Fuel Consumption Rate,Refrigerator Test Date"


class DeliveryJob;

enum class TruckType { Truck, SemiTrailer, Refrigeration, Tanker };
ostream& operator<<(ostream& os, const TruckType& truck_type);

class Truck
{
protected:
	unsigned int m_assigned_delivery_job_num; // 0 if not assigned
	// general info
	TruckType m_truck_type;
	int m_license_plate_number;
	string m_model;
	Date m_test_date;
	// technical info
	int m_empty_vehicle_weight; //in kg
	int m_max_load_weight;  //in kg
	double m_fuel_consumption_rate; //in km/liter
	int m_mileage; //in km

	void save_to_file_aux(ofstream& ofs) const; //saves only the parameters that are shared between the different truck types
	virtual void save_to_file(ofstream& ofs) const;
	void show_aux(ostream& os) const; //prints only the parameters that are shared between the different truck types
	virtual void show(ostream& os) const;
public:
	Truck(int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage);
	void assign_job(unsigned int assigned_job_num);
	void remove_job();
	bool is_assigned() const;
	virtual bool valid_tests() const; //checks if the test date is valid
	static TruckType str_to_TruckType(const string& str);
	static Truck* create_Truck_from_string(const string& truck_str);
	
	void set_test_date(const Date& new_test);
	void add_mileage(int distance); //mileage can only be increased

	int get_license_plate_num() const;
	TruckType get_truck_type() const;
	const Date& get_test_date() const;
	virtual int get_empty_vehicle_weight() const;
	virtual int get_max_load_weight() const;
	virtual double get_fuel_consumption_rate() const;
	unsigned int get_assigned_job_number() const;
	const string& get_model() const;
	int get_mileage() const;
	
	friend ofstream& operator<<(ofstream& ofs, const Truck& truck);
	friend ostream& operator<<(ostream& os, const Truck& truck);
};


class SemiTrailer : public Truck
{
private:
	int m_trailer_empty_weight;
	int m_trailer_max_load_weight;

	void save_to_file(ofstream& ofs) const;
	void show(ostream& os) const;
public:
	SemiTrailer(int trailer_empty_weight, int trailer_max_load_weight, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage);
	SemiTrailer(int trailer_empty_weight, int trailer_max_load_weight, const Truck* truck);

	void set_trailer(int empty_weight, int max_load_weight);

	int get_empty_vehicle_weight() const; //summs the tailer and vehicle empty weight
	int get_max_load_weight() const; //returns the actuall possible load weight 

	static Truck* create_SemiTrailer_from_string(const string& semi_trailer_str);//creates a semi-trailer from csv string, used by create_Truck_from_string
};

class RefrigerationTruck : public Truck
{
private:
	int m_min_temp;
	double m_cooling_fuel_consumption_rate;
	Date m_refrigerator_test_date;

	void save_to_file(ofstream& ofs) const;
	void show(ostream& os) const;
public:
	RefrigerationTruck(int min_temp, double cooling_fuel_consumption_rate, const Date& refrigeration_test, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage);
	RefrigerationTruck(int min_temp, double cooling_fuel_consumption_rate, const Date& refrigeration_test, const Truck* truck);
	bool valid_tests() const; //checks if both the test and refrigerator test dates are valid
	static Truck* create_RefrigerationTruck_from_string(const string& refrigeration_truck_str);//creates a refrigeration truck from csv string, used by create_Truck_from_string

	void set_refrigerator_test_date(const Date& test_date);
	
	double get_fuel_consumption_rate() const; //summs the fuel consumption rate of the truck itself and the refrigerator
	const Date& get_refrigerator_test_date() const;
	int get_min_temperature() const;
};

class TankerTruck : public Truck
{
private:
	int m_tanker_empty_weight;
	int m_tanker_max_load_weight;
	Date m_sealing_test_date;
	int m_max_volume;

	void save_to_file(ofstream& ofs) const;
	void show(ostream& os) const;
public:
	TankerTruck(int tanker_empty_weight, int tanker_max_load_weight, const Date& sealing_test, int max_volume, int plate_number, const string& model, const Date& test_date, int empty_vehicle_weight, int max_load_weight, double fuel_consumption_rate, int mileage);
	TankerTruck(int tanker_empty_weight, int tanker_max_load_weight, const Date& sealing_test, int max_volume, const Truck* truck);
	bool valid_tests() const; //checks if both the test and sealing test dates are valid
	static Truck* create_TankerTruck_from_string(const string& tanker_truck_str);//creates a tanker truck from csv string, used by create_Truck_from_string
	
	void set_sealing_test_date(const Date& test_date);
	void set_tank(int empty_weight, int max_load_weight, const Date& test_date, int max_volume);
	
	int get_empty_vehicle_weight() const; //summs the empty vehicle weight and empty tanker weight
	int get_max_load_weight() const; // returns the actual max load weight
	const Date& get_sealing_test_date() const; 
	int get_max_volume() const;
};

class TruckFleet
{
private:
	map<int, Truck*> m_trucks;

	Truck* create_Truck_from_string(const string& truck_line) const;//creates a Truck from csv string by type
	bool suitable_truck(const DeliveryJob* delivery_job, int truck_license) const;
	void delete_trucks();
public:
	TruckFleet();
	TruckFleet(const string& truck_fleet_file);
	~TruckFleet();

	void add_new_truck(Truck* truck);
	const Truck* get_truck(int license_plate_number) const;
	void remove_truck(int license_plate_number);
	void assign_job(int license_plate_number, unsigned int job_num);
	void remove_job(int license_plate_number);
	void edit_truck_test_date(int license_plate_number, const Date& test_date);
	void add_truck_mileage(int license_plate_number, int distance);
	void edit_truck_trailer(int license_plate_number, int empty_weight, int max_load_weight);
	void edit_refrigeration_test_date(int license_plate_number, const Date& test_date);
	void edit_truck_tank(int license_plate_number, int empty_weight, int max_load_weight, const Date& test_date, int max_volume);
	void edit_tank_test_date(int license_plate_number, const Date& test_date);

	void show_reduced_fleet() const;
	void show_by_type(TruckType truck_type) const;
	void show_by_status(bool assigned) const;
	void show_by_max_load_weight() const;
	void show_by_mileage() const;
	void show_by_truck_test_date() const;
	bool show_suitable(const DeliveryJob* delivery_job) const;

	friend ofstream& operator<<(ofstream& ofs, const TruckFleet& truck_fleet);
	friend ostream& operator<<(ostream& os, const TruckFleet& truck_fleet);
};
#endif // !TRUCK_H
