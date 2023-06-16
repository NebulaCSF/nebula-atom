/*
* <one line to give the program's name and a brief idea of what it does.>
* Copyright (C) 2021  <copyright holder> <email>
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


#ifndef PERMISSIONS_MANAGER_H
#define PERMISSIONS_MANAGER_H


#include <string>
#include <list>
#include <algorithm>

#include <Poco/Util/ServerApplication.h>

#include "query/query_actions.h"
#include "security/permission.h"
#include "tools/route.h"

namespace CPW
{
    namespace Security
    {
        class PermissionsManager;
    }
}

using namespace Poco;


class CPW::Security::PermissionsManager
{
    public:
        using PermissionToLoad = Poco::Tuple<std::string, std::string, std::string, int, std::string, bool, bool>;

        PermissionsManager();

        std::list<Permission>& get_permissions()
        {
            auto& var = permissions_;
            return var;
        }
        std::map<std::string, ActionType>& get_action_type_map()
        {
            auto& var = action_type_map_;
            return var;
        }

        static std::_List_iterator<Permission> FindPermission_(Tools::Route& requested_route);
        static bool VerifyPermission_(Tools::Route& requested_route, User& user, std::_List_iterator<Permission> found_permission);

    protected:
        static void FillActionTypeMap_();
        static void LoadPermissions_();

    private:
        static std::list<Permission> permissions_;
        static std::map<std::string, ActionType> action_type_map_;
};


#endif // PERMISSIONS_MANAGER_H