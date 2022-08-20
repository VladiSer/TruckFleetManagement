#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif // _DEBUG

#include "TFM_Application.h"

void start_application(Interface& app);
bool login(Interface& app);
bool try_again();

int main()
{

	try
	{
		Interface app(DRIVER_LIST_FILE_NAME, TRUCK_FLEET_FILE_NAME, DELIVERY_JOB_LIST_FILE_NAME);
		start_application(app);
	}
	//catches if application fails to load one of the files to start a the application w/o any data
	catch (const string& err_file_name)
	{
		cout << "An Error has occured while trying to load: " << err_file_name << endl << endl;
		cout << "Starting the application without loading from files" << endl << endl;
		Interface clean_app;
		start_application(clean_app);
	}
	cout << endl << "Leaks: " << _CrtDumpMemoryLeaks() << endl;

	return 0;
}


void start_application(Interface& app)
{

	char user_input;
	while (true)
	{
		cout << "Welcome!" << endl << "Do you want to login? (y/n) ";
		user_input = getchar();
		if (user_input == 'y')
		{
			try
			{
				if (login(app))
				{
					system("cls");
					app.main_menu();
				}
				else
					return;
			}
			catch (...)
			{
				cout << "An unexpacted Error has occured" << endl;
				return;
			}
		}
		else if (user_input == 'n')
			return;
		system("cls");
	}
}

bool login(Interface& app)
{
	string username, password;
	while (true)
	{
		system("cls");
		cout << "Please enter username:" << endl;
		cin >> username;
		cout << endl;
		cout << "Please enter password:" << endl;
		cin >> password;
		cout << endl;
		if (app.login(username, password))
			break;
		else if (!try_again())
			return false;
	}
	return true;
}

bool try_again()
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