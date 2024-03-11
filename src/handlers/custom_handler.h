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

#ifndef CPW_HANDLERS_CUSTOM_HANDLER_H
#define CPW_HANDLERS_CUSTOM_HANDLER_H


#include "handlers/root_handler.h"


namespace CPW
{
    namespace Handlers
    {
        class CustomHandler;
    }
}


class CPW::Handlers::CustomHandler :
    public RootHandler
{
    public:
        CustomHandler(std::function<void(CustomHandler&)> process_function);
        virtual ~CustomHandler();

        std::function<void(CustomHandler&)>& get_process_function()
        {
            auto& var = process_function_;
            return var;
        }

    protected:
        virtual void AddFunctions_() override;
        virtual void Process_() override;
        virtual void HandleGETMethod_() override;
        virtual void HandlePOSTMethod_() override;
        virtual void HandlePUTMethod_() override;
        virtual void HandleDELMethod_() override;

    private:
        std::function<void(CustomHandler&)> process_function_;
};

#endif // CPW_HANDLERS_CUSTOM_HANDLER_H
