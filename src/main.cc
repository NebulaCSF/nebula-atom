/*
 * CPW Woodpecker Server
 * Copyright (C) 2021 CPW Online support@cpwonline.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "Poco/Util/Application.h"
#include "Poco/Format.h"

#include "core/woodpecker_server.h"

using namespace Poco;
using namespace Poco::Util;

int main(int argc, char** argv)
{
	try
	{
		CPW::Core::WoodpeckerServer app_main(8080);
		return app_main.run(argc, argv);
	}
	catch (std::exception const& error)
	{
		Application::instance().logger().error("- Error on main.cc on main(): " + std::string(error.what()));
		return -1;
	}
}
