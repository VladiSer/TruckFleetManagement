#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif // _DEBUG

#include "TFM_Application.h"

// ------------------------ User Methods ------------------------


User::User(const string& name, const string& surname, const string& user_type)
	: m_name(name), m_surname(surname)
{
	if (user_type == "admin")
		m_user_type = UserType::admin;
	else if (user_type == "user")
		m_user_type = UserType::user;
	else
		throw *this;
}

bool User::is_admin() const
{
	return m_user_type == UserType::admin;
}

ostream& operator<<(ostream& os, const User& user)
{
	return os << user.m_name << ' ' << user.m_surname;
}



// ------------------------ Interface Methods ------------------------


Interface::Interface() :m_user(nullptr) { }

Interface::Interface(const string& driver_list_file, const string& truck_fleet_file, const string& job_list_file)
	: m_user(nullptr), m_driver_list(driver_list_file), m_truck_fleet(truck_fleet_file), m_job_list(job_list_file) { }

Interface::~Interface()
{
	delete m_user;
}

bool Interface::login(const string& username, const string& password)
{
	if (m_user)
	{
		delete m_user;
		m_user = nullptr;
	}

	//checks if there were any last logins, if so checks if yours is one of them
	if (m_latest_login_users.count(username))
	{
		if (m_latest_login_users.at(username)[0] == password)
		{
			m_user = new User(m_latest_login_users.at(username)[1], m_latest_login_users.at(username)[2], m_latest_login_users.at(username)[3]);
			return true;
		}
		else
			return false;
	}
	//if you are not one of the latest logins searches for your info in the user file
	//and if the username and password were correct saves it in the last users map
	string user_data[USER_DATA_FIELDS];
	if (search_user_in_file(username, password, user_data, USER_DATA_FIELDS))
	{
		m_latest_login_users[username][0] = password;
		for (int i = 0;i < USER_DATA_FIELDS;i++)
			m_latest_login_users[username][i + 1] = user_data[i];
		m_user = new User(user_data[0], user_data[1], user_data[2]);
		return true;
	}
	else
		return false;
}

bool Interface::search_user_in_file(const string& username, const string& password, string* user_data, int data_len) const
{
	ifstream user_file(USERS_FILE_NAME);
	if (!user_file.is_open())
		throw USERS_FILE_NAME;
	string user, pass;
	while (!user_file.eof())
	{
		user_file >> user;
		user_file >> pass;
		if (user == username && pass == password)
		{
			for (int i = 0;i < data_len;i++)
				user_file >> user_data[i];
			user_file.close();
			return true;
		}
		else
			user_file.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	user_file.close();
	return false;
}



// ------------------------------------------------ Menus ------------------------------------------------


// ------------------------ General Menus ------------------------

void Interface::show_main_menu() const
{
	system("cls");
	cout << "Hello " << *m_user << endl << endl;
	cout << "Which actions would you like to take?" << endl;
	cout << "1. Driver Actions" << endl;
	cout << "2. Truck Actions" << endl;
	cout << "3. Delivery Actions" << endl;
	cout << endl << "Press \"e\" to log-off" << endl << endl;
}

void Interface::main_menu()
{
	while (true)
	{
		show_main_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = cin.get();
		switch (user_input)
		{
		case '1':
			driver_menu();
			break;
		case '2':
			truck_menu();
			break;
		case '3':
			delivery_job_menu();
			break;
		case 'e':
			save_to_file("DriverList.csv", "TruckFleet.csv", "DeliveryJobList.csv");
			return;
		}
	}
}


void Interface::save_to_file(const string& driver_list_file_name, const string& truck_fleet_file_name, const string& job_list_file_name)
{
	ofstream driver_list_file(driver_list_file_name), truck_fleet_file(truck_fleet_file_name), job_list_file(job_list_file_name);
	driver_list_file << m_driver_list;
	driver_list_file.close();
	truck_fleet_file << m_truck_fleet;
	truck_fleet_file.close();
	job_list_file << m_job_list;
	job_list_file.close();
}

bool Interface::wait_for_return() const
{
	char user_input;
	while (true)
	{
		cout << endl;
		cout << "Press \"r\" to return" << endl;
		cout << "Press \"m\" to return to the main menu" << endl << endl;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		user_input = getchar();
		if (user_input == 'r')
			return false;
		if (user_input == 'm')
			return true;
		system("cls");
	}
}

bool Interface::try_again() const
{
	char user_input;
	while (true)
	{
		cout << "Do you want to try again? (y/n) ";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		user_input = getchar();
		if (user_input == 'y')
			return true;
		if (user_input == 'n')
			return false;
		system("cls");
	}
}



// ------------------------ Driver Menus ------------------------


void Interface::show_driver_menu() const
{
	system("cls");
	cout << "Driver Actions Menu" << endl << endl;
	cout << "Which driver actions would you like to take?" << endl;
	cout << "1. View Driver List" << endl;
	cout << "2. Search For Specific Driver" << endl;
	if (m_user->is_admin())
	{
		cout << "3. Add New Driver" << endl;
		cout << "4. Remove Driver" << endl;
	}
	cout << endl << "Press \"r\" to return" << endl << endl;
	
}

void Interface::driver_menu()
{	
	while (true)
	{
		show_driver_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			if (driver_list_menu())
				return;
			break;
		case '2':
			if (driver_search_menu())
				return;
			break;
		case '3':
			if (m_user->is_admin())
				if (add_driver())
					return;
			break;
		case '4':
			if (m_user->is_admin())
				if(remove_driver_menu())
					return;
			break;
		case 'r':
			return;
		}
	}
}



