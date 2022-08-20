#ifndef TFM_APPLICATION_H
#define TFM_APPLICATION_H

#define USER_DATA_FIELDS 3

#include "Driver.h"
#include "Delivery.h"
#include "Truck.h"

#define DRIVER_LIST_FILE_NAME "DriverList.csv"
#define TRUCK_FLEET_FILE_NAME "TruckFleet.csv"
#define DELIVERY_JOB_LIST_FILE_NAME "DeliveryJobList.csv"
#define USERS_FILE_NAME "users.txt"


enum class UserType { user, admin };

class User
{
private:
	string m_name;
	string m_surname;
	UserType m_user_type;
public:
	User(const string& name, const string& surname, const string& user_type);
	bool is_admin() const;
	friend ostream& operator<<(ostream& os, const User& user);
};

class Interface
{
private:
	DriverList m_driver_list;
	TruckFleet m_truck_fleet;
	JobList m_job_list;
	map<string, string[4]> m_latest_login_users;// saves the latest users logins for faster login
	User* m_user;


	bool search_user_in_file(const string& username, const string& password, string* user_data, int data_len) const;

	void show_main_menu() const;
	void save_to_file(const string& driver_list_file_name, const string& truck_fleet_file_name, const string& job_list_file_name);

	bool wait_for_return() const;// waits for user input to return to the last menu or the main menu
	bool try_again() const;// waits for user input to try the last action again or return to the last menu

	//all the bool menus are for the sake of returning to the main menu straight away when they return true

	// ------------------------ Driver Menus ------------------------

	void show_driver_menu() const;
	void driver_menu();

	void show_driver_list_menu() const;
	bool driver_list_menu();

	void show_driver_search_menu() const;
	bool driver_search_menu();

	Driver* add_Driver_aux();
	bool add_driver();

	void show_remove_driver_menu() const;
	bool remove_driver_menu();
	void remove_driver(bool remove_by_id);

	bool show_detailed_driver(bool search_by_id);
	void show_edit_driver_menu(int id) const;
	bool edit_driver_menu(int id);
	void edit_driver_name(int id,bool name);
	void edit_driver_license_type(int id);
	void edit_driver_expiration(int id);
	void edit_driver_glasses_limitation(int id);

	//all the bool menus are for the sake of returning to the main menu straight away when they return true

	// ------------------------ Truck Menus ------------------------
	void show_truck_menu() const;
	void truck_menu();

	void show_truck_fleet_menu() const;
	bool truck_fleet_menu();

	Truck* add_Truck_aux(TruckType truck_type);
	Truck* add_SemiTrailer_aux(const Truck* truck);
	Truck* add_RefrigerationTruck_aux(const Truck* truck);
	Truck* add_TankerTruck_aux(const Truck* truck);
	void show_add_truck_menu() const;
	bool add_truck_menu_aux(TruckType& truck_type, char user_input) const;
	bool add_truck_menu();

	void remove_truck();

	bool show_detailed_truck();
	void show_edit_truck_menu(int license_plate_number) const;
	bool edit_truck_menu(int license_plate_number);
	void edit_truck_test_date(int license_plate_number);
	void edit_truck_trailer(int license_plate_number);
	void edit_truck_refrigerator_test_date(int license_plate_number);
	void edit_truck_tanker(int license_plate_number);
	void edit_truck_tanker_test_date(int license_plate_number);

	//all the bool menus are for the sake of returning to the main menu straight away when they return true

	// ------------------------ Delivery Job Menus ------------------------

	void show_delivery_job_menu() const;
	void delivery_job_menu();

	void show_delivery_job_list_menu() const;
	bool delivery_job_list_menu();

	DeliveryJob* add_DeliveryJob_aux(Cargo cargo_type);
	DeliveryJob* add_RefrigeratedGoodsDelivery_aux(const DeliveryJob* delivery_job);
	DeliveryJob* add_LiquidGoodsDelivery_aux(const DeliveryJob* delivery_job);
	void show_add_delivery_job_menu() const;
	bool add_delivery_job_menu_aux(Cargo& cargo_type, char user_input) const;
	bool add_delivery_job_menu();

	void remove_delivery_job();
	
	bool show_detailed_delivery_job();
	void show_edit_delivery_job_menu(unsigned int job_num) const;
	bool start_delivery_job_aux(unsigned int job_num, int driver_id, int truck_license);
	void start_delivery_job(unsigned int job_num);
	bool edit_delivery_job_menu(unsigned int job_num);
	void edit_delivery_job_weight(unsigned int job_num);
	void edit_delivery_job_due_date(unsigned int job_num);
	void edit_delivery_job_start_and_end_points(unsigned int job_num);
	void edit_refregerated_delivery_temp(unsigned int job_num);
	void edit_liquid_delivery_volume(unsigned int job_num);
public:
	Interface();
	Interface(const string& driver_list_file, const string& truck_fleet_file, const string& job_list_file);
	~Interface();
	bool login(const string& username, const string& password);
	void main_menu();
};

#endif // !TFM_APPLICATION_H

