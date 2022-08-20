#ifndef DELIVERY_H
#define DELIVERY_H

#include "Time_and_Date.h"
#include <string>
#include <map>


#define DELIVERY_JOB_PARAMETERS 10
#define REFRIGERATED_DELIVERY_PARAMETERS 11
#define TOTAL_DELIVERY_JOB_PARAMETERS 12

#define NUM_OF_JOBS_CREATED_HEADER "Delivery Jobs Created:"
#define JOB_LIST_HEADER "Cargo Type,Job Number,Truck License Plate,Driver ID,Creation Time,Status,Cargo Weight,Due Date,Start Point,End Point,Distance,Refrigeration Temperature,Liquid Volume"

#define REGULAR_DELIVERY "Regular Delivery"
#define REFRIGERATED_DELIVERY "Refrigerated Delivery"
#define LIQUID_DELIVERY "Liquid Delivery"
#define STATUS_ONGOING "Ongoing"
#define STATUS_PENDING "Pending"

#define ONGOING_JOB_ERR "Ongoing Delivery Jobs Can not be changed"
#define ALREADY_ONGOING_JOB_ERR "The Delivery Job is already ongoing"
#define DUE_DATE_ERR "Due Date most be greater then today's date"
#define CARGO_TYPE_ERR "Invalid Cargo Type"
#define ONGOING_STATUS_ERR "The string does not match to \"Ongoing\" or \"Pending\""
#define DELIVERY_JOB_FROM_STRING_ERR "Invalid delivey job data in csv format string"


class Driver;
class Truck;

enum class Cargo { regular,refrigerated_goods,liquid };

ostream& operator<<(ostream& os, const Cargo& cargo_type);

class DeliveryJob
{
protected:
	int m_assigned_truck_license_plate; // 0 if not assigned
	int m_assigned_driver_id; //0 if not assigned

	Cargo m_cargo_type;
	int m_cargo_weight;
	Date m_due_date;

	string m_start_point;
	string m_end_point;
	int m_distance;

	Time m_creation_time;
	bool m_ongoing;
	unsigned int m_job_number;

	static bool str_to_ongoing_status(const string& str); //converts specific needed strings to bool
	void save_to_file_aux(ofstream& ofs) const;//saves only the parameters that are shared between the different delivery job types
	virtual void save_to_file(ofstream& ofs) const;
	void show_aux(ostream& os) const;//prints only the parameters that are shared between the different delivery job types
	virtual void show(ostream& os) const;
public:
	DeliveryJob(int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance);
	void assign_truck_and_driver(int driver_id, int truck_license);
	virtual bool suitable_truck_and_driver(const Truck* truck, const Driver* driver) const;//checks if the truck and driver can be assigned to the job
	static Cargo str_to_Cargo(const string& str);
	static DeliveryJob* create_DeliveryJob_from_string(const string& delivery_job_str);//creates a Delivery Job from csv string

	void set_job_num(unsigned int job_number);
	void set_ongoing();
	void set_cargo_weight(int cargo_weight);
	void set_due_date(const Date& due_date);
	void set_start_and_end_points(const string& start, const string& end, int distance);

	int get_assigned_truck_license_plate() const;
	int get_assigned_driver_id() const;
	unsigned int get_job_num() const;
	Cargo get_cargo_type() const;
	int get_cargo_weight() const;
	const Date& get_due_date() const;
	int get_distance() const;
	bool is_ongoing() const;

	friend ofstream& operator<<(ofstream& ofs, const DeliveryJob& delivery_job);
	friend ostream& operator<<(ostream& os, const DeliveryJob& delivery_job);
};


class RefrigeratedGoodsDelivery : public DeliveryJob
{
private:
	int m_refrigeration_temp;

	void save_to_file(ofstream& ofs) const;
	void show(ostream& os) const;
public:
	RefrigeratedGoodsDelivery(int refrigeration_temp, int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance);
	RefrigeratedGoodsDelivery(int refrigeration_temp, const DeliveryJob* delivery_job);
	
	bool suitable_truck_and_driver(const Truck* truck, const Driver* driver) const;//checks if the truck and driver can be assigned to the job
	static DeliveryJob* create_RefrigeratedGoodsDelivery_from_string(const string& refrigerated_delivery_str);//creates a Refrigerated Goods Delivery Job from csv string

	void set_refregeration_temp(int refrigeration_temp);

	int get_refrigeration_temp() const;
};


class LiquidGoodsDelivery : public DeliveryJob
{
private:
	int m_liquid_volume;

	void save_to_file(ofstream& ofs) const;
	void show(ostream& os) const;
public:
	LiquidGoodsDelivery(int liquid_volume, int cargo_weight, const Date& due_date, const string& start_point, const string& end_point, int distance);
	LiquidGoodsDelivery(int liquid_volume, const DeliveryJob* delivery_job);
	bool suitable_truck_and_driver(const Truck* truck, const Driver* driver) const;//checks if the truck and driver can be assigned to the job
	static DeliveryJob* create_LiquidGoodsDelivery_from_string(const string& liquid_delivery_str);//creates a Liquid Goods Delivery Job from csv string

	void set_liquid_volume(int liquid_volume);

	int get_liquid_volume() const;
};

class JobList
{
private:
	map<unsigned int, DeliveryJob*> m_pending_jobs;
	map<unsigned int, DeliveryJob*> m_ongoing_jobs;
	unsigned int m_job_counter;

	DeliveryJob* create_DeliveryJob_from_string(const string& delivery_line);//creates a DeliveryJob from csv string by type
	void show_by_distance_aux(bool ongoing_job) const;
	void show_by_cargo_weight_aux(bool ongoing_job) const;
	void show_by_due_date_aux(bool ongoing_job) const;
	void delete_jobs(bool ongoing_job);
	void save_to_csv_format_file_by_status(bool ongoing_job, ofstream& ofs) const;
public:
	JobList();
	JobList(const string& job_list_file);
	~JobList();
	void add_new_job(DeliveryJob* delivery_job);
	const DeliveryJob* get_job(unsigned int job_num) const;
	void start_job(unsigned int job_num, int driver_id, int truck_license);
	void remove_job(unsigned int job_num);

	void edit_job_cargo_weight(unsigned int job_num, int cargo_weight);
	void edit_job_due_date(unsigned int job_num, const Date& due_date);
	void edit_job_start_and_end_points(unsigned int job_num, const string& start, const string& end, int distance);
	void edit_job_refregeration_temp(unsigned int job_num, int refrigeration_temp);
	void edit_job_liquid_volume(unsigned int job_num,int liquid_volume);

	void show_reduced_list() const;
	void show_by_status(bool ongoing_job, ostream& os = cout) const;
	void show_by_type(Cargo cargo_type) const;
	void show_by_cargo_weight() const;
	void show_by_distance() const;
	void show_by_due_date() const;

	friend ofstream& operator<<(ofstream& ofs, const JobList& job_list);
	friend ostream& operator<<(ostream& os, const JobList& job_list);
};


#endif // !DELIVERY_H