void Interface::show_driver_list_menu() const
{
	system("cls");
	cout << "Driver List Menu" << endl << endl;
	cout << "Which driver list actions would you like to take?" << endl;
	cout << "1. View Reduced Driver List" << endl;
	cout << "2. View Detailed Driver List" << endl;
	cout << "3. View Detailed Driver List By Assignment Status" << endl;
	cout << "4. View Detailed Driver List By License Type" << endl;
	cout << "5. View Detailed Driver List By License Expiration Date" << endl;
	cout << endl << "6. View Detailed Driver" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::driver_list_menu()
{
	while (true)
	{
		show_driver_list_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		system("cls");
		switch (user_input)
		{
		case '1':
			m_driver_list.show_reduced_list();
			break;
		case '2':
			cout << m_driver_list;
			break;
		case '3':
			m_driver_list.show_by_status(true);
			m_driver_list.show_by_status(false);
			break;
		case '4':			
			m_driver_list.show_by_type(LicenseType::C);
			m_driver_list.show_by_type(LicenseType::C1);
			m_driver_list.show_by_type(LicenseType::CplusE);
			break;
		case '5':
			m_driver_list.show_by_expiration_date();
			break;
		case '6':
			if (driver_search_menu())
				return true;
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		//only the driver search menu is an actual menu by itself so you dont really need to wait for user input
		if (user_input!='6' && wait_for_return())
			return true;
	}
}


void Interface::show_driver_search_menu() const
{
	system("cls");
	cout << "Driver Search Menu" << endl << endl;
	cout << "1. Search Driver by ID" << endl;
	cout << "2. Search Driver by License Number" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl<<endl;
}

bool Interface::driver_search_menu()
{	
	while (true)
	{
		show_driver_search_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			if (show_detailed_driver(true))
				return true;
			break;
		case '2':
			if (show_detailed_driver(false))
				return true;
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
	}
}


Driver* Interface::add_Driver_aux()
{
	system("cls");
	int id, license_number;
	string date_of_birth, name, surname, start_of_work_date, license_type, receipt_of_license_date, latest_permit_date, expiration_date;
	char need_glasses_c = 0;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "Please Enter Driver's Details:" << endl;
	cout << "ID: ";
	cin >> id;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "First Name: ";
	getline(cin, name);
	cout << "Surname: ";
	getline(cin, surname);
	cout << "Date Of Birth (dd/mm/yyyy): ";
	cin >> date_of_birth;
	cout << "Start Of Work Date (dd/mm/yyyy): ";
	cin >> start_of_work_date;
	cout << "Licenese Number: ";
	cin >> license_number;
	cout << "License Type (" << TYPE_C << "," << TYPE_C1 << "," << TYPE_CplusE << "): ";
	cin >> license_type;
	cout << "Receipt Of License Date (dd/mm/yyyy): ";
	cin >> receipt_of_license_date;
	cout << "Receipt Of Latest Permit Date (dd/mm/yyyy): ";
	cin >> latest_permit_date;
	cout << "License Expiration Date (dd/mm/yyyy): ";
	cin >> expiration_date;
	cout << "Is the Driver obliged to wear glasses (y/n)? ";
	do {
		need_glasses_c = getchar();
	} while (need_glasses_c != 'y' && need_glasses_c != 'n');
	bool need_glasses = (need_glasses_c == 'y') ? true : false;

	return new Driver(id, Date(date_of_birth), name, surname, Date(start_of_work_date), license_number, Driver::str_to_LicenseType(license_type), Date(receipt_of_license_date), Date(latest_permit_date), Date(expiration_date), need_glasses);
}

bool Interface::add_driver()
{
	while (true)
	{
		try
		{
			Driver* new_driver = add_Driver_aux();
			m_driver_list.add_new_driver(new_driver);
			cout << endl << "Driver Added Successfully" << endl << endl;
			return wait_for_return();			
		}
		//thrown by date constructor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by the driver constructor,date constructor, and str_to_LicenseType
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by driver constructor
		catch (int err)
		{
			cout << err << " is an Invalid ID/License Number" << endl;
		}
		//thrown by add_new_driver
		catch (const Driver* driver_error)
		{
			cout << "Driver with ID: " << driver_error->get_id_num() << " allready exists." << endl;
			delete driver_error;
		}
		if (!try_again())
			return false;
	}
}


void Interface::show_remove_driver_menu() const
{
	system("cls");
	cout << "Remove Driver Menu" << endl << endl;
	cout << "1. Remove Driver by ID" << endl;
	cout << "2. Remove Driver by License Number" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::remove_driver_menu()
{
	while (true)
	{
		show_remove_driver_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		while (true)
		{
			char user_input = getchar();
			switch (user_input)
			{
			case '1':
				remove_driver(true);
				break;
			case '2':
				remove_driver(false);
				break;
			case 'r':
				return false;
			case 'm':
				return true;
			default:
				continue;
			}
			if (wait_for_return())
				return true;
			else
				break;
		}
	}
}

void Interface::remove_driver(bool remove_by_id)
{
	int input;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		string remove_by = remove_by_id ? "ID" : "License Number";
		cout << "Please Enter Driver's " << remove_by << ": ";
		cin >> input;
		cout << endl;
		try
		{
			remove_by_id ? m_driver_list.remove_driver_by_id(input) : m_driver_list.remove_driver_by_license(input);
			cout << "Driver Removed Successfully" << endl;
			return;
		}
		//thrown by remove_driver if the driver is busy
		catch (const Driver* busy_driver)
		{
			cout << "Driver with ID: " << busy_driver->get_id_num() << " has an assigned job" << endl;
			cout << "Cannot be removed untill the Delivery Job is finished"<<endl;
			return;
		}
		//thrown by remove_driver if the driver does not exist
		catch (int err_input)
		{
			cout << "Driver with " << remove_by << ": " << err_input << " was not found" << endl;
		}
		if (!try_again())
			return;
	}
}



//usage of trinary operator to implement both ID and License number searches
bool Interface::show_detailed_driver(bool search_by_id) 
{
	int input;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		
		string search_by = search_by_id ? "ID" : "License Number";
		cout << "Please Enter Driver's " << search_by << ": ";
		cin >> input;
		cout << endl;
		try
		{
			const Driver* driver = (search_by_id ? m_driver_list.get_driver_by_id(input) : m_driver_list.get_driver_by_license(input));
			cout << *driver << endl;
			if (m_user->is_admin())
			{
				cout << endl;
				return edit_driver_menu(driver->get_id_num());
			}
			else
				return 	wait_for_return();
		}
		//thrown by get_driver if he does not exist
		catch (int err_input)
		{
			cout << "Driver with " << search_by << ": " << err_input << " was not found" << endl;
		}
		if (!try_again())
			return false;
	}
}

void Interface::show_edit_driver_menu(int id) const
{
	system("cls");
	cout << *(m_driver_list.get_driver_by_id(id)) << endl << endl;
	cout << "1. Edit Name" << endl;
	cout << "2. Edit Surname" << endl;
	cout << "3. Edit License Type" << endl;
	cout << "4. Edit License Expiration Date" << endl;
	cout << "5. Edit glasses limitation" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::edit_driver_menu(int id)
{
	while (true)
	{
		show_edit_driver_menu(id);
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();

		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			edit_driver_name(id, true);
			break;
		case '2':
			edit_driver_name(id, false);
			break;
		case '3':
			edit_driver_license_type(id);
			break;
		case '4':
			edit_driver_expiration(id);
			break;
		case '5':
			edit_driver_glasses_limitation(id);
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		if (wait_for_return())
			return true;
	}
}

//usage of the trinary operator to implement both name and surname edit functions
void Interface::edit_driver_name(int id,bool name)
{
	string input;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter " << (name ? "Name: " : "Surname: ");
		cin >> input;
		try
		{
			name ? m_driver_list.edit_driver_name(id, input) : m_driver_list.edit_driver_surname(id, input);
			cout << "The " << (name ? "Name" : "Surname") << " has been changed successfully" << endl;
			return;
		}
		//thrown by edit_driver_name/surname if he does not exist
		catch (int err_id)
		{
			cout << "Driver with ID: " << err_id << " was not found" << endl;
			return;
		}
		//thorwn by edit_driver_name/surname if the name or surname is not only letters
		catch (const string& invalid_name)
		{
			cout << invalid_name << " is not a valid name" << endl;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_driver_license_type(int id) 
{
	string license_type,latest_permit_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter License Type (" << TYPE_C << "," << TYPE_C1 << "," << TYPE_CplusE << "): ";
		cin >> license_type;
		cout << "Enter License Latest Permit Date (dd/mm/yyyy): ";
		cin >> latest_permit_date;
		try
		{
			m_driver_list.edit_driver_license_type(id, license_type, latest_permit_date);
			cout << "The License Type has been changed successfully" << endl;
			return;
		}
		//thrown by edit_driver if he does not exist
		catch (int err_id)
		{
			cout << "Driver with ID: " << err_id << " was not found" << endl;
			return;
		}
		//thrown by str_to_LicenseType or Date constructor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by Date constructor
		catch (const Date& invalid_date)
		{
			cout << invalid_date << " is not a valid date" << endl;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_driver_expiration(int id)
{
	string expiration_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter License Expiration Date (dd/mm/yyyy): ";
		cin >> expiration_date;
		try
		{
			m_driver_list.edit_driver_expiration_date(id, expiration_date);
			cout << "The License Expiration Date has been changed successfully" << endl;
			return;
		}
		//thrown by edit_driver if he does not exist
		catch (int err_id)
		{
			cout << "Driver with ID: " << err_id << " was not found" << endl;
			return;
		}
		//thrown by Date constructor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by Date constructor
		catch (const Date& invalid_date)
		{
			cout << invalid_date << " is not a valid date" << endl;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_driver_glasses_limitation(int id)
{
	char need_glasses_c = 0;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Is the Driver obliged to wear glasses (y/n)? ";
		do {
			need_glasses_c = getchar();
		} while (need_glasses_c != 'y' && need_glasses_c != 'n');
		bool need_glasses = (need_glasses_c == 'y') ? true : false;
		try
		{
			m_driver_list.edit_driver_glasses_limitation(id, need_glasses);
			cout << "The Driver's glasses limitation was updated successfully" << endl;
			return;
		}
		//thrown by edit_driver if he does not exist
		catch (int err_id)
		{
			cout << "Driver with ID: " << err_id << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}






// ------------------------ Truck Menus ------------------------


void Interface::show_truck_menu() const
{
	system("cls");
	cout << "Truck Actions Menu" << endl << endl;
	cout << "Which truck actions would you like to take?" << endl;
	cout << "1. View Truck Fleet" << endl;
	cout << "2. Search For Specific Truck" << endl;
	if (m_user->is_admin())
	{
		cout << "3. Add New Truck" << endl;
		cout << "4. Remove Truck" << endl;
	}
	cout << endl << "Press \"r\" to return" << endl << endl;

}

void Interface::truck_menu()
{
	while (true)
	{
		show_truck_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			if (truck_fleet_menu())
				return;
			break;
		case '2':
			if (show_detailed_truck())
				return;
			break;
		case '3':
			if (m_user->is_admin())
				if (add_truck_menu())
					return;
			break;
		case '4':
			if (m_user->is_admin())
			{
				remove_truck();
				if (wait_for_return())
					return;
			}
			break;
		case 'r':
			return;
		}
	}
}



void Interface::show_truck_fleet_menu() const
{
	system("cls");
	cout << "Truck Fleet Menu" << endl << endl;
	cout << "Which truck fleet actions would you like to take?" << endl;
	cout << "1. View Reduced Truck Fleet" << endl;
	cout << "2. View Detailed Truck Fleet" << endl;
	cout << "3. View Detailed Truck Fleet By Assignment Status" << endl;
	cout << "4. View Detailed Truck Fleet By Truck Type" << endl;
	cout << "5. View Detailed Truck Fleet By Milage" << endl;
	cout << "6. View Detailed Truck Fleet By Max Load Weight" << endl;
	cout << "7. View Detailed Truck Fleet By Test Date" << endl;
	cout << endl << "8. View Detailed Truck" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::truck_fleet_menu()
{
	while (true)
	{
		show_truck_fleet_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		system("cls");
		switch (user_input)
		{
		case '1':
			m_truck_fleet.show_reduced_fleet();
			break;
		case '2':
			cout << m_truck_fleet;
			break;
		case '3':
			m_truck_fleet.show_by_status(true);
			m_truck_fleet.show_by_status(false);
			break;
		case '4':
			m_truck_fleet.show_by_type(TruckType::Truck);
			m_truck_fleet.show_by_type(TruckType::SemiTrailer);
			m_truck_fleet.show_by_type(TruckType::Refrigeration);
			m_truck_fleet.show_by_type(TruckType::Tanker);
			break;
		case '5':
			m_truck_fleet.show_by_mileage();
			break;
		case '6':
			m_truck_fleet.show_by_max_load_weight();
			break;
		case '7':
			m_truck_fleet.show_by_truck_test_date();
			break;
		case '8':
			if (show_detailed_truck())
				return true;
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		//only the show detailed truck is an actual menu by itself so you dont really need to wait for user input
		if (user_input != '8' && wait_for_return())
			return true;
	}
}


Truck* Interface::add_Truck_aux(TruckType truck_type)
{
	system("cls");
	int license_plate, empty_vehicle_weight, max_load_weight, mileage;
	string model, test_date;
	double fuel_consumption_rate;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "Please Enter Truck's Details:" << endl;
	cout << "Plate Number: ";
	cin >> license_plate;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "Model: ";
	getline(cin, model);
	cout << "Test Date (dd/mm/yyyy): ";
	cin >> test_date;
	cout << "Empty Vehicle Weight: ";
	cin >> empty_vehicle_weight;
	cout << "Max Load Weight: ";
	cin >> max_load_weight;
	cout << "Fuel Consumption Rate: ";
	cin >> fuel_consumption_rate;
	cout << "Mileage: ";
	cin >> mileage;

	Truck* truck = new Truck(license_plate, model, Date(test_date), empty_vehicle_weight, max_load_weight, fuel_consumption_rate, mileage);
	switch (truck_type)
	{
	case TruckType::Truck:
		return truck;
	case TruckType::SemiTrailer:
		return add_SemiTrailer_aux(truck);
	case TruckType::Refrigeration:
		return add_RefrigerationTruck_aux(truck);
	case TruckType::Tanker:
		return add_TankerTruck_aux(truck);
	default:
		delete truck;
		throw truck_type;
	}
}

Truck* Interface::add_SemiTrailer_aux(const Truck* truck)
{
	int trailer_empty_weight, trailer_max_load_weight;
	cout << "Trailer Empty Weight: ";
	cin >> trailer_empty_weight;
	cout << "Trailer Max Load Weight: ";
	cin >> trailer_max_load_weight;

	Truck* semi_trailer = new SemiTrailer(trailer_empty_weight, trailer_max_load_weight, truck);
	delete truck;
	return semi_trailer;
}

Truck* Interface::add_RefrigerationTruck_aux(const Truck* truck)
{
	int min_temp;
	double cooling_fuel_consumption_rate;
	string refrigerator_test_date;
	cout << "Refrigerator Minimum Temperator: ";
	cin >> min_temp;
	cout << "Refrigerator Cooling Fuel Consumption Rate: ";
	cin >> cooling_fuel_consumption_rate;
	cout << "Refrigerator Test Date (dd/mm/yyyy): ";
	cin >> refrigerator_test_date;

	Truck* refrigeration_truck = new RefrigerationTruck(min_temp, cooling_fuel_consumption_rate, Date(refrigerator_test_date), truck);
	delete truck;
	return refrigeration_truck;
}

Truck* Interface::add_TankerTruck_aux(const Truck* truck)
{
	int tanker_empty_weight, tanker_max_load_weight, max_volume;
	string sealing_test_date;
	cout << "Tanker Empty Weight: ";
	cin >> tanker_empty_weight;
	cout << "Tanker Max Load Weight: ";
	cin >> tanker_max_load_weight;
	cout << "Tanker Sealing Test Date (dd/mm/yyyy): ";
	cin >> sealing_test_date;
	cout << "Tanker Max Volume: ";
	cin >> max_volume;

	Truck* tanker_truck = new TankerTruck(tanker_empty_weight, tanker_max_load_weight, Date(sealing_test_date), max_volume, truck);
	delete truck;
	return tanker_truck;
}

void Interface::show_add_truck_menu() const
{
	system("cls");
	cout << "Add Truck Menu" << endl << endl;
	cout << "Which Truck Type would you like to add?" << endl;
	cout << "1. Regular Truck" << endl;
	cout << "2. Semi Trailer Truck" << endl;
	cout << "3. Refrigeration Truck" << endl;
	cout << "4. Tanker Truck" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::add_truck_menu_aux(TruckType& truck_type, char user_input) const
{
	switch (user_input)
	{
	case '1':
		truck_type = TruckType::Truck;
		break;
	case '2':
		truck_type = TruckType::SemiTrailer;
		break;
	case '3':
		truck_type = TruckType::Refrigeration;
		break;
	case '4':
		truck_type = TruckType::Tanker;
		break;
	default:
		return false;
	}
	return true;
}

bool Interface::add_truck_menu()
{
	while (true)
	{
		show_add_truck_menu();
		TruckType truck_type;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			//if add truck menu aux returns flase, one of the inputs was invalid - go to try again straight away
			if (!add_truck_menu_aux(truck_type, user_input))
				continue;
		}
		try
		{
			Truck* new_truck = add_Truck_aux(truck_type);
			m_truck_fleet.add_new_truck(new_truck);
			cout << endl << "Truck Added Successfully" << endl << endl;
			return wait_for_return();
		}
		//thrown by truck constructors
		catch (int invalid_plate_number)
		{
			cout << invalid_plate_number << " is an Invalid License Plate Number" << endl;
		}
		//thrown by truck and date constructors
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constructors
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl; 
		}
		//thrown by add_new_truck if its duplicate
		catch (const Truck* truck_error)
		{
			cout << "Truck with License Plate Number: " << truck_error->get_license_plate_num() << " allready exists." << endl;
			delete truck_error;
		}
		//can only be thrown by add_truck_aux only if an unexpected error occurs
		catch (const TruckType error)
		{
			cout << "An unexpacted Error has occured" << endl;
		}
		if (!try_again())
			return false;
	}
}


void Interface::remove_truck() //-------------------
{
	int license_plate;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Please Enter Truck's License Plate Number: ";
		cin >> license_plate;
		cout << endl;
		try
		{
			m_truck_fleet.remove_truck(license_plate);
			cout << "Truck Removed Successfully" << endl;
			return;
		}
		//thrown by remove_truck if it is busy
		catch (const Truck* busy_truck)
		{
			cout << "Truck with License Plate: " << busy_truck->get_license_plate_num() << " has an assigned job" << endl;
			cout << "Cannot be removed untill the Delivery Job is finished" << endl;
			return;
		}
		//thrown by remove_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
		}
		if (!try_again())
			return;
	}
}


bool Interface::show_detailed_truck()
{
	int license_plate;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Please Enter Truck's License Plate Number: ";
		cin >> license_plate;
		cout << endl;
		try
		{
			cout << m_truck_fleet.get_truck(license_plate) << endl;
			if (m_user->is_admin())
			{
				cout << endl;
				return edit_truck_menu(license_plate);
			}
			else
				return 	wait_for_return();
		}
		//thrown by get_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
		}
		if (!try_again())
			return false;
	}
}

//possibilities change depending on the truck type
void Interface::show_edit_truck_menu(int license_plate_number) const
{
	const Truck* truck = m_truck_fleet.get_truck(license_plate_number);
	TruckType truck_type = truck->get_truck_type();
	system("cls");
	cout << *(truck) << endl << endl;
	cout << "1. Edit Truck Test Date" << endl;
	if (truck_type == TruckType::SemiTrailer)
		cout << "2. Edit Trailer" << endl;
	if (truck_type == TruckType::Refrigeration)
		cout << "2. Edit Refrigerator Test Date" << endl;
	if (truck_type == TruckType::Tanker)
	{
		cout << "2. Edit Tank" << endl;
		cout << "3. Edit Tank Test Date" << endl;
	}
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::edit_truck_menu(int license_plate_number)
{
	while (true)
	{
		TruckType truck_type = m_truck_fleet.get_truck(license_plate_number)->get_truck_type();
		show_edit_truck_menu(license_plate_number);
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			edit_truck_test_date(license_plate_number);
			break;
		case '2':
			switch (truck_type)
			{
			case TruckType::SemiTrailer:
				edit_truck_trailer(license_plate_number);
				break;
			case TruckType::Refrigeration:
				edit_truck_refrigerator_test_date(license_plate_number);
				break;
			case TruckType::Tanker:
				edit_truck_tanker(license_plate_number);
				break;
			}
			break;
		case '3':
			if (truck_type == TruckType::Tanker)
				edit_truck_tanker_test_date(license_plate_number);
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		if (wait_for_return())
			return true;
	}
	
}

void Interface::edit_truck_test_date(int license_plate_number)
{
	string test_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Truck's test date: ";
		cin >> test_date;
		try
		{
			m_truck_fleet.edit_truck_test_date(license_plate_number, Date(test_date));
			cout << "The Test Date has been changed successfully" << endl;
			return;
		}
		//thrown by date construtor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constuctor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by edit_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_truck_trailer(int license_plate_number)
{
	int trailer_empty_weight, trailer_max_load_weight;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Trailer's Empty Weight: ";
		cin >> trailer_empty_weight;
		cout << "Enter Trailer's Max Load Weight: ";
		cin >> trailer_max_load_weight;
		try
		{
			m_truck_fleet.edit_truck_trailer(license_plate_number, trailer_empty_weight, trailer_max_load_weight);
			cout << "The Truck's Trailer has been changed successfully" << endl;
			return;
		}
		//thrown by set_trailer
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by edit_truck if the truck type is incorrect
		catch (TruckType err_truck_type)
		{
			cout << err_truck_type << " does not have a trailer";
			return;
		}
		//thrown by edit_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_truck_refrigerator_test_date(int license_plate_number)
{
	string test_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Truck's Refrigerator test date: ";
		cin >> test_date;
		try
		{
			m_truck_fleet.edit_refrigeration_test_date(license_plate_number, Date(test_date));
			cout << "The Refrigerator's Test Date has been changed successfully" << endl;
			return;
		}
		//thrown by set_refrigerator or date constuctor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constuctor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by edit_truck if the truck type is incorrect
		catch (TruckType err_truck_type)
		{
			cout << err_truck_type << " does not have a refrigerator" << endl;
			return;
		}
		//thrown by edit_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_truck_tanker(int license_plate_number)
{
	int tanker_empty_weight, tanker_max_load_weight, max_volume;
	string sealing_test_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Tanker Empty Weight: ";
		cin >> tanker_empty_weight;
		cout << "Tanker Max Load Weight: ";
		cin >> tanker_max_load_weight;
		cout << "Tanker Sealing Test Date (dd/mm/yyyy): ";
		cin >> sealing_test_date;
		cout << "Tanker Max Volume: ";
		cin >> max_volume;
		try
		{
			m_truck_fleet.edit_truck_tank(license_plate_number, tanker_empty_weight, tanker_max_load_weight, Date(sealing_test_date), max_volume);
			cout << "The Truck's Tank has been changed successfully" << endl;
			return;
		}
		//thrown by set_tank or date constuctor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constuctor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by edit_truck if the truck type is incorrect
		catch (TruckType err_truck_type)
		{
			cout << err_truck_type << " does not have a tank";
			return;
		}
		//thrown by edit_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}

void Interface::edit_truck_tanker_test_date(int license_plate_number)
{
	string sealing_test_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Tanker Sealing Test Date (dd/mm/yyyy): ";
		cin >> sealing_test_date;
		try
		{
			m_truck_fleet.edit_tank_test_date(license_plate_number, Date(sealing_test_date));
			cout << "The Truck's Tank Test Date has been changed successfully" << endl;
			return;
		}
		//thrown by set_tank or date constuctor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constuctor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by edit_truck if the truck type is incorrect
		catch (TruckType err_truck_type)
		{
			cout << err_truck_type << " does not have a tank";
			return;
		}
		//thrown by edit_truck if it does not exist
		catch (int err_license)
		{
			cout << "Truck with License Plate Number: " << err_license << " was not found" << endl;
			return;
		}
		if (!try_again())
			return;
	}
}


// ------------------------ Delivery Job Menus ------------------------


void Interface::show_delivery_job_menu() const
{
	system("cls");
	cout << "Delivery Job Actions Menu" << endl << endl;
	cout << "Which delivert actions would you like to take?" << endl;
	cout << "1. View Delivery Job List" << endl;
	cout << "2. Search For Specific Delivery Job" << endl;
	cout << "3. Add New Delivery Job" << endl;
	cout << "4. Remove Delivery Job" << endl;
	cout << endl << "Press \"r\" to return" << endl << endl;

}

void Interface::delivery_job_menu()
{
	while (true)
	{
		show_delivery_job_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case '1':
			if (delivery_job_list_menu())
				return;
			break;
		case '2':
			if (show_detailed_delivery_job())
				return;
			break;
		case '3':
			if (add_delivery_job_menu())
				return;
			break;
		case '4':
			remove_delivery_job();
			if (wait_for_return())
				return;
			break;
		case 'r':
			return;
		}
	}
}



void Interface::show_delivery_job_list_menu() const
{
	system("cls");
	cout << "Delivery List Menu" << endl << endl;
	cout << "Which delivery job list actions would you like to take?" << endl;
	cout << "1. View Reduced Delivery Job List" << endl;
	cout << "2. View Detailed Delivery Job List" << endl;
	cout << "3. View Detailed Delivery Job List By Cargo Type" << endl;
	cout << "4. View Detailed Delivery Job List By Cargo Weight" << endl;
	cout << "5. View Detailed Delivery Job List By Distance" << endl;
	cout << "6. View Detailed Delivery Job List By Due Date" << endl;
	cout << endl << "7. View Detailed Delivery Job" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::delivery_job_list_menu()
{
	while (true)
	{
		show_delivery_job_list_menu();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		system("cls");
		switch (user_input)
		{
		case '1':
			m_job_list.show_reduced_list();
			break;
		case '2':
			cout << m_job_list;
			break;
		case '3':
			m_job_list.show_by_type(Cargo::regular);
			m_job_list.show_by_type(Cargo::refrigerated_goods);
			m_job_list.show_by_type(Cargo::liquid);
			break;
		case '4':
			m_job_list.show_by_cargo_weight();
			break;		
		case '5':
			m_job_list.show_by_distance();
			break;
		case '6':
			m_job_list.show_by_due_date();
			break;
		case '7':
			if (show_detailed_delivery_job())
				return true;
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		//only the show detailed delivery job is an actual menu by itself so you dont really need to wait for user input
		if (user_input != '7' && wait_for_return())
			return true;
	}
}


DeliveryJob* Interface::add_DeliveryJob_aux(Cargo cargo_type)
{
	system("cls");
	int cargo_weight, distance;
	string due_date, start_point, end_point;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "Please Enter Delivery Job Details:" << endl;
	cout << "Cargo Weight: ";
	cin >> cargo_weight;
	cout << "Due Date (dd/mm/yyyy): ";
	cin >> due_date;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.clear();
	cout << "Starting Point: ";
	getline(cin, start_point);
	cout << "Ending Point: ";
	getline(cin, end_point);
	cout << "Distance between the points: ";
	cin >> distance;

	DeliveryJob* delivery_job = new DeliveryJob(cargo_weight, Date(due_date), start_point, end_point, distance);
	switch (cargo_type)
	{
	case Cargo::regular:
		return delivery_job;
	case Cargo::refrigerated_goods:
		return add_RefrigeratedGoodsDelivery_aux(delivery_job);
	case Cargo::liquid:
		return add_LiquidGoodsDelivery_aux(delivery_job);
	default:
		delete delivery_job;
		throw cargo_type;
	}
}

DeliveryJob* Interface::add_RefrigeratedGoodsDelivery_aux(const DeliveryJob* delivery_job)
{
	int refrigeration_temp;
	cout << "Refrigeration Temperature: ";
	cin >> refrigeration_temp;

	DeliveryJob* refrigerated_goods = new RefrigeratedGoodsDelivery(refrigeration_temp, delivery_job);
	delete delivery_job;
	return refrigerated_goods;
}

DeliveryJob* Interface::add_LiquidGoodsDelivery_aux(const DeliveryJob* delivery_job)
{
	int liquid_volume;
	cout << "Liquid Volume: ";
	cin >> liquid_volume;

	DeliveryJob* liquid_goods = new LiquidGoodsDelivery(liquid_volume, delivery_job);
	delete delivery_job;
	return liquid_goods;
}


void Interface::show_add_delivery_job_menu() const
{
	system("cls");
	cout << "Add Delivery Job Menu" << endl << endl;
	cout << "Which Delivery Job Type would you like to add?" << endl;
	cout << "1. Regular Delivery Job" << endl;
	cout << "2. Refrigerated Goods Delivery Job" << endl;
	cout << "3. Liquid Goods Delivery Job" << endl;
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::add_delivery_job_menu_aux(Cargo& cargo_type, char user_input) const
{
	switch (user_input)
	{
	case '1':
		cargo_type = Cargo::regular;
		break;
	case '2':
		cargo_type = Cargo::refrigerated_goods;
		break;
	case '3':
		cargo_type = Cargo::liquid;
		break;
	default:
		return false;
	}
	return true;
}

bool Interface::add_delivery_job_menu()
{
	while (true)
	{
		show_add_delivery_job_menu();
		Cargo cargo_type;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			if (!add_delivery_job_menu_aux(cargo_type, user_input))
				continue;
		}
		try
		{
			DeliveryJob* new_delivery_job = add_DeliveryJob_aux(cargo_type);
			m_job_list.add_new_job(new_delivery_job);
			cout << endl << "Delivery Job Added Successfully" << endl << endl;
			return wait_for_return();
		}
		//thrown by delivery job and date constructors
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constructor
		catch (const Date& date)
		{
			cout << date << " is an invalid date" << endl;
		}
		//thrown by add_deliveryjob_aux if an unexpected error has occured
		catch (Cargo error)
		{
			cout << "An unexpacted Error has occured" << endl;
		}
		if (!try_again())
			return false;
	}
}


void Interface::remove_delivery_job()
{
	unsigned int job_num;
	int driver_id, truck_license, distance;
	const DeliveryJob* delivery_job = nullptr;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Please Enter Delivery Job Number: ";
		cin >> job_num;
		cout << endl;
		try
		{
			delivery_job = m_job_list.get_job(job_num);
			//if delivery job was ongoing there are truck and driver than need to be unassigned
			if (delivery_job->is_ongoing())
			{
				driver_id = delivery_job->get_assigned_driver_id();
				m_driver_list.remove_job(driver_id);
				truck_license = delivery_job->get_assigned_truck_license_plate();
				distance = delivery_job->get_distance();
				m_truck_fleet.add_truck_mileage(truck_license, distance);
				m_truck_fleet.remove_job(truck_license);
			}		
			m_job_list.remove_job(job_num);
			cout << "Delivery Job Removed Successfully" << endl;
			return;
		}
		//thrown by remove_job
		catch (int err_num)
		{
			cout << "Driver ID or Truck License Plate Number: " << err_num << " associated with the Delivery Job wasn't found" << endl;
			return;
		}	
		//thrown by get_job
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		if (!try_again())
			return;
	}
}


bool Interface::show_detailed_delivery_job()
{
	unsigned int job_num;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Please Enter Delivery Job Number: ";
		cin >> job_num;
		cout << endl;
		try
		{
			cout << m_job_list.get_job(job_num) << endl << endl;
			return edit_delivery_job_menu(job_num);
		}
		//thrown by get_job if it does not exists
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		if (!try_again())
			return false;
	}
}


//possibilities change depending on the cargo type
void Interface::show_edit_delivery_job_menu(unsigned int job_num) const
{
	const DeliveryJob* delivery_job = m_job_list.get_job(job_num);
	Cargo cargo_type = delivery_job->get_cargo_type();
	system("cls");
	cout << *(delivery_job) << endl << endl;
	if (!delivery_job->is_ongoing())
		cout << "Press \"s\" to start Delivery Job" << endl << endl;
	cout << "1. Edit Cargo Weight" << endl;
	cout << "2. Edit Delivery Due Date" << endl;
	cout << "3. Edit Starting and Ending Location" << endl;
	if (cargo_type == Cargo::refrigerated_goods)
		cout << "4. Edit Refrigeration Temperature" << endl;
	if (cargo_type == Cargo::liquid)
		cout << "4. Edit Liquid Volume" << endl;
	
	cout << endl << "Press \"r\" to return" << endl;
	cout << "Press \"m\" to return to the main menu" << endl << endl;
}

bool Interface::edit_delivery_job_menu(unsigned int job_num)
{
	const DeliveryJob* delivery_job = m_job_list.get_job(job_num);
	Cargo cargo_type = delivery_job->get_cargo_type();
	while (true)
	{
		show_edit_delivery_job_menu(job_num);
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		char user_input = getchar();
		switch (user_input)
		{
		case 's':
			if (delivery_job->is_ongoing())
				continue;
			start_delivery_job(job_num);
			break;
		case '1':
			edit_delivery_job_weight(job_num);
			break;
		case '2':
			edit_delivery_job_due_date(job_num);
			break;
		case '3':
			edit_delivery_job_start_and_end_points(job_num);
			break;
		case '4':
			switch (cargo_type)
			{
			case Cargo::refrigerated_goods:
				edit_refregerated_delivery_temp(job_num);
				break;
			case Cargo::liquid:
				edit_liquid_delivery_volume(job_num);
				break;
			}
			break;
		case 'r':
			return false;
		case 'm':
			return true;
		default:
			continue;
		}
		if (wait_for_return())
			return true;
	}

}

bool Interface::start_delivery_job_aux(unsigned int job_num, int driver_id, int truck_license)
{
	try
	{
		m_job_list.start_job(job_num, driver_id, truck_license);
	}
	//thrown by start_job if it does not exists
	catch (unsigned int err_job_num)
	{
		cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		return false;
	}
	//thrown by start_job if it is allreay ongoing
	catch (const string& err_msg)
	{
		cout << err_msg << endl;
		return false;
	}
	try
	{
		m_driver_list.assign_job(driver_id, job_num);
		m_truck_fleet.assign_job(truck_license, job_num);
	}
	//thrown by assign job if the driver or truck do not exist
	catch (int err_num)
	{
		cout << "Driver ID or Truck License Plate Number: " << err_num << " associated with the Delivery Job wasn't found" << endl;
		return false;
	}
	return true;
}

void Interface::start_delivery_job(unsigned int job_num)
{
	int driver_id, truck_license;
	const Driver* driver = nullptr;
	const Truck* truck = nullptr;
	const DeliveryJob* delivery_job = nullptr;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		try
		{
			delivery_job = m_job_list.get_job(job_num);
			//shows all the drivers suitable for the job, if there are none, return, since you cannnot start the job
			if (!m_driver_list.show_suitable(delivery_job))
				return;
			cout << endl << endl << "Please Enter Driver ID: ";
			cin >> driver_id;
			cout << endl << endl;
			//shows all the trucks suitable for the job, if there are none, return, since you cannnot start the job
			if (!m_truck_fleet.show_suitable(delivery_job))
				return;
			cout << "Please Enter Truck License Plate Number: ";
			cin >> truck_license;
			driver = m_driver_list.get_driver_by_id(driver_id);
			truck = m_truck_fleet.get_truck(truck_license);
			//if all together job,driver and truck were not suitabe, it means your choice of the specific driver for the specific truck	wasnt right
			if (!delivery_job->suitable_truck_and_driver(truck, driver))
				throw string("The Driver can not use this Truck for that Delivery Job");
			if (start_delivery_job_aux(job_num, driver_id, truck_license))
				cout << endl << "Delivery Job assigned successfully" << endl;
			return;
		}
		//thrown by get_driver/truck if they do not exist
		catch (int err_num)
		{
			cout << "Driver ID or Truck License Plate Number: " << err_num << " was not found" << endl;
		}
		//thrown above
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by inc_years
		catch (const Date& err_date)
		{
			cout << "Invalid Date: " << err_date << endl;
		}

		if (!try_again())
			return;
	}
}

void Interface::edit_delivery_job_weight(unsigned int job_num)
{
	int delivery_weight;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Cargo Weight: ";
		cin >> delivery_weight;
		try
		{
			m_job_list.edit_job_cargo_weight(job_num, delivery_weight);
			cout << "The Cargo Weight has been changed successfully" << endl;
		}
		//thrown by edit_cargo if the value is invalid
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
			if (!m_job_list.get_job(job_num)->is_ongoing() && try_again())
				continue;
		}
		//thrown by edit_cargo if delivery does not exist
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		return;
	}
}

void Interface::edit_delivery_job_due_date(unsigned int job_num)
{
	string due_date;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Due Date (dd/mm/yyyy): ";
		cin >> due_date;
		try
		{
			m_job_list.edit_job_due_date(job_num, Date(due_date));
			cout << "The Due Date has been changed successfully" << endl;
			return;
		}
		//thrown by edit_cargo or date constructor
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
		}
		//thrown by date constructor
		catch (const Date& invalid_date)
		{
			cout << invalid_date << " is not a valid date" << endl;
		}
		//thrown by edit_cargo if delivery does not exist
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
			return;
		}
			if (!try_again())
				return;
	}
}

void Interface::edit_delivery_job_start_and_end_points(unsigned int job_num)
{
	string start, end;
	int distance;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Starting Point: ";
		getline(cin, start);
		cout << "Enter Ending Point: ";
		getline(cin, end);
		cout << "Enter Distance between the points: ";
		cin >> distance;
		try
		{
			m_job_list.edit_job_start_and_end_points(job_num, start, end, distance);
			cout << "The Starting and Ending Location have been changed successfully" << endl;
		}
		//thrown by edit_job if the value is invalid
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
			//if the job was not ongoing and you want to try again you can
			if (!m_job_list.get_job(job_num)->is_ongoing() && try_again())
				continue;
		}
		//thrown by edit_job if it does not exist
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		return;
	}
}

