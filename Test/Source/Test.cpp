#include "Test.h"
#include "App.h"

int main(int argc, char** argv)
{
	App app;

	if (app.Setup())
	{
		while (app.Run())
		{
		}
	}

	app.Shutdown();

	return 0;
}