#include "../../sources/employee_manager_client/Client.h"

int main(int argc, char* argv[])
{
	CClient Client("Log\\client_logs.txt");
	Client.Init("config\\client.xml");
	Client.Launch(argc, argv);
	return 0;
}