void Interface::edit_refregerated_delivery_temp(unsigned int job_num)
{
	int refrigeration_temp;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Refrigeration Temperature: ";
		cin >> refrigeration_temp;	
		try
		{
			m_job_list.edit_job_refregeration_temp(job_num, refrigeration_temp);
			cout << "The Refrigeration Temparture has been changed successfully" << endl;
		}
		//thrown by edit_job if the value is invalid
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
			//if the job was not ongoing and you want to try again you can
			if (!m_job_list.get_job(job_num)->is_ongoing() && try_again())
				continue;
		}
		//thrown by edit_job if the delivery type is incorrect
		catch (Cargo err_cargo_type)
		{
			cout << err_cargo_type << " is not a refregerated goods delivery" << endl;
		}
		//thrown by edit_job if it does not exist
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		return;
	}
}

void Interface::edit_liquid_delivery_volume(unsigned int job_num)
{
	int liquid_volume;
	while (true)
	{
		system("cls");
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.clear();
		cout << "Enter Liquid Volume: ";
		cin >> liquid_volume;
		try
		{
			m_job_list.edit_job_liquid_volume(job_num, liquid_volume);
			cout << "The Liquid Volume has been changed successfully" << endl;
		}
		//thrown by edit_job if the value is invalid
		catch (const string& err_msg)
		{
			cout << err_msg << endl;
			//if the job was not ongoing and you want to try again you can
			if (!m_job_list.get_job(job_num)->is_ongoing() && try_again())
				continue;
		}
		//thrown by edit_job if the delivery type is incorrect
		catch (Cargo err_cargo_type)
		{
			cout << err_cargo_type << " is not a liquid goods delivery" << endl;
		}
		//thrown by edit_job if it does not exist
		catch (unsigned int err_job_num)
		{
			cout << "Delivery Job Number: " << err_job_num << " was not found" << endl;
		}
		return;
	}
}
